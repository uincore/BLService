
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持







#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


extern "C" {
	int myprintf(const char * _Format, ...);
	int myfprintf(__inout FILE * _File, const char * _Format, ...);
	void progress_Changed(unsigned value);
}

// UDP protocol
#define C_POSITIVE_RESPONSE			0x40
#define C_NEGATIVE_RESPONSE			0x7F
#define C_SID_DiagnosticSessionControl	0x10
typedef enum {
	diagnosticSessionreserver=0
	,defaultSession=1
	, programmingSession=2
	,extendedDiagnosticSession=3
} em_diagnosticSessionType;
#define C_SID_ReadDataByIndentifier 	0x22
#define C_SID_RoutineControl			0x31
#define C_SID_ControlDTCSetting			0x85
typedef enum {
	DTCSettingOn=1
	,DTCSettingOff=2
} em_DTCSettingType;
#define C_SID_CommunicationControl		0x28
typedef enum {
	enableRxAndTx=0
	,enableRxAndDisableTx=1
	,disableRxAndEnableTx=2
	,disableRxAndTx=3
} em_ControlType;
typedef enum {
	communicationReserved=0
	,normalCommunicationMessages=1
	,networkManagementCommunicationMessages=2
	,bothCommunicationMessages=3
} em_CommunicationType;
#define C_SID_SecurityAccess			0x27
typedef enum {
	securityAccessReserved=0
	,requestSeedLevel1=1
	,sendKeyLevel1=2
	,requestSeedLevel2=3
	,sendKeyLevel2=4
	,requestSeedLevel=11
	,sendKeyLevel=12
} em_SecurityAccessType;
#define C_SID_WriteDataByIndentifier	0x2E
#define C_SID_EcuReset					0x10
typedef enum {
	hardReset=1
	,keyOffOnReset=2
	,softReset=3
} em_ResetType;
#define C_SID_RequestDownload			0x34
#define C_SID_TransferData				0x36
#define C_SID_RequestTransferExit		0x37
#define C_CONFIG_FILE_NAME				_T("config.ini")

typedef struct apdu_response {
	UINT			aai;
	unsigned char	apci;
	unsigned short  alength;
	unsigned char	adata[7];
}st_apdu_response;

// can
typedef struct can_msg {
	unsigned long	u32_id;
	unsigned char	u8_len;			
	unsigned char	u8_msgDat[8];
}st_can_msg;

#include "DeviceCAN.h"
#include "S19.h"
#include "Bin.h"
#include "TPprotocol.h"
#include "UDSprotocol.h"
#include "prog.h"
#include "Config.h"

extern CDeviceCAN DeviceCan;
extern int g_module;
extern CBin g_binDownload;

// config
extern int G_MEM_ST;
extern int G_MEM_ED;
extern int G_N_As;
extern int G_N_Ar;
extern int G_N_Bs;
extern int G_N_Br;
extern int G_N_Cs;
extern int G_N_Cr;
extern int G_P2_CLIENT;