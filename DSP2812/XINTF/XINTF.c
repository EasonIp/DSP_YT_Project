
#include "DSP28_Device.h"

#define RAMBASE1 0x0080000
//#define RAMBASE2 0x0080006      //保存故障次数首地址
//#define RAMBASE3 0x008000a      //保存状态信息版本号首地址
//#define RAMBASE6 0x0100000

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


/////////////////////////////////////////////////////
Uint16 *rambase1=0x0;
Uint16 *rambase2=0x0;
Uint16 *rambase3=0x0;
Uint16 *rambase6=0x0;
Uint16 Write_tmp=0x0;
Uint16 Read_tmp=0x0;  
Uint16 count=0; 
Uint32 Count_System_Error=0x5678;          
Uint32 i=0;
Uint16 j2=0;
Uint32 j3=0;
Uint16 LowWord_271_Send=0xb9;
Uint16 HighWord_271_Send=0x11;
Uint16 LowWord_272_Send=0xba;
Uint16 HighWord_272_Send=0x22; 
Uint16 LowWord_273_Send=0xbb;
Uint16 HighWord_273_Send=0x33;
Uint16 LowWord_Receive_Send=0x8b;
Uint16 HighWord_Receive_Send=0x44;

////////////////////////////////////////////////////
void InitSysCtrl(void);
void InitXintf(void);
void InitGpio(void);
void Data2_Save(void);
void Data3_Save(void);
void Data6_Save(void);
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
   SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;  //使能EVB模块的高速外设时钟
   SysCtrlRegs.PCLKCR.bit.ADCENCLK=1;  //使能ADC模块的高速外设时钟
   SysCtrlRegs.PCLKCR.bit.ECANENCLK=1; //使能CAN外设的系统时钟
				
   EDIS;
	
}


////////////////////外部接口初始化/////////////////////////
void InitXintf(void)
{  
    XintfRegs.XINTCNF2.bit.XTIMCLK = 0x0001;  //所有zone的时序是基于XTIMCLK = SYSCLKOUT/2 
    XintfRegs.XINTCNF2.bit.WRBUFF = 3;   //设置3个写缓冲
	XintfRegs.XINTCNF2.bit.CLKOFF = 0;  //使能XCLKOUT
	XintfRegs.XINTCNF2.bit.CLKMODE = 0;  //设置XCLKOUT=XTIMCLK
    	
	XintfRegs.XTIMING2.bit.X2TIMING=1;
	//Zone2写时序
	XintfRegs.XTIMING2.bit.XWRLEAD = 3;    //写操作引导阶段的周期=6*XTIMCLK
	XintfRegs.XTIMING2.bit.XWRACTIVE = 7;   //写操作激活阶段等待状态的周期=14*XTIMCLK
	XintfRegs.XTIMING2.bit.XWRTRAIL = 3;    //写操作跟踪阶段的周期=6*XTIMCLK
   //Zone2读时序
	XintfRegs.XTIMING2.bit.XRDLEAD = 3;    //读操作引导阶段的周期=6*XTIMCLK
	XintfRegs.XTIMING2.bit.XRDACTIVE=7;	    //读操作激活阶段等待状态的周期=14*XTIMCLK
	XintfRegs.XTIMING2.bit.XRDTRAIL = 3;   //读操作跟踪阶段的周期=6*XTIMCLK
	//不加倍时序
	XintfRegs.XTIMING2.bit.X2TIMING = 0; 
   //Zone2不对READY信号采样
    XintfRegs.XTIMING2.bit.USEREADY = 0;
    XintfRegs.XTIMING2.bit.READYMODE = 0;
    XintfRegs.XTIMING2.bit.XSIZE = 3;   //16位接口，唯一有效的设置

    XintfRegs.XBANK.bit.BANK = 2;   //使能内部区域2
    XintfRegs.XBANK.bit.BCYC = 3;  //连续访问之间添加了3个XTIMCLK周期

   //Zone6写时序
  /* XintfRegs.XTIMING6.bit.XWRLEAD = 3;    //写操作引导阶段的周期=6*XTIMCLK
   XintfRegs.XTIMING6.bit.XWRACTIVE = 7;   //写操作激活阶段等待状态的周期=14*XTIMCLK
   XintfRegs.XTIMING6.bit.XWRTRAIL = 3;    //写操作跟踪阶段的周期=6*XTIMCLK
   //Zone6读时序
   XintfRegs.XTIMING6.bit.XRDLEAD = 3;    //读操作引导阶段的周期=6*XTIMCLK
   XintfRegs.XTIMING6.bit.XRDACTIVE=7;	    //读操作激活阶段等待状态的周期=14*XTIMCLK
   XintfRegs.XTIMING6.bit.XRDTRAIL = 3;   //读操作跟踪阶段的周期=6*XTIMCLK
   //不加倍时序
   XintfRegs.XTIMING6.bit.X2TIMING = 0; 
   //Zone6不对READY信号采样
   XintfRegs.XTIMING6.bit.USEREADY = 0;
   XintfRegs.XTIMING6.bit.READYMODE = 0;
   XintfRegs.XTIMING6.bit.XSIZE = 3;   //16位接口，唯一有效的设置

   XintfRegs.XBANK.bit.BANK = 6;   //使能内部区域6
   XintfRegs.XBANK.bit.BCYC = 3;  //连续访问之间添加了3个XTIMCLK周期*/


}

///////////////////GPIO口初始化//////////////////////////////////
void InitGpio(void)
{
   EALLOW;
   GpioMuxRegs.GPBMUX.all=0x0000;    //GPIOB设置为I/O口 
   GpioMuxRegs.GPBDIR.all=0xefdf;    //B5,B12设置为输入，其余设置为输出	
   GpioDataRegs.GPBDAT.bit.GPIOB8=0;   
   GpioDataRegs.GPBDAT.bit.GPIOB9=1; 
   GpioDataRegs.GPBDAT.bit.GPIOB15=1;     
   EDIS;
}	

///////////////////定时器1初始化（100ms）//////////////////////////////////

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

///////////////////////////////////////////
/*void Data2_Save(void)
{ 
   *rambase2=Write_tmp&0xff; 
   for(i=0;i<0x62ff;i++){}
   j2++;
   if(j2<4)
   { rambase2++;
   }
   else
   { j2=0;
     rambase2=(Uint16 *)RAMBASE2;
   }
}

///////////////////////////////////////////
void Data3_Save(void)
{      
   *rambase3=Write_tmp&0xff; 
   for(i=0;i<0x62ff;i++){}
   j3++;
   if(j3<0x7fff5)
   {rambase3++;
   }
   else
   { j3=0;
     rambase3=(Uint16 *)RAMBASE3;
   } 
}

void Data6_Save(void)
{      
   *rambase6=Write_tmp&0xff; 
   for(i=0;i<0x62ff;i++){}
   rambase6++;

}*/


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




///////////////////////////////////////////////////
main()
{  InitSysCtrl();
   DINT;
   IER = 0x0000;
   IFR = 0x0000;

   InitPieCtrl();
   InitPieVectTable();
   InitXintf();
   InitGpio();

   EALLOW;
   PieVectTable.T1PINT=&eva_timer1_isr;
   EDIS;  
  
   //开中断
   IER |= M_INT2;
   PieCtrl.PIEIER2.bit.INTx4=1;
   EINT;  
   ERTM; 


   //rambase2=(Uint16 *)RAMBASE2;
   //rambase3=(Uint16 *)RAMBASE3;
   rambase1=(Uint16 *)RAMBASE1;

   Write_tmp=0x1;
   EvaRegs.T1CON.bit.TENABLE=1;  // 打开定时器1
     
   for(;;)
   { 
//     if(count>=20)
//     {
//       count=0;
       *rambase1=Write_tmp&0xff; 
       for(i=0;i<0x62ff;i++){}
       rambase1++;



       /*GpioDataRegs.GPBDAT.bit.GPIOB8=1;
       GpioDataRegs.GPBDAT.bit.GPIOB9=0;   //打开读写操作B9=0
       for(i=0;i<0x2ffff;i++);
       Write_tmp=(Uint16)(Count_System_Error&0xff);   //保存总故障次数
       Data2_Save();		   
       Write_tmp=(Uint16)((Count_System_Error&0xff00)>>8);
       Data2_Save();
       Write_tmp=(Uint16)((Count_System_Error&0xff0000)>>16);
       Data2_Save();
       Write_tmp=(Uint16)((Count_System_Error&0xff000000)>>24);
       Data2_Save();


       Write_tmp=LowWord_271_Send&0xff;    //保存429信号
       Data3_Save();
       Write_tmp=LowWord_271_Send>>8;
       Data3_Save();
       Write_tmp=HighWord_271_Send&0xff;    
       Data3_Save();
       Write_tmp=HighWord_271_Send>>8;
       Data3_Save();

       Write_tmp=LowWord_272_Send&0xff;    
       Data3_Save();
       Write_tmp=LowWord_272_Send>>8;
       Data3_Save();
       Write_tmp=HighWord_272_Send&0xff;    
       Data3_Save();
       Write_tmp=HighWord_272_Send>>8;
       Data3_Save();

       Write_tmp=LowWord_273_Send&0xff;    
       Data3_Save();
       Write_tmp=LowWord_273_Send>>8;
       Data3_Save();
       Write_tmp=HighWord_273_Send&0xff;    
       Data3_Save();
       Write_tmp=HighWord_273_Send>>8;
       Data3_Save();

       Write_tmp=LowWord_Receive_Send&0xff;    
       Data3_Save();
       Write_tmp=LowWord_Receive_Send>>8;
       Data3_Save();
       Write_tmp=HighWord_Receive_Send&0xff;    
       Data3_Save();
       Write_tmp=HighWord_Receive_Send>>8;
       Data3_Save();


       GpioDataRegs.GPBDAT.bit.GPIOB9=1;  //关闭读写操鰾9=1
       GpioDataRegs.GPBDAT.bit.GPIOB8=0;
       EINT; */                
//     }
   }
}
