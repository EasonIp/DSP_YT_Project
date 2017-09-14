/* 
   * ֱ�������������������
   * �ļ�����XZ-XX.pit
   * ʱ�䣺2010.6.1
   * �汾���ڶ���
   
*/

#include "DSP28_Device.h"
#include "math.h"

///////��������///////////////
int i,j;
Uint32  count1=0;
int channel;
int	a[16];
float AD0=0.0,AD1=0.0,adclo=0.0,cesi;
float LWC,Temperature,ONTIMER,OFFTIMER,ONTIMER1,OFFTIMER1;
int ONT,OFT;
int n,max,a1;
char flag=0;  


////////��������/////////////
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

   SysCtrlRegs.PLLCR = 0x3;   //��Ƶ=30*3/2=45MHz
  
   for(i= 0; i< 5000; i++){}   // Ϊ��Ƶ���ȶ���������ʱһ��ʱ��
       
   SysCtrlRegs.HISPCP.all = 0x0002;  //��������ʱ��=45/4
   SysCtrlRegs.LOSPCP.all = 0x0002;	 //��������ʱ��=45/4
 
   SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;  //ʹ��EVAģ��ĸ�������ʱ��
   SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;  //ʹ��EVBģ��ĸ�������ʱ��
   SysCtrlRegs.PCLKCR.bit.ADCENCLK=1;  //ʹ��ADCģ��ĸ�������ʱ��
   SysCtrlRegs.PCLKCR.bit.SCIENCLKA=1; //ʹ��SCIAģ��ĵ�������ʱ��
				
   EDIS;
	
}

///////////////////IO�ڳ�ʼ��//////////////////////////////////

void InitGpio(void)
{

     EALLOW;
     GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOA����ΪI/O�� 
     GpioMuxRegs.GPADIR.all=0x3F;   //A0-A5����Ϊ�����A6-A15����Ϊ����	
     //GpioDataRegs.GPADAT.bit.GPIOA0=1;
     //GpioDataRegs.GPADAT.bit.GPIOA1=1;
     //GpioDataRegs.GPADAT.bit.GPIOA2=1;
     //GpioDataRegs.GPADAT.bit.GPIOA5=1;

     GpioMuxRegs.GPBMUX.all=0x0000;    //GPIOB����ΪI/O�� 
     GpioMuxRegs.GPBDIR.all=0x3F;   //B0-B5����Ϊ�����B6-B7����Ϊ����	
     GpioDataRegs.GPBDAT.bit.GPIOB0=1;
     GpioDataRegs.GPBDAT.bit.GPIOB1=1;
	 GpioDataRegs.GPBDAT.bit.GPIOB2=1;
	 GpioDataRegs.GPBDAT.bit.GPIOB3=1;
	 //GpioDataRegs.GPBDAT.bit.GPIOB4=1;
	 GpioDataRegs.GPBDAT.bit.GPIOB5=1;
     EDIS;

}	


///////////////////��ʱ��1��ʼ����50mS��//////////////////////////////////

void init_eva_timer1(void)
{
  
    EvaRegs.GPTCONA.all = 0;   //��ʼ����ʱ��1�����ö�ʱ��1�Ĵ���
    EvaRegs.EVAIMRA.bit.T1PINT = 1;  //����ʱ��1�����ж�
    EvaRegs.EVAIFRA.bit.T1PINT = 1;  //�����ʱ��1�����жϱ�־
             
    EvaRegs.T1CON.all = 0x170C;  //Timer1Ϊ����������ģʽ��Ԥ��ƵֵΪ128��
	                             //ʹ���ڲ���������ʱ��
	EvaRegs.T1PR = 0x112a;   //Timer1�����ڼĴ���=0.05*45MHz/4/128
	EvaRegs.T1CNT = 0x0000;  //Timer1�ļ���������

}

///////////////////��ʱ��1�ж�//////////////////////////////////

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


///////////////////AD��ʼ��//////////////////////////////////
void InitAdc(void)
{
		
	AdcRegs.ADCTRL1.bit.RESET=1;  //��λADCģ��
    for(i=0;i<5000;i++)	{}
	AdcRegs.ADCTRL1.bit.RESET=0;
	AdcRegs.ADCTRL1.bit.SUSMOD=3;
	AdcRegs.ADCTRL1.bit.ACQ_PS=0;
	AdcRegs.ADCTRL1.bit.CPS=0;     //ADCCLK=HSPCLK/2
	AdcRegs.ADCTRL1.bit.CONT_RUN=0; //ADΪ����/ֹͣģʽ
	AdcRegs.ADCTRL1.bit.SEQ_CASC=1; //16����ģʽ
  	
	AdcRegs.ADCTRL3.bit.ADCBGRFDN=3; //����϶�ο���·�ϵ�
	for(i=0;i<10000;i++)  {}          //�ȴ��ȶ�
	AdcRegs.ADCTRL3.bit.ADCPWDN=1;  //������·�ϵ�
	for(i=0;i<5000;i++)	 {}           //�ȴ��ȶ�
	AdcRegs.ADCTRL3.bit.ADCCLKPS=15;
	AdcRegs.ADCTRL3.bit.SMODE_SEL=0;  //˳�����

	
	AdcRegs.MAX_CONV.bit.MAX_CONV=15;  //����ͨ��Ϊ16
  
    AdcRegs.CHSELSEQ1.all= channel;   //ͬһ��ͨ����16��
    AdcRegs.CHSELSEQ2.all= channel;
    AdcRegs.CHSELSEQ3.all= channel;
    AdcRegs.CHSELSEQ4.all= channel;

	AdcRegs.ADCTRL2.all=0x8c04;   //����EVB��һ���¼��źŴ���ADת��

	////////////���ö�ʱ��4����ADת��/////////////////////
    EvbRegs.GPTCONB.all=0x400;    //��ʱ��4�����ж�����ADת��
    EvbRegs.EVBIMRB.bit.T4PINT = 1;  //����ʱ��4�����ж�
    EvbRegs.EVBIFRB.bit.T4PINT = 1;  //�����ʱ��4�����жϱ�־
             
    EvbRegs.T4CON.all = 0x170C;  //Timer4Ϊ����������ģʽ��Ԥ��ƵֵΪ128��
	                             //ʹ���ڲ���������ʱ��
	EvbRegs.T4PR = 0x57;     //Timer4�����ڼĴ���=0.001*45MHz/4/128
	EvbRegs.T4CNT = 0x0000;  //Timer4�ļ���������
				
	AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;
	AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;	
			
}	

///////////////////AD�ж�/////////////////////////////////

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
	 for(i=0;i<12;i++)  //ȥͷβ��2��ȡƽ��
	 { a1=a1+a[i+2];
	 }
     a1=a1/12;
	 cesi=a1*3.0/4095+adclo;

	/////////���ʹ�������//////////////////////
     SciaRegs.SCITXBUF= a1&0xff00;  //���͸�8λ
     SciaRegs.SCITXBUF= a1&0xff;   //���͵�8λ
    ///////////////////////////////////////////

	 AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;  //����жϱ�־
	 AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;	
   }
   EINT;	
}

/////////////////ʱ�����////////////////////////////////////////

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
  {  GpioDataRegs.GPADAT.bit.GPIOA1=0;  //����¶ȿ����ź�
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

/////////////////ʱ�����////////////////////////////////////////

void PWM()
{ 
   //GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //�򿪼��ȿ���
   j=0;
   EvaRegs.T1CON.bit.TENABLE=1;   //�򿪶�ʱ��1
   ONT=40;
   OFT=600;
   while(1)
   {if((0<count1)&&(count1<=ONT))
    { GpioDataRegs.GPBDAT.bit.GPIOB3=0;	   //������ȿ���
	  GpioDataRegs.GPBDAT.bit.GPIOB5=0;	   //β�����ȿ���
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
    
    EvaRegs.T1CON.bit.TENABLE=0;   //�رն�ʱ��1

}


///////////////////ⲿ�ж�1��ʼ��//////////////////////////////////

void XINT1(void)
{ GpioMuxRegs.GPEMUX.bit.XINT1_XBIO_GPIOE0=1;   //�����ⲿ�ж�1����
  XIntruptRegs.XINT1CR.all=0x0;   //�½��������ж�
}


///////////////////�ⲿ�ж�1�ж�//////////////////////////////////

interrupt void XINT1_isr(void)
{ 
  DINT;
  PieCtrl.PIEACK.bit.ACK1=1;   //�������ͬ�������ж�
  EINT;

  /////////�¶ȴ��������ϣ������¶�-20��//////////////////////////
  /*if( GpioDataRegs.GPADAT.bit.GPIOA15==1)
  { while(1)
	{ if(GpioDataRegs.GPADAT.bit.GPIOA6==1) //����澯
	  {if(GpioDataRegs.GPADAT.bit.GPIOA12==1)  //��ѯ���ȵ�Դ�Ƿ��ͨ
	   {  
         channel=0x1111;    //ADCINA1�ɼ��������
         InitAdc();

	     EvbRegs.T4CON.bit.TENABLE=1;  //�򿪶�ʱ��4��ʼ����

	     while(1)
	     {if(i==12)
	       break;
	     }
         i=0;
         AD1=cesi;
         EvbRegs.T4CON.bit.TENABLE=0;  //�رն�ʱ��4

         Count();
		 ONT=13;
		 PWM();
       }
      }
     }    
   }
  
   /////////�������̽����ϣ��ֶ�������/////////////////////////
   if( GpioDataRegs.GPADAT.bit.GPIOA7==1)
   { while(1)
	{ if(GpioDataRegs.GPADAT.bit.GPIOA6==1) //����澯
	  {if(GpioDataRegs.GPADAT.bit.GPIOA12==1)  //��ѯ���ȵ�Դ�Ƿ��ͨ
	   { 
	     channel=0x0000;    //ADCINA0�ɼ������¶�
         InitAdc();

	     EvbRegs.T4CON.bit.TENABLE=1;  //�򿪶�ʱ��4��ʼ����

	     while(1)
	     {if(i==12)
	       break;
	     }
         i=0;
         AD1=cesi;
         EvbRegs.T4CON.bit.TENABLE=0;  //�رն�ʱ��4
         
         channel=0x1111;    //ADCINA1�ɼ��ֶ�������ѹ
         InitAdc();

	     EvbRegs.T4CON.bit.TENABLE=1;  //�򿪶�ʱ��4��ʼ����

	     while(1)
	     {if(i==12)
	       break;
	     }
         i=0;
         AD1=cesi;
         EvbRegs.T4CON.bit.TENABLE=0;  //�رն�ʱ��4

         Count();
		 PWM();
       }
      }
     }    
   }
  
  //////////�������ʱ����ϻ�������ȵ�������////////////////////////////
   if((GpioDataRegs.GPBDAT.bit.GPIOB7==1)|(GpioDataRegs.GPADAT.bit.GPIOA2==0))
   { while(1)
     { GpioDataRegs.GPBDAT.bit.GPIOB3=1;  //�Ͽ�������ȿ��� 
      
	  ///////////̽����ϵͳ��������///////////////////////
        channel=0x0000;    //ADCINA0�ɼ������¶�
         InitAdc();

	     EvbRegs.T4CON.bit.TENABLE=1;  //�򿪶�ʱ��4��ʼ����

	     while(1)
	     {if(i==12)
	       break;
	     }
         i=0;
         AD1=cesi;
         EvbRegs.T4CON.bit.TENABLE=0;  //�رն�ʱ��4
         
         channel=0x1111;    //ADCINA1�ɼ��ֶ�������ѹ
         InitAdc();

	     EvbRegs.T4CON.bit.TENABLE=1;  //�򿪶�ʱ��4��ʼ����

	     while(1)
	     {if(i==12)
	       break;
	     }
         i=0;
         AD1=cesi;
         EvbRegs.T4CON.bit.TENABLE=0;  //�رն�ʱ��4
	 }
   }
  
  //////////�¶ȴ���20�ȣ�ֹͣ������////////////////////////////////
  if(GpioDataRegs.GPADAT.bit.GPIOA1==0)
  { while(1) 
    { GpioDataRegs.GPBDAT.bit.GPIOB4=1;  //�Ͽ����ȿ��Ƶ�Դ 
    }
  }
 
 //////////β�����ȵ�������///////////////////////////////////////
  if(GpioDataRegs.GPBDAT.bit.GPIOB6==1)
  { while(1)
    {GpioDataRegs.GPBDAT.bit.GPIOB5=1;  //�Ͽ�β�����ȿ��� 
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
  
  /*���ж�*/
  IER |= M_INT1;
  PieCtrl.PIEIER1.bit.INTx4=1;

  IER |= M_INT1;
  PieCtrl.PIEIER1.bit.INTx6=1;

  IER |= M_INT2;
  PieCtrl.PIEIER2.bit.INTx4=1;

  EINT;  
  ERTM;	

  ////////�ϵ��Լ��//////////////////////////
 
   GpioDataRegs.GPADAT.bit.GPIOA0=0;  //��ͨ�ϵ��Լ��Դ

   EvaRegs.T1CON.bit.TENABLE=1;   //�򿪶�ʱ��1

   while(1)         
   { if(count1>=240)  //��ʱ12S���Լ�
     {count1=0;
	  break;
	 }
   }
   
   EvaRegs.T1CON.bit.TENABLE=0;   //�رն�ʱ��1

   if(GpioDataRegs.GPADAT.bit.GPIOA15==1)//��ѯ�¶ȴ������Ƿ����
   { flag=1;
   }
   
   if(GpioDataRegs.GPADAT.bit.GPIOA7==1) //��ѯ�������̽���Ƿ����
   { flag=1;
   }
   
   if(GpioDataRegs.GPADAT.bit.GPIOA9!=1)  //��ѯK61�Լ��Ƿ����
   { flag=1;
   }
   
    if(GpioDataRegs.GPADAT.bit.GPIOA10!=1)  //��ѯK62�Լ��Ƿ����
   { flag=1;
   }

    if(GpioDataRegs.GPADAT.bit.GPIOA11!=1)  //��ѯK63�Լ��Ƿ����
   { flag=1;
   }
   
   flag=0;
   if(flag==1)
   { while(1)
     { //GpioDataRegs.GPADAT.bit.GPIOA0=1;  //�Ͽ��ϵ��Լ��Դ
	 }     
   }
   else
   { flag=0;
     //GpioDataRegs.GPADAT.bit.GPIOA5=0;   //����ϵ��Լ������
     //GpioDataRegs.GPADAT.bit.GPIOA0=1;  //�Ͽ��ϵ��Լ��Դ

	 ////////�˹��Լ��//////////////////////////
	  EvaRegs.T1CON.bit.TENABLE=1;   //�򿪶�ʱ��1

      while(1)         
      { if(count1>=120)  //��ʱ6S���˹����
        {count1=0;
	     break;
	    }
      }
   
     EvaRegs.T1CON.bit.TENABLE=0;   //�رն�ʱ��1

	 //if(GpioDataRegs.GPADAT.bit.GPIOA13==1)   //���˹����Ե�Դ
     //{ 
       GpioDataRegs.GPBDAT.bit.GPIOB2=0;  //�˹������Ե���
	   //GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //�򿪼��ȿ��Ƶ�Դ       
	   /*j=0;
       count1=0;
	   EvaRegs.T1CON.bit.TENABLE=1;   //�򿪶�ʱ��1

	   while(1)
	   { if((0<count1)&&(count1<=17))
	     { GpioDataRegs.GPBDAT.bit.GPIOB3=0;  //��������ȿ���
		   GpioDataRegs.GPBDAT.bit.GPIOB5=0;  //��β�����ȿ���
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
       EvaRegs.T1CON.bit.TENABLE=0;   //�رն�ʱ��1
       GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //�Ͽ����ȿ��Ƶ�Դ*/
       
	   EvaRegs.T1CON.bit.TENABLE=1;   //�򿪶�ʱ��1
       GpioDataRegs.GPBDAT.bit.GPIOB1=0; //��������Ե�

       while(1)         
       { if(count1>=120)    //��ʱ6S
         { GpioDataRegs.GPBDAT.bit.GPIOB1=1; //�ر�������Ե�
           count1=0;		  
	       break;
	     }
       }

       GpioDataRegs.GPBDAT.bit.GPIOB0=0; //��β�����Ե�

       while(1)         
       { if(count1>=80)    //��ʱ4S
         { GpioDataRegs.GPBDAT.bit.GPIOB0=1; //�ر�β�����Ե�
           count1=0;		  
	       break;
	     }
       }
       GpioDataRegs.GPBDAT.bit.GPIOB2=1;
       EvaRegs.T1CON.bit.TENABLE=0;   //�رն�ʱ��1

	   /////////��ѯ����澯�ź�///////////////////////

	   while(1)
	   { //if(GpioDataRegs.GPADAT.bit.GPIOA6==1) //����澯
	     //{ if(GpioDataRegs.GPADAT.bit.GPIOA12==1)  //��ѯ���ȵ�Դ�Ƿ��ͨ
		  // { 
		    /* channel=0x0000;    //ADCINA0�ɼ������¶�
             InitAdc();
			 EvbRegs.T4CON.bit.TENABLE=1;  //�򿪶�ʱ��4��ʼ����
             i=0;
			 while(1)
			 {if(i==12)
			  break;
			 }
             i=0;
             AD0=cesi;
             EvbRegs.T4CON.bit.TENABLE=0;  //�رն�ʱ��4
             
			 channel=0x1111;    //ADCINA1�ɼ��������
             InitAdc();
			 EvbRegs.T4CON.bit.TENABLE=1;  //�򿪶�ʱ��4��ʼ����
			 while(1)
			 {if(i==12)
			  break;
			 }
             i=0;
             AD1=cesi;
             EvbRegs.T4CON.bit.TENABLE=0;  //�رն�ʱ��4

             Count();*/
			 PWM();
		     
		 //  }
		// }
	  }
	//}
  }
}

