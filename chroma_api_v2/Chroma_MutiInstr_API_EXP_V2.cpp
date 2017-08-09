
#include "Chroma_MutiInstr_API_EXP_V2.h"
#include "Chroma_API_V2.h"
#include <QSettings>
#include <QDebug>
#define ConnectDelay 500
#define SetPortDelay 0		//20
#define GetMarkDelay 0		//15
#define GetPeakDataDelay 0 //30

#define _ON        0
#define _OFF       1
#define _ERROR     2

#define SAVING_TIME 0
#define DEFAULT_ERROR 9999

extern CChroma_API_V2 MultiAPP[MaxInstr];
char LastErrMessage[1024];
void CstringToString(QString szInput,string& szOutput)
{   /*
	int i=szInput.GetLength();
	char *chr=new char[i+1];
	WideCharToMultiByte(CP_ACP,0,szInput.GetBuffer(),-1,chr,szInput.GetLength(),NULL,NULL);
	chr[i]='\0';
	szOutput=chr;	
	delete chr;
    */
    szOutput = szInput.toStdString();
}
CHROMA_API_DLLEXT USHORT  expMV2_ConnectSever(RF_PORT PortIndex,const char * HostAddress, const char * Port,unsigned int uiTimeout)
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
CHROMA_API_DLLEXT USHORT  expMV2_GetSeverIsConnect(RF_PORT PortIndex,bool &bIsConnect)
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
CHROMA_API_DLLEXT USHORT  expMV2_UploadWaveform(const char *WaveformFilePath)
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
CHROMA_API_DLLEXT USHORT  expMV2_OpenServer()
{
	return SUCCESS;
}
CHROMA_API_DLLEXT const char*  expMV2_GetLastErrorMessage()
{

	return MultiAPP[0].s_ErrMessage;
}
CHROMA_API_DLLEXT USHORT  expMV2_InitFunction(const char *IniFilePath)
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
	bool bFlag=MultiAPP[0].s_bIsServer_OnOff[0];  // set server on
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
            if(MultiAPP[i].s_szServer_IP[i].length()!=0 && MultiAPP[i].s_szServer_Port[i].length()!=0)
            {
                sIPAddress = MultiAPP[i].s_szServer_IP[i].toStdString();
                sPort = MultiAPP[i].s_szServer_Port[i].toStdString();

				if ( !MultiAPP[i].s_bIsServer_Connected[i] )  // no connection 
				{
				  bFlag=MultiAPP[i].ConnectSever(sIPAddress.c_str(),sPort.c_str(),15);
			      Sleep(ConnectDelay);
					if(!bFlag)
					{
						sprintf_s(MultiAPP[0].s_ErrMessage,"Connect Server_%d fail",i);
						MultiAPP[i].s_bIsServer_Connected[i]=false;
						return E_Server_NO_CONNECT;
					}
						
				}
			   
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
	if(MultiAPP[0].s_vec_WaveformPath.size()>0)  // allen_debug upload waveform
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
					Sleep(3000);
				//	usState=MultiAPP[i]. SetCenterFrequency(2440.);  // allen
				//	usState=MultiAPP[i].SetTxOutputLevel(0.);
				//	usState=MultiAPP[i].SetVsgModulationOnOff(true);
				//	usState=MultiAPP[i].SetVsgRfOnOff(true);  // allen   // need load memeory to choose waveform, upload wave form only load into MP5800
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
CHROMA_API_DLLEXT USHORT expMV2_InputCableLoss(RF_PORT PortIndex,const char *InstrCableLossFilePath)
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
    QString szFilePath=("");
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
CHROMA_API_DLLEXT USHORT  expMV2_SetCableLossEnable(RF_PORT PortIndex,const bool bEnable)
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
CHROMA_API_DLLEXT USHORT  expMV2_SetDistributionModeOnOff(RF_PORT PortIndex,const bool bOnOff)
{
	USHORT usIndex= PortIndex/Port_Nums;
	USHORT usState=SUCCESS;
	bool bFlag= false;
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
	bFlag =MultiAPP[usIndex].SetDistributionModeOnOff(bOnOff);
	if(!bFlag)
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetDistributionModeOnOff Error",usIndex);
		return usState;
	}
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT  expMV2_GetDistributionModeOnOff(RF_PORT PortIndex,bool &bDistributionMode)
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
	MultiAPP[usIndex].GetDistributionModeOnOff(bDistributionMode);
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT  expMV2_PresetServer(RF_PORT PortIndex)
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
CHROMA_API_DLLEXT USHORT  expMV2_CloseServer(RF_PORT PortIndex)
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
CHROMA_API_DLLEXT USHORT  expMV2_CloseAllServer()
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
CHROMA_API_DLLEXT USHORT  expMV2_SaveVsaData(RF_PORT PortIndex,const char *SaveName)
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
CHROMA_API_DLLEXT USHORT  expMV2_SetCenterFrequency(RF_PORT PortIndex,const double fFreqMHz,const double fFreqOffsetMHz)
{
	bool bFlag=false;
	USHORT usIndex= PortIndex/Port_Nums;
	double fSetFreq=fFreqMHz+fFreqOffsetMHz;
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
CHROMA_API_DLLEXT USHORT  expMV2_GetCenterFrequency(RF_PORT PortIndex,double &fFreqMHz)
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
CHROMA_API_DLLEXT USHORT  expMV2_SetFreqSpan(RF_PORT PortIndex,const int iFreqSpanMHz)
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
CHROMA_API_DLLEXT USHORT  expMV2_GetFreqSpan(RF_PORT PortIndex,int &iFreqSpanMHz)
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
CHROMA_API_DLLEXT USHORT  expMV2_SetRxRBW(RF_PORT PortIndex,const RBW uiEnum)
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
CHROMA_API_DLLEXT USHORT  expMV2_SetVsaTriggerType(RF_PORT PortIndex,const MP5800_VSA_Trigger_Type uiTriggerType)
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
		if(MultiAPP[usIndex].m_uiRBW_Enum==uiTriggerType)
			return SUCCESS;
	}
	bFlag=MultiAPP[usIndex].SetVsaTriggerType(uiTriggerType);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetVsaTriggerType fail",usIndex);
		return E_Command_ERR;}
	MultiAPP[usIndex].m_uiRBW_Enum=uiTriggerType;
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT  expMV2_SetRxRBW_Int(RF_PORT PortIndex,const int iIndex)
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
	bFlag=MultiAPP[usIndex].SetRxRBW((RBW)uiTemp);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetRxRBW fail",usIndex);
		return E_Command_ERR;}
	MultiAPP[usIndex].m_uiRBW_Enum=uiTemp;
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT  expMV2_GetRxRBW(RF_PORT PortIndex,UINT64 &uiRBW_Hz)
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
CHROMA_API_DLLEXT USHORT  expMV2_PowerReferenceAuto(RF_PORT PortIndex)
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
CHROMA_API_DLLEXT USHORT  expMV2_SetVsaCaptureLength(RF_PORT PortIndex,const double dCapturetureLength_ms)
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
		if(MultiAPP[usIndex].m_dCaptureLength_ms==dCapturetureLength_ms)
			return SUCCESS;
	}
	bFlag=MultiAPP[usIndex].SetVsaCaptureLength(dCapturetureLength_ms);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetVsaCaptureLength fail",usIndex);
		return E_Command_ERR;}
	MultiAPP[usIndex].m_dCaptureLength_ms=dCapturetureLength_ms;
	return SUCCESS;
}
 
CHROMA_API_DLLEXT USHORT  expMV2_SetVsaTriggerTimeOut(RF_PORT PortIndex,const double dTriggerTimeOut_ms)
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
		if(MultiAPP[usIndex].m_dTriggerTimeOut_ms == dTriggerTimeOut_ms)
			return SUCCESS;
	}
	bFlag=MultiAPP[usIndex].SetVsaTriggerTimeOut(dTriggerTimeOut_ms);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetVsaCaptureLength fail",usIndex);
		return E_Command_ERR;}
	MultiAPP[usIndex].m_dTriggerTimeOut_ms == dTriggerTimeOut_ms;
	return SUCCESS;
}
 
CHROMA_API_DLLEXT USHORT  expMV2_SetVsaGapTime(RF_PORT PortIndex,const double dGapTime_us)
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
		if(MultiAPP[usIndex].m_dGapTime_us== dGapTime_us )
			return SUCCESS;
	}
	bFlag=MultiAPP[usIndex].SetVsaGapTime(dGapTime_us);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetVsaGapTime fail",usIndex);
		return E_Command_ERR;}
	MultiAPP[usIndex].m_dGapTime_us = dGapTime_us;
	return SUCCESS;
}
 
CHROMA_API_DLLEXT USHORT  expMV2_SetRxReferenceLevel(RF_PORT PortIndex,const double fRefLevel)
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
CHROMA_API_DLLEXT USHORT  expMV2_GetRxReferenceLevel(RF_PORT PortIndex,double &fRefLevel)
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

CHROMA_API_DLLEXT USHORT  expMV2_GetPeakData(RF_PORT PortIndex,double &fPeakFreqMHz,double &fPeakPower)
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
CHROMA_API_DLLEXT USHORT  expMV2_GetFSK_FreqDeviation(RF_PORT PortIndex,double &fFreqDevMHz)
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

CHROMA_API_DLLEXT USHORT  expMV2_GetFSKDemodulation(RF_PORT PortIndex,double &dFreqDevMHz,double &dFreqOffset,double &dFreqDrift,double &DevError,double &FskError)
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

	MultiAPP[usIndex].MeasureGFSK(dFreqDevMHz,dFreqOffset,dFreqDrift,DevError,FskError);
	return SUCCESS;
}


CHROMA_API_DLLEXT USHORT  expMV2_SetVsaRx_Port(const RF_PORT PortIndex)
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

	bool bDistributionMode = false;
	MultiAPP[usIndex].GetDistributionModeOnOff(bDistributionMode);    // get distribution mode 
	 
	if (bDistributionMode)
	{
		bFlag=MultiAPP[usIndex].SetDistributionModeOnOff(false);   // turn off distribution mode 
		if(!bFlag)
		{
			sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetDistributionModeOnOff Error",usIndex);
			return bFlag;
		}
	}

	bFlag=MultiAPP[usIndex].SetVsaRx_Port(uiPortIndex);      // set vsa port
	
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetVsaRx_Port fail",usIndex);
		return E_Command_ERR;}

	MultiAPP[usIndex].m_uiVsa_Port=uiPortIndex;
	Sleep(SetPortDelay);
	
	if(bDistributionMode)
	{
		bFlag=MultiAPP[usIndex].SetDistributionModeOnOff(true);
		if(!bFlag)
		{
			sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetDistributionModeOnOff Error",usIndex);
			return bFlag;
		}
	}
	
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT  expMV2_Set_4Port_Switch(const RF_PORT PortFirstIndex,	const PORT_MODE pm_PORT_1,
	const PORT_MODE pm_Port_2, 
	const PORT_MODE pm_Port_3,
	const PORT_MODE pm_Port_4)
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

	bool bDistributionMode = false;
	MultiAPP[usIndex].GetDistributionModeOnOff(bDistributionMode);    // get distribution mode 
	 
	if (bDistributionMode)
	{
		bFlag=MultiAPP[usIndex].SetDistributionModeOnOff(false);   // turn off distribution mode 
		if(!bFlag)
		{
			sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetDistributionModeOnOff Error",usIndex);
			return bFlag;
		}
	}

	bFlag=MultiAPP[usIndex].Set_4Port_Switch(pm_PORT_1,pm_Port_2,pm_Port_3,pm_Port_4);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:Set_4Port_Switch fail",usIndex);
		return E_Command_ERR;}
	Sleep(SetPortDelay);
	
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetVsaRx_Port fail",usIndex);
		return E_Command_ERR;}
	
	if(bDistributionMode)
	{
		bFlag=MultiAPP[usIndex].SetDistributionModeOnOff(true);
		if(!bFlag)
		{
			sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:SetDistributionModeOnOff Error",usIndex);
			return bFlag;
		}
	}


	
   // Sleep(2);  // 0616
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT  expMV2_SetCWFrequencyOffset(RF_PORT PortIndex,const double fFreqOffsetMHz)
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
CHROMA_API_DLLEXT USHORT  expMV2_SetTxOutputLevel(RF_PORT PortIndex,const double fOutputLevel)//10~-90 dBm
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
CHROMA_API_DLLEXT USHORT  expMV2_GetTxOutputLevel(RF_PORT PortIndex,double &fOutputLevel)
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
CHROMA_API_DLLEXT USHORT  expMV2_LoadMemory(RF_PORT PortIndex,const char * strFileName)
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
	/*
	int iResult=0;
	string str_Temp=strFileName;
	iResult=MultiAPP[usIndex].stdStr_LoadFileName.compare(str_Temp);
	if(iResult==0)
		return SUCCESS;
	else
		MultiAPP[usIndex].stdStr_LoadFileName=strFileName;
     */
	bFlag=MultiAPP[usIndex].LoadPlayFile(strFileName);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:LoadMemory fail",usIndex);
		return E_Command_ERR;}
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT  expMV2_LoadMemoryWithFilePath(RF_PORT PortIndex,const char * strFileName ,const char * strFilePath)
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
	/*
	int iResult=0;
	string str_Temp=strFileName;
	iResult=MultiAPP[usIndex].stdStr_LoadFileName.compare(str_Temp);
	if(iResult==0)
		return SUCCESS;
	else
		MultiAPP[usIndex].stdStr_LoadFileName=strFileName;
     */
	bFlag=MultiAPP[usIndex].LoadPlayFileWithFilePath(strFileName, strFilePath);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:LoadMemory fail",usIndex);
		return E_Command_ERR;}
	return SUCCESS;
}


CHROMA_API_DLLEXT USHORT  expMV2_SetVsgPlayGapUs(RF_PORT PortIndex,const UINT GapUs)
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
	bFlag=MultiAPP[usIndex].SetVsgPlayGapUs(GapUs);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:Set Vsg play  gap time fail",usIndex);
		return E_Command_ERR;}
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT  expMV2_SetVsaTriggerOffsetUs(RF_PORT PortIndex,const int OffsetUs)
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
	bFlag=MultiAPP[usIndex].SetVsaTriggerOffsetUs(OffsetUs);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:Set Vsa Trigger Offset time fail",usIndex);
		return E_Command_ERR;}
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT  expMV2_SetVsgPlayCount(RF_PORT PortIndex,const int count)
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
	bFlag=MultiAPP[usIndex].SetVsgPlayCount(count);
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:Set Play count fail",usIndex);
		return E_Command_ERR;}
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT  expMV2_TxPlayRepeatOnOff(RF_PORT PortIndex,const bool bOnOff)
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

CHROMA_API_DLLEXT USHORT  expMV2_GetCurrentFreqLoss(const double fFreq,const RF_PORT PortIndex,double& fTxLoss,double& fRxLoss)
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

CHROMA_API_DLLEXT USHORT  expMV2_GetChannelPower(RF_PORT PortIndex,double &fChannelPower)
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

CHROMA_API_DLLEXT USHORT  expMV2_ModulationOnOff(RF_PORT PortIndex,const bool bModulationOnOff)
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

	 
	if (SAVING_TIME)  // 0602 // disable for disconnection will cause asynchronization with remote MP5800
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
			/*  // debug 20170320 -- Allen
			{
				bFlag = MultiAPP[usIndex].SetVsgOnOff(false);  //  disable "RUN"
				if (!bFlag)
				{
					sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:Set Run_Enablr/Disable fail", usIndex);
					return E_Command_ERR;
				}
			}
			*/
		//	if (MultiAPP[usIndex].byteModulationStatus != _OFF)
			{
				bFlag = MultiAPP[usIndex].SetVsgModulationOnOff(false);  // set cw
				MultiAPP[usIndex].m_bModulationOnOff = false;
				if (!bFlag) {
					sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:Set ModulationOnOff fail", usIndex);
					return E_Command_ERR;
				}
				/*
				// workrun for CW fail  // 20160826
				bFlag = MultiAPP[usIndex].SetVsgModulationOnOff(true);  // set cw
				MultiAPP[usIndex].m_bModulationOnOff = false;
				if (!bFlag) {
					sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:Set ModulationOnOff fail", usIndex);
					return E_Command_ERR;
				}
			
				// workrun for cw fail  // 20160826
				bFlag = MultiAPP[usIndex].SetVsgModulationOnOff(false);  // set cw
				MultiAPP[usIndex].m_bModulationOnOff = false;
				if (!bFlag) {
					sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:Set ModulationOnOff fail", usIndex);
					return E_Command_ERR;
				}
				*/
			}

		}



	}
	
	
	
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT  expMV2_Rf_OnOff(RF_PORT PortIndex,bool bRfOnOff)
{
	//      Close :  RF off , output stop : ==> rf = false, output start = off
	//      modulation:  CW ->(1)set CW,     WAVE -> (1) set WAVE , (2) set output start
	//      run     rf on

  
 //	so when RF_ON_OFF routiine 
//	if rf = false ; rf = off , output = false
 //    if rf = true ; rf = on
// in modulation routine
 //   if mod = cw; mode= CW
//	if mod = mod ; mode = modem, output = true;


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
	 	 if  (!bRfOnOff)  // 0511 for cycle time variation   //allen  // cw mod will continue s
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
	/* 
	if(     (( MultiAPP[usIndex].byteRfOnStatus == _ON ) && (!bRfOnOff) )
		||  ((MultiAPP[usIndex].byteRfOnStatus == _OFF ) && (bRfOnOff) ) 
		||  (MultiAPP[usIndex].byteRfOnStatus == _ERROR ) )
	*/  
 
	{
	 	bFlag=MultiAPP[usIndex].SetVsgRfOnOff(bRfOnOff);  // 
	  
     
		if(!bFlag)
		{
			sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:Set Rf_OnOff fail",usIndex);
			return E_Command_ERR;
		}
	}
	 
	MultiAPP[usIndex].m_bRF_OnOff=bRfOnOff;
	
	return SUCCESS;
}
CHROMA_API_DLLEXT USHORT  expMV2_Get_Rf_OnOff(RF_PORT PortIndex,bool& bRfOnOff)
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
// CHROMA_API_DLLEXT USHORT  expMV2_SetVsgOnOff(RF_PORT PortIndex,const bool bOnOff)
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
CHROMA_API_DLLEXT USHORT  expMV2_SetVsaOnOff(RF_PORT PortIndex,const bool bOnOff)
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
CHROMA_API_DLLEXT USHORT  expMV2_SetMark(RF_PORT PortIndex,const double fMarkFreqMHz)
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

CHROMA_API_DLLEXT USHORT  expMV2_ClearMark(RF_PORT PortIndex)
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
CHROMA_API_DLLEXT USHORT  expMV2_GetMarkValue(RF_PORT PortIndex,double &fValueMHz)
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

CHROMA_API_DLLEXT USHORT  expMV2_GetPowerByTime(RF_PORT PortIndex,double &dPowerByTime)
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
	//Sleep(GetMarkDelay);
	MultiAPP[usIndex].GetPowerByTime(dPowerByTime);
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT  expMV2_GetPeakPowerFrequencyHzByTime(RF_PORT PortIndex,double &dPeakPowerFrequencyHzByTime)
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
	//Sleep(GetMarkDelay);
	MultiAPP[usIndex].GetPeakPowerFrequencyHzByTime(dPeakPowerFrequencyHzByTime);
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT  expMV2_DemodFskOpen(RF_PORT PortIndex,bool bIsOpen)
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

CHROMA_API_DLLEXT USHORT  expMV2_Set_GPRF_DemodulationType( const RF_PORT PortIndex, const MP5800_GPRF_Demodulation& demodualtion)
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
	
	MultiAPP[usIndex].SetDemodulationtype(demodualtion);
	 
	
	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT  expMV2_SetFskDemodulation(RF_PORT PortIndex,USHORT usSolution ,const USHORT usFilerType ,const USHORT usCountinuity, const USHORT usFSKType,const USHORT usFilterLength, const double dBandwidthTime, const double dSymbolRateMHz, const double dFrequencyDeviation_kHz )
//{
//	return SUCCESS;
//}
/* 
CHROMA_API_DLLEXT USHORT  expMV2_DemodFskSetting(RF_PORT PortIndex)
	//
	//													  USHORT usSolution ,
	//													  USHORT usFilerType )
														  
	                                                      USHORT usCountinuity)
														
														  USHORT usFSKType ,
														  USHORT usFilterLength
														  
														  double dBandwidthTime ,
														  double dSymbolRateMHz ,
														  double dFrequencyDeviation_kHz )
*/														 
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
	

		expMV2_Set_GPRF_DemodulationType( PortIndex, FSK );
		//MultiAPP[usIndex].SetDemodulationtype(1); //Open Demod type
		 
		MultiAPP[usIndex].SetDemodulationTool(usSolution);  // 0:Freq-Deviation
		MultiAPP[usIndex].FskSetFilterType(usFilerType);    // 3:Gaussian
	 	MultiAPP[usIndex].FskSetSymbolPhaseContinuity(usCountinuity);  //0:continuous
	 	MultiAPP[usIndex].FskSetType(usFSKType); // 0 : 2-Fsk
		
	 	MultiAPP[usIndex].FskSetFilterLength(usFilterLength); // 6
	 	MultiAPP[usIndex].FskSetcoefficient(dBandwidthTime);
	 	MultiAPP[usIndex].FskSetSymbolRate(dSymbolRateMHz);
	 	MultiAPP[usIndex].FSKSetFreqDeviation(dFrequencyDeviation_kHz);
	 
	
	return SUCCESS;
}
 
CHROMA_API_DLLEXT USHORT  expMV2_SetDisplayOnOffByConnection( bool bDebugMode)
{
	bool bFlag = false;
	for(int i=0;i<MaxInstr;i++)
	{
		if(MultiAPP[i].s_bIsServer_Connected[i]) 
		{
			bFlag=MultiAPP[i].SetSpectrumOnOff(bDebugMode);
			if(!bFlag){
			sprintf_s(MultiAPP[i].s_ErrMessage,"Server_%d:Set SpectrumOnOff fail",i);
			return E_Command_ERR;}

		}
	}

	return SUCCESS;
}

CHROMA_API_DLLEXT USHORT  expMV2_SetSpectrumOnOff(RF_PORT PortIndex,const bool bOnOff)
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

CHROMA_API_DLLEXT USHORT  expMV2_SetCableLossTableCoupleMode(RF_PORT PortIndex,const bool bOnOff)
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


CHROMA_API_DLLEXT USHORT  expMV2_SetMeasureMode(RF_PORT PortIndex, const MP5800_STANDARD standard)
{
	bool bFlag = false;
	USHORT usIndex = PortIndex / Port_Nums;
	if (usIndex >= MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage, "PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if (!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:No Connection", usIndex);
		return  E_Server_NO_CONNECT;
	}
	if (MultiAPP[usIndex].measureMode != standard )
	{ 
			bFlag = MultiAPP[usIndex].SetMeasureMode((Mea_Mode)standard);
			if (!bFlag) {
				sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:Set SetMeasureMode fail", usIndex);
				return E_Command_ERR;
			}
	}
	MultiAPP[usIndex].measureMode = standard;
	return SUCCESS;
}


CHROMA_API_DLLEXT USHORT  expMV2_SetFilterWindow(RF_PORT PortIndex, const UINT filterType)
{
	bool bFlag = false;
	USHORT usIndex = PortIndex / Port_Nums;
	if (usIndex >= MaxInstr)////////////////////////////
	{
		sprintf_s(MultiAPP[0].s_ErrMessage, "PortIndex out of define");
		return E_Input_OutOfDefine;
	}
	if (!MultiAPP[usIndex].s_bIsServer_Connected[usIndex])
	{
		sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:No Connection", usIndex);
		return  E_Server_NO_CONNECT;
	}
	bFlag = MultiAPP[usIndex].SetFilterWindow(filterType);
	if (!bFlag) {
		sprintf_s(MultiAPP[usIndex].s_ErrMessage, "Server_%d:Set SetFilterWindow fail", usIndex);
		return E_Command_ERR;
	}
	return SUCCESS;
}
 

CHROMA_API_DLLEXT USHORT  expMV2_GetCWChannelPower(RF_PORT PortIndex,const double fMarkFreqMHz, const double leftSpan_Hz, const double rightSpan_Hz,
	double &PowerMeasured, double &PowerMeasured_Left, double &PowerMeasured_Right, double &Power_Comp, double &Frequency_Offset)
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
 
	 
	MultiAPP[usIndex].GetCWChannelPower(fMarkFreqMHz,leftSpan_Hz,rightSpan_Hz,PowerMeasured,PowerMeasured_Left,PowerMeasured_Right,Power_Comp,Frequency_Offset);
	return SUCCESS;
}


CHROMA_API_DLLEXT USHORT  expMV2_SetDigitalDownConvertor(RF_PORT PortIndex,const bool bOnOff,  const double offSet_Hz )
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

    bFlag=MultiAPP[usIndex].SetDigitalDownConvertor( bOnOff, offSet_Hz );
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:Clear Mark fail",usIndex);
		return E_Command_ERR;}
	return SUCCESS;
}
	 
	 
CHROMA_API_DLLEXT USHORT  expMV2_GetChannelPower2Span(RF_PORT PortIndex,const double fMarkFreqMHz, const double leftSpan_Hz, const double rightSpan_Hz,
	double &PowerMeasured, double &PowerMeasured_Left, double &PowerMeasured_Right)
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
 
	 
	MultiAPP[usIndex].GetChannelPower2Span(fMarkFreqMHz,leftSpan_Hz,rightSpan_Hz,PowerMeasured,PowerMeasured_Left,PowerMeasured_Right);
	return SUCCESS;
}

CHROMA_API_DLLEXT UINT  expMV2_Set_WIFI_RX(   const RF_PORT                    PortIndex  ,        \
	                                                      const UINT                           frequency_MHz ,                  \
	                                                     const double                    referenceLevel_dBm  ,                  \
														  const double                      captureLength_ms  ,                 \
														  const WIFI_STANDARD                wifiStandard   ,                      \
														 const WIFI_BANDWIDTH                wifiBandwidth  ,         \
														 const  WIFI_DATA_RATE                 wifiDataRate ,
														  const bool                             isBeamforming )
														

{
	

	unsigned int uiPortIndex=0;
	bool bFlag=false;
	UINT ErrorCode = DEFAULT_ERROR;
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
	/*
	if (wifiStandard != _802_11b)
	{
	     expMV2_SetMeasureMode(PortIndex, WIFI_OFDM);
	}
	else
	{
		expMV2_SetMeasureMode(PortIndex, WIFI_DSSS);
	}
	*/
 
	expMV2_ModulationOnOff(PortIndex,true);
	uiPortIndex+=((unsigned int)PortIndex%Port_Nums) + 1;
    ErrorCode=MultiAPP[usIndex].Set_WIFI_RX(uiPortIndex  , frequency_MHz, referenceLevel_dBm ,captureLength_ms, wifiStandard, wifiBandwidth ,wifiDataRate  ,isBeamforming );

/*	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:Clear Mark fail",usIndex);
		return E_Command_ERR;}
		*/
	return ErrorCode;
}



CHROMA_API_DLLEXT UINT  expMV2_Set_WIFI_BT_RX_Advance(  const RF_PORT                    PortIndex  ,
	                                                         const WIFI_BT_TRIGGER_TYPE               wifiBT_TriggerType            , \
	                                                         const double                             wifiBT_TriggerGapTime_ms   , \
														     const double                             wifiBT_TriggerThreshold        , \
														     const double                             wifiBT_TriggerOffset          ,  \
														     const WIFI_FREQUENCY_CORRECTION         wifiFrequencyCorrection         , \
														     const  WIFI_CHANNEL_ESTIMATION           wifiChannelEstimation    )

{
	bool bFlag=false;
	UINT ErrorCode = DEFAULT_ERROR;
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
 

    ErrorCode=MultiAPP[usIndex].Set_WIFI_BT_RX_Advance(   wifiBT_TriggerType, wifiBT_TriggerGapTime_ms , wifiBT_TriggerThreshold ,  wifiBT_TriggerOffset, wifiFrequencyCorrection ,wifiChannelEstimation  );
	/*
	if( ErrorCode >0 ){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:Set WIFI TX Advance  fail",usIndex);
		return E_Command_ERR;}
		*/
	return  ErrorCode;
}


/*
CHROMA_API_DLLEXT UINT  expMV2_Get_WIFI_RX(  const RF_PORT    PortIndex    ,            \
                                                        double         &TXPowerdBm,               \
														double         &EVM,                      \
														bool           &MASK,                     \

													   double           &PhaseError,              \
													   double           &FrequencyError,          \
													   double           &SymbolClockError,        \
													   double           &LoLeakage,	              \
													   double			&MaskTotalCount,          \
													   double			&MaskErrorCount,          \
	
													   bool             &isBFON,                  \
													   double           &PreamblePower            )
*/
CHROMA_API_DLLEXT UINT  expMV2_Get_WIFI_RX(  const RF_PORT    PortIndex    ,            \
                                                        double         &TXPowerdBm,               \
														double         &EVM,                      \
														bool           &MASK,                     \

													   double           &PhaseError,              \
													   double           &FrequencyError,          \
													   double           &SymbolClockError,        \
													   double           &LoLeakage )
{
	bool bFlag=false;
	UINT ErrorCode = DEFAULT_ERROR;
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

 //  ErrorCode=  MultiAPP[usIndex].Get_WIFI_RX( TXPowerdBm, EVM , MASK ,  PhaseError, FrequencyError , SymbolClockError ,LoLeakage, MaskTotalCount, MaskErrorCount ,isBFON , PreamblePower);
	 ErrorCode=  MultiAPP[usIndex].Get_WIFI_RX( TXPowerdBm, EVM , MASK ,  PhaseError, FrequencyError , SymbolClockError ,LoLeakage);
	return ErrorCode;

}



CHROMA_API_DLLEXT UINT  expMV2_Set_WIFI_TX(
	                                                    const RF_PORT                 PortIndex  ,       \
                                                        const UINT                    frequency_MHz  ,      \
														 const double                    outPower_dBm  ,    \
														 const UINT                       packetCount ,       \
														  const WIFI_STANDARD                wifiStandard   ,       \
														const WIFI_BANDWIDTH       wifiBandwidth  ,      \
														const WIFI_DATA_RATE                 wifiDataRate        )

{
	unsigned int uiPortIndex=0;
	UINT ErrorCode = DEFAULT_ERROR;
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
	uiPortIndex+=((unsigned int)PortIndex%Port_Nums) + 1;

/*
	if (wifiStandard != _802_11b)
	{
	     expMV2_SetMeasureMode(PortIndex,MEASURE_MODE_WIFI_OFDM);
	}
	else
	{
		expMV2_SetMeasureMode(PortIndex,MEASURE_MODE_WIFI_DSSS);
	}
*/	
    ErrorCode=MultiAPP[usIndex].Set_WIFI_TX(uiPortIndex , frequency_MHz ,  outPower_dBm ,   packetCount,  wifiStandard, wifiBandwidth ,wifiDataRate );
	/*
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:set wifi rx",usIndex);
		return E_Command_ERR;}
		*/
	return ErrorCode;
}


CHROMA_API_DLLEXT UINT  expMV2_Set_WIFI_TX_Start(   const RF_PORT      PortIndex , const bool OnOff   )
{
    unsigned int uiPortIndex=0;
	UINT ErrorCode = DEFAULT_ERROR;
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
	uiPortIndex+=((unsigned int)PortIndex%Port_Nums) + 1;
   
  //  ErrorCode=MultiAPP[usIndex].Set_WIFI_TX_Start();
    if ( OnOff)
	{
		//expMV2_Rf_OnOff(PortIndex,false);
 		ErrorCode =  expMV2_ModulationOnOff(PortIndex,true);  	
		
		ErrorCode +=  expMV2_Rf_OnOff(PortIndex,true)  ;
	}
	else
	{  
		   
		 	ErrorCode =  expMV2_Rf_OnOff(PortIndex,false)  ;
	}
	/*
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:set wifi rx",usIndex);
		return E_Command_ERR;}
		*/
	return ErrorCode;

}


// ------------------------------------------------------ BT ---------------------------------------------------------------

CHROMA_API_DLLEXT UINT  expMV2_Set_BT_RX(
	                                                    const RF_PORT                    PortIndex      ,                  \
														const UINT                           frequency_MHz  ,                   \
														const double                    referenceLevel_dBm ,                   \
														const double                      captureLength_ms ,                   \
														const BT_PACKET_TYPE             btPacketType      )
{

	unsigned int uiPortIndex=0;
	bool bFlag=false;
	UINT ErrorCode = DEFAULT_ERROR;
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

		//expMV2_SetMeasureMode(PortIndex,MEASURE_MODE_BLUETOOTH);
	
		expMV2_ModulationOnOff(PortIndex,true);
	uiPortIndex+=((unsigned int)PortIndex%Port_Nums) + 1;
    ErrorCode=MultiAPP[usIndex].Set_BT_RX(uiPortIndex  , frequency_MHz, referenceLevel_dBm ,captureLength_ms, btPacketType  );

/*	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:Clear Mark fail",usIndex);
		return E_Command_ERR;}
		*/
	return ErrorCode;

		/*
		MP5K_BT_TxTestMeas bt_tx_meas;
		ErrorCode = tester.BT_Tx_Meas( handle , bt_tx_meas); // Return tx test item

		MP5K_BT_TxTestMeasExtend bt_tx_measext;
		ErrorCode = tester.BT_Tx_MeasExtend( handle , bt_tx_measext); // Return tx test item
		*/
}



CHROMA_API_DLLEXT UINT  expMV2_Get_BT_RX(
	                                                    const RF_PORT  PortIndex    ,                    \
                                                        double         &targetPower,                     \
														double         &initialFrequencyError,           \
														double         &BDR_BW_20db,                     \
													    double         &BDR_FreqDrift,                   \
														double         &BDR_DeltaF1Avg,                  \
													    double         &BDR_DeltaF2Avg,                  \
													    double         &BDR_DeltaF2Max,	                 \
													    double		   &EDR_DEVM,                        \
													    double		   &EDR_DEVMPeak,                     \
														double         &LE_DeltaF1Avg,                   \
													    double         &LE_DeltaF2Avg,                   \
													    double         &LE_DeltaF2Max       )
{
	bool bFlag=false;
	UINT ErrorCode = DEFAULT_ERROR;
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

   ErrorCode=  MultiAPP[usIndex].Get_BT_RX( targetPower, initialFrequencyError , BDR_BW_20db ,  BDR_FreqDrift, BDR_DeltaF1Avg , BDR_DeltaF2Avg ,BDR_DeltaF2Max, EDR_DEVM, EDR_DEVMPeak ,LE_DeltaF1Avg , LE_DeltaF2Avg ,LE_DeltaF2Max);
	
	return ErrorCode;
}
 

 

CHROMA_API_DLLEXT UINT   expMV2_Set_BT_TX(
	                                                    const RF_PORT                 PortIndex      ,          \
                                                        const UINT                    frequency_MHz  ,               \
														 const double                 outPower_dBm  ,         \
														 const double                  gapTime  ,                   \
														 const  UINT			       bitCount ,                   \
														 const BT_PACKET_TYPE            btPacketType     )
{

	unsigned int uiPortIndex=0;
	UINT ErrorCode = DEFAULT_ERROR;
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
	uiPortIndex+=((unsigned int)PortIndex%Port_Nums) + 1;
	 
//	expMV2_SetMeasureMode(PortIndex,MEASURE_MODE_BLUETOOTH);
	 
     ErrorCode=MultiAPP[usIndex].Set_BT_TX(uiPortIndex , frequency_MHz ,  outPower_dBm ,    gapTime * 1e-6,  bitCount, btPacketType  );
	/*
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:set wifi rx",usIndex);
		return E_Command_ERR;}
		*/
	return ErrorCode;

}

CHROMA_API_DLLEXT UINT  expMV2_Set_BT_TX_Start(
	                                                          const RF_PORT                 PortIndex , const bool OnOff     )
{

	 unsigned int uiPortIndex=0;
	UINT ErrorCode = DEFAULT_ERROR;
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
	uiPortIndex+=((unsigned int)PortIndex%Port_Nums) + 1;
   
  //  ErrorCode=MultiAPP[usIndex].Set_WIFI_TX_Start();
    
	if (OnOff)
	{
	ErrorCode =  expMV2_ModulationOnOff(PortIndex,true) ? 0: 400;
	ErrorCode +=  expMV2_Rf_OnOff(PortIndex,true) ? 0: 1;
	}
	else
	{
		ErrorCode +=  expMV2_Rf_OnOff(PortIndex,false) ? 0: 1;
	}
	/*
	if(!bFlag){
		sprintf_s(MultiAPP[usIndex].s_ErrMessage,"Server_%d:set wifi rx",usIndex);
		return E_Command_ERR;}
		*/
	return ErrorCode;

}
 

 
