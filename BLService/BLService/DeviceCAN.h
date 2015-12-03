#pragma once


#include "stdafx.h"
#include "sysdep.h"

//接口卡类型定义
#define ZLG_USBCAN	4//ZLG usb can
#define PCAN			6//PCAN
#define KAVSER	8//ZLG usb can

//CAN错误码
#define	ERR_CAN_OVERFLOW			0x0001	//CAN控制器内部FIFO溢出
#define	ERR_CAN_ERRALARM			0x0002	//CAN控制器错误报警
#define	ERR_CAN_PASSIVE			0x0004	//CAN控制器消极错误
#define	ERR_CAN_LOSE				0x0008	//CAN控制器仲裁丢失
#define	ERR_CAN_BUSERR			0x0010	//CAN控制器总线错误

//通用错误码
#define	ERR_DEVICEOPENED			0x0100	//设备已经打开
#define	ERR_DEVICEOPEN			0x0200	//打开设备错误
#define	ERR_DEVICENOTOPEN			0x0400	//设备没有打开
#define	ERR_BUFFEROVERFLOW		0x0800	//缓冲区溢出
#define	ERR_DEVICENOTEXIST		0x1000	//此设备不存在
#define	ERR_LOADKERNELDLL			0x2000	//装载动态库失败
#define ERR_CMDFAILED				0x4000	//执行命令失败错误码
#define	ERR_BUFFERCREATE			0x8000	//内存不足


//函数调用返回状态值
#define	STATUS_OK					1
#define STATUS_ERR					0
	
#define CMD_DESIP					0
#define CMD_DESPORT				1
#define CMD_CHGDESIPANDPORT		2


//1.ZLGCAN系列接口卡信息的数据类型。
typedef  struct  _VCI_BOARD_INFO{
		USHORT	hw_Version;
		USHORT	fw_Version;
		USHORT	dr_Version;
		USHORT	in_Version;
		USHORT	irq_Num;
		BYTE	can_Num;
		CHAR	str_Serial_Num[20];
		CHAR	str_hw_Type[40];
		USHORT	Reserved[4];
} VCI_BOARD_INFO,*PVCI_BOARD_INFO; 

//2.定义CAN信息帧的数据类型。
typedef  struct  _VCI_CAN_OBJ{
	UINT	ID;
	UINT	TimeStamp;
	BYTE	TimeFlag;
	BYTE	SendType;
	BYTE	RemoteFlag;//是否是远程帧
	BYTE	ExternFlag;//是否是扩展帧
	BYTE	DataLen;
	BYTE	Data[8];
	BYTE	Reserved[3];
}VCI_CAN_OBJ,*PVCI_CAN_OBJ;

//3.定义CAN控制器状态的数据类型。
typedef struct _VCI_CAN_STATUS{
	UCHAR	ErrInterrupt;
	UCHAR	regMode;
	UCHAR	regStatus;
	UCHAR	regALCapture;
	UCHAR	regECCapture; 
	UCHAR	regEWLimit;
	UCHAR	regRECounter; 
	UCHAR	regTECounter;
	DWORD	Reserved;
}VCI_CAN_STATUS,*PVCI_CAN_STATUS;

//4.定义错误信息的数据类型。
typedef struct _VCI_ERR_INFO{
		UINT	ErrCode;
		BYTE	Passive_ErrData[3];
		BYTE	ArLost_ErrData;
} VCI_ERR_INFO,*PVCI_ERR_INFO;

//5.定义初始化CAN的数据类型
typedef struct _VCI_INIT_CONFIG{
	DWORD	AccCode;
	DWORD	AccMask;
	DWORD	Reserved;
	UCHAR	Filter;
	UCHAR	Timing0;	
	UCHAR	Timing1;	
	UCHAR	Mode;
}VCI_INIT_CONFIG,*PVCI_INIT_CONFIG;

typedef struct _tagChgDesIPAndPort
{
	char szpwd[10];
	char szdesip[20];
	int desport;
}CHGDESIPANDPORT;

// Function pointers to load ZLG USB CAN functions
//
typedef  DWORD (__stdcall *VCI_OpenDevice)(DWORD,DWORD,DWORD);
typedef  DWORD (__stdcall *VCI_CloseDevice)(DWORD,DWORD);
typedef  DWORD (__stdcall *VCI_InitCAN)(DWORD, DWORD, DWORD, PVCI_INIT_CONFIG);
typedef  DWORD (__stdcall *VCI_ReadBoardInfo)(DWORD,DWORD,PVCI_BOARD_INFO);
typedef  DWORD (__stdcall *VCI_ReadErrInfo)(DWORD,DWORD,DWORD,PVCI_ERR_INFO);
typedef  DWORD (__stdcall *VCI_ReadCANStatus)(DWORD,DWORD,DWORD,PVCI_CAN_STATUS);
typedef  DWORD (__stdcall *VCI_GetReference)(DWORD,DWORD,DWORD,DWORD,PVOID);
typedef  DWORD (__stdcall *VCI_SetReference)(DWORD,DWORD,DWORD,DWORD,PVOID);
typedef  DWORD (__stdcall *VCI_GetReceiveNum)(DWORD,DWORD,DWORD);
typedef  DWORD (__stdcall *VCI_ClearBuffer)(DWORD,DWORD,DWORD);
typedef  DWORD (__stdcall *VCI_StartCAN)(DWORD,DWORD,DWORD);
typedef  DWORD (__stdcall * VCI_ResetCAN)(DWORD,DWORD,DWORD);
typedef  DWORD (__stdcall *VCI_Receive)(DWORD,DWORD,DWORD,PVCI_CAN_OBJ,ULONG,INT);
typedef  DWORD (__stdcall *VCI_Transmit)(DWORD,DWORD,DWORD,PVCI_CAN_OBJ,DWORD);


//===============================PCAN ==========================================
enum Baudrates
{
    BAUD_1M		= 0x0014,  //   1 MBit/s
    BAUD_500K	= 0x001C,  // 500 kBit/s
    BAUD_250K	= 0x011C,  // 250 kBit/s
    BAUD_125K	= 0x031C,  // 125 kBit/s
    BAUD_100K	= 0x432F,  // 100 kBit/s
    BAUD_50K	= 0x472F,  //  50 kBit/s
    BAUD_20K	= 0x532F,  //  20 kBit/s
    BAUD_10K	= 0x672F,  //  10 kBit/s
    BAUD_5K		= 0x7F7F,  //   5 kBit/s
};

enum CANResult
{
	ERR_OK				= 0x0000,	// No error
	ERR_XMTFULL			= 0x0001,   // Send buffer of the Controller ist full
	ERR_OVERRUN			= 0x0002,   // CAN-Controller was read to late
	ERR_BUSLIGHT		= 0x0004,   // Bus error: an Error count reached the limit
	ERR_BUSHEAVY		= 0x0008,   // Bus error: an Error count reached the limit
	ERR_BUSOFF			= 0x0010,   // Bus error: CAN_Controller went to 'Bus-Off'
	ERR_QRCVEMPTY		= 0x0020,   // RcvQueue is empty
	ERR_QOVERRUN		= 0x0040,   // RcvQueue was read to late
	ERR_QXMTFULL		= 0x0080,   // Send queue is full
	ERR_REGTEST			= 0x0100,   // RegisterTest of the 82C200/SJA1000 failed
	ERR_NOVXD			= 0x0200,   // Problem with Localization of the VxD    
	ERR_ILLHW			= 0x1400,   // Invalid Hardware handle
	ERR_RESOURCE		= 0x2000,   // Not generatably Resource (FIFO, Client, Timeout)
	ERR_PARMTYP			= 0x4000,   // Parameter not permitted
	ERR_PARMVAL			= 0x8000,   // Invalid Parameter value
	ERRMASK_ILLHANDLE	= 0x1C00,   // Mask for all Handle errors
	ERR_ANYBUSERR		= ERR_BUSLIGHT | ERR_BUSHEAVY | ERR_BUSOFF, // All others error status <> 0 please ask by PEAK ......intern Driver errors.....
	ERR_NO_DLL			= 0xFFFFFFFF// A Dll could not be loaded or a function was not found into the Dll
};

typedef struct {
    DWORD ID;        // 11/29 Bit-ID
    BYTE  MSGTYPE;   // Kind of Message
    BYTE  LEN;       // Number of Data bytes (0..8)
    BYTE  DATA[8];   // Data bytes 0..7
} TPCANMsg;

// Kind of CAN Message
//
enum MsgTypes
{
    MSGTYPE_STANDARD	= 0x00,		// Standard Frame (11 bit ID)
    MSGTYPE_RTR			= 0x01,		// Remote request
    MSGTYPE_EXTENDED	= 0x02,		// CAN 2.0 B Frame (29 Bit ID)
    MSGTYPE_STATUS		= 0x80,		// Status Message
};

typedef  DWORD (__stdcall *InitPAP)(WORD, int);											// Init Plug And Play
typedef DWORD (__stdcall *InitNPAP)(WORD, int, int, DWORD, WORD);						// Init Non Plug And Play
typedef DWORD (__stdcall *C_S_RC_RF)();                                                 // Close,Status,ResetClient,ResetFilter
typedef DWORD (__stdcall *ReadWrite)(TPCANMsg*);                                        // CAN_Read CAN_Write
typedef DWORD (__stdcall *USBSDevNr)(long);			                                    // SetUSBDeviceNumber
typedef DWORD (__stdcall *USBGDevNr)(long*);				                            // GetUSBDeviceNumber
typedef DWORD (__stdcall *MessageFilter)(DWORD, DWORD, int);							// Set MsgFilter
typedef DWORD (__stdcall *GetInfo)(LPSTR);                                              // GetDllVersion, VersionInfo
typedef DWORD (__stdcall *spcFunct)(unsigned long, int);                                // SpecialFunktion
//==============================================================================


#define MAX_PACKAGE 512

class MsgBuffer
{
public:
	MsgBuffer();
	virtual ~MsgBuffer();
	void Push(VCI_CAN_OBJ& msgpg);
	bool Pop(VCI_CAN_OBJ& msgpg);
	void ClearBuff();
private:
	bool Readable();
	byte m_buf[sizeof(VCI_CAN_OBJ)*MAX_PACKAGE];
	VCI_CAN_OBJ* pWriteIndex;
	VCI_CAN_OBJ* pReadIndex;
	CRITICAL_SECTION m_section;
};


class CDeviceCAN
{
public:
	CDeviceCAN();
	~CDeviceCAN();
private:
	static HINSTANCE GetDllHandle(int HWType);
	int m_devicetype;
	int  m_deviceindex;
	int m_canindex;
	int m_canBaudrate;
	enum{USBCANCLOSE,USBCANOPEN};
	int m_USBCANState;
public: 
	MsgBuffer m_MsgBuffer;//缓冲对象
	int canhandle;
public: 
	int GetDevType();
	void SetDevType(int devtype);
	int GetCANBPS();
	void SetCANBPS(int canbps);
	int GetUSBCANConnectState();
 	void SetUSBCANConnectState(int connectstate);
	int InitCAN();
	int OpenCAN();
	int StartCAN();
	int CloseCAN();
	int ClearBuffer();
	int CAN_Send(VCI_CAN_OBJ *msg);
	void Push(VCI_CAN_OBJ p);
	void  StartReceive();
	static UINT CAN_ReceiveThread(void *param);
	int GetMsg(st_can_msg& msg);
};
