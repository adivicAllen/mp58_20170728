#include <QString>
#include <QDir>
#include <QSettings>
#include <QDebug>
#include "Chroma_API_V2.h"
#include <QDir>
#include <QFileInfo>
//#include "libtest.h"


#define MAX_STRING_LEN			1014
#define  DEFAULT_ERROR 9999
//CChroma_API_V2 theAPP; //****The Only one, don't delete.**** 

CChroma_API_V2 MultiAPP[MaxInstr];

// This is the constructor of a class that has been exported.
// see Chroma_API_V2.h for the class definition
bool CChroma_API_V2::s_bIsOpenProfile=false;
QString  CChroma_API_V2::s_szConfigFilePath="";
bool CChroma_API_V2::s_bIsServer_Connected[]={false,false};
bool CChroma_API_V2::s_bIsServer_OnOff[]={false,false};
QString CChroma_API_V2::s_szServer_IP[]={"",""};
QString CChroma_API_V2::s_szServer_Port[]={"",""};
QString CChroma_API_V2::s_szServer_PortLoss[]={"",""};
//QString CChroma_API_V2::s_szServer_PortDef[]={"",""};
vector<QString> CChroma_API_V2::s_vec_WaveformPath;
char CChroma_API_V2::s_ErrMessage[];

// define MP5800 wifi control



string CstringToString(QString szInput)
{
	string szOutput="";	
    /*
    int i=szInput.length()   ;
	char *chr=new char[i+1];
	WideCharToMultiByte(CP_ACP,0,szInput.GetBuffer(),-1,chr,szInput.GetLength(),NULL,NULL);
	chr[i]='\0';
	szOutput=chr;	
	delete chr;
    */
    szOutput = szInput.toStdString();
	return szOutput;
}
CChroma_API_V2::CChroma_API_V2()
{
	m_f_Center_CurrentFreq=0.0f;
	m_fMarkOffset=-9999;
//	ZeroMemory(m_PortLossEnable,sizeof(m_PortLossEnable) );  // allen
    memset (m_PortLossEnable,0,sizeof(m_PortLossEnable));
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
	measureMode = MEASURE_MODE_NULL;
	return;
}

CChroma_API_V2::~CChroma_API_V2()
{

}

void CChroma_API_V2::GetDutCurrentTxLoss(double fFreq,double& fTxLoss)
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

bool CChroma_API_V2::GetCurrentFreqLoss(const double fFreq,const unsigned int uiPortIndex,double& fTxLoss,double& fRxLoss)
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
bool CChroma_API_V2::SetFreqPointTxOutputLevelAndPortAtt(const double fOutputLevel)
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
// 	double fValue[4];
// 	for(int i=0;i<Port_Nums;i++)
// 	{
// 		fValue[i]=TxCableLoss.fRxLoss[i]+PortDef.fTxLoss[i];
// 	}
// 	double fMin=fValue[0];
// 	for(int i=1;i<4;i++)
// 	{
// 		fMin=min(fMin,fValue[i]);
// 	}
// 
// 	int iOffset=(int)fMin;
// 	double fSetOutPutLevel=fOutputLevel-(double)iOffset;
// 	SetTxOutputLevel(fSetOutPutLevel);
// 	
// 	for(int i=0;i<4;i++)
// 	{
// 		unsigned int uiTemp=0;
// 		uiTemp=8+(unsigned int)( ( (fValue[i]-(double)iOffset)/0.25));
// 		if(uiTemp>63)
// 		m_uiPortAtt[i]=63;
// 		else
// 		m_uiPortAtt[i]=uiTemp;	
// 	}
// 	Set_4Port_OutputATT(m_uiPortAtt[0],m_uiPortAtt[1],m_uiPortAtt[2],m_uiPortAtt[3]);
	return true;
}
bool CChroma_API_V2::FindFreqLoss(double fFreq,const vector<LossPoint>& LossPointTable,LossPoint& loss_point)
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
				double fTemp=0.0f;
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
    /*
	char szCurrentDir[255];
	//WIN32_FIND_DATAA FindFileData;
    //HANDLE	hFind=INVALID_HANDLE_VALUE;
    HANDLE	hFind =  NULL;
	string strFileDir="",strFileName="";
    QString csFileName="";
	bool bIsFind=false;
	int iIndex=0,iCount=0;

	std::memset(szCurrentDir,0,sizeof(szCurrentDir));
	GetCurrentDirectoryA(sizeof(szCurrentDir),(LPSTR)szCurrentDir);
	strFileDir=szCurrentDir;
	strFileDir=strFileDir+"\\*";
	memcpy(chFileDir,strFileDir.c_str(),strFileDir.size());
    */
    QString temp;
    temp = QDir::currentPath() + "/*";
    chFileDir = temp.toLatin1().data();

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
	bool failure;
 
	if(  !SetCableLossTableCoupleMode(false)  ) 
	{
		return FAIL;
    }

	Adivic_MP5K.SetMeasureMode(_MOD_GPRF);

	if (!SetCenterFrequency(2440))
	{
			return FAIL;
	};
	m_CenterFrequency=2440;
	SetFreqSpan(160); //default GPRF sampling rate 160MHz
	m_iFreqSpanMHz=160;
	SetRxRBW(RBW_1K_Hz); //1KHz
	m_uiRBW_Enum=0;
	SetRxReferenceLevel(-10);
	m_fRefLevel=0;
	SetVsaRx_Port(RF_PORT_1);//Port1
	m_uiVsa_Port=RF_PORT_1;
	//Set_4Port_Switch(PORT_MODE_OFF,PORT_MODE_OFF,PORT_MODE_OFF,PORT_MODE_OFF);//All OFF
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
    QString csFileName="";
	bool bIsFind=false;
	int iIndex=0,iCount=0;
	USHORT usState=SUCCESS;
	for(int i=0;i<MaxInstr;i++)
	{
		MultiAPP[i].usInstrIndex=i;
	}
	
	strFilePath=IniFilePath;

    std:: fstream  Openfile;

    Openfile.open(strFilePath.c_str(),std::fstream::in| std::fstream::binary);

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

    QString szSectionName=("");
    QString szFieldName=("");
    QString szReadString=("");
    QString settingKey;
    QSettings settings(s_szConfigFilePath , QSettings::IniFormat);
    szSectionName=("Server Config Setting");
    szFieldName=("Server_1_IP");
    settingKey = QString("%1/%2").arg(szSectionName).arg(szFieldName);
    s_szServer_IP[0]= settings.value(settingKey).toString();
    qDebug()<<"s_szServer_IP[0] =" << s_szServer_IP[0] << endl;

    szFieldName=("Server_1_Port");
    settingKey = QString("%1/%2").arg(szSectionName).arg(szFieldName);
    s_szServer_Port[0]=settings.value(settingKey).toString();
    qDebug()<<"s_szServer_Port[0] =" << s_szServer_Port[0] << endl;

	szFieldName=("Server_2_IP");
    settingKey = QString("%1/%2").arg(szSectionName).arg(szFieldName);
    s_szServer_IP[1]=settings.value(settingKey).toString();
     qDebug()<<"s_szServer_IP[1] =" << s_szServer_IP[1] << endl;

    szFieldName=("Server_2_Port");
    settingKey = QString("%1/%2").arg(szSectionName).arg(szFieldName);
    s_szServer_Port[1] = settings.value(settingKey).toString();
    qDebug()<<"s_szServer_Port[1] =" << s_szServer_Port[1] << endl;

    szFieldName=("IsServer_1_Open");
    settingKey = QString("%1/%2").arg(szSectionName).arg(szFieldName);
    szReadString = settings.value(settingKey).toString();
    qDebug()<<"s_szServer_OnOff[0] =" << szReadString << endl;

	if(szReadString=="On")
		s_bIsServer_OnOff[0]=true;
	else
		s_bIsServer_OnOff[0]=false;

    szFieldName=("IsServer_2_Open");
    settingKey = QString("%1/%2").arg(szSectionName).arg(szFieldName);
    szReadString = settings.value(settingKey).toString();
	if(szReadString=="On")
		s_bIsServer_OnOff[1]=true;
	else
		s_bIsServer_OnOff[1]=false;
     qDebug()<<"s_szServer_OnOff[1] =" << szReadString << endl;
	//-------------------------------------------------------------------------------
    szSectionName=("Server File Path");
    szFieldName=("Server_1_PortLossPath");
    settingKey = QString("%1/%2").arg(szSectionName).arg(szFieldName);
    s_szServer_PortLoss[0]=settings.value(settingKey).toString();
     qDebug()<<"s_szServer_PortLoss[0] =" << s_szServer_PortLoss[0] << endl;
    szFieldName=("Server_2_PortLossPath");
    settingKey = QString("%1/%2").arg(szSectionName).arg(szFieldName);
    s_szServer_PortLoss[1]=settings.value(settingKey).toString();
      qDebug()<<"s_szServer_PortLoss[1] =" << s_szServer_PortLoss[1] << endl;
	//-------------------------------------------------------
    szSectionName=("Waveform List");
	s_vec_WaveformPath.clear();
	USHORT usIndex=1;

	do 
	{
        szFieldName.sprintf(("Waveform_Name_%d"),usIndex++);
        settingKey = QString("%1/%2").arg(szSectionName).arg(szFieldName);
         qDebug()<< "waveform settingKey = " << settingKey << endl;
        szReadString = settings.value(settingKey).toString();
        qDebug()<<szFieldName << "= " <<  szReadString << endl;
        if(szReadString.length()!=0)
 			s_vec_WaveformPath.push_back(szReadString);
    } while (szReadString.length()!=0);

	
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
		{   // net use \\\\169.254.54.170\\USER user:adivic
			// net use \\169.254.54.170\USER user:adivic
            //sprintf_s(szBuf,"net use \\\\%s\\%s \"\" /%s:adivic",stdStr_IP_Address.c_str(),UserFolder,UserFolder);
            // smbclient -N //192.168.101.50/USER
         //   sprintf_s(szBuf,"smbclient -N //%s/USER",stdStr_IP_Address.c_str());
        //	system(szBuf);
		}
	}else
	{
        /*  allen
		sprintf_s(szBuf,"net use \\\\%s\\%s delete",stdStr_IP_Address.c_str(),UserFolder);
		system(szBuf);
        */
	}
	
	return SUCCESS;
}
USHORT CChroma_API_V2::UploadWaveform(const char *WaveformPath,bool bIsOverLoad)
{   // copy filename \\169.254.54.170
	//system("net use \\\\192.168.206.112\\user "" /user:adivic > C:\\Users\\justin.chen\\test.txt");
	//system("copy D:\\DSPXXX_API\\Win32\\Cal100Box_MP5800\\Release\\Port_Diff.pfe \\\\192.168.206.112\\user > C:\\Users\\justin.chen\\test_11.txt");
	char szBuf[1024]={0};

	if(!bIsServerConnectFlag)
		return E_Server_NO_CONNECT;
	else
	{
        QString tPath ;
        QFileInfo  f(static_cast<QString>(WaveformPath));
        tPath = f.absolutePath();

        QString tName;
        tName = f.fileName();

        
    //	sprintf_s(szBuf,"copy %s \\\\%s\\%s",WaveformPath,stdStr_IP_Address.c_str(),UserFolder);
        sprintf_s(szBuf,"smbclient -N //%s/USER -c 'lcd %s; put %s'",stdStr_IP_Address.c_str(), tPath.toStdString().c_str(), tName.toStdString().c_str());
        system(szBuf);
     //   system("smbclient -N //192.168.101.50/USER - c 'lcd %s; put BER_gfsk_120M_192ms_no_header_u1.wfm'" );

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
USHORT CChroma_API_V2::LoadCableLoss(QString szFilePath)
{
	
	string  strFilePath="";
	strFilePath=CstringToString(szFilePath);
	bool bIsFind=false;
	int iIndex=0,iCount=0;

	fstream  Openfile; 
    Openfile.open(strFilePath.c_str(),ios::in|ios::binary);
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


    QString csReadLine=("");
    QString csData[9],Symbol=(",");
	iIndex=0;
	for(int i=0;i<Port_Nums;i++)
	{
		m_PortLossEnable[i]=true;
	}
	

	while (std::getline(Openfile, strline))
	{
		csReadLine=strline.c_str();
        csReadLine.remove(QRegExp("\\r|\\t|\\n"));
        QStringList csTok = csReadLine.split(QRegExp(","));

        float fTemp;
        fTemp = csTok.at(0).simplified().toFloat();
        PointLoss.fFrequency=abs(fTemp);
        for (int i=0; i<4;i++)
        {
            PointLoss.fTxLoss[i] = csTok.at(i+1).simplified().toFloat();
            PointLoss.fRxLoss[i] = csTok.at(i+5).simplified().toFloat();
        }

       
        /*
        sscanf_s(csData[0] ,"%f",& fTemp);PointLoss.fFrequency=abs(fTemp);
        sscanf_s(csData[1] ,"%f",& PointLoss.fTxLoss[0]);
		sscanf_s(csData[2] ,"%f",& PointLoss.fTxLoss[1]);
		sscanf_s(csData[3] ,"%f",& PointLoss.fTxLoss[2]);
		sscanf_s(csData[4] ,"%f",& PointLoss.fTxLoss[3]);

		sscanf_s(csData[5] ,"%f",& PointLoss.fRxLoss[0]);
		sscanf_s(csData[6] ,"%f",& PointLoss.fRxLoss[1]);
		sscanf_s(csData[7] ,"%f",& PointLoss.fRxLoss[2]);
		sscanf_s(csData[8] ,"%f",& PointLoss.fRxLoss[3]);
        */
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
USHORT CChroma_API_V2::LoadPortDifference(QString szFilePath)
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
// 	double fFreq=0.0f,fTxLoss=0.0f,fRxLoss=0.0f;
// 	vec_InstrPortDefList.clear();
// 	LossPoint PointLoss;
// 	std::getline(Openfile, strline); //Skip first line
// 	QStringA csReadLine=_T("");
// 	QStringA csData[9],Symbol=_T(",");
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
// 			QStringA csTok=csReadLine.Tokenize(Symbol,iIndex);
// 			csData[iCount++]=csTok.Trim("\t\n ");		
// 		}
// 		double fTemp;
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
	if (!bIsServerConnectFlag)  // allen
	{      	   
	       return false;   
	}
	bIsServerConnectFlag =   Adivic_MP5KWifiBT.LoadDll(); 
    if (!bIsServerConnectFlag)  // allen
	{      	   
	       return false;   
	}
	ErrorCode = Adivic_MP5KWifiBT.Init(); // Initial MP5K Tester
	if (ErrorCode)
	{
		return false;
	}
	ErrorCode =   Adivic_MP5KWifiBT.Add( host ,handle);
	if (ErrorCode)
	{
		return false;
	}	
	return true;
}
void CChroma_API_V2::CloseServer()
{
	Adivic_MP5K.CloseServer();
	Adivic_MP5KWifiBT.Delete( handle );
    Adivic_MP5KWifiBT.Close(); 
	Adivic_MP5KWifiBT.FreeDll();
	//delete (Adivic_MP5KWifiBT);
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
bool CChroma_API_V2::SetCurrentLossPoint(const double fFreqMHz,const double fLoss)
{
	return Adivic_MP5K.SetCurrentLossPoint(fFreqMHz,fLoss);
}
bool CChroma_API_V2::GetCurrentFreqLoss(const double fFreqMHz,double &fLoss)
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
bool CChroma_API_V2::SetCenterFrequency(const double fFreqMHz)
{
	bool bFlag=false;
	bFlag=Adivic_MP5K.SetCenterFrequency(fFreqMHz);
	return bFlag;
}
void CChroma_API_V2::GetCenterFrequency(double &fFreqMHz)
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

bool  CChroma_API_V2::SetVsaTriggerType(const MP5800_VSA_Trigger_Type uiEnum)
{
	 
	double dCaptureTime=0.0f;
	bool bFlag=false;
	
	 
	return Adivic_MP5K.SetVsaTriggerType(uiEnum);
	 
	//return Adivic_MP5K.SetVsaCaptureTime(dCaptureTime);
}

bool  CChroma_API_V2::SetRxRBW(const RBW uiEnum)
{
	UINT64 uiSetRBW_Hz=0;
	double dCaptureTime_s=0.0f;
	double dVsaCaptureTimeSetting_ms = 0.0f;
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
	dCaptureTime_s=(1/(double)uiSetRBW_Hz);
	// allen capture is the minimun of 1/RBW
	Adivic_MP5K.GetVsaCaptureTime( dVsaCaptureTimeSetting_ms);
	if (      dVsaCaptureTimeSetting_ms >= (dCaptureTime_s *1000))
	{
		return true;
	}
	else
	{
	   return Adivic_MP5K.SetVsaCaptureTime(dCaptureTime_s);
	}
}
void CChroma_API_V2::GetRxRBW(UINT64 &uiRBW_Hz)
{
	Adivic_MP5K.GetVsaRBW_Hz(uiRBW_Hz);
}


bool CChroma_API_V2::PowerReferenceAuto()
{
	return Adivic_MP5K.VsaReferenceLevelAuto();
}
bool CChroma_API_V2::SetRxReferenceLevel(const double fRefLevel)
{
	return Adivic_MP5K.SetVsaReferenceLevel(fRefLevel);
}

bool CChroma_API_V2::SetVsaCaptureLength(const double dCaptureLength_ms)
{
	return Adivic_MP5K.SetVsaCaptureLength(dCaptureLength_ms);
}
 
bool CChroma_API_V2::SetVsaTriggerTimeOut(const double dTriggerTimeOut_ms)
{
	return Adivic_MP5K.SetVsaTriggerTimeOut(dTriggerTimeOut_ms);
}
 
bool CChroma_API_V2::SetVsaGapTime(const double dCapTime_us)
{
	return Adivic_MP5K.SetVsaGapTime(dCapTime_us);
}
void CChroma_API_V2::GetRxReferenceLevel(double &fRefLevel)
{
	Adivic_MP5K.GetVsaReferenceLevel(fRefLevel);
}


void CChroma_API_V2::GetPeakData(double &fPeakFreq,double &fPeakPower)
{	
	double  fFreq=0.0f,fTxLoss=0.0f;
	//GetDutCurrentTxLoss(m_f_Center_CurrentFreq,fTxLoss);
	Adivic_MP5K.GetPeakPowerData(fPeakFreq,fPeakPower);
	fPeakPower-=fTxLoss;	
}
void CChroma_API_V2::GetGFSK_FreqDeviation(double &fFreqDevMHz)
{
	Adivic_MP5K.GetGFSK_FreqDeviation(fFreqDevMHz);

}
 
void CChroma_API_V2::MeasureGFSK(double &dFreqDevMHz,double &dFreqOffset,double &dFreqDrift,double &dDevError,double &dFskError)
{
	Adivic_MP5K.MeasureGFSK(dFreqDevMHz,dFreqOffset,dFreqDrift,dDevError,dFskError);
}


bool CChroma_API_V2::SetVsaRx_Port(const unsigned int uiPortIndex) // Vaule:0~3
{
	m_CurrentVsaPort=uiPortIndex; //0~3
	unsigned int PortMode[4]={PORT_MODE_OFF,PORT_MODE_OFF,PORT_MODE_OFF,PORT_MODE_OFF};
	PortMode[uiPortIndex]=PORT_MODE_RX;
	return Adivic_MP5K.Set_4_PortMode(PortMode[0],PortMode[1],PortMode[2],PortMode[3]);
}
void PortSwitch(const PORT_MODE PORT_MODE,bool &bIsVsg)
{
	switch(PORT_MODE)
	{
	case PORT_MODE_OFF:
	case PORT_MODE_RX:
		bIsVsg=false;
		break;
	case PORT_MODE_TX:
		bIsVsg=true;
		break;
	}
}
bool CChroma_API_V2::Set_4Port_Switch(const PORT_MODE uiPORT_1,const PORT_MODE uiPort_2,const PORT_MODE uiPort_3,const PORT_MODE uiPort_4)
{	
	return Adivic_MP5K.Set_4_PortMode((UINT)uiPORT_1,(UINT)uiPort_2,(UINT)uiPort_3,(UINT)uiPort_4);
}

bool CChroma_API_V2::SetTxOutputLevel(const double fOutputLevel)
{
	return Adivic_MP5K.SetVsgOutputLevel(fOutputLevel);
}
void CChroma_API_V2::GetTxOutputLevel(double &fOutputLevel)
{
	Adivic_MP5K.GetVsgOutputLevel(fOutputLevel);
}
bool CChroma_API_V2::SetCWFrequencyOffset(const double fFreqOffset)
{
	return Adivic_MP5K.Set_CW_Offset(fFreqOffset);
}
void CChroma_API_V2::GetCWFrequencyOffset(double &fOffsetMHz)
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

bool CChroma_API_V2::LoadPlayFileWithFilePath(const char* strFileName, const char* filePathName)
{
	return Adivic_MP5K.SetVsgPlayFileNameWithFilePath(strFileName, filePathName);
}


bool CChroma_API_V2::SetVsgPlayCount(const int count)
{
	bool bFlag=false;
	 
	bFlag=Adivic_MP5K.SetVsgPlayTimes(count);//One Time

	return bFlag;
}

bool CChroma_API_V2::SetVsgPlayGapUs(const UINT GapUs)
{
	bool bFlag=false;
	 
	bFlag=Adivic_MP5K.SetVsgPlayGapUs(GapUs);//One Time

	return bFlag;
}

bool CChroma_API_V2::SetVsaTriggerOffsetUs(const int OffsetUs)
{
	bool bFlag=false;
	 
	bFlag=Adivic_MP5K.SetVsaTriggerOffsetUs(OffsetUs);//One Time

	return bFlag;
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

void CChroma_API_V2::GetChannelPower(double &fChannelPower)
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
bool CChroma_API_V2::SetMark(const double fMarkFreqMHz)
{
	double fMarkOffset=fMarkFreqMHz-m_CenterFrequency;
	if(m_fMarkOffset==fMarkOffset) 
		return true;
	else
	{
		Adivic_MP5K.ClearMarker();
		return Adivic_MP5K.AddMarkerFreqOffset(fMarkOffset);
	}
	
}

void CChroma_API_V2::GetCWChannelPower(const double fMarkFreqMHz, const double leftSpan_Hz, const double rightSpan_Hz, 
	                 double &PowerMeasured, double &PowerMeasured_Left, double &PowerMeasured_Right, double &Power_Comp, double &Frequency_Offset )
{
 	 
		return Adivic_MP5K.GetCWChannelPower(fMarkFreqMHz,leftSpan_Hz,rightSpan_Hz,PowerMeasured,PowerMeasured_Left,PowerMeasured_Right,Power_Comp,Frequency_Offset);
}

 bool CChroma_API_V2::SetDigitalDownConvertor(const bool bOnOff ,  const double offSet_Hz )
 {
	 return	Adivic_MP5K.SetDigitalDownConvertor(  bOnOff, offSet_Hz );
 }

void CChroma_API_V2::GetChannelPower2Span(const double fMarkFreqMHz, const double leftSpan_Hz, const double rightSpan_Hz, 
	double &PowerMeasured, double &PowerMeasured_Left, double &PowerMeasured_Right )
{
	return Adivic_MP5K.GetChannelPower2Span(fMarkFreqMHz,leftSpan_Hz,rightSpan_Hz,PowerMeasured,PowerMeasured_Left,PowerMeasured_Right);
}

	

bool CChroma_API_V2::ClearMark(void)
{
	 
	return	Adivic_MP5K.ClearMarker();

}
void CChroma_API_V2::GetMarkValue(double &fMarkValue)
{
	Adivic_MP5K.GetMarkerPower(fMarkValue);
}

void CChroma_API_V2::GetPowerByTime(double &dPowerByTime)
{
	Adivic_MP5K.GetPowerByTime(dPowerByTime);
}

void CChroma_API_V2::GetPeakPowerFrequencyHzByTime(double &dPeakPowerFrequencyHzByTime)
{
	Adivic_MP5K.GetPeakPowerFrequencyHzByTime(dPeakPowerFrequencyHzByTime);
}
/*
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
*/
bool CChroma_API_V2::SetDemodulationtype(USHORT usType)
{
	
	return Adivic_MP5K.SetDemodulationtype(usType);
}

bool CChroma_API_V2::SetDemodulationTool(USHORT usSloution)
{
	 
		return Adivic_MP5K.SetDemodulationTool(usSloution);
	 
}
/*
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
*/
bool CChroma_API_V2::FskSetType(USHORT usFskType)
{
		 
		return Adivic_MP5K.FskSetType(usFskType);
	
}
/*
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
*/

bool CChroma_API_V2::FSKSetFreqDeviation(double dFreqDev_kHz)
{
	 	 
		unsigned int uiFreqDev_Hz=dFreqDev_kHz*1000;
		return Adivic_MP5K.FSK_SetFreqDeviation(uiFreqDev_Hz);
}

bool CChroma_API_V2::FskSetSymbolRate(double dSymbolRateMHz)
{
	 	 
		unsigned int uiRateHz=dSymbolRateMHz*1000000;
		return Adivic_MP5K.FskSetSymbolRate(uiRateHz);
}

bool CChroma_API_V2::FskSetFilterType(USHORT usFilterType)
{
	 
		return Adivic_MP5K.FskSetFilterType((Filter_Mode)usFilterType);
	 
	
}
void CChroma_API_V2::FSK_GetFreqDeviation(double &fFreqDevMHz)
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
bool CChroma_API_V2::FskSetcoefficient(double fCofficient)
{
	if(m_fFskcoefficient==fCofficient)
		return true;
	else
	{
		m_fFskcoefficient=fCofficient;
		return Adivic_MP5K.FskSetcoefficient(fCofficient);
	}
	
}

/*bool CChroma_API_V2::FskSetSymbolPhaseContinuity(bool bIsContinue)
{
	if(m_usFskSymbolPhaseContinuity==bIsContinue)
		return true;
	else
	{
		m_usFskSymbolPhaseContinuity=bIsContinue;
		return Adivic_MP5K.FskSetSymbolPhaseContinuity(bIsContinue);
	}
}
*/
bool CChroma_API_V2::FskSetSymbolPhaseContinuity(USHORT usCountinuity)
{
		 
		return Adivic_MP5K.FskSetSymbolPhaseContinuity(usCountinuity);

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


 
bool CChroma_API_V2::SetMeasureMode(const Mea_Mode Mode)
{
	return Adivic_MP5K.SetMeasureMode(Mode);
}


void CChroma_API_V2::GetMeasureMode(Mea_Mode &Mode)
{
	 Adivic_MP5K.GetMeasureMode(Mode);
}

bool CChroma_API_V2::SetFilterWindow(const UINT filterType)
{
	 
	return Adivic_MP5K.SetFilterWindow(filterType);
}
// ------------------------------------------------------------------------------------------------------------------------------------
UINT CChroma_API_V2::Set_WIFI_RX( const unsigned int uiPortIndex ,                      \
	                               UINT                           frequency_MHz ,       \
					               double                    referenceLevel_dBm  ,      \
					               double                      captureLength_ms  ,      \
	                               WIFI_STANDARD                wifiStandard   ,        \
					               WIFI_BANDWIDTH       wifiBandwidth  ,       \
					               WIFI_DATA_RATE                 wifiDataRate  ,       \
					               bool                             isBeamforming )
	{
	
		MP5K_TxTestConfig tx_cfg;
		tx_cfg.FreqMHz = frequency_MHz;
		tx_cfg.Standard.type = (TYPE) wifiStandard;
		 
		tx_cfg.Standard.OFDM.Modulation.Bandwidth = (BANDWIDTH)wifiBandwidth;
		tx_cfg.Standard.OFDM.Modulation.rate = (RATE)wifiDataRate;
		tx_cfg.RefLeveldBm =  referenceLevel_dBm;                           // Reference Level unit:dBm
		tx_cfg.UseRFPort = uiPortIndex;                             // MP5K Test RF Port
		tx_cfg.CaptureLength = captureLength_ms;                     // unit:ms
		tx_cfg.isBeamforming =	 isBeamforming;
		ErrorCode = Adivic_MP5KWifiBT.TXSetting( handle , tx_cfg);

		 
		return ErrorCode;
	}

// ------------------------------------------------------------------------------------------------------------------------------------
	UINT CChroma_API_V2::Set_WIFI_BT_RX_Advance(               WIFI_BT_TRIGGER_TYPE               wifiBT_TriggerType            , \
	                                                        double                             wifiBT_TriggerGapTime_ms                   , \
														    double                             wifiBT_TriggerThreshold                    , \
														    double                             wifiBT_TriggerOffset                  ,  \
														    WIFI_FREQUENCY_CORRECTION         wifiFrequencyCorrection              , \
														    WIFI_CHANNEL_ESTIMATION           wifiChannelEstimation    )
	{

	  	MP5K_TriggerConfig trigger_cfg;
		trigger_cfg.TriggerType = (TRIGGER_TYPE)wifiBT_TriggerType;
		trigger_cfg.TriggerGapTime = wifiBT_TriggerGapTime_ms;
		trigger_cfg.TriggerThreshold = wifiBT_TriggerThreshold;
		trigger_cfg.TriggerOffset = wifiBT_TriggerOffset;
		 
		ErrorCode = Adivic_MP5KWifiBT.TriggerSetting( handle , trigger_cfg );
	    Mea_Mode measureMode;
		GetMeasureMode(measureMode);
		if ( measureMode == _MOD_OFDM)
		{
			MP5K_CorrectionConfig corr_cfg;
			corr_cfg.FrequencyCorrection = (MP5K_Frequency_Correction)wifiFrequencyCorrection;
			corr_cfg.ChannelEstimation = (MP5K_Channel_Estimation)wifiChannelEstimation;
		 
			ErrorCode = Adivic_MP5KWifiBT.CorrectionSetting( handle , corr_cfg);
			}
		return ErrorCode;
	}
// ------------------------------------------------------------------------------------------------------------------------------------
/*	UINT CChroma_API_V2::Get_WIFI_RX(   double         &TXPowerdBm,               \
										double         &EVM,                      \
										bool           &MASK,                     \

										double           &PhaseError,              \
										double           &FrequencyError,          \
										double           &SymbolClockError,        \
										double           &LoLeakage,	           \
										double			&MaskTotalCount,           \
										double			&MaskErrorCount,           \
	
										bool             &isBFON,                  \
										double           &PreamblePower            )
*/
UINT CChroma_API_V2::Get_WIFI_RX(   double         &TXPowerdBm,               \
										double         &EVM,                      \
										bool           &MASK,                     \

										double           &PhaseError,              \
										double           &FrequencyError,          \
										double           &SymbolClockError,        \
										double           &LoLeakage )

{

		MP5K_TxTestMeas tx_meas;
		 
		ErrorCode = Adivic_MP5KWifiBT.TXMeas( handle , tx_meas); // Return tx test item
		TXPowerdBm    = tx_meas.TXPowerdBm;
		EVM           = tx_meas.EVM; 
		MASK		  = tx_meas.MASK;
		PhaseError    = tx_meas.PhaseError;						        
		FrequencyError=	tx_meas.FrequencyError;							 
		SymbolClockError= tx_meas.SymbolClockError;								 
		LoLeakage =	tx_meas.LoLeakage;							 
		//MaskTotalCount = tx_meas.MaskTotalCount;								 
		//MaskErrorCount = tx_meas.MaskErrorCount;								 
	    //isBFON = tx_meas.isBFON;
		//PreamblePower =	tx_meas.PreamblePower;		
		return ErrorCode;
	}

	// ------------------------------------------------------------------------------------------------------------------------------------

	UINT CChroma_API_V2::Set_WIFI_TX(    const unsigned int uiPortIndex ,                   \
			                             const UINT                         frequency_MHz ,            \
										  const double                         outPower_dBm ,      \
										 const UINT                           packetCount ,            \
										 const WIFI_STANDARD                wifiStandard,               \
										 const WIFI_BANDWIDTH       wifiBandwidth,             \
										  const WIFI_DATA_RATE                 wifiDataRate			 )
	{
		MP5K_RxTestConfig rx_cfg;
		rx_cfg.FreqMHz = frequency_MHz;       // Frequency unit:MHz
		rx_cfg.PacketCount = packetCount;   // Packet count
		rx_cfg.Standard.type = (TYPE) wifiStandard;
		if (rx_cfg.Standard.type == _802_11B)
		{
	    
		rx_cfg.Standard.DSSS.rate = (RATE) wifiDataRate;
		}
		else
		{
		rx_cfg.Standard.OFDM.Modulation.Bandwidth = (BANDWIDTH) wifiBandwidth;
		rx_cfg.Standard.OFDM.Modulation.rate = (RATE) wifiDataRate;
		}

		rx_cfg.UseRFPort = uiPortIndex;
		rx_cfg.OutPowerdBm = outPower_dBm; // Output Level unit:dBm
		ErrorCode = Adivic_MP5KWifiBT.RXSetting( handle , rx_cfg );
		//tester.RXStart( handle ); // RX Starting
		return ErrorCode;
	}

	// ------------------------------------------------------------------------------------------------------------------------------------


	UINT CChroma_API_V2::Set_WIFI_TX_Start(void)
	{
		return Adivic_MP5KWifiBT.RXStart(handle);
	}	 

	//--------------------------------------------------------------------------------------------------------------------------------------

	UINT CChroma_API_V2::Set_BT_RX( const unsigned int uiPortIndex ,          \
		                            UINT              frequency_MHz ,         \
					                double             referenceLevel_dBm  ,  \
					                double             captureLength_ms ,    \
	                                BT_PACKET_TYPE      BT_PackageType          )
	{
	 
		MP5K_BT_TxTestConfig	Bt_TxCfg;
		UINT ErrorCode = DEFAULT_ERROR; 
		Bt_TxCfg.UseRFPort			= uiPortIndex;
	//	Bt_TxCfg.Type				= (PACKET_TYPE) BT_PackageType;
		Bt_TxCfg.FreqMHz			= frequency_MHz;
		Bt_TxCfg.RefLeveldBm		= referenceLevel_dBm;
		Bt_TxCfg.CaptureLength		= captureLength_ms;

		switch ( (int) BT_PackageType)
		{
		case 0 : Bt_TxCfg.Type = (PACKET_TYPE) 1 ; break;
		case 1 : Bt_TxCfg.Type = (PACKET_TYPE) 3 ; break;
		case 2 : Bt_TxCfg.Type = (PACKET_TYPE) 5 ; break;
		case 3 : Bt_TxCfg.Type = (PACKET_TYPE) 6 ; break;
		case 4 : Bt_TxCfg.Type = (PACKET_TYPE) 7  ; break;
		case 5: Bt_TxCfg.Type = (PACKET_TYPE) 8  ; break;
		case 6 : Bt_TxCfg.Type = (PACKET_TYPE) 9  ; break;
		case 7 : Bt_TxCfg.Type = (PACKET_TYPE) 10 ; break;
		case 8 : Bt_TxCfg.Type = (PACKET_TYPE) 11 ; break;
		case 9   : Bt_TxCfg.Type = (PACKET_TYPE) 12; 
        }

		ErrorCode = Adivic_MP5KWifiBT.BT_Tx_Setting( handle , Bt_TxCfg );
		Sleep(200);
		 
		return ErrorCode;
	}


	 UINT CChroma_API_V2::Get_BT_RX(
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
		 
		 MP5K_BT_TxTestMeas bt_tx_meas;
		ErrorCode = Adivic_MP5KWifiBT.BT_Tx_Meas( handle , bt_tx_meas); // Return tx test item
		 
		targetPower = bt_tx_meas.BT_TargetPower;
		initialFrequencyError = bt_tx_meas.BT_InitFreq_Error;
		BDR_BW_20db = bt_tx_meas.BT_TX_ReturnValue.BT_BDR.BW_20db;
		BDR_FreqDrift = bt_tx_meas.BT_TX_ReturnValue.BT_BDR.FreqDrift;
		BDR_DeltaF1Avg = bt_tx_meas.BT_TX_ReturnValue.BT_BDR.DeltaF1Avg;
		BDR_DeltaF2Avg = bt_tx_meas.BT_TX_ReturnValue.BT_BDR.DeltaF2Avg;
		BDR_DeltaF2Max = bt_tx_meas.BT_TX_ReturnValue.BT_BDR.DeltaF2Max;

		EDR_DEVM = bt_tx_meas.BT_TX_ReturnValue.BT_EDR.BT_DEVM;
		EDR_DEVMPeak = bt_tx_meas.BT_TX_ReturnValue.BT_EDR.BT_DEVMPeak;

		LE_DeltaF1Avg = bt_tx_meas.BT_TX_ReturnValue.BT_LE.DeltaF1Avg;
		LE_DeltaF2Avg = bt_tx_meas.BT_TX_ReturnValue.BT_LE.DeltaF2Avg;
		LE_DeltaF2Max = bt_tx_meas.BT_TX_ReturnValue.BT_LE.DeltaF2Max;
		 
		 return ErrorCode;
	 }



 UINT CChroma_API_V2::Set_BT_TX ( const unsigned int uiPortIndex ,                         \
		            const UINT                    frequency_MHz ,          \
					const double                  outPower_dBm   ,         \
					const double                  gapTime   ,              \
					const  UINT			        bitCount ,                  \
					const BT_PACKET_TYPE           btPacketType   )
{
	     MP5K_BT_RxTestConfig MP5K_Cfg;
		  MP5K_Cfg.UseRFPort		= uiPortIndex;
		 MP5K_Cfg.BitCount		= bitCount;
		// MP5K_Cfg.Type			= (PACKET_TYPE)btPacketType;
		 MP5K_Cfg.FreqMHz  		= frequency_MHz;
		 MP5K_Cfg.OutPowerdBm	= outPower_dBm;
		 MP5K_Cfg.GapTime		= gapTime;
	 
		switch ( (int) btPacketType)
		{
		case 0 :MP5K_Cfg.Type = (PACKET_TYPE) 1 ; break;
		case 1 :MP5K_Cfg.Type = (PACKET_TYPE) 3 ; break;
		case 2 :MP5K_Cfg.Type = (PACKET_TYPE) 5 ; break;
		case 3 :MP5K_Cfg.Type = (PACKET_TYPE) 6 ; break;
		case 4 :MP5K_Cfg.Type = (PACKET_TYPE) 7  ; break;
		case 5:MP5K_Cfg.Type = (PACKET_TYPE) 8  ; break;
		case 6 :MP5K_Cfg.Type = (PACKET_TYPE) 9  ; break;
		case 7 :MP5K_Cfg.Type = (PACKET_TYPE) 10 ; break;
		case 8 :MP5K_Cfg.Type = (PACKET_TYPE) 11 ; break;
		case 9   :MP5K_Cfg.Type = (PACKET_TYPE) 12; 
		}


		ErrorCode = Adivic_MP5KWifiBT.BT_Rx_Setting( handle , MP5K_Cfg );
		 
		return ErrorCode;
}

 
  UINT CChroma_API_V2:: Set_BT_TX_Start(void)
  {
	 return Adivic_MP5KWifiBT.BT_Rx_Start(handle);
  }

  

 
