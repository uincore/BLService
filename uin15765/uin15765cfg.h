

//15765 Source Address
// The source address is an 8-bit field and identifies a unique I15765 device on the network. 
// Its default value is set as a testing device, but should be changed to match its actual function.
#define UIN15765CFG_SA 241
//15765 Tick Period
//This define, which is in units of .1 milliseconds, 
//should be set to the fixed periodic interval at which i15765_update is called. 
//Due to ISO 15765-2 requirements, it is recommended that the period be no larger than 5 ms. 
//Maximum is 250 (25 ms).
#define UIN15765CFG_TICK_PERIOD 100
//I15765 Multi-frame RX Buffer Count
//This configuration defines how many incoming multi-frame messages can be received simultaneously.
#define UIN15765CFG_MF_RX_BUF_NUM 10
//15765 Multi-frame RX Buffer Size
//This configuration defines, in bytes, the largest message which can be received. 
//The largest available message, per ISO 15765-2, is 4,095 bytes.
#define UIN15765CFG_MF_RX_BUF_SIZE 100
//15765 Multi-frame TX Buffer Count
//This configuration defines how many outgoing multi-frame messages can be transmitted simultaneously.
#define UIN15765CFG_MF_TX_BUF_NUM 3
//15765 Multi-frame TX Buffer Size
//This configuration defines, in bytes, the largest message which can be transmitted. 
//The largest available message, per ISO 15765-2, is 4,095 bytes.
#define UIN15765CFG_MF_TX_BUF_SIZE 100
//timeout ms
#define UIN15765CFG_TX_TIMEOUT 100
//FC stmin ms
#define UIN15765CFG_FC_ST_MIN 20
//FC BS
#define UIN15765CFG_FC_BS 3