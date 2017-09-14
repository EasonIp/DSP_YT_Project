 /* 
   * β�������������
   * �ļ�����XZ-62.pit
   * ʱ�䣺2011.1.1.10.27
   * �汾����1.01��
   
*/

#include "DSP28_Device.h"
#include "math.h"

#define RAMBASE1 0x0080000      //����汾���׵�ַ
#define RAMBASE2 0x0080005      //������ϴ����׵�ַ
#define RAMBASE3 0x0080009      //����״̬��Ϣ���׵�ַ
#define RAMBASE6 0x0100000      //���ߵ�ַ,��Ӧ��Ƭѡ�ź�XZCS6 
#define ID1 0x01200000       //����������͵��ҿ�����MSGID
#define ID2 0x01300000       //����������͵�������MSGID
#define ID3 0x02100000       //�ҿ��������͵��������MSGID
#define ID4 0x03100000       //���������͵��������MSGID
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


////////////////��������/////////////////////////////////
Uint32 i;                  //����
Uint16 channel;           //����һ���ɼ�ͨ������
int a[16];                //����һ���������ڱ���ADת���Ľ��
int n=0;
float jrwd;                //�ɼ��¶ȴ���������
float qxdl;                //�ɼ��������
Uint16 a1,a2;              //�ɼ�У����ѹ
Uint16 a3;     
float gain,offest;         //���������ƫ��ϵ��
Uint16 *rambase1,*rambase2,*rambase3; //�����ⲿ�ӿ�ָ��
Uint16 tmp,tmp1;             //�����ⲿ�ӿڱ���
Uint16 *rambase6;      
Uint16 CSH;               //429��ʼ��
Uint16 FSDZJ_271;         //����LABLE271���ֽ�
Uint16 FSGZJ_271;         //����LABLE271���ֽ�
Uint16 FSDZJ_272;
Uint16 FSGZJ_272; 
Uint16 FSDZJ_273;
Uint16 FSGZJ_273; 
Uint16 FSDZJ_300;          //��������汾��
Uint16 FSGZJ_300;
Uint16 FSDZJ_301;       
Uint16 FSGZJ_301;
Uint16 FSDZJ_302;
Uint16 FSGZJ_302;
Uint16 FSDZJ_303;
Uint16 FSGZJ_303;
Uint16 FSDZJ_304;           //����Ӳ���汾��
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
Uint16 JSDZJ;			     //���յ��ֽ�
Uint16 JSGZJ;			     //���ո��ֽ�
Uint32 gz=0;                   //���ϴ�����¼
char m=0;
char sd[10]={0,0,0,0,0,0,0,0,0,0};
char j=0;                      //�������ư�Ť��ѹ����
char count1=0;                //1s�������� 
char count2=0;                //0.5s��������CPLD���DSPʱ��
Uint16 count3=0;              //���ȼ��ʱ�����
char count4=0;                //ȱ����ʱ�����
char count5=0;                //�ȵ��¶ȼ��ʱ�����
char count6=0;                //4�����¶ȼ��ʱ�����
char count7=0;                 //3�����¶ȼ��ʱ�����
char count8=0;                //2�����¶ȼ��ʱ�����
char count9=0;                 //1�����¶ȼ��ʱ�����
char count10=0;                //�ж϶�·��ʱ����
int t1=250,t2=250;                   //����ʱ��
char k[32]={0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0
           };                //ÿ·�¶ȳ���ʱ�� 
Uint16 lz=0;                 //���յ������źŵĴ���
int error1=0;                 //δ�յ������źŵĴ����������Ϣ
float wd;                     //��������
Uint16 wd1=0;                 //���յ������¶��źŵĴ���
int error2=0;                 //δ�յ������¶��źŵĴ����������Ϣ
Uint16 can1=0,can2=0;
char EvaTimer1InterruptCount=0,EvaTimer2InterruptCount=0;                
char flag1;                   //��/�ҿ�������־
char flag2=0;                 //ADCУ����־flag2=1���
char flag3=0;                 //�ϵ��Լ�����                
char flag4=0;                  //����ά������
char flag5=0;                 //ȱ����0.5��ʱ��־flag5=1���
char flag6=0;                 //�ɻ��ڿ��б�־flag6=1
char flag6a=0;
char flag7=0;                  //��������������·������������־
char flag8=0;                  //���������һ����·������������־
char flag9=0;                  //�ϵ��Լ���ɱ�־
char flag10=0;                  //�����Լ���ɱ�־
char flag11=0;                  //������������
char flag12=0;                  //����������־flag12=1
char flag13=0;                  //��Ŧ��/���л���־
char flag14=0;                  //����ʱ�л���־
char flag15=0;                  //��ʼ����429��־flag15=1
char flag16=0;                  //274���ؽ��ձ�־flag16=1
char flag17=0;                    //274���ؽ����жϱ�־
char flag18=0;                  //213���½��ձ�־flag18=1
char flag19=0;                    //213���½����жϱ�־
char flag20=0;                  //��������<5
char flag21=0;                  //��·�жϱ�־flag21=1��·
char flag22=0;
char flag23=0;
Uint32 TestMbox1,TestMbox2;   //����2���յ�����Ϣ
Uint32 TestMbox3,TestMbox4;   //����3���յ�����Ϣ


/////////////////��������/////////////////////////////////

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
   GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOA����ΪI/O�� 
   GpioMuxRegs.GPADIR.all=0xFFFF;   //A0-A15����Ϊ���	
   GpioDataRegs.GPADAT.all=0x23f; 

   GpioMuxRegs.GPBMUX.all=0x0000;    //GPIOB����ΪI/O�� 
   GpioMuxRegs.GPBDIR.all=0xFFDF;    //B5����Ϊ���룬��������Ϊ���	
   GpioDataRegs.GPBDAT.all=0x8010;   
     
   GpioMuxRegs.GPFMUX.all=0x00c0;  //����GPIOF6/7ΪeCAN��,��������ΪI/O��
   GpioMuxRegs.GPFDIR.bit.GPIOF9=1;
   GpioMuxRegs.GPFDIR.bit.GPIOF11=1;
   GpioMuxRegs.GPFDIR.bit.GPIOF12=0;
   GpioMuxRegs.GPFDIR.bit.GPIOF13=0;
   GpioDataRegs.GPFDAT.bit.GPIOF9=1;
   GpioDataRegs.GPFDAT.bit.GPIOF11=0;
   EDIS;

}	

///////////////////�ⲿ�ж�13��ʼ��//////////////////////////////////

void InitXINT13(void)
{  GpioMuxRegs.GPEMUX.bit.XNMI_XINT13_GPIOE2=1; //�����ж�13���� 
   XIntruptRegs.XNMICR.all=0x2;	 //�½��������ж�
}

///////////////////�ⲿ�ж�1��ʼ��//////////////////////////////////

void InitXINT1(void)
{  GpioMuxRegs.GPEMUX.bit.XINT1_XBIO_GPIOE0=1; //�������ж�1���� 
   XIntruptRegs.XINT1CR.all=0x5;      //�����������ж�
}

///////////////////�ⲿ�ж�2��ʼ��//////////////////////////////////

void InitXINT2(void)
{  GpioMuxRegs.GPEMUX.bit.XINT2_ADCSOC_GPIOE1=1; //�����ⲿ�ж�13���� 
   XIntruptRegs.XINT2CR.all=0x5;      //�����������ж�
}

///////////////////PDPINTB�жϳ�ʼ��//////////////////////////////////

void InitPDPINTB(void)
{  GpioMuxRegs.GPDMUX.bit.T3CTRIP_PDPB_GPIOD5=1; //��Ϲ���
   EvbRegs.EVBIMRA.bit.PDPINTB=1;   //���PDPINTB��
   EvbRegs.EVBIFRA.bit.PDPINTB=1;   //���PDPINTB�жϱ�־              
}

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


///////////////////��ʱ��2��ʼ����100mS��//////////////////////////////////

void init_eva_timer2(void)
{
  
    EvaRegs.GPTCONA.all = 0;   //��ʼ�������2�����ö�ʱ��2�Ĵ���
    EvaRegs.EVAIMRB.bit.T2PINT = 1;  //����ʱ��2�����ж�
    EvaRegs.EVAIFRB.bit.T2PINT = 1;  //�����ʱ��2�����жϱ�־
             
    EvaRegs.T2CON.all = 0x170C;  //Timer2��������ģʽ��Ԥ��ƵֵΪ128��
	                             //ʹ���ڲ���������ʱ��
	EvaRegs.T2PR = 0x5b8d;   //Timer2�����ڼĴ���=0.1*30MHz/128
	EvaRegs.T2CNT = 0x0000;  //Timer2�ļ�������

}

///////////////////AD��ʼ��//////////////////////////////////
void InitAdc(void)
{
		
	AdcRegs.ADCTRL1.bit.RESET=1;  //��λADCģ��
	for(i=0;i<5000;i++)	 {} 
	AdcRegs.ADCTRL1.bit.RESET=0;
	AdcRegs.ADCTRL1.bit.SUSMOD=3;
	AdcRegs.ADCTRL1.bit.ACQ_PS=0;
	AdcRegs.ADCTRL1.bit.CPS=0;     
	AdcRegs.ADCTRL1.bit.CONT_RUN=0; //ADΪ����/ֹͣģʽ
	AdcRegs.ADCTRL1.bit.SEQ_CASC=1; //16����ģʽ
     	
	AdcRegs.ADCTRL3.bit.ADCBGRFDN=3; //����϶�ο���·�ϵ�
	for(i=0;i<10000;i++) {}           //�ȴ��ȶ�
	AdcRegs.ADCTRL3.bit.ADCPWDN=1;  //������·�ϵ�
	for(i=0;i<5000;i++)	 {}           //�ȴ��ȶ�
	AdcRegs.ADCTRL3.bit.ADCCLKPS=15;  
	AdcRegs.ADCTRL3.bit.SMODE_SEL=0;  //˳�����

	
	AdcRegs.MAX_CONV.bit.MAX_CONV=15;  //����ͨ��Ϊ16
  
    AdcRegs.CHSELSEQ1.all= channel;   //ͬһ��ͨ����16��
    AdcRegs.CHSELSEQ2.all= channel;
    AdcRegs.CHSELSEQ3.all= channel;
    AdcRegs.CHSELSEQ4.all= channel;

    AdcRegs.ADCTRL2.all=0x8c04;

	////////////���ö�ʱ��4����ADת��/////////////////////
    EvbRegs.GPTCONB.all=0x400;    //��ʱ��4�����ж�����ADת��
    EvbRegs.EVBIMRB.bit.T4PINT = 1;  //����ʱ��4�����ж�
    EvbRegs.EVBIFRB.bit.T4PINT = 1;  //�����ʱ��4�����жϱ�־
             
    EvbRegs.T4CON.all = 0x170C;  //Timer4Ϊ����������ģʽ��Ԥ��ƵֵΪ128��
	                             //ʹ���ڲ���������ʱ��
	EvbRegs.T4PR = 0xea;     //Timer4�����ڼĴ���=0.001*30MHz/128
	EvbRegs.T4CNT = 0x0000;  //Timer4�ļ���������
				
	AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;
	AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;	
			
}	
	
////////////////////�ⲿ�ӿڳ�ʼ��/////////////////////////
void InitXintf(void)
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

////////////////////ECAN��ʼ��/////////////////////////
void InitECan(void)
{
	EALLOW;
	ECanaRegs.CANTIOC.bit.TXFUNC = 1;
	ECanaRegs.CANRIOC.bit.RXFUNC = 1;
	EDIS;
		
	ECanaRegs.CANME.all = 0;  //��ֹ��������

	if(flag1==0)
	{ ECanaRegs.CANMD.bit.MD0=0;  //����0���͵��ҿ�����
      ECanaRegs.CANMD.bit.MD1=0;  //����1���͵�������
      ECanaRegs.CANMD.bit.MD2=1;  //����2�����ҿ�������Ϣ
      ECanaRegs.CANMD.bit.MD3=1;  //����3������������Ϣ
     	
	  ECanaMboxes.MBOX0.MID.all = ID1;	
	  ECanaMboxes.MBOX1.MID.all = ID2;
	  ECanaMboxes.MBOX2.MID.all = ID3;
	  ECanaMboxes.MBOX3.MID.all = ID4;
	}
    else
	{ ECanaRegs.CANMD.bit.MD0=0;  //����0���͵��������
      ECanaRegs.CANMD.bit.MD2=1;  //����2�������������Ϣ
    	
	  ECanaMboxes.MBOX0.MID.all = ID3;	
	  ECanaMboxes.MBOX2.MID.all = ID1;
	}
			
	ECanaRegs.CANME.bit.ME0=1;	//����ʹ��
    ECanaRegs.CANME.bit.ME1=1;
    ECanaRegs.CANME.bit.ME2=1;
	ECanaRegs.CANME.bit.ME3=1;
		
	ECanaMboxes.MBOX0.MCF.bit.DLC = 8;	//���ݳ��� 8��BYTE
	ECanaMboxes.MBOX1.MCF.bit.DLC = 8;
	ECanaMboxes.MBOX2.MCF.bit.DLC = 8;
	ECanaMboxes.MBOX3.MCF.bit.DLC = 8;
	
	ECanaMboxes.MBOX0.MCF.bit.RTR = 0;	
	ECanaMboxes.MBOX1.MCF.bit.RTR = 0;  
	ECanaMboxes.MBOX2.MCF.bit.RTR = 0;	
	ECanaMboxes.MBOX3.MCF.bit.RTR = 0; 

	EALLOW;	
	ECanaRegs.CANMIM.all = 0xFFFFFFFF;	//��������ʹ��
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
	ECanaRegs.CANMC.bit.STM = 0; //��������ģʽ 
	ECanaRegs.CANMC.bit.SCM = 0; //sccģʽ
    ECanaRegs.CANMC.bit.DBO = 0; //���Ƚ��ջ������ݵ������Ч�ֽ�
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

///////////////���������뽻��///////////////////////////////////
void MBOX0Send(void)
{  
    ECanaMboxes.MBOX0.MDRL.all = ((Uint32)FSGZJ_271<<16)+(Uint32)FSDZJ_271;
	ECanaMboxes.MBOX0.MDRH.all = 0x444d3143;
	ECanaRegs.CANTRS.bit.TRS0=1;   //��������	
    for(i=0;i<0xffff;i++) {}
    ECanaRegs.CANTA.bit.TA0=1;
    ECanaRegs.CANTRS.bit.TRS0=0;
	for(i=0;i<0xffff;i++) {}

    ECanaMboxes.MBOX0.MDRL.all = ((Uint32)FSGZJ_272<<16)+(Uint32)FSDZJ_272;
	ECanaMboxes.MBOX0.MDRH.all = 0x444d3243;
	ECanaRegs.CANTRS.bit.TRS0=1;   //��������	
	for(i=0;i<0xffff;i++) {}
    ECanaRegs.CANTA.bit.TA0=1;
    ECanaRegs.CANTRS.bit.TRS0=0;
    for(i=0;i<0xffff;i++){}
 
    ECanaMboxes.MBOX0.MDRL.all = ((Uint32)FSGZJ_273<<16)+(Uint32)FSDZJ_273;
	ECanaMboxes.MBOX0.MDRH.all = 0x444d3343;
	ECanaRegs.CANTRS.bit.TRS0=1;   //��������
	for(i=0;i<0xffff;i++) {}
    ECanaRegs.CANTA.bit.TA0=1;
    ECanaRegs.CANTRS.bit.TRS0=0;
    for(i=0;i<0xffff;i++){}

    if((JSDZJ&0xff)==0x8b)
    { ECanaMboxes.MBOX0.MDRL.all = ((Uint32)JSGZJ<<16)+(Uint32)JSDZJ;
      ECanaMboxes.MBOX0.MDRH.all = 0x444d3443;
	  ECanaRegs.CANTRS.bit.TRS0=1;   //������	
      for(i=0;i<0xffff;i++) {}
      ECanaRegs.CANTA.bit.TA0=1;
      ECanaRegs.CANTRS.bit.TRS0=0;
    }
	can1++;
}

///////////////���������͵�������///////////////////////////////////
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

/////////////ͨ���������ⲿ�洢��дӲ���汾��////////////////////
void saveDA1(void)
{  
   *rambase1=tmp&0xff; 	
   rambase1++;    
}

/////////////ͨ���������ⲿ�洢��д�ܹ��ϴ��� ////////////////////
void saveDA2(void)
{  
   *rambase2=tmp&0xff; 	
   rambase2++;    
}

/////////////ͨ���������ⲿ�洢��д״̬��Ϣ��////////////////////
void saveDA3(void)
{  
   *rambase3=tmp&0xff; 	
   rambase3++;    
}

/////////////ͨ�����߶�ȡ�ϴι��ϴ���////////////////////
void readDA1(void)
{ 
   tmp1=(*rambase1)&0xff;       //�����������ϵ����ݶ�������tmp1��
   rambase1++;   
}

/////////////ͨ�����߶�ȡ�ϴι��ϴ���////////////////////
void readDA2(void)
{ 
   tmp1=(*rambase2)&0xff;       //�����������ϵ����ݶ�������tmp��
   rambase2++;   
}

/////////////ͨ�����߶�ȡ�ϴ�״̬��Ϣ�������////////////////////
void readDA3(void)
{ 
   tmp1=(*rambase3)&0xff;       //�����������ϵ����ݶ�������tmp�� 
   rambase3++; 
}

//////////////����洢������/////////////////////////////
void QCdata(void)
{
  rambase3=(Uint16 *)RAMBASE3;
  rambase6=(Uint16 *)RAMBASE6;
  for(;rambase3<rambase6;rambase3++)
  { *rambase3=0xff;
  } 
  
}

////////////ADCУ��////////////////////////////////////

void ADC_adjust(void)
{    
   ///////�ɼ�ADCINB6(2.5V)/////////
   channel=0xeded; 
   ADC_sample();
   a1=a3;      
   ///////�ɼ�ADCINB7(1.25V)///////////   
   channel=0xfdfd; 
   ADC_sample();  
   a2=a3;       
   //////////���������ƫ��/////////////////
   gain=(float)(HIGH_COUNT-LOW_COUNT)/(a1-a2); 
   offest=a2*gain-1733.0;
   flag2=1;     //ADCУ�����
}

///////////////���������////////////////////////////////
void DMJC(void)
{ /////////���洢����д�Ƿ�����//////////////////////  
   tmp=0x11;
   *rambase1=tmp;
   for(i=0;i<10000;i++){}
   tmp1=*rambase1;
   rambase1=(Uint16 *)RAMBASE1; 
   if(tmp1!=tmp)  //�洢������
   { GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU���������ź�  
     gz++;   
   }
   /////////��鶨ʱ���Ƿ�����//////////////////////////
   EvaRegs.T1CON.bit.TENABLE=1;   //�򿪶�ʱ��
   EvaRegs.T2CON.bit.TENABLE=1;
   while(1)         
   { if(EvaTimer2InterruptCount==10)  //��ʱ1S
     break;	 
   }
   EvaRegs.T1CON.bit.TENABLE=0;   //�رն�ʱ��
   EvaRegs.T2CON.bit.TENABLE=0;
   if(EvaTimer1InterruptCount!=10)  //��ʱ������
   { GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU���������ź�
     gz++; 
   }
   EvaTimer1InterruptCount=0;
   EvaTimer2InterruptCount=0;

}

/////////////////�ȵ���·���������///////////////////////
void redao_DLJC(void)
{  GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0xa; //������ȵ�
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))  //�ϵ��Լ�����ά��ʱ
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[0]++;
	 }
     if(sd[0]==2)
	 { sd[0]=0;
	   FSGZJ_272=FSGZJ_272|0x1;    //��1/2���й���
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU���������ź�
	 }
   } 
   
   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all|0xf;  //�һ����ȵ�
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))
   {  if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[1]++;
	 }
     if(sd[1]==2)
	 { sd[1]=0;	 
	   FSDZJ_272=FSDZJ_272|0x8000;  //��1/2���й���
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU���������ź�
	 }
   }
 
}

/////////////////1�Ŷζ�·���������///////////////////////
void abc1_DLJC(void)
{  GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0x6; //�����1�Ŷ�
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[2]++;
	 }
     if(sd[2]==2)
	 { sd[2]=0;
	   FSGZJ_272=FSGZJ_272|0x1;    //��1/2���й���
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU���������ź�
	 }
   }
   
   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0xb; //�һ���1�Ŷ�
   for(i=0;i<0x5000;i++) {}  
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[3]++;
	 }
     if(sd[3]==2)
	 { sd[3]=0;
	   FSDZJ_272=FSDZJ_272|0x8000;  //��1/2���й���
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU���������ź�
	 }
   }

}

/////////////////2�Ŷζ�·���������///////////////////////  
void abc2_DLJC(void) 
{  GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0x7;  //�����2�Ŷ�
   for(i=0;i<0x5000;i++) {}  
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[4]++;
	 }
     if(sd[4]==2)
	 { sd[4]=0;
	   FSGZJ_272=FSGZJ_272|0x1;    //��1/2���й���
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU���������ź�
	 }
   }
      
   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0xc; //�һ���2�Ŷ�
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[5]++;
	 }
     if(sd[5]==2)
	 { sd[5]=0;
	   FSDZJ_272=FSDZJ_272|0x8000;  //��1/2���й���
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU���������ź�
	 }
   }
}

/////////////////3�Ŷζ�·���������///////////////////////  
void abc3_DLJC(void)
{  GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0x8;  //�����3�Ŷ�
   for(i=0;i<0x5000;i++) {}   
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[6]++;
	 }
     if(sd[6]==2)
	 { sd[6]=0; 
	   FSGZJ_272=FSGZJ_272|0x1;    //��1/2���й���
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU���������ź�
	 }
   }

   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0xd;    //�һ���3�Ŷ�
   for(i=0;i<0x5000;i++) {}  
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[7]++;
	 }
     if(sd[7]==2)
	 { sd[7]=0; 
	   FSDZJ_272=FSDZJ_272|0x8000;  //��1/2���й���
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU���������ź�
	 }
   }

}

/////////////////4�Ŷζ�·���������///////////////////////  
void abc4_DLJC(void)
{  GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0x9;    //�����4�Ŷ�
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[8]++;
	 }
     if(sd[8]==2)
	 { sd[8]=0; 
	   FSGZJ_272=FSGZJ_272|0x1;    //��1/2���й���
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU���������ź�
	 }
   }

   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0xe; //�һ���4�Ŷ�
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[9]++;
	 }
     if(sd[9]==2)
	 { sd[9]=0;
	   FSDZJ_272=FSDZJ_272|0x8000;  //��1/2���й���
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU�������ź�
	 }
   }
}

/////////////�ϵ��Լ�//////////////////////////////////

void poweron_BIT(void)
{  
   flag9=1;
   FSDZJ_273=FSDZJ_273|0x2000;     //�����Լ�
   DMJC();

   //////////����·�������Ƿ�����///////////////
   /*GpioDataRegs.GPFDAT.bit.GPIOF9=0;  //��·�������Լ�
   GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //�򿪶�·����ͨ��ѡ��
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
   GpioDataRegs.GPFDAT.bit.GPIOF9=1;  //�ر��Լ��Դ*/

   if(GpioDataRegs.GPBDAT.bit.GPIOB15==0)
   { flag3=1;
   }
   FSDZJ_273=FSDZJ_273&0xdfff;   // �Լ���� 
   flag9=0;      

}

////////////////����ά��BIT////////////////////////////
void DMWH_BIT(void)
{  flag10=1;
   FSDZJ_273=FSDZJ_273|0x2000;     //�����Լ�  
   DMJC();

   for(i=0;i<40000;i++){}
   /*if(GpioDataRegs.GPFDAT.bit.GPIOF12==1) 
   { EvaRegs.T1CON.bit.TENABLE=1;  // �򿪶�ʱ��1
     GpioDataRegs.GPADAT.bit.GPIOA9=0;   //�򿪰�ȫ����
     GpioDataRegs.GPADAT.bit.GPIOA15=1;   //�ȵ�����
     GpioDataRegs.GPADAT.bit.GPIOA14=1;   //4������,ʱ��1      
     GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //�򿪶�·���Ե����
	 for(i=0;i<0xffff;i++){}
     redao_DLJC();     
     //redao_temperature();
     abc4_DLJC();
     //abc4_temperature();
     //QXJC();  
     
      ////////////����ά������ʱ��//////////////////////////////
     if(EvaTimer1InterruptCount==8)          //ʱ��2              
     { GpioDataRegs.GPADAT.bit.GPIOA14=0; 
       GpioDataRegs.GPADAT.bit.GPIOA13=1;
       abc3_DLJC();
	   //QXJC();
       //abc3_temperature();
     }
     if(EvaTimer1InterruptCount==16)         //ʱ��3             
     { GpioDataRegs.GPADAT.bit.GPIOA13=0; 
       GpioDataRegs.GPADAT.bit.GPIOA12=1; 
	   abc2_DLJC();
	   //QXJC();
       //abc2_temperature();
     }
     if(EvaTimer1InterruptCount==24)          //ʱ��4             
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
	 GpioDataRegs.GPBDAT.bit.GPIOB4=1;  //�رն�·����ͨ��ѡ�� 
     EvaRegs.T1CON.bit.TENABLE=0; 
   }*/
   
   if(GpioDataRegs.GPBDAT.bit.GPIOB15==0)
   { flag4=1;
   }
   FSDZJ_273=FSDZJ_273&0xdfff;   // �Լ���� 
   flag10=0; 
   arinc429fasong(); //429�ϱ��Լ���Ϣ
   MBOX0Send();  

}


/////////////429����////////////////////////////////
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

///////////////���ȿ���////////////////////////////////////
void jiare(void)
{  if(flag13==0)
   { GpioDataRegs.GPADAT.bit.GPIOA9=0;   //�򿪰�ȫ���� 
	 FSGZJ_271=FSGZJ_271|0x2000; //β�������ϵͳ��
	  ///////��ʱ2s////////////////////////////
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
     GpioDataRegs.GPADAT.bit.GPIOA15=1;   //�ȵ�����
	 FSDZJ_272=FSDZJ_272|0x2000;   //���ڽ��м���
   }

    ////////////����ʱ��//////////////////////////////
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
      GpioDataRegs.GPBDAT.bit.GPIOB4=1;  //�رն�·����ͨ��ѡ��
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
    GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //�򿪶�·����ͨ��ѡ��

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

//////////////�źŲɼ�///////////////////////////////
void ADC_sample(void)
{  InitAdc(); 
   EvbRegs.T4CON.bit.TENABLE=1;
   while(1)
   {if(n==8)
	break;
   } 
   n=0;	 
   if(flag2==1) //�ϵ��Լ���������ɼ���ѹ
   { a3=(int)(gain*a3-offest);
   }		
   EvbRegs.T4CON.bit.TENABLE=0;
}  

/////////////////�ȵ��¶ȼ��//////////////////////////////
/*void redao_temperature(void)
{ 
    GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1cc0;   //��1�ȵ�      
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095; 
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[3]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x400;     //��1�γ���
	  gz++; 
	  k[3]=k[3]+1;                     
	  if(k[3]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	  gz++; 
	  FSGZJ_272=FSGZJ_272|0x400;     //��1�γ���
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1dc0;   //��2�ȵ�     
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[7]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x200;     //��2�γ���
	  gz++; 
	  k[7]=k[7]+1;                      //��¼����ʱ��
	  if(k[7]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x200;     //��2�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x16c0;   //��3�ȵ�
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ������ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[11]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x100;     //��3�γ���
	  gz++; 
	  k[11]=k[11]+1;                      //��¼����ʱ��
	  if(k[11]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x100;     //��3�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x17c0;   //��4�ȵ�
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[15]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x80;     //��4�γ���
	  gz++; 
	  k[15]=k[15]+1;                      //��¼����ʱ��
	  if(k[15]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x80;     //��4�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1ac0;   //��1�ȵ� 
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[19]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x40;     //��1�γ���
	  gz++; 
	  k[19]=k[19]+1;                      //��¼����ʱ��
	  if(k[19]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x40;     //��1�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1bc0;   //��2�ȵ�
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[23]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x20;     //��2�γ���
	  gz++; 
	  k[23]=k[23]+1;                      //��¼����ʱ��
	  if(k[23]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x20;     //��2�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xec0;   //��3�ȵ� 
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[27]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x10;     //��3�γ���
	  gz++; 
	  k[27]=k[27]+1;                      //��¼���ʱ��
	  if(k[27]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x10;     //��3�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xfc0;   //��4�ȵ� 
    channel=0x8d8d;       //ADCINB0�ɼ�¶ȫ�����ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[31]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x8;     //��4�γ���
	  gz++; 
	  k[31]=k[15]+1;                      //��¼����ʱ��
	  if(k[31]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x8;     //��4�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}
}

/////////////////��/��4abc�¶ȼ��//////////////////////////////
void abc4_temperature(void)
{   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1700;   //��4a
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[12]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x80;     //��4�γ���
	  gz++; 
	  k[12]=k[12]+1;                      //��¼����ʱ��
	  if(k[12]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x80;     //��4�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1740;   //��4b
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[13]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x80;     //��4�γ���
	  gz++; 
	  k[13]=k[13]+1;                      //��¼����ʱ��
	  if(k[13]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x80;     //��4�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1780;   //��4c 
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();	
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[14]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x80;     //��4�γ���
	  gz++; 
	  k[14]=k[14]+1;                      //��¼����ʱ��
	  if(k[14]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x80;     //��4�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xf00;   //��4a 
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[28]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x8;     //��4�γ���
	  gz++; 
	  k[28]=k[28]+1;                      //��¼����ʱ��
	  if(k[28]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x8;     //��4�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xf40;   //��4b  
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[29]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x8;     //��4�γ���
	  gz++; 
	  k[29]=k[29]+1;                      //��¼����ʱ��
	  if(k[29]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x8;     //��4�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xf80;   //��4c 
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[23]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x8;     //��4�γ���
	  gz++; 
	  k[31]=k[31]+1;                      //��¼����ʱ��
	  if(k[31]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x8;     //��4�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	 
	}
}

/////////////////��/��3abc�¶ȼ��//////////////////////////////
void abc3_temperature(void)
{   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1600;   //��3a  
    channel=0x8d8d;       //ADCINB0�ɼ���ȸ����ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[8]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x100;     //��3�γ���
	  gz++; 
	  k[8]=k[8]+1;                      //��¼����ʱ��
	  if(k[8]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x100;     //��3�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1640;   //��3b 
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[9]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x100;     //��3�γ���
	  gz++; 
	  k[9]=k[9]+1;                      //��¼����ʱ��
	  if(k[9]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x100;     //��3�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1680;   //��3c 
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[10]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x100;     //��3�γ���
	  gz++; 
	  k[10]=k[10]+1;                      //��¼����ʱ��
	  if(k[10]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x100;     //��3�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xe00;   //��3a 
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[24]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x10;     //��3�ά��
	  gz++; 
	  k[24]=k[24]+1;                      //��¼����ʱ��
	  if(k[24]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x10;     //��3�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xe40;   //��3b 
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[25]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x10;     //��3�γ���
	  gz++; 
	  k[25]=k[25]+1;                      //��¼����ʱ��
	  if(k[25]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x10;     //��3�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU���	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xe80;   //��3c 
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[26]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x10;     //��3�γ���
	  gz++; 
	  k[26]=k[26]+1;                      //��¼����ʱ��
	  if(k[26]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x10;     //��3�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}
}

/////////////////��/��2abc�¶ȼ��//////////////////////////////
void abc2_temperature(void)
{   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1d00;   //��2a 
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[4]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x200;     //��2�γ���
	  gz++; 
	  k[4]=k[4]+1;                      //��¼����ʱ��
	  if(k[4]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x200;     //��2�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1d40;   //��2b 
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[5]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x200;     //��2�ά��
	  gz++; 
	  k[5]=k[5]+1;                      //��¼����ʱ��
	  if(k[5]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x200;     //��2γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1d80;   //��2c 
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[6]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x200;     //��2�γ���
	  gz++; 
	  k[6]=k[6]+1;                      //��¼����ʱ��
	  if(k[6]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x200;     //��2�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1b00;   //��2a 
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[20]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x20;     //��2�γ���
	  gz++; 
	  k[20]=k[20]+1;                      //��¼����ʱ��
	  if(k[20]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x20;     //��2�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1b40;   //��2b  
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[21]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x20;     //��2�γ���
	  gz++; 
	  k[21]=k[21]+1;                      //��¼����ʱ��
	  if(k[21]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x20;     //2�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1b80;   //��2c 
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[22]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x20;     //��2�γ���
	  gz++; 
	  k[22]=k[22]+1;                      //��¼����ʱ��
	  if(k[22]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x20;     //��2�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}
}

/////////////////��/��1abc�¶ȼ��//////////////////////////////
void abc1_temperature(void)
{ 
    GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1c00;   //��1a 
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[0]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x400;     //��1�γ���
	  gz++; 
	  k[0]=k[0]+1;                      //�ǳ�����
	  if(k[0]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x400;     //��1�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}
    
	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1c40;   //��1b
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[1]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x400;     //��1�γ���
	  gz++; 
	  k[1]=k[1]+1;                      //��¼����ʱ��
	  if(k[1]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x400;     //��1�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}
	
	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1c80;   //��1c
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[2]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x400;     //��1�γ���
	  gz++; 
	  k[2]=k[2]+1;                      //��¼����ʱ��
	  if(k[2]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x400;     //��1�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

    GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1a00;   //��1a  
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[20]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x40;     //��1�γ���
	  gz++; 
	  k[20]=k[20]+1;                      //��¼����ʱ��
	  if(k[20]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x40;     //��1�γ��� 
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1a40;   //��1b 
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ���
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[21]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x40;     //��1�γ���
	  gz++; 
	  k[21]=k[21]+1;                      //��¼����ʱ��
	  if(k[21]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x40;     //��1�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1a80;   //��1c
    channel=0x8d8d;       //ADCINB0�ɼ��¶ȴ�������ѹ�ź�
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[22]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x40;     //��1γ���
	  gz++; 
	  k[22]=k[22]+1;                      //��¼����ʱ��
	  if(k[22]>24)                    //����25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x40;     //��1�γ���
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU����	  
	}
}*/

//////////////��ƽβ���ȵ����ж��Ƿ�·///////////////////
void ZDLJC(void)
{ 
  if((qxdl<15.0)||(qxdl>58.0))
  { 
	GpioDataRegs.GPADAT.bit.GPIOA9=1;
	if(GpioDataRegs.GPADAT.bit.GPIOA14==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x200;         //��4�ζ�·
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA13==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x400;         //���3�ζ�·
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA12==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x800;         //���2�ζ�·
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA11==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x1000;         //���1�ζ�·
	}
	GpioDataRegs.GPBDAT.bit.GPIOB15=0;    //������������
	GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //�ر����м���
  } 
}

//////////////��ƽβ���ȵ����ж��Ƿ�·///////////////////
void YDLJC(void)
{  
  if((qxdl<15.0)||(qxdl>58.0))
  { 
	GpioDataRegs.GPADAT.bit.GPIOA9=1;
	if(GpioDataRegs.GPADAT.bit.GPIOA14==1)
    { gz++; 
	  FSGZJ_271=FSGZJ_271|0x20;         //�ҵ�4�ζ�·
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA13==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x40;         //�ҵ�3�ζ�·
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA12==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x80;         //�ҵ�2�ζ�·
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA11==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x100;         //�ҵ�1�ζ�·	 
	}
    GpioDataRegs.GPBDAT.bit.GPIOB15=0;    //������������
	GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //�ر����м���	
  }
}

///////////////////��ƽβ��·���//////////////////////////////
void ZKLJC(void)
{  
   channel=0x0d0d;   //ADCINA0�ɼ���t
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5;   //�ɼ���ѹת���ɵ���
   ZDLJC();
    
   channel=0x1d1d;   //ADCINA1�ɼ���s
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5;
   ZDLJC();
     
   channel=0x2d2d;   //ADCINA2�ɼ���r
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5;
   ZDLJC();    
}

///////////////////��ƽβ��·���//////////////////////////////
void YKLJC(void)
{ 	 
   channel=0x3d3d;   //ADCINA3�ɼ���t
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5;
   YDLJC();

   channel=0x4d4d;   //ADCINA4�ɼ���s
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5; 
   YDLJC();

   channel=0x5d5d;   //ADCINA5�ɼ���r
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5; 
   YDLJC();   
}

///////////////��/�ҿ�·ȱ����////////////////////////////////
void QXJC(void)
{
   ZKLJC();
   if(GpioDataRegs.GPBDAT.bit.GPIOB15==0)
   { FSGZJ_272=FSGZJ_272|0x1;    //�����й���	     
   }
   YKLJC();
   if(GpioDataRegs.GPBDAT.bit.GPIOB15==0)
   { FSDZJ_272=FSDZJ_272|0x8000;  //�����й���
   }
   if(flag7==1)
   { if((FSGZJ_272&0x1==0x1)&&(FSDZJ_272&0x8000==0x0000)) //�����������
     { FSGZJ_272=FSGZJ_272|0x1;    //�����й���	
	 }
	 if((FSGZJ_272&0x1==0x0000)&&(FSDZJ_272&0x8000==0x8000)) //�������ҹ���
	 { if(flag1==0)
	   { FSGZJ_272=FSGZJ_272|0x2;    //2#��Դ����
	   }
	   else
	   { FSGZJ_272=FSGZJ_272|0x4;    //1#��Դ����
	   }
	 }
	 else
	 { FSGZJ_271=FSGZJ_271|0x4000;   //������ϵͳʧЧ
	 }
   }
   if(flag8==1)
   { if((FSGZJ_272&0x1==0x0000)&&(FSDZJ_272&0x8000==0x8000)) //�������ҹ���
     { FSDZJ_272=FSDZJ_272|0x8000;  //�����й���
	 }
     if((FSGZJ_272&0x1==0x1)&&(FSDZJ_272&0x8000==0x0000)) //�����������
	 { if(flag1==0)
	   { FSGZJ_272=FSGZJ_272|0x4;    //1#�����
	   }
	   else
	   { FSGZJ_272=FSGZJ_272|0x2;    //2#��Դ����
	   }
	 }
	 else
	 { FSGZJ_271=FSGZJ_271|0x4000;   //������ϵͳʧЧ
	 }
   }
}

///////////////////��ʱ��1�ж�//////////////////////////////////

interrupt void eva_timer1_isr(void)
{  
  DINT;
  if(PieCtrl.PIEACK.bit.ACK2==1)
  { PieCtrl.PIEACK.bit.ACK2=1;
   
   /////////////////////////////////////////////////
   if((flag9==1)|(flag10==1))    //�ϵ��Լ������Լ�ʱ 
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
          { FSGZJ_271=FSGZJ_271|0x1e;       //���4��ȫ����·
	        gz++;	     
          }
	      if(GpioDataRegs.GPADAT.bit.GPIOA15==1)
          { FSGZJ_271=FSGZJ_271|0x1;	   //�ұ�4��ȫ����·
	        FSDZJ_271=FSDZJ_271|0xe000;       
	        gz++; 
          }
	      if(GpioDataRegs.GPADAT.bit.GPIOA11==1)
          {  FSGZJ_271=FSGZJ_271|0x10;     //���1�ζ�·
	         gz++; 
          }
          if(GpioDataRegs.GPADAT.bit.GPIOA11==1)
          {  FSGZJ_271=FSGZJ_271|0x1;     //��1�ζ�·
	         gz++; 
          }
          if(GpioDataRegs.GPADAT.bit.GPIOA12==1)
          {   FSGZJ_271=FSGZJ_271|0x8;     //���2�ζ�·
	          gz++; 
	      } 
	      if(GpioDataRegs.GPADAT.bit.GPIOA12==1)
	      { FSDZJ_271=FSDZJ_271|0x8000;     //�ҵ�2�ζ�·
		    gz++; 
	      }
	      if(GpioDataRegs.GPADAT.bit.GPIOA13==1)
	      { FSGZJ_271=FSGZJ_271|0x4;     //���3�ζ�·
		    gz++;
	      } 
	      if(GpioDataRegs.GPADAT.bit.GPIOA13==1)
	      { FSDZJ_271=FSDZJ_271|0x4000;     //�ҵ�3�ζ�·
		    gz++;
	      }
	      if(GpioDataRegs.GPADAT.bit.GPIOA14==1)
	      { FSGZJ_271=FSGZJ_271|0x1;     //���4�ζ�·
		    gz++; 
	      } 
	      if(GpioDataRegs.GPADAT.bit.GPIOA14==1)
	      { FSDZJ_271=FSDZJ_271|0x2000;     //�ҵ�4�ζ�·
		    gz++; 
	      }	 
          GpioDataRegs.GPADAT.bit.GPIOA9=1;		  
          GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //�ر����м���
	      //GpioDataRegs.GPBDAT.bit.GPIOB4=1;
		  GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU���������ź�
         }
		 else
		 {flag21=0;
		 }
	   }
	 }

     ///////////////////////////////////////////////////
     if(GpioDataRegs.GPFDAT.bit.GPIOF12==1) //��������
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
	       GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //�ر����м���
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
	   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff; //�ر����м���
	   FSGZJ_271=FSGZJ_271&0xdfff; //β�������ϵͳ�ر�
	   FSDZJ_272=FSDZJ_272&0xdfff;  //û�н��м��ȹ���
	   if(flag12==0)
	   { m=0;
	   }
	   else
	   { flag13=1;   //׼���л�
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

///////////////////��ʱ��2�ж�//////////////////////////////////

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

///////////////////ADC�ж�//////////////////////////////////
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
	for(n=0;n<8;n++)  //ȡ8����Ч������ƽ��
	{ a3=a3+a[2*n+1];
	}
    a3=a3/8;

    AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;
    AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;
  }	
  EINT;
	
}

///////////////////ECAN�ж������Ϣ///////////////////////////////
interrupt void ECAN0_ISR(void)
{ 
  DINT;
  if(PieCtrl.PIEACK.bit.ACK9==1)
  { PieCtrl.PIEACK.bit.ACK9=1;

   	if(ECanaRegs.CANRMP.bit.RMP2==1) //������һ��������Ϣ
    { TestMbox1=ECanaMboxes.MBOX2.MDRL.all;
      TestMbox2=ECanaMboxes.MBOX2.MDRH.all;
      if(TestMbox2==0x444d3143)      //��ѯ271
	  { TestMbox1=(TestMbox1&0x1e000000)>>28;
	    if(TestMbox1>=0x1)
		{ flag7=1;       //��һ��������ƽβ��·
		}
        TestMbox1=ECanaMboxes.MBOX2.MDRL.all;
        TestMbox1=(TestMbox1&0x1e00000)>>24;
	    if(TestMbox1>=0x1)
		{ flag8=1;       //��һ��������ƽβ��·
		}
	  }
      ECanaRegs.CANRMP.bit.RMP2=1;
    }

    if(ECanaRegs.CANRMP.bit.RMP3==1)  //������������Ϣ
    { if(flag6==0)  //�ɻ��ڵ���
      { TestMbox3=ECanaMboxes.MBOX3.MDRL.all;
        TestMbox4=ECanaMboxes.MBOX3.MDRH.all;
        ECanaRegs.CANRMP.bit.RMP3=1;

         //////���յ�Ӳ���汾��//////////////
        if((TestMbox4&0xffff0000)==0x58580000)
		{ 
		  //////////Ӳ���汾�ű���///////////////////
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
         ////////���յ����洢������////////////
        if((TestMbox4&0xffff0000)==0x44530000)
        { MBOX1Send();    
		}
         ////////���յ�����洢������/////////////
        if((TestMbox4&0xffff0000)==0x51430000)
		{ QCdata(); 
		}
      }
    }
	can2++;
   }
   EINT;
}

///////////////////���籣���ж�//////////////////////////////////

/*interrupt void XINT13_isr(void)
{  
   DINT;

   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU���������ź�
   GpioDataRegs.GPADAT.bit.GPIOA9=1;
   flag22=0;
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //�ر����м���
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

///////////////////429�����ж�//////////////////////////////////

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
    /////////����LABEL274�ź�/////////////////////////////
    if((JSDZJ&0xff)==0xbc)   
    { lz=0;
      flag16=1;
      if((JSDZJ&0x600)==0x600)  //274ͨѶ����
	  { if((JSDZJ&0x8000)==0x0000)  //�ɻ��ڵ���
        { 
          flag6=flag6a;
          
          if((JSDZJ&0x4000)==0x4000)  //Ҫ�����ά��  
          { DMWH_BIT();
          }
	      else
	      { flag4=0;   //��Ҫ�����ά��,Ĭ������ 
	      }
        }
	    else
	    { flag6=1;     //�ɻ��ڿ���
	    }
	  }
    }
    //////////����LABEL213�����ź�//////////////////////////////////
    if((JSDZJ&0xff)==0x8b)
    { wd=0;
      flag18=1;
      if((JSDZJ&0x600)==0x600)   //213ͨѶ���� 
	  { if((JSGZJ&0x8000)==0x0000)  //�¶�Ϊ��
        { JSGZJ=(JSGZJ&0x7ff0)>>4;
	      wd=(float)JSGZJ*0.25;
		  if((wd>=0.0)&&(wd<5.0))
		  { t2=250;
		    flag20=0; 
		  }
		  if(wd>=5.0)   
		  { GpioDataRegs.GPADAT.bit.GPIOA9=1;
            GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //�ر����м��� 
		    FSDZJ_272=FSDZJ_272&0xdfff; 
		    flag20=1; 
		  }              
	    }
	    else        //�¶�Ϊ��
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
      tmp=JSDZJ&0xff;    //���浽�ⲿ�洢����
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

///////////////////��·�ж�//////////////////////////////////

interrupt void XINT2_isr(void)
{ 
  DINT;
  if(PieCtrl.PIEACK.bit.ACK1==1)
  { PieCtrl.PIEACK.bit.ACK1=1;   
    if((flag9==0)&&(flag10==0))  //����Ѳ�
    { if(GpioDataRegs.GPADAT.bit.GPIOA9==0)
      { flag21=1;	     
	  }
    }
  }
  EINT;   
}

////////////////����/�����л//////////////////////////
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
   if((j==1)&&(flag1==0))  //��������Ϊ����,�ܾ��л�
   { flag11=0;        //������������
   }
   if((j==2)&&(flag1==0))  //��������Ϊ��
   { j=1;
     flag14=1;
   }
   
   if((j==2)&&(flag1==1))  //��������Ϊ����,�ܾ��л�
   { flag11=0;        //������������
   }
   if((j==1)&&(flag1==1))  //��������Ϊ�Ҹ�
   { j=2;
     flag14=1;
   }
    
   EvbRegs.EVBIMRA.bit.PDPINTB=1; 
   EvbRegs.EVBIFRA.bit.PDPINTB=1;
  } 
  EINT;
}

///////////ֱ�ӷ����жϷ������//////////////
interrupt void nothing()
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
  
   //���ж�
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

   FSDZJ_271=0x00b9;   //��ʼ��429�ź�
   FSGZJ_271=0x0000;  
   FSDZJ_272=0x40ba;   //�ϵ�Ĭ�������� 
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
     
   ///////��ʱ2s////////////////////////////
   EvaRegs.T2CON.bit.TENABLE=1;
   while(1)
   { if(EvaTimer2InterruptCount>=20)
     {EvaTimer2InterruptCount=0;
	  break;
	 }
   } 
   EvaRegs.T2CON.bit.TENABLE=0;

   //////��ȡӲ���汾��/////////////////////////
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
     
   //////��ȡ�ϴ��ܹ��ϴ���///////////////
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

   ///////��ȡ�ϴν�����־/////////////////////////   
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

   //��/�ҿ����ж�//////////////////////
   if(GpioDataRegs.GPFDAT.bit.GPIOF13==1)  
   { flag1=1;         //�ҿ�����
   }
   else
   { flag1=0;        //�������	 
   }

   InitECan(); 
   ADC_adjust(); 
   poweron_BIT();  //�ϵ��Լ��

   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffde;  //��ʼ��3282
   CSH=0x0020;
   rambase6=(Uint16 *)RAMBASE6;
   *rambase6=CSH;
   for(i=0;i<5;i++){}
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
   arinc429fasong(); //429�ϱ��Լ���Ϣ
   MBOX0Send();    //����CAN����
   
   if(flag3==0)  //�ϵ��Լ�����
   {
      /////////Ӳ���汾�ŷ���/////////////////////
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

      /////////����汾�ŷ���//////////////////////
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

	  EvaRegs.T1CON.bit.TENABLE=1;  // �򿪶�ʱ��1,���й���

	  while(1)
	  { if(flag4==0) //����ά������
	    { if(count1>=18)     //429���ͼ�CANÿ1.8s����һ��
		  { count1=0;
            arinc429fasong();  //429���߷���
            MBOX0Send();	//CAN���߷���  
			if(can1!=can2)
			{//////������can���ݴ���/////////////////
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

	        tmp=(char)gz&0xff;   //�����ܹ��ϴ���
			saveDA2();
            tmp=(char)gz&0xff00>>8;
            saveDA2();
            tmp=(char)gz&0xff0000>>16;
            saveDA2();
            tmp=(char)gz&0xff000000>>24;
            saveDA2();

	        tmp=FSDZJ_271&0xff;    //����429�ź�
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
			{ //////��ѯ�����źŴ���/////////
		      if(flag16==1)
              { flag16=0;
			    lz=0;
			  }
			  else
			  {
			    error1++;
			    gz++;
			    lz++;	 
			    tmp=0x34;	 //�����������źŴ���
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
			  if(error1>=20)  //�ۼ�20��δ�յ�������Ϣ,��ɻ��ڿ���
			  { flag6=1;
			    flag6a=1;	        
			  }
			  if(lz>=5)  //����5��δ�յ�������Ϣ,��ɻ��ڿ���
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
			    tmp=0x33;	  //������213�źŴ���
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
			  if(error2>=20)  //�ۼ�20��δ�յ�������Ϣ,��ɻ����Ͽỷ����
			  { t2=380;	    
			  }
			  if(wd1>=5)  //����5��δ�յ�������Ϣ,��ɻ����Ͽỷ����
			  { t2=380;
			    wd1=0;
			  } 
	        }	        	   
	      }
		  		  
		  if(((GpioDataRegs.GPBDAT.bit.GPIOB15==1)&&(flag6==1))&&(flag20==0))                   
          { 
            if((j==1)&&(flag1==0))
		    { FSDZJ_272=FSDZJ_272|0x4000;       //������
	          jiare();
		    }
		    if((j==2)&&(flag1==1))
		    { FSDZJ_272=FSDZJ_272&0xbfff;       //������
	          jiare();
		    }			
          }
          else
		  { GpioDataRegs.GPADAT.bit.GPIOA9=1;
		    flag22=0;
	        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff; //�ر����м���
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





