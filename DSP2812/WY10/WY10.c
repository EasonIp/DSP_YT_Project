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
Uint16 count=0; 

Uint16 ONT1=0;
Uint16 ONT2=0;
Uint16 ONT3=0;
Uint16 ONT4=0;
Uint16 ONT5=0;
Uint16 wait1=0;
Uint16 wait2=0;
Uint16 DK1=0;
Uint16 DK2=0;
Uint16 DK3=0;

int OAT=-5;

/////////////////º¯ÊýÉêÃ÷/////////////////////////////////

void InitSysCtrl(void);
void InitGpio(void);
void init_eva_timer1(void);
interrupt void eva_timer1_isr(void);
Uint16 sjzh(float sj);
void Time_Count(void);

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
   GpioMuxRegs.GPADIR.all=0xFF1F;   	
   GpioDataRegs.GPADAT.all=0x0;
   EDIS;

   if((GpioDataRegs.GPADAT.bit.GPIOA6==0)&&(GpioDataRegs.GPADAT.bit.GPIOA7==0))
   {
     OAT=-5;
   }
   else if((GpioDataRegs.GPADAT.bit.GPIOA6==0)&&(GpioDataRegs.GPADAT.bit.GPIOA7==1))
   {
     OAT=-10;
   }
   else if((GpioDataRegs.GPADAT.bit.GPIOA6==1)&&(GpioDataRegs.GPADAT.bit.GPIOA7==0))
   {
     OAT=-20;
   }
   else
   {
     OAT=-5;
   }      
}	

///////////////////¶¨Ê±Æ÷1³õÊ¼»¯£¨100ms£©//////////////////////////////////

void init_eva_timer1(void)
{
  
   EvaRegs.GPTCONA.all = 0;   //³õÊ¼»¯¶¨Ê±Æ÷1²¢ÉèÖÃ¶¨Ê±Æ÷1¼Ä´æÆ÷
   EvaRegs.EVAIMRA.bit.T1PINT = 1;  //ÔÊÐí¶¨±Æ÷1ÖÜÆÚÖÐ¶Ï
   EvaRegs.EVAIFRA.bit.T1PINT = 1;  //Çå¶¨÷1ÖÜÆÚÖÐ¶Ï±êÖ¾
             
   EvaRegs.T1CON.all = 0x170C;  //Timer1ÎªÁ¬ÐøÔö¼ÆÊýÄ£Ê½£¬Ô¤·ÖÆµÖµ128
	                             //Ê¹ÓÃÄÚ²¿¸ßËÙÍâÉèÊ±ÖÓ
   EvaRegs.T1PR = 0x5b86;   //Timer1µÄÆÚ¼Ä´æÆ÷=0.1*30MHz/128
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


/////////////////////////////////////
void Time_Count(void)
{
  float T1=0;
  float T2=0;
  float T3=0;
  float T4=0;
  float T5=0;
  float off1=0;
  float off2=0;
  float off3=0;

  if(OAT>-15)  //warm
  {
    T1=-0.326*OAT+1.37;
    T2=-0.4*OAT;
    T3=-0.6*OAT-1;
    T4=-0.4*OAT;
    T5=-0.4*OAT+1;
	wait1=10*(2*OAT+30);
    wait2=10*(OAT+15);
	
	if(T1<1)
	{
      T1=1;
	}
	if(T2<1)
	{
      T2=1;
	}
	if(T3<1)
	{
      T3=1;
	}
	if(T4<1)
	{
      T4=1;
	}
	if(T5<1)
	{
      T5=1;
	} 
	off1=2*(T2+T3+T4);
    off2=2*(T2+T3+T4);
	off3=2*(T1+T5);
  }
  else      //cold
  {
    T1=-0.93*OAT-4.67;
    T2=-1*OAT-6.5;
    T3=-0.87*OAT-4.33;
    T4=-0.7*OAT-3;
    T5=-0.7*OAT-3;

	if(T1>18.5)
	{
      T1=18.5;
	}
	if(T2>17.5)
	{
      T2=17.5;
	}
	if(T3>16.5)
	{
      T3=16.5;
	}
	if(T4>16.5)
	{
      T4=16.5;
	}
	if(T5>17.5)
	{
      T5=17.5;
	}  
	off1=2*(T2+T3+T4+T5);
    off2=2*(T2+T3+T4+T1);
  }
  
  ONT1=sjzh(T1);
  ONT2=sjzh(T2);
  ONT3=sjzh(T3);
  ONT4=sjzh(T4);
  ONT5=sjzh(T5);
  DK1=sjzh(off1);
  DK2=sjzh(off2);
  DK3=sjzh(off3);
}


//////////////////////////
Uint16 sjzh(float sj)
{
  float a; 
  Uint16 JG;

  a=10*sj;
  a=(Uint16)a+0.5;
    
  if(a>10*sj)
  { 
    a=a-0.5;
  }
  else
  { 
    a=a+0.5;
  }
  JG=(Uint16)a;
  return(JG);
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
      
  Time_Count();
  
  EvaRegs.T1CON.bit.TENABLE=1;  // ´ò¿ª¶¨Ê±Æ÷1
  
  while(1)
  { 
    if(OAT<-15)  //cold
    {
      if((count>0)&&(count<=ONT2))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0|0x2;    //T2
	  }
      if((count>ONT2)&&(count<=(ONT2+ONT3)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0|0x4;   //T3
	  } 
      if((count>(ONT2+ONT3))&&(count<=(ONT2+ONT3+ONT4)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0|0x8;   //T4
	  } 
      if((count>(ONT2+ONT3+ONT4))&&(count<=(ONT2+ONT3+ONT4+ONT5)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0|0x10;  //T5
	  } 
      if((count>(ONT2+ONT3+ONT4+ONT5))&&(count<=(2*ONT2+ONT3+ONT4+DK1)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0;  
	  }
      if((count>(ONT2+ONT3+ONT4+ONT5+DK1))&&(count<=(2*ONT2+ONT3+ONT4+ONT5+DK1)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0|0x2;  //T2
	  }	
      if((count>(2*ONT2+ONT3+ONT4+ONT5+DK1))&&(count<=(2*ONT2+2*ONT3+ONT4+ONT5+DK1)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0|0x4;  //T3
	  }
      if((count>(2*ONT2+2*ONT3+ONT4+ONT5+DK1))&&(count<=(2*ONT2+2*ONT3+2*ONT4+ONT5+DK1)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0|0x8;   //T4
	  }
      if((count>(2*ONT2+2*ONT3+2*ONT4+ONT5+DK1))&&(count<=(2*ONT2+2*ONT3+2*ONT4+ONT5+DK1+ONT1)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0|0x1;  //T1
	  }
      if((count>(2*ONT2+2*ONT3+2*ONT4+ONT5+DK1+ONT1))&&(count<=(2*ONT2+2*ONT3+2*ONT4+ONT5+DK1+ONT1+DK2)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0;  
	  }
	  if(count>(2*ONT2+2*ONT3+2*ONT4+ONT5+DK1+ONT1+DK2))
	  {
	    count=0;
	  }	  	  	  	  	     
	}
	else      //warm
	{
      if((count>0)&&(count<=ONT2))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0|0x2;    //T2
	  }
      if((count>ONT2)&&(count<=(ONT2+ONT3)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0|0x4;   //T3
	  } 
      if((count>(ONT2+ONT3))&&(count<=(ONT2+ONT3+ONT4)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0|0x8;   //T4
	  } 
      if((count>(ONT2+ONT3+ONT4))&&(count<=(ONT2+ONT3+ONT4+DK1+wait1)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0;
	  } 
      if((count>(ONT2+ONT3+ONT4+DK1+wait1))&&(count<=(2*ONT2+ONT3+ONT4+DK1+wait1)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0|0x2;  //T2
	  }
      if((count>(2*ONT2+ONT3+ONT4+DK1+wait1))&&(count<=(2*ONT2+2*ONT3+ONT4+DK1+wait1)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0|0x4;  //T3
	  }	
      if((count>(2*ONT2+2*ONT3+ONT4+DK1+wait1))&&(count<=(2*ONT2+2*ONT3+2*ONT4+DK1+wait1)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0|0x8;  //T4
	  }
      if((count>(2*ONT2+2*ONT3+2*ONT4+DK1+wait1))&&(count<=(2*ONT2+2*ONT3+2*ONT4+DK1+wait1+DK2+wait2)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0;
	  }
      if((count>(2*ONT2+2*ONT3+2*ONT4+DK1+wait1+DK2+wait2))&&(count<=(2*ONT2+2*ONT3+2*ONT4+DK1+wait1+DK2+wait2+ONT1)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0|0x1;  //T1
	  }
      if((count>(2*ONT2+2*ONT3+2*ONT4+DK1+wait1+DK2+wait2+ONT1))&&(count<=(2*ONT2+2*ONT3+2*ONT4+DK1+wait1+DK2+wait2+ONT1+ONT5)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0|0x10;  //T5
	  }
      if((count>(2*ONT2+2*ONT3+2*ONT4+DK1+wait1+DK2+wait2+ONT1+ONT5))&&(count<=(2*ONT2+2*ONT3+2*ONT4+DK1+wait1+DK2+2*wait2+ONT1+ONT5+DK3)))
	  {
        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x0;
	  }
	  if(count>(2*ONT2+2*ONT3+2*ONT4+DK1+wait1+DK2+2*wait2+ONT1+ONT5+DK3))
	  {
	    count=0;
	  } 
	}
  } 
} 
