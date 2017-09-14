#include "DSP28_Device.h"

#define RAMBASE 0x100000      //���ߵ�ַ,��Ӧ��Ƭѡ�ź�XZCS6
//#define RAMBASE 0x080000      //���ߵ�ַ,��Ӧ��Ƭѡ�ź�XZCS2


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


////////////////��������/////////////////////////////////

Uint32 i=0;                  //����
Uint16 count=0;            
Uint16 *rambase=0; 
Uint16 tmp=0x66;

/////////////////��������/////////////////////////////////

void InitSysCtrl(void);
void InitGpio(void);
void init_eva_timer1(void);
void InitXintf(void);
interrupt void eva_timer1_isr(void);


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
   SysCtrlRegs.PCLKCR.bit.SCIENCLKA=1;
				
   EDIS;
	
}


///////////////////IO�ڳ�ʼ��//////////////////////////////////

void InitGpio(void)
{

   EALLOW;

   GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOA����ΪI/O�� 
   GpioMuxRegs.GPADIR.all=0xFFFF;   //A0-A15����Ϊ���	
   GpioDataRegs.GPADAT.all=0x0000; 

   EDIS;

}	


///////////////////��ʱ��1��ʼ����100mS)//////////////////////////////////

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
void InitXintf(void)
{  
    XintfRegs.XINTCNF2.bit.XTIMCLK = 0x0001;  //����zone��ʱ���ǻ���XTIMCLK = SYSCLKOUT/2 
    XintfRegs.XINTCNF2.bit.WRBUFF = 3;   //����3��д����
	XintfRegs.XINTCNF2.bit.CLKOFF = 0;  //ʹ��XCLKOUT
	XintfRegs.XINTCNF2.bit.CLKMODE = 0;  //����XCLKOUT=XTIMCLK
    	

	//Zone2дʱ��
	/*XintfRegs.XTIMING2.bit.XWRLEAD = 3;    //д���������׶ε�����=XTIMCLK
	XintfRegs.XTIMING2.bit.XWRACTIVE = 7;   //д��������׶εȴ�״̬������=XTIMCLK
	XintfRegs.XTIMING2.bit.XWRTRAIL = 3;    //д�������ٽ׶ε�����=XTIMCLK
   //Zone6��ʱ��
	XintfRegs.XTIMING2.bit.XRDLEAD = 3;    //�����������׶ε�����=XTIMCLK
	XintfRegs.XTIMING2.bit.XRDACTIVE=7;	    //����������׶εȴ�״̬������=XTIMCLK
	XintfRegs.XTIMING2.bit.XRDTRAIL = 3;   //���������ٽ׶ε�����=XTIMCLK
	//���ӱ�ʱ��
	XintfRegs.XTIMING2.bit.X2TIMING = 0; 
   //Zone6����READY�źŲ���
    XintfRegs.XTIMING2.bit.USEREADY = 0;
    XintfRegs.XTIMING2.bit.READYMODE = 0;
    XintfRegs.XTIMING2.bit.XSIZE = 3;   //16λ�ӿڣ�Ψһ��Ч������

    XintfRegs.XBANK.bit.BANK = 2;   //ʹ���ڲ�����6
    XintfRegs.XBANK.bit.BCYC = 3;  //��������֮�������3��XTIMCLK���� */


	//Zone6дʱ��
	XintfRegs.XTIMING6.bit.XWRLEAD = 3;    //д���������׶ε�����=XTIMCLK
	XintfRegs.XTIMING6.bit.XWRACTIVE = 7;   //д��������׶εȴ�״̬������=XTIMCLK
	XintfRegs.XTIMING6.bit.XWRTRAIL = 3;    //д�������ٽ׶ε�����=XTIMCLK
   //Zone6��ʱ��
	XintfRegs.XTIMING6.bit.XRDLEAD = 3;    //�����������׶ε�����=XTIMCLK
	XintfRegs.XTIMING6.bit.XRDACTIVE=7;	    //����������׶εȴ�״̬������=XTIMCLK
	XintfRegs.XTIMING6.bit.XRDTRAIL = 3;   //���������ٽ׶ε�����=XTIMCLK
	//���ӱ�ʱ��
	XintfRegs.XTIMING6.bit.X2TIMING = 0; 
   //Zone6����READY�źŲ���
    XintfRegs.XTIMING6.bit.USEREADY = 0;
    XintfRegs.XTIMING6.bit.READYMODE = 0;
    XintfRegs.XTIMING6.bit.XSIZE = 3;   //16λ�ӿڣ�Ψһ��Ч������

    XintfRegs.XBANK.bit.BANK = 6;   //ʹ���ڲ�����6
    XintfRegs.XBANK.bit.BCYC = 3;  //��������֮�������3��XTIMCLK����
	
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
   InitXintf();
		
   EALLOW;
   PieVectTable.T1PINT=&eva_timer1_isr;
   EDIS;  
  
   //���ж�
   IER |= M_INT2;
   PieCtrl.PIEIER2.bit.INTx4=1;
   EINT;  
   ERTM;

   EvaRegs.T1CON.bit.TENABLE=1;  // �򿪶�ʱ��1

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
