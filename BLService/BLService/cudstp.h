#define	NPDU_FLOWCONTROL                3
#define	NPDU_MORE						2
#define	NPDU_MULT						1
#define	NPDU_SINGLE						0

typedef struct npdu_first_frame {
	unsigned char	npci_type : 4;
	unsigned short	ff_dl : 12;			
	unsigned char	data[6];
}st_npdu_first_frame;

typedef struct npdu_consecutive_frame {
	unsigned char	npci_type : 4;
	unsigned short	sn : 4;			
	unsigned char	data[7];
}st_npdu_consecutive_frame;

typedef struct npdu_flow_control {
	unsigned char	npci_type : 4;
	unsigned short	fs : 4;			
	unsigned char	bs;
	unsigned char	stmin;
}st_npdu_flow_control;

int Send(UINT nai,unsigned char *data,UINT length);
static UINT ReceiveThread(void *param);
