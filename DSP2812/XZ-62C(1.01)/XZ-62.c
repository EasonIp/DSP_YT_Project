 /* 
   * 尾翼防除冰控制器
   * 文件名：XZ-62.pit
   * 时间：2011.1.1.10.27
   * 版本：第1.01版
   
*/

#include "DSP28_Device.h"
#include "math.h"

#define RAMBASE1 0x0080000      //保存版本号首地址
#define RAMBASE2 0x0080005      //保存故障次数首地址
#define RAMBASE3 0x0080009      //保存状态信息码首地址
#define RAMBASE6 0x0100000      //总线地址,对应于片选信号XZCS6 
#define ID1 0x01200000       //左控制器发送到右控制器MSGID
#define ID2 0x01300000       //左控制器发送到试验器MSGID
#define ID3 0x02100000       //右控制器发送到左控制器MSGID
#define ID4 0x03100000       //试验器发送到左控制器MSGID
#define HIGH_COUNT 3426    //2.51*4095/3.0
#define LOW_COUNT 1733     //1.27*4095/3.0

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


////////////////变量申明/////////////////////////////////
Uint32 i;                  //变量
Uint16 channel;           //定义一个采集通道变量
int a[16];                //定义一个数组用于保存AD转换的结果
int n=0;
float jrwd;                //采集温度传感器数据
float qxdl;                //采集断相电流
Uint16 a1,a2;              //采集校正电压
Uint16 a3;     
float gain,offest;         //定义增益和偏置系数
Uint16 *rambase1,*rambase2,*rambase3; //定义外部接口指针
Uint16 tmp,tmp1;             //定义外部接口变量
Uint16 *rambase6;      
Uint16 CSH;               //429初始化
Uint16 FSDZJ_271;         //发送LABLE271低字节
Uint16 FSGZJ_271;         //发送LABLE271高字节
Uint16 FSDZJ_272;
Uint16 FSGZJ_272; 
Uint16 FSDZJ_273;
Uint16 FSGZJ_273; 
Uint16 FSDZJ_300;          //发送软件版本号
Uint16 FSGZJ_300;
Uint16 FSDZJ_301;       
Uint16 FSGZJ_301;
Uint16 FSDZJ_302;
Uint16 FSGZJ_302;
Uint16 FSDZJ_303;
Uint16 FSGZJ_303;
Uint16 FSDZJ_304;           //发送硬件版本号
Uint16 FSGZJ_304;
Uint16 FSDZJ_305;       
Uint16 FSGZJ_305;
Uint16 FSDZJ_306;
Uint16 FSGZJ_306;
Uint16 FSDZJ_307;
Uint16 FSGZJ_307;
Uint16 banbenhao1;
Uint16 banbenhao2;
Uint16 banbenhao3;
Uint16 banbenhao4;
Uint16 banbenhao5;
Uint16 FSDZJ1;
Uint16 FSGZJ1;
Uint16 JSDZJ;			     //接收低字节
Uint16 JSGZJ;			     //接收高字节
Uint32 gz=0;                   //故障次数记录
char m=0;
char sd[10]={0,0,0,0,0,0,0,0,0,0};
char j=0;                      //除冰控制按扭按压次数
char count1=0;                //1s计数变量 
char count2=0;                //0.5s计数变量CPLD监控DSP时钟
Uint16 count3=0;              //加热监控时间变量
char count4=0;                //缺相监控时间变量
char count5=0;                //热刀温度监测时间变量
char count6=0;                //4区域温度监测时间变量
char count7=0;                 //3区域温度监测时间变量
char count8=0;                //2区域温度监测时间变量
char count9=0;                 //1区域温度监测时间变量
char count10=0;                //判断短路延时变量
int t1=250,t2=250;                   //加热时间
char k[32]={0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0
           };                //每路温度超温时间 
Uint16 lz=0;                 //接收到轮载信号的次数
int error1=0;                 //未收到轮载信号的次数或错误信息
float wd;                     //大气静温
Uint16 wd1=0;                 //接收到大气温度信号的次数
int error2=0;                 //未收到大气温度信号的次数或错误信息
Uint16 can1=0,can2=0;
char EvaTimer1InterruptCount=0,EvaTimer2InterruptCount=0;                
char flag1;                   //左/右控制器标志
char flag2=0;                 //ADC校正标志flag2=1完成
char flag3=0;                 //上电自检正常                
char flag4=0;                  //地面维护正常
char flag5=0;                 //缺相监控0.5延时标志flag5=1完成
char flag6=0;                 //飞机在空中标志flag6=1
char flag6a=0;
char flag7=0;                  //主控制器左机翼断路传感器正常标志
char flag8=0;                  //主控制器右机翼断路传感器正常标志
char flag9=0;                  //上电自检完成标志
char flag10=0;                  //地面自检完成标志
char flag11=0;                  //主控制器正常
char flag12=0;                  //除冰启动标志flag12=1
char flag13=0;                  //按纽主/辅切换标志
char flag14=0;                  //故障时切换标志
char flag15=0;                  //开始接受429标志flag15=1
char flag16=0;                  //274轮载接收标志flag16=1
char flag17=0;                    //274轮载接收判断标志
char flag18=0;                  //213静温接收标志flag18=1
char flag19=0;                    //213静温接收判断标志
char flag20=0;                  //大气静温<5
char flag21=0;                  //短路判断标志flag21=1短路
char flag22=0;
char flag23=0;
Uint32 TestMbox1,TestMbox2;   //邮箱2接收到的信息
Uint32 TestMbox3,TestMbox4;   //邮箱3接收到的信息


/////////////////函数申明/////////////////////////////////

void InitSysCtrl(void);
void InitGpio(void);
void init_eva_timer1(void);
void init_eva_timer2(void);
void InitXintf(void);
void InitAdc(void);
void InitXINT13(void);
void InitXINT1(void);
void InitXINT2(void);
void InitECan(void);
void InitPDPINTB(void);
void ADC_adjust(void);
void ADC_sample(void);
void poweron_BIT(void);
void DMWH_BIT(void);
void MBOX0Send(void);
void MBOX1Send(void);
void saveDA1(void);
void saveDA2(void);
void saveDA3(void);
void readDA1(void);
void readDA2(void);
void readDA3(void);
void QCdata(void);
void DMJC(void);
void arinc429fasong(void);
void redao_DLJC(void);
void abc1_DLJC(void);
void abc2_DLJC(void);
void abc3_DLJC(void);
void abc4_DLJC(void);
//void redao_temperature(void);
//void abc4_temperature(void);
//void abc3_temperature(void);
//void abc2_temperature(void);
//void abc1_temperature(void);
void ZDLJC(void);
void YDLJC(void);
void ZKLJC(void);
void YKLJC(void);
void QXJC(void);
void jiare(void);
interrupt void eva_timer1_isr(void);
interrupt void eva_timer2_isr(void);
interrupt void ad_isr(void);
interrupt void ECAN0_ISR(void);
interrupt void XINT13_isr(void);
interrupt void XINT1_isr(void);
interrupt void XINT2_isr(void);
interrupt void PDPINTB_isr(void);


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


///////////////////IO口初始化//////////////////////////////////

void InitGpio(void)
{

   EALLOW;
   GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOA设置为I/O口 
   GpioMuxRegs.GPADIR.all=0xFFFF;   //A0-A15设置为输出	
   GpioDataRegs.GPADAT.all=0x23f; 

   GpioMuxRegs.GPBMUX.all=0x0000;    //GPIOB设置为I/O口 
   GpioMuxRegs.GPBDIR.all=0xFFDF;    //B5设置为输入，其余设置为输出	
   GpioDataRegs.GPBDAT.all=0x8010;   
     
   GpioMuxRegs.GPFMUX.all=0x00c0;  //设置GPIOF6/7为eCAN口,其余设置为I/O口
   GpioMuxRegs.GPFDIR.bit.GPIOF9=1;
   GpioMuxRegs.GPFDIR.bit.GPIOF11=1;
   GpioMuxRegs.GPFDIR.bit.GPIOF12=0;
   GpioMuxRegs.GPFDIR.bit.GPIOF13=0;
   GpioDataRegs.GPFDAT.bit.GPIOF9=1;
   GpioDataRegs.GPFDAT.bit.GPIOF11=0;
   EDIS;

}	

///////////////////外部中断13初始化//////////////////////////////////

void InitXINT13(void)
{  GpioMuxRegs.GPEMUX.bit.XNMI_XINT13_GPIOE2=1; //设置中断13功能 
   XIntruptRegs.XNMICR.all=0x2;	 //下降沿生成中断
}

///////////////////外部中断1初始化//////////////////////////////////

void InitXINT1(void)
{  GpioMuxRegs.GPEMUX.bit.XINT1_XBIO_GPIOE0=1; //设置外恐卸�1功能 
   XIntruptRegs.XINT1CR.all=0x5;      //上升沿生成中断
}

///////////////////外部中断2初始化//////////////////////////////////

void InitXINT2(void)
{  GpioMuxRegs.GPEMUX.bit.XINT2_ADCSOC_GPIOE1=1; //设置外部中断13功能 
   XIntruptRegs.XINT2CR.all=0x5;      //上升沿生成中断
}

///////////////////PDPINTB中断初始化//////////////////////////////////

void InitPDPINTB(void)
{  GpioMuxRegs.GPDMUX.bit.T3CTRIP_PDPB_GPIOD5=1; //置瞎δ�
   EvbRegs.EVBIMRA.bit.PDPINTB=1;   //市鞵DPINTB中
   EvbRegs.EVBIFRA.bit.PDPINTB=1;   //清除PDPINTB中断标志              
}

///////////////////定时器1初始化（100mS�//////////////////////////////////

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


///////////////////定时器2初始化（100mS）//////////////////////////////////

void init_eva_timer2(void)
{
  
    EvaRegs.GPTCONA.all = 0;   //初始化定逼�2并设置定时器2寄存器
    EvaRegs.EVAIMRB.bit.T2PINT = 1;  //允许定时器2周期中断
    EvaRegs.EVAIFRB.bit.T2PINT = 1;  //清除定时器2周期中断标志
             
    EvaRegs.T2CON.all = 0x170C;  //Timer2黾剖模式，预分频值为128，
	                             //使用内部高速外设时钟
	EvaRegs.T2PR = 0x5b8d;   //Timer2的周期寄存器=0.1*30MHz/128
	EvaRegs.T2CNT = 0x0000;  //Timer2的计数器清

}

///////////////////AD初始化//////////////////////////////////
void InitAdc(void)
{
		
	AdcRegs.ADCTRL1.bit.RESET=1;  //复位ADC模块
	for(i=0;i<5000;i++)	 {} 
	AdcRegs.ADCTRL1.bit.RESET=0;
	AdcRegs.ADCTRL1.bit.SUSMOD=3;
	AdcRegs.ADCTRL1.bit.ACQ_PS=0;
	AdcRegs.ADCTRL1.bit.CPS=0;     
	AdcRegs.ADCTRL1.bit.CONT_RUN=0; //AD为启动/停止模式
	AdcRegs.ADCTRL1.bit.SEQ_CASC=1; //16级联模式
     	
	AdcRegs.ADCTRL3.bit.ADCBGRFDN=3; //带间隙参考电路上电
	for(i=0;i<10000;i++) {}           //等待稳定
	AdcRegs.ADCTRL3.bit.ADCPWDN=1;  //其它电路上电
	for(i=0;i<5000;i++)	 {}           //等待稳定
	AdcRegs.ADCTRL3.bit.ADCCLKPS=15;  
	AdcRegs.ADCTRL3.bit.SMODE_SEL=0;  //顺序采样

	
	AdcRegs.MAX_CONV.bit.MAX_CONV=15;  //采样通道为16
  
    AdcRegs.CHSELSEQ1.all= channel;   //同一个通道采16次
    AdcRegs.CHSELSEQ2.all= channel;
    AdcRegs.CHSELSEQ3.all= channel;
    AdcRegs.CHSELSEQ4.all= channel;

    AdcRegs.ADCTRL2.all=0x8c04;

	////////////采用定时器4启动AD转换/////////////////////
    EvbRegs.GPTCONB.all=0x400;    //定时器4周期中断启动AD转换
    EvbRegs.EVBIMRB.bit.T4PINT = 1;  //允许定时器4周期中断
    EvbRegs.EVBIFRB.bit.T4PINT = 1;  //清除定时器4周期中断标志
             
    EvbRegs.T4CON.all = 0x170C;  //Timer4为连续增计数模式，预分频值为128，
	                             //使用内部高速外设时钟
	EvbRegs.T4PR = 0xea;     //Timer4的周期寄存器=0.001*30MHz/128
	EvbRegs.T4CNT = 0x0000;  //Timer4的计数器清零
				
	AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;
	AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;	
			
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
	XintfRegs.XTIMING6.bit.XWRTRAIL = 2;    //写操作跟踪阶段的周期=XTIMCLK
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
		
	ECanaRegs.CANME.all = 0;  //禁止所有邮箱

	if(flag1==0)
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
    ECanaRegs.CANMC.bit.DBO = 0; //首先接收或发送数据的最高有效字节
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

///////////////控制器代码交换///////////////////////////////////
void MBOX0Send(void)
{  
    ECanaMboxes.MBOX0.MDRL.all = ((Uint32)FSGZJ_271<<16)+(Uint32)FSDZJ_271;
	ECanaMboxes.MBOX0.MDRH.all = 0x444d3143;
	ECanaRegs.CANTRS.bit.TRS0=1;   //启动发送	
    for(i=0;i<0xffff;i++) {}
    ECanaRegs.CANTA.bit.TA0=1;
    ECanaRegs.CANTRS.bit.TRS0=0;
	for(i=0;i<0xffff;i++) {}

    ECanaMboxes.MBOX0.MDRL.all = ((Uint32)FSGZJ_272<<16)+(Uint32)FSDZJ_272;
	ECanaMboxes.MBOX0.MDRH.all = 0x444d3243;
	ECanaRegs.CANTRS.bit.TRS0=1;   //启动发送	
	for(i=0;i<0xffff;i++) {}
    ECanaRegs.CANTA.bit.TA0=1;
    ECanaRegs.CANTRS.bit.TRS0=0;
    for(i=0;i<0xffff;i++){}
 
    ECanaMboxes.MBOX0.MDRL.all = ((Uint32)FSGZJ_273<<16)+(Uint32)FSDZJ_273;
	ECanaMboxes.MBOX0.MDRH.all = 0x444d3343;
	ECanaRegs.CANTRS.bit.TRS0=1;   //启动发送
	for(i=0;i<0xffff;i++) {}
    ECanaRegs.CANTA.bit.TA0=1;
    ECanaRegs.CANTRS.bit.TRS0=0;
    for(i=0;i<0xffff;i++){}

    if((JSDZJ&0xff)==0x8b)
    { ECanaMboxes.MBOX0.MDRL.all = ((Uint32)JSGZJ<<16)+(Uint32)JSDZJ;
      ECanaMboxes.MBOX0.MDRH.all = 0x444d3443;
	  ECanaRegs.CANTRS.bit.TRS0=1;   //舣发送	
      for(i=0;i<0xffff;i++) {}
      ECanaRegs.CANTA.bit.TA0=1;
      ECanaRegs.CANTRS.bit.TRS0=0;
    }
	can1++;
}

///////////////控制器发送到试验器///////////////////////////////////
void MBOX1Send(void)
{   rambase3=(Uint16 *)RAMBASE3;
    while(1)
    { readDA3();
      ECanaMboxes.MBOX1.MDRL.all=((Uint32)rambase3<<8)+(Uint32)tmp&0xff;
	  ECanaMboxes.MBOX1.MDRH.all=0x46533030;
	  ECanaRegs.CANTRS.bit.TRS1=1;   	
	  for(i=0;i<0xffff;i++) {}
      ECanaRegs.CANTA.bit.TA1=1;
      ECanaRegs.CANTRS.bit.TRS1=0;
	  for(i=0;i<5000;i++){}
	}
}

/////////////通过总线向外部存储器写硬件版本号////////////////////
void saveDA1(void)
{  
   *rambase1=tmp&0xff; 	
   rambase1++;    
}

/////////////通过总线向外部存储器写总故障次数 ////////////////////
void saveDA2(void)
{  
   *rambase2=tmp&0xff; 	
   rambase2++;    
}

/////////////通过总线向外部存储器写状态信息码////////////////////
void saveDA3(void)
{  
   *rambase3=tmp&0xff; 	
   rambase3++;    
}

/////////////通过总线读取上次故障次数////////////////////
void readDA1(void)
{ 
   tmp1=(*rambase1)&0xff;       //将数据总线上的数据读到变量tmp1中
   rambase1++;   
}

/////////////通过总线读取上次故障次数////////////////////
void readDA2(void)
{ 
   tmp1=(*rambase2)&0xff;       //将数据总线上的数据读到变量tmp中
   rambase2++;   
}

/////////////通过总线读取上次状态信息码结束处////////////////////
void readDA3(void)
{ 
   tmp1=(*rambase3)&0xff;       //将数据总线上的数据读到变量tmp中 
   rambase3++; 
}

//////////////清除存储器数据/////////////////////////////
void QCdata(void)
{
  rambase3=(Uint16 *)RAMBASE3;
  rambase6=(Uint16 *)RAMBASE6;
  for(;rambase3<rambase6;rambase3++)
  { *rambase3=0xff;
  } 
  
}

////////////ADC校正////////////////////////////////////

void ADC_adjust(void)
{    
   ///////采集ADCINB6(2.5V)/////////
   channel=0xeded; 
   ADC_sample();
   a1=a3;      
   ///////采集ADCINB7(1.25V)///////////   
   channel=0xfdfd; 
   ADC_sample();  
   a2=a3;       
   //////////计算增益和偏置/////////////////
   gain=(float)(HIGH_COUNT-LOW_COUNT)/(a1-a2); 
   offest=a2*gain-1733.0;
   flag2=1;     //ADC校正完毕
}

///////////////软件代码检查////////////////////////////////
void DMJC(void)
{ /////////检查存储器读写是否正常//////////////////////  
   tmp=0x11;
   *rambase1=tmp;
   for(i=0;i<10000;i++){}
   tmp1=*rambase1;
   rambase1=(Uint16 *)RAMBASE1; 
   if(tmp1!=tmp)  //存储器故障
   { GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU发出故障信号  
     gz++;   
   }
   /////////检查定时器是否正常//////////////////////////
   EvaRegs.T1CON.bit.TENABLE=1;   //打开定时器
   EvaRegs.T2CON.bit.TENABLE=1;
   while(1)         
   { if(EvaTimer2InterruptCount==10)  //延时1S
     break;	 
   }
   EvaRegs.T1CON.bit.TENABLE=0;   //关闭定时器
   EvaRegs.T2CON.bit.TENABLE=0;
   if(EvaTimer1InterruptCount!=10)  //定时器故障
   { GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU发出故障信号
     gz++; 
   }
   EvaTimer1InterruptCount=0;
   EvaTimer2InterruptCount=0;

}

/////////////////热刀短路传感器检查///////////////////////
void redao_DLJC(void)
{  GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0xa; //左机翼热刀
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))  //上电自检或地面维护时
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[0]++;
	 }
     if(sd[0]==2)
	 { sd[0]=0;
	   FSGZJ_272=FSGZJ_272|0x1;    //左1/2配电盒故障
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU发出故障信号
	 }
   } 
   
   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all|0xf;  //右机翼热刀
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))
   {  if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[1]++;
	 }
     if(sd[1]==2)
	 { sd[1]=0;	 
	   FSDZJ_272=FSDZJ_272|0x8000;  //右1/2配电盒故障
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU发出故障信号
	 }
   }
 
}

/////////////////1号段短路传感器检查///////////////////////
void abc1_DLJC(void)
{  GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0x6; //左机翼1号段
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[2]++;
	 }
     if(sd[2]==2)
	 { sd[2]=0;
	   FSGZJ_272=FSGZJ_272|0x1;    //左1/2配电盒故障
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU发出故障信号
	 }
   }
   
   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0xb; //右机翼1号段
   for(i=0;i<0x5000;i++) {}  
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[3]++;
	 }
     if(sd[3]==2)
	 { sd[3]=0;
	   FSDZJ_272=FSDZJ_272|0x8000;  //右1/2配电盒故障
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU发出故障信号
	 }
   }

}

/////////////////2号段短路传感器检查///////////////////////  
void abc2_DLJC(void) 
{  GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0x7;  //左机翼2号段
   for(i=0;i<0x5000;i++) {}  
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[4]++;
	 }
     if(sd[4]==2)
	 { sd[4]=0;
	   FSGZJ_272=FSGZJ_272|0x1;    //左1/2配电盒故障
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU发出故障信号
	 }
   }
      
   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0xc; //右机翼2号段
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[5]++;
	 }
     if(sd[5]==2)
	 { sd[5]=0;
	   FSDZJ_272=FSDZJ_272|0x8000;  //右1/2配电盒故障
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU发出故障信号
	 }
   }
}

/////////////////3号段短路传感器检查///////////////////////  
void abc3_DLJC(void)
{  GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0x8;  //左机翼3号段
   for(i=0;i<0x5000;i++) {}   
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[6]++;
	 }
     if(sd[6]==2)
	 { sd[6]=0; 
	   FSGZJ_272=FSGZJ_272|0x1;    //左1/2配电盒故障
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU发出故障信号
	 }
   }

   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0xd;    //右机翼3号段
   for(i=0;i<0x5000;i++) {}  
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[7]++;
	 }
     if(sd[7]==2)
	 { sd[7]=0; 
	   FSDZJ_272=FSDZJ_272|0x8000;  //右1/2配电盒故障
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU发出故障信号
	 }
   }

}

/////////////////4号段短路传感器检查///////////////////////  
void abc4_DLJC(void)
{  GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0x9;    //左机翼4号段
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[8]++;
	 }
     if(sd[8]==2)
	 { sd[8]=0; 
	   FSGZJ_272=FSGZJ_272|0x1;    //左1/2配电盒故障
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU发出故障信号
	 }
   }

   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0xe; //右机翼4号段
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[9]++;
	 }
     if(sd[9]==2)
	 { sd[9]=0;
	   FSDZJ_272=FSDZJ_272|0x8000;  //右1/2配电盒故障
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU出故障信号
	 }
   }
}

/////////////上电自检//////////////////////////////////

void poweron_BIT(void)
{  
   flag9=1;
   FSDZJ_273=FSDZJ_273|0x2000;     //正在自检
   DMJC();

   //////////检查短路传感器是否正常///////////////
   /*GpioDataRegs.GPFDAT.bit.GPIOF9=0;  //短路传感器自检
   GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //打开短路测试通道选择
   for(i=0;i<0x124f80;i++){}
   redao_DLJC();
   abc1_DLJC();
   abc2_DLJC();
   abc3_DLJC();
   abc4_DLJC();
   for(i=0;i<0x124f80;i++){}
   redao_DLJC();
   abc1_DLJC();
   abc2_DLJC();
   abc3_DLJC();
   abc4_DLJC();
   GpioDataRegs.GPBDAT.bit.GPIOB4=1;
   GpioDataRegs.GPFDAT.bit.GPIOF9=1;  //关闭自检电源*/

   if(GpioDataRegs.GPBDAT.bit.GPIOB15==0)
   { flag3=1;
   }
   FSDZJ_273=FSDZJ_273&0xdfff;   // 自检完成 
   flag9=0;      

}

////////////////地面维护BIT////////////////////////////
void DMWH_BIT(void)
{  flag10=1;
   FSDZJ_273=FSDZJ_273|0x2000;     //正在自检  
   DMJC();

   for(i=0;i<40000;i++){}
   /*if(GpioDataRegs.GPFDAT.bit.GPIOF12==1) 
   { EvaRegs.T1CON.bit.TENABLE=1;  // 打开定时器1
     GpioDataRegs.GPADAT.bit.GPIOA9=0;   //打开安全开关
     GpioDataRegs.GPADAT.bit.GPIOA15=1;   //热刀加热
     GpioDataRegs.GPADAT.bit.GPIOA14=1;   //4区加热,时刻1      
     GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //打开短路测试道≡�
	 for(i=0;i<0xffff;i++){}
     redao_DLJC();     
     //redao_temperature();
     abc4_DLJC();
     //abc4_temperature();
     //QXJC();  
     
      ////////////地面维护加热时序//////////////////////////////
     if(EvaTimer1InterruptCount==8)          //时刻2              
     { GpioDataRegs.GPADAT.bit.GPIOA14=0; 
       GpioDataRegs.GPADAT.bit.GPIOA13=1;
       abc3_DLJC();
	   //QXJC();
       //abc3_temperature();
     }
     if(EvaTimer1InterruptCount==16)         //时刻3             
     { GpioDataRegs.GPADAT.bit.GPIOA13=0; 
       GpioDataRegs.GPADAT.bit.GPIOA12=1; 
	   abc2_DLJC();
	   //QXJC();
       //abc2_temperature();
     }
     if(EvaTimer1InterruptCount==24)          //时刻4             
     { GpioDataRegs.GPADAT.bit.GPIOA12=0; 
       GpioDataRegs.GPADAT.bit.GPIOA11=1; 
	   abc1_DLJC();
	   //QXJC();
       //abc1_temperature();	 
     }
     EvaTimer1InterruptCount=0;        
     GpioDataRegs.GPADAT.bit.GPIOA11=0;       
     GpioDataRegs.GPADAT.bit.GPIOA15=0;
     GpioDataRegs.GPADAT.bit.GPIOA9=0;	 
	 GpioDataRegs.GPBDAT.bit.GPIOB4=1;  //关闭短路测试通道选择 
     EvaRegs.T1CON.bit.TENABLE=0; 
   }*/
   
   if(GpioDataRegs.GPBDAT.bit.GPIOB15==0)
   { flag4=1;
   }
   FSDZJ_273=FSDZJ_273&0xdfff;   // 自检完成 
   flag10=0; 
   arinc429fasong(); //429上报自检信息
   MBOX0Send();  

}


/////////////429发送////////////////////////////////
void arinc429fasong(void)
{ 
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;   
   *rambase6=FSDZJ_271;
   for(i=0;i<5;i++){} 
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;   
   *rambase6=FSGZJ_271;
   for(i=0;i<5;i++){}
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
   
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;  
   *rambase6=FSDZJ_272;
   for(i=0;i<5;i++){} 
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;   
   *rambase6=FSGZJ_272;
   for(i=0;i<5;i++){}
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;

   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;   
   *rambase6=FSDZJ_273;
   for(i=0;i<5;i++){} 
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;   
   *rambase6=FSGZJ_273;
   for(i=0;i<5;i++){}
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;

   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;   
   *rambase6=FSDZJ1;
   for(i=0;i<5;i++){} 
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;   
   *rambase6=FSGZJ1;
   for(i=0;i<5;i++){}
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;

}

///////////////加热控制////////////////////////////////////
void jiare(void)
{  if(flag13==0)
   { GpioDataRegs.GPADAT.bit.GPIOA9=0;   //打开安全开关 
	 FSGZJ_271=FSGZJ_271|0x2000; //尾翼防除冰系统开
	  ///////延时2s////////////////////////////
     if(flag22==0)
     { EvaRegs.T2CON.bit.TENABLE=1;
       while(1)
       { if(EvaTimer2InterruptCount>=20)
         {EvaTimer2InterruptCount=0;	      
	      break;
		 }
		 flag22=1;
	   } 
	   EvaRegs.T2CON.bit.TENABLE=0;
     }      
     GpioDataRegs.GPADAT.bit.GPIOA15=1;   //热刀加热
	 FSDZJ_272=FSDZJ_272|0x2000;   //正在进行加热
   }

    ////////////加热时序//////////////////////////////
    if((0<count3)&&(count3<=(t1-2)))                     
    { GpioDataRegs.GPADAT.bit.GPIOA11=0;
      GpioDataRegs.GPADAT.bit.GPIOA12=0;
      GpioDataRegs.GPADAT.bit.GPIOA13=0; 
      GpioDataRegs.GPADAT.bit.GPIOA14=1; 
           
    }
	if((count3>(t1-2))&&(count3<=(t1+2)))
	{ count6=0;
	  GpioDataRegs.GPADAT.bit.GPIOA11=0;
      GpioDataRegs.GPADAT.bit.GPIOA12=0;
      GpioDataRegs.GPADAT.bit.GPIOA13=0; 
      GpioDataRegs.GPADAT.bit.GPIOA14=0;
	}
    if((count3>(t1+2))&&(count3<=(2*t1-2)))                       
    { GpioDataRegs.GPADAT.bit.GPIOA11=0;
      GpioDataRegs.GPADAT.bit.GPIOA12=0;
      GpioDataRegs.GPADAT.bit.GPIOA14=0; 
      GpioDataRegs.GPADAT.bit.GPIOA13=1;      
    }
    if((count3>(2*t1-2))&&(count3<=(2*t1+2)))
	{ count7=0;
	  GpioDataRegs.GPADAT.bit.GPIOA11=0;
      GpioDataRegs.GPADAT.bit.GPIOA12=0;
      GpioDataRegs.GPADAT.bit.GPIOA13=0; 
      GpioDataRegs.GPADAT.bit.GPIOA14=0;
	}
    if((count3>(2*t1+2))&&(count3<=(3*t1+2)))                       
    { GpioDataRegs.GPADAT.bit.GPIOA11=0;
      GpioDataRegs.GPADAT.bit.GPIOA13=0;
      GpioDataRegs.GPADAT.bit.GPIOA14=0; 
      GpioDataRegs.GPADAT.bit.GPIOA12=1;      
    }
	if((count3>(3*t1-2))&&(count3<=(3*t1+2)))
	{ count8=0;
	  GpioDataRegs.GPADAT.bit.GPIOA11=0;
      GpioDataRegs.GPADAT.bit.GPIOA12=0;
      GpioDataRegs.GPADAT.bit.GPIOA13=0; 
      GpioDataRegs.GPADAT.bit.GPIOA14=0;
	}
    if((count3>(3*t1+2))&&(count3<=(4*t1+2)))                    
    { GpioDataRegs.GPADAT.bit.GPIOA12=0;
      GpioDataRegs.GPADAT.bit.GPIOA13=0;
      GpioDataRegs.GPADAT.bit.GPIOA14=0; 
      GpioDataRegs.GPADAT.bit.GPIOA11=1;      
    }
    if((count3>(4*t1+2))&&(count3<=6*t1))                      
    { count9=0;
      GpioDataRegs.GPADAT.bit.GPIOA12=0;
      GpioDataRegs.GPADAT.bit.GPIOA13=0;
      GpioDataRegs.GPADAT.bit.GPIOA14=0; 
      GpioDataRegs.GPADAT.bit.GPIOA11=0;   
    }
	if(count3>6*t1)
	{ count3=0;
      t1=t2;      
      GpioDataRegs.GPBDAT.bit.GPIOB4=1;  //关闭短路测试通道选择
      if(GpioDataRegs.GPBDAT.bit.GPIOB4==1)
      { if(GpioDataRegs.GPBDAT.bit.GPIOB5==0)
        {GpioDataRegs.GPBDAT.bit.GPIOB15=1;
	    }
      }  
	}

	if(GpioDataRegs.GPBDAT.bit.GPIOB4==1)
     { if(GpioDataRegs.GPBDAT.bit.GPIOB5==0)
      {GpioDataRegs.GPBDAT.bit.GPIOB15=1;
	  }
     }       
    GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //打开短路测试通道选择

    if((GpioDataRegs.GPADAT.bit.GPIOA15==1)&&(count5>=10))   
    {  count5=0;
       //redao_temperature(); 
    }    
	if((GpioDataRegs.GPADAT.bit.GPIOA14==1)&&(count6>=10)) 
	{  count6=0;
	   QXJC();
       //abc4_temperature();	
	}
	if((GpioDataRegs.GPADAT.bit.GPIOA13==1)&&(count7>=10)) 
	{  count7=0;
	   QXJC();
       //abc3_temperature();	
	}
	if((GpioDataRegs.GPADAT.bit.GPIOA12==1)&&(count8>=10)) 
	{  count8=0;
	   QXJC();
       //abc2_temperature();	
	}
	if((GpioDataRegs.GPADAT.bit.GPIOA11==1)&&(count9>=10)) 
	{  count9=0;
	   QXJC();
       //abc1_temperature();	
	}
}

//////////////信号采集///////////////////////////////
void ADC_sample(void)
{  InitAdc(); 
   EvbRegs.T4CON.bit.TENABLE=1;
   while(1)
   {if(n==8)
	break;
   } 
   n=0;	 
   if(flag2==1) //上电自检完后修正采集电压
   { a3=(int)(gain*a3-offest);
   }		
   EvbRegs.T4CON.bit.TENABLE=0;
}  

/////////////////热刀温度监测//////////////////////////////
/*void redao_temperature(void)
{ 
    GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1cc0;   //左1热刀      
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095; 
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[3]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x400;     //左1段超温
	  gz++; 
	  k[3]=k[3]+1;                     
	  if(k[3]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	  gz++; 
	  FSGZJ_272=FSGZJ_272|0x400;     //左1段超温
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1dc0;   //左2热刀     
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[7]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x200;     //左2段超温
	  gz++; 
	  k[7]=k[7]+1;                      //记录超温时间
	  if(k[7]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x200;     //左2段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x16c0;   //左3热刀
    channel=0x8d8d;       //ADCINB0采集温度传感器信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[11]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x100;     //左3段超温
	  gz++; 
	  k[11]=k[11]+1;                      //记录超温时间
	  if(k[11]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x100;     //左3段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x17c0;   //左4热刀
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[15]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x80;     //左4段超温
	  gz++; 
	  k[15]=k[15]+1;                      //记录超温时间
	  if(k[15]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x80;     //左4段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1ac0;   //右1热刀 
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[19]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x40;     //右1段超温
	  gz++; 
	  k[19]=k[19]+1;                      //记录超温时间
	  if(k[19]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x40;     //右1段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1bc0;   //右2热刀
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[23]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x20;     //右2段超温
	  gz++; 
	  k[23]=k[23]+1;                      //记录超温时间
	  if(k[23]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x20;     //右2段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xec0;   //右3热刀 
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[27]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x10;     //右3段超温
	  gz++; 
	  k[27]=k[27]+1;                      //记录率奔�
	  if(k[27]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x10;     //右3段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xfc0;   //右4热刀 
    channel=0x8d8d;       //ADCINB0采集露全感器缪剐藕�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[31]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x8;     //右4段超温
	  gz++; 
	  k[31]=k[15]+1;                      //记录超温时间
	  if(k[31]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x8;     //右4段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}
}

/////////////////左/右4abc温度监测//////////////////////////////
void abc4_temperature(void)
{   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1700;   //左4a
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[12]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x80;     //左4段超温
	  gz++; 
	  k[12]=k[12]+1;                      //记录超温时间
	  if(k[12]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x80;     //左4段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1740;   //左4b
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[13]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x80;     //左4段超温
	  gz++; 
	  k[13]=k[13]+1;                      //记录超温时间
	  if(k[13]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x80;     //左4段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1780;   //左4c 
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();	
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[14]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x80;     //左4段超温
	  gz++; 
	  k[14]=k[14]+1;                      //记录超温时间
	  if(k[14]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x80;     //左4段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xf00;   //右4a 
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[28]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x8;     //右4段超温
	  gz++; 
	  k[28]=k[28]+1;                      //记录超温时间
	  if(k[28]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x8;     //右4段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xf40;   //右4b  
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[29]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x8;     //右4段超温
	  gz++; 
	  k[29]=k[29]+1;                      //记录超温时间
	  if(k[29]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x8;     //右4段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xf80;   //右4c 
    channel=0x8d8d;       //ADCINB0采集温度传衅鞯缪剐藕�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[23]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x8;     //右4段超温
	  gz++; 
	  k[31]=k[31]+1;                      //记录超温时间
	  if(k[31]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x8;     //右4段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	 
	}
}

/////////////////左/右3abc温度监测//////////////////////////////
void abc3_temperature(void)
{   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1600;   //左3a  
    channel=0x8d8d;       //ADCINB0采集温雀绪电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[8]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x100;     //左3段超温
	  gz++; 
	  k[8]=k[8]+1;                      //记录超温时间
	  if(k[8]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x100;     //左3段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1640;   //左3b 
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[9]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x100;     //左3段超温
	  gz++; 
	  k[9]=k[9]+1;                      //记录超温时间
	  if(k[9]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x100;     //左3段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1680;   //左3c 
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[10]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x100;     //左3段超温
	  gz++; 
	  k[10]=k[10]+1;                      //记录超温时间
	  if(k[10]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x100;     //左3段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xe00;   //右3a 
    channel=0x8d8d;       //ADCINB0采集温度传感器缪剐藕�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[24]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x10;     //右3段�
	  gz++; 
	  k[24]=k[24]+1;                      //记录超温时间
	  if(k[24]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x10;     //右3段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xe40;   //右3b 
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[25]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x10;     //右3段超温
	  gz++; 
	  k[25]=k[25]+1;                      //记录超温时间
	  if(k[25]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x10;     //右3段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报�	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xe80;   //右3c 
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[26]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x10;     //右3段超温
	  gz++; 
	  k[26]=k[26]+1;                      //记录超温时间
	  if(k[26]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x10;     //右3段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}
}

/////////////////左/右2abc温度监测//////////////////////////////
void abc2_temperature(void)
{   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1d00;   //左2a 
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[4]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x200;     //左2段超温
	  gz++; 
	  k[4]=k[4]+1;                      //记录超温时间
	  if(k[4]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x200;     //左2段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1d40;   //左2b 
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[5]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x200;     //左2段�
	  gz++; 
	  k[5]=k[5]+1;                      //记录超温时间
	  if(k[5]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x200;     //左2纬�
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1d80;   //左2c 
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[6]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x200;     //左2段超温
	  gz++; 
	  k[6]=k[6]+1;                      //记录超温时间
	  if(k[6]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x200;     //左2段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1b00;   //右2a 
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[20]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x20;     //右2段超温
	  gz++; 
	  k[20]=k[20]+1;                      //记录超温时间
	  if(k[20]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x20;     //右2段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1b40;   //右2b  
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[21]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x20;     //右2段超温
	  gz++; 
	  k[21]=k[21]+1;                      //记录超温时间
	  if(k[21]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x20;     //2段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1b80;   //右2c 
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[22]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x20;     //右2段超温
	  gz++; 
	  k[22]=k[22]+1;                      //记录超温时间
	  if(k[22]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x20;     //右2段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}
}

/////////////////左/右1abc温度监测//////////////////////////////
void abc1_temperature(void)
{ 
    GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1c00;   //左1a 
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[0]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x400;     //左1段超温
	  gz++; 
	  k[0]=k[0]+1;                      //记超温输
	  if(k[0]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x400;     //左1段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}
    
	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1c40;   //左1b
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[1]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x400;     //左1段超温
	  gz++; 
	  k[1]=k[1]+1;                      //记录超温时间
	  if(k[1]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x400;     //左1段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}
	
	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1c80;   //左1c
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[2]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x400;     //左1段超温
	  gz++; 
	  k[2]=k[2]+1;                      //记录超温时间
	  if(k[2]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x400;     //左1段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

    GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1a00;   //右1a  
    channel=0x8d8d;       //ADCINB0采集温度传衅鞯缪剐藕�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[20]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x40;     //右1段超温
	  gz++; 
	  k[20]=k[20]+1;                      //记录超温时间
	  if(k[20]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x40;     //右1段超温 
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1a40;   //右1b 
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[21]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x40;     //右1段超温
	  gz++; 
	  k[21]=k[21]+1;                      //记录超温时间
	  if(k[21]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x40;     //右1段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1a80;   //右1c
    channel=0x8d8d;       //ADCINB0采集温度传感器电压信号
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[22]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x40;     //右1纬�
	  gz++; 
	  k[22]=k[22]+1;                      //记录超温时间
	  if(k[22]>24)                    //超温25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x40;     //右1段超温
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU报故	  
	}
}*/

//////////////左平尾加热电流判断是否开路///////////////////
void ZDLJC(void)
{ 
  if((qxdl<15.0)||(qxdl>58.0))
  { 
	GpioDataRegs.GPADAT.bit.GPIOA9=1;
	if(GpioDataRegs.GPADAT.bit.GPIOA14==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x200;         //左4段断路
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA13==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x400;         //左第3段断路
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA12==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x800;         //左第2段断路
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA11==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x1000;         //左第1段断路
	}
	GpioDataRegs.GPBDAT.bit.GPIOB15=0;    //本控制器故障
	GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //关闭所有加热
  } 
}

//////////////右平尾加热电流判断是否开路///////////////////
void YDLJC(void)
{  
  if((qxdl<15.0)||(qxdl>58.0))
  { 
	GpioDataRegs.GPADAT.bit.GPIOA9=1;
	if(GpioDataRegs.GPADAT.bit.GPIOA14==1)
    { gz++; 
	  FSGZJ_271=FSGZJ_271|0x20;         //右第4段断路
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA13==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x40;         //右第3段断路
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA12==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x80;         //右第2段断路
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA11==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x100;         //右第1段断路	 
	}
    GpioDataRegs.GPBDAT.bit.GPIOB15=0;    //本控制器故障
	GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //关闭所有加热	
  }
}

///////////////////左平尾开路监控//////////////////////////////
void ZKLJC(void)
{  
   channel=0x0d0d;   //ADCINA0采集左t
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5;   //采集电压转换成电流
   ZDLJC();
    
   channel=0x1d1d;   //ADCINA1采集左s
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5;
   ZDLJC();
     
   channel=0x2d2d;   //ADCINA2采集左r
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5;
   ZDLJC();    
}

///////////////////右平尾开路监控//////////////////////////////
void YKLJC(void)
{ 	 
   channel=0x3d3d;   //ADCINA3采集右t
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5;
   YDLJC();

   channel=0x4d4d;   //ADCINA4采集右s
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5; 
   YDLJC();

   channel=0x5d5d;   //ADCINA5采集右r
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5; 
   YDLJC();   
}

///////////////左/右开路缺相监控////////////////////////////////
void QXJC(void)
{
   ZKLJC();
   if(GpioDataRegs.GPBDAT.bit.GPIOB15==0)
   { FSGZJ_272=FSGZJ_272|0x1;    //左配电盒故障	     
   }
   YKLJC();
   if(GpioDataRegs.GPBDAT.bit.GPIOB15==0)
   { FSDZJ_272=FSDZJ_272|0x8000;  //右配电盒故障
   }
   if(flag7==1)
   { if((FSGZJ_272&0x1==0x1)&&(FSDZJ_272&0x8000==0x0000)) //左故障右正常
     { FSGZJ_272=FSGZJ_272|0x1;    //左配电盒故障	
	 }
	 if((FSGZJ_272&0x1==0x0000)&&(FSDZJ_272&0x8000==0x8000)) //左正常右故障
	 { if(flag1==0)
	   { FSGZJ_272=FSGZJ_272|0x2;    //2#电源故障
	   }
	   else
	   { FSGZJ_272=FSGZJ_272|0x4;    //1#电源故障
	   }
	 }
	 else
	 { FSGZJ_271=FSGZJ_271|0x4000;   //防除冰系统失效
	 }
   }
   if(flag8==1)
   { if((FSGZJ_272&0x1==0x0000)&&(FSDZJ_272&0x8000==0x8000)) //左正常右故障
     { FSDZJ_272=FSDZJ_272|0x8000;  //右配电盒故障
	 }
     if((FSGZJ_272&0x1==0x1)&&(FSDZJ_272&0x8000==0x0000)) //左故障右正常
	 { if(flag1==0)
	   { FSGZJ_272=FSGZJ_272|0x4;    //1#电故障
	   }
	   else
	   { FSGZJ_272=FSGZJ_272|0x2;    //2#电源故障
	   }
	 }
	 else
	 { FSGZJ_271=FSGZJ_271|0x4000;   //防除冰系统失效
	 }
   }
}

///////////////////定时器1中断//////////////////////////////////

interrupt void eva_timer1_isr(void)
{  
  DINT;
  if(PieCtrl.PIEACK.bit.ACK2==1)
  { PieCtrl.PIEACK.bit.ACK2=1;
   
   /////////////////////////////////////////////////
   if((flag9==1)|(flag10==1))    //上电自检或地面自检时 
   { EvaTimer1InterruptCount++;
   }

   else
   { GpioDataRegs.GPADAT.bit.GPIOA8=~GpioDataRegs.GPADAT.bit.GPIOA8;
     count1++; 
     count2++;

	 ////////////////////////////////////////////////
	 if(flag21==1)
	 { count10++;	   
	   if(count10>=10)
	   { count10=0;	     
	    if((qxdl<15.0)||(qxdl>58.0))
        { flag22=0;
		  
		  if(GpioDataRegs.GPADAT.bit.GPIOA15==1)
          { FSGZJ_271=FSGZJ_271|0x1e;       //左边4段全部短路
	        gz++;	     
          }
	      if(GpioDataRegs.GPADAT.bit.GPIOA15==1)
          { FSGZJ_271=FSGZJ_271|0x1;	   //右边4段全部短路
	        FSDZJ_271=FSDZJ_271|0xe000;       
	        gz++; 
          }
	      if(GpioDataRegs.GPADAT.bit.GPIOA11==1)
          {  FSGZJ_271=FSGZJ_271|0x10;     //左第1段短路
	         gz++; 
          }
          if(GpioDataRegs.GPADAT.bit.GPIOA11==1)
          {  FSGZJ_271=FSGZJ_271|0x1;     //右1段短路
	         gz++; 
          }
          if(GpioDataRegs.GPADAT.bit.GPIOA12==1)
          {   FSGZJ_271=FSGZJ_271|0x8;     //左第2段短路
	          gz++; 
	      } 
	      if(GpioDataRegs.GPADAT.bit.GPIOA12==1)
	      { FSDZJ_271=FSDZJ_271|0x8000;     //右第2段短路
		    gz++; 
	      }
	      if(GpioDataRegs.GPADAT.bit.GPIOA13==1)
	      { FSGZJ_271=FSGZJ_271|0x4;     //左第3段短路
		    gz++;
	      } 
	      if(GpioDataRegs.GPADAT.bit.GPIOA13==1)
	      { FSDZJ_271=FSDZJ_271|0x4000;     //右第3段短路
		    gz++;
	      }
	      if(GpioDataRegs.GPADAT.bit.GPIOA14==1)
	      { FSGZJ_271=FSGZJ_271|0x1;     //左第4段短路
		    gz++; 
	      } 
	      if(GpioDataRegs.GPADAT.bit.GPIOA14==1)
	      { FSDZJ_271=FSDZJ_271|0x2000;     //右第4段短路
		    gz++; 
	      }	 
          GpioDataRegs.GPADAT.bit.GPIOA9=1;		  
          GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //关闭所有加热
	      //GpioDataRegs.GPBDAT.bit.GPIOB4=1;
		  GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU发出故障信号
         }
		 else
		 {flag21=0;
		 }
	   }
	 }

     ///////////////////////////////////////////////////
     if(GpioDataRegs.GPFDAT.bit.GPIOF12==1) //除冰启动
     { if(flag12==0)
       { m++;
		 if(m==2)
		 { flag12=1;
		   j=1;
		 }
       }
	   if(flag12==1)
	   {  
		 if(flag14==1)
		 { if(GpioDataRegs.GPADAT.bit.GPIOA15==0)
		   { count3=0;
		     count4=0;
		     count5=0;
		     count6=0;
		     count7=0;
		     count8=0;
		     count9=0;
		     t1=t2;
		   }		   
		 }		   		   
		 if(flag13==1)
		 { if(j==1)
		   { j=2;
		   }
		   else
		   { j=1;
		   }
		   flag13=0;
		   GpioDataRegs.GPADAT.bit.GPIOA9=1;
	       GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //关闭所有加热
		 }
         if(GpioDataRegs.GPADAT.bit.GPIOA15==1)
         { count3++;
		   count4++;
		   count5++;
		   if(GpioDataRegs.GPADAT.bit.GPIOA14==1)
		   {count6++;
		   }
		   if(GpioDataRegs.GPADAT.bit.GPIOA13==1)
		   {count7++;
		   }
		   if(GpioDataRegs.GPADAT.bit.GPIOA12==1)
		   {count8++;
		   }
		   if(GpioDataRegs.GPADAT.bit.GPIOA11==1)
		   {count9++;
		   }
		 }
	   }
	 }
     else
	 { GpioDataRegs.GPADAT.bit.GPIOA9=1;
	   flag22=0;
	   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff; //关闭所有加热
	   FSGZJ_271=FSGZJ_271&0xdfff; //尾翼防除冰系统关闭
	   FSDZJ_272=FSDZJ_272&0xdfff;  //没有进行加热工作
	   if(flag12==0)
	   { m=0;
	   }
	   else
	   { flag13=1;   //准备切换
	     count3=0;
	     count4=0;
	     count5=0;
	     count6=0;
	     count7=0;
	     count8=0;
	     count9=0;
	     if(flag14==1)
		 { flag12=0;
		   flag13=0;
		   flag14=0;
		   j=0;
		   m=0;
		 }
	   }
	 }	 	 
   }
   EvaRegs.EVAIMRA.bit.T1PINT = 1;
   EvaRegs.EVAIFRA.bit.T1PINT = 1;
  }
  EINT;

}

///////////////////定时器2中断//////////////////////////////////

interrupt void eva_timer2_isr(void)
{  
  DINT;
  if(PieCtrl.PIEACK.bit.ACK3==1)
  { PieCtrl.PIEACK.bit.ACK3=1;

   EvaTimer2InterruptCount++;

   EvaRegs.EVAIMRB.bit.T2PINT = 1;
   EvaRegs.EVAIFRB.bit.T2PINT = 1;
  }
  EINT;

}

///////////////////ADC中断//////////////////////////////////
interrupt void ad_isr(void)
{
  DINT;
  if(PieCtrl.PIEACK.bit.ACK1==1)
  { PieCtrl.PIEACK.bit.ACK1=1;

    a[0]=AdcRegs.RESULT0>>4;
    a[1]=AdcRegs.RESULT1>>4;
    a[2]=AdcRegs.RESULT2>>4;
    a[3]=AdcRegs.RESULT3>>4;
    a[4]=AdcRegs.RESULT4>>4;
    a[5]=AdcRegs.RESULT5>>4;
    a[6]=AdcRegs.RESULT6>>4;
    a[7]=AdcRegs.RESULT7>>4;
    a[8]=AdcRegs.RESULT8>>4;
    a[9]=AdcRegs.RESULT9>>4;
    a[10]=AdcRegs.RESULT10>>4;
    a[11]=AdcRegs.RESULT11>>4;
    a[12]=AdcRegs.RESULT12>>4;
    a[13]=AdcRegs.RESULT13>>4;
    a[14]=AdcRegs.RESULT14>>4;
    a[15]=AdcRegs.RESULT15>>4;
   
	a3=0;
	for(n=0;n<8;n++)  //取8个有效数据求平均
	{ a3=a3+a[2*n+1];
	}
    a3=a3/8;

    AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;
    AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;
  }	
  EINT;
	
}

///////////////////ECAN中断邮招畔�///////////////////////////////
interrupt void ECAN0_ISR(void)
{ 
  DINT;
  if(PieCtrl.PIEACK.bit.ACK9==1)
  { PieCtrl.PIEACK.bit.ACK9=1;

   	if(ECanaRegs.CANRMP.bit.RMP2==1) //接收另一控制器信息
    { TestMbox1=ECanaMboxes.MBOX2.MDRL.all;
      TestMbox2=ECanaMboxes.MBOX2.MDRH.all;
      if(TestMbox2==0x444d3143)      //查询271
	  { TestMbox1=(TestMbox1&0x1e000000)>>28;
	    if(TestMbox1>=0x1)
		{ flag7=1;       //另一控制器左平尾开路
		}
        TestMbox1=ECanaMboxes.MBOX2.MDRL.all;
        TestMbox1=(TestMbox1&0x1e00000)>>24;
	    if(TestMbox1>=0x1)
		{ flag8=1;       //另一控制器右平尾开路
		}
	  }
      ECanaRegs.CANRMP.bit.RMP2=1;
    }

    if(ECanaRegs.CANRMP.bit.RMP3==1)  //接收试验器信息
    { if(flag6==0)  //飞机在地面
      { TestMbox3=ECanaMboxes.MBOX3.MDRL.all;
        TestMbox4=ECanaMboxes.MBOX3.MDRH.all;
        ECanaRegs.CANRMP.bit.RMP3=1;

         //////接收到硬件版本号//////////////
        if((TestMbox4&0xffff0000)==0x58580000)
		{ 
		  //////////硬件版本号保存///////////////////
          tmp=(Uint16)TestMbox3&0xff;    
          saveDA1();
          tmp=(Uint16)(TestMbox3>>8)&0xff;
          saveDA1();
          tmp=(Uint16)(TestMbox3>>16)&0xff;
          saveDA1();
          tmp=(Uint16)(TestMbox3>>24)&0xff;
          saveDA1();
          tmp=(Uint16)TestMbox4&0xff; 
          saveDA1();
		}	    
         ////////接收到读存储器命令////////////
        if((TestMbox4&0xffff0000)==0x44530000)
        { MBOX1Send();    
		}
         ////////接收到清除存储器命令/////////////
        if((TestMbox4&0xffff0000)==0x51430000)
		{ QCdata(); 
		}
      }
    }
	can2++;
   }
   EINT;
}

///////////////////掉电保护中断//////////////////////////////////

/*interrupt void XINT13_isr(void)
{  
   DINT;

   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU发出故障信号
   GpioDataRegs.GPADAT.bit.GPIOA9=1;
   flag22=0;
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //关闭所有加热
   FSGZJ_271=FSGZJ_271&0xdfff;
   FSDZJ_272=FSDZJ_272&0xdfff;

   tmp=0x45;
   saveDA3();
   tmp=0x4e;
   saveDA3();
   tmp=0x44;
   saveDA3();
   while(1)
   {GpioDataRegs.GPADAT.bit.GPIOA8=~GpioDataRegs.GPADAT.bit.GPIOA8;
   }
}*/      

///////////////////429接收中断//////////////////////////////////

interrupt void XINT1_isr(void)
{ Uint16 ix;
  DINT;
  if(PieCtrl.PIEACK.bit.ACK1==1)
  { PieCtrl.PIEACK.bit.ACK1=1;
   	
   	flag15=1;	
    GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfffc; 
    for(ix=0;ix<10;ix++){}  
    JSDZJ=*rambase6; 
    for(ix=0;ix<10;ix++){}  
    GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
    GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfffd;
    for(ix=0;ix<10;ix++){}
    JSGZJ=*rambase6;
    for(ix=0;ix<10;ix++){}   
    GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
    
    FSDZJ1=JSDZJ;  
    FSGZJ1=JSGZJ;    
    /////////接收LABEL274信号/////////////////////////////
    if((JSDZJ&0xff)==0xbc)   
    { lz=0;
      flag16=1;
      if((JSDZJ&0x600)==0x600)  //274通讯正常
	  { if((JSDZJ&0x8000)==0x0000)  //飞机在地面
        { 
          flag6=flag6a;
          
          if((JSDZJ&0x4000)==0x4000)  //要求地面维护  
          { DMWH_BIT();
          }
	      else
	      { flag4=0;   //不要求地面维护,默认正常 
	      }
        }
	    else
	    { flag6=1;     //飞机在空中
	    }
	  }
    }
    //////////接收LABEL213静温信号//////////////////////////////////
    if((JSDZJ&0xff)==0x8b)
    { wd=0;
      flag18=1;
      if((JSDZJ&0x600)==0x600)   //213通讯正常 
	  { if((JSGZJ&0x8000)==0x0000)  //温度为正
        { JSGZJ=(JSGZJ&0x7ff0)>>4;
	      wd=(float)JSGZJ*0.25;
		  if((wd>=0.0)&&(wd<5.0))
		  { t2=250;
		    flag20=0; 
		  }
		  if(wd>=5.0)   
		  { GpioDataRegs.GPADAT.bit.GPIOA9=1;
            GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //关闭所有加热 
		    FSDZJ_272=FSDZJ_272&0xdfff; 
		    flag20=1; 
		  }              
	    }
	    else        //温度为负
	    { flag20=0; 
	      JSGZJ=(JSGZJ&0x7ff0)>>4;
		  JSGZJ=JSGZJ-0x1;
		  JSGZJ=~JSGZJ;	      
          JSGZJ=JSGZJ&0x7ff;
          wd=(float)JSGZJ*0.25;
		  if(wd>=20.0)   
		  { t2=380;
		  }
		  if((wd>0.0)&&(wd<20.0))
		  { t2=250;
		  }            
	    }
	  }             
      tmp=JSDZJ&0xff;    //保存到外部存储器中
      saveDA3();
      tmp=JSDZJ>>8;
      saveDA3();
      tmp=JSGZJ&0xff;    
      saveDA3();
      tmp=JSGZJ>>8;
      saveDA3();
    }
  }
  EINT;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              

///////////////////短路中断//////////////////////////////////

interrupt void XINT2_isr(void)
{ 
  DINT;
  if(PieCtrl.PIEACK.bit.ACK1==1)
  { PieCtrl.PIEACK.bit.ACK1=1;   
    if((flag9==0)&&(flag10==0))  //周期巡毂
    { if(GpioDataRegs.GPADAT.bit.GPIOA9==0)
      { flag21=1;	     
	  }
    }
  }
  EINT;   
}

////////////////工作/备份切�//////////////////////////
interrupt void PDPINTB_isr(void)
{  
  DINT;

  if(PieCtrl.PIEACK.bit.ACK1==1)
  { PieCtrl.PIEACK.bit.ACK1=1;
  
   if(j==0)   
   { if(flag1==0)
     { flag1=1;
	 }
     else
	 { flag1=0; 
	 }
   }
   if((j==1)&&(flag1==0))  //本控制器为左主,拒绝切换
   { flag11=0;        //主控制器正常
   }
   if((j==2)&&(flag1==0))  //本控制器为左辅
   { j=1;
     flag14=1;
   }
   
   if((j==2)&&(flag1==1))  //本控制器为右主,拒绝切换
   { flag11=0;        //主控制器正常
   }
   if((j==1)&&(flag1==1))  //本控制器为右辅
   { j=2;
     flag14=1;
   }
    
   EvbRegs.EVBIMRA.bit.PDPINTB=1; 
   EvbRegs.EVBIFRA.bit.PDPINTB=1;
  } 
  EINT;
}

///////////直接返回中断服务程序//////////////
interrupt void nothing()
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
   init_eva_timer2();
   InitXINT1();
   InitXINT2();
   InitXINT13();
   InitPDPINTB();	
   InitXintf();
		
   EALLOW;
   //PieVectTable.XINT13=&XINT13_isr;
   PieVectTable.XINT1=&XINT1_isr;
   PieVectTable.XINT2=&XINT2_isr;
   PieVectTable.PDPINTB=&PDPINTB_isr;	
   PieVectTable.ADCINT=&ad_isr;
   PieVectTable.T1PINT=&eva_timer1_isr;
   PieVectTable.T2PINT=&eva_timer2_isr;
   PieVectTable.ECAN0INTA = &ECAN0_ISR;
   EDIS;  
  
   //开中断
   IER |= (M_INT1|M_INT2|M_INT3|M_INT9);
   PieCtrl.PIEIER1.bit.INTx2=1;
   PieCtrl.PIEIER1.bit.INTx4=1;
   PieCtrl.PIEIER1.bit.INTx5=1;
   PieCtrl.PIEIER1.bit.INTx6=1;
   PieCtrl.PIEIER2.bit.INTx4=1;
   PieCtrl.PIEIER3.bit.INTx1=1;
   PieCtrl.PIEIER9.bit.INTx5=1;
   EINT;  
   ERTM;

   FSDZJ_271=0x00b9;   //初始化429信号
   FSGZJ_271=0x0000;  
   FSDZJ_272=0x40ba;   //上电默认左主控 
   FSGZJ_272=0x0000;
   FSDZJ_273=0x00bb;  
   FSGZJ_273=0x0000;
   FSDZJ_300=0x22c0;  
   FSGZJ_300=0x8306;
   FSDZJ_301=0x22c1;  
   FSGZJ_301=0x6d66;
   FSDZJ_302=0x42c2;  
   FSGZJ_302=0x9306;
   FSDZJ_303=0x22c3;  
   FSGZJ_303=0x9b06;  
   FSDZJ_304=0x22c4;  
   FSGZJ_304=0x8306;
   FSDZJ_305=0x02c5;  
   FSGZJ_305=0x6b16;
   FSDZJ_306=0x42c6;  
   FSGZJ_306=0x9306;
   FSDZJ_307=0x22c7;  
   FSGZJ_307=0x9b06; 
     
   ///////延时2s////////////////////////////
   EvaRegs.T2CON.bit.TENABLE=1;
   while(1)
   { if(EvaTimer2InterruptCount>=20)
     {EvaTimer2InterruptCount=0;
	  break;
	 }
   } 
   EvaRegs.T2CON.bit.TENABLE=0;

   //////读取硬件版本号/////////////////////////
   rambase1=(Uint16 *)RAMBASE1;    
   /*readDA1();
   banbenhao1=(Uint16)tmp1;
   readDA1();
   banbenhao2=(Uint16)tmp1;   
   readDA1();
   banbenhao3=(Uint16)tmp1;
   readDA1();
   banbenhao4=(Uint16)tmp1;
   readDA1();
   banbenhao5=(Uint16)tmp1;

   FSDZJ_304=((banbenhao1&0x7)<<13)+((banbenhao3&0x60)<<4)+0xc4;
   FSGZJ_304=((banbenhao3&0x1f)<<11)+((banbenhao2&0x7f)<<4)+(Uint16)((banbenhao1&0x78)>>3);
   FSDZJ_305=((banbenhao4&0x7)<<13)+0x2c5;
   FSGZJ_305=((banbenhao5&0x7f)<<4)+((banbenhao4&0x78)>>3)+0x6800;
      
   rambase1=(Uint16 *)RAMBASE1; */    
     
   //////读取上次总故障次数///////////////
   rambase2=(Uint16 *)RAMBASE2;
   readDA2();
   gz=(Uint32)tmp1;
   readDA2();
   gz=((Uint32)tmp1<<8)+gz;
   readDA2();
   gz=((Uint32)tmp1<<16)+gz;
   readDA2();
   gz=((Uint32)tmp1<<24)+gz;
   rambase2=(Uint16 *)RAMBASE2;

   ///////读取上次结束标志/////////////////////////   
   rambase3=(Uint16 *)RAMBASE3;
   readDA3();
   if(tmp1==0x45)
   { readDA3();
     if(tmp1==0x4e)
	 { readDA3();
	   if(tmp1==0x44)
	   { readDA3();
	   }
	 }
   }

   //左/右控制判断//////////////////////
   if(GpioDataRegs.GPFDAT.bit.GPIOF13==1)  
   { flag1=1;         //右控制器
   }
   else
   { flag1=0;        //左控制器	 
   }

   InitECan(); 
   ADC_adjust(); 
   poweron_BIT();  //上电自检测

   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffde;  //初始化3282
   CSH=0x0020;
   rambase6=(Uint16 *)RAMBASE6;
   *rambase6=CSH;
   for(i=0;i<5;i++){}
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
   arinc429fasong(); //429上报自检信息
   MBOX0Send();    //左右CAN交流
   
   if(flag3==0)  //上电自检正常
   {
      /////////硬件版本号发送/////////////////////
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;
      *rambase6=FSDZJ_300;
      for(i=0;i<5;i++){}
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;      
      *rambase6=FSGZJ_300;
	  for(i=0;i<5;i++){}
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;      
      *rambase6=FSDZJ_301;
	  for(i=0;i<5;i++){}
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;      
      *rambase6=FSGZJ_301;
	  for(i=0;i<5;i++){}
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;     
      *rambase6=FSDZJ_302;
	  for(i=0;i<5;i++){}
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;      
      *rambase6=FSGZJ_302;
	  for(i=0;i<5;i++){}
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;      
      *rambase6=FSDZJ_303;
	  for(i=0;i<5;i++){}
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;
      *rambase6=FSGZJ_303;
      for(i=0;i<5;i++){}
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;

      /////////软件版本号发送//////////////////////
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;      
      *rambase6=FSDZJ_304;
	  for(i=0;i<5;i++){}
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;      
      *rambase6=FSGZJ_304;
	  for(i=0;i<5;i++){}
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;      
      *rambase6=FSDZJ_305;
	  for(i=0;i<5;i++){}
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;      
      *rambase6=FSGZJ_305;
	  for(i=0;i<5;i++){}
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;      
      *rambase6=FSDZJ_306;
	  for(i=0;i<5;i++){}
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;      
      *rambase6=FSGZJ_306;
	  for(i=0;i<5;i++){}
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;      
      *rambase6=FSDZJ_307;
	  for(i=0;i<5;i++){}
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;      
      *rambase6=FSGZJ_307;
	  for(i=0;i<5;i++){}
      GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;

	  EvaRegs.T1CON.bit.TENABLE=1;  // 打开定时器1,空中工作

	  while(1)
	  { if(flag4==0) //地面维护正常
	    { if(count1>=18)     //429发送及CAN每1.8s发送一次
		  { count1=0;
            arinc429fasong();  //429总线发送
            MBOX0Send();	//CAN总线发送  
			if(can1!=can2)
			{//////保存无can数据代码/////////////////
              gz++;
              tmp=0x52;
			  saveDA3();
              tmp=0x52;
			  saveDA3();
			  tmp=0x43;
			  saveDA3();
			  tmp=0x45;
			  saveDA3();
			  tmp=0x45;
			  saveDA3();
			  tmp=0x54;
			  saveDA3();
			}

	        tmp=(char)gz&0xff;   //保存总故障次数
			saveDA2();
            tmp=(char)gz&0xff00>>8;
            saveDA2();
            tmp=(char)gz&0xff0000>>16;
            saveDA2();
            tmp=(char)gz&0xff000000>>24;
            saveDA2();

	        tmp=FSDZJ_271&0xff;    //保存429信号
            saveDA3();
            tmp=FSDZJ_271>>8;
            saveDA3();
	        tmp=FSGZJ_271&0xff;    
            saveDA3();
            tmp=FSGZJ_271>>8;
            saveDA3();

            tmp=FSDZJ_272&0xff;    
            saveDA3();
            tmp=FSDZJ_272>>8;
            saveDA3();
	        tmp=FSGZJ_272&0xff;    
            saveDA3();
            tmp=FSGZJ_272>>8;
            saveDA3();

	        tmp=FSDZJ_273&0xff;    
            saveDA3();
            tmp=FSDZJ_273>>8;
            saveDA3();
	        tmp=FSGZJ_273&0xff;    
            saveDA3();
            tmp=FSGZJ_273>>8;
            saveDA3();
		  }

	      if(count2>=5)    
	      { count2=0;
	        GpioDataRegs.GPFDAT.bit.GPIOF11=~GpioDataRegs.GPFDAT.bit.GPIOF11;
			if(flag15==1)
			{ //////查询轮载信号次数/////////
		      if(flag16==1)
              { flag16=0;
			    lz=0;
			  }
			  else
			  {
			    error1++;
			    gz++;
			    lz++;	 
			    tmp=0x34;	 //保存无轮载信号代码
				saveDA3();
			    tmp=0x37;
			    saveDA3();
				tmp=0x32;
				saveDA3();
				tmp=0x34;
				saveDA3();
				tmp=0x45;
				saveDA3();
				tmp=0x54;
				saveDA3();
			  }
			  if(error1>=20)  //累计20次未收到轮载信息,则飞机在空中
			  { flag6=1;
			    flag6a=1;	        
			  }
			  if(lz>=5)  //连续5次未收到轮载信息,则飞机在空中
			  { flag6=1;
			    flag6a=1;
			    lz=0;
			  }
		     
			  ////////////////////////////////////////////////
		      if(flag18==1)
			  { flag18=0;
			    wd1=0;
			  }
			  else
			  {			  
			    gz++;
				error2++;
			    wd++;				 
			    tmp=0x33;	  //保存无213信号代码
				saveDA3();
			    tmp=0x31;
			    saveDA3();
				tmp=0x32;
				saveDA3();
				tmp=0x34;
				saveDA3();
				tmp=0x45;
				saveDA3();
				tmp=0x54;
				saveDA3();        
			  }
			  if(error2>=20)  //累计20次未收到静温信息,则飞机在严酷环境下
			  { t2=380;	    
			  }
			  if(wd1>=5)  //连续5次未收到静温信息,则飞机在严酷环境下
			  { t2=380;
			    wd1=0;
			  } 
	        }	        	   
	      }
		  		  
		  if(((GpioDataRegs.GPBDAT.bit.GPIOB15==1)&&(flag6==1))&&(flag20==0))                   
          { 
            if((j==1)&&(flag1==0))
		    { FSDZJ_272=FSDZJ_272|0x4000;       //左主控
	          jiare();
		    }
		    if((j==2)&&(flag1==1))
		    { FSDZJ_272=FSDZJ_272&0xbfff;       //右主控
	          jiare();
		    }			
          }
          else
		  { GpioDataRegs.GPADAT.bit.GPIOA9=1;
		    flag22=0;
	        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff; //关闭所有加热
	        FSGZJ_271=FSGZJ_271&0xdfff;
			FSDZJ_272=FSDZJ_272&0xdfff;
		  }          
		}
        else
        { GpioDataRegs.GPADAT.bit.GPIOA8=~GpioDataRegs.GPADAT.bit.GPIOA8;
        }
	  }	      
   }
   else
   { GpioDataRegs.GPADAT.bit.GPIOA8=~GpioDataRegs.GPADAT.bit.GPIOA8;
   }
}





