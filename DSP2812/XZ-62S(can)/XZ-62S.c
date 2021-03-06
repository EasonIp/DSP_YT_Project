

#include "DSP28_Device.h"
#include "math.h"


#define RAMBASE1 0x080000      //总线地址,对应于片选信号XZCS2 

#define ID1 0x01200000       //左控制器发送到右控制器MSGID
#define ID2 0x01300000       //左控制器发送到试验器MSGID
#define ID3 0x02100000       //右控制器发送到左控制器MSGID
#define ID4 0x03100000       //试验器发送到左控制器MSGID


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
Uint32 i=0;                    //延时变量
Uint32 j[6]={0,0,0,0,0,0};     
Uint16 Write_tmp=0;			    //定义外部接口写操作变量
Uint16 Read_tmp=0;              //定义外部接口读操作变量
Uint16 *rambase1;              //定义Zone6外部接口指针变量
Uint16 LowWord_271_Send;        //发送LABLE271低字节
Uint16 HighWord_271_Send;       //发送LABLE271高字节
Uint16 LowWord_272_Send;        //发送LABLE272低字节
Uint16 HighWord_272_Send;        //发送LABLE272高字节
Uint16 LowWord_273_Send;         //发送LABLE273低字节
Uint16 HighWord_273_Send;        //发送LABLE273高字节
Uint16 LowWord_300_Send;          //发送软件版本号300低字节
Uint16 HighWord_300_Send;         //发送软件版本号300高字节
Uint16 LowWord_301_Send;          //发送软件版本号301低字节
Uint16 HighWord_301_Send;         //发送软件版本号301高字节
Uint16 LowWord_302_Send;         //发送软件版本号302低字节
Uint16 HighWord_302_Send;         //发送软件版本号302高字节
Uint16 LowWord_303_Send;          //发送软件版本号303低字节
Uint16 HighWord_303_Send;         //发送软件版本号303高字节
Uint16 LowWord_304_Send;           //发送硬件版本号304低字节
Uint16 HighWord_304_Send;          //发送硬件版本号304高字节
Uint16 LowWord_305_Send;           //发送硬件版本号305低字节
Uint16 HighWord_305_Send;          //发送硬件版本号305高字节
Uint16 LowWord_306_Send;           //发送硬件版本号306低字节
Uint16 HighWord_306_Send;          //发送硬件版本号306高字节
Uint16 LowWord_307_Send;           //发送硬件版本号307低字节
Uint16 HighWord_307_Send;          //发送硬件版本号307高字节
Uint16 LowWord_310_Send;           //发送硬件版本号310低字节
Uint16 HighWord_310_Send;          //发送硬件版本号310高字节
Uint32 TestMbox1=0x0;			   //邮箱2接收到的信息
Uint32 TestMbox2=0x0;     
Uint32 TestMbox3=0x0;			   //邮箱3接收到的信息
Uint32 TestMbox4=0x0;  
    
char  Flag_Controller_Judge=0;
/////////////////函数申明/////////////////////////////////

void InitSysCtrl(void);
void InitGpio(void);
void InitXintf(void);
void InitECan(void);
void VersionNumber_Hardware_Get(void);
void Data1_Save(void);
void Data1_Read(void);
interrupt void ECAN0_ISR(void);


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
   for(i=0; i<5000; i++){}   // 为了频率稳定匦胙踊
      
   SysCtrlRegs.HISPCP.all = 0x0002;  //高速外设时钟=120/4=30MHz
   SysCtrlRegs.LOSPCP.all = 0x0002;	 //低速外设时钟=120/4=30MHz
 
   SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;  //使能EVA模块的高速外设时钟
   SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;  //使能EVB模块的高速外设时钟
   SysCtrlRegs.PCLKCR.bit.ADCENCLK=1;  //使能ADC模块的高速外设时钟
   SysCtrlRegs.PCLKCR.bit.ECANENCLK=1; //使能CAN外设的系统时钟
				
   EDIS;	
}

///////////////////GPIO口初始化//////////////////////////////////
void InitGpio(void)
{
   EALLOW;
   GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOA设置为I/O口 
   GpioMuxRegs.GPADIR.all=0xffff;   //A0-A15设置为输出	
   GpioDataRegs.GPADAT.all=0x23e; 

   GpioMuxRegs.GPBMUX.all=0x0000;    //GPIOB设置为I/O口 
   GpioMuxRegs.GPBDIR.all=0xefdf;    //B5,B12设置为输入，其余设置为输出	
   GpioDataRegs.GPBDAT.all=0x8221;   

    
   GpioMuxRegs.GPFMUX.all=0x00c0;  //设置GPIOF6/7为eCAN口,其余设置为I/O口
   GpioMuxRegs.GPFDIR.all=0xa00;
   GpioDataRegs.GPFDAT.bit.GPIOF9=1;
   GpioDataRegs.GPFDAT.bit.GPIOF11=0;
   EDIS;
}	

	
////////////////////外部接口XINTF初始化/////////////////////////
void InitXintf(void)
{  
    XintfRegs.XINTCNF2.bit.XTIMCLK = 0x0001;  //所有zone的时序是基于XTIMCLK = SYSCLKOUT/2 
    XintfRegs.XINTCNF2.bit.WRBUFF = 3;   //设置3个写缓冲
	XintfRegs.XINTCNF2.bit.CLKOFF = 0;  //使能XCLKOUT
	XintfRegs.XINTCNF2.bit.CLKMODE = 0;  //设置XCLKOUT=XTIMCLK
    	
	XintfRegs.XTIMING2.bit.X2TIMING=1;
	//Zone2写时序
	XintfRegs.XTIMING2.bit.XWRLEAD = 1;    //写操作引导阶段的周期=2*XTIMCLK
	XintfRegs.XTIMING2.bit.XWRACTIVE = 7;   //写操作激活阶段等待状态的周期=14*XTIMCLK
	XintfRegs.XTIMING2.bit.XWRTRAIL = 2;    //写操作跟踪阶段的周期=4*XTIMCLK
   //Zone2读时序
	XintfRegs.XTIMING2.bit.XRDLEAD = 1;    //读操作引导阶段的周期=2*XTIMCLK
	XintfRegs.XTIMING2.bit.XRDACTIVE=7;	    //读操作激活阶段等待状态的周期=14*XTIMCLK
	XintfRegs.XTIMING2.bit.XRDTRAIL = 2;   //读操作跟踪阶段的周期=4*XTIMCLK
	//不加倍时序
	XintfRegs.XTIMING2.bit.X2TIMING = 0; 
   //Zone2不对READY信号采样
    XintfRegs.XTIMING2.bit.USEREADY = 0;
    XintfRegs.XTIMING2.bit.READYMODE = 0;
    XintfRegs.XTIMING2.bit.XSIZE = 3;   //16位接口，唯一有效的设置

    XintfRegs.XBANK.bit.BANK = 2;   //使能内部区域2
    XintfRegs.XBANK.bit.BCYC = 3;  //连续访问之间添加了3个XTIMCLK周期

	//Zone6写时序
	XintfRegs.XTIMING6.bit.XWRLEAD = 1;    //写操作引导阶段的周期=XTIMCLK
	XintfRegs.XTIMING6.bit.XWRACTIVE = 7;   //写操作激活阶段等待状态的周期=XTIMCLK
	XintfRegs.XTIMING6.bit.XWRTRAIL = 3;    //写操作跟踪阶段的周期=XTIMCLK
   //Zone6读时序
	XintfRegs.XTIMING6.bit.XRDLEAD = 1;    //读操作引导阶段的周期=XTIMCLK
	XintfRegs.XTIMING6.bit.XRDACTIVE=1;	    //读操作激活阶段等待状态的周期=XTIMCLK
	XintfRegs.XTIMING6.bit.XRDTRAIL = 1;   //读操作跟踪阶段的周期=XTIMCLK
	//不加倍时序
	XintfRegs.XTIMING6.bit.X2TIMING = 0; 
   //Zone6不对READY信号采样
    XintfRegs.XTIMING6.bit.USEREADY = 0;
    XintfRegs.XTIMING6.bit.READYMODE = 0;
    XintfRegs.XTIMING6.bit.XSIZE = 3;   //16位接口，唯一有效的设置

    XintfRegs.XBANK.bit.BANK = 6;   //使能内部区域6
    XintfRegs.XBANK.bit.BCYC = 3;  //连续访问之间添加了3个XTIMCLK周期
}


////////////////////ECAN初始化/////////////////////////
void InitECan(void)
{
	EALLOW;
	ECanaRegs.CANTIOC.bit.TXFUNC = 1;
	ECanaRegs.CANRIOC.bit.RXFUNC = 1;
	EDIS;
		
	ECanaRegs.CANME.all = 0;  //禁止杏氏�

      ECanaRegs.CANMD.bit.MD0=0;  //邮箱0发送到右控制器
      ECanaRegs.CANMD.bit.MD1=0;  //邮箱1发送到试验器
      ECanaRegs.CANMD.bit.MD2=1;  //邮箱2接收右控制器信息
      ECanaRegs.CANMD.bit.MD3=1;  //邮箱3接收试验器信息
     	
	if(Flag_Controller_Judge==0)
	{ ECanaRegs.CANMD.bit.MD0=0;  //邮箱0发送到右控制器
      ECanaRegs.CANMD.bit.MD1=0;  //邮箱1发送到试验器
      ECanaRegs.CANMD.bit.MD2=1;  //邮箱2接收右控制器信息
      ECanaRegs.CANMD.bit.MD3=1;  //邮箱3接收试验器信息
     	
	  ECanaMboxes.MBOX0.MID.all = ID1;	
	  ECanaMboxes.MBOX1.MID.all = ID2;
	  ECanaMboxes.MBOX2.MID.all = ID3;
	  ECanaMboxes.MBOX3.MID.all = ID4;
	}
    else
	{ ECanaRegs.CANMD.bit.MD0=0;  //邮箱0发送到左控制器
      ECanaRegs.CANMD.bit.MD2=1;  //邮箱2接收左控制器信息
    	
	  ECanaMboxes.MBOX0.MID.all = ID3;	
	  ECanaMboxes.MBOX2.MID.all = ID1;
	}
			
	ECanaRegs.CANME.bit.ME0=1;	//邮箱使能
    ECanaRegs.CANME.bit.ME1=1;
    ECanaRegs.CANME.bit.ME2=1;
	ECanaRegs.CANME.bit.ME3=1;
		
	ECanaMboxes.MBOX0.MCF.bit.DLC = 8;	//数据长度 8个BYTE
	ECanaMboxes.MBOX1.MCF.bit.DLC = 8;
	ECanaMboxes.MBOX2.MCF.bit.DLC = 8;
	ECanaMboxes.MBOX3.MCF.bit.DLC = 8;
	
	ECanaMboxes.MBOX0.MCF.bit.RTR = 0;	
	ECanaMboxes.MBOX1.MCF.bit.RTR = 0;  
	ECanaMboxes.MBOX2.MCF.bit.RTR = 0;	
	ECanaMboxes.MBOX3.MCF.bit.RTR = 0; 

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
    ECanaRegs.CANMC.bit.DBO = 0; //首先接收或发送数据的最高字节
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


/////////////通过总线向外部存储器写硬件版本号////////////////////
void Data1_Save(void)
{  
   *rambase1=Write_tmp&0xff; 	//将变量Read_tmp写入数据总线上
   for(i=0;i<0xffff;i++){}

   j[0]++;
   if(j[0]<7)
   { rambase1++;  
   }
   else
   { rambase1=(Uint16 *)RAMBASE1;
     j[0]=0;
   } 
}


/////////////通过总线读取上次故障次数////////////////////
void Data1_Read(void)
{  
   Read_tmp=(*rambase1)&0xff;       //将数据总线上的数据读到变量Read_tmp中
   for(i=0;i<0xffff;i++){}
   j[3]++;
   if(j[3]<7)
   { rambase1++;  
   }
   else
   { rambase1=(Uint16 *)RAMBASE1;
     j[3]=0;
   }
}

///////////////读取硬件版本号///////////////////////////////
void VersionNumber_Hardware_Get(void)
{  
   Uint16 Version_Number1=0;         //预存硬件版本号第1位
   Uint16 Version_Number2=0;         //预存硬件版本号第2位
   Uint16 Version_Number3=0;         //预存硬件版本号第3位
   Uint16 Version_Number4=0;         //预存硬件版本号第4位
   Uint16 Version_Number5=0;         //预存硬件版本号第5位
   Uint16 Version_Number6=0;         //预存硬件版本号第6位
   Uint16 Version_Number7=0;         //预存硬件版本号第7位
   
   DINT;
   GpioDataRegs.GPBDAT.bit.GPIOB8=1;
   GpioDataRegs.GPBDAT.bit.GPIOB9=0;   //打开读写操作B9=0
   for(i=0;i<0x5ffff;i++) {}
   rambase1=(Uint16 *)RAMBASE1;    
   Data1_Read();
   Version_Number1=Read_tmp;
   Data1_Read();
   Version_Number2=Read_tmp;   
   Data1_Read();
   Version_Number3=Read_tmp;
   Data1_Read();
   Version_Number4=Read_tmp;
   Data1_Read();
   Version_Number5=Read_tmp;
   Data1_Read();
   Version_Number6=Read_tmp;
   Data1_Read();
   Version_Number7=Read_tmp;
   GpioDataRegs.GPBDAT.bit.GPIOB9=1;   //关闭读写操作B9=1
   GpioDataRegs.GPBDAT.bit.GPIOB8=0;
   EINT;

   LowWord_306_Send=((Version_Number1&0x1f)<<11)+0xc6;
   HighWord_306_Send=((Version_Number3&0x7f)<<9)+((Version_Number2&0x7f)<<2)+((Version_Number1&0x60)>>5);

   LowWord_307_Send=((Version_Number4&0x1f)<<11)+0xc7;
   HighWord_307_Send=((Version_Number6&0x7f)<<9)+((Version_Number5&0x7f)<<2)+((Version_Number4&0x60)>>5);
 
   LowWord_310_Send=((Version_Number7&0x1f)<<11)+0xc8;
   HighWord_310_Send=(HighWord_310_Send&0xfffc)+((Version_Number7&0x60)>>5);
 
   rambase1=(Uint16 *)RAMBASE1;
}



///////////////////CAN中断邮招畔?//////////////////////////////
interrupt void ECAN0_ISR(void)
{   
  DINT;
  if(PieCtrl.PIEACK.bit.ACK9==1)
  { PieCtrl.PIEACK.bit.ACK9=1;

   //////////////////////////////////////////////////////////////
    if(ECanaRegs.CANRMP.bit.RMP3==1)  //接收试验器信息
    { ECanaRegs.CANRMP.bit.RMP3=1;
     
       TestMbox3=ECanaMboxes.MBOX3.MDRL.all;
       TestMbox4=ECanaMboxes.MBOX3.MDRH.all;
        
         //////接收到硬件版本号//////////////
        if((TestMbox4&0xff000000)==0x58000000)
		{ //////////硬件版本号保存///////////////////
		  GpioDataRegs.GPBDAT.bit.GPIOB8=1;
		  GpioDataRegs.GPBDAT.bit.GPIOB9=0;   //打开读写操作B9=0		  
          for(i=0;i<0xffff;i++) {}
          rambase1=(Uint16 *)RAMBASE1; 
		  
          Write_tmp=(Uint16)((TestMbox4&0xff0000)>>16); 
          Data1_Save();
          Write_tmp=(Uint16)((TestMbox4&0xff00)>>8);
          Data1_Save();
          Write_tmp=(Uint16)(TestMbox4&0xff);
          Data1_Save();
          Write_tmp=(Uint16)((TestMbox3&0xff000000)>>24);
          Data1_Save();
          Write_tmp=(Uint16)((TestMbox3&0xff0000)>>16); 
          Data1_Save();
          Write_tmp=(Uint16)((TestMbox3&0xff00)>>8); 
          Data1_Save(); 
          Write_tmp=(Uint16)(TestMbox3&0xff); 
          Data1_Save();	
          	  
		  //GpioDataRegs.GPBDAT.bit.GPIOB9=1;   //打开读写操作B9=0 
		  //GpioDataRegs.GPBDAT.bit.GPIOB8=0;           
		}	    
    }	
   }
   EINT;
}



/////////////主程序//////////////////////////////////////

main()
{  InitSysCtrl();
	//把InitFlash（）函数搬移到ram中运行
   MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);	
   InitFlash();

   DINT;
   IER = 0x0000;
   IFR = 0x0000;

   InitPieCtrl();
   InitPieVectTable();
   InitGpio();	
   InitXintf();
   //左/右控制判断//////////////////////
   if(GpioDataRegs.GPFDAT.bit.GPIOF13==1)  
   { Flag_Controller_Judge=1;         //右控制器
   }
   else
   { Flag_Controller_Judge=0;        //左控制器	 
   } 


   InitECan(); 
    		
   EALLOW;
   PieVectTable.ECAN0INTA = &ECAN0_ISR;
   EDIS;  
  
   //开中断
   IER |= M_INT9;
   PieCtrl.PIEIER9.bit.INTx5=1;
   EINT;  
   ERTM;

   LowWord_271_Send=0x00b9;   //初始化429信号
   HighWord_271_Send=0x0000;  
   LowWord_272_Send=0x40ba;   
   HighWord_272_Send=0x0000;
   LowWord_273_Send=0x00bb;  
   HighWord_273_Send=0x0001;
   LowWord_300_Send=0x98c0;  
   HighWord_300_Send=0x62c1;
   LowWord_301_Send=0x90c1;  
   HighWord_301_Send=0x62c1;
   LowWord_302_Send=0x90c2;    
   HighWord_302_Send=0x64c1;
   LowWord_303_Send=0x88c3;  
   HighWord_303_Send=0x1;  
   LowWord_304_Send=0x98c4;  
   HighWord_304_Send=0x62c1;
   LowWord_305_Send=0x90c5;  
   HighWord_305_Send=0x62c1;
   LowWord_306_Send=0x88c6;  
   HighWord_306_Send=0x62c5;
   LowWord_307_Send=0x90c7;  
   HighWord_307_Send=0x60c1; 
   LowWord_310_Send=0xc8c8;  
   HighWord_310_Send=0xc5; 
     
    //////读取预置硬件版本号///////////////
   VersionNumber_Hardware_Get();
         
   for(;;){}

}