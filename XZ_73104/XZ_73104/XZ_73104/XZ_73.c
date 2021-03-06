/* 
   * 标题：旋翼防除冰控制器软件
   * 文件名：XZ_73.pit
   * 时间：2013.6.28
   * 版本：第V1.04版
   * 版权：181厂 
   * 作者：郑华  
*/

#include "DSP28_Device.h"

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart; 

#pragma CODE_SECTION(InitFlash, "ramfuncs");

////////////////变量申明/////////////////////////////////     
Uchar Count_sample;            //采集数据个数
Uchar Sample_Result;           //采样值 
Uchar Count_MBIT_Receive;      //连续接收维护BIT命令次数
Uchar Count_WDI;               //喂狗时间变量
Uchar Count_SCI_Send;          //SCI发送时间变量
Uint16 Count_MRB_Heat;         //主桨加热时间变量
Uchar Count_TRB_Heat;          //尾桨加热时间变量
Uchar Time_Test;               //断路查询时间间隔   
Uchar Count_Pulse;             //加热区个数
Uchar Count_MRB_Test;		   //主桨监测时间变量
Uchar Count_TRB_Test;		   //尾桨监测时间变量
Uchar Count_Light;             //结果灯闪烁变量
Uchar Count_E422_Check;        //查询422数据时间变量
Uchar Count_E422_Error;        //连续未收到422数据次数
Uint16 TRBSensor_Temperature;  //尾桨温度传感器温度
Uint16 Current_MRB[3];         //主桨加热三相电流
Uint16 Current_TRB[3];         //尾桨加热三相电流
Uchar Count_Open_MRB[3];       //主桨加热三相断路次数 
Uchar Count_Open_TRB[3];       //尾桨加热三相断路次数 
Uint16 ONT_Real;               //主桨每个区实际加热时间
Uint16 ONT_Vary;               //主桨每个区计算加热时间 
Uint16 OFT_Real;               //主桨每个区实际停止时间
Uint16 OFT_Vary;               //主桨每个区计算停止时间
Uchar LWC;                     //接收的液态水含量数据
Uchar Ice_Degree;              //结冰强度数据
Uchar Number1_Error;           //故障字1
Uchar Number2_Error;           //故障字2
Uchar SJB[9];                  //接收到的SJB5的数据
Uchar m;                       //每次接到数据个数
Uchar Flag_PowerOnBIT_Finish;  //上电自检完成标志1完成  
Uchar Flag_MBIT_Start;         //维护BIT开始标志=1            
Uchar Flag_MBIT_Finish;        //维护BIT自检完成标志
Uchar Flag_MBIT_MRB;          //维护BIT中主桨加热标志
Uchar Flag_MBIT_TRB;          //维护BIT中尾桨加热标志
Uchar Flag_Switch_Check;      //接触器检查完成标志1完成 
Uchar Flag_Controller_OK;      //控制器正常标志
Uchar Flag_MRB_OK;             //主桨正常标志
Uchar Flag_TRB_OK;             //尾桨正常标志
Uchar Flag_TRBSensor1_OK;     //尾桨温度传感器1正常
Uchar Flag_TRBSensor2_OK;     //尾桨温度传感器2正常
Uchar Flag_MRB_Heat;          //主桨正在加热标志
Uchar Flag_E422_Receive;      //接收到422数据标志
Uchar Flag_MRB_Check;         //地面维护BIT中主桨检查完标志
Uchar Flag_TRB_Check;         //地面维护BIT中尾桨检查完标志
Uchar Flag_Heat_Start;        //主桨开始加热标志

Uchar gs=0;
Uchar fk[7]={0,0,0,0,0,0,0};    //反馈时序编码
Uchar ZZ[7]={1,1,1,0,0,1,0};    //反馈时序真值表
Uchar Flag_FK_Test=0;           //反馈时序检查标志
Uchar fk_error=0;               //反馈时序故障次数

/////////////////函数申明/////////////////////////////////
void InitFlash(void);
void Init_Variable(void);
void InitSysCtrl(void);
void InitGpio(void);
void Init_eva_timer1(void);
void Init_evb_timer4(void);
void InitSci(void);
void InitAdc(void);
void InitXINT1(void);
void InitXINT2(void); 
void CPU_Store_Check(void);
void CPU_Timer_Check(void);
void AirSensor_Check(void);
void TRBSensor_Check(void);
void MRB_QXJC(void);
void TRB_QXJC(void);
void FK_Check(void);
void Delay(Uint32 xms);
void ADC_sample(Uint16 channel);
void PowerOn_BIT(void); 
void Switch_Check(void);
void Detector_Check(void);
void MBIT(void); 
void Heat_Time_Count(void);
void MRB_Heat(void); 
void TRB_Heat(void);  
interrupt void eva_timer1_isr(void);
interrupt void evb_timer4_isr(void);
interrupt void SCIB_ISR(void);
interrupt void ad_isr(void);
interrupt void XINT1_isr(void);
interrupt void XINT2_isr(void); 


/////////////////Flash初始化///////////////////////////
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

/////////////////全局变量初始化/////////////////////////////////
void Init_Variable(void)
{
  Uchar i=0;
  Count_sample=0;            
  Sample_Result=0;          
  Count_MBIT_Receive=0;
  Count_WDI=0;      
  Count_SCI_Send=0;          
  Count_MRB_Heat=0;         
  Count_TRB_Heat=0;         
  Time_Test=80;               
  Count_Pulse=0;            
  Count_MRB_Test=0;		     
  Count_TRB_Test=0;		     
  Count_Light=0;            
  Count_E422_Check=0;        
  Count_E422_Error=0;        
  TRBSensor_Temperature=0;  
  for(i=0;i<3;i++)
  {
	Current_MRB[i]=0;
  }
  for(i=0;i<3;i++)
  {
	Current_TRB[i]=0;
  } 
  for(i=0;i<3;i++)
  {
	Count_Open_MRB[i]=0;
  }  
  for(i=0;i<3;i++)
  {
	Count_Open_TRB[i]=0;
  }
  ONT_Real=0;              
  ONT_Vary=0;             
  OFT_Real=0;            
  OFT_Vary=0;                
  LWC=0x0;                   
  Ice_Degree=0x0;            
  Number1_Error=0x0;        
  Number2_Error=0x0;                                       
  Flag_PowerOnBIT_Finish=0; 
  Flag_MBIT_Start=0;                 
  Flag_MBIT_Finish=0;        
  Flag_MBIT_MRB=0;         
  Flag_MBIT_TRB=0;          
  Flag_Switch_Check=0;     
  Flag_Controller_OK=0;        
  Flag_MRB_OK=0; 
  Flag_TRB_OK=0;           
  Flag_TRBSensor1_OK=0;     
  Flag_TRBSensor2_OK=0;    
  Flag_MRB_Heat=0;         
  Flag_E422_Receive=0;  
  for(m=0;m<9;m++)
  {
    SJB[m]=0;
  }  
  m=0;
  Flag_MRB_Check=0;
  Flag_TRB_Check=0;
  Flag_Heat_Start=0;
}

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
  SysCtrlRegs.PLLCR = 0x8;    //主频=30*8/2=120MHz  
  Delay(5000);                 //为了频率稳定延时一段时间      
  SysCtrlRegs.HISPCP.all = 0x0002;  //高速外设时钟=120/4=30MHz
  SysCtrlRegs.LOSPCP.all = 0x0002;	 //低速外设时钟=120/4=30MHz
 
  SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;   //使能EVA模块的高速外设时钟
  SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;   //使能EVB模块的高速外设时钟
  SysCtrlRegs.PCLKCR.bit.ADCENCLK=1;   //使能ADC模块的高速外设时钟
  SysCtrlRegs.PCLKCR.bit.SCIENCLKA=1;  //使能SCIA模块的高速外设时钟
  SysCtrlRegs.PCLKCR.bit.SCIENCLKB=1;  //使能SCIB模块的高速外设时钟				
  EDIS;	
}

///////////////////GPIO口初始化//////////////////////////////////
void InitGpio(void)
{
  EALLOW;
  GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOA设置为I/O口 
  GpioMuxRegs.GPADIR.all=0x0000;    //A0-A8设置为输入	

  GpioMuxRegs.GPBMUX.all=0x0000;   //GPIOB设置为I/O口 
  GpioMuxRegs.GPBDIR.all=0xffff;   //B0-B15设置为输出	
  GpioDataRegs.GPBDAT.all=0x1c4;       
  EDIS;
}	

///////////////////外部中断1初始化//////////////////////////////////
void InitXINT1(void)
{  
  GpioMuxRegs.GPEMUX.bit.XINT1_XBIO_GPIOE0=1; //设置中断1功能 
  XIntruptRegs.XINT1CR.all=0x5;      //上升沿生成中断
}

///////////////////外部中断2初始化//////////////////////////////////
void InitXINT2(void)
{  
  GpioMuxRegs.GPEMUX.bit.XINT2_ADCSOC_GPIOE1=1; //设置外部中断2功能 
  XIntruptRegs.XINT2CR.all=0x5;      //上升沿生成中断
}

///////////////////定时器1初始化（25ms)/////////////////////////////////
void Init_eva_timer1(void)
{  
  EvaRegs.GPTCONA.all = 0x0;   //初始化定时器1并设置定时器1寄存器
  EvaRegs.EVAIMRA.bit.T1PINT = 1;  //允许定时器1周期中断
  EvaRegs.EVAIFRA.bit.T1PINT = 1;  //清除定时器1周期中断标志
             
  EvaRegs.T1CON.all = 0x170C;  //Timer1为连续增计数模式，预分频值128
	                             //使用内部高速外设时钟
  EvaRegs.T1PR = 0x16e3;   //Timer1的期寄存器=0.025*30MHz/128
  EvaRegs.T1CNT = 0x0000;  //Timer1的计数清零
}

///////////////////定时器4初始化（25ms）//////////////////////////////////
void Init_evb_timer4(void)
{ 
  EvbRegs.GPTCONB.all=0x0;     //初始化定时器4并设置定时器4寄存器
  EvbRegs.EVBIMRB.bit.T4PINT = 1;  //允许定时4周期中断
  EvbRegs.EVBIFRB.bit.T4PINT = 1;  //清除定时器4周期曛�
             
  EvbRegs.T4CON.all = 0x170C;  //Timer4为连续增计数模式，预分频值128，
	                           //使用内部高速外设时钟
  EvbRegs.T4PR = 0x16e3;     //Timer4的周期寄存器=0.025*30MHz/128
  EvbRegs.T4CNT = 0x0000;  //Timer4的计数器清零
}

/////////////////串口初始化//////////////////////////////////////
void InitSci(void)
{	
  EALLOW;
  GpioMuxRegs.GPFMUX.all = 0x0030;     //设置SCIA功能
  GpioMuxRegs.GPGMUX.all = 0x0030;     //设置SCIB功能
  EDIS;

  //////////////SCIA初始化//////////////////////////////
  SciaRegs.SCIFFTX.all=0xe040;    //允许接收,使能FIFO
  SciaRegs.SCIFFRX.all=0x2021;    //使能FIFO接收
  SciaRegs.SCIFFCT.all=0x0000;    //禁止波特率校验
  SciaRegs.SCICCR.all = 0x07;     //1个停止位,奇校验,有效数据长度8位	
  SciaRegs.SCICTL1.all = 0x03;    //复位
  SciaRegs.SCICTL2.all = 0x03;    //复位  	
  SciaRegs.SCIHBAUD = 0x01;     //设置波特率9600bps
  SciaRegs.SCILBAUD = 0x85;	
  SciaRegs.SCICTL1.all = 0x23;   //使能发送接收功能
  
  //////////////SCIB初始化//////////////////////////////
  ScibRegs.SCIFFTX.all=0xe040;    //允许接收,使能FIFO
  ScibRegs.SCIFFRX.all=0x2021;    //使能FIFO接收
  ScibRegs.SCIFFCT.all=0x0000;    //禁止波特率校验
  ScibRegs.SCICCR.all = 0x07;     //1个停止位,奇校验,有效数据长度8位	
  ScibRegs.SCICTL1.all = 0x03;    //复位
  ScibRegs.SCICTL2.all = 0x03;    //复位  	
  ScibRegs.SCIHBAUD = 0x01;     //设置波特率9600bps
  ScibRegs.SCILBAUD = 0x85;	
  ScibRegs.SCICTL1.all = 0x23;   //使能发送接收功能  	
} 

///////////////////AD初始化//////////////////////////////////
void InitAdc(void)
{	
  AdcRegs.ADCTRL1.bit.RESET=1;  //复位ADC模块
  Delay(5000);	 
  AdcRegs.ADCTRL1.bit.RESET=0;
  AdcRegs.ADCTRL1.bit.SUSMOD=3;
  AdcRegs.ADCTRL1.bit.ACQ_PS=0;
  AdcRegs.ADCTRL1.bit.CPS=0;     
  AdcRegs.ADCTRL1.bit.CONT_RUN=0; //AD为启动/停止模式
  AdcRegs.ADCTRL1.bit.SEQ_CASC=1; //16级联模式

  AdcRegs.ADCTRL3.bit.ADCEXTREF = 0;    //使用内部参考电压     	
  AdcRegs.ADCTRL3.bit.ADCBGRFDN=3; //带间隙参考电路上电	
  Delay(5000); 	                  //等待稳定          
  AdcRegs.ADCTRL3.bit.ADCPWDN=1;  //其它电路上电
  Delay(5000);                    //等待稳定
  AdcRegs.ADCTRL3.bit.ADCCLKPS=15;  
  AdcRegs.ADCTRL3.bit.SMODE_SEL=0;  //顺序采样
	
  AdcRegs.MAX_CONV.bit.MAX_CONV=15;  //采样通道为16   
  AdcRegs.ADCTRL2.all=0x8c04;
  				
  AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;
  AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;				
} 

/////////////延时函数/////////////////////////////////
void Delay(Uint32 xms)
{
  Uint32 i=0;
  for(i=0;i<xms;i++)
  {
    asm("NOP");
  } 
}

//////////////信号采集///////////////////////////////
void ADC_sample(Uint16 channel)
{  
  AdcRegs.CHSELSEQ1.all= channel;   
  AdcRegs.CHSELSEQ2.all= channel;
  AdcRegs.CHSELSEQ3.all= channel;
  AdcRegs.CHSELSEQ4.all= channel; 

  EvbRegs.T4CON.bit.TENABLE=1;
  while(1)
  { 
    if(Count_sample>=15)
	{
	  Count_sample=0;
 	  break;
	}
  }   	 		
  EvbRegs.T4CON.bit.TENABLE=0;
}   

////////////CPU读写检查////////////////////////
void CPU_Store_Check(void)
{
  Uint16 i=0;
  Uint16 a[20]; 

  for(i=0;i<20;i++)
  { 
    a[i]=i;
  }
  Delay(0xfff);
  i=0;
  while(1)
  {
    if(a[i]!=i)
    { 
      Flag_Controller_OK=1;   //控制器故障
      Number1_Error=Number1_Error|0x40;   //上报控制器失效    
	  break;
    }
    else
    {
	  Flag_Controller_OK=0;   //控制器正常
      Number1_Error=Number1_Error&0xbf;   //上报控制器正常
	  i++;
	  if(i>19)
	  {
	    break;
	  }
    }
  }
}

////////////CPU定时器检查////////////////////////
void CPU_Timer_Check(void)
{
  EvaRegs.T1CON.bit.TENABLE=1;   //打开定时器1
  EvbRegs.T4CON.bit.TENABLE=1;   //打开定时器4
  Delay(0x9fff);
  EvaRegs.T1CON.bit.TENABLE=0;    //关闭定时器1
  EvbRegs.T4CON.bit.TENABLE=0;    //关闭定时器4
  
  if(abs(EvaRegs.T1CNT-EvbRegs.T4CNT)>=5)
  {
    Flag_Controller_OK=1;    //控制器故障
    Number1_Error=Number1_Error|0x40;   //上报控制器失效
  }
  else
  {
    Flag_Controller_OK=0;   //系统正常
    Number1_Error=Number1_Error&0xbf;   //上报控制器正常
	EvaRegs.T1CNT=0x0000;
	EvbRegs.T4CNT=0x0000;
  }

  /////////////////////////////////////////////////////
  if(Flag_Controller_OK==0)   //控制器正常
  {
    EvaRegs.T1CON.bit.TENABLE=1;   //打开定时器1
    /////////将定时器4功能改为启动AD转换功能///////////// 
    PieCtrl.PIEIER5.bit.INTx1=0;   //关闭定时器4中断
    EvbRegs.GPTCONB.all=0x400;    //定时器4周期中断启动AD转换
    EvbRegs.T4PR = 0xea;          //Timer4的周期寄存器=0.001*30MHz/128 
  }    
}

///////// 大气温度传感器检查///////////////////////

void AirSensor_Check(void)
{      
  ADC_sample(0x0000);   //ADCINA0采集大气温度 
  if(Flag_PowerOnBIT_Finish==0)    //上电BIT时	 
  { 
    if((Sample_Result<530)||(Sample_Result>630))
    { 
      Flag_Controller_OK=1;    //控制器故障
	  Number1_Error=Number1_Error|0x40;   //上报控制器失效
    }
	else
	{
	  Flag_Controller_OK=0;    //控制器正常
	  Number1_Error=Number1_Error&0xbf;   //上报控制器正常
	}
  }
  else 
  { 
	if((Sample_Result<100)||(Sample_Result>2900))
    { 
      Delay(0xffff);
      ADC_sample(0x0000);   //ADCINA0采集大气温度
	  if((Sample_Result<100)||(Sample_Result>2900))   //连续判断2次
	  {
	    Number1_Error=Number1_Error|0x24;   //上报大气温度传感器尾桨防冰失效
        GpioDataRegs.GPBDAT.bit.GPIOB10=0;  //温度不阻塞
	    Flag_TRB_OK=1;        //尾桨故障
	    if(Flag_MRB_Heat==0)
	    {
	      Flag_MRB_OK=1;        //主桨故障        
	    }
	    Number1_Error=Number1_Error|0x2;  //上报主桨除冰失效
	    GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //点亮主桨除冰失效灯 
        GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //点亮尾桨除冰失效灯		
	  }
    }
	else
	{
	  if(Sample_Result<=1271)       //大气温度>=20度
	  {
	    GpioDataRegs.GPBDAT.bit.GPIOB10=1;  //输出温度阻塞
	  }
	  else
	  { 
	    GpioDataRegs.GPBDAT.bit.GPIOB10=0;  //温度不阻塞
	  }
	}
  }
} 

//////////检查尾桨温度传感器检查//////////////////////
void TRBSensor_Check(void)
{   
  Uchar i=0; 
  
  if(Flag_PowerOnBIT_Finish==0)    //上电BIT时	    	 
  {
    while(i<2)
    {
	  ADC_sample(0x1111+i*0x1111);        
      if((Sample_Result<1175)||(Sample_Result>1275))
      { 
        Flag_Controller_OK=1;      //控制器故障
	    Number1_Error=Number1_Error|0x40;   //上报控制器失效
		break;
      }
      else
	  { 
	    Flag_Controller_OK=0;      //控制器正常
	    Number1_Error=Number1_Error&0xbf;   //上报控制器正常
		i++;
	  }
    }
  }
  else
  { 
    if(Flag_TRBSensor1_OK==0)    //尾桨温度传感器1正常
    {
	  i=0;			   //ADCINA1采集尾桨温度传感器1
	}
    else
	{
	  i=1;            //ADCINA2采集尾桨温度传感器2
	}
   
    ADC_sample(0x1111+i*0x1111);      
    TRBSensor_Temperature=Sample_Result;     	 
    if((TRBSensor_Temperature<100)||(TRBSensor_Temperature>2900))
    {       
      Delay(0xffff);
      ADC_sample(0x1111+i*0x1111);
      TRBSensor_Temperature=Sample_Result;     	 
      if((TRBSensor_Temperature<100)||(TRBSensor_Temperature>2900))        
      {
	    if(i==0)
	    {
	      Flag_TRBSensor1_OK=1;     //尾桨温度传感器1故障
	    }
	    else if(i==1)
	    {
	      Flag_TRBSensor2_OK=1;     //尾桨温度传感器2故障
	    }
        Number2_Error=Number2_Error|0x2;  //尾桨温度传感器故障
      }       
    }
    if((Flag_TRBSensor1_OK==1)&&(Flag_TRBSensor2_OK==1))
    { 
      Flag_TRB_OK=1;                    //尾桨故障
      Number2_Error=Number2_Error|0x1;  //尾桨温度传感器失效 
      Number1_Error=Number1_Error|0x4;  //尾桨防冰失效 
      GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //关闭尾桨加热
      GpioDataRegs.GPBDAT.bit.GPIOB0=0;   //关闭尾桨控制开关
      GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //点亮尾桨除冰失�       
	}   
  } 
}

/////////////主桨感应检查/////////////////////////////////////////
void MRB_QXJC(void)
{   
  Uchar i=0;
  
  if(Flag_PowerOnBIT_Finish==0)    //上电BIT时
  {
    while(i<3)
    {
      ADC_sample(0x3333+i*0x1111);    
      if((Sample_Result<2450)||(Sample_Result>2550))
	  {
	    Flag_Controller_OK=1;      //控制器故障
	    Number1_Error=Number1_Error|0x40;   //上报控制器失
		break;
	  }
	  else
	  {
	    Flag_Controller_OK=0;      //控制器正常
	    Number1_Error=Number1_Error&0xbf;   //上报控制器正常
		i++;
	  }    
    }
  }
  else
  {
    for(i=0;i<3;i++) 	
	{ 
	  ADC_sample(0x3333+i*0x1111); 
      Current_MRB[i]=Sample_Result;  	          
      if((Current_MRB[i]<2450)||(Current_MRB[i]>3000))	//主桨缺相电流
      { 
        Delay(0xffff);
        ADC_sample(0x3333+i*0x1111); 
        Current_MRB[i]=Sample_Result;        
        if((Current_MRB[i]<2450)||(Current_MRB[i]>3000))
        { 
          Flag_MRB_OK=1;   //主桨除冰故障
        }
      } 	
	}      
    if((abs(Current_MRB[0]-Current_MRB[1])>350)||(abs(Current_MRB[0]-Current_MRB[2])>350)
      ||(abs(Current_MRB[1]-Current_MRB[2])>350))
    { 
      Flag_MRB_OK=1;     //主桨除冰故障
    }
	//////////////////////////////////////////////	
	if(Flag_MRB_OK==1)  //主桨除冰故障
	{
	  Number1_Error=Number1_Error|0x10;   //上报主桨除冰故障
      GpioDataRegs.GPBDAT.bit.GPIOB11=0;  //关闭主桨加热
      GpioDataRegs.GPBDAT.bit.GPIOB2=1;   //关闭主桨控制开关
	  if(Flag_MBIT_Start==0)
      {
        GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //点亮主桨除冰失效灯
	  }
	}
  }
} 

/////////////尾桨感应检查/////////////////////////////////////////
void TRB_QXJC(void)
{   
  Uchar i=0;
  
  if(Flag_PowerOnBIT_Finish==0)    //上电BIT时
  {
    while(i<3)
    {
      ADC_sample(0x6666+i*0x1111);
      if((Sample_Result<1540)||(Sample_Result>1640))
	  {
	    Flag_Controller_OK=1;      //控制器故障
	    Number1_Error=Number1_Error|0x40;   //上报控制器失效
		break;
	  }
	  else
	  {
	    Flag_Controller_OK=0;      //控制器正常
	    Number1_Error=Number1_Error&0xbf;   //上报控制器正常
		i++;
	  }   
    }	
  }
  else
  {
    for(i=0;i<3;i++) 	
	{
	  ADC_sample(0x6666+i*0x1111); 
      Current_TRB[i]=Sample_Result; 	 
      if((Current_TRB[i]<1440)||(Current_TRB[i]>1980))
      { 
        Delay(0xffff);
	    ADC_sample(0x6666+i*0x1111); 
        Current_TRB[i]=Sample_Result;  	 
        if((Current_TRB[i]<1440)||(Current_TRB[i]>1980))         
        { 
          Flag_TRB_OK=1;                    //尾桨故障
        }
      }
	}      
    if((abs(Current_TRB[0]-Current_TRB[1])>360)||(abs(Current_TRB[0]-Current_TRB[2])>360)
      ||(abs(Current_TRB[1]-Current_TRB[2])>360))
    { 
      Flag_TRB_OK=1;                    //尾桨故障
    }
	//////////////////////////////////////////
	if(Flag_TRB_OK==1)  //尾桨防冰失效
	{
	  Number1_Error=Number1_Error|0x4;   //上报尾桨防冰失效
      GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //关闭尾桨加热
      GpioDataRegs.GPBDAT.bit.GPIOB0=0;   //关闭尾桨控制开关
	  if(Flag_MBIT_Start==0)
      {
        GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //点亮尾桨除冰失效灯
	  }
	} 
  }
} 

/////////////上电BIT//////////////////////////////////
void PowerOn_BIT(void)
{  
  CPU_Timer_Check();
  if(Flag_Controller_OK==1)
  {
    CPU_Timer_Check();
  }  
  ///////////////////////////////////////
  if(Flag_Controller_OK==0)
  {
    CPU_Store_Check();
    if(Flag_Controller_OK==1)
    {
      CPU_Store_Check();
    }
  }
  /////////检查自动控制电路是否正常//////////////////////////
  if(Flag_Controller_OK==0)
  { 
    GpioDataRegs.GPBDAT.bit.GPIOB8=0;    //输出"温度输入自检"信号
    Delay(0xffff);
    AirSensor_Check(); //判断大气温度处理电路是否正常
    if(Flag_Controller_OK==1)
	{
	  AirSensor_Check();
	}

    TRBSensor_Check(); //判断尾桨温度传感器处理电路是否正常
    if(Flag_Controller_OK==1)
	{
	  TRBSensor_Check();
	}

    //////检查主桨三相处理电路//////////////////
	GpioDataRegs.GPBDAT.bit.GPIOB6=0;    //启动"主桨感应自检"信号
    Delay(0x3ffff);
    MRB_QXJC();
    if(Flag_Controller_OK==1)
	{
	  MRB_QXJC();
	}
    //////检查尾桨三相处理电路//////////////////
	GpioDataRegs.GPBDAT.bit.GPIOB7=0;    //启动"尾桨感应自检"信号
    Delay(0x3ffff);
    TRB_QXJC();
    if(Flag_Controller_OK==1)
	{
	  TRB_QXJC();
	}

    GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all|0x1e0;  //关闭三个自检命令
  }

  /////////检查手动控制电路是否正常//////////////////////////
  if(Flag_Controller_OK==0)
  { 
    GpioDataRegs.GPBDAT.bit.GPIOB5=1;    //启动"手动除冰自检"信号
    Delay(0xffff); 
    if(GpioDataRegs.GPADAT.bit.GPIOA8==0)
    {       
      Delay(0xffff);
      if(GpioDataRegs.GPADAT.bit.GPIOA8==0)
      {
	    Flag_Controller_OK=1;
	    Number1_Error=Number1_Error|0x40;   //上报控制器失效
      } 
	}
    GpioDataRegs.GPBDAT.bit.GPIOB5=0;
  }
  ////////////////////////////////////////////////
  if(Flag_Controller_OK==0)     //上电BIT正常
  {
    GpioDataRegs.GPBDAT.bit.GPIOB9=1;  //点亮控制器指示灯
	Flag_PowerOnBIT_Finish=1;   //上电BIT完成 
  }        
}  

//////////接触器检查////////////////////////////
void Switch_Check(void)
{    
  ////////////断开主尾桨所有加热/////////////////////////
  GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //关闭尾桨加热
  GpioDataRegs.GPBDAT.bit.GPIOB0=0;  //关闭尾桨控制开关
  GpioDataRegs.GPBDAT.bit.GPIOB2=1;  //关闭主桨加热
  GpioDataRegs.GPBDAT.bit.GPIOB11=0;  //关闭主桨控制开关 
  Flag_MRB_OK=0;    //自动除冰出现故障时仍可进入维护BIT
  Flag_TRB_OK=0;
  Count_MRB_Heat=0;
  Count_TRB_Heat=0;
  Flag_MRB_Heat=0;
  Flag_Heat_Start=0;
  Count_MRB_Test=0;
  Count_TRB_Test=0;
  GpioDataRegs.GPBDAT.bit.GPIOB14=0;
  GpioDataRegs.GPBDAT.bit.GPIOB15=0;
  Count_Pulse=0;
  /////////////////检查主桨接触器断开状态是否正常//////////////////
  Delay(0x3ffff);
  if(GpioDataRegs.GPADAT.bit.GPIOA0==0)	//主桨接触器自检信号0故障
  {
    Delay(0xffff);
    if(GpioDataRegs.GPADAT.bit.GPIOA0==0)
	{
	  Flag_MRB_OK=1;      //主桨故障
	}
  }
  /////////////////检查尾桨接触器断开状态是否正常//////////////////
  Delay(0x3ffff); 
  if(GpioDataRegs.GPADAT.bit.GPIOA1==0)	//尾桨接触器自检信号0故障
  {
    Delay(0xffff);
    if(GpioDataRegs.GPADAT.bit.GPIOA1==0)
	{
	  Flag_TRB_OK=1;     //尾桨故障
	}
  }
}

/////////结冰探测子系统检查//////////////////////
void Detector_Check(void)
{  
  if(GpioDataRegs.GPADAT.bit.GPIOA6==0)   
  { 
    Delay(0xffff);
	if(GpioDataRegs.GPADAT.bit.GPIOA6==0)
    {
      Number2_Error=Number2_Error|0x4;  //上报结冰探测失效	  
      if(Flag_MRB_Heat==0)
	  {
	    Flag_MRB_OK=1;      //主桨故障
		PieCtrl.PIEIER9.bit.INTx3=0;     //不接收LWC
	  } 
    }
  }
}

/////////////地面维护BIT//////////////////////////////////
void MBIT(void)
{     
  if(Flag_MBIT_Finish==0)
  {
    if(Flag_Switch_Check==0)
    {
      Switch_Check();
      Flag_Switch_Check=1;    //接触器检查完成
    }
    /////////////主桨接触器检查正常后进行加热///////////////////  
    if(Flag_MRB_OK==0)
    {
	  if((Flag_MBIT_MRB==0)&&(Flag_MRB_Check==0))
      { 
        ONT_Real=34;     //维护BIT加热时间为0.85s    
        Time_Test=16;   //0.4s完成一次监控            
	    Flag_MBIT_MRB=1;   //准备进行维护BIT主桨加热
      }
	  /////////先检查主桨加热是否正常////////////////////
	  if(Flag_MBIT_MRB==1)
	  {
	    MRB_Heat();
        if(Flag_MBIT_MRB==0)
		{
          Flag_MRB_Check=1;
		}
	  }	
	}
	else
	{
	  Flag_MBIT_MRB=0;
      Flag_MRB_Check=1;
	}    
    ///////////////再检查尾桨加热////////
    if(Flag_TRB_OK==0)
    {
	  if((Flag_MBIT_TRB==0)&&(Flag_MRB_Check==1)&&(Flag_TRB_Check==0))
	  {
        Flag_MBIT_TRB=1;  //准备进行维护BIT尾桨加热
	  }            	  
	  if(Flag_MBIT_TRB==1)
	  {
	    TRB_Heat();
		if(Flag_MBIT_TRB==0)
		{
          Flag_TRB_Check=1;
          Flag_MBIT_Finish=1;  //检测完成准备上报结果
		}
	  }
    }
    if((Flag_TRB_OK==1)&&(Flag_MRB_Check==1))
	{
      Flag_MBIT_TRB=0;
      Flag_TRB_Check=1;
      Flag_MBIT_Finish=1;  //检测完成准备上报结�
	} 
  }

  if((Flag_MBIT_Finish==1)&&(GpioDataRegs.GPADAT.bit.GPIOA3==0))    
  {
    if(Flag_MRB_OK==0)
	{
	  GpioDataRegs.GPBDAT.bit.GPIOB3=1;
	} 
    if(Flag_TRB_OK==0)
    {
	  GpioDataRegs.GPBDAT.bit.GPIOB4=1;
	}
    ///////////////////////////////////////////////
    if((Flag_MRB_OK==1)||(Flag_TRB_OK==1))
	{
      if(Count_Light>=5)
	  {
        Count_Light=0;
		if((Flag_MRB_OK==1)&&(Flag_TRB_OK==0))
        {
          GpioDataRegs.GPBDAT.bit.GPIOB3=~GpioDataRegs.GPBDAT.bit.GPIOB3;
		}
		if((Flag_MRB_OK==0)&&(Flag_TRB_OK==1))
        {
          GpioDataRegs.GPBDAT.bit.GPIOB4=~GpioDataRegs.GPBDAT.bit.GPIOB4;
		}
		if((Flag_MRB_OK==1)&&(Flag_TRB_OK==1))
		{
		  GpioDataRegs.GPBDAT.bit.GPIOB3=~GpioDataRegs.GPBDAT.bit.GPIOB3;
		  GpioDataRegs.GPBDAT.bit.GPIOB4=~GpioDataRegs.GPBDAT.bit.GPIOB4;  
		} 
	  }
	} 
  }
  if(GpioDataRegs.GPADAT.bit.GPIOA3==1)
  {
    GpioDataRegs.GPBDAT.bit.GPIOB3=0;      //灭灯
    GpioDataRegs.GPBDAT.bit.GPIOB4=0;
    Flag_Switch_Check=0;
    Flag_MBIT_Finish=0;
    Flag_MBIT_Start=0;   //准备下一次维护BIT
	Time_Test=80;
	Flag_MRB_Check=0;
    Flag_TRB_Check=0;
  }
}

///////////////主桨加热控制////////////////////////////////////
void MRB_Heat(void)
{  
  if(GpioDataRegs.GPADAT.bit.GPIOA7==0)  //查询系统电源是否启动
  { 
    if(GpioDataRegs.GPBDAT.bit.GPIOB2==1)   
    {       
      GpioDataRegs.GPBDAT.bit.GPIOB2=0;    //接通主桨控制开关 	   
	  Delay(0x8fffff);
      if(Flag_MBIT_MRB==1)    //维护BIT时检查主桨接触器接通状态是否正常
      {
        if(GpioDataRegs.GPADAT.bit.GPIOA0==1)	//主桨接触器自检信号1故障
        {
          Delay(0xffff);
          if(GpioDataRegs.GPADAT.bit.GPIOA0==1)
	      {
	        Flag_MRB_OK=1;      //主桨故障
	      }
		}
      }	  
	}
	if((GpioDataRegs.GPBDAT.bit.GPIOB2==0)&&(Count_MRB_Heat<=0))
	{
	  Count_Pulse=1;
	  Flag_Heat_Start=1;
	}
    /////////////////////加热时序///////////////////// 
    if(((Count_Pulse-1)*(ONT_Real+6)<Count_MRB_Heat)
       &&(Count_MRB_Heat<=((Count_Pulse-1)*(ONT_Real+6)+6)))
	{ 
	  GpioDataRegs.GPBDAT.bit.GPIOB11=1;
	  Count_MRB_Test=0;
	  Flag_FK_Test=0;
	}
    if(((Count_Pulse-1)*(ONT_Real+6)+6<Count_MRB_Heat)
       &&(Count_MRB_Heat<=Count_Pulse*(ONT_Real+6)))
	{ 
	  GpioDataRegs.GPBDAT.bit.GPIOB11=0;
	  if(Count_MRB_Heat==Count_Pulse*(ONT_Real+6))
	  {
	    Count_Pulse++;		
	    if(Count_Pulse>7)
	    { 
	      Count_Pulse=0;
	    }
	  }	   
    }
    if((7*ONT_Real+42<Count_MRB_Heat)&&(Count_MRB_Heat<=7*ONT_Real+48))
	{ 
	  GpioDataRegs.GPBDAT.bit.GPIOB11=1;
      OFT_Real=OFT_Vary;    //将计算的断开时间赋值
	  Count_MRB_Test=0;
	  Flag_FK_Test=0;
	}
	if(Flag_MBIT_MRB==1)    //维护BIT主桨加热时
	{
	  if(Count_MRB_Heat>(7*ONT_Real+48))
	  { 
	    GpioDataRegs.GPBDAT.bit.GPIOB11=0;   //关闭主桨加热
        GpioDataRegs.GPBDAT.bit.GPIOB2=1;    //断开主桨电源 
        Flag_Heat_Start=0; 
	    Count_MRB_Heat=0;
	    Flag_MBIT_MRB=0;           //维护BIT主桨加热结束
	    Count_MRB_Test=0;
	    Flag_FK_Test=0;      
	  }
	}
	else    //自动除冰
	{	  
	  if((ONT_Real+OFT_Real)<(7*ONT_Real+48))   //加热周期T<(7*Ton+8*0.15)s
	  {
	    if(((7*ONT_Real+48)<Count_MRB_Heat)&&(Count_MRB_Heat<=(7*ONT_Real+54)))
	    { 
	      GpioDataRegs.GPBDAT.bit.GPIOB11=0;
	    } 
	    if(Count_MRB_Heat>(7*ONT_Real+54))
	    { 
	      Count_MRB_Heat=0;
		  Flag_Heat_Start=0;
	      Flag_MRB_Heat=0;           //一个加热周期结束
		  Count_MRB_Test=0;
		  Flag_FK_Test=0;
	    }  
	  }
      else
      {
        if(((7*ONT_Real+48)<Count_MRB_Heat)&&(Count_MRB_Heat<=(ONT_Real+OFT_Real)))
	    { 
	      GpioDataRegs.GPBDAT.bit.GPIOB11=0;
	    } 
	    if(Count_MRB_Heat>(ONT_Real+OFT_Real))
	    { 
	      Count_MRB_Heat=0;
		  Flag_Heat_Start=0;
	      Flag_MRB_Heat=0;           //一个加热周期结束
		  Count_MRB_Test=0; 
		  Flag_FK_Test=0;
	    }
      }
	}
  }
  else
  { 
    GpioDataRegs.GPBDAT.bit.GPIOB11=0;   //断开加热时序
    GpioDataRegs.GPBDAT.bit.GPIOB2=1;    //断开主桨电源
    Flag_Heat_Start=0; 
	Flag_Heat_Start=0;
    Count_MRB_Heat=0;
    Count_MRB_Test=0;
    Flag_FK_Test=0; 
  }   
 	
  //////////////缺相及反馈时序监控/////////////////////////////////////////////  	
  if(((GpioDataRegs.GPBDAT.bit.GPIOB11==0)&&(Count_MRB_Test>=Time_Test))
       &&(Count_Pulse>0))             //每1s查询一次
  {  
    Count_MRB_Test=0;
	MRB_QXJC();		
    if(Flag_FK_Test==0)
	{
	  FK_Check();
	  if(fk_error>0)
	  {
	    FK_Check();
		if(fk_error<2)
		{
		  fk_error=0;
		}
	  }
	}
  }
} 

///////////////反馈时序检查//////////////////////////////////
void FK_Check(void)
{
  if(GpioDataRegs.GPADAT.bit.GPIOA4==1)   //加热时序反馈检查
  {
    fk[Count_Pulse-1]=1;		
  }
  else
  {
    fk[Count_Pulse-1]=0; 
  }     
  ///////////////////////////////////////
  for(gs=0;gs<Count_Pulse;gs++)
  {
    if(fk[gs]!=ZZ[gs])
    {
      fk_error++;
      if(fk_error>=2)
      {		
        Flag_MRB_OK=1;                      
        GpioDataRegs.GPBDAT.bit.GPIOB11=0;   //断开加热时序
        GpioDataRegs.GPBDAT.bit.GPIOB2=1;    //断开主桨电源		
        Number1_Error=Number1_Error|0xa;  //上报主桨同步失效,主桨故障
        if(Flag_MBIT_Start==0)
        {
         GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //点亮主桨除冰失效灯
        }
		Flag_FK_Test=1;			  //本区反馈时序检查完成
      }
	}
  }

  if(fk_error==0)
  {
    Flag_FK_Test=1;          //本区反馈时序检查完成
  }
}

///////////////尾桨加热控制////////////////////////////////////
void TRB_Heat(void)
{  
  if(GpioDataRegs.GPADAT.bit.GPIOA7==0)  //查询加热电源是否接通
  { 
    if(Flag_MBIT_TRB==1)     //维护BIT时检查尾桨接触器接通状态是否正常
    { 
      GpioDataRegs.GPBDAT.bit.GPIOB0=1;    //接通尾桨控制开关
      Delay(0x3ffff);
      if(GpioDataRegs.GPADAT.bit.GPIOA1==1)	//尾桨接触器自检信号1故障
      {
        Delay(0xffff);
        if(GpioDataRegs.GPADAT.bit.GPIOA1==1)
	    {
	      Flag_TRB_OK=1;     //尾桨故障
	    }
      } 
      //////////////////////////////////////////////////      
      if((Count_TRB_Heat>0)&&(Count_TRB_Heat<=34))
	  { 	    	    
	    GpioDataRegs.GPBDAT.bit.GPIOB1=1;   //尾桨加热
	  }
	  if(Count_TRB_Heat>34)
	  { 
	    Count_TRB_Heat=0;
	    GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //尾桨停止加热	
	    GpioDataRegs.GPBDAT.bit.GPIOB0=0;    //断开尾桨电源 
	    Flag_MBIT_TRB=0;          //地面维护尾桨加热完毕     
	  }
    }
	else
	{ 
	  if(Flag_TRBSensor1_OK==0)
	  { 	    	    
	    if(TRBSensor_Temperature>1060)
	    { 
	      GpioDataRegs.GPBDAT.bit.GPIOB1=1;   //尾桨加热
	      GpioDataRegs.GPBDAT.bit.GPIOB0=1;    //接通尾桨电源	      
		}
        if(TRBSensor_Temperature<740)
		{ 
		  GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //尾桨停止加热		  	 
		}
	  }
      else
	  { 
	    if(Flag_TRBSensor2_OK==0)
	    { 
	      if(TRBSensor_Temperature>1060)
	      { 
	        GpioDataRegs.GPBDAT.bit.GPIOB1=1;   //尾桨加热
	        GpioDataRegs.GPBDAT.bit.GPIOB0=1;    //接通尾桨电源
		  }
          if(TRBSensor_Temperature<740)
		  { 
		    GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //尾桨停止加热  
		  }
		}
        else
	    { 
	      GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //尾桨停止加热
		  GpioDataRegs.GPBDAT.bit.GPIOB0=0;    //断开尾桨电源        
	    }
	  }
    }
    //////////////缺相监控/////////////////////////////////////////////  	
    if((GpioDataRegs.GPBDAT.bit.GPIOB1==1)&&(Count_TRB_Test>=Time_Test))
    { 
      Count_TRB_Test=0;
	  TRB_QXJC();	
    }
	if(GpioDataRegs.GPBDAT.bit.GPIOB1==0)
	{
	  Count_TRB_Test=0;
	}
  }
  else
  {
    Count_TRB_Heat=0;
	Count_TRB_Test=0;
	GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //尾桨停止加热
    GpioDataRegs.GPBDAT.bit.GPIOB0=0;    //断开尾桨电源
  }
}

/////////////////计算加热时间////////////////////////////////////////
void Heat_Time_Count(void)
{           
  float Air_Temperature1=0.0;     //大气静温
  int Air_Temperature2=0;
  float Ontime1=0.0;
  float Ontime2=0.0;
 
  ADC_sample(0x0000); 	  //ADCINA0采集大气温度
  Air_Temperature1=74.46-0.0446*Sample_Result;    	   
  Air_Temperature2=(int)Air_Temperature1;

  
  if((Air_Temperature1<-20.0)||(Air_Temperature2==-20))
  { 
    Ontime1=16.0*40;
  }
  if(((-20.0<Air_Temperature1)&&(Air_Temperature1<0.0))||(Air_Temperature2==0))
  { 
    Ontime1=(4-12*Air_Temperature1/20)*40;
  }
  if((0.0<Air_Temperature1)&&(Air_Temperature1<20.0))
  { 
    Ontime1=4.0*40;
  }
  if((Air_Temperature1>20.0)||(Air_Temperature2==20))
  {
    GpioDataRegs.GPBDAT.bit.GPIOB10=1;  //输出温度阻塞
  }

  Ontime2=(Uint16)Ontime1+0.5;  
  if(Ontime2>Ontime1)
  { 
    Ontime2=Ontime2-0.5;
  }
  else
  { 
    Ontime2=Ontime2+0.5;
  }
  ONT_Vary=(Uint16)Ontime2;
} 
 
///////////////////定时器1中断//////////////////////////////////

interrupt void eva_timer1_isr(void)
{  
  DINT;
  if(PieCtrl.PIEACK.bit.ACK2==1)
  { 
    PieCtrl.PIEACK.bit.ACK2=1; 
    Count_WDI++;   
    Count_SCI_Send++;      //喂狗及串口发送时浔淞�
	Count_E422_Check++;
	/////////////////////////////////////////////////
	if(Count_WDI>=20)
	{
	  GpioDataRegs.GPBDAT.bit.GPIOB12=~GpioDataRegs.GPBDAT.bit.GPIOB12;  //喂狗         
	}
    ///////////////////////////////////////////////
    if(Flag_PowerOnBIT_Finish==0)    //上电BIT时
	{ 
	  if(Flag_Controller_OK==1)
	  {
	    Count_Light++;
	  }
	}
    else
    {              	    
	  if((GpioDataRegs.GPBDAT.bit.GPIOB2==1)&&(GpioDataRegs.GPBDAT.bit.GPIOB0==0))   
	  {
	    if(GpioDataRegs.GPADAT.bit.GPIOA3==0) //接收到维护自检命令
	    { 
	      Count_MBIT_Receive++;
	      if(Count_MBIT_Receive>=2)
          {	
		    Count_MBIT_Receive=0; 
	        Flag_MBIT_Start=1;         //准备启动维护BIT
	      }
	    }
	    else
	    {
	      Count_MBIT_Receive=0;
	    }
	  }	  
	  /////////////////////////////////////////////////////////////	  
	  if(Flag_MBIT_Start==1)    //地面维护BIT时
	  {
	    if((Flag_MBIT_MRB==1)&&(Flag_Heat_Start==1))    //主桨
	    {
	      Count_MRB_Heat++;	    
	    }
	    if((GpioDataRegs.GPBDAT.bit.GPIOB0==1)&&(Flag_MBIT_TRB==1))  //尾桨
	    {
	      Count_TRB_Heat++;
	    }
	    if(Flag_MBIT_Finish==1)   //检测完成
	    {
	      if((Flag_TRB_OK==1)||(Flag_MRB_OK==1))
	      {
	        Count_Light++;
	      }
		}
	  }        
      else  //巡检时
	  {
        if(Flag_Heat_Start==1)    //主桨
	    {
	      Count_MRB_Heat++;	    
	    }
	  }
      ////////////////////////////////////////////////////
      if(GpioDataRegs.GPBDAT.bit.GPIOB2==0)    //主桨
	  { 
	    if((Flag_MRB_Heat==0)&&(Flag_MBIT_MRB==0))   //一个周期结束时
		{
		  Count_Pulse=1;
		}
	    if((GpioDataRegs.GPBDAT.bit.GPIOB11==0)&&(Count_Pulse!=0))
		{ 
		  Count_MRB_Test++;
		}
	  }
	  else
	  {
	    Count_Pulse=0;
	  } 
	  if(GpioDataRegs.GPBDAT.bit.GPIOB1==1)  //尾桨
      { 
		Count_TRB_Test++;
	  } 
    }   
    EvaRegs.EVAIMRA.bit.T1PINT = 1;
    EvaRegs.EVAIFRA.bit.T1PINT = 1;
  }
  EINT;
} 


//////////////接收LWC/////////////////////////////////
interrupt void SCIB_ISR(void)
{   
  float Offtime1=0.0;
  float Offtime2=0.0;
  Uint16 GYH=0x0;

  DINT;
  if(PieCtrl.PIEACK.bit.ACK9==1)
  { 
    PieCtrl.PIEACK.bit.ACK9=1;   	
    SJB[m]=(Uchar)ScibRegs.SCIRXBUF.all;	
	if(SJB[0]==0xaa)
	{
	  m++;
      if(m>8)
      {                     
        m=0;
        if((SJB[1]==0x55)&&(SJB[2]==0x00)&&(SJB[3]==0x09)
	      &&(SJB[4]==0x02)&&(SJB[6]==0x0)&&(SJB[7]==0x0))
	    {		  
          GYH=SJB[0]+SJB[1]+SJB[2]+SJB[3]+SJB[4]+SJB[5];
          GYH=GYH&0xff;
		  if(SJB[8]==(Uchar)GYH)
		  { 
		    Flag_E422_Receive=1;  //收到数据
            Count_E422_Error=0; 
            LWC=SJB[5];  
            if(LWC>0xc8)
            {
              LWC=0xc8;
            }         
            if(LWC>0)
		    {
		      /////////计算断开时间////////////////////                     		  
              Offtime1=6000/LWC;
              Offtime2=(Uint16)Offtime1+0.5;  
              if(Offtime2>Offtime1)
              { 
                Offtime2=Offtime2-0.5;
              }
              else
              { 
                Offtime2=Offtime2+0.5;
              }
              OFT_Vary=(Uint16)(Offtime2*40);
              /////////计算结冰强度////////////////////
              if((LWC>=0x01)&&(LWC<=0x14))
		      {
		        Ice_Degree=0x0;    //轻微结冰
		      }
              if((LWC>=0x15)&&(LWC<=0x32))
		      {
		        Ice_Degree=0x1;    //轻度结冰
		      }
              if((LWC>=0x33)&&(LWC<=0x64))
		      {
		        Ice_Degree=0x2;    //中度结冰
		      }
              if((LWC>=0x65)&&(LWC<=0xc8))
		      {
		        Ice_Degree=0x3;    //重度结冰
		      }
			}
		  }		  		  		           		  
		}
	  }
    }
    ScibRegs.SCIFFRX.bit.RXFFINTCLR=1;
    ScibRegs.SCIFFRX.bit.RXFFOVF=1;
  }
  EINT;
} 


///////////////////ADC中断//////////////////////////////////
interrupt void ad_isr(void)
{ 
  Uint16 Result[16];          //保存AD数据
  Uint16 i=0;
  Uint16 max=0;
  DINT;
  if(PieCtrl.PIEACK.bit.ACK1==1)
  { PieCtrl.PIEACK.bit.ACK1=1;
    
    Result[0]=AdcRegs.RESULT0>>4; 
    Result[1]=AdcRegs.RESULT1>>4;
    Result[2]=AdcRegs.RESULT2>>4;
    Result[3]=AdcRegs.RESULT3>>4;
    Result[4]=AdcRegs.RESULT4>>4;
    Result[5]=AdcRegs.RESULT5>>4;
    Result[6]=AdcRegs.RESULT6>>4;
    Result[7]=AdcRegs.RESULT7>>4;
    Result[8]=AdcRegs.RESULT8>>4;
    Result[9]=AdcRegs.RESULT9>>4;
    Result[10]=AdcRegs.RESULT10>>4;
    Result[11]=AdcRegs.RESULT11>>4;
    Result[12]=AdcRegs.RESULT12>>4;
    Result[13]=AdcRegs.RESULT13>>4;
    Result[14]=AdcRegs.RESULT14>>4;
    Result[15]=AdcRegs.RESULT15>>4;
 		
	///////////16个数据由小到大排列//////////////////////
	for(Count_sample=0;Count_sample<16;Count_sample++)
	{ for(i=0;i<16-Count_sample;i++)                      
	  { if(Result[i]>Result[i+1])
	    { max=Result[i];
		  Result[i]=Result[i+1];
          Result[i+1]=max;
		}
	  }
	}    	
	//////////去头去尾取中间14个有效数据求平均////////////	
	Sample_Result=0;
	for(Count_sample=1;Count_sample<15;Count_sample++)  
	{ Sample_Result=Sample_Result+Result[Count_sample];
	}
    Sample_Result=Sample_Result/14;
	Sample_Result=(Uint16)(Sample_Result*2.94*1000/4095);

    AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;
    AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;
  }	
  EINT;  	
} 


///////////////////尾桨短路中断//////////////////////////////////
interrupt void XINT1_isr(void)
{ 
  DINT;
  if(PieCtrl.PIEACK.bit.ACK1==1)
  { 
    PieCtrl.PIEACK.bit.ACK1=1;  
    if(Flag_PowerOnBIT_Finish==1)   //上电BIT完成
    {
      GpioMuxRegs.GPEMUX.bit.XINT1_XBIO_GPIOE0=1;
	  GpioMuxRegs.GPEDIR.bit.GPIOE0=0;
	  Delay(0x3ffff);
      if(GpioDataRegs.GPEDAT.bit.GPIOE0==1)
	  { 
	    Flag_TRB_OK=1;          //尾桨加热故障
	    Number1_Error=Number1_Error|0x4;   //上报尾桨防冰失效
        GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //关闭尾桨加热
        GpioDataRegs.GPBDAT.bit.GPIOB0=0;   //关闭尾桨控制开关
		if(Flag_MBIT_Start==0)
        {
          GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //点亮尾桨除冰失效灯
        } 
	  }
	}
  }
  EINT;
}

///////////////////主桨短路中断//////////////////////////////////
interrupt void XINT2_isr(void)
{ 
  DINT;
  if(PieCtrl.PIEACK.bit.ACK1==1)
  { 
    PieCtrl.PIEACK.bit.ACK1=1;  
    if(Flag_PowerOnBIT_Finish==1)   //上电BIT完成
    {
      GpioMuxRegs.GPEMUX.bit.XINT2_ADCSOC_GPIOE1=1;
	  GpioMuxRegs.GPEDIR.bit.GPIOE1=0;
	  Delay(0x3ffff);
	  if(GpioDataRegs.GPEDAT.bit.GPIOE1==1)
	  { 
	    Flag_MRB_OK=1;        //主桨加热故障
        GpioDataRegs.GPBDAT.bit.GPIOB11=0;  //关闭主桨加热
        GpioDataRegs.GPBDAT.bit.GPIOB2=1;   //关闭主桨控制开关       
	    Number1_Error=Number1_Error|0x2;   //上报主桨除冰失效
		if(Flag_MBIT_Start==0)
		{
		  GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //点亮主桨除冰失效灯
		}
	  }
	} 
  }
  EINT;
}


///////////直接返回中断服务程序//////////////
interrupt void nothing()
{ 
  EINT;
  return;
}

/////////////主程序//////////////////////////////////////

main()
{  
  Init_Variable();
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
  Init_eva_timer1();
  Init_evb_timer4();
  InitXINT1();
  InitXINT2();
  InitSci();
  InitAdc();
 		
  EALLOW;
  PieVectTable.XINT1=&XINT1_isr;
  PieVectTable.XINT2=&XINT2_isr;	
  PieVectTable.ADCINT=&ad_isr;
  PieVectTable.T1PINT=&eva_timer1_isr;
  PieVectTable.RXBINT=&SCIB_ISR;
  EDIS;  
  
  //开中断   
  IER |= (M_INT1|M_INT2|M_INT9);
  PieCtrl.PIEIER1.bit.INTx4=1;
  PieCtrl.PIEIER1.bit.INTx5=1;
  PieCtrl.PIEIER1.bit.INTx6=1;
  PieCtrl.PIEIER2.bit.INTx4=1;
  PieCtrl.PIEIER9.bit.INTx3=1;

  EINT;  
  ERTM;  
  GpioDataRegs.GPBDAT.bit.GPIOB12=~GpioDataRegs.GPBDAT.bit.GPIOB12;
  PowerOn_BIT();

  while(1)
  { 
	if(EvaRegs.T1CON.bit.TENABLE==1)  //定时器正常
	{
	  if(Count_SCI_Send>=40)     //1s到发送422监控信息
	  { 
	    Count_SCI_Send=0;
        SciaRegs.SCITXBUF=0xaa;
        SciaRegs.SCITXBUF=0x55;
        SciaRegs.SCITXBUF=0x00;
        SciaRegs.SCITXBUF=0x0c;
		SciaRegs.SCITXBUF=0x01;
		SciaRegs.SCITXBUF=LWC;
		SciaRegs.SCITXBUF=Ice_Degree;
        SciaRegs.SCITXBUF=Number1_Error;
        SciaRegs.SCITXBUF=Number2_Error;
        SciaRegs.SCITXBUF=0x00;
        SciaRegs.SCITXBUF=0x00;
        SciaRegs.SCITXBUF=0xaa+0x55+0x0c+0x01+LWC+Ice_Degree+Number1_Error+Number2_Error;
	  }
      //////////////////////////////////////////////       
      if(Count_E422_Check>=52)  
      {  
        Count_E422_Check=0;
        if(Flag_E422_Receive==1)  //收到LWC数据
	    {
          Flag_E422_Receive=0;
	    }
	    else
	    {
          Count_E422_Error++;
		  if(Count_E422_Error>=5)    //连续5次未收到LWC
		  {          
            Number2_Error=Number2_Error|0x4;   //上报结冰探测失效 
		  }
	    }
      }
    }
    else
    {
	  GpioDataRegs.GPBDAT.bit.GPIOB12=~GpioDataRegs.GPBDAT.bit.GPIOB12;  //喂狗         
      SciaRegs.SCITXBUF=0xaa;
      SciaRegs.SCITXBUF=0x55;
      SciaRegs.SCITXBUF=0x00;
      SciaRegs.SCITXBUF=0x0c;
      SciaRegs.SCITXBUF=0x01;
      SciaRegs.SCITXBUF=LWC;
	  SciaRegs.SCITXBUF=Ice_Degree;
      SciaRegs.SCITXBUF=Number1_Error;
      SciaRegs.SCITXBUF=Number2_Error;
      SciaRegs.SCITXBUF=0x00;
      SciaRegs.SCITXBUF=0x00;
      SciaRegs.SCITXBUF=0xaa+0x55+0x0c+0x01+LWC+Ice_Degree+Number1_Error+Number2_Error;
      Delay(0x8ffff);
    }     
    ///////////////////////////////////////////////////////////  
    if(Flag_PowerOnBIT_Finish==1)   //上电BIT完成
    {
      if(Flag_MBIT_Start==1)   //准备进行维护BIT
      { 
	    Number2_Error=Number2_Error|0x10;  //正在自检
        MBIT();   //地面维护BIT
      }
      else      //巡检时
      {
        Number2_Error=Number2_Error&0xef;   //自检完成
        ////////////////////////////////////
        if((Number2_Error&0x4)==0x0)
        {
          Detector_Check();   //结冰探测检查
		}
		else	   //结冰探测失效上报主桨故障
		{
	      if((Flag_MBIT_Start==0)&&(Flag_MRB_Heat==0))
		  {
			Flag_MRB_OK=1;     //主桨故障
		  }
		} 
        /////////////////////////////////////////////
        if((Number1_Error&0x20)==0x0)
        {
          AirSensor_Check();  //大气温度传感器检查
		}
		////////////////////////////////////////////////
		if(Flag_TRBSensor1_OK==0)
		{
		  TRBSensor_Check();  //尾桨温度传感器检查
		}
		if((Flag_TRBSensor1_OK==1)&&(Flag_TRBSensor2_OK==0))
		{
		  TRBSensor_Check();  //1故障时立即检查2
		} 			
		/////////////////////////////////////////////////////
        if(GpioDataRegs.GPBDAT.bit.GPIOB10==0)  //温度不阻塞
        {		  
		  if(Flag_TRB_OK==0)     //尾桨正常
		  {
		    if(((Number2_Error&0x4)==0x0)&&(GpioDataRegs.GPADAT.bit.GPIOA5==0))  //结冰
		    {
		      TRB_Heat();         //尾桨加热
			}
			else
			{
              GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //关闭尾桨加热
              GpioDataRegs.GPBDAT.bit.GPIOB0=0;  //关闭尾桨控制开关			  
			}
		  }
          if(((Number2_Error&0x4)==0x0)&&((Number1_Error&0x20)==0x0))
          {
            if(GpioDataRegs.GPADAT.bit.GPIOA5==0)  //结冰
            {
              Delay(0xffff);
              if(GpioDataRegs.GPADAT.bit.GPIOA5==0)
			  {
                Number1_Error=Number1_Error|0x1;
                if((Flag_MRB_Heat==0)&&(Flag_MRB_OK==0))
                {
                  Heat_Time_Count();  //计算加热时间
				  ONT_Real=ONT_Vary;
                  Flag_MRB_Heat=1;     //主桨准备加热
                }                            
			  }
		    }
		    else
		    {
              Number1_Error=Number1_Error&0xfe;
		    }
		  }
		}
        ////////////////////////////////////////////////
        if((Flag_MRB_OK==0)&&(Flag_MRB_Heat==1))
		{
          MRB_Heat();	  //主桨正在加热
		}
		else
		{
          GpioDataRegs.GPBDAT.bit.GPIOB11=0;  //关闭主桨加热
          GpioDataRegs.GPBDAT.bit.GPIOB2=1;   //关闭主桨控制开关
		}
        /////////////////////////////////////////////////////////////
        if((GpioDataRegs.GPBDAT.bit.GPIOB10==1)||(Flag_TRB_OK==1))
		{
          GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //关闭尾桨加热
          GpioDataRegs.GPBDAT.bit.GPIOB0=0;  //关闭尾桨控制开关
		}
	  } 
	  ////////////////////////////////////////////////////////////
	  if((GpioDataRegs.GPBDAT.bit.GPIOB2==0)||(GpioDataRegs.GPBDAT.bit.GPIOB1==1))
	  {
	    GpioDataRegs.GPBDAT.bit.GPIOB13=1;    //点亮防除冰系统灯
	    Number2_Error=Number2_Error|0x8;
	  } 
	  else
	  {
	    GpioDataRegs.GPBDAT.bit.GPIOB13=0;    //防除冰系统灯灭掉
        Number2_Error=Number2_Error&0xf7;
	  }   
    }  
    else   //上电BIT故障
    {
	  GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //点亮主桨除冰失效灯 
      GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //点亮尾桨除冰失效灯      
      if(Count_Light>=5)   //上电BIT故障控制器指示灯4Hz闪烁
      {
        Count_Light=0;
        GpioDataRegs.GPBDAT.bit.GPIOB9=~GpioDataRegs.GPBDAT.bit.GPIOB9;
	  }  
    }  
  }          
}  

