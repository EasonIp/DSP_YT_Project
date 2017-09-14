
#include "DSP28_Device.h"
//#include "IQmathLib.h"
#include "fir.h"
#include "math.h"
//#include "stdio.h"

#define FIR_ORDER 50
#pragma DATA_SECTION(lpf, "firfilt");
FIR16 lpf=FIR16_DEFAULTS;
#pragma DATA_SECTION(dbuffer,"firldb");
long dbuffer[(FIR_ORDER+2)/2];
const long coeff[(FIR_ORDER+2)/2]=FIR16_LPF50; 
Uint16 input;
Uint16 output;
float out;
Uint16 a[100];
Uint16 b[16];
Uint16 x[100];
float yn[100];
Uint16 channel;
Uint16 n=0,j=0,m=0;

float max=0;
unsigned int i;
char flag1=0;   //第一个100个数据已取出
char flag2=0;   //

void InitAdc(void);
interrupt void ad(void);


/*U#pragma CODE_SECTION(InitFlash, "secureRamFuncs")
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

////////////////////////
void main(void)
{

	InitSysCtrl();

	DINT;
	IER = 0x0000;
	IFR = 0x0000;

	InitPieCtrl();
	InitPieVectTable();	
		
	EALLOW;	
	PieVectTable.ADCINT=&ad;
	EDIS;   
    
	IER |= M_INT1;	
    PieCtrl.PIEIER1.bit.INTx6=1;

	EINT;  
	ERTM;


	
	/*channel=0x0000;
	InitAdc();  
	EvbRegs.T4CON.bit.TENABLE=1;
	
	while(1)
	{}
	while(1)
	{ if(flag2==1) 
	  { x[j]=a[j];
	    j++;
		n++;
		flag2=0;
		if(j>100)
		{ j=0;
		  n=0;
		  break;
		}
      }
	}
 
    EvbRegs.T4CON.bit.TENABLE=0;
	
	for(j=0;j<100;j++)
    {	input=x[j];
	    lpf.input=input;
	    lpf.calc(&lpf);
		output=lpf.output;
		out=(float)output;
		yn[j]=out*3.0/4095;
	}   
    EvbRegs.T4CON.bit.TENABLE=1;  
    
    for(;;)
    { j=0;
      for(j=0;j<99;j++)
      { x[j]=x[j+1];
	  }
	  while(1)
	  { if(flag2==1)
	    {  x[99]=a[n];
		   n++;
		   if(n>100)
		   { n=0;
		   }
           flag2=0;
		   break;
		}
	  }
          
      EvbRegs.T4CON.bit.TENABLE=0;
	  for(j=0;j<100;j++)
	  {	input=x[j];
	    lpf.input=input;
	    lpf.calc(&lpf);
		output=lpf.output;
		out=(float)output;
		yn[j]=out*3.0/4095;
	  }
	  EvbRegs.T4CON.bit.TENABLE=1;
    }*/
  
} 	

///////////////////AD初始化//////////////////////////////////
void InitAdc(void)
{
		
	AdcRegs.ADCTRL1.bit.RESET=1;  //复位ADC模块
    NOP;
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
	AdcRegs.ADCTRL3.bit.ADCCLKPS=15;  //ADCLK=30MHz/2=15MHz
	AdcRegs.ADCTRL3.bit.SMODE_SEL=0;  //顺序采样

	
	AdcRegs.MAX_CONV.bit.MAX_CONV=16;  //采样通道为16
  
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

//////////////////////////////////////////
interrupt void ad(void)
{   
    a[1]=0;
    DINT;
	IFR = 0x0000;
    PieCtrl.PIEACK.all=0xffff;

	a[1]= AdcRegs.RESULT0>>4;
    /*b[0]=AdcRegs.RESULT0>>4;
	b[1]=AdcRegs.RESULT1>>4;
	b[2]=AdcRegs.RESULT2>>4;
	b[3]=AdcRegs.RESULT3>>4;
	b[4]=AdcRegs.RESULT4>>4;
	b[5]=AdcRegs.RESULT5>>4;
	b[6]=AdcRegs.RESULT6>>4;
	b[7]=AdcRegs.RESULT7>>4;
	b[8]=AdcRegs.RESULT8>>4;
	b[9]=AdcRegs.RESULT9>>4;
	b[10]=AdcRegs.RESULT10>>4;
	b[11]=AdcRegs.RESULT11>>4;
  	b[12]=AdcRegs.RESULT12>>4;
	b[13]=AdcRegs.RESULT13>>4;
	b[14]=AdcRegs.RESULT14>>4;
	b[15]=AdcRegs.RESULT15>>4;
	
	for(i=0;i<15;i++)
    { for(m=0;m<15-i;m++)
	  { if(b[m+1]<b[m])
	    { max=b[m];
	      b[m]=b[m+1];
	      b[m+1]=max;
	    }
	  }	    	  
	}

	a[n]=0;
	for(m=0;m<12;m++)
	{a[n]=a[n]+b[m+2];
	}
	a[n]=a[n]/12; */

	flag2=1;
	AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;
	AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;	
	EINT;	
}






