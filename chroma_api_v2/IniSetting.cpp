#include "StdAfx.h"
#include "IniSetting.h"
#define MAX_STRING_LEN			1000

IniSetting::IniSetting(void)
{
}


IniSetting::~IniSetting(void)
{
}

USHORT IniSetting::SetConfigFilePath(CString csFilePath)
{
	s_szConfigFilePath=csFilePath;
	return SUCCESS;
}
USHORT IniSetting::ParserConfigFile()
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
USHORT IniSetting::WriteConfigFile()
{
	CString szSectionName=_T("");
	CString szFieldName=_T("");
	CString szWriteString=_T("");
	CString szStep=_T("");
	USHORT usState=0;
	DWORD dwError;
	char szBuf[1024]={0};
	//***************************************************
	szSectionName.Format(_T("Server Config Setting"));
	

	szFieldName.Format(_T("Server_1_IP"));
	szWriteString=s_szServer_IP[0];
	usState=WritePrivateProfileString(szSectionName,szFieldName,szWriteString.GetBuffer(),s_szConfigFilePath);
	dwError=GetLastError();
	
	szFieldName.Format(_T("Server_1_Port"));
	szWriteString=s_szServer_Port[0];
	usState=WritePrivateProfileString(szSectionName,szFieldName,szWriteString.GetBuffer(),s_szConfigFilePath);

	szFieldName.Format(_T("Server_2_IP"));
	szWriteString=s_szServer_IP[1];
	usState=WritePrivateProfileString(szSectionName,szFieldName,szWriteString.GetBuffer(),s_szConfigFilePath);
	dwError=GetLastError();

	szFieldName.Format(_T("Server_2_Port"));
	szWriteString=s_szServer_Port[1];
	usState=WritePrivateProfileString(szSectionName,szFieldName,szWriteString.GetBuffer(),s_szConfigFilePath);

	szFieldName.Format(_T("IsServer_1_Open"));
	szWriteString=s_bIsServer_OnOff[0]?"On":"Off";
	usState=WritePrivateProfileString(szSectionName,szFieldName,szWriteString.GetBuffer(),s_szConfigFilePath);

	szFieldName.Format(_T("IsServer_2_Open"));
	szWriteString=s_bIsServer_OnOff[1]?"On":"Off";
	usState=WritePrivateProfileString(szSectionName,szFieldName,szWriteString.GetBuffer(),s_szConfigFilePath);

	//---------------------------------------------
	szSectionName.Format(_T("Server File Path"));
	
	szFieldName.Format(_T("Server_1_PortLossPath"));
	szWriteString=s_szServer_PortLoss[0];
	usState=WritePrivateProfileString(szSectionName,szFieldName,szWriteString.GetBuffer(),s_szConfigFilePath);

	szFieldName.Format(_T("Server_2_PortLossPath"));
	szWriteString=s_szServer_PortLoss[1];
	usState=WritePrivateProfileString(szSectionName,szFieldName,szWriteString.GetBuffer(),s_szConfigFilePath);
	//--------------------------------------------------------
	szSectionName.Format(_T("Waveform List"));
	
	usState=WritePrivateProfileString(szSectionName,0,0,s_szConfigFilePath);

	for(int iCount=0;iCount<s_vec_WaveformPath.size();iCount++)
	{
		szFieldName.Format(_T("Waveform_Name_%d"),iCount+1);
		szWriteString=s_vec_WaveformPath.at(iCount);
		usState=WritePrivateProfileString(szSectionName,szFieldName,szWriteString.GetBuffer(),s_szConfigFilePath);
		//sprintf_s(szBuf,"Waveform_Name_%d",iCount);
	}

	return SUCCESS;
}