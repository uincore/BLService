#pragma once

#include "stdafx.h"
#include "sysdep.h"

#define PC_NODE_ID 0x45
#define MAX_NUM_BLOCKS	 9// max. no. of Flash blocks
#define SDID_UNDEF		-1	// end marker in SDID properties table
#define MAX_ADDRESS	0x3EC00
#define MAX_LENGTH		512//block max size 512byte

#define MODULE_PANEL_ID      1
#define MODULE_FRONT_ID      10
#define MODULE_TOP_ID          13
#define MODULE_BEHIND_ID    16
#define MODULE_SWITCH1_ID 30
#define MODULE_SWITCH2_ID 31
#define MODULE_SWITCH3_ID 32
#define MODULE_SWITCH4_ID 33
#define MODULE_SWITCH5_ID 34
#define MODULE_SWITCH6_ID 35
#define MODULE_NUM              10

typedef enum {CONNECTSTEP_INIT,CONNECTSTEP_BOOTUP, CONNECTSTEP_OK} ConnectStep_t;
typedef enum {PROGSTEP_INIT,PROGSTEP_ADDRESS,PROGSTEP_REQ,PROGSTEP_START,PROGSTEP_END,PROGSTEP_CRC,PROGSTEP_OK,PROGSTEP_ABORT} ProgStep_t;

typedef struct
{
	unsigned long mem_start[MAX_NUM_BLOCKS];// start of usable flash
	unsigned long mem_end[MAX_NUM_BLOCKS];	// end of usable flash
	unsigned long erblk;			// erase block size
	unsigned long wrblk;			// write block size
	unsigned long bl_tbl;		// start of boot-loader table (private data and jump table)
	unsigned long int_vect_tbl;	// start of hard-wired interrupt vectors
	unsigned char priv_data[8];		// 8 bytes of private info
	unsigned char targ_name[40];		// target name
	unsigned num_blocks;	// number of flash memory blocks (BL protocol version 2 and up)
	unsigned sdid;			// SDID number   (BL protocol version 2 and up)
	unsigned sdidrev;		// SDID revision (BL protocol version 2 and up)
} IDENT_DATA;

class  CProg
{
public:
	CProg();
	~CProg();
private:
	static const char clientId_index[MODULE_NUM];
	unsigned int m_progress;
	unsigned int m_progress_parts;
	int m_ConnectState;
	IDENT_DATA m_ident;
public:
	st_can_msg m_msg;
public:
	//void * ReserBuffer(void *pBufferData,unsigned int nLen);
	int Get_Module();
	void Set_Module(int module);
	int Connect_detect();
	void DisConnect();
	int GetConnectState();
	void SetConnectState(int connect);
	void Progress_Reset(unsigned progressParts);
	void Progress_Set(unsigned progressPart, unsigned locProgress);
	static unsigned umin(unsigned a, unsigned b)
	{
		return a < b ? a : b;
	}
	int check_image();
	int erase_ok();

	int OpenCAN();
	void StartReceive();
	//
	int preProgramming();
	int programming();
	int postProgramming();
	int downloadingFlashDriver();
	int downloading();
};

extern CProg prog;
