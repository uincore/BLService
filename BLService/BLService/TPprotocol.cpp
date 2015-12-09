#include "StdAfx.h"
#include "TPprotocol.h"
#include "kavser\canlib.h"

CTPprotocol::CTPprotocol(void)
{

}


CTPprotocol::~CTPprotocol(void)
{
}

// 返回值
//  0:发送成功
//  -9:超时
int CTPprotocol::Send(UINT nai,unsigned char *data,UINT length)
{
	int rt =0;
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
		CTime timest = CTime::GetCurrentTime();
		CTime timeed;
		st_can_msg revmsg;
		st_npdu_flow_control npdufc;
		st_npdu_consecutive_frame more;
		more.npci_type = NPDU_MORE;
		int timeout = G_N_As + G_N_Bs;
		while(1)
		{
			timeed = CTime::GetCurrentTime();
			if(timeed-timest>timeout){
				rt = -9; // timeout
				break;
			}
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
							timest = CTime::GetCurrentTime();
							Sleep(npdufc.stmin);
						}
					}
				}
			}
		}
	}
	
	return rt;
}

UINT CTPprotocol::ReceiveThread(void *param)
{
	CDeviceCAN * self = (CDeviceCAN *)param; 

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

	return 0;
}