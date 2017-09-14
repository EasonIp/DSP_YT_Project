//
//      TMDX ALPHA RELEASE
//      Intended for product evaluation purposes
//
//###########################################################################
//
// FILE:	DSP28_Xintf.c
//
// TITLE:	DSP28 Device External Interface Init & Support Functions.
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
// InitXINTF: 
//---------------------------------------------------------------------------
// This function initializes the External Interface to a known state.
//
void InitXintf(void)
{

	#if  F2812
    // Example of chaning the timing of XINTF Zones.  
    // Note acutal values should be based on the hardware 
    // attached to the zone - timings presented here are 
    // for example purposes.
     
    // All Zones:
    // Timing for all zones based on XTIMCLK = SYSCLKOUT 
    XintfRegs.XINTCNF2.bit.XTIMCLK = 0x0000;
    
    // Zone 0:
    // Change write access lead active trail timing
	// When using ready, ACTIVE must be 1 or greater
	// Lead must always be 1 or greater
	// Use timings based on SYSCLKOUT = XTIMCLK
	XintfRegs.XTIMING0.bit.XWRTRAIL = 3;
	XintfRegs.XTIMING0.bit.XWRACTIVE = 7;
	XintfRegs.XTIMING0.bit.XWRLEAD = 3;
	// Do not double lead/active/trail for Zone 0
	XintfRegs.XTIMING0.bit.X2TIMING = 0;
	
	// Zone 2
	// Ignore XREADY for Zone 2 accesses
	// Change read access lead/active/trail timing
	XintfRegs.XTIMING2.bit.USEREADY = 0;
	XintfRegs.XTIMING2.bit.XRDLEAD = 3;
	XintfRegs.XTIMING2.bit.XWRACTIVE = 7;
	XintfRegs.XTIMING2.bit.XRDTRAIL = 3;
	// Double lead/active/trial timing for Zone 2
	XintfRegs.XTIMING2.bit.X2TIMING = 1;

    // Zone 2 is slow, so add additional BCYC cycles when ever switching
    // from Zone 2 to another Zone.  This will help avoid
    // bus contention.
    XintfRegs.XBANK.bit.BANK = 2;
    XintfRegs.XBANK.bit.BCYC = 3; 
	
	#endif
}	
	
//===========================================================================
// No more.
//===========================================================================
