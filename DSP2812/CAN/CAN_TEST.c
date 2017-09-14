
#include "DSP28_Device.h"
#include "math.h"
#include "stdio.h"


//Global variable for this example

Uint32 MessageCount=0;
Uint16 i=0,j=0;
Uint32 TestMbox1=0;
Uint32 TestMbox2=0;
Uint16 can_send=0;
Uint16 can_receive=0;
Uint16 count=0;

Uint16 a=0;


void InitSysCtrl(void);
void InitECan(void);
void InitGpio(void);
void Init_eva_timer1(void);
interrupt void ECAN0_ISR(void);
interrupt void eva_timer1_isr(void);
char Flag_Controller_Judge=0;
///////////////////////////////////////////////////
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

////////////////////////////////////////////////
void InitSysCtrl(void)
{
   
   EALLOW;
   DevEmuRegs.M0RAMDFT = 0x0300;
   DevEmuRegs.M1RAMDFT = 0x0300;
   DevEmuRegs.L0RAMDFT = 0x0300;
   DevEmuRegs.L1RAMDFT = 0x0300;
   DevEmuRegs.H0RAMDFT = 0x0300;
   
   SysCtrlRegs.WDCR= 0x0068;

   SysCtrlRegs.PLLCR = 0x8;  //120MHz

   for(i= 0; i< 5000; i++){}
       
   SysCtrlRegs.HISPCP.all = 0x0002;
   SysCtrlRegs.LOSPCP.all = 0x0002;	
  
   SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;
   SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;
   SysCtrlRegs.PCLKCR.bit.ECANENCLK = 1;				
   EDIS;	
}

////////////////////ECAN初始化/////////////////////////
void InitECan(void)
{
	EALLOW;
	ECanaRegs.CANTIOC.bit.TXFUNC = 1;
	ECanaRegs.CANRIOC.bit.RXFUNC = 1;
	EDIS;
		
	ECanaRegs.CANME.all = 0;  //禁止所有邮箱
	
	 ECanaRegs.CANMD.bit.MD0=0;  //邮箱0发送到右控制器
     ECanaRegs.CANMD.bit.MD1=1;  



	if(Flag_Controller_Judge==0)
	{     	
	  ECanaMboxes.MBOX0.MID.all = 0x01200000;	
	  ECanaMboxes.MBOX1.MID.all = 0x02100000;

	}
    else
	{ 	
	  ECanaMboxes.MBOX0.MID.all = 0x02100000;	
	  ECanaMboxes.MBOX1.MID.all = 0x01200000;
	}
			
	ECanaRegs.CANME.bit.ME0=1;	//邮箱使能
    ECanaRegs.CANME.bit.ME1=1;

		
	ECanaMboxes.MBOX0.MCF.bit.DLC = 8;	//数据长度 8个BYTE
	ECanaMboxes.MBOX1.MCF.bit.DLC = 8;

	
	ECanaMboxes.MBOX0.MCF.bit.RTR = 0;	
	ECanaMboxes.MBOX1.MCF.bit.RTR = 0;  


    ECanaMboxes.MBOX0.MDRL.all=0xb9;
    ECanaMboxes.MBOX0.MDRH.all=0x444d3143;

	EALLOW;	
	ECanaRegs.CANMIM.all = 0xFFFFFFFF;	//邮箱屏蔽使能
	ECanaRegs.CANMC.bit.CCR = 1;
	EDIS;
	
	while(ECanaRegs.CANES.bit.CCE != 1){}
	
	EALLOW;	
	ECanaRegs.CANBTC.bit.BRP = 79;	 //125Kb/s
	ECanaRegs.CANBTC.bit.TSEG2 = 3;
	ECanaRegs.CANBTC.bit.TSEG1 = 6;
	ECanaRegs.CANMC.bit.CCR = 0;
	EDIS;
	
    while(ECanaRegs.CANES.bit.CCE != 0){}
	
	EALLOW;
	ECanaRegs.CANMC.bit.STM = 0; //正常工作模式 
	ECanaRegs.CANMC.bit.SCM = 0; //scc模式
    ECanaRegs.CANMC.bit.DBO = 0; //首先接收或发送数据的最哂行ё纸?
	EDIS;
	
	EALLOW;
	ECanaRegs.CANMIM.all = 0xFFFFFFFF;
	ECanaRegs.CANMIL.all = 0;
	ECanaRegs.CANGIF0.all = 0xFFFFFFFF;
	ECanaRegs.CANGIM.bit.I0EN = 1;
	ECanaRegs.CANGIM.bit.I1EN = 1;
	ECanaRegs.CANGIM.all = 0x3ff07;
	EDIS;
	PieCtrl.PIEIER9.bit.INTx6 = 1;
	PieCtrl.PIEIER9.bit.INTx5 = 1;
}

///////////////////IO口初始化//////////////////////////////////
void InitGpio(void)
{
   EALLOW;
   GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOA设置为I/O口 
   GpioMuxRegs.GPADIR.all=0xFFFF;   //A0-A15设置为输出	
   GpioDataRegs.GPADAT.all=0x23e; 

   GpioMuxRegs.GPBMUX.all=0x0000;    //GPIOB设置为I/O口 
   GpioMuxRegs.GPBDIR.all=0xFFDF;    //B5设置为输入，其余设置为输出	
   GpioDataRegs.GPBDAT.all=0x8001;   
     
   GpioMuxRegs.GPFMUX.all=0x00c0;  //设置GPIOF6/7为eCAN口,其余设置为I/O口
   GpioMuxRegs.GPFDIR.all=0xa00;
   GpioDataRegs.GPFDAT.bit.GPIOF9=1;
   GpioDataRegs.GPFDAT.bit.GPIOF11=0;
   EDIS;
} 

///////////////////定时器1初始化（100mS)/////////////////////////////////
void Init_eva_timer1(void)
{  
   EvaRegs.GPTCONA.all = 0;   //初始化定时器1并设置定时器1寄存器
   EvaRegs.EVAIMRA.bit.T1PINT = 1;  //允许定时器1周期中断
   EvaRegs.EVAIFRA.bit.T1PINT = 1;  //清除定时器1周期中断标志
             
   EvaRegs.T1CON.all = 0x170C;  //Timer1为连续增计数模式，预分频值128
	                             //使用内部高速外设时钟
   EvaRegs.T1PR = 0x5b8d;   //Timer1的期寄存器=0.1*30MHz/128
   EvaRegs.T1CNT = 0x0000;  //Timer1的计数清零
}

/////////////////////////////////////////////
void main(void)
{
	/*初始化系统*/
	InitSysCtrl();

	/*关中断*/
	DINT;
	IER = 0x0000;
	IFR = 0x0000;

	/*初始化PIE中断*/
	InitPieCtrl();
	InitPieVectTable();
	Init_eva_timer1();
	InitGpio(); 
   //左/右控制判断//////////////////////
  if(GpioDataRegs.GPFDAT.bit.GPIOF13==1)  
   { Flag_Controller_Judge=1;         //右控制器
   }
   else
   { Flag_Controller_Judge=0;        //左控制器	 
   }

    InitECan();	
        
	/*设置中断服务程序入口地址*/
	EALLOW;	
	PieVectTable.T1PINT=&eva_timer1_isr;
	PieVectTable.ECAN0INTA = &ECAN0_ISR;
	EDIS;  

   IER |= (M_INT2|M_INT9);
   PieCtrl.PIEIER2.bit.INTx4=1;
   PieCtrl.PIEIER9.bit.INTx5=1;
   EINT;  
   ERTM;
   EvaRegs.T1CON.bit.TENABLE=1;   //打开定时器

   a=pow(2,15);


	for(;;)
	{
		if(count>=20)
		{ count=0;
		  ECanaRegs.CANTRS.bit.TRS0=1;   //启动发送
		  for(i=0;i<0xffff;i++){}
		  ECanaRegs.CANTA.bit.TA0=1;
		  for(i=0;i<0xffff;i++){}
          can_send++;
		}	
	}	
} 	

////////////////////////////////////
interrupt void ECAN0_ISR(void)
{ 
    DINT;
	PieCtrl.PIEACK.all=0xffff;
    
    if(ECanaRegs.CANRMP.bit.RMP1==1)
    { ECanaRegs.CANRMP.bit.RMP1=1;	  
      TestMbox1=ECanaMboxes.MBOX1.MDRL.all;
      TestMbox2=ECanaMboxes.MBOX1.MDRH.all;
      if(TestMbox2==0x444d3143)
	  { can_receive++;
	  }
    } 
    EINT;     
}

///////////////////定时器1中断//////////////////////////////////

interrupt void eva_timer1_isr(void)
{  
  DINT;
  if(PieCtrl.PIEACK.bit.ACK2==1)
  { PieCtrl.PIEACK.bit.ACK2=1;
 	count++;   
    EvaRegs.EVAIMRA.bit.T1PINT = 1;
    EvaRegs.EVAIFRA.bit.T1PINT = 1;
  }
  EINT;

} 