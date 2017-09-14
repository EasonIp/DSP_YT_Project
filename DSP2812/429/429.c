#include "DSP28_Device.h"

#define RAMBASE6 0x0100000      //×ÜÏßµØÖ·,¶ÔÓ¦ÓÚÆ¬Ñ¡ÐÅºÅXZCS6 

/*#pragma CODE_SECTION(InitFlash, "secureRamFuncs")
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
}*/


////////////////±äÁ¿ÉêÃ÷/////////////////////////////////

Uint32 i;                  //±äÁ¿
Uint16 count=0;            
/*Uint16 *rambase6; 
Uint16 CSH;               //429³õÊ¼»¯
Uint16 FSDZJ_271;         //·¢ËÍLABLE271µÍ×Ö½Ú
Uint16 FSGZJ_271;         //·¢ËÍLABLE271¸ß×Ö½Ú
Uint16 FSDZJ_272;
Uint16 FSGZJ_272; 
Uint16 FSDZJ_273;
Uint16 FSGZJ_273; 
Uint16 FSDZJ1;
Uint16 FSGZJ1;
Uint16 JSDZJ;			     //½ÓÊÕµÍ×Ö½Ú
Uint16 JSGZJ;			     //½ÓÊÕ¸ß×Ö½Ú*/
char flag=0;

/////////////////º¯ÊýÉêÃ÷/////////////////////////////////

void InitSysCtrl(void);
void InitGpio(void);
void init_eva_timer1(void);
//void InitXintf(void);
//void InitXINT1(void);
//void arinc429fasong(void);
interrupt void eva_timer1_isr(void);
//interrupt void XINT1_isr(void);



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
   SysCtrlRegs.PCLKCR.bit.ADCENCLK=1;  //Ê¹ÄÜADCÄ£¿éµÄ¸ßËÙÍâÉèÊ±ÖÓ
   SysCtrlRegs.PCLKCR.bit.ECANENCLK=1; //Ê¹ÄÜCANÍâÉèµÄÏµÍ³Ê±ÖÓ
				
   EDIS;
	
}


///////////////////IO¿Ú³õÊ¼»¯//////////////////////////////////

void InitGpio(void)
{

   EALLOW;
   /*GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOAÉèÖÃÎªI/O¿Ú 
   GpioMuxRegs.GPADIR.all=0xFFFF;   //A0-A15ÉèÖÃÎªÊä³ö	
   GpioDataRegs.GPADAT.all=0x1e; */


   //////Ð¡³ÌÐò////////////////
   GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOAÉèÖÃÎªI/O¿Ú 
   GpioMuxRegs.GPADIR.bit.GPIOA2=0;
   GpioMuxRegs.GPADIR.bit.GPIOA3=1;
   GpioDataRegs.GPADAT.bit.GPIOA3=0;
   ///////////////////////////////////////

   EDIS;

}	

///////////////////Íâ²¿ÖÐ¶Ï1³õÊ¼»¯//////////////////////////////////

/*void InitXINT1(void)
{  GpioMuxRegs.GPEMUX.bit.XINT1_XBIO_GPIOE0=1; //ÉèÖÃÍâ¿ÖÐ¶Ï1¹¦ÄÜ 
   XIntruptRegs.XINT1CR.all=0x5;      //ÉÏÉýÑØÉú³ÉÖÐ¶Ï
}*/

///////////////////¶¨Ê±Æ÷1³õÊ¼»¯£¨100mS©//////////////////////////////////

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
/*void InitXintf(void)
{  
    XintfRegs.XINTCNF2.bit.XTIMCLK = 0x0001;  //ËùÓÐzoneµÄÊ±ÐòÊÇ»ùÓÚXTIMCLK = SYSCLKOUT/2 
    XintfRegs.XINTCNF2.bit.WRBUFF = 3;   //ÉèÖÃ3¸öÐ´»º³å
	XintfRegs.XINTCNF2.bit.CLKOFF = 0;  //Ê¹ÄÜXCLKOUT
	XintfRegs.XINTCNF2.bit.CLKMODE = 0;  //ÉèÖÃXCLKOUT=XTIMCLK
    	
	XintfRegs.XTIMING2.bit.X2TIMING=1;
	//Zone2Ð´Ê±Ðò
	XintfRegs.XTIMING2.bit.XWRLEAD = 1;    //Ð´²Ù×÷Òýµ¼½×¶ÎµÄÖÜÆÚ=2*XTIMCLK
	XintfRegs.XTIMING2.bit.XWRACTIVE = 7;   //Ð´²Ù×÷¼¤»î½×¶ÎµÈ´ý×´Ì¬µÄÖÜÆÚ=14*XTIMCLK
	XintfRegs.XTIMING2.bit.XWRTRAIL = 2;    //Ð´²Ù×÷¸ú×Ù½×¶ÎµÄÖÜÆÚ=4*XTIMCLK
   //Zone2¶ÁÊ±Ðò
	XintfRegs.XTIMING2.bit.XRDLEAD = 1;    //¶Á²Ù×÷Òýµ¼½×¶ÎµÄÖÜÆÚ=2*XTIMCLK
	XintfRegs.XTIMING2.bit.XRDACTIVE=7;	    //¶Á²Ù×÷¼¤»î½×¶ÎµÈ´ý×´Ì¬µÄÖÜÆÚ=14*XTIMCLK
	XintfRegs.XTIMING2.bit.XRDTRAIL = 2;   //¶Á²Ù×÷¸ú×Ù½×¶ÎµÄÖÜÆÚ=4*XTIMCLK
	//²»¼Ó±¶Ê±Ðò
	XintfRegs.XTIMING2.bit.X2TIMING = 0; 
   //Zone2²»¶ÔREADYÐÅºÅ²ÉÑù
    XintfRegs.XTIMING2.bit.USEREADY = 0;
    XintfRegs.XTIMING2.bit.READYMODE = 0;
    XintfRegs.XTIMING2.bit.XSIZE = 3;   //16Î»½Ó¿Ú£¬Î¨Ò»ÓÐÐ§µÄÉèÖÃ

    XintfRegs.XBANK.bit.BANK = 2;   //Ê¹ÄÜÄÚ²¿ÇøÓò2
    XintfRegs.XBANK.bit.BCYC = 3;  //Á¬Ðø·ÃÎÊÖ®¼äÌí¼ÓÁË3¸öXTIMCLKÖÜÆÚ

	//Zone6Ð´Ê±Ðò
	XintfRegs.XTIMING6.bit.XWRLEAD = 1;    //Ð´²Ù×÷Òýµ¼½×¶ÎµÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING6.bit.XWRACTIVE = 7;   //Ð´²Ù×÷¼¤»î½×¶ÎµÈ´ý×´Ì¬µÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING6.bit.XWRTRAIL = 2;    //Ð´²Ù×÷¸ú×Ù½×¶ÎµÄÖÜÆÚ=XTIMCLK
   //Zone6¶ÁÊ±Ðò
	XintfRegs.XTIMING6.bit.XRDLEAD = 1;    //¶Á²Ù×÷Òýµ¼½×¶ÎµÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING6.bit.XRDACTIVE=1;	    //¶Á²Ù×÷¼¤»î½×¶ÎµÈ´ý×´Ì¬µÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING6.bit.XRDTRAIL = 1;   //¶Á²Ù×÷¸ú×Ù½×¶ÎµÄÖÜÆÚ=XTIMCLK
	//²»¼Ó±¶Ê±Ðò
	XintfRegs.XTIMING6.bit.X2TIMING = 0; 
   //Zone6²»¶ÔREADYÐÅºÅ²ÉÑù
    XintfRegs.XTIMING6.bit.USEREADY = 0;
    XintfRegs.XTIMING6.bit.READYMODE = 0;
    XintfRegs.XTIMING6.bit.XSIZE = 3;   //16Î»½Ó¿Ú£¬Î¨Ò»ÓÐÐ§µÄÉèÖÃ

    XintfRegs.XBANK.bit.BANK = 6;   //Ê¹ÄÜÄÚ²¿ÇøÓò6
    XintfRegs.XBANK.bit.BCYC = 3;  //Á¬Ðø·ÃÎÊÖ®¼äÌí¼ÓÁË3¸öXTIMCLKÖÜÆÚ
	
}

/////////////429·¢ËÍ////////////////////////////////
void arinc429fasong(void)
{ 
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;   
   *rambase6=FSDZJ_271;
   for(i=0;i<5;i++){} 
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x8;
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;   
   *rambase6=FSGZJ_271;
   for(i=0;i<5;i++){}
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x10;
   
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;  
   *rambase6=FSDZJ_272;
   for(i=0;i<5;i++){} 
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x8;
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;   
   *rambase6=FSGZJ_272;
   for(i=0;i<5;i++){}
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x10;

   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;   
   *rambase6=FSDZJ_273;
   for(i=0;i<5;i++){} 
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x8;
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;   
   *rambase6=FSGZJ_273;
   for(i=0;i<5;i++){}
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x10;

   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;   
   *rambase6=FSDZJ1;
   for(i=0;i<5;i++){} 
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x8;
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;   
   *rambase6=FSGZJ1;
   for(i=0;i<5;i++){}
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x10;

}*/

///////////////////////////////////////////////////////
interrupt void eva_timer1_isr(void)
{  DINT;

  /*if(PieCtrl.PIEACK.bit.ACK2==1)
  { PieCtrl.PIEACK.bit.ACK2=1;
    count++; 
    if(count>=18)
    { count=0;
      arinc429fasong();
    }
   EvaRegs.EVAIMRA.bit.T1PINT = 1;
   EvaRegs.EVAIFRA.bit.T1PINT = 1;
  }*/

  if(PieCtrl.PIEACK.bit.ACK2==1)
  { PieCtrl.PIEACK.bit.ACK2=1;
    count++; 
   EvaRegs.EVAIMRA.bit.T1PINT = 1;
   EvaRegs.EVAIFRA.bit.T1PINT = 1;
  }

  EINT;
}




///////////////////429½ÓÊÕÖÐ¶Ï//////////////////////////////////

/*interrupt void XINT1_isr(void)
{ Uint16 ix;
  DINT;
  if(PieCtrl.PIEACK.bit.ACK1==1)
  { PieCtrl.PIEACK.bit.ACK1=1;
   		
    GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfffc; 
    for(ix=0;ix<10;ix++){}  
    JSDZJ=*rambase6;  
    for(ix=0;ix<10;ix++){} 
    GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
    for(ix=0;ix<5;ix++){} 
    GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfffd;
    for(ix=0;ix<10;ix++){}
    JSGZJ=*rambase6;
    for(ix=0;ix<10;ix++){}   
    GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
    
    FSDZJ1=JSDZJ;  
    FSGZJ1=JSGZJ;    
  }
  EINT;
} */


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
   //InitXINT1();	
   //InitXintf();
		
   EALLOW;
   //PieVectTable.XINT1=&XINT1_isr;
   PieVectTable.T1PINT=&eva_timer1_isr;
   EDIS;  
  
   //¿ªÖÐ¶Ï
   IER |= (M_INT1|M_INT2);
  // PieCtrl.PIEIER1.bit.INTx4=1;
   PieCtrl.PIEIER2.bit.INTx4=1;
   EINT;  
   ERTM;

   /*FSDZJ_271=0x00b9;   //³õÊ¼»¯429ÐÅºÅ
   FSGZJ_271=0x0000;  
   FSDZJ_272=0x40ba;   //ÉÏµçÄ¬ÈÏ×óÖ÷¿Ø 
   FSGZJ_272=0x0000;
   FSDZJ_273=0x00bb;  
   FSGZJ_273=0x0000;


   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffdf;  //³õÊ¼»¯3282
   CSH=0x0020;
   rambase6=(Uint16 *)RAMBASE6;
   *rambase6=CSH;
   for(i=0;i<5;i++){}
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x30;*/


   
     /*if(GpioDataRegs.GPADAT.bit.GPIOA2==1)
     { EvaRegs.T1CON.bit.TENABLE=1;  // ´ò¿ª¶¨Ê±Æ÷1
       while(1)
       { if(count>=100)
         { count=0;
	       GpioDataRegs.GPADAT.bit.GPIOA3=1;
		   break;
         }
       }     
     }  
     for(;;){} */


	 while(1)
	 { EvaRegs.T1CON.bit.TENABLE=1;  // ´ò¿ª¶¨Ê±Æ÷1
	   while(1)
	   { if(count>=10)
	     { count=0;
		   flag=1;
	       break;		
		 }
	   }
	   EvaRegs.T1CON.bit.TENABLE=0;
       if(flag==0)break;
	 }

	 for(;;){}

} 