#include "StdAfx.h"
#include "Config.h"


CConfig::CConfig(void)
{
	G_MEM_ST = 0x20000;
	G_MEM_ED = 0x23FFF;
	G_N_As = 1000;
	G_N_Ar = 1000;
	G_N_Bs = 1000;
	G_N_Br = 0;
	G_N_Cs = 0;
	G_N_Cr = 1000;

}


CConfig::~CConfig(void)
{
}

void CConfig::ReadINI()
{
	CFileFind finder;   //查找是否存在ini文件，若不存在，则生成一个新的默认设置的ini文件，这样就保证了我们更改后的设置每次都可用  
	BOOL ifFind = finder.FindFile(C_CONFIG_FILE_NAME); 
	CString temp;
	if( !ifFind )  
	{
		temp.Format("%x",G_MEM_ST);
		::WritePrivateProfileStringA(_T("mem"),_T("MEM_ST"),temp,C_CONFIG_FILE_NAME);
		temp.Format("%x",G_MEM_ED);
		::WritePrivateProfileStringA(_T("mem"),_T("MEM_ED"),temp,C_CONFIG_FILE_NAME);
		temp.Format("%x",G_N_As);
		::WritePrivateProfileStringA(_T("tp"),_T("N_As"),temp,C_CONFIG_FILE_NAME);
		temp.Format("%x",G_N_Ar);
		::WritePrivateProfileStringA(_T("tp"),_T("N_Ar"),temp,C_CONFIG_FILE_NAME);
		temp.Format("%x",G_N_Bs);
		::WritePrivateProfileStringA(_T("tp"),_T("N_Bs"),temp,C_CONFIG_FILE_NAME);
		temp.Format("%x",G_N_Br);
		::WritePrivateProfileStringA(_T("tp"),_T("N_Br"),temp,C_CONFIG_FILE_NAME);
		temp.Format("%x",G_N_Cs);
		::WritePrivateProfileStringA(_T("tp"),_T("N_Cs"),temp,C_CONFIG_FILE_NAME);
		temp.Format("%x",G_N_Cr);
		::WritePrivateProfileStringA(_T("tp"),_T("N_Cr"),temp,C_CONFIG_FILE_NAME);
		temp.Format("%x",G_P2_CLIENT);
		::WritePrivateProfileStringA(_T("uds"),_T("P2_CLIENT"),temp,C_CONFIG_FILE_NAME);
		
	}else{
		::GetPrivateProfileStringA(_T("mem"),_T("MEM_ST"),_T("0x20000"),temp.GetBuffer(MAX_PATH),MAX_PATH,C_CONFIG_FILE_NAME); 
		G_MEM_ST = atoi(temp);
		::GetPrivateProfileStringA(_T("mem"),_T("MEM_ED"),_T("0x23FFF"),temp.GetBuffer(MAX_PATH),MAX_PATH,C_CONFIG_FILE_NAME); 
		G_MEM_ED = atoi(temp);
		::GetPrivateProfileStringA(_T("tp"),_T("N_As"),_T("1000"),temp.GetBuffer(MAX_PATH),MAX_PATH,C_CONFIG_FILE_NAME); 
		G_N_As = atoi(temp);
		::GetPrivateProfileStringA(_T("tp"),_T("N_Ar"),_T("1000"),temp.GetBuffer(MAX_PATH),MAX_PATH,C_CONFIG_FILE_NAME); 
		G_N_Ar = atoi(temp);
		::GetPrivateProfileStringA(_T("tp"),_T("N_Bs"),_T("1000"),temp.GetBuffer(MAX_PATH),MAX_PATH,C_CONFIG_FILE_NAME); 
		G_N_Bs = atoi(temp);
		::GetPrivateProfileStringA(_T("tp"),_T("N_Br"),_T("0"),temp.GetBuffer(MAX_PATH),MAX_PATH,C_CONFIG_FILE_NAME); 
		G_N_Br = atoi(temp);
		::GetPrivateProfileStringA(_T("tp"),_T("N_Cs"),_T("0"),temp.GetBuffer(MAX_PATH),MAX_PATH,C_CONFIG_FILE_NAME); 
		G_N_Cs = atoi(temp);
		::GetPrivateProfileStringA(_T("tp"),_T("N_Cr"),_T("1000"),temp.GetBuffer(MAX_PATH),MAX_PATH,C_CONFIG_FILE_NAME); 
		G_N_Cr = atoi(temp);
		::GetPrivateProfileStringA(_T("uds"),_T("P2_CLIENT"),_T("5000"),temp.GetBuffer(MAX_PATH),MAX_PATH,C_CONFIG_FILE_NAME); 
		G_P2_CLIENT = atoi(temp);
	}
}
