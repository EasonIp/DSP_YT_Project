
#include "DSP28_Device.h"

void InitSci(void)
{
	
	EALLOW;
	GpioMuxRegs.GPFMUX.all = 0x0030;
	EDIS;
	/* loopback   8 bit data */
	SciaRegs.SCICCR.all = 0x07;
	
	SciaRegs.SCICTL1.all = 0x03;
	SciaRegs.SCICTL2.all = 0x03;
	
	SciaRegs.SCIHBAUD = 0x00;
	SciaRegs.SCILBAUD = 0xF3;
	
	SciaRegs.SCICTL1.all = 0x23;
	
	PieCtrl.PIEIER9.bit.INTx1 = 1;
	PieCtrl.PIEIER9.bit.INTx2 = 1;
	//tbd...
 	

	// Initialize SCI-B:

	//tbd...
}
