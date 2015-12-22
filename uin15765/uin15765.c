#include <time.h>
#include <canlib.h>
#include "uin15765cfg.h"
#include "uin15765.h"
#include "uin15765tp.h"
#include "uin15765app.h"

//Initializes protocol stack
void i15765_init(void)
{
	// init buf
	pthread_mutex_init(&m_section_rx,NULL); 
	pthread_mutex_init(&m_section_tx,NULL);
	//
	pthread_create(&thread, NULL, i15765_update, NULL);
}

i15765_session_t* getFreeRxBuf()
{
	int i = 0;
	i15765_session_t* rt = NULL;
	pthread_mutex_lock(&m_section_rx);
	for (i=0;i<UIN15765CFG_MF_RX_BUF_NUM;i++)
	{
		if(m_buf_rx[i].state == TP_FREE)
		{
			rt = &m_buf_rx[i];
			break;
		}
	}
	pthread_mutex_unlock(&m_section_rx);
	return rt;
}

i15765_session_t* getFreeTxBuf()
{
	int i = 0;
	i15765_session_t* rt = NULL;
	pthread_mutex_lock(&m_section_tx);
	for (i=0;i<UIN15765CFG_MF_TX_BUF_NUM;i++)
	{
		if(m_buf_tx[i].state == TP_FREE)
		{
			rt = &m_buf_tx[i];
			break;
		}
	}
	pthread_mutex_unlock(&m_section_tx);
	return rt;
}

i15765_session_t* getRxBufInTPRX(uint32_t id)
{
	int i = 0;
	i15765_session_t* rt = NULL;
	pthread_mutex_lock(&m_section_rx);
	for (i=0;i<UIN15765CFG_MF_RX_BUF_NUM;i++)
	{
		if(m_buf_rx[i].state == TP_RX_DT && m_buf_rx[i].id == id)
		{
			rt = &m_buf_rx[i];
			break;
		}
	}
	pthread_mutex_unlock(&m_section_rx);
	return rt;
}

//1: No CAN frame was written to the transmit buffer.
//0: The CAN frame was successfully written to the transmit buffer.
uint8_t sendFC(int buf_len,int point,uint8_t fs)
{
	can_t data;
	data.buf_len = 3;
	data.buf[0] = ((NPDU_FLOWCONTROL << 4) & 0xf0) + fs;
	if(buf_len-point>UIN15765CFG_FC_BS)
	{
		data.buf[1] = UIN15765CFG_FC_BS;
	}else{
		data.buf[1] = 0;
	}
	data.buf[2] = UIN15765CFG_FC_ST_MIN;
	return can_tx(&data);
}

i15765_session_t* getTxBufInTPTX(uint8_t sa,uint8_t ta)
{
	int i = 0;
	i15765_session_t* rt = NULL;
	pthread_mutex_lock(&m_section_tx);
	for (i=0;i<UIN15765CFG_MF_TX_BUF_NUM;i++)
	{
		if(m_buf_tx[i].state == TP_TX_DT && m_buf_tx[i].data.sa == ta && m_buf_tx[i].data.ta == sa)
		{
			rt = &m_buf_tx[i];
			break;
		}
	}
	pthread_mutex_unlock(&m_section_tx);
	return rt;
}

// 必须先启动接收线程 i15765_update
void sendDataWithFC(npdu_flow_control_t *ctrl)
{
	can_t canbuf;
	int index = 0;
	int bs = 0;
	int i = 0;
	uint8_t sa = (ctrl->id >> 24) & 0xFF;
	uint8_t ta = (ctrl->id >> 16) & 0xFF;
	npdu_consecutive_frame_t more;
	time_t timest;
	i15765_session_t* s = getTxBufInTPTX(sa,ta);
	time(&timest);
	// 超时
	if(timest-s->time>UIN15765CFG_TX_TIMEOUT){
		s->state = TP_TX_TIMEOUT;
		i15765app_tx_completed_process(&s->data,&s->state);
		s->state = TP_FREE;
		return;
	}
	canbuf.id = s->id;
	if(ctrl->fs == 0)
	{
		// send rx wait
		// send ff
		if(ctrl->bs == 0)
		{
			bs = 1 + (s->data.buf_len - s->point) / 7;
		}else{
			bs = ctrl->bs;
		}
		for(i=0;i<bs;i++)
		{
			// more
			s->last_seq++;
			if(s->last_seq>0x0F) s->last_seq = 0;
			more.sn = s->last_seq;
			memcpy(canbuf.buf+1,s->data.buf+s->point,7);
			s->point += 7;
			can_tx(&canbuf);
			//time(&timest);
			Sleep(ctrl->stmin);
		}
	}
}

//Provides periodic time base
// 必须先连接
void* i15765_update(void* args)
{
	can_t data;
	int len = 0;
	uint8_t n_pcitype,seq;
	i15765_t frame;
	int sleepperoid = UIN15765CFG_TICK_PERIOD/10;
	i15765_session_t* s;
	npdu_flow_control_t ctrl;
	while(1)
	{
		if(can_rx(&data) == 0 && data.buf_len>1)
		{
			frame.sa = (data.id >> 24) & 0xff;
			frame.ta = (data.id >> 16) & 0xff;
			frame.ae = (data.id >> 8) & 0xff;
			frame.tat = data.id & 0xff;
			frame.pri = 0;
			n_pcitype = (data.buf[0] >> 4) & 0x0f;
			switch(n_pcitype){
			case NPDU_SINGLE:
				frame.buf_len = data.buf[0] & 0x0f;
				memcpy(frame.buf,data.buf+1,frame.buf_len);
				i15765app_process(&frame);
				break;
			case NPDU_MULT:
				s = getFreeRxBuf();
				if(s==NULL)
				{
					UIN15765_DEBUG_TRACE("%s rx overflow!\n\r",  __FUNCTION__ );
				}
				else
				{
					s->state = TP_RX_DT;
					s->id = data.id;
					s->last_seq = 0;
					memcpy(frame.buf,data.buf+2,6);
					memcpy(&s->data,&frame,I15765_ST_SIZE);
					s->point = 6;
					s->data.buf_len = ((data.buf[0] & 0x0f) << 8) + data.buf[1];
					time(&s->time);
					// send flow contral
					if(sendFC(s->data.buf_len,s->point,FS_CTS)!=0)
					{
						UIN15765_DEBUG_TRACE("%s send fc failed!\n\r",  __FUNCTION__ );
						s->state = TP_FREE;
					}
				}

				break;
			case NPDU_MORE:
				s = getRxBufInTPRX(data.id);
				if(s==NULL)
				{
					UIN15765_DEBUG_TRACE("%s rx overflow!\n\r",  __FUNCTION__ );
				}
				else
				{
					if(data.buf_len - 1 +s->point > UIN15765CFG_MF_RX_BUF_SIZE)
					{
						s->state = TP_FREE;
						UIN15765_DEBUG_TRACE("%s rx buf overflow!\n\r",  __FUNCTION__ );
					}else{
						seq = data.buf[0] & 0x0f;
						if(s->last_seq++>0x0f) s->last_seq = 0;
						if(s->last_seq == seq)
						{
							memcpy(s->data.buf + s->point,data.buf+1,7);
							s->point += 7;
							// m_buf_rx[i].timeout = data.time;
							if(s->point>=s->data.buf_len)
							{
								s->state = TP_RX_COMPLETED;
								i15765app_process(&s->data);
								s->state = TP_FREE;
							}
						}else{
							s->state = TP_FREE;
							UIN15765_DEBUG_TRACE("%s rx error seq!\n\r",  __FUNCTION__ );
						}
					}
				}
				break;
			case NPDU_FLOWCONTROL:
				ctrl.id = data.id;
				ctrl.fs = data.buf[0] & 0x0f;
				ctrl.bs = data.buf[1];
				ctrl.stmin = data.buf[2];
				sendDataWithFC(&ctrl);
				break;
			default:
				break;
			}
		}
		Sleep(sleepperoid);
	}
}

void i15765_tx(i15765_t *frame, uint8_t *status)
{
	int rt =0;
	int i = 0;
	can_t canbuf;
	i15765_session_t* s;
	if(frame->buf_len<8){
		canbuf.buf_len = frame->buf_len +1;
		canbuf.buf[0] = (frame->buf_len & 0xFF);
		memcpy(canbuf.buf+1,frame->buf,frame->buf_len);
		if(can_tx(&canbuf) == 0)
		{
			*status = TP_TX_DT;
		}
		else
		{
			*status = TP_TX_CAN_FAIL;
		}
	}else if(frame->buf_len > UIN15765CFG_MF_TX_BUF_SIZE)
	{
		*status = TP_TX_BUF_OVERFLOW;
	}else{
		s = getFreeTxBuf();
		if(s==NULL)
		{
			*status = TP_TX_OVERFLOW;
		}else{
			memcpy(&s->data,frame,I15765_ST_SIZE);
			s->state = TP_TX_DT;
			// first
			canbuf.buf[0] = (NPDU_MULT << 4) & ((frame->buf_len >> 8) & 0x0F);
			canbuf.buf[1] = frame->buf_len & 0xFF;
			memcpy(canbuf.buf+2,frame->buf,6);
			if(can_tx(&canbuf) == 0)
			{
				s->point = 6;
				s->last_seq = 0;
				time(&s->time);
				*status = TP_TX_DT;

			}else
			{
				s->state = TP_FREE;
				*status = TP_TX_CAN_FAIL;
			}
		}
	}
}