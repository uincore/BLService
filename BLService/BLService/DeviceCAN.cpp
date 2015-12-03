

#include "stdafx.h"
#include "kavser\canlib.h"

#pragma warning(disable : 4996)

#ifdef _WINDOWS

	int myprintf(const char * _Format, ...);
	int myfprintf(__inout FILE * _File, const char * _Format, ...);
	void progress_Changed(unsigned value);


	#define printf  myprintf
	#define fprintf myfprintf
#else
	#define progress_Changed(x)
#endif


MsgBuffer::MsgBuffer()
{
	pWriteIndex = (VCI_CAN_OBJ*)m_buf;//初始化指针，写指针指向缓冲区头
	pReadIndex = (VCI_CAN_OBJ*)m_buf;//初始化指针，读指针指向缓冲区头
	memset(m_buf,0,sizeof(VCI_CAN_OBJ)*MAX_PACKAGE);
	InitializeCriticalSection(&m_section);
	//InitializeCriticalSectionAndSpinCount(&m_section,2000);
}


MsgBuffer::~MsgBuffer()
{
	LeaveCriticalSection(&m_section);
	DeleteCriticalSection(&m_section);
}

void MsgBuffer::Push(VCI_CAN_OBJ& msgpg)
{
	EnterCriticalSection(&m_section);
	if (pWriteIndex < (VCI_CAN_OBJ*)m_buf + sizeof(m_buf)/sizeof(VCI_CAN_OBJ))
	{//防止越界
		*pWriteIndex = msgpg;//存入缓冲区
		pWriteIndex++;
	}
	else
	{
		pWriteIndex = (VCI_CAN_OBJ*)m_buf;
		*pWriteIndex = msgpg;//存入缓冲区
		pWriteIndex++;
	}
	LeaveCriticalSection(&m_section);
}


bool MsgBuffer::Pop(VCI_CAN_OBJ& msgpg)
{
	if(Readable())//是否有包可读
	{
		EnterCriticalSection(&m_section);
		if (pReadIndex > (VCI_CAN_OBJ*)m_buf + sizeof(m_buf)/sizeof(VCI_CAN_OBJ))
		{
			pReadIndex = (VCI_CAN_OBJ*)m_buf;
			msgpg = *pReadIndex;
			pReadIndex++;
		}
		else
		{
			msgpg = *pReadIndex;
			pReadIndex++;
		}
		LeaveCriticalSection(&m_section);
		return true;
	}
	return false;
}

void MsgBuffer::ClearBuff()
{
	EnterCriticalSection(&m_section);
	pWriteIndex = (VCI_CAN_OBJ*)m_buf;//初始化指针，写指针指向缓冲区头
	pReadIndex = (VCI_CAN_OBJ*)m_buf;//初始化指针，读指针指向缓冲区头
	memset(m_buf,0,sizeof(VCI_CAN_OBJ)*MAX_PACKAGE);
	LeaveCriticalSection(&m_section);
}


//******************************************************************
// FUNCTION:	-	Readable
// RETURNS:		-	
// PARAMETERS:	-	
// COMMENTS:	-	是否有数据可读
//******************************************************************
bool MsgBuffer::Readable()
{
	EnterCriticalSection(&m_section);
	if (pReadIndex != pWriteIndex)
	{
		LeaveCriticalSection(&m_section);
		return true;
	}
	LeaveCriticalSection(&m_section);
	return false;
}


CDeviceCAN::CDeviceCAN()
{
	m_devicetype = ZLG_USBCAN;
	m_deviceindex = 0;
	SetUSBCANConnectState(USBCANCLOSE);
	m_canindex = 1;//设置CAN 端口
}

CDeviceCAN::~CDeviceCAN()
{
	 HINSTANCE CANDll = NULL;
	 CloseCAN();
	CANDll = GetModuleHandle(TEXT("ControlCAN"));
	if(CANDll != NULL)
		FreeLibrary(CANDll);//释放动态库句柄
	 CANDll = GetModuleHandle(TEXT("PCAN_USB"));
	if(CANDll != NULL)
		FreeLibrary(CANDll);//释放动态库句柄
}


int CDeviceCAN::GetDevType()
{
	return m_devicetype;
}

void CDeviceCAN::SetDevType(int devtype)
{
	switch(devtype)
	{
		case 0:
			m_devicetype = KAVSER;
			break;
		case 1:
			m_devicetype = PCAN;
			break;
		case 2:
			m_devicetype = ZLG_USBCAN;
			break;
		default:
			m_devicetype = ZLG_USBCAN;
			break;
	}
}

 int CDeviceCAN::GetCANBPS()
 {
	return m_canBaudrate;
 }

 void CDeviceCAN::SetCANBPS(int canbps)
 {
	m_canBaudrate = canbps;
 }

 int CDeviceCAN::GetUSBCANConnectState()
 {
	return m_USBCANState;
 }

 void CDeviceCAN::SetUSBCANConnectState(int connectstate)
 {
	m_USBCANState = connectstate;
 }

HINSTANCE CDeviceCAN::GetDllHandle(int HWType)
{
    HINSTANCE CANDll = NULL;
	
    switch(HWType)
    {
        case ZLG_USBCAN:
		CANDll = GetModuleHandle(TEXT("ControlCAN"));
		return (CANDll == NULL) ? LoadLibrary(TEXT("ControlCAN")) : CANDll;
        case PCAN:
            	CANDll = GetModuleHandle(TEXT("PCAN_USB"));
            	return (CANDll == NULL) ? LoadLibrary(TEXT("PCAN_USB")) : CANDll;	
        default:
            	return NULL;
    }
}

int CDeviceCAN::InitCAN()
{
	HINSTANCE CANDll;
	
	if(m_devicetype == ZLG_USBCAN)
	{
		VCI_INIT_CONFIG InitInfo[1];
		InitInfo->Filter=3;
		InitInfo->AccCode=0;
		InitInfo->AccMask=0xFFFFFFFF;
		InitInfo->Mode=0;
		CANDll = GetDllHandle(m_devicetype);
		
		if(CANDll != NULL)
		{
			VCI_InitCAN pVCI_InitCAN;
			InitInfo->Timing0=0x01;//默认250K
			InitInfo->Timing1=0x1C;
		
			if(m_canBaudrate == 500)
			{
				InitInfo->Timing0=0x00;
				InitInfo->Timing1=0x1C;
			}

			//初始化通道
			pVCI_InitCAN = (VCI_InitCAN)GetProcAddress(CANDll,"VCI_InitCAN");
	        	if(pVCI_InitCAN != NULL)
	        	{
				if(pVCI_InitCAN(m_devicetype,m_deviceindex, m_canindex,InitInfo)  != STATUS_OK)
				{
					CloseCAN();
					printf("初始化 CAN 通道失败，请确定USB CAN是否支持两路CAN！");
					return STATUS_ERR;
				}
				return STATUS_OK;
			}
			else
				return STATUS_ERR;
		}
	}
	else if(m_devicetype == PCAN)
	{
		CANResult Res;
		CANDll = GetDllHandle(m_devicetype);
		
		if(CANDll != NULL)
		{
			InitPAP PCAN_Init;
			Baudrates PCANBaudrate;
			
			if(m_canBaudrate == 500)
			{
				PCANBaudrate = BAUD_500K;
			}
			else
			{
				PCANBaudrate = BAUD_250K;
			}

			PCAN_Init = (InitPAP)GetProcAddress(CANDll,"CAN_Init");

  			if(PCAN_Init != NULL)
  			{
				Res = (CANResult)PCAN_Init(PCANBaudrate,1);
				if(Res == ERR_OK)
					return STATUS_OK;
			}
			else
				return STATUS_ERR;

		}
	}
	else if(m_devicetype == KAVSER)
	{
		return STATUS_OK;
	}
	return STATUS_ERR;
}

int CDeviceCAN::StartCAN()
{
	HINSTANCE CANDll;
	if(m_devicetype == ZLG_USBCAN)
	{
		CANDll = GetDllHandle(m_devicetype);
		
		if(CANDll != NULL)
	    	{
	    		VCI_StartCAN pVCI_StartCAN;
				
	        	pVCI_StartCAN = (VCI_StartCAN)GetProcAddress(CANDll,"VCI_StartCAN");
				
	        	if(pVCI_StartCAN != NULL)
	        	{
				if(pVCI_StartCAN(m_devicetype,m_deviceindex, m_canindex) !=  STATUS_OK)
				{
					printf("打开 CAN 通道失败，请确定USB CAN是否支持两路CAN！");
					return STATUS_ERR;
				}
				return STATUS_OK;
			}
			else
				return STATUS_ERR;
		}
	}
	else if(m_devicetype == PCAN)
	{
		return STATUS_OK;
	}
	else if(m_devicetype == KAVSER)
	{
		return STATUS_OK;
	}
	return STATUS_ERR;
}
 
int CDeviceCAN::OpenCAN()
{
	if(GetUSBCANConnectState() == USBCANOPEN)
		return STATUS_OK;
	
	HINSTANCE CANDll;
	
	if(m_devicetype == ZLG_USBCAN)
	{
		CANDll = GetDllHandle(m_devicetype);

		if(CANDll != NULL)
	    	{
	    		VCI_OpenDevice pOpenDevice;
	        	pOpenDevice = (VCI_OpenDevice)GetProcAddress(CANDll,"VCI_OpenDevice");
	        	if(pOpenDevice != NULL)
	        	{
				if(pOpenDevice(m_devicetype,m_deviceindex,0) != STATUS_OK)
				{
					printf("打开USB CAN 失败,请检查USB CAN是否连接正常或者是否安装USB CAN驱动 !");
					return STATUS_ERR;
				}
			}
			else
				return STATUS_ERR;

			Sleep(100);
			
			if(InitCAN() == STATUS_OK)
			{
				Sleep(100);
				if(StartCAN() != STATUS_OK)
					return STATUS_ERR;
			}
			else 
				return STATUS_ERR;

			ClearBuffer();
			
			SetUSBCANConnectState(USBCANOPEN);
			
			return STATUS_OK;
	    	}
	}
	else if(m_devicetype == PCAN)
	{
		if(InitCAN() == STATUS_OK)
		{
			SetUSBCANConnectState(USBCANOPEN);
			return STATUS_OK;
		}
	}
	else if(m_devicetype == KAVSER){
		int stat;

	    canInitializeLibrary();
	    canhandle = canOpenChannel(0, canOPEN_EXCLUSIVE);
	    if (canhandle < 0) {
			AfxMessageBox("打开端口错误");	
			return STATUS_ERR;
		}
		Baudrates PCANBaudrate;
	  	if(m_canBaudrate == 500)
		{
			PCANBaudrate = BAUD_500K;
		}
		else
		{
			PCANBaudrate = BAUD_250K;
		}
		stat = canSetBusParams(canhandle, PCANBaudrate, 0, 0, 1, 1, 0);

		if (stat < 0) 
		{
			AfxMessageBox("设置参数错误");	
			 return STATUS_ERR;
		}
		 stat = canBusOn(canhandle);
		if (stat < 0) {
			AfxMessageBox("挂入总线错误");
			 return STATUS_ERR;
		}
		return STATUS_OK;
	}
	return STATUS_ERR;
}

int CDeviceCAN::ClearBuffer()
{
	HINSTANCE CANDll;

	CANDll = GetDllHandle(m_devicetype);

	if(CANDll != NULL)
	{
		VCI_ClearBuffer pVCI_ClearBuffer;
		
		pVCI_ClearBuffer = (VCI_ClearBuffer)GetProcAddress(CANDll,"VCI_ClearBuffer");
		
		if(pVCI_ClearBuffer != NULL)
		{//清空缓冲区
			if(pVCI_ClearBuffer(m_devicetype,m_deviceindex, m_canindex) != STATUS_OK)
				return STATUS_ERR;
			else
				return STATUS_OK;
		}
	}
	return STATUS_ERR;
}

int CDeviceCAN::CloseCAN()
{
	if(m_devicetype == KAVSER){
		(void)canBusOff(canhandle);
		canClose(canhandle);
		return STATUS_OK;
	}else{
		if(GetUSBCANConnectState() == USBCANCLOSE)
		return STATUS_OK;
	
		HINSTANCE CANDll;
	
		SetUSBCANConnectState(USBCANCLOSE);
		Sleep(200);
		CANDll = GetDllHandle(m_devicetype);
	
		if(CANDll != NULL)
		{
			VCI_CloseDevice pVCI_CloseDevice;
		
        		pVCI_CloseDevice = (VCI_CloseDevice)GetProcAddress(CANDll,"VCI_CloseDevice");

			if(pVCI_CloseDevice != NULL)
			{
				if(pVCI_CloseDevice(m_devicetype,m_deviceindex) != STATUS_OK)
					return STATUS_ERR;
				else
					return STATUS_OK;

			}
		}
	}
	return STATUS_ERR;
}


int CDeviceCAN::CAN_Send(VCI_CAN_OBJ *msg)
{
	if(m_devicetype == KAVSER){
		canWrite(canhandle, msg->ID, msg->Data, msg->DataLen, canMSG_EXT);
		return STATUS_OK;
	}else{
		HINSTANCE CANDll;

		CANDll = GetDllHandle(m_devicetype);
		if(CANDll != NULL)
		{
			if(m_devicetype == ZLG_USBCAN)
			{
				VCI_Transmit pVCI_Transmit;
			
				pVCI_Transmit = (VCI_Transmit)GetProcAddress(CANDll,"VCI_Transmit");

				if(pVCI_Transmit != NULL)
				{
					if(pVCI_Transmit(m_devicetype,m_deviceindex, m_canindex,msg,1) < 1)
					{
						printf("发送失败\n");
						return STATUS_ERR;
					}
					else
						return STATUS_OK;
				}
			}
			else if(m_devicetype == PCAN)
			{
				 ReadWrite PCan_Write;
   				 CANResult Res;
				 TPCANMsg MsgToSend;
			 
				MsgToSend.ID = msg->ID;
				MsgToSend.LEN = msg->DataLen;
				MsgToSend.MSGTYPE = MSGTYPE_EXTENDED;
				memcpy(MsgToSend.DATA,msg->Data,msg->DataLen);
				PCan_Write = (ReadWrite)GetProcAddress(CANDll,"CAN_Write");
        			if(PCan_Write != NULL)
        			{
        				Res = (CANResult)PCan_Write(&MsgToSend);
					if(Res == ERR_OK)
						return STATUS_OK;
				}
				else
					return STATUS_ERR;
			}
		}
	}
	return STATUS_ERR;
}


UINT CDeviceCAN::CAN_ReceiveThread(void *param)
{
	CDeviceCAN * self = (CDeviceCAN *)param; 

	if(self->m_devicetype == KAVSER){
		VCI_CAN_OBJ m_frameinfo;
	  unsigned int    j;
      long            id;
      unsigned char   data[8];
      unsigned int    dlc;
      unsigned int    flags;
      DWORD           time;

	  int           stat;
	  do
	  {
	    stat = canRead(self->canhandle, &id, &data[0], &dlc, &flags, &time);	 
		if (stat == canERR_NOMSG)
		{
			return 0;
		}
		if (stat == canOK) 
		{
			if(((id & 0xFFFFFF00) == 0x0FFF0E00) ||((id & 0xFFFFFF00) == 0x0FEFF900))
			{
				//memset(frameinfo,0,sizeof(frameinfo));
				m_frameinfo.ID = id;
				m_frameinfo.DataLen = dlc;
				memcpy(m_frameinfo.Data,data,dlc);
				self->m_MsgBuffer.Push(m_frameinfo);
			}
		}
	  }while(!(stat & canERR_NOMSG));
	}else{
		VCI_CAN_OBJ frameinfo[50];
		VCI_ERR_INFO errinfo;
		int i,len=1;
	
		HINSTANCE CANDll; 

		self->m_MsgBuffer.ClearBuff();
	
		CANDll = GetDllHandle(self->m_devicetype);
	
		if(CANDll == NULL)
			return 0;

		if(self->m_devicetype == ZLG_USBCAN)
		{
			VCI_Receive pVCI_Receive;
			VCI_ReadErrInfo pVCI_ReadErrInfo;
		
			pVCI_Receive = (VCI_Receive)GetProcAddress(CANDll,"VCI_Receive");
			pVCI_ReadErrInfo = (VCI_ReadErrInfo)GetProcAddress(CANDll,"VCI_ReadErrInfo");
			if(pVCI_Receive == NULL ||pVCI_Receive == NULL)
				return 0;
			
			while(1)
			{
				Sleep(1);
		
				if(self->GetUSBCANConnectState() == USBCANCLOSE)
					break;

				memset(frameinfo,0,sizeof(frameinfo));
			
				len=(pVCI_Receive)(self->m_devicetype,self->m_deviceindex,self->m_canindex,frameinfo,50,-1);

				if(len<=0)
					{
							//注意：如果没有读到数据则必须调用此函数来读取出当前的错误码，
							//千万不能省略这一步（即使你可能不想知道错误码是什么）
					(pVCI_ReadErrInfo)(self->m_devicetype,self->m_deviceindex,self->m_canindex,&errinfo);
					}
				 else
				 {
			 		for(i = 0; i < len; i++)
			 		{
						self->m_MsgBuffer.Push(frameinfo[i]);
					}
				 }
			
			}
		}
		else if(self->m_devicetype == PCAN)
		{
 			ReadWrite PCAN_Read;
			TPCANMsg MyMsg;
			memset(&MyMsg,0,sizeof(TPCANMsg));

			PCAN_Read = (ReadWrite)GetProcAddress(CANDll,"CAN_Read");
		
			if(PCAN_Read == NULL)
				return 0;

			while(1)
			{
				Sleep(1);
		
				if(self->GetUSBCANConnectState() == USBCANCLOSE)
					break;

				if((CANResult)PCAN_Read(&MyMsg) == ERR_OK)
				{
					if(MyMsg.ID>>16 ==0x0FEF)
					{
						memset(frameinfo,0,sizeof(frameinfo));
						frameinfo[0].ID = MyMsg.ID;
						frameinfo[0].DataLen = MyMsg.LEN;
						memcpy(frameinfo[0].Data,MyMsg.DATA,MyMsg.LEN);
						self->m_MsgBuffer.Push(frameinfo[0]);
					}
				}
			}
		}
	}
	
	//_endthread();
	return 0;
}


void  CDeviceCAN::StartReceive()
{
	 AfxBeginThread(CAN_ReceiveThread,this); 
}

int CDeviceCAN::GetMsg(st_can_msg& msg)
{
	VCI_CAN_OBJ canobj;
	if(DeviceCan.m_MsgBuffer.Pop(canobj))
	{	
		memcpy(&msg.u8_msgDat,canobj.Data,8);//取数据
		msg.u8_len = canobj.DataLen;
		msg.u32_id = canobj.ID;
		return 0;
	}
	return -1;
}
