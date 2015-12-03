#include "StdAfx.h"
#include "UDSprotocol.h"

CTPprotocol CUDSprotocol::m_tp;

CUDSprotocol::CUDSprotocol(void)
{
}

CUDSprotocol::~CUDSprotocol(void)
{
}

int CUDSprotocol::SendUDS(unsigned char apci,unsigned char *adata,int alength)
{
	unsigned char msg[MAX_LENGTH];
	msg[0] = apci;
	if(alength>0)
	{
		memcpy(msg+1,adata,alength);
	}
	return m_tp.Send(g_module,msg,alength+1);
}

int CUDSprotocol::GetResponse(st_apdu_response &res)
{
	st_can_msg msg;
	if(DeviceCan.GetMsg(msg) != -1)
	{
		res.aai = msg.u32_id;
		res.apci = msg.u8_msgDat[0];
		memcpy(res.adata,msg.u8_msgDat+1, msg.u8_len - 1);
		res.alength = msg.u8_len - 1;
		return 0;
	}
	return -1;
}

int CUDSprotocol::DiagnosticSessionControl(em_diagnosticSessionType type)
{
	unsigned char msg[1] = {type};
	return SendUDS(C_SID_DiagnosticSessionControl,msg,1);
}

int CUDSprotocol::ReadDataByIndentifier(unsigned char *msg,int length)
{
	return SendUDS(C_SID_ReadDataByIndentifier,msg,length);
}

int CUDSprotocol::RoutineControl(unsigned char *msg,int length)
{
	return SendUDS(C_SID_RoutineControl,msg,length);
}

int CUDSprotocol::ControlDTCSetting(em_DTCSettingType type)
{
	unsigned char msg[1] = {type};
	return SendUDS(C_SID_ControlDTCSetting,msg,1);
}

int CUDSprotocol::CommunicationControl(em_ControlType contralType,em_CommunicationType commType)
{
	unsigned char msg[2] = {contralType,commType};
	return SendUDS(C_SID_CommunicationControl,msg,2);
}

int CUDSprotocol::SecurityAccess(em_SecurityAccessType type,unsigned char *msg,int length)
{
	unsigned char nmsg[MAX_LENGTH];
	nmsg[0] = type;
	if(length>0)
	{
		memcpy(nmsg+1,msg,length);
	}
	return SendUDS(C_SID_SecurityAccess,nmsg,length+1);
}

int CUDSprotocol::WriteDataByIndentifier(unsigned char *msg,int length)
{
	return SendUDS(C_SID_WriteDataByIndentifier,msg,length);
}

int CUDSprotocol::RequestDownload(unsigned char dataFormatIndentifier,UINT address,UINT size)
{
	unsigned char msg[9];
	msg[0] = dataFormatIndentifier;
	memcpy(msg+1,&address,4);
	memcpy(msg+5,&size,4);
	return SendUDS(C_SID_RequestDownload,msg,9);
}

int CUDSprotocol::TransferData(unsigned char seq,unsigned char *data,int length)
{
	unsigned char nmsg[MAX_LENGTH];
	nmsg[0] = seq;
	if(length>0)
	{
		memcpy(nmsg+1,data,length);
	}
	return SendUDS(C_SID_TransferData,nmsg,length+1);
}

int CUDSprotocol::RequestTransferExit()
{
	return SendUDS(C_SID_RequestTransferExit,NULL,0);
}

int CUDSprotocol::EcuReset(em_ResetType type)
{
	unsigned char msg[1] = {type};
	return SendUDS(C_SID_EcuReset,msg,1);
}