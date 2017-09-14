

#include "DSP28_Device.h"
#include "math.h"


#define RAMBASE1 0x080000      //���ߵ�ַ,��Ӧ��Ƭѡ�ź�XZCS2 

#define ID1 0x01200000       //����������͵��ҿ�����MSGID
#define ID2 0x01300000       //����������͵�������MSGID
#define ID3 0x02100000       //�ҿ��������͵��������MSGID
#define ID4 0x03100000       //���������͵��������MSGID


extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;  

#pragma CODE_SECTION(InitFlash, "ramfuncs");

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


////////////////��������/////////////////////////////////               
Uint32 i=0;                    //��ʱ����
Uint32 j[6]={0,0,0,0,0,0};     
Uint16 Write_tmp=0;			    //�����ⲿ�ӿ�д��������
Uint16 Read_tmp=0;              //�����ⲿ�ӿڶ���������
Uint16 *rambase1;              //����Zone6�ⲿ�ӿ�ָ�����
Uint16 LowWord_271_Send;        //����LABLE271���ֽ�
Uint16 HighWord_271_Send;       //����LABLE271���ֽ�
Uint16 LowWord_272_Send;        //����LABLE272���ֽ�
Uint16 HighWord_272_Send;        //����LABLE272���ֽ�
Uint16 LowWord_273_Send;         //����LABLE273���ֽ�
Uint16 HighWord_273_Send;        //����LABLE273���ֽ�
Uint16 LowWord_300_Send;          //��������汾��300���ֽ�
Uint16 HighWord_300_Send;         //��������汾��300���ֽ�
Uint16 LowWord_301_Send;          //��������汾��301���ֽ�
Uint16 HighWord_301_Send;         //��������汾��301���ֽ�
Uint16 LowWord_302_Send;         //��������汾��302���ֽ�
Uint16 HighWord_302_Send;         //��������汾��302���ֽ�
Uint16 LowWord_303_Send;          //��������汾��303���ֽ�
Uint16 HighWord_303_Send;         //��������汾��303���ֽ�
Uint16 LowWord_304_Send;           //����Ӳ���汾��304���ֽ�
Uint16 HighWord_304_Send;          //����Ӳ���汾��304���ֽ�
Uint16 LowWord_305_Send;           //����Ӳ���汾��305���ֽ�
Uint16 HighWord_305_Send;          //����Ӳ���汾��305���ֽ�
Uint16 LowWord_306_Send;           //����Ӳ���汾��306���ֽ�
Uint16 HighWord_306_Send;          //����Ӳ���汾��306���ֽ�
Uint16 LowWord_307_Send;           //����Ӳ���汾��307���ֽ�
Uint16 HighWord_307_Send;          //����Ӳ���汾��307���ֽ�
Uint16 LowWord_310_Send;           //����Ӳ���汾��310���ֽ�
Uint16 HighWord_310_Send;          //����Ӳ���汾��310���ֽ�
Uint32 TestMbox1=0x0;			   //����2���յ�����Ϣ
Uint32 TestMbox2=0x0;     
Uint32 TestMbox3=0x0;			   //����3���յ�����Ϣ
Uint32 TestMbox4=0x0;  
    
char  Flag_Controller_Judge=0;
/////////////////��������/////////////////////////////////

void InitSysCtrl(void);
void InitGpio(void);
void InitXintf(void);
void InitECan(void);
void VersionNumber_Hardware_Get(void);
void Data1_Save(void);
void Data1_Read(void);
interrupt void ECAN0_ISR(void);


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
   for(i=0; i<5000; i++){}   // Ϊ��Ƶ���ȶ�������ӻ
      
   SysCtrlRegs.HISPCP.all = 0x0002;  //��������ʱ��=120/4=30MHz
   SysCtrlRegs.LOSPCP.all = 0x0002;	 //��������ʱ��=120/4=30MHz
 
   SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;  //ʹ��EVAģ��ĸ�������ʱ��
   SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;  //ʹ��EVBģ��ĸ�������ʱ��
   SysCtrlRegs.PCLKCR.bit.ADCENCLK=1;  //ʹ��ADCģ��ĸ�������ʱ��
   SysCtrlRegs.PCLKCR.bit.ECANENCLK=1; //ʹ��CAN�����ϵͳʱ��
				
   EDIS;	
}

///////////////////GPIO�ڳ�ʼ��//////////////////////////////////
void InitGpio(void)
{
   EALLOW;
   GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOA����ΪI/O�� 
   GpioMuxRegs.GPADIR.all=0xffff;   //A0-A15����Ϊ���	
   GpioDataRegs.GPADAT.all=0x23e; 

   GpioMuxRegs.GPBMUX.all=0x0000;    //GPIOB����ΪI/O�� 
   GpioMuxRegs.GPBDIR.all=0xefdf;    //B5,B12����Ϊ���룬��������Ϊ���	
   GpioDataRegs.GPBDAT.all=0x8221;   

    
   GpioMuxRegs.GPFMUX.all=0x00c0;  //����GPIOF6/7ΪeCAN��,��������ΪI/O��
   GpioMuxRegs.GPFDIR.all=0xa00;
   GpioDataRegs.GPFDAT.bit.GPIOF9=1;
   GpioDataRegs.GPFDAT.bit.GPIOF11=0;
   EDIS;
}	

	
////////////////////�ⲿ�ӿ�XINTF��ʼ��/////////////////////////
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
	XintfRegs.XTIMING6.bit.XWRTRAIL = 3;    //д�������ٽ׶ε�����=XTIMCLK
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
		
	ECanaRegs.CANME.all = 0;  //��ֹ�������

      ECanaRegs.CANMD.bit.MD0=0;  //����0���͵��ҿ�����
      ECanaRegs.CANMD.bit.MD1=0;  //����1���͵�������
      ECanaRegs.CANMD.bit.MD2=1;  //����2�����ҿ�������Ϣ
      ECanaRegs.CANMD.bit.MD3=1;  //����3������������Ϣ
     	
	if(Flag_Controller_Judge==0)
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
    ECanaRegs.CANMC.bit.DBO = 0; //���Ƚ��ջ������ݵ�����ֽ�
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


/////////////ͨ���������ⲿ�洢��дӲ���汾��////////////////////
void Data1_Save(void)
{  
   *rambase1=Write_tmp&0xff; 	//������Read_tmpд������������
   for(i=0;i<0xffff;i++){}

   j[0]++;
   if(j[0]<7)
   { rambase1++;  
   }
   else
   { rambase1=(Uint16 *)RAMBASE1;
     j[0]=0;
   } 
}


/////////////ͨ�����߶�ȡ�ϴι��ϴ���////////////////////
void Data1_Read(void)
{  
   Read_tmp=(*rambase1)&0xff;       //�����������ϵ����ݶ�������Read_tmp��
   for(i=0;i<0xffff;i++){}
   j[3]++;
   if(j[3]<7)
   { rambase1++;  
   }
   else
   { rambase1=(Uint16 *)RAMBASE1;
     j[3]=0;
   }
}

///////////////��ȡӲ���汾��///////////////////////////////
void VersionNumber_Hardware_Get(void)
{  
   Uint16 Version_Number1=0;         //Ԥ��Ӳ���汾�ŵ�1λ
   Uint16 Version_Number2=0;         //Ԥ��Ӳ���汾�ŵ�2λ
   Uint16 Version_Number3=0;         //Ԥ��Ӳ���汾�ŵ�3λ
   Uint16 Version_Number4=0;         //Ԥ��Ӳ���汾�ŵ�4λ
   Uint16 Version_Number5=0;         //Ԥ��Ӳ���汾�ŵ�5λ
   Uint16 Version_Number6=0;         //Ԥ��Ӳ���汾�ŵ�6λ
   Uint16 Version_Number7=0;         //Ԥ��Ӳ���汾�ŵ�7λ
   
   DINT;
   GpioDataRegs.GPBDAT.bit.GPIOB8=1;
   GpioDataRegs.GPBDAT.bit.GPIOB9=0;   //�򿪶�д����B9=0
   for(i=0;i<0x5ffff;i++) {}
   rambase1=(Uint16 *)RAMBASE1;    
   Data1_Read();
   Version_Number1=Read_tmp;
   Data1_Read();
   Version_Number2=Read_tmp;   
   Data1_Read();
   Version_Number3=Read_tmp;
   Data1_Read();
   Version_Number4=Read_tmp;
   Data1_Read();
   Version_Number5=Read_tmp;
   Data1_Read();
   Version_Number6=Read_tmp;
   Data1_Read();
   Version_Number7=Read_tmp;
   GpioDataRegs.GPBDAT.bit.GPIOB9=1;   //�رն�д����B9=1
   GpioDataRegs.GPBDAT.bit.GPIOB8=0;
   EINT;

   LowWord_306_Send=((Version_Number1&0x1f)<<11)+0xc6;
   HighWord_306_Send=((Version_Number3&0x7f)<<9)+((Version_Number2&0x7f)<<2)+((Version_Number1&0x60)>>5);

   LowWord_307_Send=((Version_Number4&0x1f)<<11)+0xc7;
   HighWord_307_Send=((Version_Number6&0x7f)<<9)+((Version_Number5&0x7f)<<2)+((Version_Number4&0x60)>>5);
 
   LowWord_310_Send=((Version_Number7&0x1f)<<11)+0xc8;
   HighWord_310_Send=(HighWord_310_Send&0xfffc)+((Version_Number7&0x60)>>5);
 
   rambase1=(Uint16 *)RAMBASE1;
}



///////////////////CAN�ж�������?//////////////////////////////
interrupt void ECAN0_ISR(void)
{   
  DINT;
  if(PieCtrl.PIEACK.bit.ACK9==1)
  { PieCtrl.PIEACK.bit.ACK9=1;

   //////////////////////////////////////////////////////////////
    if(ECanaRegs.CANRMP.bit.RMP3==1)  //������������Ϣ
    { ECanaRegs.CANRMP.bit.RMP3=1;
     
       TestMbox3=ECanaMboxes.MBOX3.MDRL.all;
       TestMbox4=ECanaMboxes.MBOX3.MDRH.all;
        
         //////���յ�Ӳ���汾��//////////////
        if((TestMbox4&0xff000000)==0x58000000)
		{ //////////Ӳ���汾�ű���///////////////////
		  GpioDataRegs.GPBDAT.bit.GPIOB8=1;
		  GpioDataRegs.GPBDAT.bit.GPIOB9=0;   //�򿪶�д����B9=0		  
          for(i=0;i<0xffff;i++) {}
          rambase1=(Uint16 *)RAMBASE1; 
		  
          Write_tmp=(Uint16)((TestMbox4&0xff0000)>>16); 
          Data1_Save();
          Write_tmp=(Uint16)((TestMbox4&0xff00)>>8);
          Data1_Save();
          Write_tmp=(Uint16)(TestMbox4&0xff);
          Data1_Save();
          Write_tmp=(Uint16)((TestMbox3&0xff000000)>>24);
          Data1_Save();
          Write_tmp=(Uint16)((TestMbox3&0xff0000)>>16); 
          Data1_Save();
          Write_tmp=(Uint16)((TestMbox3&0xff00)>>8); 
          Data1_Save(); 
          Write_tmp=(Uint16)(TestMbox3&0xff); 
          Data1_Save();	
          	  
		  //GpioDataRegs.GPBDAT.bit.GPIOB9=1;   //�򿪶�д����B9=0 
		  //GpioDataRegs.GPBDAT.bit.GPIOB8=0;           
		}	    
    }	
   }
   EINT;
}



/////////////������//////////////////////////////////////

main()
{  InitSysCtrl();
	//��InitFlash�����������Ƶ�ram������
   MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);	
   InitFlash();

   DINT;
   IER = 0x0000;
   IFR = 0x0000;

   InitPieCtrl();
   InitPieVectTable();
   InitGpio();	
   InitXintf();
   //��/�ҿ����ж�//////////////////////
   if(GpioDataRegs.GPFDAT.bit.GPIOF13==1)  
   { Flag_Controller_Judge=1;         //�ҿ�����
   }
   else
   { Flag_Controller_Judge=0;        //�������	 
   } 


   InitECan(); 
    		
   EALLOW;
   PieVectTable.ECAN0INTA = &ECAN0_ISR;
   EDIS;  
  
   //���ж�
   IER |= M_INT9;
   PieCtrl.PIEIER9.bit.INTx5=1;
   EINT;  
   ERTM;

   LowWord_271_Send=0x00b9;   //��ʼ��429�ź�
   HighWord_271_Send=0x0000;  
   LowWord_272_Send=0x40ba;   
   HighWord_272_Send=0x0000;
   LowWord_273_Send=0x00bb;  
   HighWord_273_Send=0x0001;
   LowWord_300_Send=0x98c0;  
   HighWord_300_Send=0x62c1;
   LowWord_301_Send=0x90c1;  
   HighWord_301_Send=0x62c1;
   LowWord_302_Send=0x90c2;    
   HighWord_302_Send=0x64c1;
   LowWord_303_Send=0x88c3;  
   HighWord_303_Send=0x1;  
   LowWord_304_Send=0x98c4;  
   HighWord_304_Send=0x62c1;
   LowWord_305_Send=0x90c5;  
   HighWord_305_Send=0x62c1;
   LowWord_306_Send=0x88c6;  
   HighWord_306_Send=0x62c5;
   LowWord_307_Send=0x90c7;  
   HighWord_307_Send=0x60c1; 
   LowWord_310_Send=0xc8c8;  
   HighWord_310_Send=0xc5; 
     
    //////��ȡԤ��Ӳ���汾��///////////////
   VersionNumber_Hardware_Get();
         
   for(;;){}

}