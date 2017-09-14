
#include "DSP28_Device.h"

#define RAMBASE1 0x0080000
//#define RAMBASE2 0x0080006      //������ϴ����׵�ַ
//#define RAMBASE3 0x008000a      //����״̬��Ϣ�汾���׵�ַ
//#define RAMBASE6 0x0100000

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


/////////////////////////////////////////////////////
Uint16 *rambase1=0x0;
Uint16 *rambase2=0x0;
Uint16 *rambase3=0x0;
Uint16 *rambase6=0x0;
Uint16 Write_tmp=0x0;
Uint16 Read_tmp=0x0;  
Uint16 count=0; 
Uint32 Count_System_Error=0x5678;          
Uint32 i=0;
Uint16 j2=0;
Uint32 j3=0;
Uint16 LowWord_271_Send=0xb9;
Uint16 HighWord_271_Send=0x11;
Uint16 LowWord_272_Send=0xba;
Uint16 HighWord_272_Send=0x22; 
Uint16 LowWord_273_Send=0xbb;
Uint16 HighWord_273_Send=0x33;
Uint16 LowWord_Receive_Send=0x8b;
Uint16 HighWord_Receive_Send=0x44;

////////////////////////////////////////////////////
void InitSysCtrl(void);
void InitXintf(void);
void InitGpio(void);
void Data2_Save(void);
void Data3_Save(void);
void Data6_Save(void);
void init_eva_timer1(void);
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
   SysCtrlRegs.PCLKCR.bit.ADCENCLK=1;  //ʹ��ADCģ��ĸ�������ʱ��
   SysCtrlRegs.PCLKCR.bit.ECANENCLK=1; //ʹ��CAN�����ϵͳʱ��
				
   EDIS;
	
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
	XintfRegs.XTIMING2.bit.XWRLEAD = 3;    //д���������׶ε�����=6*XTIMCLK
	XintfRegs.XTIMING2.bit.XWRACTIVE = 7;   //д��������׶εȴ�״̬������=14*XTIMCLK
	XintfRegs.XTIMING2.bit.XWRTRAIL = 3;    //д�������ٽ׶ε�����=6*XTIMCLK
   //Zone2��ʱ��
	XintfRegs.XTIMING2.bit.XRDLEAD = 3;    //�����������׶ε�����=6*XTIMCLK
	XintfRegs.XTIMING2.bit.XRDACTIVE=7;	    //����������׶εȴ�״̬������=14*XTIMCLK
	XintfRegs.XTIMING2.bit.XRDTRAIL = 3;   //���������ٽ׶ε�����=6*XTIMCLK
	//���ӱ�ʱ��
	XintfRegs.XTIMING2.bit.X2TIMING = 0; 
   //Zone2����READY�źŲ���
    XintfRegs.XTIMING2.bit.USEREADY = 0;
    XintfRegs.XTIMING2.bit.READYMODE = 0;
    XintfRegs.XTIMING2.bit.XSIZE = 3;   //16λ�ӿڣ�Ψһ��Ч������

    XintfRegs.XBANK.bit.BANK = 2;   //ʹ���ڲ�����2
    XintfRegs.XBANK.bit.BCYC = 3;  //��������֮�������3��XTIMCLK����

   //Zone6дʱ��
  /* XintfRegs.XTIMING6.bit.XWRLEAD = 3;    //д���������׶ε�����=6*XTIMCLK
   XintfRegs.XTIMING6.bit.XWRACTIVE = 7;   //д��������׶εȴ�״̬������=14*XTIMCLK
   XintfRegs.XTIMING6.bit.XWRTRAIL = 3;    //д�������ٽ׶ε�����=6*XTIMCLK
   //Zone6��ʱ��
   XintfRegs.XTIMING6.bit.XRDLEAD = 3;    //�����������׶ε�����=6*XTIMCLK
   XintfRegs.XTIMING6.bit.XRDACTIVE=7;	    //����������׶εȴ�״̬������=14*XTIMCLK
   XintfRegs.XTIMING6.bit.XRDTRAIL = 3;   //���������ٽ׶ε�����=6*XTIMCLK
   //���ӱ�ʱ��
   XintfRegs.XTIMING6.bit.X2TIMING = 0; 
   //Zone6����READY�źŲ���
   XintfRegs.XTIMING6.bit.USEREADY = 0;
   XintfRegs.XTIMING6.bit.READYMODE = 0;
   XintfRegs.XTIMING6.bit.XSIZE = 3;   //16λ�ӿڣ�Ψһ��Ч������

   XintfRegs.XBANK.bit.BANK = 6;   //ʹ���ڲ�����6
   XintfRegs.XBANK.bit.BCYC = 3;  //��������֮�������3��XTIMCLK����*/


}

///////////////////GPIO�ڳ�ʼ��//////////////////////////////////
void InitGpio(void)
{
   EALLOW;
   GpioMuxRegs.GPBMUX.all=0x0000;    //GPIOB����ΪI/O�� 
   GpioMuxRegs.GPBDIR.all=0xefdf;    //B5,B12����Ϊ���룬��������Ϊ���	
   GpioDataRegs.GPBDAT.bit.GPIOB8=0;   
   GpioDataRegs.GPBDAT.bit.GPIOB9=1; 
   GpioDataRegs.GPBDAT.bit.GPIOB15=1;     
   EDIS;
}	

///////////////////��ʱ��1��ʼ����100ms��//////////////////////////////////

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

///////////////////////////////////////////
/*void Data2_Save(void)
{ 
   *rambase2=Write_tmp&0xff; 
   for(i=0;i<0x62ff;i++){}
   j2++;
   if(j2<4)
   { rambase2++;
   }
   else
   { j2=0;
     rambase2=(Uint16 *)RAMBASE2;
   }
}

///////////////////////////////////////////
void Data3_Save(void)
{      
   *rambase3=Write_tmp&0xff; 
   for(i=0;i<0x62ff;i++){}
   j3++;
   if(j3<0x7fff5)
   {rambase3++;
   }
   else
   { j3=0;
     rambase3=(Uint16 *)RAMBASE3;
   } 
}

void Data6_Save(void)
{      
   *rambase6=Write_tmp&0xff; 
   for(i=0;i<0x62ff;i++){}
   rambase6++;

}*/


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




///////////////////////////////////////////////////
main()
{  InitSysCtrl();
   DINT;
   IER = 0x0000;
   IFR = 0x0000;

   InitPieCtrl();
   InitPieVectTable();
   InitXintf();
   InitGpio();

   EALLOW;
   PieVectTable.T1PINT=&eva_timer1_isr;
   EDIS;  
  
   //���ж�
   IER |= M_INT2;
   PieCtrl.PIEIER2.bit.INTx4=1;
   EINT;  
   ERTM; 


   //rambase2=(Uint16 *)RAMBASE2;
   //rambase3=(Uint16 *)RAMBASE3;
   rambase1=(Uint16 *)RAMBASE1;

   Write_tmp=0x1;
   EvaRegs.T1CON.bit.TENABLE=1;  // �򿪶�ʱ��1
     
   for(;;)
   { 
//     if(count>=20)
//     {
//       count=0;
       *rambase1=Write_tmp&0xff; 
       for(i=0;i<0x62ff;i++){}
       rambase1++;



       /*GpioDataRegs.GPBDAT.bit.GPIOB8=1;
       GpioDataRegs.GPBDAT.bit.GPIOB9=0;   //�򿪶�д����B9=0
       for(i=0;i<0x2ffff;i++);
       Write_tmp=(Uint16)(Count_System_Error&0xff);   //�����ܹ��ϴ���
       Data2_Save();		   
       Write_tmp=(Uint16)((Count_System_Error&0xff00)>>8);
       Data2_Save();
       Write_tmp=(Uint16)((Count_System_Error&0xff0000)>>16);
       Data2_Save();
       Write_tmp=(Uint16)((Count_System_Error&0xff000000)>>24);
       Data2_Save();


       Write_tmp=LowWord_271_Send&0xff;    //����429�ź�
       Data3_Save();
       Write_tmp=LowWord_271_Send>>8;
       Data3_Save();
       Write_tmp=HighWord_271_Send&0xff;    
       Data3_Save();
       Write_tmp=HighWord_271_Send>>8;
       Data3_Save();

       Write_tmp=LowWord_272_Send&0xff;    
       Data3_Save();
       Write_tmp=LowWord_272_Send>>8;
       Data3_Save();
       Write_tmp=HighWord_272_Send&0xff;    
       Data3_Save();
       Write_tmp=HighWord_272_Send>>8;
       Data3_Save();

       Write_tmp=LowWord_273_Send&0xff;    
       Data3_Save();
       Write_tmp=LowWord_273_Send>>8;
       Data3_Save();
       Write_tmp=HighWord_273_Send&0xff;    
       Data3_Save();
       Write_tmp=HighWord_273_Send>>8;
       Data3_Save();

       Write_tmp=LowWord_Receive_Send&0xff;    
       Data3_Save();
       Write_tmp=LowWord_Receive_Send>>8;
       Data3_Save();
       Write_tmp=HighWord_Receive_Send&0xff;    
       Data3_Save();
       Write_tmp=HighWord_Receive_Send>>8;
       Data3_Save();


       GpioDataRegs.GPBDAT.bit.GPIOB9=1;  //�رն�д���B9=1
       GpioDataRegs.GPBDAT.bit.GPIOB8=0;
       EINT; */                
//     }
   }
}
