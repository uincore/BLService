#pragma once

#include "prog.h"

typedef struct bin_segment {
	UINT32 address;
	UINT32 length;
}st_bin_segment;

typedef struct bin_segment_data {
	unsigned char d[MAX_ADDRESS];	// data
}st_bin_segment_data;

typedef struct bin_data {
	unsigned char dcid[3];
	unsigned char swv[3];
	unsigned char swpn[14];
	unsigned char wv[3];
	unsigned char system_name[9];
	unsigned char noar;
}st_bin_data;



class CBin
{
public:
	st_bin_data m_bindata; 
	st_bin_segment m_segment[MAX_LENGTH]; 
	st_bin_segment_data m_segment_data[MAX_LENGTH]; 
	unsigned int m_lenth;
	unsigned m_maxlen;
public:
	CBin(void);
	~CBin(void);
	unsigned int GetTotal(void);
	int read_bin(char *fn);
};

