#include "stdafx.h"
#include "Chroma_API_V2.h"
#define MAX_STRING_LEN			1014

//CChroma_API_V2 theAPP; //****The Only one, don't delete.**** 

CChroma_API_V2 MultiAPP[MaxInstr];

// This is the constructor of a class that has been exported.
// see Chroma_API_V2.h for the class definition
bool CChroma_API_V2::s_bIsOpenProfile=false;
CString CChroma_API_V2::s_szConfigFilePath="";
bool CChroma_API_V2::s_bIsServer_Connected[]={false,false};
bool CChroma_API_V2::s_bIsServer_OnOff[]={false,false};
CString CChroma_API_V2::s_szServer_IP[]={"",""};
CString CChroma_API_V2::s_szServer_Port[]={"",""};
CString CChroma_API_V2::s_szServer_PortLoss[]={"",""};
//CString CChroma_API_V2::s_szServer_PortDef[]={"",""};
vector<CString> CChroma_API_V2::s_vec_WaveformPath;
char CChroma_API_V2::s_ErrMessage[];


string CstringToString(CString szInput)
{
	string szOutput="";	
	int i=szInput.GetLength();
	char *chr=new char[i+1];
	WideCharToMultiByte(CP_ACP,0,szInput.GetBuffer(),-1,chr,szInput.GetLength(),NULL,NULL);
	chr[i]='\0';
	szOutput=chr;	
	delete chr;
	return szOutput;
}
CChroma_API_V2::CChroma_API_V2()
{
	m_f_Center_CurrentFreq=0.0f;
	m_fMarkOffset=-9999;
	ZeroMemory(m_PortLossEnable,sizeof(m_PortLossEnable) );
	m_CurrentVsaPort=999;
	m_uiVsa_Port=999;
	m_uiRBW_Enum=999;
	stdStr_LoadFileName=NONE;
	bIsServerConnectFlag=false;
	//m_Instr_CableLossOnOff=false;
	m_usDemodTool_Index=999;
	m_usFSK_SetType=999;
	m_usFSK_SampleRateMHz=999;
	m_usFSK_FilterType=999;
	m_usFSK_FilterLength=999;
	m_fFskcoefficient=-9999;
	m_usFskSymbolPhaseContinuity=999;
	return;
}

CChroma_API_V2::~CChroma_API_V2()
{

}

void CChroma_API_V2::GetDutCurrentTxLoss(float fFreq,float& fTxLoss)
{
	LossPoint TxCableLoss;
	if(m_bDutCableLossEnable) //DUT Tx
	{
		FindFreqLoss(fFreq,vec_DutCableLossList,TxCableLoss);
	}else
	{
		TxCableLoss.fTxLoss[0]=0.0f;
		TxCableLoss.fTxLoss[1]=0.0f;
		TxCableLoss.fTxLoss[2]=0.0f;
		TxCableLoss.fTxLoss[3]=0.0f;
	}

	fTxLoss=TxCableLoss.fTxLoss[m_CurrentVsaPort];
}

bool CChroma_API_V2::GetCurrentFreqLoss(const float fFreq,const unsigned int uiPortIndex,float& fTxLoss,float& fRxLoss)
{
	LossPoint CableLoss;
	if(m_bDutCableLossEnable) //DUT Rx
	{
		FindFreqLoss(m_f_Center_CurrentFreq,vec_DutCableLossList,CableLoss);
	}else
	{
		for(int i=0;i<4;i++)
		{
			CableLoss.fRxLoss[i]=0.0f;
			CableLoss.fTxLoss[i]=0.0f;
		}
	}
	
	fTxLoss=CableLoss.fTxLoss[uiPortIndex];
	fRxLoss=CableLoss.fRxLoss[uiPortIndex];
	return true;
}
bool CChroma_API_V2::SetFreqPointTxOutputLevelAndPortAtt(const float fOutputLevel)
{
// 	LossPoint TxCableLoss,PortDef;	
// 	if(m_bDutCableLossEnable) //DUT Rx
// 	{
// 		FindFreqLoss(m_f_Center_CurrentFreq,vec_DutCableLossList,TxCableLoss);
// 	}else
// 	{
// 		TxCableLoss.fRxLoss[0]=0.0f;
// 		TxCableLoss.fRxLoss[1]=0.0f;
// 		TxCableLoss.fRxLoss[2]=0.0f;
// 		TxCableLoss.fRxLoss[3]=0.0f;
// 	}
// 	if(m_bInstrPortDefEnable) //INSTR TX
// 	{
// 		FindFreqLoss(m_f_Center_CurrentFreq,vec_InstrPortDefList,PortDef);
// 	}else
// 	{
// 		PortDef.fTxLoss[0]=0.0f;
// 		PortDef.fTxLoss[1]=0.0f;
// 		PortDef.fTxLoss[2]=0.0f;
// 		PortDef.fTxLoss[3]=0.0f;
// 	}
// 	float fValue[4];
// 	for(int i=0;i<Port_Nums;i++)
// 	{
// 		fValue[i]=TxCableLoss.fRxLoss[i]+PortDef.fTxLoss[i];
// 	}
// 	float fMin=fValue[0];
// 	for(int i=1;i<4;i++)
// 	{
// 		fMin=min(fMin,fValue[i]);
// 	}
// 
// 	int iOffset=(int)fMin;
// 	float fSetOutPutLevel=fOutputLevel-(float)iOffset;
// 	SetTxOutputLevel(fSetOutPutLevel);
// 	
// 	for(int i=0;i<4;i++)
// 	{
// 		unsigned int uiTemp=0;
// 		uiTemp=8+(unsigned int)( ( (fValue[i]-(float)iOffset)/0.25));
// 		if(uiTemp>63)
// 		m_uiPortAtt[i]=63;
// 		else
// 		m_uiPortAtt[i]=uiTemp;	
// 	}
// 	Set_4Port_OutputATT(m_uiPortAtt[0],m_uiPortAtt[1],m_uiPortAtt[2],m_uiPortAtt[3]);
	return true;
}
bool CChroma_API_V2::FindFreqLoss(float fFreq,const vector<LossPoint>& LossPointTable,LossPoint& loss_point)
{
	
	if(LossPointTable.size()==0)
	{
	   memset(&loss_point,0,sizeof(loss_point));
	   return false;
	}else
	{
		if(LossPointTable.at(0).fFrequency>=fFreq)
		{
			loss_point.fFrequency=fFreq;
			for(int i=0;i<4;i++)
			{
				loss_point.fTxLoss[i]=LossPointTable.at(0).fTxLoss[i];
				loss_point.fRxLoss[i]=LossPointTable.at(0).fRxLoss[i];
			}
			return true;
		}else if(LossPointTable.at(LossPointTable.size()-1).fFrequency<=fFreq)
		{
			loss_point.fFrequency=fFreq;
			for(int i=0;i<4;i++)
			{
				
				loss_point.fTxLoss[i]=LossPointTable.at(LossPointTable.size()-1).fTxLoss[i];
				loss_point.fRxLoss[i]=LossPointTable.at(LossPointTable.size()-1).fRxLoss[i];
			}
			return true;
		}
		int iLow=0,iHigh=LossPointTable.size()-1;
		while( iLow<= iHigh)
		{
			
			int iMid=(iLow+iHigh)/2;
			
			if(LossPointTable[iMid].fFrequency==fFreq)
			{
				loss_point.fFrequency=fFreq;
				for(int i=0;i<4;i++)
				{

					loss_point.fTxLoss[i]=LossPointTable.at(iMid).fTxLoss[i];
					loss_point.fRxLoss[i]=LossPointTable.at(iMid).fRxLoss[i];
				}
				return true;
			}
			else if(LossPointTable[iMid].fFrequency<fFreq && fFreq<LossPointTable[iMid+1].fFrequency)
			{
				float fTemp=0.0f;
				loss_point.fFrequency=fFreq;
				fTemp= (fFreq-LossPointTable[iMid].fFrequency)/(LossPointTable[iMid+1].fFrequency-LossPointTable[iMid].fFrequency);
				for(int i=0;i<4;i++)
				{
					loss_point.fTxLoss[i]=fTemp*(LossPointTable[iMid+1].fTxLoss[i]-LossPointTable[iMid].fTxLoss[i])+LossPointTable[iMid].fTxLoss[i];
					loss_point.fRxLoss[i]=fTemp*(LossPointTable[iMid+1].fRxLoss[i]-LossPointTable[iMid].fRxLoss[i])+LossPointTable[iMid].fRxLoss[i];
				}
				return true;
			}else
			{
				if(LossPointTable[iMid].fFrequency>fFreq)
				{
					iHigh=iMid-1;
				}else if(LossPointTable[iMid].fFrequency<fFreq)
				{
					iLow=iMid+1;
				}
			}
		}
	}
	return false;
}
bool CChroma_API_V2::SortLossPointTable(vector<LossPoint>& LossPointTable)
{
	LossPoint TempPoint;
	int iTotalNum=LossPointTable.size();

	for(int i=0;i<iTotalNum;i++)
	{
		for(int j=i;j<iTotalNum;j++)
		{
			if(LossPointTable.at(i).fFrequency>LossPointTable.at(j).fFrequency)
			{
				TempPoint.fFrequency=LossPointTable.at(j).fFrequency;
				LossPointTable.at(j).fFrequency=LossPointTable.at(i).fFrequency;
				LossPointTable.at(i).fFrequency=TempPoint.fFrequency;
				for(int k=0;k<4;k++)
				{
					TempPoint.fTxLoss[k]=LossPointTable.at(j).fTxLoss[k];
					TempPoint.fRxLoss[k]=LossPointTable.at(j).fRxLoss[k];

					LossPointTable.at(j).fTxLoss[k]=LossPointTable.at(i).fTxLoss[k];
					LossPointTable.at(j).fRxLoss[k]=LossPointTable.at(i).fRxLoss[k];

					LossPointTable.at(i).fTxLoss[k]=TempPoint.fTxLoss[k];
					LossPointTable.at(i).fRxLoss[k]=TempPoint.fRxLoss[k];
				}
			}
		}//for(int j=i;j<iTotalNum;j++)
	}//for(int i=0;i<iTotalNum;i++)
	return true;
}
void CChroma_API_V2::CurrentFilePath(char * chFileDir)
{
	char szCurrentDir[255];
	//WIN32_FIND_DATAA FindFileData;
	HANDLE	hFind=INVALID_HANDLE_VALUE;
	string strFileDir="",strFileName="";
	CString csFileName="";
	bool bIsFind=false;
	int iIndex=0,iCount=0;

	std::memset(szCurrentDir,0,sizeof(szCurrentDir));
	GetCurrentDirectoryA(sizeof(szCurrentDir),(LPSTR)szCurrentDir);
	strFileDir=szCurrentDir;
	strFileDir=strFileDir+"\\*";
	memcpy(chFileDir,strFileDir.c_str(),strFileDir.size());
}
void CChroma_API_V2::SetCurrentErrorMessage(USHORT usErrorCode)
{
	switch(usErrorCode)
	{
	case E_INSTR_CONNECT_ERR:
		break;
	case E_Server_NO_CONNECT:
		break;
	case E_Input_OutOfDefine:
		break;
	default:
		break;
	}
}
const char* CChroma_API_V2::GetLastErrorMessage()
{
	return MultiAPP[0].s_ErrMessage;
}
USHORT CChroma_API_V2::PresetFunc()
{
	SetCableLossTableCoupleMode(false);  // 0419 
	Adivic_MP5K.SetMeasureMode(_MOD_GPRF);
	SetCenterFrequency(2440);
	m_CenterFrequency=2440;
	SetFreqSpan(160); //default GPRF sampling rate 160MHz
	m_iFreqSpanMHz=160;
	SetRxRBW(RBW_1K_Hz); //1KHz
	m_uiRBW_Enum=0;
	SetRxReferenceLevel(-10);
	m_fRefLevel=0;
	SetVsaRx_Port(Port_1);//Port1
	m_uiVsa_Port=Port_1;
	//Set_4Port_Switch(Switch_OFF,Switch_OFF,Switch_OFF,Switch_OFF);//All OFF
	SetTxOutputLevel(-10);
	SetCWFrequencyOffset(0);
	
	SetVsaOnOff(true);

	TxPlayRepeatOnOff(true);
	SetVsgOnOff(false);
	SetVsgRfOnOff(false);
	SetVsgModulationOnOff(false);
	stdStr_LoadFileName=NONE;
	m_bRF_OnOff=false;
	m_bModulationOnOff=false;
	SetDistributionModeOnOff(false);
	SetAllPortLossFile(true);//Clear Cable Loss Setting
	
	m_fMarkOffset=-999;//Clear Marker
	Adivic_MP5K.ClearMarker();

	return SUCCESS;
}
bool CChroma_API_V2::SaveVsaData(const string *strTableName)
{
	return Adivic_MP5K.SaveVsaData(strTableName);
}
USHORT CChroma_API_V2::InitFunction(const char *IniFilePath)
{
	
	//HANDLE	hFind=INVALID_HANDLE_VALUE;
	string strFileDir="",strFilePath="",strFileName="";
	CString csFileName="";
	bool bIsFind=false;
	int iIndex=0,iCount=0;
	USHORT usState=SUCCESS;
	for(int i=0;i<MaxInstr;i++)
	{
		MultiAPP[i].usInstrIndex=i;
	}
	
	strFilePath=IniFilePath;
	fstream  Openfile; 
	Openfile.open(strFilePath,ios::in|ios::binary);
	if(!Openfile.is_open())
	{
		sprintf_s(s_ErrMessage,"File path:%s open fail",strFilePath.c_str());
		return E_File_No_Exist;
	}
	Openfile.close();
	s_szConfigFilePath=IniFilePath;
	
	usState=ConfingFileParser(); //Init Config file parser

	return usState;
}
USHORT CChroma_API_V2::ConfingFileParser()
{
	CString szSectionName=_T("");
	CString szFieldName=_T("");
	CString szReadString=_T("");
//----------------------------------------------------------------------------------
	szSectionName=_T("Server Config Setting");
 	szFieldName=_T("Server_1_IP");
	GetPrivateProfileString( szSectionName,szFieldName,_T(""),szReadString.GetBuffer(MAX_STRING_LEN),100,s_szConfigFilePath);
 	szReadString.ReleaseBuffer();
	s_szServer_IP[0]=szReadString;

	szFieldName=_T("Server_1_Port");
	GetPrivateProfileString( szSectionName,szFieldName,_T(""),szReadString.GetBuffer(MAX_STRING_LEN),100,s_szConfigFilePath);
	szReadString.ReleaseBuffer();
	s_szServer_Port[0]=szReadString;

	szFieldName=_T("Server_2_IP");
	GetPrivateProfileString( szSectionName,szFieldName,_T(""),szReadString.GetBuffer(MAX_STRING_LEN),100,s_szConfigFilePath);
	szReadString.ReleaseBuffer();
	s_szServer_IP[1]=szReadString;

	szFieldName=_T("Server_2_Port");
	GetPrivateProfileString( szSectionName,szFieldName,_T(""),szReadString.GetBuffer(MAX_STRING_LEN),100,s_szConfigFilePath);
	szReadString.ReleaseBuffer();
	s_szServer_Port[1]=szReadString;

	szFieldName=_T("IsServer_1_Open");
	GetPrivateProfileString( szSectionName,szFieldName,_T("Off"),szReadString.GetBuffer(MAX_STRING_LEN),100,s_szConfigFilePath);
	szReadString.ReleaseBuffer();
	if(szReadString=="On")
		s_bIsServer_OnOff[0]=true;
	else
		s_bIsServer_OnOff[0]=false;

	szFieldName=_T("IsServer_2_Open");
	GetPrivateProfileString( szSectionName,szFieldName,_T("Off"),szReadString.GetBuffer(MAX_STRING_LEN),100,s_szConfigFilePath);
	szReadString.ReleaseBuffer();
	if(szReadString=="On")
		s_bIsServer_OnOff[1]=true;
	else
		s_bIsServer_OnOff[1]=false;
	//-------------------------------------------------------------------------------
	szSectionName=_T("Server File Path");

	szFieldName=_T("Server_1_PortLossPath");
	GetPrivateProfileString( szSectionName,szFieldName,_T(""),szReadString.GetBuffer(MAX_STRING_LEN),100,s_szConfigFilePath);
	szReadString.ReleaseBuffer();
	s_szServer_PortLoss[0]=szReadString;

	szFieldName=_T("Server_2_PortLossPath");
	GetPrivateProfileString( szSectionName,szFieldName,_T(""),szReadString.GetBuffer(MAX_STRING_LEN),100,s_szConfigFilePath);
	szReadString.ReleaseBuffer();
	s_szServer_PortLoss[1]=szReadString;
	//-------------------------------------------------------
	szSectionName=_T("Waveform List");
	s_vec_WaveformPath.clear();
	USHORT usIndex=1;
	do 
	{
		szFieldName.Format(_T("Waveform_Name_%d"),usIndex++);
		GetPrivateProfileString( szSectionName,szFieldName,_T(""),szReadString.GetBuffer(MAX_STRING_LEN),100,s_szConfigFilePath);
		szReadString.ReleaseBuffer();
 		if(szReadString.GetLength()!=0)
 			s_vec_WaveformPath.push_back(szReadString);
	} while (szReadString.GetLength()!=0);

	
	return SUCCESS;
}
USHORT CChroma_API_V2::NetConnect(bool bIsConnect)
{
	char szBuf[1024]={0};

	if(bIsConnect)
	{
		if(!bIsServerConnectFlag)
			return E_Server_NO_CONNECT;
		else
		{
			sprintf_s(szBuf,"net use \\\\%s\\%s \"\" /%s:adivic",stdStr_IP_Address.c_str(),UserFolder,UserFolder);
			system(szBuf);
		}
	}else
	{
		sprintf_s(szBuf,"net use \\\\%s\\%s delete",stdStr_IP_Address.c_str(),UserFolder);
		system(szBuf);
	}
	
	return SUCCESS;
}
USHORT CChroma_API_V2::UploadWaveform(const char *WaveformPath,bool bIsOverLoad)
{
	//system("net use \\\\192.168.206.112\\user "" /user:adivic > C:\\Users\\justin.chen\\test.txt");
	//system("copy D:\\DSPXXX_API\\Win32\\Cal100Box_MP5800\\Release\\Port_Diff.pfe \\\\192.168.206.112\\user > C:\\Users\\justin.chen\\test_11.txt");
	char szBuf[1024]={0};

	if(!bIsServerConnectFlag)
		return E_Server_NO_CONNECT;
	else
	{
		sprintf_s(szBuf,"copy %s \\\\%s\\%s",WaveformPath,stdStr_IP_Address.c_str(),UserFolder);
		system(szBuf);
		if(bIsOverLoad)
		{
			system("Yes");
		}else
		{
			system("No");
		}	
	}
	return SUCCESS;
}
USHORT CChroma_API_V2::LoadCableLoss(CString szFilePath)
{
	
	string  strFilePath="";
	strFilePath=CstringToString(szFilePath);
	bool bIsFind=false;
	int iIndex=0,iCount=0;

	fstream  Openfile; 
	Openfile.open(strFilePath,ios::in|ios::binary);
	if(!Openfile.is_open())
	{
		m_bDutCableLossEnable=false;
		sprintf_s(s_ErrMessage,"Open %s fail",strFilePath.c_str());
		return E_File_No_Exist;
	}
	std::string strline;
	float fFreq=0.0f,fTxLoss=0.0f,fRxLoss=0.0f;
	vec_DutCableLossList.clear();
	LossPoint PointLoss;
	std::getline(Openfile, strline); //Skip first line
	CStringA csReadLine=_T("");
	CStringA csData[9],Symbol=_T(",");
	iIndex=0;
	for(int i=0;i<Port_Nums;i++)
	{
		m_PortLossEnable[i]=true;
	}
	

	while (std::getline(Openfile, strline))
	{
		csReadLine=strline.c_str();

		iIndex=0;
		iCount=0;
		while(iIndex<csReadLine.GetLength())
		{
			CStringA csTok=csReadLine.Tokenize(Symbol,iIndex);
			csData[iCount++]=csTok.Trim("\t\n ");		
		}
		float fTemp;
		sscanf_s(csData[0] ,"%f",& fTemp);PointLoss.fFrequency=abs(fTemp);
		sscanf_s(csData[1] ,"%f",& PointLoss.fTxLoss[0]);
		sscanf_s(csData[2] ,"%f",& PointLoss.fTxLoss[1]);
		sscanf_s(csData[3] ,"%f",& PointLoss.fTxLoss[2]);
		sscanf_s(csData[4] ,"%f",& PointLoss.fTxLoss[3]);

		sscanf_s(csData[5] ,"%f",& PointLoss.fRxLoss[0]);
		sscanf_s(csData[6] ,"%f",& PointLoss.fRxLoss[1]);
		sscanf_s(csData[7] ,"%f",& PointLoss.fRxLoss[2]);
		sscanf_s(csData[8] ,"%f",& PointLoss.fRxLoss[3]);

		vec_DutCableLossList.push_back(PointLoss);
		for(int i=0;i<Port_Nums;i++)
		{
			 if(PointLoss.fTxLoss[i]==0 || PointLoss.fRxLoss[i]==0 )
				m_PortLossEnable[i]=false;
		}
	}
	SortLossPointTable(vec_DutCableLossList);
	Openfile.close();
	m_bDutCableLossEnable=true;

	//Check Loss Table Tx Range
	int iSize=vec_DutCableLossList.size();
	float fTx_Max,fTx_Min;
	for(int i=0;i<iSize;i++)
	{
		for(int j=0;j<Port_Nums;j++)
		{
			if(m_PortLossEnable[j])
			{
				fTx_Max=vec_DutCableLossList.at(i).fTxLoss[j];
				fTx_Min=vec_DutCableLossList.at(i).fTxLoss[j];
				break;
			}
		}

		for(int j=0;j<Port_Nums;j++)
		{
			if(m_PortLossEnable[j])
			{
				fTx_Max=max(fTx_Max,vec_DutCableLossList.at(i).fTxLoss[j]); 
				fTx_Min=min(fTx_Min,vec_DutCableLossList.at(i).fTxLoss[j]); 
			}
		}
		if( (fTx_Max-fTx_Min)>5)
		{
			sprintf_s(s_ErrMessage,"Tx Loss FREQ:%.3f Tx_Max:%.3f Tx_Min:%.3f Range:5db",vec_DutCableLossList.at(i).fFrequency,fTx_Max,fTx_Min);
			return E_Input_OutOfDefine;
		}

	}
	

	return SUCCESS;
}
USHORT CChroma_API_V2::LoadPortDifference(CString szFilePath)
{
// 	string  strFilePath="";
// 	strFilePath=CstringToString(szFilePath);
// 	bool bIsFind=false;
// 	int iIndex=0,iCount=0;
// 
// 	fstream  Openfile; 
// 	Openfile.open(strFilePath,ios::in|ios::binary);
// 	if(!Openfile.is_open())
// 	{
// 		m_bInstrPortDefEnable=false;
// 		sprintf_s(s_ErrMessage,"Open file path:%s fail.",strFilePath.c_str());
// 		return E_File_No_Exist;
// 	}
// 	std::string strline;
// 	float fFreq=0.0f,fTxLoss=0.0f,fRxLoss=0.0f;
// 	vec_InstrPortDefList.clear();
// 	LossPoint PointLoss;
// 	std::getline(Openfile, strline); //Skip first line
// 	CStringA csReadLine=_T("");
// 	CStringA csData[9],Symbol=_T(",");
// 	iIndex=0;
// 
// 	while (std::getline(Openfile, strline))
// 	{
// 		csReadLine=strline.c_str();
// 
// 		iIndex=0;
// 		iCount=0;
// 		while(iIndex<csReadLine.GetLength())
// 		{
// 			CStringA csTok=csReadLine.Tokenize(Symbol,iIndex);
// 			csData[iCount++]=csTok.Trim("\t\n ");		
// 		}
// 		float fTemp;
// 		sscanf_s(csData[0] ,"%f",& fTemp);PointLoss.fFrequency=abs(fTemp);
// 		sscanf_s(csData[1] ,"%f",& PointLoss.fTxLoss[0]);
// 		sscanf_s(csData[2] ,"%f",& PointLoss.fRxLoss[0]);
// 		sscanf_s(csData[3] ,"%f",& PointLoss.fTxLoss[1]);
// 		sscanf_s(csData[4] ,"%f",& PointLoss.fRxLoss[1]);
// 		sscanf_s(csData[5] ,"%f",& PointLoss.fTxLoss[2]);
// 		sscanf_s(csData[6] ,"%f",& PointLoss.fRxLoss[2]);
// 		sscanf_s(csData[7] ,"%f",& PointLoss.fTxLoss[3]);
// 		sscanf_s(csData[8] ,"%f",& PointLoss.fRxLoss[3]);
// 		vec_InstrPortDefList.push_back(PointLoss);		
// 	}
// 	SortLossPointTable(vec_InstrPortDefList);
// 	Openfile.close();
// 	m_bInstrPortDefEnable=true;
	return SUCCESS;
}

bool CChroma_API_V2::ConnectSever(const char * host, const char * port,unsigned int uiTimeout)
{
	bIsServerConnectFlag=false;
	stdStr_IP_Address=host;
	bIsServerConnectFlag=Adivic_MP5K.ConnectSever(host,port,uiTimeout);
	return bIsServerConnectFlag;
}
void CChroma_API_V2::CloseServer()
{
	Adivic_MP5K.CloseServer();
}

bool CChroma_API_V2::TestFunction()
{
	return true;
}
bool CChroma_API_V2::SetDistributionModeOnOff(bool bOnOff)
{
	return Adivic_MP5K.SetDistributionModeOnOff(bOnOff);
}
void CChroma_API_V2::GetDistributionModeOnOff(bool &bOnOff)
{
	Adivic_MP5K.GetDistributionModeOnOff(bOnOff);
}
bool CChroma_API_V2::SetCurrentTableName(unsigned int uiPortIndex,bool bIsTx)
{
	char szBuf[1024]={0};
	sprintf_s(szBuf,"%s_Port_%d",bIsTx?"TX":"RX",uiPortIndex);
	string strName=szBuf;
 	return Adivic_MP5K.SetCurrentTableName(&strName);
}
void CChroma_API_V2::GetCurrentSetTableName(string &strTableName)
{
	Adivic_MP5K.GetCurrentSetTableName(strTableName);
}
bool CChroma_API_V2::SetCurrentLossPoint(const float fFreqMHz,const float fLoss)
{
	return Adivic_MP5K.SetCurrentLossPoint(fFreqMHz,fLoss);
}
bool CChroma_API_V2::GetCurrentFreqLoss(const float fFreqMHz,float &fLoss)
{
	return Adivic_MP5K.GetCurrentFreqLoss(fFreqMHz,fLoss);
}
bool CChroma_API_V2::ClearCurrentAllLossData()
{
	return Adivic_MP5K.ClearCurrentAllLossData();
}
USHORT CChroma_API_V2::SetAllPortLossFile(bool bIsClear)
{
	char szBuf[1024]={0};
	string strName="";
	bool bFlag;
	if(!bIsClear)
	{
		for(int i=0;i<Port_Nums;i++)
		{
			if(m_PortLossEnable[i])
			{
				sprintf_s(szBuf,"TX_Port_%d",i+1);
				strName=szBuf;
				bFlag=Adivic_MP5K.SetPortModeLossFile(i+1,true,strName);

				sprintf_s(szBuf,"RX_Port_%d",i+1);
				strName=szBuf;
				bFlag=Adivic_MP5K.SetPortModeLossFile(i+1,false,strName);
			}
		}
	}else
	{
		for(int i=0;i<Port_Nums;i++)
		{
			strName="";
			Adivic_MP5K.SetPortModeLossFile(i+1,true,strName);
			Adivic_MP5K.SetPortModeLossFile(i+1,false,strName);
		}
	}
	
	return SUCCESS;;
}
bool CChroma_API_V2::SaveCurrentLossFileToInstr()
{
	return Adivic_MP5K.SaveLossFileToInstr("");
}
bool CChroma_API_V2::DeleteLossFileInInstr(unsigned int uiPortIndex,bool bIsTx)
{
	char szBuf[1024]={0};
	sprintf_s(szBuf,"%s_Port_%d",bIsTx?"TX":"RX",uiPortIndex);
	string strName=szBuf;
	return Adivic_MP5K.DeleteLossFileInInstr(strName);
}
USHORT CChroma_API_V2::ResetInstrCableLossSetting()
{
	bool bFlag=false;
	USHORT usState=SUCCESS;
	for(int i=1;i<Port_Nums+1;i++)
	{
		bFlag=DeleteLossFileInInstr(i,true);
		bFlag=DeleteLossFileInInstr(i,false);
	}
	
	for (int iPort=0;iPort<Port_Nums;iPort++)
	{
		if(m_PortLossEnable[iPort])
		{
			//1:Input Tx Loss
			ClearCurrentAllLossData();
			SetCurrentTableName(iPort+1,true);
			for(unsigned int iSize=0;iSize<vec_DutCableLossList.size();iSize++)
			{
				SetCurrentLossPoint(vec_DutCableLossList.at(iSize).fFrequency,vec_DutCableLossList.at(iSize).fTxLoss[iPort]);
			}
			SaveCurrentLossFileToInstr();
			//2:Input Rx Loss
			ClearCurrentAllLossData();
			SetCurrentTableName(iPort+1,false);
			for(unsigned int iSize=0;iSize<vec_DutCableLossList.size();iSize++)
			{
				SetCurrentLossPoint(vec_DutCableLossList.at(iSize).fFrequency,vec_DutCableLossList.at(iSize).fRxLoss[iPort]);
			}
			SaveCurrentLossFileToInstr();
		}
	}
	usState=SetAllPortLossFile(false);

	return usState;
}
bool CChroma_API_V2::SetCenterFrequency(const float fFreqMHz)
{
	bool bFlag=false;
	bFlag=Adivic_MP5K.SetCenterFrequency(fFreqMHz);
	return bFlag;
}
void CChroma_API_V2::GetCenterFrequency(float &fFreqMHz)
{
	Adivic_MP5K.GetCenterFrequency(fFreqMHz);
}

bool CChroma_API_V2::SetFreqSpan(const int iFreqSpanMHz)
{
	bool bFlag=false;
	bFlag=Adivic_MP5K.SetVsaSampleRate(iFreqSpanMHz);
	return bFlag;
}

void CChroma_API_V2::GetFreqSpan(int &iFreqSpanMHz)
{
	//int iSampleRate=0.0f;
	Adivic_MP5K.GetVsaSampleRate(iFreqSpanMHz);
	//iFreqSpanMHz=(int)fSampleRate;
}

bool  CChroma_API_V2::SetRxRBW(const RBW_Set uiEnum)
{
	UINT64 uiSetRBW_Hz=0;
	double dCaptureTime=0.0f;
	bool bFlag=false;
	
// 	switch(uiEnum)
// 	{
// 	case RBW_100_Hz:dCaptureTime=0.01;break;
// 	case RBW_500_Hz:dCaptureTime=0.002;break;
// 	case RBW_1K_Hz:
// 	case RBW_2K_Hz:
// 	case RBW_5K_Hz:
// 	case RBW_10K_Hz:
// 	case RBW_20K_Hz:
// 	case RBW_50K_Hz:
// 	case RBW_100K_Hz:
// 	case RBW_200K_Hz:
// 	case RBW_500K_Hz:
// 	case RBW_1M_Hz:
// 	case RBW_2M_Hz:
// 	case RBW_5M_Hz:dCaptureTime=0.002;break;
// 	}
// 	

	switch(uiEnum)
	{
	case RBW_100_Hz:uiSetRBW_Hz=100;break;
	case RBW_500_Hz:uiSetRBW_Hz=500;break;
	case RBW_1K_Hz:uiSetRBW_Hz=1000;break;	
	case RBW_2K_Hz:uiSetRBW_Hz=2000;break;
	case RBW_5K_Hz:uiSetRBW_Hz=5000;break;
	case RBW_10K_Hz:uiSetRBW_Hz=10000;break;
	case RBW_20K_Hz:uiSetRBW_Hz=20000;break;
	case RBW_50K_Hz:uiSetRBW_Hz=50000;break;
	case RBW_100K_Hz:uiSetRBW_Hz=100000;break;
	case RBW_200K_Hz:uiSetRBW_Hz=200000;break;
	case RBW_500K_Hz:uiSetRBW_Hz=500000;break;
	case RBW_1M_Hz:uiSetRBW_Hz=1000000;break;
	case RBW_2M_Hz:uiSetRBW_Hz=2000000;break;
	case RBW_5M_Hz:uiSetRBW_Hz=5000000;break;
	}
	bFlag=Adivic_MP5K.SetVsaRBW(uiSetRBW_Hz);
	if(!bFlag)return bFlag;
	dCaptureTime=(1/(double)uiSetRBW_Hz);
	return Adivic_MP5K.SetVsaCaptureTime(dCaptureTime);
}
void CChroma_API_V2::GetRxRBW(UINT64 &uiRBW_Hz)
{
	Adivic_MP5K.GetVsaRBW_Hz(uiRBW_Hz);
}


bool CChroma_API_V2::PowerReferenceAuto()
{
	return Adivic_MP5K.VsaReferenceLevelAuto();
}
bool CChroma_API_V2::SetRxReferenceLevel(const float fRefLevel)
{
	return Adivic_MP5K.SetVsaReferenceLevel(fRefLevel);
}
void CChroma_API_V2::GetRxReferenceLevel(float &fRefLevel)
{
	Adivic_MP5K.GetVsaReferenceLevel(fRefLevel);
}


void CChroma_API_V2::GetPeakData(float &fPeakFreq,float &fPeakPower)
{	
	float  fFreq=0.0f,fTxLoss=0.0f;
	//GetDutCurrentTxLoss(m_f_Center_CurrentFreq,fTxLoss);
	Adivic_MP5K.GetPeakPowerData(fPeakFreq,fPeakPower);
	fPeakPower-=fTxLoss;	
}
void CChroma_API_V2::GetGFSK_FreqDeviation(float &fFreqDevMHz)
{
	Adivic_MP5K.GetGFSK_FreqDeviation(fFreqDevMHz);

}
bool CChroma_API_V2::SetVsaRx_Port(const unsigned int uiPortIndex) // Vaule:0~3
{
	m_CurrentVsaPort=uiPortIndex; //0~3
	unsigned int PortMode[4]={Switch_OFF,Switch_OFF,Switch_OFF,Switch_OFF};
	PortMode[uiPortIndex]=Switch_RX;
	return Adivic_MP5K.Set_4_PortMode(PortMode[0],PortMode[1],PortMode[2],PortMode[3]);
}
void PortSwitch(const PORT_Mode Port_Mode,bool &bIsVsg)
{
	switch(Port_Mode)
	{
	case Switch_OFF:
	case Switch_RX:
		bIsVsg=false;
		break;
	case Switch_TX:
		bIsVsg=true;
		break;
	}
}
bool CChroma_API_V2::Set_4Port_Switch(const PORT_Mode uiPort_1,const PORT_Mode uiPort_2,const PORT_Mode uiPort_3,const PORT_Mode uiPort_4)
{	
	return Adivic_MP5K.Set_4_PortMode((UINT)uiPort_1,(UINT)uiPort_2,(UINT)uiPort_3,(UINT)uiPort_4);
}

bool CChroma_API_V2::SetTxOutputLevel(const float fOutputLevel)
{
	return Adivic_MP5K.SetVsgOutputLevel(fOutputLevel);
}
void CChroma_API_V2::GetTxOutputLevel(float &fOutputLevel)
{
	Adivic_MP5K.GetVsgOutputLevel(fOutputLevel);
}
bool CChroma_API_V2::SetCWFrequencyOffset(const float fFreqOffset)
{
	return Adivic_MP5K.Set_CW_Offset(fFreqOffset);
}
void CChroma_API_V2::GetCWFrequencyOffset(float &fOffsetMHz)
{
	Adivic_MP5K.Get_CW_Offset(fOffsetMHz);
}
bool CChroma_API_V2::SetVsgRfOnOff(const bool bRfOn)
{
	bool bResult = false;
	byteRfOnStatus = _ERROR;
	bResult = Adivic_MP5K.SetVsgRfOnOff(bRfOn);
	
	if (bResult )
	{
		byteRfOnStatus = bRfOn ? _ON : _OFF;
	}
	return bResult;
}


void CChroma_API_V2::GetVsgRfOnOff(bool &bRfOn)
{
	 Adivic_MP5K.GetVsgRfOnOff(bRfOn);
}


bool CChroma_API_V2::SetVsgModulationOnOff(const bool bModOn)
{
	bool bResult = false;
 	byteModulationStatus = _ERROR;
	bResult = Adivic_MP5K.SetVsgModulationOnOff(bModOn);
	if ( bResult )
	{
		byteModulationStatus = bModOn ? _ON : _OFF;
	}
	return bResult;
}
void CChroma_API_V2::GetVsgModulationOnOff(bool &bModOn)
{
	Adivic_MP5K.GetVsgModulationOnOff(bModOn);
}
bool CChroma_API_V2::LoadPlayFile(const char* strFileName)
{
	return Adivic_MP5K.SetVsgPlayFileName(strFileName);
}

bool CChroma_API_V2::TxPlayRepeatOnOff(const bool bOnOff)
{
	bool bFlag=false;
	if(bOnOff)
	bFlag=Adivic_MP5K.SetVsgPlayTimes(0);//Repeat
	else
	bFlag=Adivic_MP5K.SetVsgPlayTimes(1);//One Time

	return bFlag;
}

void CChroma_API_V2::GetChannelPower(float &fChannelPower)
{
	Adivic_MP5K.GetChannelPower(fChannelPower);
}
bool CChroma_API_V2::SetVsgOnOff(const bool bOnOff)
{
	bool bResult = false;
	byteRunStatus= _ERROR; // 0419 for SW inquiry RUN status
	bResult = Adivic_MP5K.SetVsgRfStartStop(bOnOff);  
	if (bResult )
	{
      byteRunStatus = bOnOff ? _ON : _OFF;
	}
	return bResult;
}
/*
void CChroma_API_V2::GetVsgOnOff(bool &bRfOn)
{
	Adivic_MP5K.GetVsgOnOff(bRfOn);
}
*/
bool CChroma_API_V2::SetVsaOnOff(const bool bOnOff)
{
	return Adivic_MP5K.SetVsaStartOnOff(bOnOff);
}
bool CChroma_API_V2::SetMark(const float fMarkFreqMHz)
{
	float fMarkOffset=fMarkFreqMHz-m_CenterFrequency;
	if(m_fMarkOffset==fMarkOffset) 
		return true;
	else
	{
		Adivic_MP5K.ClearMarker();
		return Adivic_MP5K.AddMarkerFreqOffset(fMarkOffset);
	}
	
}
bool CChroma_API_V2::ClearMark(void)
{
	 
	return	Adivic_MP5K.ClearMarker();

}
void CChroma_API_V2::GetMarkValue(float &fMarkValue)
{
	Adivic_MP5K.GetMarkerPower(fMarkValue);
}
bool CChroma_API_V2::SetDemodulationtype(USHORT usType)
{
	bool bFlag=false;
	switch(usType)
	{
	default:
	case 0:
		bFlag=false;
		break;
	case 1:
		bFlag=true;
		break;
	}
	return Adivic_MP5K.SetDemodulationtype(bFlag);
}
bool CChroma_API_V2::SetDemodulationTool(USHORT usIndex)
{
	if(m_usDemodTool_Index==usIndex)
		return true;
	else
	{
		m_usDemodTool_Index=usIndex;
		return Adivic_MP5K.SetDemodulationTool(usIndex);
	}
}
bool CChroma_API_V2::FskSetType(USHORT usType)
{
	if(m_usFSK_SetType==usType)
		return true;
	else
	{
		m_usFSK_SetType=usType;
		return Adivic_MP5K.FskSetType(usType);
	}
	
}
bool CChroma_API_V2::FskSetSymbolRate(USHORT usMHz)
{
	if(m_usFSK_SampleRateMHz==usMHz)
	{
		return true;
	}else
	{
		m_usFSK_SampleRateMHz=usMHz;
		unsigned int uiRateHz=usMHz*1000000;
		return Adivic_MP5K.FskSetSymbolRate(uiRateHz);
	}
}
bool CChroma_API_V2::FskSetFilterType(USHORT usType)
{
	if(m_usFSK_FilterType==usType)
	{
		return true;
	}else
	{
		m_usFSK_FilterType=usType;
		return Adivic_MP5K.FskSetFilterType((Filter_Mode)usType);
	}
	
}
void CChroma_API_V2::FSK_GetFreqDeviation(float &fFreqDevMHz)
{
	Adivic_MP5K.FSK_GetFreqDeviation(fFreqDevMHz);
}
bool CChroma_API_V2::FskSetFilterLength(USHORT usLength)
{
	if(m_usFSK_FilterLength==usLength)
		return true;
	else
	{
		m_usFSK_FilterLength=usLength;
		return Adivic_MP5K.FskSetFilterLength(usLength);
	}
}
bool CChroma_API_V2::FskSetcoefficient(float fCofficient)
{
	if(m_fFskcoefficient==fCofficient)
		return true;
	else
	{
		m_fFskcoefficient=fCofficient;
		return Adivic_MP5K.FskSetcoefficient(fCofficient);
	}
	
}
bool CChroma_API_V2::FskSetSymbolPhaseContinuity(bool bIsContinue)
{
	if(m_usFskSymbolPhaseContinuity==bIsContinue)
		return true;
	else
	{
		m_usFskSymbolPhaseContinuity=bIsContinue;
		return Adivic_MP5K.FskSetSymbolPhaseContinuity(bIsContinue);
	}
}

bool CChroma_API_V2::SetSpectrumOnOff(const bool bModOn)
{
	return Adivic_MP5K.SetSpectrumOnOff(bModOn);
}

void CChroma_API_V2::GetSpectrumOnOff(bool &bModOn)
{
	Adivic_MP5K.GetSpectrumOnOff(bModOn);
}

bool CChroma_API_V2::SetCableLossTableCoupleMode(const bool bModOn)
{
	return Adivic_MP5K.SetCableLossTableCoupleMode(bModOn);
}

void CChroma_API_V2::GetCableLossTableCoupleMode(bool &bModOn)
{
	Adivic_MP5K.GetCableLossTableCoupleMode(bModOn);
}


 
