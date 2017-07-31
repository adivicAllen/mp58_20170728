#include "StdAfx.h"
#include "Chroma_MutiInstr_API_EXP_V2.h"
#include "Chroma_API_V2.h"

#define ConnectDelay 500
#define SetPortDelay 0		//20
#define GetMarkDelay 0		//15
#define GetPeakDataDelay 0 //30

#define _ON        0
#define _OFF       1
#define _ERROR     2

#define SAVING_TIME 0

extern CChroma_API_V2 MultiAPP[MaxInstr];
char LastErrMessage[1024];
void CstringToString(CString szInput,string& szOutput)
{
	int i=szInput.GetLength();
	char *chr=new char[i+1];
	WideCharToMultiByte(CP_ACP,0,szInput.GetBuffer(),-1,chr,szInput.GetLength(),NULL,NULL);
	chr[i]='\0';
	szOutput=chr;	
	delete chr;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_ConnectSever(PORT_Select PortIndex,const char * HostAddress, const char * Port,unsigned int uiTimeout)
{
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(MultiAPP[usIndex].ConnectSever(HostAddress, Port,uiTimeout))
	{
		MultiAPP[usIndex].s_bIsServer_Connected[usIndex]=true;
		return SUCCESS;
	}	
	else
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		MultiAPP[usIndex].s_bIsServer_Connected[usIndex]=false;
		return E_Server_NO_CONNECT;
	}	
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetSeverIsConnect(PORT_Select PortIndex,bool &bIsConnect)
{
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	//bIsConnect=MultiAPP[usIndex].bIsServerConnectFlag;
	bIsConnect=MultiAPP[usIndex].SetCenterFrequency(MultiAPP[usIndex].m_f_Center_CurrentFreq);
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_UploadWaveform(const char *WaveformFilePath)
{
	USHORT usState=SUCCESS;
	for(int i=0;i<MaxInstr;i++)
	{
		if(MultiAPP[0].s_bIsServer_Connected[i])
		{
			MultiAPP[i].NetConnect(true);
			MultiAPP[i].UploadWaveform(WaveformFilePath,true);		
		}
	}
	return SUCCESS;	
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_OpenServer()
{
	return SUCCESS;
}
CHROMA_API_DLLEXT const char* __stdcall expMV2_GetLastErrorMessage()
{

	return MultiAPP[0].s_ErrMessage;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_InitFunction(const char *IniFilePath)
{
	USHORT usState=SUCCESS;
	//Parser Init Config File
	usState=MultiAPP[0].InitFunction(IniFilePath);
	if(usState!=SUCCESS)
	{
		return usState;
	}
	//Compare IP Address
	for(int i=0;i<MaxInstr;i++)
	{
		for(int j=i+1;j<MaxInstr;j++)
		{
			if(MultiAPP[0].s_szServer_IP[i]==MultiAPP[0].s_szServer_IP[j])
			{
				sprintf_s(MultiAPP[0].s_ErrMessage,"Two instr have same IP address!!");
				return E_InputData_ERR;
			}	
		}
	}


	//Load Loss Table File
	for(int i=0;i<MaxInstr;i++)
	{
		if(MultiAPP[i].s_bIsServer_OnOff[i])
		{
			usState=MultiAPP[i].LoadCableLoss(MultiAPP[i].s_szServer_PortLoss[i]);
			if(usState!=SUCCESS) return usState;
		}
	}
	//Connect server
	string sIPAddress,sPort;
	bool bFlag=MultiAPP[0].s_bIsServer_OnOff[0];
	for(int i=1;i<MaxInstr;i++)
	{
		bFlag=bFlag| MultiAPP[0].s_bIsServer_OnOff[i];
	}
	if(bFlag==false)
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"Config setting:Server all disable!");
		return E_InputData_ERR;
	}

	for(int i=0;i<MaxInstr;i++)
	{
		if(MultiAPP[i].s_bIsServer_OnOff[i])
		{
			if(MultiAPP[i].s_szServer_IP[i].GetLength()!=0 && MultiAPP[i].s_szServer_Port[i].GetLength()!=0)
			{
				CstringToString(MultiAPP[i].s_szServer_IP[i],sIPAddress);
				CstringToString(MultiAPP[i].s_szServer_Port[i],sPort);
				bFlag=MultiAPP[i].ConnectSever(sIPAddress.c_str(),sPort.c_str());
				if(!bFlag)
				{
					sprintf_s(MultiAPP[0].s_ErrMessage,"Connect Server_%d fail",i);
					MultiAPP[i].s_bIsServer_Connected[i]=false;
					return E_Server_NO_CONNECT;
				}
				Sleep(ConnectDelay);
				usState=MultiAPP[i].PresetFunc();
				if(usState!=SUCCESS)
				{
					sprintf_s(MultiAPP[0].s_ErrMessage,"Server_%d:Preset fail",i);
					return usState;
				}
				MultiAPP[i].s_bIsServer_Connected[i]=true;
				MultiAPP[i].m_bRF_OnOff=false;
				MultiAPP[i].m_bModulationOnOff=false;
			}else
			{
				sprintf_s(MultiAPP[0].s_ErrMessage,"Server_%d:IP address or Port empty.",i);
				return E_InputData_ERR;
			}	

		
			///Input Loss
  			usState=MultiAPP[i].ResetInstrCableLossSetting();
  			bFlag=MultiAPP[i].SetDistributionModeOnOff(true);
		}//if(MultiAPP[i].s_bIsServer_OnOff[i])
	}//for(int i=0;i<MaxInstr;i++)

	string szWaveFormPath="";
	//UpLoad Waveform
	if(MultiAPP[0].s_vec_WaveformPath.size()>0)
	{
		for(int i=0;i<MaxInstr;i++)
		{
			if(MultiAPP[i].s_bIsServer_Connected[i])
			{
				usState=MultiAPP[i].NetConnect(true);
				for(unsigned int j=0;j<MultiAPP[0].s_vec_WaveformPath.size();j++)
				{
					CstringToString(MultiAPP[0].s_vec_WaveformPath.at(j),szWaveFormPath);
					usState=MultiAPP[i].UploadWaveform(szWaveFormPath.c_str(),true);
					if(usState!=SUCCESS)
					{
						sprintf_s(MultiAPP[0].s_ErrMessage,"Server_%d:UploadWaveform:%s fail",i,szWaveFormPath.c_str());
						return usState;
					}
				}//for(unsigned int j=0;j<MultiAPP[0].s_vec_WaveformPath.size();j++)
			}
		}//for(int i=0;i<MaxInstr;i++)
	}
	return usState;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_InputCableLoss(PORT_Select PortIndex,const char *InstrCableLossFilePath)
{
	USHORT usIndex= PortIndex/Port_Nums;
	USHORT usState=SUCCESS;
	string strTemp;
	if(usIndex>=MaxInstr)
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	CString szFilePath=_T("");
	szFilePath=InstrCableLossFilePath;
	usState=MultiAPP[usIndex].LoadCableLoss(szFilePath);
	if(usState!=SUCCESS)
	{
		strTemp=MultiAPP[usIndex].s_ErrMessage;
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:%s",usIndex,strTemp.c_str());
		return usState;
	}
	usState=MultiAPP[usIndex].ResetInstrCableLossSetting();
	if(usState!=SUCCESS)
	{
		strTemp=MultiAPP[usIndex].s_ErrMessage;
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:ResetInstrCableLossSetting fail",usIndex);
		return usState;
	}
	
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetCableLossEnable(PORT_Select PortIndex,const bool bEnable)
{
	USHORT usIndex= PortIndex/Port_Nums;
	USHORT usState=SUCCESS;
	if(usIndex>=MaxInstr)
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}

	usState=MultiAPP[usIndex].SetAllPortLossFile(!bEnable);
	if(usState!=SUCCESS)
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetCableLossEnable Error",usIndex);
		return usState;
	}
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetDistributionModeOnOff(PORT_Select PortIndex,const bool bOnOff)
{
	USHORT usIndex= PortIndex/Port_Nums;
	USHORT usState=SUCCESS;
	if(usIndex>=MaxInstr)
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	usState=MultiAPP[usIndex].SetDistributionModeOnOff(bOnOff);
	if(usState!=SUCCESS)
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetDistributionModeOnOff Error",usIndex);
		return usState;
	}
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_PresetServer(PORT_Select PortIndex)
{
	USHORT usState=SUCCESS;
	USHORT usIndex= PortIndex/Port_Nums;
	if(MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		MultiAPP[usIndex].PresetFunc();
	}else
		return E_Server_NO_CONNECT;

	return usState;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_CloseServer(PORT_Select PortIndex)
{
	USHORT usState=SUCCESS;
	USHORT usIndex= PortIndex/Port_Nums;	
	if(MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		MultiAPP[usIndex].CloseServer();
		MultiAPP[usIndex].s_bIsServer_Connected[usIndex]=false;
	}
	return usState;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_CloseAllServer()
{
	for(int i=0;i<MaxInstr;i++)
	{
		if(MultiAPP[i].s_bIsServer_Connected[i])
		{
			MultiAPP[i].CloseServer();
			MultiAPP[i].s_bIsServer_Connected[i]=false;
		}
	}
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SaveVsaData(PORT_Select PortIndex,const char *SaveName)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	string strSaveName=SaveName;
	bFlag=MultiAPP[usIndex].SaveVsaData(&strSaveName);
	if(!bFlag)
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SaveVsaData fail",usIndex);
		return E_Command_ERR;
	}
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetCenterFrequency(PORT_Select PortIndex,const float fFreqMHz,const float fFreqOffsetMHz)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	float fSetFreq=fFreqMHz+fFreqOffsetMHz;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	 
	if (SAVING_TIME)  // 0602
	{ 
		if(MultiAPP[usIndex].m_CenterFrequency==fSetFreq)
			return SUCCESS;
	}

	bFlag=MultiAPP[usIndex].SetCenterFrequency(fSetFreq);
	if(!bFlag)
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetCenterFrequency fail",usIndex);
		return E_Command_ERR;
	}
	MultiAPP[usIndex].m_CenterFrequency=fSetFreq;
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetCenterFrequency(PORT_Select PortIndex,float &fFreqMHz)
{
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	MultiAPP[usIndex].GetCenterFrequency(fFreqMHz);
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetFreqSpan(PORT_Select PortIndex,const int iFreqSpanMHz)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}

	if(SAVING_TIME)  // 0602
	{ 
 		if(MultiAPP[usIndex].m_iFreqSpanMHz==iFreqSpanMHz)
 			return SUCCESS;
	}
	bFlag=MultiAPP[usIndex].SetFreqSpan(iFreqSpanMHz);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetFreqSpan fail",usIndex);
		return E_Command_ERR;
	}
	MultiAPP[usIndex].m_iFreqSpanMHz=iFreqSpanMHz;
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetFreqSpan(PORT_Select PortIndex,int &iFreqSpanMHz)
{
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	MultiAPP[usIndex].GetFreqSpan(iFreqSpanMHz);
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetRxRBW(PORT_Select PortIndex,const RBW_Set uiEnum)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	if (SAVING_TIME)  // 0602
	{ 
		if(MultiAPP[usIndex].m_uiRBW_Enum==uiEnum)
			return SUCCESS;
	}
	bFlag=MultiAPP[usIndex].SetRxRBW(uiEnum);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetRxRBW fail",usIndex);
		return E_Command_ERR;}
	MultiAPP[usIndex].m_uiRBW_Enum=uiEnum;
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetRxRBW_Int(PORT_Select PortIndex,const int iIndex)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	unsigned int uiTemp=(unsigned int)iIndex;
	if(SAVING_TIME)  // 0602
	{ 
		if(MultiAPP[usIndex].m_uiRBW_Enum==uiTemp)
			return SUCCESS;
	}
	bFlag=MultiAPP[usIndex].SetRxRBW((RBW_Set)uiTemp);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetRxRBW fail",usIndex);
		return E_Command_ERR;}
	MultiAPP[usIndex].m_uiRBW_Enum=uiTemp;
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetRxRBW(PORT_Select PortIndex,UINT64 &uiRBW_Hz)
{
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	MultiAPP[usIndex].GetRxRBW(uiRBW_Hz);
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_PowerReferenceAuto(PORT_Select PortIndex)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	bFlag=MultiAPP[usIndex].PowerReferenceAuto();
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:PowerReferenceAuto fail",usIndex);
		return E_Command_ERR;
	}
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetRxReferenceLevel(PORT_Select PortIndex,const float fRefLevel)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	if(SAVING_TIME)
	{ 
		if(MultiAPP[usIndex].m_fRefLevel==fRefLevel)
			return SUCCESS;
	}
	bFlag=MultiAPP[usIndex].SetRxReferenceLevel(fRefLevel);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetRxReferenceLevel fail",usIndex);
		return E_Command_ERR;}
	MultiAPP[usIndex].m_fRefLevel=fRefLevel;
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetRxReferenceLevel(PORT_Select PortIndex,float &fRefLevel)
{
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	MultiAPP[usIndex].GetRxReferenceLevel(fRefLevel);
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetPeakData(PORT_Select PortIndex,float &fPeakFreqMHz,float &fPeakPower)
{
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	Sleep(GetPeakDataDelay);
	MultiAPP[usIndex].GetPeakData(fPeakFreqMHz,fPeakPower);
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetGFSK_FreqDeviation(PORT_Select PortIndex,float &fFreqDevMHz)
{
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}

	MultiAPP[usIndex].GetGFSK_FreqDeviation(fFreqDevMHz);
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetVsaRx_Port(const PORT_Select PortIndex)
{
	unsigned int uiPortIndex=0;
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	uiPortIndex+=((unsigned int)PortIndex%Port_Nums);

	bFlag=MultiAPP[usIndex].SetVsaRx_Port(uiPortIndex);
	
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetVsaRx_Port fail",usIndex);
		return E_Command_ERR;}
	MultiAPP[usIndex].m_uiVsa_Port=uiPortIndex;
	Sleep(SetPortDelay);
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_Set_4Port_Switch(const PORT_Select PortFirstIndex,	const PORT_Mode pm_Port_1,
	const PORT_Mode pm_Port_2, 
	const PORT_Mode pm_Port_3,
	const PORT_Mode pm_Port_4)
{
	bool bFlag=false;
	USHORT usIndex= PortFirstIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	bFlag=MultiAPP[usIndex].Set_4Port_Switch(pm_Port_1,pm_Port_2,pm_Port_3,pm_Port_4);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:Set_4Port_Switch fail",usIndex);
		return E_Command_ERR;}
	Sleep(SetPortDelay);
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetCWFrequencyOffset(PORT_Select PortIndex,const float fFreqOffsetMHz)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	bFlag=MultiAPP[usIndex].SetCWFrequencyOffset(fFreqOffsetMHz);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetCWFrequencyOffset fail",usIndex);
		return E_Command_ERR;}
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetTxOutputLevel(PORT_Select PortIndex,const float fOutputLevel)//10~-90 dBm
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	//bFlag=MultiAPP[usIndex].SetFreqPointTxOutputLevelAndPortAtt(fOutputLevel);
	bFlag=MultiAPP[usIndex].SetTxOutputLevel(fOutputLevel);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetTxOutputLevel fail",usIndex);
		return E_Command_ERR;}
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetTxOutputLevel(PORT_Select PortIndex,float &fOutputLevel)
{
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	MultiAPP[usIndex].GetTxOutputLevel(fOutputLevel);
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_LoadMemory(PORT_Select PortIndex,const char * strFileName)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}

	int iResult=0;
	string str_Temp=strFileName;
	iResult=MultiAPP[usIndex].stdStr_LoadFileName.compare(str_Temp);
	if(iResult==0)
		return SUCCESS;
	else
		MultiAPP[usIndex].stdStr_LoadFileName=strFileName;

	bFlag=MultiAPP[usIndex].LoadPlayFile(strFileName);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:LoadMemory fail",usIndex);
		return E_Command_ERR;}
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT __stdcall expMV2_TxPlayRepeatOnOff(PORT_Select PortIndex,const bool bOnOff)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	bFlag=MultiAPP[usIndex].TxPlayRepeatOnOff(bOnOff);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:TxPlayRepeatOnOff fail",usIndex);
		return E_Command_ERR;}
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetCurrentFreqLoss(const float fFreq,const PORT_Select PortIndex,float& fTxLoss,float& fRxLoss)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	unsigned int uiPortIndex=PortIndex%Port_Nums;
	bFlag=MultiAPP[usIndex].GetCurrentFreqLoss(fFreq,uiPortIndex,fTxLoss,fRxLoss);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:GetCurrentFreqLoss fail",usIndex);
		return E_Command_ERR;}
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetChannelPower(PORT_Select PortIndex,float &fChannelPower)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	MultiAPP[usIndex].GetChannelPower(fChannelPower);
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT __stdcall expMV2_ModulationOnOff(PORT_Select PortIndex,const bool bModulationOnOff)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}

	 
	if (SAVING_TIME)  // 0602
	{
		if (bModulationOnOff)  // when modulation = wave (true), (1) set wave mode (2) enable run
		{                      // when modulation = CW, (1)  disable run   (2) set CW  mode    


			if (MultiAPP[usIndex].byteModulationStatus != _ON)
			{
				bFlag = MultiAPP[usIndex].SetVsgModulationOnOff(true);  // set modulation
				MultiAPP[usIndex].m_bModulationOnOff = true;
				if (!bFlag) {
					sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:Set ModulationOnOff fail", usIndex);
					return E_Command_ERR;
				}
			}


			if (MultiAPP[usIndex].byteRunStatus != _ON)
			{
				bFlag = MultiAPP[usIndex].SetVsgOnOff(true);  //  enable "RUN"
				if (!bFlag)
				{
					sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:Set Run_Enablr/Disable fail", usIndex);
					return E_Command_ERR;
				}
			}

		}
		else
		{

			if (MultiAPP[usIndex].byteRunStatus != _OFF)
			{
				bFlag = MultiAPP[usIndex].SetVsgOnOff(false);  //  disable "RUN"
				if (!bFlag)
				{
					sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:Set Run_Enablr/Disable fail", usIndex);
					return E_Command_ERR;
				}
			}


			if (MultiAPP[usIndex].byteModulationStatus != _OFF)
			{
				bFlag = MultiAPP[usIndex].SetVsgModulationOnOff(false);  // set cw
				MultiAPP[usIndex].m_bModulationOnOff = false;
				if (!bFlag) {
					sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:Set ModulationOnOff fail", usIndex);
					return E_Command_ERR;
				}
			}

		}

	}
	else //  if (SAVE_TIME)  // 0602
	{

		if (bModulationOnOff)  // when modulation = wave (true), (1) set wave mode (2) enable run
		{                      // when modulation = CW, (1)  disable run   (2) set CW  mode    


			//if (MultiAPP[usIndex].byteModulationStatus != _ON)
			{
				bFlag = MultiAPP[usIndex].SetVsgModulationOnOff(true);  // set modulation
				MultiAPP[usIndex].m_bModulationOnOff = true;
				if (!bFlag) {
					sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:Set ModulationOnOff fail", usIndex);
					return E_Command_ERR;
				}
			}


			//if (MultiAPP[usIndex].byteRunStatus != _ON)
			{
				bFlag = MultiAPP[usIndex].SetVsgOnOff(true);  //  enable "RUN"
				if (!bFlag)
				{
					sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:Set Run_Enablr/Disable fail", usIndex);
					return E_Command_ERR;
				}
			}

		}
		else
		{

		//	if (MultiAPP[usIndex].byteRunStatus != _OFF)
			{
				bFlag = MultiAPP[usIndex].SetVsgOnOff(false);  //  disable "RUN"
				if (!bFlag)
				{
					sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:Set Run_Enablr/Disable fail", usIndex);
					return E_Command_ERR;
				}
			}


		//	if (MultiAPP[usIndex].byteModulationStatus != _OFF)
			{
				bFlag = MultiAPP[usIndex].SetVsgModulationOnOff(false);  // set cw
				MultiAPP[usIndex].m_bModulationOnOff = false;
				if (!bFlag) {
					sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:Set ModulationOnOff fail", usIndex);
					return E_Command_ERR;
				}
			}

		}



	}
	
	
	
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT __stdcall expMV2_Rf_OnOff(PORT_Select PortIndex,bool bRfOnOff)
{
	bool bFlag=false;
	bool bRfStatus =false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	
	if (SAVING_TIME)
	{
		if ((MultiAPP[usIndex].byteRunStatus != _OFF) && (!bRfOnOff))  // 0511 for cycle time variation
		{
			bFlag = MultiAPP[usIndex].SetVsgOnOff(false);  //  disable "RUN"
			if (!bFlag)
			{
				sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:Set Run_Enablr/Disable fail", usIndex);
				return E_Command_ERR;
			}
		}

	}
	else
	{
		if  (!bRfOnOff)  // 0511 for cycle time variation
		{
			bFlag = MultiAPP[usIndex].SetVsgOnOff(false);  //  disable "RUN"
			if (!bFlag)
			{
				sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:Set Run_Enablr/Disable fail", usIndex);
				return E_Command_ERR;
			}
		}


	}




	// inquiry status is less time than set command
		 
	if(     (( MultiAPP[usIndex].byteRfOnStatus == _ON ) && (!bRfOnOff) )
		||  ((MultiAPP[usIndex].byteRfOnStatus == _OFF ) && (bRfOnOff) ) 
		||  (MultiAPP[usIndex].byteRfOnStatus == _ERROR ) )
	{
		bFlag=MultiAPP[usIndex].SetVsgRfOnOff(bRfOnOff);
		if(!bFlag)
		{
			sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:Set Rf_OnOff fail",usIndex);
			return E_Command_ERR;
		}
	}
		
	MultiAPP[usIndex].m_bRF_OnOff=bRfOnOff;
	
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_Get_Rf_OnOff(PORT_Select PortIndex,bool& bRfOnOff)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}

	MultiAPP[usIndex].GetVsgRfOnOff(bRfOnOff);	
	MultiAPP[usIndex].m_bRF_OnOff=bRfOnOff;
	return SUCCESS;
}
// CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetVsgOnOff(PORT_Select PortIndex,const bool bOnOff)
// {
// 	bool bFlag=false;
// 	USHORT usIndex= PortIndex/Port_Nums;
// 	if(usIndex>=MaxInstr)////////////////////////////
// 	{
// 		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
// 		return E_Input_OutOfDefine;
// 	}
// 	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
// 	{
// 		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
// 		return  E_Server_NO_CONNECT;
// 	}
// 	unsigned int uiPortIndex=PortIndex%Port_Nums;
// 	bFlag=MultiAPP[usIndex].SetVsgOnOff(bOnOff);
// 	if(!bFlag){
// 		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:Set VsgOnOff fail",usIndex);
// 		return E_Command_ERR;}
// 	return SUCCESS;
// }
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetVsaOnOff(PORT_Select PortIndex,const bool bOnOff)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	bFlag=MultiAPP[usIndex].SetVsaOnOff(bOnOff);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:Set VsaOnOff fail",usIndex);
		return E_Command_ERR;}
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetMark(PORT_Select PortIndex,const float fMarkFreqMHz)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	bFlag=MultiAPP[usIndex].SetMark(fMarkFreqMHz);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:Set Mark fail",usIndex);
		return E_Command_ERR;}
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT __stdcall expMV2_ClearMark(PORT_Select PortIndex)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	bFlag=MultiAPP[usIndex].ClearMark();
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:Clear Mark fail",usIndex);
		return E_Command_ERR;}
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_GetMarkValue(PORT_Select PortIndex,float &fValueMHz)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	Sleep(GetMarkDelay);
	MultiAPP[usIndex].GetMarkValue(fValueMHz);
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_DemodFskOpen(PORT_Select PortIndex,bool bIsOpen)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	if(bIsOpen)
	{
		MultiAPP[usIndex].SetDemodulationtype(1); //Open Demod type
	}else{
		MultiAPP[usIndex].SetDemodulationtype(0); //Close Demod type
	}

	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT __stdcall expMV2_DemodFskSetting(PORT_Select PortIndex,bool bIsDemod)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	if(bIsDemod)
	{
		MultiAPP[usIndex].SetDemodulationtype(1); //Open Demod type
		MultiAPP[usIndex].SetDemodulationTool(0);
		MultiAPP[usIndex].FskSetType(2);
		MultiAPP[usIndex].FskSetSymbolRate(1);
		MultiAPP[usIndex].FskSetFilterType(3);
		MultiAPP[usIndex].FskSetFilterLength(6);
		MultiAPP[usIndex].FskSetcoefficient(0.5);
		MultiAPP[usIndex].FskSetSymbolPhaseContinuity(true);
		//MultiAPP[usIndex].SetFreqSpan(160);

	}else{
		MultiAPP[usIndex].SetDemodulationtype(0); //Close Demod type
	}
	
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetSpectrumOnOff(PORT_Select PortIndex,const bool bOnOff)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	bFlag=MultiAPP[usIndex].SetSpectrumOnOff(bOnOff);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:Set SpectrumOnOff fail",usIndex);
		return E_Command_ERR;}
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT __stdcall expMV2_SetCableLossTableCoupleMode(PORT_Select PortIndex,const bool bOnOff)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	if(usIndex>=MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage,"PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if(!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:No Connection",usIndex);
		return  E_Server_NO_CONNECT;
	}
	bFlag=MultiAPP[usIndex].SetCableLossTableCoupleMode(bOnOff);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:Set CableLoss Table Couple Mode fail",usIndex);
		return E_Command_ERR;}
	return SUCCESS;
}