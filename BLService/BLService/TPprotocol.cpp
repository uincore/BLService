#include "StdAfx.h"
#include "TPprotocol.h"


CTPprotocol::CTPprotocol(void)
{
}


CTPprotocol::~CTPprotocol(void)
{
}

int CTPprotocol::Send(UINT nai,unsigned char *data,UINT length)
{
	unsigned char sn = 0;
	int index = 0;
	int bs = 0;
	VCI_CAN_OBJ canbuf;
	// 必须先连接
	// 必须先启动接收线程 StartReceive
	if(length<8){
		canbuf.ID = nai;
		canbuf.TimeStamp = 0;
		canbuf.TimeFlag = 0;
		canbuf.SendType = 0;
		canbuf.RemoteFlag = 0;
		canbuf.ExternFlag = 1;
		canbuf.Data[0] = length &0xFF;
		memcpy(canbuf.Data+1,data,7);
		DeviceCan.CAN_Send(&canbuf);
	}else{
		// first
		canbuf.ID = nai;
		canbuf.TimeStamp = 0;
		canbuf.TimeFlag = 0;
		canbuf.SendType = 0;
		canbuf.RemoteFlag = 0;
		canbuf.ExternFlag = 1;
		canbuf.Data[0] = (NPDU_MULT << 4) & ((length >> 8) & 0x0F);
		canbuf.Data[1] = length & 0xFF;

		if(length>6)
		{
			memcpy(canbuf.Data+2,data,6);
			index = 6;
		}
		else
		{
			memcpy(canbuf.Data+2,data,length);
		}
		DeviceCan.CAN_Send(&canbuf);
		st_can_msg revmsg;
		st_npdu_flow_control npdufc;
		st_npdu_consecutive_frame more;
		more.npci_type = NPDU_MORE;

		while(1)
		{
			if(DeviceCan.GetMsg(revmsg)==0)
			{
				memcpy(&npdufc,revmsg.u8_msgDat,3);
				if(revmsg.u32_id == nai && npdufc.npci_type == NPDU_FLOWCONTROL)
				{
					if(npdufc.fs == 0)
					{
						if(npdufc.bs == 0)
						{
							bs = 1 + (length - index) / 7;
						}else{
							bs = npdufc.bs;
						}
						for(int i=0;i<bs;i++)
						{
							// more
							sn++;
							if(sn>0x0F) sn = 0;
							more.sn = sn;
							if(index+7>length)
							{
								memcpy(canbuf.Data+1,data+index,length-index);
								index = length;
								break;
							}
							else
							{
								memcpy(canbuf.Data+1,data+index,7);
								index += 7;
							}
							DeviceCan.CAN_Send(&canbuf);
							Sleep(npdufc.stmin);
						}
					}
				}
			}
		}
	}
	
	return 0;
}
