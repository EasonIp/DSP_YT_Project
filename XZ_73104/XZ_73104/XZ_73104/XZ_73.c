/* 
   * ���⣺������������������
   * �ļ�����XZ_73.pit
   * ʱ�䣺2013.6.28
   * �汾����V1.04��
   * ��Ȩ��181�� 
   * ���ߣ�֣��  
*/

#include "DSP28_Device.h"

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart; 

#pragma CODE_SECTION(InitFlash, "ramfuncs");

////////////////��������/////////////////////////////////     
Uchar Count_sample;            //�ɼ����ݸ���
Uchar Sample_Result;           //����ֵ 
Uchar Count_MBIT_Receive;      //��������ά��BIT�������
Uchar Count_WDI;               //ι��ʱ�����
Uchar Count_SCI_Send;          //SCI����ʱ�����
Uint16 Count_MRB_Heat;         //��������ʱ�����
Uchar Count_TRB_Heat;          //β������ʱ�����
Uchar Time_Test;               //��·��ѯʱ����   
Uchar Count_Pulse;             //����������
Uchar Count_MRB_Test;		   //�������ʱ�����
Uchar Count_TRB_Test;		   //β�����ʱ�����
Uchar Count_Light;             //�������˸����
Uchar Count_E422_Check;        //��ѯ422����ʱ�����
Uchar Count_E422_Error;        //����δ�յ�422���ݴ���
Uint16 TRBSensor_Temperature;  //β���¶ȴ������¶�
Uint16 Current_MRB[3];         //���������������
Uint16 Current_TRB[3];         //β�������������
Uchar Count_Open_MRB[3];       //�������������·���� 
Uchar Count_Open_TRB[3];       //β�����������·���� 
Uint16 ONT_Real;               //����ÿ����ʵ�ʼ���ʱ��
Uint16 ONT_Vary;               //����ÿ�����������ʱ�� 
Uint16 OFT_Real;               //����ÿ����ʵ��ֹͣʱ��
Uint16 OFT_Vary;               //����ÿ��������ֹͣʱ��
Uchar LWC;                     //���յ�Һ̬ˮ��������
Uchar Ice_Degree;              //���ǿ������
Uchar Number1_Error;           //������1
Uchar Number2_Error;           //������2
Uchar SJB[9];                  //���յ���SJB5������
Uchar m;                       //ÿ�νӵ����ݸ���
Uchar Flag_PowerOnBIT_Finish;  //�ϵ��Լ���ɱ�־1���  
Uchar Flag_MBIT_Start;         //ά��BIT��ʼ��־=1            
Uchar Flag_MBIT_Finish;        //ά��BIT�Լ���ɱ�־
Uchar Flag_MBIT_MRB;          //ά��BIT���������ȱ�־
Uchar Flag_MBIT_TRB;          //ά��BIT��β�����ȱ�־
Uchar Flag_Switch_Check;      //�Ӵ��������ɱ�־1��� 
Uchar Flag_Controller_OK;      //������������־
Uchar Flag_MRB_OK;             //����������־
Uchar Flag_TRB_OK;             //β��������־
Uchar Flag_TRBSensor1_OK;     //β���¶ȴ�����1����
Uchar Flag_TRBSensor2_OK;     //β���¶ȴ�����2����
Uchar Flag_MRB_Heat;          //�������ڼ��ȱ�־
Uchar Flag_E422_Receive;      //���յ�422���ݱ�־
Uchar Flag_MRB_Check;         //����ά��BIT������������־
Uchar Flag_TRB_Check;         //����ά��BIT��β��������־
Uchar Flag_Heat_Start;        //������ʼ���ȱ�־

Uchar gs=0;
Uchar fk[7]={0,0,0,0,0,0,0};    //����ʱ�����
Uchar ZZ[7]={1,1,1,0,0,1,0};    //����ʱ����ֵ��
Uchar Flag_FK_Test=0;           //����ʱ�����־
Uchar fk_error=0;               //����ʱ����ϴ���

/////////////////��������/////////////////////////////////
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


/////////////////Flash��ʼ��///////////////////////////
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

/////////////////ȫ�ֱ�����ʼ��/////////////////////////////////
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
  SysCtrlRegs.PLLCR = 0x8;    //��Ƶ=30*8/2=120MHz  
  Delay(5000);                 //Ϊ��Ƶ���ȶ���ʱһ��ʱ��      
  SysCtrlRegs.HISPCP.all = 0x0002;  //��������ʱ��=120/4=30MHz
  SysCtrlRegs.LOSPCP.all = 0x0002;	 //��������ʱ��=120/4=30MHz
 
  SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;   //ʹ��EVAģ��ĸ�������ʱ��
  SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;   //ʹ��EVBģ��ĸ�������ʱ��
  SysCtrlRegs.PCLKCR.bit.ADCENCLK=1;   //ʹ��ADCģ��ĸ�������ʱ��
  SysCtrlRegs.PCLKCR.bit.SCIENCLKA=1;  //ʹ��SCIAģ��ĸ�������ʱ��
  SysCtrlRegs.PCLKCR.bit.SCIENCLKB=1;  //ʹ��SCIBģ��ĸ�������ʱ��				
  EDIS;	
}

///////////////////GPIO�ڳ�ʼ��//////////////////////////////////
void InitGpio(void)
{
  EALLOW;
  GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOA����ΪI/O�� 
  GpioMuxRegs.GPADIR.all=0x0000;    //A0-A8����Ϊ����	

  GpioMuxRegs.GPBMUX.all=0x0000;   //GPIOB����ΪI/O�� 
  GpioMuxRegs.GPBDIR.all=0xffff;   //B0-B15����Ϊ���	
  GpioDataRegs.GPBDAT.all=0x1c4;       
  EDIS;
}	

///////////////////�ⲿ�ж�1��ʼ��//////////////////////////////////
void InitXINT1(void)
{  
  GpioMuxRegs.GPEMUX.bit.XINT1_XBIO_GPIOE0=1; //�����ж�1���� 
  XIntruptRegs.XINT1CR.all=0x5;      //�����������ж�
}

///////////////////�ⲿ�ж�2��ʼ��//////////////////////////////////
void InitXINT2(void)
{  
  GpioMuxRegs.GPEMUX.bit.XINT2_ADCSOC_GPIOE1=1; //�����ⲿ�ж�2���� 
  XIntruptRegs.XINT2CR.all=0x5;      //�����������ж�
}

///////////////////��ʱ��1��ʼ����25ms)/////////////////////////////////
void Init_eva_timer1(void)
{  
  EvaRegs.GPTCONA.all = 0x0;   //��ʼ����ʱ��1�����ö�ʱ��1�Ĵ���
  EvaRegs.EVAIMRA.bit.T1PINT = 1;  //����ʱ��1�����ж�
  EvaRegs.EVAIFRA.bit.T1PINT = 1;  //�����ʱ��1�����жϱ�־
             
  EvaRegs.T1CON.all = 0x170C;  //Timer1Ϊ����������ģʽ��Ԥ��Ƶֵ128
	                             //ʹ���ڲ���������ʱ��
  EvaRegs.T1PR = 0x16e3;   //Timer1���ڼĴ���=0.025*30MHz/128
  EvaRegs.T1CNT = 0x0000;  //Timer1�ļ�������
}

///////////////////��ʱ��4��ʼ����25ms��//////////////////////////////////
void Init_evb_timer4(void)
{ 
  EvbRegs.GPTCONB.all=0x0;     //��ʼ����ʱ��4�����ö�ʱ��4�Ĵ���
  EvbRegs.EVBIMRB.bit.T4PINT = 1;  //����ʱ4�����ж�
  EvbRegs.EVBIFRB.bit.T4PINT = 1;  //�����ʱ��4�����־
             
  EvbRegs.T4CON.all = 0x170C;  //Timer4Ϊ����������ģʽ��Ԥ��Ƶֵ128��
	                           //ʹ���ڲ���������ʱ��
  EvbRegs.T4PR = 0x16e3;     //Timer4�����ڼĴ���=0.025*30MHz/128
  EvbRegs.T4CNT = 0x0000;  //Timer4�ļ���������
}

/////////////////���ڳ�ʼ��//////////////////////////////////////
void InitSci(void)
{	
  EALLOW;
  GpioMuxRegs.GPFMUX.all = 0x0030;     //����SCIA����
  GpioMuxRegs.GPGMUX.all = 0x0030;     //����SCIB����
  EDIS;

  //////////////SCIA��ʼ��//////////////////////////////
  SciaRegs.SCIFFTX.all=0xe040;    //�������,ʹ��FIFO
  SciaRegs.SCIFFRX.all=0x2021;    //ʹ��FIFO����
  SciaRegs.SCIFFCT.all=0x0000;    //��ֹ������У��
  SciaRegs.SCICCR.all = 0x07;     //1��ֹͣλ,��У��,��Ч���ݳ���8λ	
  SciaRegs.SCICTL1.all = 0x03;    //��λ
  SciaRegs.SCICTL2.all = 0x03;    //��λ  	
  SciaRegs.SCIHBAUD = 0x01;     //���ò�����9600bps
  SciaRegs.SCILBAUD = 0x85;	
  SciaRegs.SCICTL1.all = 0x23;   //ʹ�ܷ��ͽ��չ���
  
  //////////////SCIB��ʼ��//////////////////////////////
  ScibRegs.SCIFFTX.all=0xe040;    //�������,ʹ��FIFO
  ScibRegs.SCIFFRX.all=0x2021;    //ʹ��FIFO����
  ScibRegs.SCIFFCT.all=0x0000;    //��ֹ������У��
  ScibRegs.SCICCR.all = 0x07;     //1��ֹͣλ,��У��,��Ч���ݳ���8λ	
  ScibRegs.SCICTL1.all = 0x03;    //��λ
  ScibRegs.SCICTL2.all = 0x03;    //��λ  	
  ScibRegs.SCIHBAUD = 0x01;     //���ò�����9600bps
  ScibRegs.SCILBAUD = 0x85;	
  ScibRegs.SCICTL1.all = 0x23;   //ʹ�ܷ��ͽ��չ���  	
} 

///////////////////AD��ʼ��//////////////////////////////////
void InitAdc(void)
{	
  AdcRegs.ADCTRL1.bit.RESET=1;  //��λADCģ��
  Delay(5000);	 
  AdcRegs.ADCTRL1.bit.RESET=0;
  AdcRegs.ADCTRL1.bit.SUSMOD=3;
  AdcRegs.ADCTRL1.bit.ACQ_PS=0;
  AdcRegs.ADCTRL1.bit.CPS=0;     
  AdcRegs.ADCTRL1.bit.CONT_RUN=0; //ADΪ����/ֹͣģʽ
  AdcRegs.ADCTRL1.bit.SEQ_CASC=1; //16����ģʽ

  AdcRegs.ADCTRL3.bit.ADCEXTREF = 0;    //ʹ���ڲ��ο���ѹ     	
  AdcRegs.ADCTRL3.bit.ADCBGRFDN=3; //����϶�ο���·�ϵ�	
  Delay(5000); 	                  //�ȴ��ȶ�          
  AdcRegs.ADCTRL3.bit.ADCPWDN=1;  //������·�ϵ�
  Delay(5000);                    //�ȴ��ȶ�
  AdcRegs.ADCTRL3.bit.ADCCLKPS=15;  
  AdcRegs.ADCTRL3.bit.SMODE_SEL=0;  //˳�����
	
  AdcRegs.MAX_CONV.bit.MAX_CONV=15;  //����ͨ��Ϊ16   
  AdcRegs.ADCTRL2.all=0x8c04;
  				
  AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;
  AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;				
} 

/////////////��ʱ����/////////////////////////////////
void Delay(Uint32 xms)
{
  Uint32 i=0;
  for(i=0;i<xms;i++)
  {
    asm("NOP");
  } 
}

//////////////�źŲɼ�///////////////////////////////
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

////////////CPU��д���////////////////////////
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
      Flag_Controller_OK=1;   //����������
      Number1_Error=Number1_Error|0x40;   //�ϱ�������ʧЧ    
	  break;
    }
    else
    {
	  Flag_Controller_OK=0;   //����������
      Number1_Error=Number1_Error&0xbf;   //�ϱ�����������
	  i++;
	  if(i>19)
	  {
	    break;
	  }
    }
  }
}

////////////CPU��ʱ�����////////////////////////
void CPU_Timer_Check(void)
{
  EvaRegs.T1CON.bit.TENABLE=1;   //�򿪶�ʱ��1
  EvbRegs.T4CON.bit.TENABLE=1;   //�򿪶�ʱ��4
  Delay(0x9fff);
  EvaRegs.T1CON.bit.TENABLE=0;    //�رն�ʱ��1
  EvbRegs.T4CON.bit.TENABLE=0;    //�رն�ʱ��4
  
  if(abs(EvaRegs.T1CNT-EvbRegs.T4CNT)>=5)
  {
    Flag_Controller_OK=1;    //����������
    Number1_Error=Number1_Error|0x40;   //�ϱ�������ʧЧ
  }
  else
  {
    Flag_Controller_OK=0;   //ϵͳ����
    Number1_Error=Number1_Error&0xbf;   //�ϱ�����������
	EvaRegs.T1CNT=0x0000;
	EvbRegs.T4CNT=0x0000;
  }

  /////////////////////////////////////////////////////
  if(Flag_Controller_OK==0)   //����������
  {
    EvaRegs.T1CON.bit.TENABLE=1;   //�򿪶�ʱ��1
    /////////����ʱ��4���ܸ�Ϊ����ADת������///////////// 
    PieCtrl.PIEIER5.bit.INTx1=0;   //�رն�ʱ��4�ж�
    EvbRegs.GPTCONB.all=0x400;    //��ʱ��4�����ж�����ADת��
    EvbRegs.T4PR = 0xea;          //Timer4�����ڼĴ���=0.001*30MHz/128 
  }    
}

///////// �����¶ȴ��������///////////////////////

void AirSensor_Check(void)
{      
  ADC_sample(0x0000);   //ADCINA0�ɼ������¶� 
  if(Flag_PowerOnBIT_Finish==0)    //�ϵ�BITʱ	 
  { 
    if((Sample_Result<530)||(Sample_Result>630))
    { 
      Flag_Controller_OK=1;    //����������
	  Number1_Error=Number1_Error|0x40;   //�ϱ�������ʧЧ
    }
	else
	{
	  Flag_Controller_OK=0;    //����������
	  Number1_Error=Number1_Error&0xbf;   //�ϱ�����������
	}
  }
  else 
  { 
	if((Sample_Result<100)||(Sample_Result>2900))
    { 
      Delay(0xffff);
      ADC_sample(0x0000);   //ADCINA0�ɼ������¶�
	  if((Sample_Result<100)||(Sample_Result>2900))   //�����ж�2��
	  {
	    Number1_Error=Number1_Error|0x24;   //�ϱ������¶ȴ�����β������ʧЧ
        GpioDataRegs.GPBDAT.bit.GPIOB10=0;  //�¶Ȳ�����
	    Flag_TRB_OK=1;        //β������
	    if(Flag_MRB_Heat==0)
	    {
	      Flag_MRB_OK=1;        //��������        
	    }
	    Number1_Error=Number1_Error|0x2;  //�ϱ���������ʧЧ
	    GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //������������ʧЧ�� 
        GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //����β������ʧЧ��		
	  }
    }
	else
	{
	  if(Sample_Result<=1271)       //�����¶�>=20��
	  {
	    GpioDataRegs.GPBDAT.bit.GPIOB10=1;  //����¶�����
	  }
	  else
	  { 
	    GpioDataRegs.GPBDAT.bit.GPIOB10=0;  //�¶Ȳ�����
	  }
	}
  }
} 

//////////���β���¶ȴ��������//////////////////////
void TRBSensor_Check(void)
{   
  Uchar i=0; 
  
  if(Flag_PowerOnBIT_Finish==0)    //�ϵ�BITʱ	    	 
  {
    while(i<2)
    {
	  ADC_sample(0x1111+i*0x1111);        
      if((Sample_Result<1175)||(Sample_Result>1275))
      { 
        Flag_Controller_OK=1;      //����������
	    Number1_Error=Number1_Error|0x40;   //�ϱ�������ʧЧ
		break;
      }
      else
	  { 
	    Flag_Controller_OK=0;      //����������
	    Number1_Error=Number1_Error&0xbf;   //�ϱ�����������
		i++;
	  }
    }
  }
  else
  { 
    if(Flag_TRBSensor1_OK==0)    //β���¶ȴ�����1����
    {
	  i=0;			   //ADCINA1�ɼ�β���¶ȴ�����1
	}
    else
	{
	  i=1;            //ADCINA2�ɼ�β���¶ȴ�����2
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
	      Flag_TRBSensor1_OK=1;     //β���¶ȴ�����1����
	    }
	    else if(i==1)
	    {
	      Flag_TRBSensor2_OK=1;     //β���¶ȴ�����2����
	    }
        Number2_Error=Number2_Error|0x2;  //β���¶ȴ���������
      }       
    }
    if((Flag_TRBSensor1_OK==1)&&(Flag_TRBSensor2_OK==1))
    { 
      Flag_TRB_OK=1;                    //β������
      Number2_Error=Number2_Error|0x1;  //β���¶ȴ�����ʧЧ 
      Number1_Error=Number1_Error|0x4;  //β������ʧЧ 
      GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //�ر�β������
      GpioDataRegs.GPBDAT.bit.GPIOB0=0;   //�ر�β�����ƿ���
      GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //����β������ʧ�       
	}   
  } 
}

/////////////������Ӧ���/////////////////////////////////////////
void MRB_QXJC(void)
{   
  Uchar i=0;
  
  if(Flag_PowerOnBIT_Finish==0)    //�ϵ�BITʱ
  {
    while(i<3)
    {
      ADC_sample(0x3333+i*0x1111);    
      if((Sample_Result<2450)||(Sample_Result>2550))
	  {
	    Flag_Controller_OK=1;      //����������
	    Number1_Error=Number1_Error|0x40;   //�ϱ�������ʧ
		break;
	  }
	  else
	  {
	    Flag_Controller_OK=0;      //����������
	    Number1_Error=Number1_Error&0xbf;   //�ϱ�����������
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
      if((Current_MRB[i]<2450)||(Current_MRB[i]>3000))	//����ȱ�����
      { 
        Delay(0xffff);
        ADC_sample(0x3333+i*0x1111); 
        Current_MRB[i]=Sample_Result;        
        if((Current_MRB[i]<2450)||(Current_MRB[i]>3000))
        { 
          Flag_MRB_OK=1;   //������������
        }
      } 	
	}      
    if((abs(Current_MRB[0]-Current_MRB[1])>350)||(abs(Current_MRB[0]-Current_MRB[2])>350)
      ||(abs(Current_MRB[1]-Current_MRB[2])>350))
    { 
      Flag_MRB_OK=1;     //������������
    }
	//////////////////////////////////////////////	
	if(Flag_MRB_OK==1)  //������������
	{
	  Number1_Error=Number1_Error|0x10;   //�ϱ�������������
      GpioDataRegs.GPBDAT.bit.GPIOB11=0;  //�ر���������
      GpioDataRegs.GPBDAT.bit.GPIOB2=1;   //�ر��������ƿ���
	  if(Flag_MBIT_Start==0)
      {
        GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //������������ʧЧ��
	  }
	}
  }
} 

/////////////β����Ӧ���/////////////////////////////////////////
void TRB_QXJC(void)
{   
  Uchar i=0;
  
  if(Flag_PowerOnBIT_Finish==0)    //�ϵ�BITʱ
  {
    while(i<3)
    {
      ADC_sample(0x6666+i*0x1111);
      if((Sample_Result<1540)||(Sample_Result>1640))
	  {
	    Flag_Controller_OK=1;      //����������
	    Number1_Error=Number1_Error|0x40;   //�ϱ�������ʧЧ
		break;
	  }
	  else
	  {
	    Flag_Controller_OK=0;      //����������
	    Number1_Error=Number1_Error&0xbf;   //�ϱ�����������
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
          Flag_TRB_OK=1;                    //β������
        }
      }
	}      
    if((abs(Current_TRB[0]-Current_TRB[1])>360)||(abs(Current_TRB[0]-Current_TRB[2])>360)
      ||(abs(Current_TRB[1]-Current_TRB[2])>360))
    { 
      Flag_TRB_OK=1;                    //β������
    }
	//////////////////////////////////////////
	if(Flag_TRB_OK==1)  //β������ʧЧ
	{
	  Number1_Error=Number1_Error|0x4;   //�ϱ�β������ʧЧ
      GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //�ر�β������
      GpioDataRegs.GPBDAT.bit.GPIOB0=0;   //�ر�β�����ƿ���
	  if(Flag_MBIT_Start==0)
      {
        GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //����β������ʧЧ��
	  }
	} 
  }
} 

/////////////�ϵ�BIT//////////////////////////////////
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
  /////////����Զ����Ƶ�·�Ƿ�����//////////////////////////
  if(Flag_Controller_OK==0)
  { 
    GpioDataRegs.GPBDAT.bit.GPIOB8=0;    //���"�¶������Լ�"�ź�
    Delay(0xffff);
    AirSensor_Check(); //�жϴ����¶ȴ����·�Ƿ�����
    if(Flag_Controller_OK==1)
	{
	  AirSensor_Check();
	}

    TRBSensor_Check(); //�ж�β���¶ȴ����������·�Ƿ�����
    if(Flag_Controller_OK==1)
	{
	  TRBSensor_Check();
	}

    //////����������ദ���·//////////////////
	GpioDataRegs.GPBDAT.bit.GPIOB6=0;    //����"������Ӧ�Լ�"�ź�
    Delay(0x3ffff);
    MRB_QXJC();
    if(Flag_Controller_OK==1)
	{
	  MRB_QXJC();
	}
    //////���β�����ദ���·//////////////////
	GpioDataRegs.GPBDAT.bit.GPIOB7=0;    //����"β����Ӧ�Լ�"�ź�
    Delay(0x3ffff);
    TRB_QXJC();
    if(Flag_Controller_OK==1)
	{
	  TRB_QXJC();
	}

    GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all|0x1e0;  //�ر������Լ�����
  }

  /////////����ֶ����Ƶ�·�Ƿ�����//////////////////////////
  if(Flag_Controller_OK==0)
  { 
    GpioDataRegs.GPBDAT.bit.GPIOB5=1;    //����"�ֶ������Լ�"�ź�
    Delay(0xffff); 
    if(GpioDataRegs.GPADAT.bit.GPIOA8==0)
    {       
      Delay(0xffff);
      if(GpioDataRegs.GPADAT.bit.GPIOA8==0)
      {
	    Flag_Controller_OK=1;
	    Number1_Error=Number1_Error|0x40;   //�ϱ�������ʧЧ
      } 
	}
    GpioDataRegs.GPBDAT.bit.GPIOB5=0;
  }
  ////////////////////////////////////////////////
  if(Flag_Controller_OK==0)     //�ϵ�BIT����
  {
    GpioDataRegs.GPBDAT.bit.GPIOB9=1;  //����������ָʾ��
	Flag_PowerOnBIT_Finish=1;   //�ϵ�BIT��� 
  }        
}  

//////////�Ӵ������////////////////////////////
void Switch_Check(void)
{    
  ////////////�Ͽ���β�����м���/////////////////////////
  GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //�ر�β������
  GpioDataRegs.GPBDAT.bit.GPIOB0=0;  //�ر�β�����ƿ���
  GpioDataRegs.GPBDAT.bit.GPIOB2=1;  //�ر���������
  GpioDataRegs.GPBDAT.bit.GPIOB11=0;  //�ر��������ƿ��� 
  Flag_MRB_OK=0;    //�Զ��������ֹ���ʱ�Կɽ���ά��BIT
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
  /////////////////��������Ӵ����Ͽ�״̬�Ƿ�����//////////////////
  Delay(0x3ffff);
  if(GpioDataRegs.GPADAT.bit.GPIOA0==0)	//�����Ӵ����Լ��ź�0����
  {
    Delay(0xffff);
    if(GpioDataRegs.GPADAT.bit.GPIOA0==0)
	{
	  Flag_MRB_OK=1;      //��������
	}
  }
  /////////////////���β���Ӵ����Ͽ�״̬�Ƿ�����//////////////////
  Delay(0x3ffff); 
  if(GpioDataRegs.GPADAT.bit.GPIOA1==0)	//β���Ӵ����Լ��ź�0����
  {
    Delay(0xffff);
    if(GpioDataRegs.GPADAT.bit.GPIOA1==0)
	{
	  Flag_TRB_OK=1;     //β������
	}
  }
}

/////////���̽����ϵͳ���//////////////////////
void Detector_Check(void)
{  
  if(GpioDataRegs.GPADAT.bit.GPIOA6==0)   
  { 
    Delay(0xffff);
	if(GpioDataRegs.GPADAT.bit.GPIOA6==0)
    {
      Number2_Error=Number2_Error|0x4;  //�ϱ����̽��ʧЧ	  
      if(Flag_MRB_Heat==0)
	  {
	    Flag_MRB_OK=1;      //��������
		PieCtrl.PIEIER9.bit.INTx3=0;     //������LWC
	  } 
    }
  }
}

/////////////����ά��BIT//////////////////////////////////
void MBIT(void)
{     
  if(Flag_MBIT_Finish==0)
  {
    if(Flag_Switch_Check==0)
    {
      Switch_Check();
      Flag_Switch_Check=1;    //�Ӵ���������
    }
    /////////////�����Ӵ��������������м���///////////////////  
    if(Flag_MRB_OK==0)
    {
	  if((Flag_MBIT_MRB==0)&&(Flag_MRB_Check==0))
      { 
        ONT_Real=34;     //ά��BIT����ʱ��Ϊ0.85s    
        Time_Test=16;   //0.4s���һ�μ��            
	    Flag_MBIT_MRB=1;   //׼������ά��BIT��������
      }
	  /////////�ȼ�����������Ƿ�����////////////////////
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
    ///////////////�ټ��β��������////////
    if(Flag_TRB_OK==0)
    {
	  if((Flag_MBIT_TRB==0)&&(Flag_MRB_Check==1)&&(Flag_TRB_Check==0))
	  {
        Flag_MBIT_TRB=1;  //׼������ά��BITβ������
	  }            	  
	  if(Flag_MBIT_TRB==1)
	  {
	    TRB_Heat();
		if(Flag_MBIT_TRB==0)
		{
          Flag_TRB_Check=1;
          Flag_MBIT_Finish=1;  //������׼���ϱ����
		}
	  }
    }
    if((Flag_TRB_OK==1)&&(Flag_MRB_Check==1))
	{
      Flag_MBIT_TRB=0;
      Flag_TRB_Check=1;
      Flag_MBIT_Finish=1;  //������׼���ϱ����
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
    GpioDataRegs.GPBDAT.bit.GPIOB3=0;      //���
    GpioDataRegs.GPBDAT.bit.GPIOB4=0;
    Flag_Switch_Check=0;
    Flag_MBIT_Finish=0;
    Flag_MBIT_Start=0;   //׼����һ��ά��BIT
	Time_Test=80;
	Flag_MRB_Check=0;
    Flag_TRB_Check=0;
  }
}

///////////////�������ȿ���////////////////////////////////////
void MRB_Heat(void)
{  
  if(GpioDataRegs.GPADAT.bit.GPIOA7==0)  //��ѯϵͳ��Դ�Ƿ�����
  { 
    if(GpioDataRegs.GPBDAT.bit.GPIOB2==1)   
    {       
      GpioDataRegs.GPBDAT.bit.GPIOB2=0;    //��ͨ�������ƿ��� 	   
	  Delay(0x8fffff);
      if(Flag_MBIT_MRB==1)    //ά��BITʱ��������Ӵ�����ͨ״̬�Ƿ�����
      {
        if(GpioDataRegs.GPADAT.bit.GPIOA0==1)	//�����Ӵ����Լ��ź�1����
        {
          Delay(0xffff);
          if(GpioDataRegs.GPADAT.bit.GPIOA0==1)
	      {
	        Flag_MRB_OK=1;      //��������
	      }
		}
      }	  
	}
	if((GpioDataRegs.GPBDAT.bit.GPIOB2==0)&&(Count_MRB_Heat<=0))
	{
	  Count_Pulse=1;
	  Flag_Heat_Start=1;
	}
    /////////////////////����ʱ��///////////////////// 
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
      OFT_Real=OFT_Vary;    //������ĶϿ�ʱ�丳ֵ
	  Count_MRB_Test=0;
	  Flag_FK_Test=0;
	}
	if(Flag_MBIT_MRB==1)    //ά��BIT��������ʱ
	{
	  if(Count_MRB_Heat>(7*ONT_Real+48))
	  { 
	    GpioDataRegs.GPBDAT.bit.GPIOB11=0;   //�ر���������
        GpioDataRegs.GPBDAT.bit.GPIOB2=1;    //�Ͽ�������Դ 
        Flag_Heat_Start=0; 
	    Count_MRB_Heat=0;
	    Flag_MBIT_MRB=0;           //ά��BIT�������Ƚ���
	    Count_MRB_Test=0;
	    Flag_FK_Test=0;      
	  }
	}
	else    //�Զ�����
	{	  
	  if((ONT_Real+OFT_Real)<(7*ONT_Real+48))   //��������T<(7*Ton+8*0.15)s
	  {
	    if(((7*ONT_Real+48)<Count_MRB_Heat)&&(Count_MRB_Heat<=(7*ONT_Real+54)))
	    { 
	      GpioDataRegs.GPBDAT.bit.GPIOB11=0;
	    } 
	    if(Count_MRB_Heat>(7*ONT_Real+54))
	    { 
	      Count_MRB_Heat=0;
		  Flag_Heat_Start=0;
	      Flag_MRB_Heat=0;           //һ���������ڽ���
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
	      Flag_MRB_Heat=0;           //һ���������ڽ���
		  Count_MRB_Test=0; 
		  Flag_FK_Test=0;
	    }
      }
	}
  }
  else
  { 
    GpioDataRegs.GPBDAT.bit.GPIOB11=0;   //�Ͽ�����ʱ��
    GpioDataRegs.GPBDAT.bit.GPIOB2=1;    //�Ͽ�������Դ
    Flag_Heat_Start=0; 
	Flag_Heat_Start=0;
    Count_MRB_Heat=0;
    Count_MRB_Test=0;
    Flag_FK_Test=0; 
  }   
 	
  //////////////ȱ�༰����ʱ����/////////////////////////////////////////////  	
  if(((GpioDataRegs.GPBDAT.bit.GPIOB11==0)&&(Count_MRB_Test>=Time_Test))
       &&(Count_Pulse>0))             //ÿ1s��ѯһ��
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

///////////////����ʱ����//////////////////////////////////
void FK_Check(void)
{
  if(GpioDataRegs.GPADAT.bit.GPIOA4==1)   //����ʱ�������
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
        GpioDataRegs.GPBDAT.bit.GPIOB11=0;   //�Ͽ�����ʱ��
        GpioDataRegs.GPBDAT.bit.GPIOB2=1;    //�Ͽ�������Դ		
        Number1_Error=Number1_Error|0xa;  //�ϱ�����ͬ��ʧЧ,��������
        if(Flag_MBIT_Start==0)
        {
         GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //������������ʧЧ��
        }
		Flag_FK_Test=1;			  //��������ʱ�������
      }
	}
  }

  if(fk_error==0)
  {
    Flag_FK_Test=1;          //��������ʱ�������
  }
}

///////////////β�����ȿ���////////////////////////////////////
void TRB_Heat(void)
{  
  if(GpioDataRegs.GPADAT.bit.GPIOA7==0)  //��ѯ���ȵ�Դ�Ƿ��ͨ
  { 
    if(Flag_MBIT_TRB==1)     //ά��BITʱ���β���Ӵ�����ͨ״̬�Ƿ�����
    { 
      GpioDataRegs.GPBDAT.bit.GPIOB0=1;    //��ͨβ�����ƿ���
      Delay(0x3ffff);
      if(GpioDataRegs.GPADAT.bit.GPIOA1==1)	//β���Ӵ����Լ��ź�1����
      {
        Delay(0xffff);
        if(GpioDataRegs.GPADAT.bit.GPIOA1==1)
	    {
	      Flag_TRB_OK=1;     //β������
	    }
      } 
      //////////////////////////////////////////////////      
      if((Count_TRB_Heat>0)&&(Count_TRB_Heat<=34))
	  { 	    	    
	    GpioDataRegs.GPBDAT.bit.GPIOB1=1;   //β������
	  }
	  if(Count_TRB_Heat>34)
	  { 
	    Count_TRB_Heat=0;
	    GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //β��ֹͣ����	
	    GpioDataRegs.GPBDAT.bit.GPIOB0=0;    //�Ͽ�β����Դ 
	    Flag_MBIT_TRB=0;          //����ά��β���������     
	  }
    }
	else
	{ 
	  if(Flag_TRBSensor1_OK==0)
	  { 	    	    
	    if(TRBSensor_Temperature>1060)
	    { 
	      GpioDataRegs.GPBDAT.bit.GPIOB1=1;   //β������
	      GpioDataRegs.GPBDAT.bit.GPIOB0=1;    //��ͨβ����Դ	      
		}
        if(TRBSensor_Temperature<740)
		{ 
		  GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //β��ֹͣ����		  	 
		}
	  }
      else
	  { 
	    if(Flag_TRBSensor2_OK==0)
	    { 
	      if(TRBSensor_Temperature>1060)
	      { 
	        GpioDataRegs.GPBDAT.bit.GPIOB1=1;   //β������
	        GpioDataRegs.GPBDAT.bit.GPIOB0=1;    //��ͨβ����Դ
		  }
          if(TRBSensor_Temperature<740)
		  { 
		    GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //β��ֹͣ����  
		  }
		}
        else
	    { 
	      GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //β��ֹͣ����
		  GpioDataRegs.GPBDAT.bit.GPIOB0=0;    //�Ͽ�β����Դ        
	    }
	  }
    }
    //////////////ȱ����/////////////////////////////////////////////  	
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
	GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //β��ֹͣ����
    GpioDataRegs.GPBDAT.bit.GPIOB0=0;    //�Ͽ�β����Դ
  }
}

/////////////////�������ʱ��////////////////////////////////////////
void Heat_Time_Count(void)
{           
  float Air_Temperature1=0.0;     //��������
  int Air_Temperature2=0;
  float Ontime1=0.0;
  float Ontime2=0.0;
 
  ADC_sample(0x0000); 	  //ADCINA0�ɼ������¶�
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
    GpioDataRegs.GPBDAT.bit.GPIOB10=1;  //����¶�����
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
 
///////////////////��ʱ��1�ж�//////////////////////////////////

interrupt void eva_timer1_isr(void)
{  
  DINT;
  if(PieCtrl.PIEACK.bit.ACK2==1)
  { 
    PieCtrl.PIEACK.bit.ACK2=1; 
    Count_WDI++;   
    Count_SCI_Send++;      //ι�������ڷ���ʱ����
	Count_E422_Check++;
	/////////////////////////////////////////////////
	if(Count_WDI>=20)
	{
	  GpioDataRegs.GPBDAT.bit.GPIOB12=~GpioDataRegs.GPBDAT.bit.GPIOB12;  //ι��         
	}
    ///////////////////////////////////////////////
    if(Flag_PowerOnBIT_Finish==0)    //�ϵ�BITʱ
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
	    if(GpioDataRegs.GPADAT.bit.GPIOA3==0) //���յ�ά���Լ�����
	    { 
	      Count_MBIT_Receive++;
	      if(Count_MBIT_Receive>=2)
          {	
		    Count_MBIT_Receive=0; 
	        Flag_MBIT_Start=1;         //׼������ά��BIT
	      }
	    }
	    else
	    {
	      Count_MBIT_Receive=0;
	    }
	  }	  
	  /////////////////////////////////////////////////////////////	  
	  if(Flag_MBIT_Start==1)    //����ά��BITʱ
	  {
	    if((Flag_MBIT_MRB==1)&&(Flag_Heat_Start==1))    //����
	    {
	      Count_MRB_Heat++;	    
	    }
	    if((GpioDataRegs.GPBDAT.bit.GPIOB0==1)&&(Flag_MBIT_TRB==1))  //β��
	    {
	      Count_TRB_Heat++;
	    }
	    if(Flag_MBIT_Finish==1)   //������
	    {
	      if((Flag_TRB_OK==1)||(Flag_MRB_OK==1))
	      {
	        Count_Light++;
	      }
		}
	  }        
      else  //Ѳ��ʱ
	  {
        if(Flag_Heat_Start==1)    //����
	    {
	      Count_MRB_Heat++;	    
	    }
	  }
      ////////////////////////////////////////////////////
      if(GpioDataRegs.GPBDAT.bit.GPIOB2==0)    //����
	  { 
	    if((Flag_MRB_Heat==0)&&(Flag_MBIT_MRB==0))   //һ�����ڽ���ʱ
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
	  if(GpioDataRegs.GPBDAT.bit.GPIOB1==1)  //β��
      { 
		Count_TRB_Test++;
	  } 
    }   
    EvaRegs.EVAIMRA.bit.T1PINT = 1;
    EvaRegs.EVAIFRA.bit.T1PINT = 1;
  }
  EINT;
} 


//////////////����LWC/////////////////////////////////
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
		    Flag_E422_Receive=1;  //�յ�����
            Count_E422_Error=0; 
            LWC=SJB[5];  
            if(LWC>0xc8)
            {
              LWC=0xc8;
            }         
            if(LWC>0)
		    {
		      /////////����Ͽ�ʱ��////////////////////                     		  
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
              /////////������ǿ��////////////////////
              if((LWC>=0x01)&&(LWC<=0x14))
		      {
		        Ice_Degree=0x0;    //��΢���
		      }
              if((LWC>=0x15)&&(LWC<=0x32))
		      {
		        Ice_Degree=0x1;    //��Ƚ��
		      }
              if((LWC>=0x33)&&(LWC<=0x64))
		      {
		        Ice_Degree=0x2;    //�жȽ��
		      }
              if((LWC>=0x65)&&(LWC<=0xc8))
		      {
		        Ice_Degree=0x3;    //�ضȽ��
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


///////////////////ADC�ж�//////////////////////////////////
interrupt void ad_isr(void)
{ 
  Uint16 Result[16];          //����AD����
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
 		
	///////////16��������С��������//////////////////////
	for(Count_sample=0;Count_sample<16;Count_sample++)
	{ for(i=0;i<16-Count_sample;i++)                      
	  { if(Result[i]>Result[i+1])
	    { max=Result[i];
		  Result[i]=Result[i+1];
          Result[i+1]=max;
		}
	  }
	}    	
	//////////ȥͷȥβȡ�м�14����Ч������ƽ��////////////	
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


///////////////////β����·�ж�//////////////////////////////////
interrupt void XINT1_isr(void)
{ 
  DINT;
  if(PieCtrl.PIEACK.bit.ACK1==1)
  { 
    PieCtrl.PIEACK.bit.ACK1=1;  
    if(Flag_PowerOnBIT_Finish==1)   //�ϵ�BIT���
    {
      GpioMuxRegs.GPEMUX.bit.XINT1_XBIO_GPIOE0=1;
	  GpioMuxRegs.GPEDIR.bit.GPIOE0=0;
	  Delay(0x3ffff);
      if(GpioDataRegs.GPEDAT.bit.GPIOE0==1)
	  { 
	    Flag_TRB_OK=1;          //β�����ȹ���
	    Number1_Error=Number1_Error|0x4;   //�ϱ�β������ʧЧ
        GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //�ر�β������
        GpioDataRegs.GPBDAT.bit.GPIOB0=0;   //�ر�β�����ƿ���
		if(Flag_MBIT_Start==0)
        {
          GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //����β������ʧЧ��
        } 
	  }
	}
  }
  EINT;
}

///////////////////������·�ж�//////////////////////////////////
interrupt void XINT2_isr(void)
{ 
  DINT;
  if(PieCtrl.PIEACK.bit.ACK1==1)
  { 
    PieCtrl.PIEACK.bit.ACK1=1;  
    if(Flag_PowerOnBIT_Finish==1)   //�ϵ�BIT���
    {
      GpioMuxRegs.GPEMUX.bit.XINT2_ADCSOC_GPIOE1=1;
	  GpioMuxRegs.GPEDIR.bit.GPIOE1=0;
	  Delay(0x3ffff);
	  if(GpioDataRegs.GPEDAT.bit.GPIOE1==1)
	  { 
	    Flag_MRB_OK=1;        //�������ȹ���
        GpioDataRegs.GPBDAT.bit.GPIOB11=0;  //�ر���������
        GpioDataRegs.GPBDAT.bit.GPIOB2=1;   //�ر��������ƿ���       
	    Number1_Error=Number1_Error|0x2;   //�ϱ���������ʧЧ
		if(Flag_MBIT_Start==0)
		{
		  GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //������������ʧЧ��
		}
	  }
	} 
  }
  EINT;
}


///////////ֱ�ӷ����жϷ������//////////////
interrupt void nothing()
{ 
  EINT;
  return;
}

/////////////������//////////////////////////////////////

main()
{  
  Init_Variable();
  InitSysCtrl();
  //��InitFlash�����������Ƶ�ram������
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
  
  //���ж�   
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
	if(EvaRegs.T1CON.bit.TENABLE==1)  //��ʱ������
	{
	  if(Count_SCI_Send>=40)     //1s������422�����Ϣ
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
        if(Flag_E422_Receive==1)  //�յ�LWC����
	    {
          Flag_E422_Receive=0;
	    }
	    else
	    {
          Count_E422_Error++;
		  if(Count_E422_Error>=5)    //����5��δ�յ�LWC
		  {          
            Number2_Error=Number2_Error|0x4;   //�ϱ����̽��ʧЧ 
		  }
	    }
      }
    }
    else
    {
	  GpioDataRegs.GPBDAT.bit.GPIOB12=~GpioDataRegs.GPBDAT.bit.GPIOB12;  //ι��         
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
    if(Flag_PowerOnBIT_Finish==1)   //�ϵ�BIT���
    {
      if(Flag_MBIT_Start==1)   //׼������ά��BIT
      { 
	    Number2_Error=Number2_Error|0x10;  //�����Լ�
        MBIT();   //����ά��BIT
      }
      else      //Ѳ��ʱ
      {
        Number2_Error=Number2_Error&0xef;   //�Լ����
        ////////////////////////////////////
        if((Number2_Error&0x4)==0x0)
        {
          Detector_Check();   //���̽����
		}
		else	   //���̽��ʧЧ�ϱ���������
		{
	      if((Flag_MBIT_Start==0)&&(Flag_MRB_Heat==0))
		  {
			Flag_MRB_OK=1;     //��������
		  }
		} 
        /////////////////////////////////////////////
        if((Number1_Error&0x20)==0x0)
        {
          AirSensor_Check();  //�����¶ȴ��������
		}
		////////////////////////////////////////////////
		if(Flag_TRBSensor1_OK==0)
		{
		  TRBSensor_Check();  //β���¶ȴ��������
		}
		if((Flag_TRBSensor1_OK==1)&&(Flag_TRBSensor2_OK==0))
		{
		  TRBSensor_Check();  //1����ʱ�������2
		} 			
		/////////////////////////////////////////////////////
        if(GpioDataRegs.GPBDAT.bit.GPIOB10==0)  //�¶Ȳ�����
        {		  
		  if(Flag_TRB_OK==0)     //β������
		  {
		    if(((Number2_Error&0x4)==0x0)&&(GpioDataRegs.GPADAT.bit.GPIOA5==0))  //���
		    {
		      TRB_Heat();         //β������
			}
			else
			{
              GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //�ر�β������
              GpioDataRegs.GPBDAT.bit.GPIOB0=0;  //�ر�β�����ƿ���			  
			}
		  }
          if(((Number2_Error&0x4)==0x0)&&((Number1_Error&0x20)==0x0))
          {
            if(GpioDataRegs.GPADAT.bit.GPIOA5==0)  //���
            {
              Delay(0xffff);
              if(GpioDataRegs.GPADAT.bit.GPIOA5==0)
			  {
                Number1_Error=Number1_Error|0x1;
                if((Flag_MRB_Heat==0)&&(Flag_MRB_OK==0))
                {
                  Heat_Time_Count();  //�������ʱ��
				  ONT_Real=ONT_Vary;
                  Flag_MRB_Heat=1;     //����׼������
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
          MRB_Heat();	  //�������ڼ���
		}
		else
		{
          GpioDataRegs.GPBDAT.bit.GPIOB11=0;  //�ر���������
          GpioDataRegs.GPBDAT.bit.GPIOB2=1;   //�ر��������ƿ���
		}
        /////////////////////////////////////////////////////////////
        if((GpioDataRegs.GPBDAT.bit.GPIOB10==1)||(Flag_TRB_OK==1))
		{
          GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //�ر�β������
          GpioDataRegs.GPBDAT.bit.GPIOB0=0;  //�ر�β�����ƿ���
		}
	  } 
	  ////////////////////////////////////////////////////////////
	  if((GpioDataRegs.GPBDAT.bit.GPIOB2==0)||(GpioDataRegs.GPBDAT.bit.GPIOB1==1))
	  {
	    GpioDataRegs.GPBDAT.bit.GPIOB13=1;    //����������ϵͳ��
	    Number2_Error=Number2_Error|0x8;
	  } 
	  else
	  {
	    GpioDataRegs.GPBDAT.bit.GPIOB13=0;    //������ϵͳ�����
        Number2_Error=Number2_Error&0xf7;
	  }   
    }  
    else   //�ϵ�BIT����
    {
	  GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //������������ʧЧ�� 
      GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //����β������ʧЧ��      
      if(Count_Light>=5)   //�ϵ�BIT���Ͽ�����ָʾ��4Hz��˸
      {
        Count_Light=0;
        GpioDataRegs.GPBDAT.bit.GPIOB9=~GpioDataRegs.GPBDAT.bit.GPIOB9;
	  }  
    }  
  }          
}  

