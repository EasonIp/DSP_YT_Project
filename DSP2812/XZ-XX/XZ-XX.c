/* 
   * 直升机旋翼防除冰控制器
   * 文件名：XZ-XX.pit
   * 时间：2010.6.1
   * 版本：第二版
   
*/

#include "DSP28_Device.h"
#include "math.h"

///////变量申明///////////////
int i,j;
Uint32  count1=0;
int channel;
int	a[16];
float AD0=0.0,AD1=0.0,adclo=0.0,cesi;
float LWC,Temperature,ONTIMER,OFFTIMER,ONTIMER1,OFFTIMER1;
int ONT,OFT;
int n,max,a1;
char flag=0;  


////////函数申明/////////////
void InitSysCtrl(void);
void InitGpio(void);
void init_eva_timer1(void);
void InitAdc(void);
void XINT1(void);
void Count(void);
void PWM(void);
interrupt void eva_timer1_isr(void);
interrupt void ad(void);
interrupt void XINT1_isr(void);


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

   SysCtrlRegs.PLLCR = 0x3;   //主频=30*3/2=45MHz
  
   for(i= 0; i< 5000; i++){}   // 为了频率稳定，必须延时一段时间
       
   SysCtrlRegs.HISPCP.all = 0x0002;  //高速外设时钟=45/4
   SysCtrlRegs.LOSPCP.all = 0x0002;	 //低速外设时钟=45/4
 
   SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;  //使能EVA模块的高速外设时钟
   SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;  //使能EVB模块的高速外设时钟
   SysCtrlRegs.PCLKCR.bit.ADCENCLK=1;  //使能ADC模块的高速外设时钟
   SysCtrlRegs.PCLKCR.bit.SCIENCLKA=1; //使能SCIA模块的低速外设时钟
				
   EDIS;
	
}

///////////////////IO口初始化//////////////////////////////////

void InitGpio(void)
{

     EALLOW;
     GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOA设置为I/O口 
     GpioMuxRegs.GPADIR.all=0x3F;   //A0-A5设置为输出，A6-A15设置为输入	
     //GpioDataRegs.GPADAT.bit.GPIOA0=1;
     //GpioDataRegs.GPADAT.bit.GPIOA1=1;
     //GpioDataRegs.GPADAT.bit.GPIOA2=1;
     //GpioDataRegs.GPADAT.bit.GPIOA5=1;

     GpioMuxRegs.GPBMUX.all=0x0000;    //GPIOB设置为I/O口 
     GpioMuxRegs.GPBDIR.all=0x3F;   //B0-B5设置为输出，B6-B7设置为输入	
     GpioDataRegs.GPBDAT.bit.GPIOB0=1;
     GpioDataRegs.GPBDAT.bit.GPIOB1=1;
	 GpioDataRegs.GPBDAT.bit.GPIOB2=1;
	 GpioDataRegs.GPBDAT.bit.GPIOB3=1;
	 //GpioDataRegs.GPBDAT.bit.GPIOB4=1;
	 GpioDataRegs.GPBDAT.bit.GPIOB5=1;
     EDIS;

}	


///////////////////定时器1初始化（50mS）//////////////////////////////////

void init_eva_timer1(void)
{
  
    EvaRegs.GPTCONA.all = 0;   //初始化定时器1并设置定时器1寄存器
    EvaRegs.EVAIMRA.bit.T1PINT = 1;  //允许定时器1周期中断
    EvaRegs.EVAIFRA.bit.T1PINT = 1;  //清除定时器1周期中断标志
             
    EvaRegs.T1CON.all = 0x170C;  //Timer1为连续增计数模式，预分频值为128，
	                             //使用内部高速外设时钟
	EvaRegs.T1PR = 0x112a;   //Timer1的周期寄存器=0.05*45MHz/4/128
	EvaRegs.T1CNT = 0x0000;  //Timer1的计数器清零

}

///////////////////定时器1中断//////////////////////////////////

interrupt void eva_timer1_isr(void)
{ DINT; 
  if(PieCtrl.PIEACK.bit.ACK2==1)
  { PieCtrl.PIEACK.bit.ACK2=1;  
    count1++;
    EvaRegs.EVAIMRA.bit.T1PINT = 1;
    EvaRegs.EVAIFRA.bit.T1PINT = 1;
  }
  EINT;
}


///////////////////AD初始化//////////////////////////////////
void InitAdc(void)
{
		
	AdcRegs.ADCTRL1.bit.RESET=1;  //复位ADC模块
    for(i=0;i<5000;i++)	{}
	AdcRegs.ADCTRL1.bit.RESET=0;
	AdcRegs.ADCTRL1.bit.SUSMOD=3;
	AdcRegs.ADCTRL1.bit.ACQ_PS=0;
	AdcRegs.ADCTRL1.bit.CPS=0;     //ADCCLK=HSPCLK/2
	AdcRegs.ADCTRL1.bit.CONT_RUN=0; //AD为启动/停止模式
	AdcRegs.ADCTRL1.bit.SEQ_CASC=1; //16级联模式
  	
	AdcRegs.ADCTRL3.bit.ADCBGRFDN=3; //带间隙参考电路上电
	for(i=0;i<10000;i++)  {}          //等待稳定
	AdcRegs.ADCTRL3.bit.ADCPWDN=1;  //其它电路上电
	for(i=0;i<5000;i++)	 {}           //等待稳定
	AdcRegs.ADCTRL3.bit.ADCCLKPS=15;
	AdcRegs.ADCTRL3.bit.SMODE_SEL=0;  //顺序采样

	
	AdcRegs.MAX_CONV.bit.MAX_CONV=15;  //采样通道为16
  
    AdcRegs.CHSELSEQ1.all= channel;   //同一个通道采16次
    AdcRegs.CHSELSEQ2.all= channel;
    AdcRegs.CHSELSEQ3.all= channel;
    AdcRegs.CHSELSEQ4.all= channel;

	AdcRegs.ADCTRL2.all=0x8c04;   //可用EVB的一个事件信号触发AD转换

	////////////采用定时器4启动AD转换/////////////////////
    EvbRegs.GPTCONB.all=0x400;    //定时器4周期中断启动AD转换
    EvbRegs.EVBIMRB.bit.T4PINT = 1;  //允许定时器4周期中断
    EvbRegs.EVBIFRB.bit.T4PINT = 1;  //清除定时器4周期中断标志
             
    EvbRegs.T4CON.all = 0x170C;  //Timer4为连续增计数模式，预分频值为128，
	                             //使用内部高速外设时钟
	EvbRegs.T4PR = 0x57;     //Timer4的周期寄存器=0.001*45MHz/4/128
	EvbRegs.T4CNT = 0x0000;  //Timer4的计数器清零
				
	AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;
	AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;	
			
}	

///////////////////AD中断/////////////////////////////////

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
        
	 for(i=0;i<15;i++)
     { for(n=0;n<15-i;n++)
	   { if(a[n+1]<a[n])
	     { max=a[n];
	       a[n]=a[n+1];
	       a[n+1]=max;
	     }
	   }	    	  
	 }

     cesi=0.0;
	 for(i=0;i<12;i++)  //去头尾各2个取平均
	 { a1=a1+a[i+2];
	 }
     a1=a1/12;
	 cesi=a1*3.0/4095+adclo;

	/////////发送串口数据//////////////////////
     SciaRegs.SCITXBUF= a1&0xff00;  //发送高8位
     SciaRegs.SCITXBUF= a1&0xff;   //发送低8位
    ///////////////////////////////////////////

	 AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;  //清除中断标志
	 AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;	
   }
   EINT;	
}

/////////////////时间计算////////////////////////////////////////

void Count(void)
{ Temperature=((AD0/2+3.654)/40-0.10276)*1000/0.38;
  LWC=(AD1*4.7/1.49-1)/4;

  if(Temperature<-20)
  { ONTIMER=13.0;
    OFFTIMER=60/LWC;
  }
  if((-20<=Temperature)&&(Temperature<=0))
  { ONTIMER=1-12*Temperature/20;
    OFFTIMER=60/LWC;
  }
   if((0<Temperature)&&(Temperature<20))
  { ONTIMER=1.0;
    OFFTIMER=60/LWC;
  }
   if(Temperature>=20)
  {  GpioDataRegs.GPADAT.bit.GPIOA1=0;  //输出温度控制信号
  }

  ONTIMER1=(int)ONTIMER+0.5;  
  if(ONTIMER1>ONTIMER)
  { ONTIMER1=ONTIMER1-0.5;
  }
  else
  { ONTIMER1=ONTIMER1+0.5;
  }
  ONT=(int)(ONTIMER1/0.05);

  OFFTIMER=OFFTIMER-0.15*8-7*ONT*0.05;
  OFFTIMER1=(int)OFFTIMER+0.5;
  if(OFFTIMER1>OFFTIMER)
  { OFFTIMER1=OFFTIMER1-0.5;
  }
  else
  { OFFTIMER1=OFFTIMER1+0.5;
  }
  OFT=(int)(OFFTIMER1/0.05);

  ONT=40;
  OFT=200;
}

/////////////////时序加热////////////////////////////////////////

void PWM()
{ 
   //GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //打开加热控制
   j=0;
   EvaRegs.T1CON.bit.TENABLE=1;   //打开定时器1
   ONT=40;
   OFT=600;
   while(1)
   {if((0<count1)&&(count1<=ONT))
    { GpioDataRegs.GPBDAT.bit.GPIOB3=0;	   //旋翼加热控制
	  GpioDataRegs.GPBDAT.bit.GPIOB5=0;	   //尾桨加热控制
	}
    if((ONT<count1)&&(count1<=(ONT+3)))
    { GpioDataRegs.GPBDAT.bit.GPIOB3=1;
	  GpioDataRegs.GPBDAT.bit.GPIOB5=1;	 
	}
	if(count1>=(ONT+3))
	{ count1=0;
	  j++;
	  if(j==8)break;
	}
   }    
   
   while(1)
	{ if(count1>=OFT)
	  { count1=0;
	    break;
	  }
	}
    
    EvaRegs.T1CON.bit.TENABLE=0;   //关闭定时器1

}


///////////////////獠恐卸�1初始化//////////////////////////////////

void XINT1(void)
{ GpioMuxRegs.GPEMUX.bit.XINT1_XBIO_GPIOE0=1;   //设置外部中断1功能
  XIntruptRegs.XINT1CR.all=0x0;   //下降沿生成中断
}


///////////////////外部中断1中断//////////////////////////////////

interrupt void XINT1_isr(void)
{ 
  DINT;
  PieCtrl.PIEACK.bit.ACK1=1;   //允许进入同组其它中断
  EINT;

  /////////温度传感器故障（给定温度-20）//////////////////////////
  /*if( GpioDataRegs.GPADAT.bit.GPIOA15==1)
  { while(1)
	{ if(GpioDataRegs.GPADAT.bit.GPIOA6==1) //结冰告警
	  {if(GpioDataRegs.GPADAT.bit.GPIOA12==1)  //查询加热电源是否接通
	   {  
         channel=0x1111;    //ADCINA1采集结冰速率
         InitAdc();

	     EvbRegs.T4CON.bit.TENABLE=1;  //打开定时器4开始采样

	     while(1)
	     {if(i==12)
	       break;
	     }
         i=0;
         AD1=cesi;
         EvbRegs.T4CON.bit.TENABLE=0;  //关闭定时器4

         Count();
		 ONT=13;
		 PWM();
       }
      }
     }    
   }
  
   /////////结冰速率探测故障（手动三档）/////////////////////////
   if( GpioDataRegs.GPADAT.bit.GPIOA7==1)
   { while(1)
	{ if(GpioDataRegs.GPADAT.bit.GPIOA6==1) //结冰告警
	  {if(GpioDataRegs.GPADAT.bit.GPIOA12==1)  //查询加热电源是否接通
	   { 
	     channel=0x0000;    //ADCINA0采集大气温度
         InitAdc();

	     EvbRegs.T4CON.bit.TENABLE=1;  //打开定时器4开始采样

	     while(1)
	     {if(i==12)
	       break;
	     }
         i=0;
         AD1=cesi;
         EvbRegs.T4CON.bit.TENABLE=0;  //关闭定时器4
         
         channel=0x1111;    //ADCINA1采集手动三档电压
         InitAdc();

	     EvbRegs.T4CON.bit.TENABLE=1;  //打开定时器4开始采样

	     while(1)
	     {if(i==12)
	       break;
	     }
         i=0;
         AD1=cesi;
         EvbRegs.T4CON.bit.TENABLE=0;  //关闭定时器4

         Count();
		 PWM();
       }
      }
     }    
   }
  
  //////////旋翼加热时序故障或旋翼加热电流故障////////////////////////////
   if((GpioDataRegs.GPBDAT.bit.GPIOB7==1)|(GpioDataRegs.GPADAT.bit.GPIOA2==0))
   { while(1)
     { GpioDataRegs.GPBDAT.bit.GPIOB3=1;  //断开旋翼加热控制 
      
	  ///////////探测子系统正常工作///////////////////////
        channel=0x0000;    //ADCINA0采集大气温度
         InitAdc();

	     EvbRegs.T4CON.bit.TENABLE=1;  //打开定时器4开始采样

	     while(1)
	     {if(i==12)
	       break;
	     }
         i=0;
         AD1=cesi;
         EvbRegs.T4CON.bit.TENABLE=0;  //关闭定时器4
         
         channel=0x1111;    //ADCINA1采集手动三档电压
         InitAdc();

	     EvbRegs.T4CON.bit.TENABLE=1;  //打开定时器4开始采样

	     while(1)
	     {if(i==12)
	       break;
	     }
         i=0;
         AD1=cesi;
         EvbRegs.T4CON.bit.TENABLE=0;  //关闭定时器4
	 }
   }
  
  //////////温度大于20度（停止工作）////////////////////////////////
  if(GpioDataRegs.GPADAT.bit.GPIOA1==0)
  { while(1) 
    { GpioDataRegs.GPBDAT.bit.GPIOB4=1;  //断开加热控制电源 
    }
  }
 
 //////////尾桨加热电流故障///////////////////////////////////////
  if(GpioDataRegs.GPBDAT.bit.GPIOB6==1)
  { while(1)
    {GpioDataRegs.GPBDAT.bit.GPIOB5=1;  //断开尾桨加热控制 
	}
  }*/

}

////////////////////////////////////////////////////////////////
main()
{ 
  InitSysCtrl();

  DINT;
  IER = 0x0000;
  IFR = 0x0000;

  InitPieCtrl();
  InitPieVectTable();
  InitGpio();
  init_eva_timer1();	
		
  EALLOW;
  PieVectTable.XINT1=&XINT1_isr;	
  PieVectTable.ADCINT=&ad_isr;
  PieVectTable.T1PINT=&eva_timer1_isr;
  EDIS;  
  
  /*开中断*/
  IER |= M_INT1;
  PieCtrl.PIEIER1.bit.INTx4=1;

  IER |= M_INT1;
  PieCtrl.PIEIER1.bit.INTx6=1;

  IER |= M_INT2;
  PieCtrl.PIEIER2.bit.INTx4=1;

  EINT;  
  ERTM;	

  ////////上电自检测//////////////////////////
 
   GpioDataRegs.GPADAT.bit.GPIOA0=0;  //接通上电自检电源

   EvaRegs.T1CON.bit.TENABLE=1;   //打开定时器1

   while(1)         
   { if(count1>=240)  //延时12S后自检
     {count1=0;
	  break;
	 }
   }
   
   EvaRegs.T1CON.bit.TENABLE=0;   //关闭定时器1

   if(GpioDataRegs.GPADAT.bit.GPIOA15==1)//查询温度传感器是否故障
   { flag=1;
   }
   
   if(GpioDataRegs.GPADAT.bit.GPIOA7==1) //查询结冰速率探测是否故障
   { flag=1;
   }
   
   if(GpioDataRegs.GPADAT.bit.GPIOA9!=1)  //查询K61自检是否故障
   { flag=1;
   }
   
    if(GpioDataRegs.GPADAT.bit.GPIOA10!=1)  //查询K62自检是否故障
   { flag=1;
   }

    if(GpioDataRegs.GPADAT.bit.GPIOA11!=1)  //查询K63自检是否故障
   { flag=1;
   }
   
   flag=0;
   if(flag==1)
   { while(1)
     { //GpioDataRegs.GPADAT.bit.GPIOA0=1;  //断开上电自检电源
	 }     
   }
   else
   { flag=0;
     //GpioDataRegs.GPADAT.bit.GPIOA5=0;   //输出上电自检测正常
     //GpioDataRegs.GPADAT.bit.GPIOA0=1;  //断开上电自检电源

	 ////////人工自检测//////////////////////////
	  EvaRegs.T1CON.bit.TENABLE=1;   //打开定时器1

      while(1)         
      { if(count1>=120)  //延时6S后人工检测
        {count1=0;
	     break;
	    }
      }
   
     EvaRegs.T1CON.bit.TENABLE=0;   //关闭定时器1

	 //if(GpioDataRegs.GPADAT.bit.GPIOA13==1)   //打开人工测试电源
     //{ 
       GpioDataRegs.GPBDAT.bit.GPIOB2=0;  //人工检测测试灯亮
	   //GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //打开加热控制电源       
	   /*j=0;
       count1=0;
	   EvaRegs.T1CON.bit.TENABLE=1;   //打开定时器1

	   while(1)
	   { if((0<count1)&&(count1<=17))
	     { GpioDataRegs.GPBDAT.bit.GPIOB3=0;  //打开旋翼加热控制
		   GpioDataRegs.GPBDAT.bit.GPIOB5=0;  //打开尾桨加热控制
		 }
         if((17<count1)&&(count1<=20))
	     { GpioDataRegs.GPBDAT.bit.GPIOB3=1;
		   GpioDataRegs.GPBDAT.bit.GPIOB5=1;		    	      
		 }
		 if(count1>20)
		 { count1=0;
		   j++;
		   if(j==8) break;
		 }
	   }
       j=0;
       EvaRegs.T1CON.bit.TENABLE=0;   //关闭定时器1
       GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //断开加热控制电源*/
       
	   EvaRegs.T1CON.bit.TENABLE=1;   //打开定时器1
       GpioDataRegs.GPBDAT.bit.GPIOB1=0; //打开旋翼测试灯

       while(1)         
       { if(count1>=120)    //延时6S
         { GpioDataRegs.GPBDAT.bit.GPIOB1=1; //关闭旋翼测试灯
           count1=0;		  
	       break;
	     }
       }

       GpioDataRegs.GPBDAT.bit.GPIOB0=0; //打开尾桨测试灯

       while(1)         
       { if(count1>=80)    //延时4S
         { GpioDataRegs.GPBDAT.bit.GPIOB0=1; //关闭尾桨测试灯
           count1=0;		  
	       break;
	     }
       }
       GpioDataRegs.GPBDAT.bit.GPIOB2=1;
       EvaRegs.T1CON.bit.TENABLE=0;   //关闭定时器1

	   /////////查询结冰告警信号///////////////////////

	   while(1)
	   { //if(GpioDataRegs.GPADAT.bit.GPIOA6==1) //结冰告警
	     //{ if(GpioDataRegs.GPADAT.bit.GPIOA12==1)  //查询加热电源是否接通
		  // { 
		    /* channel=0x0000;    //ADCINA0采集大气温度
             InitAdc();
			 EvbRegs.T4CON.bit.TENABLE=1;  //打开定时器4开始采样
             i=0;
			 while(1)
			 {if(i==12)
			  break;
			 }
             i=0;
             AD0=cesi;
             EvbRegs.T4CON.bit.TENABLE=0;  //关闭定时器4
             
			 channel=0x1111;    //ADCINA1采集结冰速率
             InitAdc();
			 EvbRegs.T4CON.bit.TENABLE=1;  //打开定时器4开始采样
			 while(1)
			 {if(i==12)
			  break;
			 }
             i=0;
             AD1=cesi;
             EvbRegs.T4CON.bit.TENABLE=0;  //关闭定时器4

             Count();*/
			 PWM();
		     
		 //  }
		// }
	  }
	//}
  }
}

