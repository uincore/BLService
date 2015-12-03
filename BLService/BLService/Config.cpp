#include "StdAfx.h"
#include "Config.h"


CConfig::CConfig(void)
{
	MEM_ST = 0x20000;
	MEM_ED = 0x23FFF;
	N_As = 1000;
	N_Ar = 1000;
	N_Bs = 1000;
	N_Br = 0;
	N_Cs = 0;
	N_Cr = 1000;
}


CConfig::~CConfig(void)
{
}
