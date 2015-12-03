
#pragma once

#include "stdafx.h"
#include "sysdep.h"
#include "prog.h"

typedef struct {
	unsigned char d[MAX_ADDRESS];	// data
	unsigned char f[MAX_ADDRESS];	// valid flag 0=empty; 1=usercode; 2=systemcode
} BOARD_MEM;


class S19
{
// ππ‘Ï
public:
	S19();
	~S19();
private:
	static char m_buf[512];
	static unsigned short m_s19_crc;
	static unsigned int m_total;
	static unsigned m_maxlen;
	static unsigned m_warningCnt;
public:
	static void CRC_AddByte(unsigned short *pCrc, unsigned char data);
	static void ResetCRC(unsigned short *pCrc, unsigned short seed);
	static unsigned short GetCRC(unsigned short *pCrc);
	static unsigned short S19_GetCRC(void);
	static unsigned int GetTotal(void);
	static void reset_S19image(BOARD_MEM* pImage);
	static int read_s19(char *fn, BOARD_MEM* pImage);
	static int write_s19(char *fn, BOARD_MEM* pImage);
};

