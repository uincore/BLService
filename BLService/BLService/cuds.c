#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <canlib.h>

void init()
{
	pWriteIndex = (VCI_CAN_OBJ*)m_buf;//初始化指针，写指针指向缓冲区头
	pReadIndex = (VCI_CAN_OBJ*)m_buf;//初始化指针，读指针指向缓冲区头
	memset(m_buf,0,sizeof(VCI_CAN_OBJ)*MAX_PACKAGE);
	InitializeCriticalSection(&m_section);
	//InitializeCriticalSectionAndSpinCount(&m_section,2000);
		m_deviceindex = 0;
	SetUSBCANConnectState(USBCANCLOSE);
	m_canindex = 1;//设置CAN 端口
}


void del()
{
	CloseCAN();
	LeaveCriticalSection(&m_section);
	DeleteCriticalSection(&m_section);
}

void Push(VCI_CAN_OBJ& msgpg)
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


bool Pop(VCI_CAN_OBJ& msgpg)
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

void ClearBuff()
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
bool Readable()
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

int GetCANBPS()
 {
	return m_canBaudrate;
 }

 void SetCANBPS(int canbps)
 {
	m_canBaudrate = canbps;
 }

 int GetUSBCANConnectState()
 {
	return m_USBCANState;
 }

 void SetUSBCANConnectState(int connectstate)
 {
	m_USBCANState = connectstate;
 }

int InitCAN()
{
	return STATUS_OK;
}

int StartCAN()
{
	return STATUS_OK;
}
 
int OpenCAN()
{	
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

int ClearBuffer()
{
	return STATUS_ERR;
}

int CloseCAN()
{
	(void)canBusOff(canhandle);
	canClose(canhandle);
	return STATUS_OK;
}


int CAN_Send(VCI_CAN_OBJ *msg)
{
	canWrite(canhandle, msg->ID, msg->Data, msg->DataLen, canMSG_EXT);
	return STATUS_OK;
}

void  StartReceive()
{
	 AfxBeginThread(CTPprotocol::ReceiveThread,this); 
}

int GetMsg(st_can_msg& msg)
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
