#include "StdAfx.h"
#include "Bin.h"

CBin::CBin(void)
{
	m_lenth = 0;
	m_maxlen = 0;
}


CBin::~CBin(void)
{
}

unsigned int CBin::GetTotal(void)
{
	return m_lenth;
}

int CBin::read_bin(char *fn)
{
	unsigned long addr, addr_lo = MAX_ADDRESS, addr_hi = 0,addr_global = 0;
	unsigned u;

	//initialize image
	//memset(&(pImage->d), 0xff, sizeof(pImage->d));
	//memset(&(pImage->f), 0, sizeof(pImage->f));
	memset(&m_bindata,0,sizeof(st_bin_data));
	memset(m_segment,0,sizeof(m_segment));
	memset(m_segment_data,0,sizeof(m_segment_data));
	m_lenth = 0;
	m_maxlen = 0;
	// head
	CFile file(fn, CFile::modeRead|CFile::typeBinary); 
	file.Read(&m_bindata, sizeof(st_bin_data));
	//char buf[512] = {0};
	// file.Read(buf, sizeof(st_bin_data));
	//m_bindata = (st_bin_data *)buf;
	// segment
	for(int i=0;i<m_bindata.noar;i++)
	{
		file.Read(&m_segment[i].address,sizeof(UINT32));
		file.Read(&m_segment[i].length,sizeof(UINT32));
		m_lenth +=m_segment[i].length;
	}
	for(int i=0;i<m_bindata.noar;i++)
	{
		file.Read(&m_segment_data[i].d,MAX_ADDRESS);
		addr_global = m_segment[i].address;
		// TODO:segment的地址是绝对还是相对的？
		/*
		if((addr_global >= 0x4000 && addr_global <= 0x7FFF )
			||(addr_global >= 0xC000 && addr_global <= 0xFFFF )
		)
		{
			addr_global &= 0xFFFF;
			addr_global |= 0x30000;
		}
		else
		{
			addr_global &= 0x3FFF;//取低14位
			addr_global = addr_global | ((addr&0xF0000)>> 2);
		}
		*/
		if(addr_global + m_segment[i].length >= MAX_ADDRESS)
		{
			fprintf(stderr, "Address out of range at line %d\n", i);
			return -1;
		}
		if(m_segment[i].length > m_maxlen)
			m_maxlen = m_segment[i].length;
		/*
		for(u=0; u<m_segment[i].length; u++)
		{
			pImage->d[addr_global+u] = m_segment_data[i].d[u];
			pImage->f[addr_global+u] = 1;
			if (addr_global+u < addr_lo)
				addr_lo = addr_global+u;
			if (addr_global+u > addr_hi)
				addr_hi = addr_global+u;
		}
		*/
	}
	if (m_lenth == 0)
	{
		fprintf(stderr, "BIN contains no valid data!\n");
		return -1;
	}
	return 0;
}
