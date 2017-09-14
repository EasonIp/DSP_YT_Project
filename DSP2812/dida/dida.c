#include "DSP28_Device.h"

#define RAMBASE 0x100000      //×ÜÏßµØÖ·,¶ÔÓ¦ÓÚÆ¬Ñ¡ÐÅºÅXZCS6
//#define RAMBASE 0x080000      //×ÜÏßµØÖ·,¶ÔÓ¦ÓÚÆ¬Ñ¡ÐÅºÅXZCS2


#pragma CODE_SECTION(InitFlash, "secureRamFuncs")
void InitFlash(void)
{
	asm(" EALLOW"); 
	
	FlashRegs.FPWR.bit.PWR = 3; 
	FlashRegs.FSTATUS.bit.V3STAT = 1; 
	FlashRegs.FSTDBYWAIT.bit.STDBYWAIT = 0x01FF; 
	FlashRegs.FACTIVEWAIT.bit.ACTIVEWAIT = 0x01FF; 
	FlashRegs.FBANKWAIT.bit.RANDWAIT = 5;
	FlashRegs.FBANKWAIT.bit.PAGEWAIT = 5; 
	FlashRegs.FOPT.bit.ENPIPE = 1;
	
	asm(" EDIS");
	asm(" RPT #6 || NOP");
}


////////////////±äÁ¿ÉêÃ÷/////////////////////////////////

Uint32 i=0;                  //±äÁ¿
Uint16 count=0;            
Uint16 *rambase=0; 
Uint16 tmp=0x66;

/////////////////º¯ÊýÉêÃ÷/////////////////////////////////

void InitSysCtrl(void);
void InitGpio(void);
void init_eva_timer1(void);
void InitXintf(void);
interrupt void eva_timer1_isr(void);


//////////////////////ÏµÍ³³õÊ¼»¯//////////////////////////////////

void InitSysCtrl(void)
{
   EALLOW;
   
   DevEmuRegs.M0RAMDFT = 0x0300;
   DevEmuRegs.M1RAMDFT = 0x0300;
   DevEmuRegs.L0RAMDFT = 0x0300;
   DevEmuRegs.L1RAMDFT = 0x0300;
   DevEmuRegs.H0RAMDFT = 0x0300;
        
   SysCtrlRegs.WDCR= 0x0068;   //½ûÖ¹¿´ÃÅ¹·

   SysCtrlRegs.PLLCR = 0x8;   //Ö÷Æµ=30*8/2=120MHz
  
   for(i=0; i<5000; i++){}   // ÎªÁËÆµÂÊÎÈ¶¨£¬±ØÐëÑÓÊ±Ò»¶ÎÊ±¼ä
      
   SysCtrlRegs.HISPCP.all = 0x0002;  //¸ßËÙÍâÉèÊ±ÖÓ=120/4=30
   SysCtrlRegs.LOSPCP.all = 0x0002;	 //µÍËÙÍâÉèÊ±ÖÓ=120/4=30
 
   SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;  //Ê¹ÄÜEVAÄ£¿éµÄ¸ßËÙÍâÉèÊ±ÖÓ
   SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;  //Ê¹ÄÜEVBÄ£¿éµÄ¸ßËÙÍâÉèÊ±ÖÓ
   SysCtrlRegs.PCLKCR.bit.SCIENCLKA=1;
				
   EDIS;
	
}


///////////////////IO¿Ú³õÊ¼»¯//////////////////////////////////

void InitGpio(void)
{

   EALLOW;

   GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOAÉèÖÃÎªI/O¿Ú 
   GpioMuxRegs.GPADIR.all=0xFFFF;   //A0-A15ÉèÖÃÎªÊä³ö	
   GpioDataRegs.GPADAT.all=0x0000; 

   EDIS;

}	


///////////////////¶¨Ê±Æ÷1³õÊ¼»¯£¨100mS)//////////////////////////////////

void init_eva_timer1(void)
{
  
   EvaRegs.GPTCONA.all = 0;   //³õÊ¼»¯¶¨Ê±Æ÷1²¢ÉèÖÃ¶¨Ê±Æ÷1¼Ä´æÆ÷
   EvaRegs.EVAIMRA.bit.T1PINT = 1;  //ÔÊÐí¶¨±Æ÷1ÖÜÆÚÖÐ¶Ï
   EvaRegs.EVAIFRA.bit.T1PINT = 1;  //Çå¶¨÷1ÖÜÆÚÖÐ¶Ï±êÖ¾
             
   EvaRegs.T1CON.all = 0x170C;  //Timer1ÎªÁ¬ÐøÔö¼ÆÊýÄ£Ê½£¬Ô¤·ÖÆµÖµ128
	                             //Ê¹ÓÃÄÚ²¿¸ßËÙÍâÉèÊ±ÖÓ
   EvaRegs.T1PR = 0x5b8d;   //Timer1µÄÆÚ¼Ä´æÆ÷=0.1*30MHz/128
   EvaRegs.T1CNT = 0x0000;  //Timer1µÄ¼ÆÊýã

}

	
////////////////////Íâ²¿½Ó¿Ú³õÊ¼»¯/////////////////////////
void InitXintf(void)
{  
    XintfRegs.XINTCNF2.bit.XTIMCLK = 0x0001;  //ËùÓÐzoneµÄÊ±ÐòÊÇ»ùÓÚXTIMCLK = SYSCLKOUT/2 
    XintfRegs.XINTCNF2.bit.WRBUFF = 3;   //ÉèÖÃ3¸öÐ´»º³å
	XintfRegs.XINTCNF2.bit.CLKOFF = 0;  //Ê¹ÄÜXCLKOUT
	XintfRegs.XINTCNF2.bit.CLKMODE = 0;  //ÉèÖÃXCLKOUT=XTIMCLK
    	

	//Zone2Ð´Ê±Ðò
	/*XintfRegs.XTIMING2.bit.XWRLEAD = 3;    //Ð´²Ù×÷Òýµ¼½×¶ÎµÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING2.bit.XWRACTIVE = 7;   //Ð´²Ù×÷¼¤»î½×¶ÎµÈ´ý×´Ì¬µÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING2.bit.XWRTRAIL = 3;    //Ð´²Ù×÷¸ú×Ù½×¶ÎµÄÖÜÆÚ=XTIMCLK
   //Zone6¶ÁÊ±Ðò
	XintfRegs.XTIMING2.bit.XRDLEAD = 3;    //¶Á²Ù×÷Òýµ¼½×¶ÎµÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING2.bit.XRDACTIVE=7;	    //¶Á²Ù×÷¼¤»î½×¶ÎµÈ´ý×´Ì¬µÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING2.bit.XRDTRAIL = 3;   //¶Á²Ù×÷¸ú×Ù½×¶ÎµÄÖÜÆÚ=XTIMCLK
	//²»¼Ó±¶Ê±Ðò
	XintfRegs.XTIMING2.bit.X2TIMING = 0; 
   //Zone6²»¶ÔREADYÐÅºÅ²ÉÑù
    XintfRegs.XTIMING2.bit.USEREADY = 0;
    XintfRegs.XTIMING2.bit.READYMODE = 0;
    XintfRegs.XTIMING2.bit.XSIZE = 3;   //16Î»½Ó¿Ú£¬Î¨Ò»ÓÐÐ§µÄÉèÖÃ

    XintfRegs.XBANK.bit.BANK = 2;   //Ê¹ÄÜÄÚ²¿ÇøÓò6
    XintfRegs.XBANK.bit.BCYC = 3;  //Á¬Ðø·ÃÎÊÖ®¼äÌí¼ÓÁË3¸öXTIMCLKÖÜÆÚ */


	//Zone6Ð´Ê±Ðò
	XintfRegs.XTIMING6.bit.XWRLEAD = 3;    //Ð´²Ù×÷Òýµ¼½×¶ÎµÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING6.bit.XWRACTIVE = 7;   //Ð´²Ù×÷¼¤»î½×¶ÎµÈ´ý×´Ì¬µÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING6.bit.XWRTRAIL = 3;    //Ð´²Ù×÷¸ú×Ù½×¶ÎµÄÖÜÆÚ=XTIMCLK
   //Zone6¶ÁÊ±Ðò
	XintfRegs.XTIMING6.bit.XRDLEAD = 3;    //¶Á²Ù×÷Òýµ¼½×¶ÎµÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING6.bit.XRDACTIVE=7;	    //¶Á²Ù×÷¼¤»î½×¶ÎµÈ´ý×´Ì¬µÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING6.bit.XRDTRAIL = 3;   //¶Á²Ù×÷¸ú×Ù½×¶ÎµÄÖÜÆÚ=XTIMCLK
	//²»¼Ó±¶Ê±Ðò
	XintfRegs.XTIMING6.bit.X2TIMING = 0; 
   //Zone6²»¶ÔREADYÐÅºÅ²ÉÑù
    XintfRegs.XTIMING6.bit.USEREADY = 0;
    XintfRegs.XTIMING6.bit.READYMODE = 0;
    XintfRegs.XTIMING6.bit.XSIZE = 3;   //16Î»½Ó¿Ú£¬Î¨Ò»ÓÐÐ§µÄÉèÖÃ

    XintfRegs.XBANK.bit.BANK = 6;   //Ê¹ÄÜÄÚ²¿ÇøÓò6
    XintfRegs.XBANK.bit.BCYC = 3;  //Á¬Ðø·ÃÎÊÖ®¼äÌí¼ÓÁË3¸öXTIMCLKÖÜÆÚ
	
}



///////////////////////////////////////////////////////
interrupt void eva_timer1_isr(void)
{  DINT;

  if(PieCtrl.PIEACK.bit.ACK2==1)
  { PieCtrl.PIEACK.bit.ACK2=1;
    count++; 
    EvaRegs.EVAIMRA.bit.T1PINT = 1;
    EvaRegs.EVAIFRA.bit.T1PINT = 1;
  }

  EINT;
}


///////////Ö±½Ó·µ»ØÖÐ¶Ï·þÎñ³ÌÐò//////////////
interrupt void nothing(void)
{ EINT;
  return;
}

/////////////Ö÷³ÌÐò//////////////////////////////////////

main()
{  InitSysCtrl();

   DINT;
   IER = 0x0000;
   IFR = 0x0000;

   InitPieCtrl();
   InitPieVectTable();
   InitGpio();
   init_eva_timer1();
   InitXintf();
		
   EALLOW;
   PieVectTable.T1PINT=&eva_timer1_isr;
   EDIS;  
  
   //¿ªÖÐ¶Ï
   IER |= M_INT2;
   PieCtrl.PIEIER2.bit.INTx4=1;
   EINT;  
   ERTM;

   EvaRegs.T1CON.bit.TENABLE=1;  // ´ò¿ª¶¨Ê±Æ÷1

   rambase=(Uint16 *)RAMBASE;
    
   for(;;)
   { if(count>=20)
     { count=0;
       *rambase=tmp&0xff;
       for(i=0;i<0x62ff;i++) {}
       rambase++;
	 }
   }	 
}  
