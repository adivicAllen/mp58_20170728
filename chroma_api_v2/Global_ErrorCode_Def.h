#ifndef _GLOBAL_ERRORCODE_DEF_
#define _GLOBAL_ERRORCODE_DEF_

/*****************************************************
*		Global error code definition
*******************************************************/
#define SUCCESS								0
#define FAIL								1
#define SKIP								2

#define GPIB_ERR_BASE						100
#define ADIVIC_ERR_BASE						200
#define BT_ERR_BASE							300
#define INSTR_ERR_BASE						400

/********************************************************
*  GPIB Control Error code definition
********************************************************/
#define E_GPIB_WRITE_FAIL				GPIB_ERR_BASE+1
#define E_GPIB_READ_FAIL				GPIB_ERR_BASE+2
#define E_GPIB_NO_SESSION_INSTANCE		GPIB_ERR_BASE+3
#define E_GPIB_WRITE_BUF				GPIB_ERR_BASE+4
#define E_GPIB_READ_BUF					GPIB_ERR_BASE+5
#define E_GPIB_OpenSession_FAIL			GPIB_ERR_BASE+6
#define E_GPIB_CloseSession_FAIL		GPIB_ERR_BASE+7
#define E_GPIB_FIND_INSTR_ERR			GPIB_ERR_BASE+8
#define E_GPIB_Command_ERR				GPIB_ERR_BASE+9
#define E_GPIB_Time_Out					GPIB_ERR_BASE+10
#define E_INSTR_Wrong					GPIB_ERR_BASE+11

/*********************************************************
*		ADIVIC Error code definition    
**********************************************************/
#define		E_DSP300_ERR							ADIVIC_ERR_BASE+1
#define		E_CAL100_BOX_ERR						ADIVIC_ERR_BASE+2
#define		E_File_No_Exist							ADIVIC_ERR_BASE+3
#define		E_Incorrect_File						ADIVIC_ERR_BASE+4
#define		E_Command_ERR							ADIVIC_ERR_BASE+5
#define		E_YIG510_CAL_DATA_INVALID				ADIVIC_ERR_BASE+6
#define		E_RFIC_INI_MIC_FAIL						ADIVIC_ERR_BASE+7
#define		E_ADC_OPEN_FAIL							ADIVIC_ERR_BASE+8
#define		E_InputData_ERR							ADIVIC_ERR_BASE+9
#define		E_Function_ERR							ADIVIC_ERR_BASE+10
#define		E_TestProcess_ERR						ADIVIC_ERR_BASE+11
/*********************************************************
*		BT Error code
**********************************************************/
#define		E_BT_MEA_ERR							BT_ERR_BASE+1
#define		E_BT_OPEN_DUT_ERR						BT_ERR_BASE+2
#define		E_BT_TXP_ERR							BT_ERR_BASE+3
#define		E_BT_BER_ERR							BT_ERR_BASE+4
/********************************************************
*		Instr Remote
*********************************************************/
#define  E_INSTR_CONNECT_ERR						INSTR_ERR_BASE+1
#define  E_Server_NO_CONNECT						INSTR_ERR_BASE+2
#define  E_Input_OutOfDefine						INSTR_ERR_BASE+3
#define  E_INSTR_ERR								INSTR_ERR_BASE+4

#endif//_GLOBAL_ERRORCODE_DEF_