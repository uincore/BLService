#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include "uin15765cfg.h"

#ifndef _UIN15765_DEF
#define _UIN15765_DEF

#define	NPDU_FLOWCONTROL                3
#define	NPDU_MORE						2
#define	NPDU_MULT						1
#define	NPDU_SINGLE						0

enum SessionState
{
    TP_FREE			= 0, 
    TP_RX_DT		= 0x01, 
    TP_RX_COMPLETED	= 0x03, 
    TP_TX_DT		= 0x11, 
    TP_TX_COMPLETED	= 0x13,
	TP_TX_OVERFLOW	= 0x16,
	TP_TX_BUF_OVERFLOW	= 0x17,
	TP_TX_TIMEOUT	= 0x18,
	TP_TX_CAN_FAIL	= 0x19
};

enum FlowControlFS
{
    FS_CTS			= 0, 
    FS_WT			= 0x01
};

typedef struct{
	uint8_t sa; // source address ->(id>>24) && FF
	uint8_t ta; // target address ->(id>>16) && FF
	uint8_t ae; // address extension ->(id>>8) && FF
	uint8_t tat; // target address type ->id && FF
	uint8_t pri; // priority of message 
	uint8_t *buf; // pointer to data 
	uint16_t buf_len;	// size of data (in bytes) 
} i15765_t;

#define I15765_ST_SIZE		sizeof(i15765_t)

typedef struct{
	uint8_t state;			//每个事务有该状态维护一个FSM
	uint32_t	id;			//提高代码效率，存放发送CAN报文的ID
	uint8_t		last_seq;		//对于接受事务是记录上一次接收到的数据包的序号
	i15765_t data;
	uint32_t point;
	time_t time;
} i15765_session_t;

typedef struct{
	uint32_t	id;			//提高代码效率，存放发送CAN报文的ID
	unsigned char	npci_type : 4;
	unsigned short	fs : 4;			
	unsigned char	bs;
	unsigned char	stmin;
} npdu_flow_control_t;

typedef struct{
	unsigned char	npci_type : 4;
	unsigned short	sn : 4;			
	unsigned char	data[7];
} npdu_consecutive_frame_t;

#endif

pthread_mutex_t m_section_rx;
pthread_mutex_t m_section_tx;
pthread_t thread;
i15765_session_t m_buf_rx[UIN15765CFG_MF_RX_BUF_NUM];
i15765_session_t m_buf_tx[UIN15765CFG_MF_TX_BUF_NUM];

void i15765_init(void);		//Initializes protocol stack
void* i15765_update(void* args);	//Provides periodic time base
void i15765_tx(i15765_t *frame, uint8_t *status);	//Transmits i15765 frame (buffered I/O)