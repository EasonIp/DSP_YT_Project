#include "DSP28_Device.h"

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;  

#pragma CODE_SECTION(InitFlash, "ramfuncs");

void InitFlash(void)
{
   asm(" EALLOW"); 
   FlashRegs.FPWR.bit.PWR = 3; 
   FlashRegs.FSTATUS.bit.V3STAT = 1; 
   FlashRegs.FOPT.bit.ENPIPE = 1;
   FlashRegs.FBANKWAIT.bit.RANDWAIT = 5;
   FlashRegs.FBANKWAIT.bit.PAGEWAIT = 5;
   FlashRegs.FSTDBYWAIT.bit.STDBYWAIT = 0x01FF;
   FlashRegs.FACTIVEWAIT.bit.ACTIVEWAIT = 0x01FF;
  
   asm(" EDIS");
   asm(" RPT #7 || NOP");
}
////////////////±äÁ¿ÉêÃ÷/////////////////////////////////

Uint32 i=0;  
Uint32 j=0;            
Uint16 count=0; 
Uint16 t=1090;        //¼ÓÈÈÊ±¼ä
Uint16 t1=16895;      //Ã¿¸öÖÜÆÚ¶Ï¿ªÊ±¼ä24000-7*1000-105=16895


/////////////////º¯ÊýÉêÃ÷/////////////////////////////////

void InitSysCtrl(void);
void InitGpio(void);
void init_eva_timer1(void);
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
				
   EDIS;
	
}


///////////////////IO¿Ú³õÊ¼»¯//////////////////////////////////

void InitGpio(void)
{

   EALLOW;
   GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOAÉèÖÃÎªI/O¿Ú 
   GpioMuxRegs.GPADIR.all=0xFFFF;   //A0-A15ÉèÖÃÎªÊä³ö	
   GpioDataRegs.GPADAT.all=0x0;
   EDIS;

}	

///////////////////¶¨Ê±Æ÷1³õÊ¼»¯£¨10ms£©//////////////////////////////////

void init_eva_timer1(void)
{
  
   EvaRegs.GPTCONA.all = 0;   //³õÊ¼»¯¶¨Ê±Æ÷1²¢ÉèÖÃ¶¨Ê±Æ÷1¼Ä´æÆ÷
   EvaRegs.EVAIMRA.bit.T1PINT = 1;  //ÔÊÐí¶¨±Æ÷1ÖÜÆÚÖÐ¶Ï
   EvaRegs.EVAIFRA.bit.T1PINT = 1;  //Çå¶¨÷1ÖÜÆÚÖÐ¶Ï±êÖ¾
             
   EvaRegs.T1CON.all = 0x170C;  //Timer1ÎªÁ¬ÐøÔö¼ÆÊýÄ£Ê½£¬Ô¤·ÖÆµÖµ128
	                             //Ê¹ÓÃÄÚ²¿¸ßËÙÍâÉèÊ±ÖÓ
   EvaRegs.T1PR = 0x927;   //Timer1µÄÆÚ¼Ä´æÆ÷=0.01*30MHz/128
   EvaRegs.T1CNT = 0x0000;  //Timer1µÄ¼ÆÊýã

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
{  
   InitSysCtrl();	
  	//°ÑInitFlash£¨£©º¯Êý°áÒÆµ½ramÖÐÔËÐÐ
   MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);	
   InitFlash();

   DINT;
   IER = 0x0000;
   IFR = 0x0000;

   InitPieCtrl();
   InitPieVectTable();
   InitGpio();
   init_eva_timer1();

   EALLOW;
   PieVectTable.T1PINT=&eva_timer1_isr;
   EDIS;  
  
   //¿ªÖÐ¶Ï
   IER |= M_INT2;
   PieCtrl.PIEIER2.bit.INTx4=1;
   EINT;  
   ERTM;
   
   EvaRegs.T1CON.bit.TENABLE=1;  // ´ò¿ª¶¨Ê±Æ÷1

   /////////////////////////ÁõÏè²âÊÔ³ÌÐò///////////////////////
   for(;;)  
   { if((count>0)&&(count<=t))
     { GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x1;		 //GPIOA0=1
	 }
     if((count>t)&&(count<=(t+15)))
     { GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0;       //GPIOA0=0
	 }
     if((count>(t+15))&&(count<=(2*t+15)))
     { GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x2;   	  //GPIOA1=1
	 }
     if((count>(2*t+15))&&(count<=(2*t+30)))
     { GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0;       //GPIOA1=0
	 } 
     if((count>(2*t+30))&&(count<=(3*t+30)))
     { GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x4;    	   //GPIOA2=1
	 }
     if((count>(3*t+30))&&(count<=(3*t+45)))
     { GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0;        //GPIOA2=0
	 }
     if((count>(3*t+45))&&(count<=(4*t+45)))
     { GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x8;    	  //GPIOA3=1
	 }
     if((count>(4*t+45))&&(count<=(4*t+60)))
     { GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0;        //GPIOA3=0
	 }
     if((count>(4*t+60))&&(count<=(5*t+60)))
     { GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x10;      //GPIOA4=1
	 }
     if((count>(5*t+60))&&(count<=(5*t+75)))
     { GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0;        //GPIOA4=0
	 }
     if((count>(5*t+75))&&(count<=(6*t+75)))
     { GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x20;        //GPIOA5=1
	 }
     if((count>(6*t+75))&&(count<=(6*t+90)))
     { GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0;        //GPIOA5=0
	 }
     if((count>(6*t+90))&&(count<=(7*t+90)))
     { GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x40;     	//GPIOA6=1
	 }
     if((count>(7*t+90))&&(count<=(7*t+105)))
     { GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0;         //GPIOA6=0
	 } 
     if((count>(7*t+105))&&(count<=(7*t+105+t1)))
     { GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0;
	   count=0;
	   j++;    //ÖÜÆÚÑ­»·´ÎÊý
	 }
	 if(j>=1)
	 {
	   j=0;
	   break;
	 } 
   }
   
   for(;;){}

   ////////////////Ð¡Âí²âÊÔ³ÌÐò//////////////////////////////////

   /*for(;;)
   { if((count>0)&&(count<=15))
     { GpioDataRegs.GPADAT.bit.GPIOA0=1;
	 }
     if((count>15)&&(count<=(t+15)))
     { GpioDataRegs.GPADAT.bit.GPIOA0=0;
	 }
     if((count>(t+15))&&(count<=(t+30)))
     { GpioDataRegs.GPADAT.bit.GPIOA0=1;
	 }
     if((count>(t+30))&&(count<=(2*t+30)))
     { GpioDataRegs.GPADAT.bit.GPIOA0=0;
	 } 
     if((count>(2*t+30))&&(count<=(2*t+45)))
     { GpioDataRegs.GPADAT.bit.GPIOA0=1;
	 }
     if((count>(2*t+45))&&(count<=(3*t+45)))
     { GpioDataRegs.GPADAT.bit.GPIOA0=0;
	 }
     if((count>(3*t+45))&&(count<=(3*t+60)))
     { GpioDataRegs.GPADAT.bit.GPIOA0=1;
	 }
     if((count>(3*t+60))&&(count<=(4*t+60)))
     { GpioDataRegs.GPADAT.bit.GPIOA0=0;
	 }
     if((count>(4*t+60))&&(count<=(4*t+75)))
     { GpioDataRegs.GPADAT.bit.GPIOA0=1;
	 }
     if((count>(4*t+75))&&(count<=(5*t+75)))
     { GpioDataRegs.GPADAT.bit.GPIOA0=0;
	 }
     if((count>(5*t+75))&&(count<=(5*t+90)))
     { GpioDataRegs.GPADAT.bit.GPIOA0=1;
	 }
     if((count>(5*t+90))&&(count<=(6*t+90)))
     { GpioDataRegs.GPADAT.bit.GPIOA0=0;
	 }
     if((count>(6*t+90))&&(count<=(6*t+105)))
     { GpioDataRegs.GPADAT.bit.GPIOA0=1;
	 }
     if((count>(6*t+105))&&(count<=(7*t+105)))
     { GpioDataRegs.GPADAT.bit.GPIOA0=0;
	 }
     if((count>(7*t+105))&&(count<=(7*t+120)))
     { GpioDataRegs.GPADAT.bit.GPIOA0=1;
	 }
     if(count>(7*t+120))
     { GpioDataRegs.GPADAT.bit.GPIOA0=0;
	   count=0;
	   break;
	 }	 	  
   }*/
 
} 
