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
////////////////变量申明/////////////////////////////////

Uint32 i=0;  
Uint32 j=0;            
Uint16 count=0; 
Uint16 t=1090;        //加热时间
Uint16 t1=16895;      //每个周期断开时间24000-7*1000-105=16895


/////////////////函数申明/////////////////////////////////

void InitSysCtrl(void);
void InitGpio(void);
void init_eva_timer1(void);
interrupt void eva_timer1_isr(void);


//////////////////////系统初始化//////////////////////////////////

void InitSysCtrl(void)
{
   EALLOW;
   
   DevEmuRegs.M0RAMDFT = 0x0300;
   DevEmuRegs.M1RAMDFT = 0x0300;
   DevEmuRegs.L0RAMDFT = 0x0300;
   DevEmuRegs.L1RAMDFT = 0x0300;
   DevEmuRegs.H0RAMDFT = 0x0300;
        
   SysCtrlRegs.WDCR= 0x0068;   //禁止看门狗

   SysCtrlRegs.PLLCR = 0x8;   //主频=30*8/2=120MHz
  
   for(i=0; i<5000; i++){}   // 为了频率稳定，必须延时一段时间
      
   SysCtrlRegs.HISPCP.all = 0x0002;  //高速外设时钟=120/4=30
   SysCtrlRegs.LOSPCP.all = 0x0002;	 //低速外设时钟=120/4=30
 
   SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;  //使能EVA模块的高速外设时钟
				
   EDIS;
	
}


///////////////////IO口初始化//////////////////////////////////

void InitGpio(void)
{

   EALLOW;
   GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOA设置为I/O口 
   GpioMuxRegs.GPADIR.all=0xFFFF;   //A0-A15设置为输出	
   GpioDataRegs.GPADAT.all=0x0;
   EDIS;

}	

///////////////////定时器1初始化（10ms）//////////////////////////////////

void init_eva_timer1(void)
{
  
   EvaRegs.GPTCONA.all = 0;   //初始化定时器1并设置定时器1寄存器
   EvaRegs.EVAIMRA.bit.T1PINT = 1;  //允许定逼�1周期中断
   EvaRegs.EVAIFRA.bit.T1PINT = 1;  //清定�1周期中断标志
             
   EvaRegs.T1CON.all = 0x170C;  //Timer1为连续增计数模式，预分频值128
	                             //使用内部高速外设时钟
   EvaRegs.T1PR = 0x927;   //Timer1的期寄存器=0.01*30MHz/128
   EvaRegs.T1CNT = 0x0000;  //Timer1的计数�

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

///////////直接返回中断服务程序//////////////
interrupt void nothing(void)
{ EINT;
  return;
}

/////////////主程序//////////////////////////////////////

main()
{  
   InitSysCtrl();	
  	//把InitFlash（）函数搬移到ram中运行
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
  
   //开中断
   IER |= M_INT2;
   PieCtrl.PIEIER2.bit.INTx4=1;
   EINT;  
   ERTM;
   
   EvaRegs.T1CON.bit.TENABLE=1;  // 打开定时器1

   /////////////////////////刘翔测试程序///////////////////////
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
	   j++;    //周期循环次数
	 }
	 if(j>=1)
	 {
	   j=0;
	   break;
	 } 
   }
   
   for(;;){}

   ////////////////小马测试程序//////////////////////////////////

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
