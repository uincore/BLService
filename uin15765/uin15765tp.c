#include <canlib.h>
#include "uin15765tp.h"
#include "uin15765cfg.h"

//Initializes CAN hardware
void can_init(void)
{
	int stat;

	// cn open
	g_canhandle = canOpenChannel(UIN15765CFG_CHANNEL, canOPEN_EXCLUSIVE);

	if (g_canhandle < 0) {
		UIN15765_DEBUG_TRACE("%s Open channel failed!\n\r",  __FUNCTION__ );
	}
	stat = canSetBusParams(g_canhandle, UIN15765CFG_BAUD_RATE, 0, 0, 1, 1, 0);

	if (stat < 0) 
	{
		UIN15765_DEBUG_TRACE("%s Baud rate failed!\n\r",  __FUNCTION__ );
	}
	stat = canBusOn(g_canhandle);
	if (stat < 0) {
		UIN15765_DEBUG_TRACE("%s Bus on failed!\n\r",  __FUNCTION__ );
	}
	//j1939tp_rx_session_init();
	//j1939tp_tx_session_init();
	//tmr_start(&j1939_loop_tmr, J1939CFG_TICK_PERIOD);
	//j1939_claim_init();
	UIN15765_DEBUG_TRACE("%s OK!\n\r",  __FUNCTION__ );
}

//Receives CAN frame (buffered I/O)
//1: No CAN frame was read from the receive buffer.
//0: A CAN frame was successfully read from the receive buffer.
uint8_t can_rx(can_t *msg)
{
	uint8_t rt = 0;
	long id;
	unsigned int flag;
	unsigned long time;
	unsigned int len;
	if(canReadWait(g_canhandle, &id, msg->buf, &len, &flag, &time,100) == canOK)
	{
		msg->id = id;
		msg->buf_len = len;
	}else{
		rt = 1;
	}
	return rt;
}

//Transmits CAN frame (buffered I/O)
//1: No CAN frame was written to the transmit buffer.
//0: The CAN frame was successfully written to the transmit buffer.
uint8_t can_tx(can_t *msg)
{
	return 1;
}