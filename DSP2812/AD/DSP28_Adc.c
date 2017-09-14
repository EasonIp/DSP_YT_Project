//
//      TMDX ALPHA RELEASE
//      Intended for product evaluation purposes
//
//###########################################################################
//
// FILE:	DSP28_Adc.c
//
// TITLE:	DSP28 ADC Initialization & Support Functions.
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
// InitAdc: 
//---------------------------------------------------------------------------
// This function initializes ADC to a known state.
//
void InitAdc(void)
{
	unsigned int i;
    Uint16 j;	
	AdcRegs.ADCTRL1.bit.RESET=1;
	NOP;
	AdcRegs.ADCTRL1.bit.RESET=0;
	AdcRegs.ADCTRL1.bit.SUSMOD=3;
	AdcRegs.ADCTRL1.bit.ACQ_PS=0;
	AdcRegs.ADCTRL1.bit.CPS=0;
	AdcRegs.ADCTRL1.bit.CONT_RUN=1;
	AdcRegs.ADCTRL1.bit.SEQ_CASC=1;
	
	AdcRegs.ADCTRL3.bit.ADCBGRFDN=3;
	for(i=0;i<10000;i++)	NOP;
	AdcRegs.ADCTRL3.bit.ADCPWDN=1;
	for(i=0;i<5000;i++)	NOP;
	AdcRegs.ADCTRL3.bit.ADCCLKPS=15;
	AdcRegs.ADCTRL3.bit.SMODE_SEL=1;
	
	AdcRegs.MAX_CONV.bit.MAX_CONV=15;
	
    AdcRegs.CHSELSEQ1.all=j;
    AdcRegs.CHSELSEQ2.all=j;
    AdcRegs.CHSELSEQ3.all=j;
    AdcRegs.CHSELSEQ4.all=j;

	
	AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;
	AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;	
	
	AdcRegs.ADCTRL2.bit.EVB_SOC_SEQ=0;
	AdcRegs.ADCTRL2.bit.RST_SEQ1=0;
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1=1;
	AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1=0;
	AdcRegs.ADCTRL2.bit.EVA_SOC_SEQ1=0;
	AdcRegs.ADCTRL2.bit.EXT_SOC_SEQ1=0;
	AdcRegs.ADCTRL2.bit.RST_SEQ2=0;
	AdcRegs.ADCTRL2.bit.SOC_SEQ2=0;
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ2=1;
	AdcRegs.ADCTRL2.bit.INT_MOD_SEQ2=0;
	AdcRegs.ADCTRL2.bit.EVB_SOC_SEQ2=0;
	AdcRegs.ADCTRL2.bit.SOC_SEQ1=1;
	
	
	
	
}	


//===========================================================================
// No more.
//===========================================================================
