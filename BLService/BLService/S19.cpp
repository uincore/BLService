

#include "stdafx.h"


#pragma warning(disable : 4996)

#ifdef _WINDOWS
	int myprintf(const char * _Format, ...);
	int myfprintf(__inout FILE * _File, const char * _Format, ...);
	void progress_Changed(unsigned value);
	#define printf  myprintf
	#define fprintf myfprintf
#else
	#define progress_Changed(x)
#endif


char S19::m_buf[512] = {0};
unsigned short S19::m_s19_crc = 0;
unsigned int S19::m_total = 0;
unsigned S19::m_maxlen = 0;
unsigned S19::m_warningCnt = 0;


S19::S19()
{
}

S19::~S19()
{
}

void S19::CRC_AddByte(unsigned short *pCrc, unsigned char data)
{
	unsigned short temp;
	unsigned short quick;
	unsigned short tmp_crc = *pCrc;

	temp = (tmp_crc >> 8) ^ data;
	tmp_crc <<= 8;
	quick = temp ^ (temp >> 4);
	tmp_crc ^= quick;
	quick <<= 5;
	tmp_crc ^= quick;
	quick <<= 7;
	tmp_crc ^= quick;

	*pCrc = tmp_crc;
}

void S19::ResetCRC(unsigned short *pCrc, unsigned short seed)
{
	*pCrc = seed;
}


unsigned short S19::GetCRC(unsigned short *pCrc)
{
	return *pCrc;
}

unsigned short S19::S19_GetCRC(void)
{
	return GetCRC(&m_s19_crc);
}

unsigned int S19::GetTotal(void)
{
	return m_total;
}

void S19::reset_S19image(BOARD_MEM* pImage)
{
	//initialize image
	memset(&(pImage->d), 0xff, sizeof(pImage->d));
	memset(&(pImage->f), 0, sizeof(pImage->f));
	memset(m_buf,0,sizeof(m_buf));
	m_warningCnt = 0;
	m_s19_crc = 0;
	m_total = 0;
	m_maxlen = 0;
}

int S19::read_s19(char *fn, BOARD_MEM* pImage)
{
	FILE *f;
	char c;
	char* pc;
	char afmt[7];
	unsigned u, b, sum, len, alen;
	unsigned long addr, addr_lo = MAX_ADDRESS, addr_hi = 0,addr_global = 0;
	int line = 0, terminate = 0;

	reset_S19image(pImage);

	if((f = *fn ? fopen(fn, "r") : stdin) == NULL)
	{
		fprintf(stderr, "Can't open input file %s\n", fn);
		return -1;
	}

	ResetCRC(&m_s19_crc, 0x0000);

	while(!terminate && (pc = fgets(m_buf, sizeof(m_buf), f)) != NULL)
	{
		line++;

		// S-records only
		if(*pc++ != 'S') 
			continue;

		// record type
		switch(c = *pc++)
		{
		case '0':	// S0 is skipped
			continue;
		case '1':	// S1 is accepted
			alen = 4;
			break;
		case '2':	// S2 is accepted
			alen = 6;
			break;
		case '3':	// S3 is accepted
			alen = 8;
			break;
		case '9':	// S9 terminates
		case '8':	// S8 terminates
		case '7':	// S7 terminates
			terminate = 1;
			continue;
		default:	// others are reported and skipped
			fprintf(stderr, "Skipping S%c record at line %d", c, line);
			continue;
		}

		// prepare len & addr scan format
		sprintf(afmt, "%%2x%%%dx", alen);

		// scan len & address
		if(sscanf(pc, afmt, &len, &addr)!=2 || len >= 0x100)
		{
			fprintf(stderr, "S-record parse error at line %d\n", line);
			return -1;
		}
//=======================================================================
		addr_global = addr;
		if((addr_global >= 0x4000 && addr_global <= 0x7FFF )
			||(addr_global >= 0xC000 && addr_global <= 0xFFFF )
		)
		{
			addr_global &= 0xFFFF;
			addr_global |= 0x30000;
		}
		else
		{
			addr_global &= 0x3FFF;//È¡µÍ14Î»
			addr_global = addr_global | ((addr&0xF0000)>> 2);
		}
//=======================================================================
		if(addr_global >= MAX_ADDRESS)
		{
			fprintf(stderr, "Address out of range at line %d\n", line);
			return -1;
		}


		printf("\rParsing S-record line %d ", line);
		// skip len & address
		pc += alen+2;
	
		// init checksum
		sum = len;
		for(u=0; u<4; u++)
			sum += (addr >> (u*8)) & 0xff;

		// len & addr processed in record
		len -= alen/2 + 1;

		// remember the longest record (see output)
		if(len > m_maxlen)
			m_maxlen = len;
		
		for(u=0; u<len; u++)
		{
			if(sscanf(pc, "%2x", &b) != 1 || b >= 0x100)
			{
				fprintf(stderr, "S-record data parse error at line %d\n", line);
				return -1;
			}
			pc += 2;

			if(pImage->f[addr_global+u] == 1)
			{
				if(!m_warningCnt)
				{				
					fprintf(stderr, "\rS-record data parse warning at line %d, address 0x%X. Rewriting exist data in image (bad image?).\r\n", line, addr+u);
					fprintf(stderr, "\r\n");
				}
				else if(m_warningCnt < 2)
				{
					fprintf(stderr, "\rS-record data parsing operation found more than one warning. Rewriting exist data in image (bad image?).\r\n");
					fprintf(stderr, "\r\n");
				}
				
				m_warningCnt++;
			}
			pImage->d[addr_global+u] = b;
			pImage->f[addr_global+u] = 1;
			sum += b;
			m_total++;

			CRC_AddByte(&m_s19_crc, b);

			if (addr_global+u < addr_lo)
				addr_lo = addr_global+u;
			if (addr_global+u > addr_hi)
				addr_hi = addr_global+u;
		}

		printf("Bytes: %d", m_total);

		// test CS
		if(sscanf(pc, "%2x", &b) != 1 || b >= 0x100 || ((sum+b)&0xff) != 0xff)
		{
			fprintf(stderr, "S-record checksum error at line %d\n", line);
			return -1;
		}
	}

	if (m_total == 0)
	{
		fprintf(stderr, "S-record contains no valid data!\n");
		return -1;
	}
	else
	{
		printf("\rParsed S-record lines: %d  Bytes total: %d\n", line, m_total);
		if (addr_hi >= 0x1000000)
			printf("Source address range: 0x%08X-0x%08X\n\n", addr_lo, addr_hi);
		else if (addr_hi >= 0x10000)
			printf("Source address range: 0x%06X-0x%06X\n\n", addr_lo, addr_hi);
		else
			printf("Source address range: 0x%04X-0x%04X\n\n", addr_lo, addr_hi);
	}
	fclose(f);
	return 0;
}

int S19::write_s19(char *fn, BOARD_MEM* pImage)
{
	FILE *f;
	char tmp[10];
	unsigned u, sum, len, addr, a2, line, total, addr_lo, addr_hi;

	// maximal record length
	if(!m_maxlen) 
		m_maxlen = 32;

	if((f = *fn ? fopen(fn, "w") : stdout) == NULL)
	{
		fprintf(stderr, "Can't open output file %s\n", fn);
		return -1;
	}

	line = 0;
	total = 0;
	addr_lo = (unsigned)-1;
	addr_hi = 0;

	addr = 0;
	for(;;)
	{
		line++;
		// find first used address
		while(addr < sizeof(pImage->f) && !pImage->f[addr])
			addr++;

		// count how many bytes we can dump
		for(len=0; (addr+len) < sizeof(pImage->f) && pImage->f[addr+len] && len < m_maxlen; len++)
			;

		total += len;

		// no more data to dump 
		if(!len)
			break;

		// prepare output line
		sprintf(m_buf, "S3%02X%08X", len+5, a2 = addr);

		// init checksum
		sum = len+5;
		for(u=0; u<4; u++)
		{
			sum += a2 & 0xff;
			a2 >>= 8;
		}

		if(addr_lo > addr)
			addr_lo = addr;

		if(addr_hi < addr + len)
			addr_hi = addr + len;


		// dump data 
		for(u=0; u<len; u++)
		{
			sprintf(tmp, "%02X", pImage->d[addr]);
			strcat(m_buf, tmp);
			sum += pImage->d[addr];
			addr++;
		}

		// dump checksum
		sprintf(tmp, "%02X\n", 0xff - (sum & 0xff));
		strcat(m_buf, tmp);

		// put line
		fputs(m_buf, f);
	}

	fputs("S70500000000FA\n", f);
	fclose(f);

	if (total == 0)
	{
		fprintf(stderr, "Image contains no valid data!\n");
		return -1;
	}
	else
	{
		printf("\r\nWrites S-record lines: %d  Bytes total: %d\r\n", line, total);
		
		if (addr_hi >= 0x10000)
			printf("Destination address range: 0x%06X-0x%06X\n", addr_lo, addr_hi);
		else
			printf("Destination address range: 0x%04X-0x%04X\n", addr_lo, addr_hi);
	}

	return 0;
}

