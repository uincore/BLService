#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

#if UIN15765_DEBUG_TRACE
	#define UIN15765_DEBUG_TRACE(format,...) \
		printf(format, ## __VA_ARGS__) 
#else
	#define UIN15765_DEBUG_TRACE(format,...) 
#endif

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

//Config CHANNEL
#define UIN15765CFG_CHANNEL		0
//Config CHANNEL
#define UIN15765CFG_BAUD_RATE	BAUD_250K

typedef struct{
	uint32_t id;
	uint8_t buf[8];
	uint8_t buf_len;
	
}can_t;

int g_canhandle;

void can_init(void);			//Initializes CAN hardware
void* can_update(void* args);	//
uint8_t can_rx(can_t *msg);	//Receives CAN frame (buffered I/O)
uint8_t can_tx(can_t *msg);	//Transmits CAN frame (buffered I/O)