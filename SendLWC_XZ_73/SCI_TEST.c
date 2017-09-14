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

    SciaRegs.SCIFFTX.all=0xe040;    //�������,ʹ��FIFO
    SciaRegs.SCIFFRX.all=0x2021;    //ʹ��FIFO����
    SciaRegs.SCIFFCT.all=0x0000;    //��ֹ������У��
    SciaRegs.SCICCR.all = 0x27;     //1��ֹͣλ,��У��,��Ч���ݳ���8λ	
    SciaRegs.SCICTL1.all = 0x03;    //��λ
    SciaRegs.SCICTL2.all = 0x03;    //��λ  	
    SciaRegs.SCIHBAUD = 0x01;     //���ò�����9600bps
    SciaRegs.SCILBAUD = 0x86;	
    SciaRegs.SCICTL1.all = 0x23;   //ʹ�ܷ��ͽ��չ���  


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
        
  SysCtrlRegs.WDCR= 0x0068;   //��ֹ���Ź�
  SysCtrlRegs.PLLCR = 0x8;    //��Ƶ=30*8/2=120MHz  
  for(i=0;i<5000;i++);                 //Ϊ��Ƶ���ȶ���ʱһ��ʱ��      
  SysCtrlRegs.HISPCP.all = 0x0002;  //��������ʱ��=120/4=30MHz
  SysCtrlRegs.LOSPCP.all = 0x0002;	 //��������ʱ��=120/4=30MHz
 
  SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;   //ʹ��EVAģ��ĸ�������ʱ��
  SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;   //ʹ��EVBģ��ĸ�������ʱ��
  SysCtrlRegs.PCLKCR.bit.ADCENCLK=1;   //ʹ��ADCģ��ĸ�������ʱ��
  SysCtrlRegs.PCLKCR.bit.SCIENCLKA=1;  //ʹ��SCIAģ��ĸ�������ʱ��				
  EDIS;	
} 

//////////////Main/////////////////
void main(void)
{
  volatile int idata;
   /*ϵͳ��ʼ��*/
   InitSysCtrl();
   /*��InitFlash()�������Ƶ�ram������*/
   MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
   InitFlash();
   /*���ж�*/
   DINT; 
   IER = 0x0000;
   IFR = 0x0000;

   /*��ʼ��PIE�ж�*/
   InitPieCtrl();
   /*��ʼ��PIE�ж�ʸ����*/
   InitPieVectTable();
   init_eva_timer1();
   /*��ʼ��SCI�Ĵ���*/
   InitSci();
   /*�����жϷ��������ڵ�ַ*/
   EALLOW; 
   PieVectTable.T1PINT = &eva_timer1_isr;
   EDIS;
   
   /*��ʼ��GPIO*/
   EALLOW;   
   GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOA����ΪI/O�� 
   GpioMuxRegs.GPADIR.all=0x00c0;    //��Ϊ����	
   GpioDataRegs.GPADAT.all=0x0000;   /*��ʼ״̬��Ϊ�͵�ƽ*/ 
   EDIS; 

   /*���ж�*/
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
	    /*���I/0��״̬:A7A6A5A4
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
