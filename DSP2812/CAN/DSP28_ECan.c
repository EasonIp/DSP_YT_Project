//
//      TMDX ALPHA RELEASE
//      Intended for product evaluation purposes
//
//###########################################################################
//
// FILE:	DSP28_ECan.c
//
// TITLE:	DSP28 Enhanced CAN Initialization & Support Functions.
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  0.55| 06 May 2002 | L.H. | EzDSP Alpha Release
//  0.56| 20 May 2002 | L.H. | No change
//  0.57| 27 May 2002 | L.H. | No change
//###########################################################################

#include "DSP28_Device.h"


//---------------------------------------------------------------------------
// InitECan: 
//---------------------------------------------------------------------------
// This function initializes to a known state.
//
void InitECan(void)
{
    
	struct ECAN_REGS ECanaShadow;
	EALLOW;
	ECanaShadow.CANTIOC.all = ECanaRegs.CANTIOC.all;
	ECanaShadow.CANTIOC.bit.TXFUNC = 1;
	ECanaRegs.CANTIOC.all = ECanaShadow.CANRIOC.all;
	
	ECanaShadow.CANRIOC.all = ECanaRegs.CANRIOC.all;
	ECanaShadow.CANRIOC.bit.RXFUNC = 1;
	ECanaRegs.CANRIOC.all = ECanaShadow.CANRIOC.all;
	EDIS;
	
	ECanaRegs.CANME.all = 0;

    ECanaRegs.CANMD.bit.MD0=0;  //邮箱0发送
    ECanaRegs.CANMD.bit.MD1=1;
		
	ECanaMboxes.MBOX0.MID.all = 0x00000013;	/*发送邮箱的ID号*/
	ECanaMboxes.MBOX1.MID.all = 0x00000012;
	
	
	ECanaRegs.CANME.bit.ME0=1;	//邮箱使能
    ECanaRegs.CANME.bit.ME1=1;

	
	/*数据长度 8个BYTE*/
	ECanaMboxes.MBOX0.MCF.bit.DLC = 8;
	ECanaMboxes.MBOX1.MCF.bit.DLC = 8;

			
	/*没有远方应答帧被请求*/
	ECanaMboxes.MBOX0.MCF.bit.RTR = 0;
	ECanaMboxes.MBOX1.MCF.bit.RTR = 1;

	
	ECanaMboxes.MBOX0.MDRL.all = 0x12345678;
	ECanaMboxes.MBOX0.MDRH.all = 0x87654321;
	
	ECanaMboxes.MBOX1.MDRL.all = 0x12345678;
	ECanaMboxes.MBOX1.MDRH.all = 0x87654321;

	EALLOW;
	/*邮箱屏蔽使能*/
	ECanaRegs.CANMIM.all = 0xFFFFFFFF;
	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CCR = 1;
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
	EDIS;
	
	do
	{
		ECanaShadow.CANES.all = ECanaRegs.CANES.all;
	}while(ECanaShadow.CANES.bit.CCE != 1);
	
	EALLOW;
	
	ECanaShadow.CANBTC.bit.BRP = 79;
	
	ECanaShadow.CANBTC.bit.TSEG2 = 2;
	ECanaShadow.CANBTC.bit.TSEG1 = 7;
	ECanaRegs.CANBTC.all = ECanaShadow.CANBTC.all;
	
	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CCR = 0;
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
	EDIS;
	
	do
	{
		ECanaShadow.CANES.all = ECanaRegs.CANES.all;
	}while(ECanaShadow.CANES.bit.CCE != 0);
	
	EALLOW;
	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.STM = 0;
	ECanaShadow.CANMC.bit.SCM = 1;
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
	EDIS;
	
	#if	CAN_INT
	EALLOW;
	ECanaRegs.CANMIM.all = 0xFFFFFFFF;
	ECanaRegs.CANMIL.all = 0;
	ECanaRegs.CANGIF0.all = 0xFFFFFFFF;
	ECanaRegs.CANGIM.bit.I0EN = 1;
	ECanaRegs.CANGIM.bit.I1EN = 1;
	ECanaRegs.CANGIM.all = 0x3ff07;
	EDIS;
	PieCtrl.PIEIER9.bit.INTx6 = 1;
	PieCtrl.PIEIER9.bit.INTx5 = 1;
	#endif
}	

//===========================================================================
// No more.
//===========================================================================
