#include "DSP28_Device.h"

#define RAMBASE 0x100000      //总线地址,对应于片选信号XZCS6
//#define RAMBASE 0x080000      //总线地址,对应于片选信号XZCS2


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


////////////////变量申明/////////////////////////////////

Uint32 i=0;                  //变量
Uint16 count=0;            
Uint16 *rambase=0; 
Uint16 tmp=0x66;

/////////////////函数申明/////////////////////////////////

void InitSysCtrl(void);
void InitGpio(void);
void init_eva_timer1(void);
void InitXintf(void);
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
   SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;  //使能EVB模块的高速外设时钟
   SysCtrlRegs.PCLKCR.bit.SCIENCLKA=1;
				
   EDIS;
	
}


///////////////////IO口初始化//////////////////////////////////

void InitGpio(void)
{

   EALLOW;

   GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOA设置为I/O口 
   GpioMuxRegs.GPADIR.all=0xFFFF;   //A0-A15设置为输出	
   GpioDataRegs.GPADAT.all=0x0000; 

   EDIS;

}	


///////////////////定时器1初始化（100mS)//////////////////////////////////

void init_eva_timer1(void)
{
  
   EvaRegs.GPTCONA.all = 0;   //初始化定时器1并设置定时器1寄存器
   EvaRegs.EVAIMRA.bit.T1PINT = 1;  //允许定逼�1周期中断
   EvaRegs.EVAIFRA.bit.T1PINT = 1;  //清定�1周期中断标志
             
   EvaRegs.T1CON.all = 0x170C;  //Timer1为连续增计数模式，预分频值128
	                             //使用内部高速外设时钟
   EvaRegs.T1PR = 0x5b8d;   //Timer1的期寄存器=0.1*30MHz/128
   EvaRegs.T1CNT = 0x0000;  //Timer1的计数�

}

	
////////////////////外部接口初始化/////////////////////////
void InitXintf(void)
{  
    XintfRegs.XINTCNF2.bit.XTIMCLK = 0x0001;  //所有zone的时序是基于XTIMCLK = SYSCLKOUT/2 
    XintfRegs.XINTCNF2.bit.WRBUFF = 3;   //设置3个写缓冲
	XintfRegs.XINTCNF2.bit.CLKOFF = 0;  //使能XCLKOUT
	XintfRegs.XINTCNF2.bit.CLKMODE = 0;  //设置XCLKOUT=XTIMCLK
    	

	//Zone2写时序
	/*XintfRegs.XTIMING2.bit.XWRLEAD = 3;    //写操作引导阶段的周期=XTIMCLK
	XintfRegs.XTIMING2.bit.XWRACTIVE = 7;   //写操作激活阶段等待状态的周期=XTIMCLK
	XintfRegs.XTIMING2.bit.XWRTRAIL = 3;    //写操作跟踪阶段的周期=XTIMCLK
   //Zone6读时序
	XintfRegs.XTIMING2.bit.XRDLEAD = 3;    //读操作引导阶段的周期=XTIMCLK
	XintfRegs.XTIMING2.bit.XRDACTIVE=7;	    //读操作激活阶段等待状态的周期=XTIMCLK
	XintfRegs.XTIMING2.bit.XRDTRAIL = 3;   //读操作跟踪阶段的周期=XTIMCLK
	//不加倍时序
	XintfRegs.XTIMING2.bit.X2TIMING = 0; 
   //Zone6不对READY信号采样
    XintfRegs.XTIMING2.bit.USEREADY = 0;
    XintfRegs.XTIMING2.bit.READYMODE = 0;
    XintfRegs.XTIMING2.bit.XSIZE = 3;   //16位接口，唯一有效的设置

    XintfRegs.XBANK.bit.BANK = 2;   //使能内部区域6
    XintfRegs.XBANK.bit.BCYC = 3;  //连续访问之间添加了3个XTIMCLK周期 */


	//Zone6写时序
	XintfRegs.XTIMING6.bit.XWRLEAD = 3;    //写操作引导阶段的周期=XTIMCLK
	XintfRegs.XTIMING6.bit.XWRACTIVE = 7;   //写操作激活阶段等待状态的周期=XTIMCLK
	XintfRegs.XTIMING6.bit.XWRTRAIL = 3;    //写操作跟踪阶段的周期=XTIMCLK
   //Zone6读时序
	XintfRegs.XTIMING6.bit.XRDLEAD = 3;    //读操作引导阶段的周期=XTIMCLK
	XintfRegs.XTIMING6.bit.XRDACTIVE=7;	    //读操作激活阶段等待状态的周期=XTIMCLK
	XintfRegs.XTIMING6.bit.XRDTRAIL = 3;   //读操作跟踪阶段的周期=XTIMCLK
	//不加倍时序
	XintfRegs.XTIMING6.bit.X2TIMING = 0; 
   //Zone6不对READY信号采样
    XintfRegs.XTIMING6.bit.USEREADY = 0;
    XintfRegs.XTIMING6.bit.READYMODE = 0;
    XintfRegs.XTIMING6.bit.XSIZE = 3;   //16位接口，唯一有效的设置

    XintfRegs.XBANK.bit.BANK = 6;   //使能内部区域6
    XintfRegs.XBANK.bit.BCYC = 3;  //连续访问之间添加了3个XTIMCLK周期
	
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
  
   //开中断
   IER |= M_INT2;
   PieCtrl.PIEIER2.bit.INTx4=1;
   EINT;  
   ERTM;

   EvaRegs.T1CON.bit.TENABLE=1;  // 打开定时器1

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
