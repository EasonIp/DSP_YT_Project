// TI File $Revision: /main/5 $
// Checkin $Date: August 16, 2007   11:06:26 $
//###########################################################################
//
// FILE:   DSP2833x_Xintf.c
//
// TITLE:   DSP2833x Device External Interface Init & Support Functions.
//
// DESCRIPTION:
//
//          Example initialization function for the external interface (XINTF).
//          This example configures the XINTF to its default state.  For an
//          example of how this function being used refer to the
//          examples/run_from_xintf project.
//
//###########################################################################
// $TI Release: DSP2833x/DSP2823x Header Files V1.20 $
// $Release Date: August 1, 2008 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

//---------------------------------------------------------------------------
// InitXINTF:
//---------------------------------------------------------------------------
// This function initializes the External Interface the default reset state.
//
// Do not modify the timings of the XINTF while running from the XINTF.  Doing
// so can yield unpredictable results


void InitXintf(void)
{
    // This shows how to write to the XINTF registers.  The
    // values used here are the default state after reset.
    // Different hardware will require a different configuration.

    // For an example of an XINTF configuration used with the
    // F28335 eZdsp, refer to the examples/run_from_xintf project.

    // Any changes to XINTF timing should only be made by code
    // running outside of the XINTF.

    // All Zones---------------------------------
    // Timing for all zones based on XTIMCLK = 1/2 SYSCLKOUT
    EALLOW;
    XintfRegs.XINTCNF2.bit.XTIMCLK = 1;
    // No write buffering
    XintfRegs.XINTCNF2.bit.WRBUFF = 0;
    // XCLKOUT is enabled
    XintfRegs.XINTCNF2.bit.CLKOFF = 0;
    // XCLKOUT = XTIMCLK/2
    XintfRegs.XINTCNF2.bit.CLKMODE = 1;


    // Zone 0------------------------------------
    // When using ready, ACTIVE must be 1 or greater
    // Lead must always be 1 or greater
    // Zone write timing
    XintfRegs.XTIMING0.bit.XWRLEAD = 3;
    XintfRegs.XTIMING0.bit.XWRACTIVE = 7;
    XintfRegs.XTIMING0.bit.XWRTRAIL = 3;
    // Zone read timing
    XintfRegs.XTIMING0.bit.XRDLEAD = 3;
    XintfRegs.XTIMING0.bit.XRDACTIVE = 7;
    XintfRegs.XTIMING0.bit.XRDTRAIL = 3;

    // double all Zone read/write lead/active/trail timing
    XintfRegs.XTIMING0.bit.X2TIMING = 1;

    // Zone will sample XREADY signal
    XintfRegs.XTIMING0.bit.USEREADY = 1;
    XintfRegs.XTIMING0.bit.READYMODE = 1;  // sample asynchronous

    // Size must be either:
    // 0,1 = x32 or
    // 1,1 = x16 other values are reserved
    XintfRegs.XTIMING0.bit.XSIZE = 3;

    // Zone 6------------------------------------
    // When using ready, ACTIVE must be 1 or greater
    // Lead must always be 1 or greater
    // Zone write timing
    XintfRegs.XTIMING6.bit.XWRLEAD = 3;
    XintfRegs.XTIMING6.bit.XWRACTIVE = 7;
    XintfRegs.XTIMING6.bit.XWRTRAIL = 3;
    // Zone read timing
    XintfRegs.XTIMING6.bit.XRDLEAD = 3;
    XintfRegs.XTIMING6.bit.XRDACTIVE = 7;
    XintfRegs.XTIMING6.bit.XRDTRAIL = 3;

    // double all Zone read/write lead/active/trail timing
    XintfRegs.XTIMING6.bit.X2TIMING = 1;

    // Zone will sample XREADY signal
    XintfRegs.XTIMING6.bit.USEREADY = 1;
    XintfRegs.XTIMING6.bit.READYMODE = 1;  // sample asynchronous

    // Size must be either:
    // 0,1 = x32 or
    // 1,1 = x16 other values are reserved
    XintfRegs.XTIMING6.bit.XSIZE = 3;


    // Zone 7------------------------------------
    // When using ready, ACTIVE must be 1 or greater
    // Lead must always be 1 or greater
    // Zone write timing
    XintfRegs.XTIMING7.bit.XWRLEAD = 3;
    XintfRegs.XTIMING7.bit.XWRACTIVE = 7;
    XintfRegs.XTIMING7.bit.XWRTRAIL = 3;
    // Zone read timing
    XintfRegs.XTIMING7.bit.XRDLEAD = 3;
    XintfRegs.XTIMING7.bit.XRDACTIVE = 7;
    XintfRegs.XTIMING7.bit.XRDTRAIL = 3;

    // double all Zone read/write lead/active/trail timing
    XintfRegs.XTIMING7.bit.X2TIMING = 1;

    // Zone will sample XREADY signal
    XintfRegs.XTIMING7.bit.USEREADY = 1;
    XintfRegs.XTIMING7.bit.READYMODE = 1;  // sample asynchronous

    // Size must be either:
    // 0,1 = x32 or
    // 1,1 = x16 other values are reserved
    XintfRegs.XTIMING7.bit.XSIZE = 3;

    // Bank switching
    // Assume Zone 7 is slow, so add additional BCYC cycles
    // when ever switching from Zone 7 to another Zone.
    // This will help avoid bus contention.
    XintfRegs.XBANK.bit.BANK = 7;
    XintfRegs.XBANK.bit.BCYC = 7;
    EDIS;
   //Force a pipeline flush to ensure that the write to
   //the last register configured occurs before returning.

   InitXintf16Gpio();
// InitXintf32Gpio();

   asm(" RPT #7 || NOP");

}

void InitXintf32Gpio()
{
     EALLOW;
     GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 3;  // XD31
     GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 3;  // XD30
     GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 3;  // XD29
     GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 3;  // XD28
     GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 3;  // XD27
     GpioCtrlRegs.GPBMUX2.bit.GPIO53 = 3;  // XD26
     GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 3;  // XD25
     GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 3;  // XD24
     GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 3;  // XD23
     GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 3;  // XD22
     GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 3;  // XD21
     GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 3;  // XD20
     GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 3;  // XD19
     GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 3;  // XD18
     GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 3;  // XD17
     GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 3;  // XD16

     GpioCtrlRegs.GPBQSEL2.bit.GPIO48 = 3;  // XD31 asynchronous input
     GpioCtrlRegs.GPBQSEL2.bit.GPIO49 = 3;  // XD30 asynchronous input
     GpioCtrlRegs.GPBQSEL2.bit.GPIO50 = 3;  // XD29 asynchronous input
     GpioCtrlRegs.GPBQSEL2.bit.GPIO51 = 3;  // XD28 asynchronous input
     GpioCtrlRegs.GPBQSEL2.bit.GPIO52 = 3;  // XD27 asynchronous input
     GpioCtrlRegs.GPBQSEL2.bit.GPIO53 = 3;  // XD26 asynchronous input
     GpioCtrlRegs.GPBQSEL2.bit.GPIO54 = 3;  // XD25 asynchronous input
     GpioCtrlRegs.GPBQSEL2.bit.GPIO55 = 3;  // XD24 asynchronous input
     GpioCtrlRegs.GPBQSEL2.bit.GPIO56 = 3;  // XD23 asynchronous input
     GpioCtrlRegs.GPBQSEL2.bit.GPIO57 = 3;  // XD22 asynchronous input
     GpioCtrlRegs.GPBQSEL2.bit.GPIO58 = 3;  // XD21 asynchronous input
     GpioCtrlRegs.GPBQSEL2.bit.GPIO59 = 3;  // XD20 asynchronous input
     GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 3;  // XD19 asynchronous input
     GpioCtrlRegs.GPBQSEL2.bit.GPIO61 = 3;  // XD18 asynchronous input
     GpioCtrlRegs.GPBQSEL2.bit.GPIO62 = 3;  // XD17 asynchronous input
     GpioCtrlRegs.GPBQSEL2.bit.GPIO63 = 3;  // XD16 asynchronous input


     InitXintf16Gpio();
}

void InitXintf16Gpio()
{
     EALLOW;
     GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 3;  // XD15
     GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 3;  // XD14
     GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 3;  // XD13
     GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 3;  // XD12
     GpioCtrlRegs.GPCMUX1.bit.GPIO68 = 3;  // XD11
     GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 3;  // XD10
     GpioCtrlRegs.GPCMUX1.bit.GPIO70 = 3;  // XD19
     GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 3;  // XD8
     GpioCtrlRegs.GPCMUX1.bit.GPIO72 = 3;  // XD7
     GpioCtrlRegs.GPCMUX1.bit.GPIO73 = 3;  // XD6
     GpioCtrlRegs.GPCMUX1.bit.GPIO74 = 3;  // XD5
     GpioCtrlRegs.GPCMUX1.bit.GPIO75 = 3;  // XD4
     GpioCtrlRegs.GPCMUX1.bit.GPIO76 = 3;  // XD3
     GpioCtrlRegs.GPCMUX1.bit.GPIO77 = 3;  // XD2
     GpioCtrlRegs.GPCMUX1.bit.GPIO78 = 3;  // XD1
     GpioCtrlRegs.GPCMUX1.bit.GPIO79 = 3;  // XD0

     GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 3;  // XA0/XWE1n
     GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 3;  // XA1
     GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 3;  // XA2
     GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 3;  // XA3
     GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 3;  // XA4
     GpioCtrlRegs.GPBMUX1.bit.GPIO45 = 3;  // XA5
     GpioCtrlRegs.GPBMUX1.bit.GPIO46 = 3;  // XA6
     GpioCtrlRegs.GPBMUX1.bit.GPIO47 = 3;  // XA7

     GpioCtrlRegs.GPCMUX2.bit.GPIO80 = 3;  // XA8
     GpioCtrlRegs.GPCMUX2.bit.GPIO81 = 3;  // XA9
     GpioCtrlRegs.GPCMUX2.bit.GPIO82 = 3;  // XA10
     GpioCtrlRegs.GPCMUX2.bit.GPIO83 = 3;  // XA11
     GpioCtrlRegs.GPCMUX2.bit.GPIO84 = 3;  // XA12
     GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 3;  // XA13
     GpioCtrlRegs.GPCMUX2.bit.GPIO86 = 3;  // XA14
     GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 3;  // XA15
     GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 3;  // XA16
     GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 3;  // XA17
     GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 3;  // XA18
     GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 3;  // XA19

     GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 3;  // XREADY
     GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 3;  // XRNW
     GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 3;  // XWE0

     GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 3;  // XZCS0
     GpioCtrlRegs.GPBMUX1.bit.GPIO37 = 3;  // XZCS7
     GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 3;  // XZCS6	 
     
    GpioDataRegs.GPADAT.bit.GPIO10  = 1;	  //485使能
 //  GpioDataRegs.GPCDAT.bit.GPIO82  = 0;	  //故障指示灯
//     GpioDataRegs.GPADAT.bit.GPIO24  = 0;	  //停止指示灯
//     GpioDataRegs.GPADAT.bit.GPIO25  = 1;	  //运行指示灯
     GpioDataRegs.GPADAT.bit.GPIO26  = 0;	//继电器控制
      GpioDataRegs.GPADAT.bit.GPIO30  = 0;
     GpioDataRegs.GPADAT.bit.GPIO27  = 0;	//风扇控制
     GpioDataRegs.GPBDAT.bit.GPIO55  = 0;	//DSP正常运行指示灯
    
    GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 0;
//     GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 0;
//     GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 0;
//     GpioCtrlRegs.GPCMUX2.bit.GPIO82 = 0;
//     GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 0;
//     GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0;
      GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 0;	//接触器
      GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 0;	//风扇
      GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 0; 	//开关
      GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 0;	//DDOG

    GpioCtrlRegs.GPADIR.bit.GPIO10  = 1;	//输出
//     GpioCtrlRegs.GPADIR.bit.GPIO21  = 0;	//输入
//     GpioCtrlRegs.GPCDIR.bit.GPIO22  = 0;
  //   GpioCtrlRegs.GPCDIR.bit.GPIO82  = 1;
//     GpioCtrlRegs.GPADIR.bit.GPIO24  = 1;
//     GpioCtrlRegs.GPADIR.bit.GPIO25  = 1;
      GpioCtrlRegs.GPADIR.bit.GPIO26  = 1;	//接触器
      GpioCtrlRegs.GPADIR.bit.GPIO27  = 1;	//风扇
      GpioCtrlRegs.GPADIR.bit.GPIO30  = 0;	//开关
      GpioCtrlRegs.GPBDIR.bit.GPIO55  = 1;	//DDOG

	  GpioCtrlRegs.GPAMUX1.bit.GPIO4  = 0;
	  GpioCtrlRegs.GPAMUX1.bit.GPIO5  = 0;
	  GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;
	  GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 0;

	  GpioCtrlRegs.GPADIR.bit.GPIO4   = 0;
	  GpioCtrlRegs.GPADIR.bit.GPIO5   = 0;
	  GpioCtrlRegs.GPADIR.bit.GPIO31  = 1;
	  GpioCtrlRegs.GPBDIR.bit.GPIO39  = 1;











/*DSP内部EPWM配置
     	 GpioCtrlRegs.GPAMUX1.bit.GPIO0  = 1;
	 GpioCtrlRegs.GPAMUX1.bit.GPIO1  = 1;
	 GpioCtrlRegs.GPAMUX1.bit.GPIO2  = 1;
	 GpioCtrlRegs.GPAMUX1.bit.GPIO3  = 1;
	 GpioCtrlRegs.GPAMUX1.bit.GPIO4  = 1;
	 GpioCtrlRegs.GPAMUX1.bit.GPIO5  = 1;
	 GpioCtrlRegs.GPAMUX1.bit.GPIO6  = 1;
	 GpioCtrlRegs.GPAMUX1.bit.GPIO7  = 1;
	 GpioCtrlRegs.GPAMUX1.bit.GPIO8  = 1;
	 GpioCtrlRegs.GPAMUX1.bit.GPIO9  = 1;
	 GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;
	 GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;
	 GpioCtrlRegs.GPAPUD.bit.GPIO0   = 1;
	 GpioCtrlRegs.GPAPUD.bit.GPIO1   = 1;
	 GpioCtrlRegs.GPAPUD.bit.GPIO2   = 1;
	 GpioCtrlRegs.GPAPUD.bit.GPIO3   = 1;
	 GpioCtrlRegs.GPAPUD.bit.GPIO4   = 1;
	 GpioCtrlRegs.GPAPUD.bit.GPIO5   = 1;
	 GpioCtrlRegs.GPAPUD.bit.GPIO6   = 1;
	 GpioCtrlRegs.GPAPUD.bit.GPIO7   = 1;
	 GpioCtrlRegs.GPAPUD.bit.GPIO8   = 1;
	 GpioCtrlRegs.GPAPUD.bit.GPIO9   = 1;
	 GpioCtrlRegs.GPAPUD.bit.GPIO10  = 1;
	 GpioCtrlRegs.GPAPUD.bit.GPIO11  = 1;*/

//中断GPIO配置
     GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 0;
//     GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 0;
//     GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 0;

     GpioCtrlRegs.GPADIR.bit.GPIO17  = 0;
//     GpioCtrlRegs.GPBDIR.bit.GPIO49  = 0;
//     GpioCtrlRegs.GPBDIR.bit.GPIO50  = 0;

     GpioCtrlRegs.GPAQSEL2.bit.GPIO17= 2;
//     GpioCtrlRegs.GPBQSEL2.bit.GPIO49= 2;
//     GpioCtrlRegs.GPBQSEL2.bit.GPIO50= 2;

     GpioCtrlRegs.GPACTRL.bit.QUALPRD0=0xFF;
     GpioCtrlRegs.GPBCTRL.bit.QUALPRD0=0xFF;

     GpioIntRegs.GPIOXINT2SEL.bit.GPIOSEL = 17;
//     GpioIntRegs.GPIOXINT4SEL.bit.GPIOSEL = 49;
//     GpioIntRegs.GPIOXINT5SEL.bit.GPIOSEL = 50;

     XIntruptRegs.XINT2CR.bit.POLARITY = 1;
//     XIntruptRegs.XINT4CR.bit.POLARITY = 1;
//     XIntruptRegs.XINT5CR.bit.POLARITY = 1;	//上升沿

     XIntruptRegs.XINT2CR.bit.ENABLE = 1;
//     XIntruptRegs.XINT4CR.bit.ENABLE = 1;
//     XIntruptRegs.XINT5CR.bit.ENABLE = 1;	

     EDIS;
}

//===========================================================================
// No more.
//===========================================================================
