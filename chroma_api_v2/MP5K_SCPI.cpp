#include "stdafx.h"
#include "MP5K_SCPI.h"
# include "math.h"
//#define MP5K	

#define TxPower_Max 10
#define TxPower_Min -130

#define CWFrequencyOffset_Max 50
#define CWFrequencyOffset_Min -50

#define  VsaSampleRateMHz_Max 160 
#define  VsaSampleRateMHz_Min 10

#define VsaRefLevel_Max 30
#define VsaRefLevel_Min -90
#define VsgOutputLevel_Max 120
#define VsgOutputLevel_Min -120

#define VsaCaptureTime_S_Max 10
#define VsaCaptureTime_S_Min 0.000001
#define VsaCaptureCount_Max 100
#define VsaCaptureCount_Min 1

MP5K_SCPI::MP5K_SCPI(void)
{

}

MP5K_SCPI::~MP5K_SCPI()
{

}



bool MP5K_SCPI::ConnectSever(const char * host, const char * port,unsigned int uiTimeout)
{
	bool bFlag;
	stringstream strValue;
    strValue << port;

unsigned int intValue;
strValue >> intValue;
	//const std::string& host, const unsigned int port = 5499, const unsigned int timeout_sec = 2
	//bFlag=advClient.Open(host, static_cast<unsigned short>( *port ),uiTimeout);
bFlag=advClient.Open(host, intValue,uiTimeout);
	return bFlag;
}
void MP5K_SCPI::CloseServer()
{
	advClient.Close();
}

bool MP5K_SCPI::ReadRespond(unsigned int &uiRead)
{
	bool bFlag=false;
	stdStr_ReadRespond="";
	uiRead=0xFFFF;
	int i=0;
	for(i=0;i<WaitTimesMAX;i++)
	{
		if(advClient.WaitToRead(stdStr_ReadRespond))  // put data to queue
		{
			bFlag=true;
			sscanf_s( stdStr_ReadRespond.c_str(),"%d", &uiRead );
			break;
		}
	}
	return bFlag;
}

bool MP5K_SCPI::WaitUntilReapond()
{
	bool bFlag=false;
	stdStr_ReadRespond="";
	for(int i=0;i<WaitTimesMAX;i++)
	{

		if(advClient.WaitToRead(stdStr_ReadRespond))
		{
			bFlag=true;
			break;
		}
	}
	return bFlag;
}
bool MP5K_SCPI::GetTestRawData()
{
	unsigned int uiRead=0;
	bool bFlag=false;

	stdStr_SendCommand="CALCulate:TXQuality:ALL:SYNC\r\n";
	advClient.Send(stdStr_SendCommand);
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
bool MP5K_SCPI::SaveVsaData(const string *strTableName)
{
	sprintf_s(m_SendBuf,"CAPTure:STORe %s.wfm\r\n",strTableName->c_str());
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);
	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
bool MP5K_SCPI::SetCenterFrequency(const double fFreqMHz)
{
	double fCenterFreq=0.0f;
	if(fFreqMHz<10)
		fCenterFreq=10;
	else if(fFreqMHz>6000)
		fCenterFreq=6000;
	else
		fCenterFreq=fFreqMHz;
	m_f_Center_CurrentFreq=fCenterFreq;

	sprintf_s(m_SendBuf,"INPut:FREQuency %.2fMHz\r\n",fCenterFreq);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);
	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
void MP5K_SCPI::GetCenterFrequency(double &fFreqMHz)
{
	stdStr_SendCommand="INPut:FREQuency?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	UINT64 uiFreqHz=0;
	if(!WaitUntilReapond())
		fFreqMHz=-9999;
	else
	{
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
		//sscanf_s( stdStr_ReadRespond.c_str(),"%ld", &uiFreqHz);   // 0606 allen
		sscanf_s(stdStr_ReadRespond.c_str(), "%lld", &uiFreqHz); 
		fFreqMHz= static_cast<double>(uiFreqHz/1e6);
	}
}

bool MP5K_SCPI::SetVsaTriggerType(const MP5800::MP5800_VSA_Trigger_Type triggerType)
{
	switch(triggerType)
	{ 
	 
	case  MP5800::Immediate:
		stdStr_SendCommand="CONFigure:TRIGger:TYPE IMMediate\r\n";
		break;
	case MP5800::Edge_Positive:
		stdStr_SendCommand="CONFigure:TRIGger:TYPE POSTive\r\n";
		break;
   
	case MP5800::Edge_Negative:
		stdStr_SendCommand="CONFigure:TRIGger:TYPE NEGative\r\n";
		break;

	case MP5800::Level:
		stdStr_SendCommand="CONFigure:TRIGger:TYPE LEVel\r\n";
		break;

	case MP5800::External:
		 	stdStr_SendCommand="CONFigure:TRIGger:TYPE EXTernal\r\n";
		break;

	default:
		stdStr_SendCommand="CONFigure:TRIGger:TYPE IMMediate\r\n";
		break;
	}
	advClient.Send(stdStr_SendCommand);
	unsigned int uiRead=0; 
	for(int iTimes=0;iTimes<100;iTimes++)
	{
		if(!ReadRespond(uiRead))
			Sleep(100);
		else
			break;
	}

	return uiRead==0? true:false;
}

bool MP5K_SCPI::SetMeasureMode(const Mea_Mode CurrentMode)
{
	switch(CurrentMode)
	{
	case  _MOD_OFDM:
		stdStr_SendCommand="WIFI:OFDM\r\n";
		break;
	case _MOD_DSSS:
		stdStr_SendCommand="WIFI:DSSS\r\n";
		break;
   
	case _MOD_BLUETOOTH:
		stdStr_SendCommand="BT\r\n";
		break;

	case _MOD_GPRF:
	default:
		stdStr_SendCommand="GPRF\r\n";
		break;
	}
	advClient.Send(stdStr_SendCommand);
	unsigned int uiRead=0; 
	for(int iTimes=0;iTimes<100;iTimes++)
	{
		if(!ReadRespond(uiRead))
			Sleep(100);
		else
			break;
	}

	return uiRead==0? true:false;
}
void MP5K_SCPI::GetMeasureMode(Mea_Mode &CurrentMode)
{
	stdStr_SendCommand="MODule?\r\n";
	advClient.Send(stdStr_SendCommand);

	if(!WaitUntilReapond())
		CurrentMode=_MOD_Error;
	if(0==stdStr_ReadRespond.compare("OFDM"))
		CurrentMode=_MOD_OFDM;
	else if(0==stdStr_ReadRespond.compare("DSSS"))
		CurrentMode=_MOD_DSSS;
	else if(0==stdStr_ReadRespond.compare("GPRF"))
		CurrentMode=_MOD_GPRF;
	else
		CurrentMode=_MOD_Error;	
}
void PORT_Mode(const UINT uiPortMode,string& strMode)
{
	switch(uiPortMode)
	{
	case 0:
	default:
		strMode="OFF";
		break;
	case 1:
		strMode="VSA";
		break;
	case 2:
		strMode="VSG";
		break;
	}
}
bool MP5K_SCPI::Set_4_PortMode(const UINT uiPORT_1,const UINT uiPort_2,const UINT uiPort_3,const UINT uiPort_4)
{
	UINT PortMode[4]={uiPORT_1,uiPort_2,uiPort_3,uiPort_4};
	UINT uiRead=0;
	string strMode="",strTemp="";

#ifdef MP5K
	for(int i=0;i<4;i++)
	{
		PORT_Mode(PortMode[i],strMode);
		sprintf_s(m_SendBuf,"ROUTe:RESource:MAP %s,RF%d\r\n",strMode.c_str(),i+1);
		stdStr_SendCommand=m_SendBuf;
		advClient.Send(stdStr_SendCommand);
		ReadRespond(uiRead);
	}

#else
	stdStr_SendCommand="ROUTe:RESource:MAP ";
	for(int i=0;i<4;i++)
	{
		PORT_Mode(PortMode[i],strMode);
		sprintf_s(m_SendBuf,"%s,RF%d,",strMode.c_str(),i+1);
		strTemp=m_SendBuf;
		stdStr_SendCommand+=strTemp;
	}
	stdStr_SendCommand+="\r\n";
	advClient.Send(stdStr_SendCommand);
	ReadRespond(uiRead);
#endif
	
	if(uiRead!=0) 
		return false;
	else
		return true;
}
bool MP5K_SCPI::SetDistributionModeOnOff(bool bOnOff)
{
	unsigned int uiRead=0;
	bool bFlag=false;

	sprintf_s(m_SendBuf,"CONFigure:RFCorrection:DISTribution %s\r\n",bOnOff?"ON":"OFF");
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
void MP5K_SCPI::GetDistributionModeOnOff(bool &bOnOff)
{
	unsigned int uiRead=0;

	stdStr_SendCommand="CONFigure:RFCorrection:DISTribution?\r\n";
	advClient.Send(stdStr_SendCommand);
	ReadRespond(uiRead);
	bOnOff=(uiRead==1? true:false);
}
bool MP5K_SCPI::SetCurrentTableName(const string *strTableName)
{
	unsigned int uiRead=0;
	bool bFlag=false;

	sprintf_s(m_SendBuf,"TABLe %s\r\n",strTableName->c_str());
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
void MP5K_SCPI::GetCurrentSetTableName(string &strTableName)
{
	unsigned int uiRead=0;
	bool bFlag=false;

	stdStr_SendCommand="TABLe?\r\n";
	advClient.Send(stdStr_SendCommand);

	if(!WaitUntilReapond())
	{
		strTableName="Read Fail";
	}else
	{
		strTableName=stdStr_ReadRespond;
	}
}
bool MP5K_SCPI::SetCurrentLossPoint(const double fFreqMHz,const double fLoss)
{
	unsigned int uiRead=0;
	bool bFlag=false;

	sprintf_s(m_SendBuf,"TABLe:POINt:INSert %.2fMHz,%.2f\r\n",fFreqMHz,fLoss);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
bool MP5K_SCPI::GetCurrentFreqLoss(const double fFreqMHz,double &fLoss)
{
	sprintf_s(m_SendBuf,"TABLe:POINt? %.2fMHz\r\n",fFreqMHz);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);
	int iIndex=0;
	string strTemp;
	double dFreq_Temp=0.0f;
	INT64 iFreqOffsetHz=0;
	INT64 iCenterFreqHz=0;
	if(!WaitUntilReapond())
	{
		fLoss=-9999;return false;
	}
	else
	{
		sscanf_s( stdStr_ReadRespond.c_str(),"%lf", &fLoss);
		return true;
	}
}
bool MP5K_SCPI::ClearCurrentAllLossData()
{
	unsigned int uiRead=0;
	bool bFlag=false;

	stdStr_SendCommand="TABLe:CLEar\r\n";
	advClient.Send(stdStr_SendCommand);
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
bool MP5K_SCPI::SetPortModeLossFile(unsigned int uiPort,bool bIsTx,string strTableName)
{
	unsigned int uiRead=0;
	bool bFlag=false;

	sprintf_s(m_SendBuf,"TABLe:LOSS:PORT%d:%s:LOAD %s\r\n",uiPort,bIsTx?"TX":"RX",strTableName.c_str());
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
bool MP5K_SCPI::SaveLossFileToInstr(string strTableName)
{
	unsigned int uiRead=0;
	bool bFlag=false;

	sprintf_s(m_SendBuf,"TABLe:STORe %s\r\n",strTableName.c_str());
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
bool MP5K_SCPI::DeleteLossFileInInstr(const string& strTableName)
{
	unsigned int uiRead=0;
	bool bFlag=false;

	sprintf_s(m_SendBuf,"TABLe:DELete %s\r\n",strTableName.c_str());
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
bool MP5K_SCPI::Set_4_PortModeIsVsg(bool bPORT_1,bool bPort_2,bool bPort_3,bool bPort_4)
{
	unsigned int uiRead=0; 

	sprintf_s(m_SendBuf,"ROUTe:PORT:RESource RF1,%s\r\n",bPORT_1?"VSG":"VSA");
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);
	ReadRespond(uiRead);
	if(uiRead!=0) return false;

	sprintf_s(m_SendBuf,"ROUTe:PORT:RESource RF2,%s\r\n",bPort_2?"VSG":"VSA");
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);
	ReadRespond(uiRead);
	if(uiRead!=0) return false;

	sprintf_s(m_SendBuf,"ROUTe:PORT:RESource RF3,%s\r\n",bPort_3?"VSG":"VSA");
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);
	ReadRespond(uiRead);
	if(uiRead!=0) return false;

	sprintf_s(m_SendBuf,"ROUTe:PORT:RESource RF4,%s\r\n",bPort_4?"VSG":"VSA");
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);
	ReadRespond(uiRead);
	if(uiRead!=0) return false;


	return true;
}
bool MP5K_SCPI::SetPortMode(unsigned int uiPort,bool bIsVsg)
{
	USHORT uiSetPortIndex=0;
	if(uiPort>=MP5K_Port_Nums)
		uiSetPortIndex=MP5K_Port_Nums-1;
	else
		uiSetPortIndex=uiPort;

	if(bIsVsg)
		sprintf_s(m_SendBuf,"ROUTe:PORT:RESource RF%d,VSG\r\n",uiSetPortIndex+1);
	else
		sprintf_s(m_SendBuf,"ROUTe:PORT:RESource RF%d,VSA\r\n",uiSetPortIndex+1);

	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
void MP5K_SCPI::GetPeakPowerData(double &fFreqMHz,double &fPower,int iWindowIndex)  // allen 0728
{

	GetTestRawData();

	sprintf_s(m_SendBuf,"CALCulate:MARKer:PEAK%d? \r\n",iWindowIndex);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);
	int iIndex=0;
	string strTemp;
	double dFreq_Temp=0.0f;
	double fFreqMHztmp=0.0f;
	double tt = 0.0f;
	long double tempFreq = 0.0f;
	long long tempFreqL = 0.0f;
	INT64 iFreqOffsetHz=0;
	INT64 iCenterFreqHz=0;

	if(!WaitUntilReapond())
	{
		fPower=-9999;fFreqMHz=0;
	}
	else
	{
		iIndex=stdStr_ReadRespond.find_first_of(' ');
		strTemp=stdStr_ReadRespond.substr(iIndex+1);

		sscanf_s( stdStr_ReadRespond.substr(0,iIndex).c_str(),"%lE", &dFreq_Temp );
		 
		//char Result[16]; // string which will contain the number
 
		fFreqMHz = static_cast<double>(dFreq_Temp / 1e6 + m_f_Center_CurrentFreq);
		//sprintf(Result, "%.3f", fFreqMHztmp); // %d ma 
		//fFreqMHz = stod(Result);
		 	 
		sscanf_s( strTemp.c_str(),"%lf", &fPower);
	}
}
void MP5K_SCPI::GetGFSK_FreqDeviation(double &fFreqDevMHz)
{
	if (GetTestRawData())
	{
		stdStr_SendCommand="CALCulate:PEAK:GFSK:DEVIation?\r\n";
		advClient.Send(stdStr_SendCommand);
		//UINT64 uiFreqHz=0;
 
		double dFreq_DevMHz=0.0f;
		if(!WaitUntilReapond())
		{
			fFreqDevMHz=-9999;
		}
		else
		{
			sscanf_s( stdStr_ReadRespond.c_str(),"%lE", &dFreq_DevMHz );
			fFreqDevMHz= static_cast<double>(dFreq_DevMHz/1e6);
		}

	}
}

void MP5K_SCPI::MeasureGFSK(double &dFreqDevMHz,double &dFreqOffset,double &dFreqDrift,double &dDevError,double &dFskError)
{
	if (GetTestRawData())
	{
		stdStr_SendCommand="CALCulate:PEAK:GFSK:DEVIation?\r\n";
		advClient.Send(stdStr_SendCommand);
		//UINT64 uiFreqHz=0;
 
		double dFreq_DevMHz=0.0f;
		double dFreq_Offset = 0.0f;
		double dFreq_Drift = 0.0f;
		double dDev_Error = 0.0f;
		double dFsk_Error = 0.0f;
		if(!WaitUntilReapond())
		{
			dFreqDevMHz=-9999;
		}
		else
		{
			sscanf_s( stdStr_ReadRespond.c_str(),"%lE", &dFreq_DevMHz );
			dFreqDevMHz= static_cast<double>(dFreq_DevMHz/1e6);
		}

		//-------------------------------- GFSK offset
		stdStr_SendCommand="CALCulate:GFSK:FREQuency:OFFSet?\r\n";
		advClient.Send(stdStr_SendCommand);
		//UINT64 uiFreqHz=0;
 
		 
		if(!WaitUntilReapond())
		{
			dFreqOffset=-9999;
		}
		else
		{
			sscanf_s( stdStr_ReadRespond.c_str(),"%lE", &dFreq_Offset );
			dFreqOffset = static_cast<double>(dFreq_Offset/1e6);
		}

		//-------------------------------- GFSK Drift
		stdStr_SendCommand="CALCulate:GFSK:FREQuency:DRIFt?\r\n";
		advClient.Send(stdStr_SendCommand);
		//UINT64 uiFreqHz=0;
 
		 
		if(!WaitUntilReapond())
		{
			dFreqDrift=-9999;
		}
		else
		{
			sscanf_s( stdStr_ReadRespond.c_str(),"%lE", &dFreq_Drift );
			dFreqDrift = static_cast<double>(dFreq_Drift/1e6);
		}

		//-------------------------------- Dev error
		stdStr_SendCommand="CALCulate:GFSK:DEVIation:ERRor?\r\n";
		advClient.Send(stdStr_SendCommand);
		//UINT64 uiFreqHz=0;
 
		 
		if(!WaitUntilReapond())
		{
			dDevError=-9999;
		}
		else
		{
			sscanf_s( stdStr_ReadRespond.c_str(),"%lE", &dDev_Error );
			dDevError = static_cast<double>(dDev_Error/1e6);
		}

		//-------------------------------- Fsk error
		stdStr_SendCommand="CALCulate:GFSK:FSK:ERRor?\r\n";
		advClient.Send(stdStr_SendCommand);
		//UINT64 uiFreqHz=0;
 
		 
		if(!WaitUntilReapond())
		{
			dFskError=-9999;
		}
		else
		{
			sscanf_s( stdStr_ReadRespond.c_str(),"%lE", &dFsk_Error );
			dFskError = static_cast<double>(dFsk_Error/1e6);
		}
	}
}

bool MP5K_SCPI::GetMeasureRawData()
{
	stdStr_SendCommand="CALCulate:TXQuality:ALL:SYNC\r\n";
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	for(int i=0;i<10;i++)
	{
		if(ReadRespond(uiRead))
			break;
		Sleep(100);
	}
	return uiRead==0? true:false;
}
void MP5K_SCPI::GetChannelPower(double &fChPower)
{
	GetTestRawData();

	stdStr_SendCommand="FETCh:SEGMent1:POWer?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;


	if(!WaitUntilReapond())
		fChPower=-9999;
	else
	{
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%lf", &fChPower);
	}
}
void MP5K_SCPI::GetChannelPowerAverage(double &fChPowerAvge)
{
	GetTestRawData();
	stdStr_SendCommand="FETCh:SEGMent1:POWer:AVERage?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;

	if(!WaitUntilReapond())
		fChPowerAvge=-9999;
	else
	{
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%lf", &fChPowerAvge);
	}
}
void MP5K_SCPI::GetChannelPowerMax(double &fChPowerMax)
{
	GetTestRawData();
	stdStr_SendCommand="FETCh:SEGMent1:POWer:MAXimum?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	if(!WaitUntilReapond())
		fChPowerMax=-9999;
	else
	{	
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%lf", &fChPowerMax);
	}
}
void MP5K_SCPI::GetChannelPowerMin(double &fChPowerMin)
{
	GetTestRawData();
	stdStr_SendCommand="FETCh:SEGMent1:POWer:MINimum?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	if(!WaitUntilReapond())
		fChPowerMin=-9999;
	else
	{
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%lf", &fChPowerMin);
	}
}
void MP5K_SCPI::GetChannelPowerPeak(double &fChPowerPeak)
{
	GetTestRawData();
	stdStr_SendCommand="FETCh:SEGMent1:POWer:PEAK?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;

	if(!WaitUntilReapond())
		fChPowerPeak=-9999;
	else
	{
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%lf", &fChPowerPeak);
	}

}
void MP5K_SCPI::GetChannelPowerPeakAverage(double &fChPowerPeakAvge)
{
	GetTestRawData();
	stdStr_SendCommand="FETCh:SEGMent1:POWer:PEAK:AVERage?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	if(!WaitUntilReapond())
		fChPowerPeakAvge=-9999;
	else
	{
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%lf", &fChPowerPeakAvge);
	}
}
void MP5K_SCPI::GetChannelPowerPeakMax(double &fChPowerPeakMax)
{
	GetTestRawData();
	stdStr_SendCommand="FETCh:SEGMent1:POWer:PEAK:MAXimum?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	if(!WaitUntilReapond())
		fChPowerPeakMax=-9999;
	else
	{
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%lf", &fChPowerPeakMax);
	}
}
void MP5K_SCPI::GetChannelPowerPeakMin(double &fChPowerPeakMin)
{
	GetTestRawData();
	stdStr_SendCommand="FETCh:SEGMent1:POWer:PEAK:MINimum?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	if(!WaitUntilReapond())
		fChPowerPeakMin=-9999;
	else
	{
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%lf", &fChPowerPeakMin);
	}
}
bool MP5K_SCPI::SetVsaStartOnOff(const bool bIsStart)
{
	if(bIsStart)
		stdStr_SendCommand="INPut:STARt\r\n";
	else
		stdStr_SendCommand="INPut:STOP\r\n";

	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
bool MP5K_SCPI::SetVsaSampleRate(const int iSampleRateMHz)
{
	int iSetSampleRateMHz=0;
	if(iSetSampleRateMHz>VsaSampleRateMHz_Max)
		iSetSampleRateMHz=VsaSampleRateMHz_Max;
	else if(iSampleRateMHz<VsaSampleRateMHz_Min)
		iSetSampleRateMHz=VsaSampleRateMHz_Min;
	else
		iSetSampleRateMHz=iSampleRateMHz;

	sprintf_s(m_SendBuf,"INPut:SRATe %d MHz\r\n",iSetSampleRateMHz);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
void MP5K_SCPI::GetVsaSampleRate(int &iSampleRateMHz)
{
	stdStr_SendCommand="INPut:SRATe?\r\n";
	advClient.Send(stdStr_SendCommand);
	long lSampleRate_Hz;
	CStringA csTemp;int iPos=2;
	if(!WaitUntilReapond())
		iSampleRateMHz=-9999;
	else
	{
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%d", &lSampleRate_Hz);
		iSampleRateMHz=(int)(lSampleRate_Hz/1000000);
	}
}
bool MP5K_SCPI::SetVsaRBW(const UINT64 uiRBW_Hz)
{
	bool bFlag=false;
	int iSetRBW_Hz=0;

	// 	switch(uiEnum)
	// 	{
	// 	case RBW_100_Hz:iSetRBW_Hz=100;break;
	// 	case RBW_500_Hz:iSetRBW_Hz=500;break;
	// 	case RBW_1K_Hz:iSetRBW_Hz=1000;break;	
	// 	case RBW_2K_Hz:iSetRBW_Hz=2000;break;
	// 	case RBW_5K_Hz:iSetRBW_Hz=5000;break;
	// 	case RBW_10K_Hz:iSetRBW_Hz=10000;break;
	// 	case RBW_20K_Hz:iSetRBW_Hz=20000;break;
	// 	case RBW_50K_Hz:iSetRBW_Hz=5e4;break;
	// 	case RBW_100K_Hz:iSetRBW_Hz=1e5;break;
	// 	case RBW_200K_Hz:iSetRBW_Hz=2e5;break;
	// 	case RBW_500K_Hz:iSetRBW_Hz=5e5;break;
	// 	case RBW_1M_Hz:iSetRBW_Hz=1e6;break;
	// 	case RBW_2M_Hz:iSetRBW_Hz=2e6;break;
	// 	case RBW_5M_Hz:iSetRBW_Hz=5e6;break;
	// 	}

	//sprintf_s(m_SendBuf,"CONFigure:GPRF:SPECtrum:RBW %d\r\n",uiRBW_Hz);   // 0602
	sprintf_s(m_SendBuf, "CONFigure:GPRF:SPECtrum:RBW %I64d\r\n", uiRBW_Hz);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
void MP5K_SCPI::GetVsaRBW_Hz(UINT64 &uiRBW_Hz)
{
	stdStr_SendCommand="CONFigure:GPRF:SPECtrum:RBW?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	if(!WaitUntilReapond())
		uiRBW_Hz=0;
	else
	{
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
		//sscanf_s(stdStr_ReadRespond.c_str(), "%d", &uiRBW_Hz);
		sscanf_s( stdStr_ReadRespond.c_str(),"%I64d", &uiRBW_Hz);
	}
}
bool MP5K_SCPI::SetVsaReferenceLevel(const double fRefLevel)
{
	double fSetRefLevel=0.0f;
	if(fRefLevel>VsaRefLevel_Max)
		fSetRefLevel=VsaRefLevel_Max;
	else if(fRefLevel<VsaRefLevel_Min)
		fSetRefLevel=VsaRefLevel_Min;
	else
		fSetRefLevel=fRefLevel;

	sprintf_s(m_SendBuf,"INPut:LEVel:REFerence %.2f dBm\r\n",fSetRefLevel);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}

bool MP5K_SCPI::SetVsaCaptureLength(const double dCaptureLength_ms)
{
	
	sprintf_s(m_SendBuf,"INPut:CAPTure:TIME %.6f \r\n",dCaptureLength_ms );
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
 
bool MP5K_SCPI::SetVsaTriggerTimeOut(const double dTriggerTimeOut_ms)
{
	
	sprintf_s(m_SendBuf,"CONFigure:TRIGger:TIMeout %.6f \r\n",dTriggerTimeOut_ms);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
 
bool MP5K_SCPI::SetVsaGapTime(const double dCapTime_us)
{
	 sprintf_s(m_SendBuf,"CONFigure:TRIGger:SLICe %.9f \r\n",dCapTime_us );
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}

void MP5K_SCPI::GetVsaReferenceLevel(double &fRefLevel)
{
	stdStr_SendCommand="INPut:LEVel:REFerence?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	if(!WaitUntilReapond())
		fRefLevel=-9999;
	else
	{
		// 		csTemp=stdStr_ReadRespond.c_str();
		// 		iPos=csTemp.FindOneOf(",");
		// 		csTemp=csTemp.Tokenize(",",iPos);
		// 		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%lf", &fRefLevel);
		//fRefLevel = atof(stdStr_ReadRespond.c_str());
	}
}
bool MP5K_SCPI::VsaReferenceLevelAuto()
{
	sprintf_s(m_SendBuf,"INPut:LEVel:REFerence:AUTO\r\n");
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
bool MP5K_SCPI::SetVsaCaptureTime(const double dRate_S)//Max 10s,Min 0.000001
{
	double dSetRate_S=0.0f;
	if(dRate_S>VsaCaptureTime_S_Max)
		dSetRate_S=VsaCaptureTime_S_Max;
	else if(dRate_S<VsaCaptureTime_S_Min)
		dSetRate_S=VsaCaptureTime_S_Min;
	else
		dSetRate_S=dRate_S;

	sprintf_s(m_SendBuf,"INPut:CAPTure:TIME %f s\r\n",dSetRate_S);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
void MP5K_SCPI::GetVsaCaptureTime(double &fRate_S)
{
	stdStr_SendCommand="INPut:CAPTure:TIME?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	if(!WaitUntilReapond())
		fRate_S=-9999;
	else
	{
		/*  allen 20170619
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
		*/
		sscanf_s( stdStr_ReadRespond.c_str(),"%lf", &fRate_S);
	}
}
bool MP5K_SCPI::SetVsaCaptureCount(const UINT uiCountNum)
{
	UINT uiSetCount=0;

	if(uiCountNum>VsaCaptureCount_Max)
		uiSetCount=VsaCaptureCount_Max;
	else if(uiCountNum<VsaCaptureCount_Min)
		uiSetCount=VsaCaptureCount_Min;
	else
		uiSetCount=uiCountNum;

	sprintf_s(m_SendBuf,"INPut:CAPTure:COUNt %d\r\n",uiCountNum);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
void MP5K_SCPI::GetVsaCaptureCount(UINT &uiCountNum)
{
	stdStr_SendCommand="INPut:CAPTure:COUNt?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	if(!WaitUntilReapond())
		uiCountNum=9999;
	else
	{
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%d", &uiCountNum);
	}
}
bool MP5K_SCPI::SetVsaSkipCaptureCount(const UINT uiSkipNum)
{
	UINT uiSetSkip=0;

	if(uiSkipNum>VsaCaptureCount_Max)
		uiSetSkip=VsaCaptureCount_Max;
	else
		uiSetSkip=uiSkipNum;

	sprintf_s(m_SendBuf,"INPut:TXQuality:SKIP:COUNt %d\r\n",uiSetSkip);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
void MP5K_SCPI::GetVsaSkipCaptureCount(UINT &uiSkipNum)
{
	stdStr_SendCommand="INPut:CAPTure:COUNt?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	if(!WaitUntilReapond())
		uiSkipNum=9999;
	else
	{
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%d", &uiSkipNum);
	}
}
bool MP5K_SCPI::SaveMeasureWaveForm(const char * WaveFormName)
{
	sprintf_s(m_SendBuf,"CAPTure:STORe %s\r\n",WaveFormName);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
bool MP5K_SCPI::AddMarkerFreqOffset(const double fFreqOffsetMHz)
{
	double fSetFreq=0.0f;
	if(fFreqOffsetMHz>100)
		fSetFreq=100;
	else if(fFreqOffsetMHz<-100)
		fSetFreq=-100;
	else
		fSetFreq=fFreqOffsetMHz;

	sprintf_s(m_SendBuf,"INPut:GPRF:MARKer:ADD %.3fMHz\r\n",fSetFreq);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	uiRead==0? true:false;
	//--------------------------------------------------------------------------------
	sprintf_s(m_SendBuf,"INPut:MARKer:FIG4:ADD %.3fMHz\r\n",fSetFreq);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}

void MP5K_SCPI::GetCWChannelPower(const double fMarkFreqMHz, const double leftSpan_Hz, const double rightSpan_Hz, 
	                 double &PowerMeasured, double &PowerMeasured_Left, double &PowerMeasured_Right, double &Power_Comp, double &Frequency_Offset )
{
	 
	GetTestRawData();
	sprintf_s(m_SendBuf,":CHPower:CW? %.3fMHz,%.3fHz,%.3fHz \r\n",fMarkFreqMHz, leftSpan_Hz,rightSpan_Hz  );
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	if(!WaitUntilReapond())
	{
		    PowerMeasured = -9999;
			PowerMeasured_Left = -9999;
			PowerMeasured_Right = -9999;
			Power_Comp = -9999;
			Frequency_Offset = -9999;
	}
	else
	{

		int errorCode;
		sscanf_s( stdStr_ReadRespond.c_str(),"%d,%lf,%lf,%lf,%lf,%lf", &errorCode,  &PowerMeasured, &PowerMeasured_Left,&PowerMeasured_Right, &Power_Comp ,&Frequency_Offset);

		if ( errorCode != 0)
		{
			PowerMeasured = -9999;
			PowerMeasured_Left = -9999;
			PowerMeasured_Right = -9999;
			Power_Comp = -9999;
			Frequency_Offset = -9999;
		}
	}
		//fPower = atof(stdStr_ReadRespond.c_str());
}


bool MP5K_SCPI:: SetDigitalDownConvertor(const bool bOnOff,  const double offSet_Hz )
{
	 
 	sprintf_s(m_SendBuf,"INPut:DDC %s, %.2f\r\n",bOnOff?"ON":"OFF", offSet_Hz);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}

void MP5K_SCPI::GetChannelPower2Span(const double fMarkFreqMHz, const double leftSpan_Hz, const double rightSpan_Hz, 
	                 double &PowerMeasured, double &PowerMeasured_Left, double &PowerMeasured_Right )
{
	 
	GetTestRawData();
	sprintf_s(m_SendBuf,":CHPower? %.3fMHz,%.3fHz,%.3fHz \r\n",fMarkFreqMHz, leftSpan_Hz,rightSpan_Hz  );
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	if(!WaitUntilReapond())
	{
		    PowerMeasured = -9999;
			PowerMeasured_Left = -9999;
			PowerMeasured_Right = -9999;
			}
	else
	{

		int errorCode;
		sscanf_s( stdStr_ReadRespond.c_str(),"%d,%lf,%lf,%lf", &errorCode,  &PowerMeasured, &PowerMeasured_Left,&PowerMeasured_Right);

		if ( errorCode != 0)
		{
			PowerMeasured = -9999;
			PowerMeasured_Left = -9999;
			PowerMeasured_Right = -9999;
		 }
	}
		//fPower = atof(stdStr_ReadRespond.c_str());
}

void MP5K_SCPI::GetMarkerPower(double &fPower)
{
	GetTestRawData();

	stdStr_SendCommand="INPut:GPRF:MARKer:POWer?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	int iIndex=0;
	if(!WaitUntilReapond())
		fPower=-9999;
	else
	{
#ifdef MP5K
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
#endif
		sscanf_s( stdStr_ReadRespond.c_str(),"%lf", &fPower);
		//fPower = atof(stdStr_ReadRespond.c_str());
	}
}

void MP5K_SCPI::GetPowerByTime(double &dPowerByTime)
{
	GetTestRawData();

	stdStr_SendCommand="FETCh:ALL:POWer?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	int iIndex=0;
	if(!WaitUntilReapond())
		dPowerByTime=-9999;
	else
	{
#ifdef MP5K
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
#endif
		int errCode;
		sscanf_s( stdStr_ReadRespond.c_str(),"%d,%lf", &errCode, &dPowerByTime);
		if (errCode != 0)
		{
			dPowerByTime = -9999;
		}
		//fPower = atof(stdStr_ReadRespond.c_str());
	}
}

void MP5K_SCPI::GetPeakPowerFrequencyHzByTime(double &dPeakPowerFrequencyHzByTime)
{
	GetTestRawData();

	stdStr_SendCommand="FETCh:SPECtrum:PEAK:OFRequency?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	int iIndex=0;
	if(!WaitUntilReapond())
		dPeakPowerFrequencyHzByTime=-9999;
	else
	{
#ifdef MP5K
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
#endif
		int errorCode;
		sscanf_s( stdStr_ReadRespond.c_str(),"%d, %lf",&errorCode, &dPeakPowerFrequencyHzByTime);
		if ( errorCode !=0)
		{
			dPeakPowerFrequencyHzByTime=-9999;
		}

		//fPower = atof(stdStr_ReadRespond.c_str());
	}
}

bool MP5K_SCPI::FskSetFilterType(Filter_Mode _filterMode)
{
	switch(_filterMode)
	{
		case _FMOD_None:
			stdStr_SendCommand="CONFigure:GFSK:FILTer:TYPE No\r\n";
			break;
		case _FMOD_Raise_cosine:
			stdStr_SendCommand="CONFigure:GFSK:FILTer:TYPE Raise_cosine\r\n";
			break;
		case _FMOD_Root_raise_cosine:
			stdStr_SendCommand="CONFigure:GFSK:FILTer:TYPE Root_raise_cosine\r\n";
			break;
		case _FMOD_Gaussian:
			stdStr_SendCommand="CONFigure:GFSK:FILTer:TYPE Gaussian\r\n";
			break;
	}
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
//bool MP5K_SCPI::FskSetSymbolPhaseContinuity(bool bIsContinue)
bool MP5K_SCPI::FskSetSymbolPhaseContinuity(USHORT usCountinuity)
{
	//sprintf_s(m_SendBuf,"CONFigure:GFSK:SYMBol:PHASe:CONTinuity %s\r\n",bIsContinue?"Continuity":"Discontiunity");
	char strSymbolPhaseCountinuity[2][20] = {"Continuity","Discontiunity"};

	sprintf_s(m_SendBuf,"CONFigure:GFSK:SYMBol:PHASe:CONTinuity %s\r\n",strSymbolPhaseCountinuity[usCountinuity]);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
bool MP5K_SCPI::FskSetcoefficient(double fCofficient)
{
	if(fCofficient<0)fCofficient=0;
	else if(fCofficient>1)fCofficient=1;
	sprintf_s(m_SendBuf,"CONFigure:GFSK:FILTer:COEFficient %.2f\r\n",fCofficient);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
bool MP5K_SCPI::FskSetFilterLength(USHORT usLength)
{
	if(usLength<1)usLength=1;
	else if(usLength>16)usLength=16;
	sprintf_s(m_SendBuf,"CONFigure:GFSK:FILTer:LENGth %d\r\n",usLength);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
bool MP5K_SCPI::FskSetSymbolRate(unsigned int uiRateHz)
{
	if(uiRateHz<FSK_SymboRate_Min)uiRateHz=FSK_SymboRate_Min;
	else if(uiRateHz>FSK_SymboRate_Max)uiRateHz=FSK_SymboRate_Max;

	sprintf_s(m_SendBuf,"CONFigure:GFSK:SYMBol:RATE %d\r\n",uiRateHz);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
bool MP5K_SCPI::FSK_SetFreqDeviation(unsigned int uiFreqDev_Hz)
{
	if(uiFreqDev_Hz<(FSK_SymboRate_Min/2))
		uiFreqDev_Hz=FSK_SymboRate_Min/2;
	else if(uiFreqDev_Hz>(FSK_SymboRate_Max/2))
		uiFreqDev_Hz=(FSK_SymboRate_Max/2);

	sprintf_s(m_SendBuf,"CONFigure:GFSK:FREQuency:DEVIation %d\r\n",uiFreqDev_Hz);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
void MP5K_SCPI::FSK_GetFreqDeviation(double &fFreqDevMHz)
{
	unsigned int uiFreqDev_Hz=0;
	stdStr_SendCommand="CONFigure:GFSK:FREQuency:DEVIation?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	if(!WaitUntilReapond())
		fFreqDevMHz=9999;
	else
	{
// 		csTemp=stdStr_ReadRespond.c_str();
// 		iPos=csTemp.FindOneOf(",");
// 		csTemp=csTemp.Tokenize(",",iPos);
// 		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%d", &uiFreqDev_Hz);
		// fFreqDevMHz=uiFreqDev_Hz/1e6;   // 0606 Allen
		fFreqDevMHz = static_cast<double>(uiFreqDev_Hz / 1e6);
	}
}
/*
bool MP5K_SCPI::FskSetType(USHORT usType)
{
	USHORT usSetType=0;
	if(usType==2||usType==4||usType==8)
		usSetType=usType;
	else
	usSetType=2;

	sprintf_s(m_SendBuf,"CONFigure:GFSK:FSK:TYPE %d\r\n",usSetType);

	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
*/

 
bool MP5K_SCPI::FskSetType(USHORT usFskType)
{
	USHORT FskType[3]={2,4,8};

	sprintf_s(m_SendBuf,"CONFigure:GFSK:FSK:TYPE %d\r\n",FskType[usFskType]);

	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
 
void MP5K_SCPI::FskGetType(USHORT &usType)
{
	stdStr_SendCommand="CONFigure:GFSK:FSK:TYPE?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	if(!WaitUntilReapond())
		usType=9999;
	else
	{
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
	//	sscanf_s( stdStr_ReadRespond.c_str(),"%d", &usType);
		sscanf_s(stdStr_ReadRespond.c_str(), "%hd", &usType);
	}
}
/*
bool MP5K_SCPI::SetDemodulationtype(bool bIsDeMode)
{
	unsigned int uiRead=0;
	sprintf_s(m_SendBuf,"CONFigure:GFSK:DEMOdulation:TYPE %s\r\n",bIsDeMode?"FSK":"NULL");
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
*/
bool MP5K_SCPI::SetDemodulationtype(USHORT usType)
{
	unsigned int uiRead=0;
	char strDemodulationtype[2][10] = {"NULL","FSK"};
	sprintf_s(m_SendBuf,"CONFigure:GFSK:DEMOdulation:TYPE %s\r\n",strDemodulationtype[usType]);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}

bool MP5K_SCPI::SetDemodulationTool(USHORT usSloution)
{
	sprintf_s(m_SendBuf,"CONFigure:GFSK:DEMO:TOOL %d\r\n",usSloution);

	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
bool MP5K_SCPI::ClearMarker()
{
	stdStr_SendCommand="INPut:GPRF:MARKer:CLEar\r\n";
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	uiRead==0? true:false;
	//----------------------------------------------------------
	stdStr_SendCommand="INPut:MARKer:FIG4:CLEar\r\n";
	advClient.Send(stdStr_SendCommand);
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
bool MP5K_SCPI::SetVsgRfStartStop(const bool bVsgStart)
{

	if(bVsgStart)
		stdStr_SendCommand="OUTPut:STARt\r\n";
	else
		stdStr_SendCommand="OUTPut:STOP\r\n";

	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
bool MP5K_SCPI::SetVsgOutputLevel(const double fOutputLevel)
{
	double fSetOutputLevel=0.0f;

	if(fOutputLevel>VsgOutputLevel_Max)
		fSetOutputLevel=VsgOutputLevel_Max;
	else if(fOutputLevel<VsgOutputLevel_Min)
		fSetOutputLevel=VsgOutputLevel_Min;
	else
		fSetOutputLevel=fOutputLevel;

	sprintf_s(m_SendBuf,"OUTPut:POWer:LEVel %.2f dBm\r\n",fSetOutputLevel);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;

}
void MP5K_SCPI::GetVsgOutputLevel(double &fOutputLevel)
{
	stdStr_SendCommand="OUTPut:POWer:LEVel?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	if(!WaitUntilReapond())
		fOutputLevel=-9999;
	else
	{
		//csTemp=stdStr_ReadRespond.c_str();
		//iPos=csTemp.FindOneOf(",");
		//csTemp=csTemp.Tokenize(",",iPos);
		//stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%lf", &fOutputLevel);
	}
}
bool MP5K_SCPI::SetVsgPlayFileName(const char * PlayFileName)
{
	sprintf_s(m_SendBuf,"OUTPut:WAVeform:LOAD \\\\USER\\%s\r\n",PlayFileName);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}

bool MP5K_SCPI::SetVsgPlayFileNameWithFilePath(const char * PlayFileName, const char *filePathName)
{
	sprintf_s(m_SendBuf,"OUTPut:WAVeform:LOAD \\\\%s\\%s\r\n",filePathName,PlayFileName);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}

bool MP5K_SCPI::SetVsgPlayTimes(const UINT uiTimes)
{
	sprintf_s(m_SendBuf,"OUTPut:LOOP:COUNt %d \r\n",uiTimes);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
void MP5K_SCPI::GetVsgPlayTimes(UINT &uiTimes)
{
	stdStr_SendCommand="OUTPut:LOOP:COUNt?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	if(!WaitUntilReapond())
		uiTimes=9999;
	else
	{
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%d", &uiTimes);
	}
}
bool MP5K_SCPI::SetVsgModulationOnOff(const bool bModOn)
{
	if(bModOn) 
		stdStr_SendCommand="OUTPut:MODulation:STATe ON\r\n"; 
	else
		stdStr_SendCommand="OUTPut:MODulation:STATe OFF\r\n";

	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
void MP5K_SCPI::GetVsgModulationOnOff(bool &bModOn)
{
	stdStr_SendCommand="OUTPut:MODulation:STATe?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	int iTemp=0;
	if(!WaitUntilReapond())
	{	
		bModOn=false;
	}
	else
	{
		// remark for new CW modulation, if CW mode , it is not need "RUN" (output:start)
//		csTemp=stdStr_ReadRespond.c_str();
//		iPos=csTemp.FindOneOf(",");
//		csTemp=csTemp.Tokenize(",",iPos);
//		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%d",&iTemp);
	}
	bModOn=iTemp==1?true:false;
}
bool MP5K_SCPI::SetVsgRfOnOff(const bool bRfOn)
{
	if(bRfOn) 
		stdStr_SendCommand="OUTPut:POWer:STATe ON\r\n"; 
	else
		stdStr_SendCommand="OUTPut:POWer:STATe OFF\r\n";

	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
void MP5K_SCPI::GetVsgRfOnOff(bool &bRfOn)
{
	stdStr_SendCommand="OUTPut:POWer:STATe?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	int iTemp=0;
	if(!WaitUntilReapond())
	{	
		bRfOn=false;
	}
	else
	{
		// 		csTemp=stdStr_ReadRespond.c_str();
		// 		iPos=csTemp.FindOneOf(",");
		// 		csTemp=csTemp.Tokenize(",",iPos);
		// 		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%d",&iTemp);
	}
	bRfOn=iTemp==1?true:false;
}

bool MP5K_SCPI::SetVsgPlayGapUs(const UINT GapUs)
{
	double Gap_s=GapUs * 1e-6;
	 

	sprintf_s(m_SendBuf,"CONFigure:WAVE:GAP %f\r\n",Gap_s);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}

bool MP5K_SCPI::SetVsaTriggerOffsetUs(const int OffsetUs)
{
	double Offset_s=OffsetUs * 1e-6;
	 

	sprintf_s(m_SendBuf,"CONFigure:TRIGger:OFFSet %f\r\n",Offset_s);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}


bool MP5K_SCPI::Set_CW_Offset(const double fOffsetMHz)
{
	double fSetFreq=0.0f;
	if(fOffsetMHz>60)
		fSetFreq=60;
	else if(fOffsetMHz<-60)
		fSetFreq=-60;
	else
		fSetFreq=fOffsetMHz;

	sprintf_s(m_SendBuf,"OUTPut:WAVeform:CWAV:OFFSet %fMHz\r\n",fSetFreq);
	stdStr_SendCommand=m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
void MP5K_SCPI::Get_CW_Offset(double &fOffsetMHz)
{
	stdStr_SendCommand="OUTPut:WAVeform:CWAV:OFFSet?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	double fFreqOffset=0.0f;
	if(!WaitUntilReapond())
	{	
		fOffsetMHz=-9999;
		return;
	}
	else
	{
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
	    sscanf_s( stdStr_ReadRespond.c_str(),"%lf",fFreqOffset);	
		fFreqOffset/=1e6;
		fOffsetMHz=fFreqOffset;
	}
}


//------------------------------------------------------------------
//  set/get spectrum mode
//-----------------------------------------------------------------
bool MP5K_SCPI::SetSpectrumOnOff(const bool bModOn)
{
	if(bModOn) 
		stdStr_SendCommand="DISPlay:GRAPhics:STATe ON\r\n"; 
	else
		stdStr_SendCommand="DISPlay:GRAPhics:STATe OFF\r\n";

	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
void MP5K_SCPI::GetSpectrumOnOff(bool &bModOn)
{
	stdStr_SendCommand="DISPlay:GRAPhics:STATe?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
	int iTemp=0;
	if(!WaitUntilReapond())
	{	
		bModOn=false;
	}
	else
	{
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%d",&iTemp);
	}
	bModOn=iTemp==1?true:false;
}

//------------------------------------------------------------------
//  set/get cableLoss Table Couple mode : couple means TX,RX cable loss are same.
//-----------------------------------------------------------------
bool MP5K_SCPI::SetCableLossTableCoupleMode(const bool bModOn)
{
	if(bModOn) 
		stdStr_SendCommand="TABLe:COUPled ON\r\n"; 
	else
		stdStr_SendCommand="TABLe:COUPled OFF\r\n";

	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead=0; 
	ReadRespond(uiRead);
	return uiRead==0? true:false;
}
void MP5K_SCPI::GetCableLossTableCoupleMode(bool &bModOn)
{
	stdStr_SendCommand="TABLe:COUPled?\r\n";
	advClient.Send(stdStr_SendCommand);
	CStringA csTemp;int iPos=2;
 	int iTemp=0;
	if(!WaitUntilReapond())
	{	
		bModOn=false;
	}
	else
	{
		csTemp=stdStr_ReadRespond.c_str();
		iPos=csTemp.FindOneOf(",");
		csTemp=csTemp.Tokenize(",",iPos);
		stdStr_ReadRespond=csTemp;
		sscanf_s( stdStr_ReadRespond.c_str(),"%d",&iTemp);
	}
	bModOn=iTemp==1?true:false;
}


bool MP5K_SCPI::SetFilterWindow(const UINT filterType)
{
	sprintf_s(m_SendBuf, "INPut:GPRF:FTT:Window %d \r\n", filterType);
	stdStr_SendCommand = m_SendBuf;
	advClient.Send(stdStr_SendCommand);

	unsigned int uiRead = 0;
	ReadRespond(uiRead);
	return uiRead == 0 ? true : false;
}