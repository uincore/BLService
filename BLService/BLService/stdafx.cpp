
// stdafx.cpp : 只包括标准包含文件的源文件
// BLService.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

CDeviceCAN DeviceCan;
int g_module = 0;
CBin g_binDownload;

// config
int G_MEM_ST;
int G_MEM_ED;
int G_N_As;
int G_N_Ar;
int G_N_Bs;
int G_N_Br;
int G_N_Cs;
int G_N_Cr;
int G_P2_CLIENT;