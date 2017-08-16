#pragma once
#define Port_Nums	4

#define UserFolder				"user"
#define PortFileExtName			"pfe"
#define DutCableFileExtName		"tbl"
#define ConfigFileExtName		"ini"
#define NONE	"NONE"
#define MaxInstr	2

#define TxPower_Max 10
#define TxPower_Min -130


#define CWFrequencyOffset_Max 50
#define CWFrequencyOffset_Min -50

#define f_NoneValue	-9999.0

typedef struct {
	float fFrequency;
	float fTxLoss[4];
	float fRxLoss[4];
}LossPoint;