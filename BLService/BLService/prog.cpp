

#include "stdafx.h"

#ifdef _WINDOWS
	int myprintf(const char * _Format, ...);
	int myfprintf(__inout FILE * _File, const char * _Format, ...);
	void progress_Changed(unsigned value);
	#define printf myprintf
	#define fprintf myfprintf
#else
	#define progress_Changed(x)
#endif

// SDO (un)packing macros

/** Returns the command specifier (cs, ccs, scs) from the first byte of the SDO   
 */
#define getSDOcs(byte) (byte >> 5)

/** Returns the number of bytes without data from the first byte of the SDO. Coded in 2 bits   
 */
#define getSDOn2(byte) ((byte >> 2) & 3)

/** Returns the number of bytes without data from the first byte of the SDO. Coded in 3 bits   
 */
#define getSDOn3(byte) ((byte >> 1) & 7)

/** Returns the transfer type from the first byte of the SDO   
 */
#define getSDOe(byte) ((byte >> 1) & 1)

/** Returns the size indicator from the first byte of the SDO   
 */
#define getSDOs(byte) (byte & 1)

/** Returns the indicator of end transmission from the first byte of the SDO   
 */
#define getSDOc(byte) (byte & 1)

/** Returns the toggle from the first byte of the SDO   
 */
#define getSDOt(byte) ((byte >> 4) & 1)

/** Returns the index from the bytes 1 and 2 of the SDO   
 */
#define getSDOindex(byte1, byte2) ((byte2 << 8) | (byte1))

/** Returns the subIndex from the byte 3 of the SDO   
 */
#define getSDOsubIndex(byte3) (byte3)

/** Returns the subcommand in SDO block transfer
*/
#define getSDOCRC(byte1, byte2) ((byte2 << 8) | (byte1))
/** Returns the CRC from the bytes 1 and 2 of the SDO   
 */
#define getSDOblockSC(byte) (byte & 3)
/** Returns the blockSC   
 */
#define getSDONodeID(dword) (dword & 0x000000ff)
/** Returns the nodeid from the can message id   
 */
#define getSDOsa(dword) (dword & 0x000000ff)
/** Returns the nodeid from the can message id   
 */
#define getSDOda(dword) ((dword & 0x0000ff00)>>8)
/** Returns the da from the can message id   
 */
#define FLASHMODULO(x,y) ((unsigned long)(MAX_ADDRESS - (y) *(1 + ((MAX_ADDRESS-(unsigned long)((x)+(y))-1) / (y)))))




CProg prog;

CProg::CProg()
{
	g_module = -1;
	SetConnectState(CONNECTSTEP_INIT);
}

CProg::~CProg()
{
}

const char CProg::clientId_index[MODULE_NUM] = 
{
	MODULE_PANEL_ID,
	MODULE_FRONT_ID,
	MODULE_TOP_ID,
	MODULE_BEHIND_ID,
	MODULE_SWITCH1_ID,
	MODULE_SWITCH2_ID,
	MODULE_SWITCH3_ID,
	MODULE_SWITCH4_ID,
	MODULE_SWITCH5_ID,
	MODULE_SWITCH6_ID,
};

/*
//数据翻转
void * CProg::ReserBuffer(void *pBufferData,unsigned int nLen)
{
	void *pRet = NULL;
	unsigned char *pBuffer;
	unsigned char nTemp;
	int nMidPos,i,nPos;
	
	do 
	{
		// 参数有效性检查
		if (NULL == pBufferData  || nLen <= 0 )
		{
			break;
		}
		pBuffer = (unsigned char *)pBufferData;
		nMidPos = nLen / 2;
		for (i = 0; i < nMidPos; i++ )
		{
			nTemp = pBuffer[i];
			nPos = nLen - i - 1;
			pBuffer[i] = pBuffer[nPos];
			pBuffer[nPos] = nTemp;
		}
		pRet = pBufferData;
	} while (FALSE);
	return pRet;
}
*/

int CProg::GetConnectState()
{
	return m_ConnectState;
}

void CProg::SetConnectState(int connect)
{
	m_ConnectState = connect;
}

int CProg::Get_Module()
{
	return g_module;
}

void CProg::Set_Module(int module)
{
	g_module =  clientId_index[module];
	memset(&m_ident, 0, sizeof(m_ident));// initialize memory space
	switch(g_module)
	{
		case MODULE_PANEL_ID:
			m_ident.mem_start[0] = 0x20000;	// start of usable flash
			m_ident.mem_end[0] = 0x23FFF;	// end of usable flash

			m_ident.mem_start[1] = 0x24000;	// start of usable flash
			m_ident.mem_end[1] = 0x27FFF;	// end of usable flash

			m_ident.mem_start[2] = 0x28000;	// start of usable flash
			m_ident.mem_end[2] = 0x2BFFF;	// end of usable flash

			m_ident.mem_start[3] = 0x2C000;	// start of usable flash
			m_ident.mem_end[3] = 0x2FFFF;	// end of usable flash

			m_ident.mem_start[4] = 0x30000;	// start of usable flash
			m_ident.mem_end[4] = 0x33FFF;	// end of usable flash

			m_ident.mem_start[5] = 0x34000;	// start of usable flash
			m_ident.mem_end[5] = 0x37FFF;	// end of usable flash

			m_ident.mem_start[6] = 0x38000;	// start of usable flash
			m_ident.mem_end[6] = 0x3BFFF;	// end of usable flash

			m_ident.mem_start[7] = 0x3C000;	// start of usable flash
			m_ident.mem_end[7] = MAX_ADDRESS;// end of usable flash
			
			m_ident.erblk = 512;			// erase block size
			m_ident.wrblk = 512;			// write block size
			m_ident.bl_tbl = 0;		// start of boot-loader table (private data and jump table)
			m_ident.int_vect_tbl = 0;	// start of hard-wired interrupt vectors
			//m_ident.priv_data[8];		// 8 bytes of private info
			//m_ident.targ_name[40];		// target name
			m_ident.num_blocks = 8;	// number of flash memory blocks
			m_ident.sdid = 0;			// SDID number
			m_ident.sdidrev = 0;		// SDID revision
			break;
		case MODULE_FRONT_ID:
		case MODULE_TOP_ID:
		case MODULE_BEHIND_ID:
			m_ident.mem_start[0] = 0x20000;	// start of usable flash
			m_ident.mem_end[0]  = 0x23FFF;	// end of usable flash

			m_ident.mem_start[1] = 0x24000;	// start of usable flash
			m_ident.mem_end[1]  =  0x27FFF;	// end of usable flash

			m_ident.mem_start[2] = 0x28000;	// start of usable flash
			m_ident.mem_end[2]  = 0x2BFFF;	// end of usable flash

			m_ident.mem_start[3] = 0x2C000;	// start of usable flash
			m_ident.mem_end[3]  = 0x2FFFF;	// end of usable flash

			m_ident.mem_start[4] = 0x30000;	// start of usable flash
			m_ident.mem_end[4]  = 0x33FFF;	// end of usable flash

			m_ident.mem_start[5] = 0x34000;	// start of usable flash
			m_ident.mem_end[5]  = 0x37FFF;	// end of usable flash

			m_ident.mem_start[6] = 0x38000;	// start of usable flash
			m_ident.mem_end[6]  = 0x3BFFF;	// end of usable flash

			m_ident.mem_start[7] = 0x3C000;	// start of usable flash
			m_ident.mem_end[7]  = MAX_ADDRESS-1;	// end of usable flash
			
			m_ident.erblk = 512;			// erase block size
			m_ident.wrblk = 512;			// write block size
			m_ident.bl_tbl = 0;		// start of boot-loader table (private data and jump table)
			m_ident.int_vect_tbl = 0;	// start of hard-wired interrupt vectors
			//m_ident.priv_data[8];		// 8 bytes of private info
			//m_ident.targ_name[40];		// target name
			m_ident.num_blocks = 8;	// number of flash memory blocks
			m_ident.sdid = 0;			// SDID number
			m_ident.sdidrev = 0;		// SDID revision
			break;
		case MODULE_SWITCH1_ID:
		case MODULE_SWITCH2_ID:
		case MODULE_SWITCH3_ID:
		case MODULE_SWITCH4_ID:
		case MODULE_SWITCH5_ID:
		case MODULE_SWITCH6_ID:
			m_ident.mem_start[0] = 0x34000;	// start of usable flash
			m_ident.mem_end[0]  = 0x37FFF;	// end of usable flash

			m_ident.mem_start[1] = 0x38000;	// start of usable flash
			m_ident.mem_end[1]  = 0x3BFFF;	// end of usable flash

			m_ident.mem_start[2] = 0x3C000;	// start of usable flash
			m_ident.mem_end[2]  = MAX_ADDRESS-1;	// end of usable flash
			
			m_ident.erblk = 512;			// erase block size
			m_ident.wrblk = 512;			// write block size
			m_ident.bl_tbl = 0;		// start of boot-loader table (private data and jump table)
			m_ident.int_vect_tbl = 0;	// start of hard-wired interrupt vectors
			//m_ident.priv_data[8];		// 8 bytes of private info
			//m_ident.targ_name[40];		// target name
			m_ident.num_blocks = 3;	// number of flash memory blocks
			m_ident.sdid = 0;			// SDID number
			m_ident.sdidrev = 0;		// SDID revision
			break;
		default:
			break;
	}
}


void CProg::Progress_Reset(unsigned progressParts)
{	
	if(progressParts > 0)
		m_progress_parts = progressParts;
	else
		m_progress_parts = 1;

	m_progress = 0;

	progress_Changed(0);
}


void CProg::Progress_Set(unsigned progressPart, unsigned locProgress)
{
	unsigned tmp_progress;
	if(progressPart >= m_progress_parts)
		progressPart = m_progress_parts - 1;

	if(locProgress > 100)
		locProgress = 100;


	// compute finished parts of progress
	tmp_progress = (progressPart * 100) / m_progress_parts;


	tmp_progress += locProgress / m_progress_parts;

	if(tmp_progress != m_progress)
	{
		m_progress = tmp_progress;
		progress_Changed(m_progress);
	}
}

int CProg::OpenCAN()
{
	if(DeviceCan.OpenCAN() == STATUS_ERR)
	{
		printf("prog0");
		printf("打开KAVSER CAN 失败！\n");
		return -1;
	}
	return 0;
}

void CProg::StartReceive()
{
	// TODO: 模块物理地址需要先设置
	g_module = 0;
	DeviceCan.StartReceive();//启动接收线程
}

void CProg::DisConnect()
{
	Sleep(200);
	DeviceCan.CloseCAN();	
	SetConnectState(CONNECTSTEP_INIT);
	//printf("断开\n");
}

int CProg::Connect_detect()
{
	int index;
	unsigned char subIndex;
	if(DeviceCan.GetMsg(m_msg) != -1)
	{
		m_ConnectState = CONNECTSTEP_OK;
	}
	return -1;
}

int CProg::erase_ok()
{
	return 0;
}

//////////////////////////////////////////////////////////////
// check if code goes to valid memory only
int	CProg::check_image()
{

	// no error
	return 0;

//err_hit:
//		fprintf(stderr, "\nWARNING! BIN image will not fit into available memory (at address 0x%04X)!\n", a);
//	return -1;

}

int CProg::preProgramming()
{
	unsigned char msg[4] = {'x','x','y','y'};
	st_apdu_response res;

	if(CUDSprotocol::ReadDataByIndentifier(msg,4)!=0)
	{
		printf("ReadDataByIndentifier failed！\n");
		return -1;
	}
	while(1)
	{
		if(CUDSprotocol::GetResponse(res) == 0 && res.aai == g_module)
		{
			switch(res.apci)
			{
			case C_SID_ReadDataByIndentifier + C_POSITIVE_RESPONSE:
				if(CUDSprotocol::DiagnosticSessionControl(extendedDiagnosticSession)!=0)
				{
					printf("DiagnosticSessionControl failed！\n");
					return -1;
				}
				break;
			case C_SID_DiagnosticSessionControl + C_POSITIVE_RESPONSE:
				msg[0] = 1;
				msg[1] = 2;
				msg[2] = 3;
				if(CUDSprotocol::RoutineControl(msg,3)!=0)
				{
					printf("RoutineControl failed！\n");
					return -1;
				}
				break;
			case C_SID_RoutineControl + C_POSITIVE_RESPONSE:
				if(CUDSprotocol::ControlDTCSetting(DTCSettingOff)!=0)
				{
					printf("ControlDTCSetting failed！\n");
					return -1;
				}
				break;
			case C_SID_ControlDTCSetting + C_POSITIVE_RESPONSE:
				if(CUDSprotocol::CommunicationControl(disableRxAndTx,normalCommunicationMessages)!=0)
				{
					printf("CommunicationControl failed！\n");
					return -1;
				}
				break;
			case C_SID_CommunicationControl + C_POSITIVE_RESPONSE:
				return 0;
				break;
			case C_NEGATIVE_RESPONSE:
				printf("负响应！\n");
				return -1;
				break;
			default:
				
				break;
			}
		}
		Sleep(10);
	}
	return 0;
}

int CProg::programming()
{
	unsigned char msg[4] = {'x','x','y','y'};
	st_apdu_response res;
	if(CUDSprotocol::DiagnosticSessionControl(programmingSession)!=0)
	{
		printf("2 DiagnosticSessionControl failed！\n");
		return -1;
	}
	while(1)
	{
		if(CUDSprotocol::GetResponse(res) == 0 && res.aai == g_module)
		{
			switch(res.apci)
			{
			case C_SID_DiagnosticSessionControl + C_POSITIVE_RESPONSE:
				if(CUDSprotocol::SecurityAccess(requestSeedLevel,NULL,0)!=0)
				{
					printf("2 SecurityAccess 1 failed！\n");
					return -1;
				}
				break;
			case C_SID_SecurityAccess + C_POSITIVE_RESPONSE:
				if(res.adata[0] == requestSeedLevel)
				{
					// TODO:计算key
					if(CUDSprotocol::SecurityAccess(sendKeyLevel,msg,4)!=0)
					{
						printf("2 SecurityAccess 2 failed！\n");
						return -1;
					}
				}else if(res.adata[0] == sendKeyLevel)
				{
					msg[0] = 0xF1;
					msg[1] = 0x84;
					if(CUDSprotocol::WriteDataByIndentifier(msg,2)!=0)
					{
						printf("2 WriteDataByIndentifier failed！\n");
						return -1;
					}
				}
				break;
			case C_SID_WriteDataByIndentifier + C_POSITIVE_RESPONSE:
				if(downloadingFlashDriver()!=0)
				{
					printf("2 Downloading Flash Driver failed！\n");
					return -1;
				}
				msg[0] = 0x01;
				msg[1] = 0xFF;
				msg[2] = 0;
				if(CUDSprotocol::RoutineControl(msg,3)!=0)
				{
					printf("2 RoutineControl 1 failed！\n");
					return -1;
				}
				break;
			case C_SID_RoutineControl + C_POSITIVE_RESPONSE:
				if(res.adata[2] == 0xFF && res.adata[3] == 0x00){
					if(downloading()!=0)
					{
						printf("2 Downloading failed！\n");
						return -1;
					}
					msg[0] = 0x01;
					msg[1] = 0x02;
					msg[2] = 0x02;
					if(CUDSprotocol::RoutineControl(msg,3)!=0)
					{
						printf("2 RoutineControl 2 failed！\n");
						return -1;
					}
				}
				if(res.adata[2] = 0x02){
					msg[0] = 0x01;
					msg[1] = 0xFF;
					msg[2] = 0x01;
					if(CUDSprotocol::RoutineControl(msg,3)!=0)
					{
						printf("2 RoutineControl 2 failed！\n");
						return -1;
					}
				}
				if(res.adata[2] == 0xFF && res.adata[3] == 0x01){
					return 0;
				}
				break;
			case C_NEGATIVE_RESPONSE:
				printf("负响应！\n");
				return -1;
				break;
			default:
				
				break;
			}
		}
		Sleep(10);
	}
	return 0;
}

int CProg::postProgramming()
{
	unsigned char msg[2] = {0xF1,0x99};
	st_apdu_response res;
	if(CUDSprotocol::WriteDataByIndentifier(msg,2)!=0)
	{
		printf("3 WriteDataByIndentifier failed！\n");
		return -1;
	}
	while(1)
	{
		if(CUDSprotocol::GetResponse(res) == 0 && res.aai == g_module)
		{
			switch(res.apci)
			{
			case C_SID_WriteDataByIndentifier + C_POSITIVE_RESPONSE:
				if(CUDSprotocol::EcuReset(hardReset)!=0)
				{
					printf("3 EcuReset failed！\n");
					return -1;
				}
				break;
			case C_SID_EcuReset + C_POSITIVE_RESPONSE:
				return 0;
				break;
			case C_NEGATIVE_RESPONSE:
				printf("负响应！\n");
				return -1;
				break;
			default:
				
				break;
			}
		}
		Sleep(10);
	}
	return 0;
}

int CProg::downloadingFlashDriver()
{
	return 0;
}

int CProg::downloading()
{
	st_apdu_response res;
	unsigned short seq = 0;
	bool b_loop = true;
	for(int i=0;i<g_binDownload.m_bindata.noar;i++)
	{
		if(CUDSprotocol::RequestDownload(0,g_binDownload.m_segment[i].address,g_binDownload.m_segment[i].length)!=0)
		{
			printf("downloading RequestDownload failed！\n");
			return -1;
		}
		b_loop = true;
		while(b_loop)
		{
			if(CUDSprotocol::GetResponse(res) == 0 && res.aai == g_module)
			{
				switch(res.apci)
				{
				case C_SID_RequestDownload + C_POSITIVE_RESPONSE:
					seq ++;
					if(seq>0xFF) seq = 0;
					if(CUDSprotocol::TransferData(seq,g_binDownload.m_segment_data[i].d, g_binDownload.m_segment[i].length)!=0)
					{
						printf("downloading TransferData failed！\n");
						return -1;
					}
					break;
				case C_SID_TransferData + C_POSITIVE_RESPONSE:
					if(CUDSprotocol::RequestTransferExit()!=0)
					{
						printf("downloading RequestTransferExit failed！\n");
						return -1;
					}
					break;
				case C_SID_RequestTransferExit + C_POSITIVE_RESPONSE:
					b_loop = false;
					break;
				case C_NEGATIVE_RESPONSE:
					printf("负响应！\n");
					return -1;
					break;
				default:
				
					break;
				}
			}
			Sleep(10);
		}
	}
	return 0;
}