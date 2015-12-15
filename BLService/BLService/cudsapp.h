
	static CTPprotocol m_tp;
	static int SendUDS(unsigned char apci,unsigned char  *adata,int alength);

	//
	static int GetResponse(st_apdu_response &res);
	// ·þÎñ
	static int DiagnosticSessionControl(em_diagnosticSessionType type);
	static int RoutineControl(unsigned char *msg,int length);
	static int ReadDataByIndentifier(unsigned char *msg,int length);
	static int ControlDTCSetting(em_DTCSettingType type);
	static int CommunicationControl(em_ControlType contralType,em_CommunicationType commType);
	static int SecurityAccess(em_SecurityAccessType type,unsigned char *msg,int length);
	static int WriteDataByIndentifier(unsigned char *msg,int length);
	static int RequestDownload(unsigned char dataFormatIndentifier,UINT address,UINT size);
	static int TransferData(unsigned char seq,unsigned char *data,int length);
	static int RequestTransferExit();
	static int EcuReset(em_ResetType type);
