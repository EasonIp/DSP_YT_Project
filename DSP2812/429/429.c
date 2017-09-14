#include "DSP28_Device.h"

#define RAMBASE6 0x0100000      //���ߵ�ַ,��Ӧ��Ƭѡ�ź�XZCS6 

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


////////////////��������/////////////////////////////////

Uint32 i;                  //����
Uint16 count=0;            
/*Uint16 *rambase6; 
Uint16 CSH;               //429��ʼ��
Uint16 FSDZJ_271;         //����LABLE271���ֽ�
Uint16 FSGZJ_271;         //����LABLE271���ֽ�
Uint16 FSDZJ_272;
Uint16 FSGZJ_272; 
Uint16 FSDZJ_273;
Uint16 FSGZJ_273; 
Uint16 FSDZJ1;
Uint16 FSGZJ1;
Uint16 JSDZJ;			     //���յ��ֽ�
Uint16 JSGZJ;			     //���ո��ֽ�*/
char flag=0;

/////////////////��������/////////////////////////////////

void InitSysCtrl(void);
void InitGpio(void);
void init_eva_timer1(void);
//void InitXintf(void);
//void InitXINT1(void);
//void arinc429fasong(void);
interrupt void eva_timer1_isr(void);
//interrupt void XINT1_isr(void);



//////////////////////ϵͳ��ʼ��//////////////////////////////////

void InitSysCtrl(void)
{
   EALLOW;
   
   DevEmuRegs.M0RAMDFT = 0x0300;
   DevEmuRegs.M1RAMDFT = 0x0300;
   DevEmuRegs.L0RAMDFT = 0x0300;
   DevEmuRegs.L1RAMDFT = 0x0300;
   DevEmuRegs.H0RAMDFT = 0x0300;
        
   SysCtrlRegs.WDCR= 0x0068;   //��ֹ���Ź�

   SysCtrlRegs.PLLCR = 0x8;   //��Ƶ=30*8/2=120MHz
  
   for(i=0; i<5000; i++){}   // Ϊ��Ƶ���ȶ���������ʱһ��ʱ��
      
   SysCtrlRegs.HISPCP.all = 0x0002;  //��������ʱ��=120/4=30
   SysCtrlRegs.LOSPCP.all = 0x0002;	 //��������ʱ��=120/4=30
 
   SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;  //ʹ��EVAģ��ĸ�������ʱ��
   SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;  //ʹ��EVBģ��ĸ�������ʱ��
   SysCtrlRegs.PCLKCR.bit.ADCENCLK=1;  //ʹ��ADCģ��ĸ�������ʱ��
   SysCtrlRegs.PCLKCR.bit.ECANENCLK=1; //ʹ��CAN�����ϵͳʱ��
				
   EDIS;
	
}


///////////////////IO�ڳ�ʼ��//////////////////////////////////

void InitGpio(void)
{

   EALLOW;
   /*GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOA����ΪI/O�� 
   GpioMuxRegs.GPADIR.all=0xFFFF;   //A0-A15����Ϊ���	
   GpioDataRegs.GPADAT.all=0x1e; */


   //////С����////////////////
   GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOA����ΪI/O�� 
   GpioMuxRegs.GPADIR.bit.GPIOA2=0;
   GpioMuxRegs.GPADIR.bit.GPIOA3=1;
   GpioDataRegs.GPADAT.bit.GPIOA3=0;
   ///////////////////////////////////////

   EDIS;

}	

///////////////////�ⲿ�ж�1��ʼ��//////////////////////////////////

/*void InitXINT1(void)
{  GpioMuxRegs.GPEMUX.bit.XINT1_XBIO_GPIOE0=1; //�������ж�1���� 
   XIntruptRegs.XINT1CR.all=0x5;      //�����������ж�
}*/

///////////////////��ʱ��1��ʼ����100mS�//////////////////////////////////

void init_eva_timer1(void)
{
  
   EvaRegs.GPTCONA.all = 0;   //��ʼ����ʱ��1�����ö�ʱ��1�Ĵ���
   EvaRegs.EVAIMRA.bit.T1PINT = 1;  //�������1�����ж�
   EvaRegs.EVAIFRA.bit.T1PINT = 1;  //�嶨�1�����жϱ�־
             
   EvaRegs.T1CON.all = 0x170C;  //Timer1Ϊ����������ģʽ��Ԥ��Ƶֵ128
	                             //ʹ���ڲ���������ʱ��
   EvaRegs.T1PR = 0x5b8d;   //Timer1���ڼĴ���=0.1*30MHz/128
   EvaRegs.T1CNT = 0x0000;  //Timer1�ļ����

}

	
////////////////////�ⲿ�ӿڳ�ʼ��/////////////////////////
/*void InitXintf(void)
{  
    XintfRegs.XINTCNF2.bit.XTIMCLK = 0x0001;  //����zone��ʱ���ǻ���XTIMCLK = SYSCLKOUT/2 
    XintfRegs.XINTCNF2.bit.WRBUFF = 3;   //����3��д����
	XintfRegs.XINTCNF2.bit.CLKOFF = 0;  //ʹ��XCLKOUT
	XintfRegs.XINTCNF2.bit.CLKMODE = 0;  //����XCLKOUT=XTIMCLK
    	
	XintfRegs.XTIMING2.bit.X2TIMING=1;
	//Zone2дʱ��
	XintfRegs.XTIMING2.bit.XWRLEAD = 1;    //д���������׶ε�����=2*XTIMCLK
	XintfRegs.XTIMING2.bit.XWRACTIVE = 7;   //д��������׶εȴ�״̬������=14*XTIMCLK
	XintfRegs.XTIMING2.bit.XWRTRAIL = 2;    //д�������ٽ׶ε�����=4*XTIMCLK
   //Zone2��ʱ��
	XintfRegs.XTIMING2.bit.XRDLEAD = 1;    //�����������׶ε�����=2*XTIMCLK
	XintfRegs.XTIMING2.bit.XRDACTIVE=7;	    //����������׶εȴ�״̬������=14*XTIMCLK
	XintfRegs.XTIMING2.bit.XRDTRAIL = 2;   //���������ٽ׶ε�����=4*XTIMCLK
	//���ӱ�ʱ��
	XintfRegs.XTIMING2.bit.X2TIMING = 0; 
   //Zone2����READY�źŲ���
    XintfRegs.XTIMING2.bit.USEREADY = 0;
    XintfRegs.XTIMING2.bit.READYMODE = 0;
    XintfRegs.XTIMING2.bit.XSIZE = 3;   //16λ�ӿڣ�Ψһ��Ч������

    XintfRegs.XBANK.bit.BANK = 2;   //ʹ���ڲ�����2
    XintfRegs.XBANK.bit.BCYC = 3;  //��������֮�������3��XTIMCLK����

	//Zone6дʱ��
	XintfRegs.XTIMING6.bit.XWRLEAD = 1;    //д���������׶ε�����=XTIMCLK
	XintfRegs.XTIMING6.bit.XWRACTIVE = 7;   //д��������׶εȴ�״̬������=XTIMCLK
	XintfRegs.XTIMING6.bit.XWRTRAIL = 2;    //д�������ٽ׶ε�����=XTIMCLK
   //Zone6��ʱ��
	XintfRegs.XTIMING6.bit.XRDLEAD = 1;    //�����������׶ε�����=XTIMCLK
	XintfRegs.XTIMING6.bit.XRDACTIVE=1;	    //����������׶εȴ�״̬������=XTIMCLK
	XintfRegs.XTIMING6.bit.XRDTRAIL = 1;   //���������ٽ׶ε�����=XTIMCLK
	//���ӱ�ʱ��
	XintfRegs.XTIMING6.bit.X2TIMING = 0; 
   //Zone6����READY�źŲ���
    XintfRegs.XTIMING6.bit.USEREADY = 0;
    XintfRegs.XTIMING6.bit.READYMODE = 0;
    XintfRegs.XTIMING6.bit.XSIZE = 3;   //16λ�ӿڣ�Ψһ��Ч������

    XintfRegs.XBANK.bit.BANK = 6;   //ʹ���ڲ�����6
    XintfRegs.XBANK.bit.BCYC = 3;  //��������֮�������3��XTIMCLK����
	
}

/////////////429����////////////////////////////////
void arinc429fasong(void)
{ 
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;   
   *rambase6=FSDZJ_271;
   for(i=0;i<5;i++){} 
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x8;
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;   
   *rambase6=FSGZJ_271;
   for(i=0;i<5;i++){}
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x10;
   
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;  
   *rambase6=FSDZJ_272;
   for(i=0;i<5;i++){} 
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x8;
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;   
   *rambase6=FSGZJ_272;
   for(i=0;i<5;i++){}
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x10;

   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;   
   *rambase6=FSDZJ_273;
   for(i=0;i<5;i++){} 
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x8;
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;   
   *rambase6=FSGZJ_273;
   for(i=0;i<5;i++){}
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x10;

   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfff7;   
   *rambase6=FSDZJ1;
   for(i=0;i<5;i++){} 
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x8;
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffef;   
   *rambase6=FSGZJ1;
   for(i=0;i<5;i++){}
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x10;

}*/

///////////////////////////////////////////////////////
interrupt void eva_timer1_isr(void)
{  DINT;

  /*if(PieCtrl.PIEACK.bit.ACK2==1)
  { PieCtrl.PIEACK.bit.ACK2=1;
    count++; 
    if(count>=18)
    { count=0;
      arinc429fasong();
    }
   EvaRegs.EVAIMRA.bit.T1PINT = 1;
   EvaRegs.EVAIFRA.bit.T1PINT = 1;
  }*/

  if(PieCtrl.PIEACK.bit.ACK2==1)
  { PieCtrl.PIEACK.bit.ACK2=1;
    count++; 
   EvaRegs.EVAIMRA.bit.T1PINT = 1;
   EvaRegs.EVAIFRA.bit.T1PINT = 1;
  }

  EINT;
}




///////////////////429�����ж�//////////////////////////////////

/*interrupt void XINT1_isr(void)
{ Uint16 ix;
  DINT;
  if(PieCtrl.PIEACK.bit.ACK1==1)
  { PieCtrl.PIEACK.bit.ACK1=1;
   		
    GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfffc; 
    for(ix=0;ix<10;ix++){}  
    JSDZJ=*rambase6;  
    for(ix=0;ix<10;ix++){} 
    GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
    for(ix=0;ix<5;ix++){} 
    GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xfffd;
    for(ix=0;ix<10;ix++){}
    JSGZJ=*rambase6;
    for(ix=0;ix<10;ix++){}   
    GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
    
    FSDZJ1=JSDZJ;  
    FSGZJ1=JSGZJ;    
  }
  EINT;
} */


///////////ֱ�ӷ����жϷ������//////////////
interrupt void nothing(void)
{ EINT;
  return;
}

/////////////������//////////////////////////////////////

main()
{  InitSysCtrl();

   DINT;
   IER = 0x0000;
   IFR = 0x0000;

   InitPieCtrl();
   InitPieVectTable();
   InitGpio();
   init_eva_timer1();
   //InitXINT1();	
   //InitXintf();
		
   EALLOW;
   //PieVectTable.XINT1=&XINT1_isr;
   PieVectTable.T1PINT=&eva_timer1_isr;
   EDIS;  
  
   //���ж�
   IER |= (M_INT1|M_INT2);
  // PieCtrl.PIEIER1.bit.INTx4=1;
   PieCtrl.PIEIER2.bit.INTx4=1;
   EINT;  
   ERTM;

   /*FSDZJ_271=0x00b9;   //��ʼ��429�ź�
   FSGZJ_271=0x0000;  
   FSDZJ_272=0x40ba;   //�ϵ�Ĭ�������� 
   FSGZJ_272=0x0000;
   FSDZJ_273=0x00bb;  
   FSGZJ_273=0x0000;


   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffdf;  //��ʼ��3282
   CSH=0x0020;
   rambase6=(Uint16 *)RAMBASE6;
   *rambase6=CSH;
   for(i=0;i<5;i++){}
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x30;*/


   
     /*if(GpioDataRegs.GPADAT.bit.GPIOA2==1)
     { EvaRegs.T1CON.bit.TENABLE=1;  // �򿪶�ʱ��1
       while(1)
       { if(count>=100)
         { count=0;
	       GpioDataRegs.GPADAT.bit.GPIOA3=1;
		   break;
         }
       }     
     }  
     for(;;){} */


	 while(1)
	 { EvaRegs.T1CON.bit.TENABLE=1;  // �򿪶�ʱ��1
	   while(1)
	   { if(count>=10)
	     { count=0;
		   flag=1;
	       break;		
		 }
	   }
	   EvaRegs.T1CON.bit.TENABLE=0;
       if(flag==0)break;
	 }

	 for(;;){}

} 