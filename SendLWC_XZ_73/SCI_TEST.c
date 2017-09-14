#include "DSP28_Device.h"

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;


#pragma CODE_SECTION(InitFlash,"ramfuncs");


//////////////InitFlash////////////////////
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
////////////////Define and State///////////////////
unsigned int i = 0; 
unsigned int j = 0; 
unsigned int count = 0;
unsigned int sj[9] = {0,0,0,0,0,0,0,0,0};
char LWC = 0x11;
char ICE = 0x22;
char ERROR1= 0x33;
char ERROR2 = 0x44;

void InitSci(void); 
void E422_Send(char sj);
void init_eva_timer1(void);
interrupt void eva_timer1_isr(void);

/////////////InitTimer1//////////////////////////
void init_eva_timer1(void)
{
  EvaRegs.GPTCONA.all = 0;
  EvaRegs.EVAIMRA.bit.T1PINT = 1;
  EvaRegs.EVAIFRA.bit.T1PINT = 1;
  EvaRegs.T1CON.all = 0x170C;
  EvaRegs.T1PR = 0x16E3; 
  EvaRegs.T1CNT = 0x0000;
}

///////////InitSCI////////////////////////////
void InitSci(void)
{
  EALLOW;
  GpioMuxRegs.GPFMUX.all = 0x0030;
  GpioMuxRegs.GPGMUX.all = 0x0030;
  SciaRegs.SCICTL1.all = 0x23;

    SciaRegs.SCIFFTX.all=0xe040;    //允许接收,使能FIFO
    SciaRegs.SCIFFRX.all=0x2021;    //使能FIFO接收
    SciaRegs.SCIFFCT.all=0x0000;    //禁止波特率校验
    SciaRegs.SCICCR.all = 0x27;     //1个停止位,奇校验,有效数据长度8位	
    SciaRegs.SCICTL1.all = 0x03;    //复位
    SciaRegs.SCICTL2.all = 0x03;    //复位  	
    SciaRegs.SCIHBAUD = 0x01;     //设置波特率9600bps
    SciaRegs.SCILBAUD = 0x86;	
    SciaRegs.SCICTL1.all = 0x23;   //使能发送接收功能  


  EDIS;
}


/////////////SendE422/////////////////////
void E422_Send(char sj)
{
  SciaRegs.SCITXBUF = sj;
  for(i=0;i<0xFF;i++);
}

////////////EvaTimer1Interrupt////////////
interrupt void eva_timer1_isr(void)
{
   DINT;
   if(PieCtrl.PIEACK.bit.ACK2 == 1)
   {
      PieCtrl.PIEACK.bit.ACK2 = 1;
	  count++;
	  EvaRegs.EVAIMRA.bit.T1PINT = 1;
	  EvaRegs.EVAIFRA.bit.T1PINT = 1;
   }
   EINT;
}

void InitSysCtrl(void)
{    
  EALLOW;   
  DevEmuRegs.M0RAMDFT = 0x0300;
  DevEmuRegs.M1RAMDFT = 0x0300;
  DevEmuRegs.L0RAMDFT = 0x0300;
  DevEmuRegs.L1RAMDFT = 0x0300;
  DevEmuRegs.H0RAMDFT = 0x0300;
        
  SysCtrlRegs.WDCR= 0x0068;   //禁止看门狗
  SysCtrlRegs.PLLCR = 0x8;    //主频=30*8/2=120MHz  
  for(i=0;i<5000;i++);                 //为了频率稳定延时一段时间      
  SysCtrlRegs.HISPCP.all = 0x0002;  //高速外设时钟=120/4=30MHz
  SysCtrlRegs.LOSPCP.all = 0x0002;	 //低速外设时钟=120/4=30MHz
 
  SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;   //使能EVA模块的高速外设时钟
  SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;   //使能EVB模块的高速外设时钟
  SysCtrlRegs.PCLKCR.bit.ADCENCLK=1;   //使能ADC模块的高速外设时钟
  SysCtrlRegs.PCLKCR.bit.SCIENCLKA=1;  //使能SCIA模块的高速外设时钟				
  EDIS;	
} 

//////////////Main/////////////////
void main(void)
{
  volatile int idata;
   /*系统初始化*/
   InitSysCtrl();
   /*把InitFlash()函数搬移到ram中运行*/
   MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
   InitFlash();
   /*关中断*/
   DINT; 
   IER = 0x0000;
   IFR = 0x0000;

   /*初始化PIE中断*/
   InitPieCtrl();
   /*初始化PIE中断矢量表*/
   InitPieVectTable();
   init_eva_timer1();
   /*初始化SCI寄存器*/
   InitSci();
   /*设置中断服务程序入口地址*/
   EALLOW; 
   PieVectTable.T1PINT = &eva_timer1_isr;
   EDIS;
   
   /*初始化GPIO*/
   EALLOW;   
   GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOA设置为I/O口 
   GpioMuxRegs.GPADIR.all=0x00c0;    //均为输入	
   GpioDataRegs.GPADAT.all=0x0000;   /*初始状态均为低电平*/ 
   EDIS; 

   /*开中断*/
   IER |= M_INT2;
   PieCtrl.PIEIER2.bit.INTx4 = 1;

   EINT;
   ERTM;

   EvaRegs.T1CON.bit.TENABLE = 1;
  
   LWC = 0x00;
   idata=0;
   for(;;)
   { 
     idata=GpioDataRegs.GPADAT.all&0x00f0;
     switch(idata)
	 {
	    /*检查I/0口状态:A7A6A5A4
         0000:LWC=0;1010:LWC=0x19;0101:LWC=0x32;1111:LWC=0x64*/
	   case 0x00://0    
               LWC = 0x00;
			   break;
	   case 0x10://0.5
	           LWC = 0x19;
			   break;
	   case 0x20://0.25
	           LWC= 0x32;
			   break;
	   case 0x30://1
	           LWC= 0x64;
			   break;
	   default:
	           break;
	 }
     if(count>=48)
	 {
	    count = 0;
		E422_Send(0xaa);
		E422_Send(0x55);
		E422_Send(0x00);
		E422_Send(0x09);
		E422_Send(0x02);
		E422_Send(LWC);
		E422_Send(0x00);
		E422_Send(0x00);
		E422_Send(0xff-(0xaa+0x55+0x09+0x02+LWC)+0x1);
	 }
   }
}
