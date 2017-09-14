#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "math.h"
#define adcclk 0x03
#define EN_485        GpioDataRegs.GPADAT.bit.GPIO10          //485ʹ���ź�
#define DDOG          GpioDataRegs.GPBDAT.bit.GPIO55          //DSP��������ָʾ��
#define led_run_d     GpioDataRegs.GPADAT.bit.GPIO25          //����ָʾ��
#define led_stop_d    GpioDataRegs.GPADAT.bit.GPIO24          //ָֹͣʾ��
#define led_err_d     GpioDataRegs.GPADAT.bit.GPIO23          //����ָʾ��
//#define contactorK1   
//#define contactorK2
//#define fanK3       
#define pi 3.14159265359


Uint16    *CommandStart = (Uint16 *)0x100001;       //Start Command address
Uint16    *CommandStop  = (Uint16 *)0x100002;       //Stop  Command address
 
Uint16     *Cmp1Write   = (Uint16 *)0x100011;       //���Ʋ��ź�
Uint16     *Cmp2Write   = (Uint16 *)0x100012;
Uint16     *Cmp3Write   = (Uint16 *)0x100013;

Uint16    *IgbtFaultRead = (Uint16 *)0x100021;       //IGBT�����źŵ�ַ


struct IGBTFault_BITS{      //bits  description     //�����źżĴ�������
    Uint16  Aup:1;     //0  Aup fault  
	Uint16  Ado:1;     //1  Adown fault
	Uint16  Bup:1;     //2  Bup fault
	Uint16  Bdo:1;     //3  Bdown fault
	Uint16  Cup:1;     //4  Cup fault
	Uint16  Cdo:1;     //5  Cdown fault
	Uint16  foef:1;    //6  ��żУ�����
	Uint16  rxf:1;     //7  ����ͨ�Ź���
	Uint16  rsvd:8;   //15:8 reserved
};
union IGBTFault_REG{
    Uint16            all;
	struct IGBTFault_BITS bit;
};
union IGBTFault_REG igbtfault;

struct FaultStaSec_BITS{
    Uint16  faultf:1;      //0  ��������
    Uint16  uovf:1;        //1  ĸ�߹�ѹ
	Uint16  ubvf:1;        //2  ĸ��Ƿѹ
	Uint16  igbtf:1;       //3  IGBT����
	Uint16  otf:1;         //4  ���¹���
	Uint16  socf:1;        //5  ģ�����
	Uint16  contactor1:1;  //6  �Ӵ���1״̬
	Uint16  contactor2:1;  //7  �Ӵ���2״̬
	Uint16  fan:1;         //8  ����״̬
	Uint16  rxf_foef:1;    //9  ���˽��ܹ���(�Ӱ�)
	Uint16  rsstate:2;     //11:10 
	Uint16  rsvd:4;        //15:12 reserved
};
union FaultStaSec_REG{
    Uint16  all;
	struct FaultStaSec_BITS bit;
};
union FaultStaSec_REG faultstasec;

Uint16 uovf=0,ubvf=0,igbtf=0,otf=0,socf=0,contactor1=0,contactor2=0,fan=0,rsstate=0,rxf_foef=0;

struct ControlSta_BITS{
    Uint16  start_stop:2;    //0:1 �������1 ���ֹͣ2
    Uint16  charg_chargof:2; //2:3 ���1 �ϵ�2
    Uint16  reset:1;         //4   ��λ1
    Uint16  rsvd:11;         //����
};
union ControlSta_REG{
    Uint16  all;
    struct  ControlSta_BITS bit;
};
union ControlSta_REG controlstasec;                             //����ָ��

extern int BytleValue[];

Uint16 i=0,j=0,k=0;                                 //һЩ���õı���
float SinTable[1400];										  //���ұ�
float Sin5Table[600];
float Sin7Table[600];
//float Sin11Table[600];
float Iddata[400];
float Iqdata[400];
float yucea[405];
float yuceb[405];
float yucec[405];
float ceshi1[400];
float ceshi2[400];

float  UBCAV=380,UBCSUM=0,UABAV=380,UABSUM=0,UCAAV=380,UCASUM=0;
float  ILASUM=0,ILBSUM=0,ILCSUM=0;
float  ILAAV=0,ILBAV=0,ILCAV=0;
float  ICOM1SUM=0,ICOM2SUM=0,ICOM3SUM=0;
float  ICOM1AV=0,ICOM2AV=0,ICOM3AV=0,Udcav=0;

Uint16 UAB_temp,UCB_temp,UCA_temp,ILA_temp,ILB_temp,ILC_temp,Tem1_temp,Tem2_temp;   //������ѹ��ֱ�����ѹ���¶ȵĲ���ֵ
Uint16 Udc_temp,ICOM1_temp,ICOM2_temp,ICOM3_temp;                 //���ص����Ͳ��������Ĳ���ֵ

float  UAB,UBC,UCA,ILA,ILB,ILC,Udc,Tem1,Tem2,Temax,ICOM1,ICOM2,ICOM3;      //������ѹ��ֱ�����ѹ���¶ȵ�ʵ��ֵ�͸��ص����Ͳ���������ʵ��ֵ                                  //
float  Tesecmax;
float  UABsim,UBCsim,ILAsim,ILBsim,ILCsim,ICOM1sim,ICOM2sim,ICOM3sim,Udcavsim;//ʵ��ֵת���������е�ֵ


float Id,Iq;                                                  //abc-dq0�е�dq����
float Idav=0,Iqav=0;                                          //��dq�������е�ͨ�˲�
float IAHam,IBHam,ICHam;
float IHam_ref1,IHam_ref2,IHam_ref3;  
float Iref1se,Iref2se,Iref3se;                                //ͨ������ѡ��õ���ָ�����

Uint16 DelayCount=0;                                          //������ʱ����
Uint16 rsorder = 0;                                          //ģ���������ָ�� 	              
Uint16 pllt,prdtemp;										  //PLL�жϷ����ӳ�������жϴ�����cputimer0�ļ���ֵ
Uint16 pllflag=0;											  //PLL��־                                           
Uint16 fmainflag=0;
Uint16 resetcount=0,resetflag=0;
float UBCPLL;
Uint16 yucecount=0;

Uint16 Operamode=1;//1��ʾ��ѹ ��2�������ԣ�3�������ԣ�4����г����5��̬��г��
float Capval=0,Indval=0;
float Ham1val=0,Ham3val=0,Ham5val=0,Ham7val=0,Ham9val=0,Ham11val=0,Ham13val=0;
Uint16 DynHam3=0,DynHam5=0,DynHam7=0,DynHam9=0,DynHam11=0,DynHam13=0,DynHamHi=0,DynHamAll=0;
Uint16 Wiring=1;// 1��ʾ�������� 2��ʾ��������  ��ʱûʹ��
Uint16 Controlmode=1;//1��ʾ��⸺�ص�����2��ʾ������������3��ʾ���Ͽ��Ʒ�ʽ   ��ʱûʹ��
float L1val=0.1,L2val=0.2;
float CT1Prop,CT2Prop,Hall1Prop,Hall2Prop;
float UdcTarget=700;
float UdcKp=3.96,UdcKi=0.00335,IKp=6;                                                    //����������ϵ��;
Uint16 SwitchFre=10000;
float Udcovval=800,Udcbvval=520,Isocval=70,Usovval=420,Usbvval=360,Totval=80;
float Gridpowfac=0,Loadpowfac=0,THDGrid=0,THDLoad=0;
Uint16 contactor1,contactor2,fan;

float Udci=0,Udcpi=0;
float Udct;                                                   //ֱ����ѹ���
float Iref1dc,Iref2dc,Iref3dc;                                      //��ѹ����ָ��
float Iref1se,Iref2se,Iref3se;
float Iref1,Iref2,Iref3;   
float Iref1pi,Iref2pi,Iref3pi;                                //�����������������
float Uref1,Uref2,Uref3;                                      //���Ʋ���ѹָ��
//float cmp1,cmp2,cmp3;
//float Di1,Di2,Di3;                                            //Ԥ�����ָ��


Uint16 Time0Count=0;
Uint16 faultf  = 0;											  //���󣨹��ϣ���־
Uint16 state = 0;                                           //����״̬
Uint16 ftime   = 0;								              //���ط�����������е�����ã�
Uint16 runcount= 0;                                           //���е���˸���������������
Uint16 dataflag= 0;											  //�������ݱ�־
Uint16 chargflag=0;                                           //��ϵ��־λ

interrupt void cpu_timer0_isr(void);                          //cpu��ʱ��0�жϷ����ӳ���
interrupt void scibrx(void);                                  //SCIͨ�Ž����ж�
interrupt void adcisr(void);                                  //AD���������ӳ���
interrupt void Zero_cap_isr(void);                            //������ѹ���㲶׽�ж�

void maindataset(void);
void controlset(void);
void datadeal(void);
void fcheck(void);                                           //���ϵͳ��ѹ��Ƶ��
void secdeal(void);
void fcheck1(void);                                           //���ֱ��ĸ���Ƿ�Ƿѹ�����£�IGBT�Ƿ����
void fcheck2(void);                                           //���ֱ��ĸ���Ƿ��ѹ������
void start(void);
void stop(void);

void LPFCAL(float Id_in,float Iq_in);

float Idavtemp=0;
float Iqavtemp=0;
Uint16 lpfcount=0;

void main(){
    InitSysCtrl();                                            //��ʼ��ϵͳ����
    EALLOW;
    SysCtrlRegs.HISPCP.all=adcclk;                            //25M
    EDIS;                                           
	memcpy(&ramfuncsRunStart, &ramfuncsLoadStart, &ramfuncsLoadEnd-&ramfuncsLoadStart);
	InitFlash();                                              //��ʼ��FLASH
    InitXintf16Gpio();                                        //����GPIO
	InitScibGpio();
	DINT;
	InitPieCtrl();											  //��ʼ���ж�
	IER=0x0000;
	IFR=0x0000;																
	InitPieVectTable();                                       //��ʼ���ж�������
	EALLOW;
	PieVectTable.TINT0  = &cpu_timer0_isr;                    //CPU��ʱ��0�жϷ������
	PieVectTable.SCIRXINTB=&scibrx;                           //SCIͨѶ�����ж�
	PieVectTable.ADCINT = &adcisr;                            //AD�����жϷ������
	PieVectTable.XINT2  = &Zero_cap_isr;                      //������ѹ���㲶׽�ж�
	EDIS;
	InitCpuTimers();										  //��ʼ��cpu��ʱ��
	ConfigCpuTimer(&CpuTimer0,150,50);                        //CPU��ʱ��0����
    scib_fifo_init();                                         // ��ʼ��SCI FIFO
	SciInit();
	Initcmptx();
	InitAdc();
	//���¼����ʼ��ADת��
	AdcRegs.ADCTRL1.bit.ACQ_PS=0x5;
	AdcRegs.ADCTRL2.bit.RST_SEQ1=1;
	AdcRegs.ADCTRL1.bit.SEQ_CASC=1;
	AdcRegs.ADCTRL1.bit.CONT_RUN=0;
	AdcRegs.ADCTRL1.bit.SEQ_OVRD=0;
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1=1;
	AdcRegs.ADCCHSELSEQ1.all = 0x0;
	AdcRegs.ADCCHSELSEQ2.all = 0x0;
	AdcRegs.ADCCHSELSEQ3.all = 0x0;
	AdcRegs.ADCCHSELSEQ4.all = 0x0;


	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x7;     //UAN
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x6;     //UBN
	AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x5;     //UCN
	AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x4;     //UDC1
	AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0x3;     //UDC2
	AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 0x2;     //IASYS
	AdcRegs.ADCCHSELSEQ2.bit.CONV06 = 0x1;     //IBSYS
	AdcRegs.ADCCHSELSEQ2.bit.CONV07 = 0x0;     //ICSYS
	AdcRegs.ADCCHSELSEQ3.bit.CONV08 = 0x8;     //TEM
	AdcRegs.ADCCHSELSEQ3.bit.CONV09 = 0x9;     //I1
	AdcRegs.ADCCHSELSEQ3.bit.CONV10 = 0xa;     //I2
	AdcRegs.ADCCHSELSEQ3.bit.CONV11 = 0xb;     //I3
	AdcRegs.ADCCHSELSEQ4.bit.CONV12 = 0xc;     //I4
	AdcRegs.ADCCHSELSEQ4.bit.CONV13 = 0xd;     //I5
	AdcRegs.ADCCHSELSEQ4.bit.CONV14 = 0xe;     //I6
	AdcRegs.ADCCHSELSEQ4.bit.CONV15 = 0xf;     //I7
    AdcRegs.ADCMAXCONV.all = 0x000F;           //16��ͨ��ת��

	for(i=0;i<1400;i++)
    {
        SinTable[i]=sin(2*pi*i/400);										//�������ұ�
	}
	for(i=0;i<600;i++)
	{
	    Sin5Table[i]=sin(2*pi*i/80);
	}
	for(i=0;i<600;i++)
	{
	    Sin7Table[i]=sin(2*pi*i/57);
	}
	for(i=0;i<405;i++)
	{
	    yucea[i]=0;
	    yuceb[i]=0;
	    yucec[i]=0;
	}
/*	for(i=0;i<600;i++)
	{
	    Sin9Table[i]=sin(2*pi*i/44);
	}*/
/*	for(i=0;i<600;i++)
	{
	    Sin11Table[i]=sin(2*pi*i/36);
	}*/
/*	for(i=0;i<600;i++)
	{
	    Sin13Table[i]=sin(2*pi*i/31);
	}*/ 
	for(i=0;i<400;i++) Iddata[i]=0;
	for(i=0;i<400;i++) Iqdata[i]=0;
	i=0;
	igbtfault.all = 0x0000;
	faultsta.all = 0x0000;
	faultstasec.all=0x0000;
	faultstasecall.all=0x0000;
	controlstasec.all = 0x0000;
 
	EnableDog();
	IER|=M_INT1;
 	IER|=M_INT9;
	PieCtrlRegs.PIEIER1.bit.INTx7  = 1;
	PieCtrlRegs.PIEIER9.bit.INTx3  = 1;
	PieCtrlRegs.PIEIER1.bit.INTx6  = 1;
	PieCtrlRegs.PIEIER1.bit.INTx5  = 1;
	EINT;													 //���ϼ��俪�ж�
	ERTM;
	StartCpuTimer0();                                         //������ʱ��0
    stop();
	EN_485=0;
	DELAY_US(1E5);                                                                                                           																
    for(;;)
	{
	    if(runcount>=10000)                                        //DSP������˸��һ����������
 	    {
	       DDOG=~DDOG;
		   runcount=0;
	    }

        fcheck();                                                 //���ϼ��

		secdeal();

		if(BytleValue[77]==1)                                     //��ģ������źŽ��и�λ
		{
			controlstasec.bit.reset=1;
			resetflag=1;
		}
				     
	    if(resetcount==4)                                        //��λ�������� ��200us�ĸ�λʱ��                 
		{
		    controlstasec.bit.reset=0;
	        BytleValue[77]=0;
		    resetflag=0;
		}

		if(BytleValue[78]==1)                                       // ģ����
		    controlstasec.bit.charg_chargof=1;
		else  controlstasec.bit.charg_chargof=2;

		if(faultstasec.bit.faultf==1) BytleValue[78]=0;
		   
		if(rsorder==1)                                                //�������ָ��
		{
			if(state==0) start();
			if(state==1&&faultstasec.bit.faultf==1)
			{
			   stop();
			   BytleValue[79]=0;                                      //װ�����й����г���������������������ָ������
			}
		}
		else if(state==1) stop();

		if(controlstasec.bit.reset==1)                        //��λ
		{
		    faultf=0;
			uovf=0;
			socf=0;
			igbtf=0;
		}

		if(controlstasec.bit.charg_chargof==1&&faultf==0&&chargflag==0)  //ģ����
		{
//		    contactorK1=1;
//			contactorK2=0;
            contactor1=1;
			contactor2=0;
			chargflag=1;
			rsstate=1;
		}

		if(controlstasec.bit.charg_chargof==2&&chargflag==1||faultf==1)  //ģ��ϵ�
		{
		    chargflag=0;
			stop();
//		    contactorK1=0;
//			contactorK2=0;
            contactor1=0;
   			contactor2=0;
		}

        fcheck1();                                                    
        
		datadeal();

		if(crcflag==1) modbusdeal();

		cmptxdeal();
    }
}

interrupt void cpu_timer0_isr(void)
{   
    ServiceDog();
	runcount++;
	
	if(rsorder==1) ftime++;                                          //������ʱ
	else ftime=0;
	if(resetflag==1) resetcount++;                                   //��λ��ʱ
	else resetcount=0;
	if(ftime>30000) ftime=30000;
	
	txcount++;
	if(txcount>10000)txcount=10000;
	txfree++;
	if(txfree>10000) txfree=10000;
	if(ScibRegs.SCIFFRX.bit.RXFFST!=0&&dust==0) mbcount++;
	else mbcount=0;
	if(mbcount>300){
		dust=ScibRegs.SCIFFRX.bit.RXFFST;
		ScibRegs.SCIFFRX.bit.RXFFIL=dust+8;				
		mbcount=0;
	}
	if(ScibRegs.SCIRXST.bit.RXERROR){
		ScibRegs.SCICTL1.bit.SWRESET=0;
		asm(" RPT #7 || NOP");
		ScibRegs.SCICTL1.bit.SWRESET=1;
	}

	if(BytleValue[83]==1)
	{
	    maindataset();
	//	secdataset();
		BytleValue[83]=0;
	} 
	if(BytleValue[84]==1)
	{
	    controlset();
		BytleValue[84]=0;
	}
	                                                     
    AdcRegs.ADCTRL2.bit.SOC_SEQ1=1;
	CpuTimer0.InterruptCount++;	  
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;
	CpuTimer0Regs.TCR.bit.TIF   = 1;                                               
}

interrupt void adcisr(void)
{
    UAB_temp	= (AdcRegs.ADCRESULT0>>4);
	UCB_temp	= (AdcRegs.ADCRESULT1>>4);
	Udc_temp	= (AdcRegs.ADCRESULT3>>4);
	
	ICOM1_temp	= (AdcRegs.ADCRESULT5>>4);
	ICOM2_temp	= (AdcRegs.ADCRESULT6>>4);
	ICOM3_temp	= (AdcRegs.ADCRESULT7>>4);
	
	ILA_temp = (AdcRegs.ADCRESULT9>>4);
	ILB_temp = (AdcRegs.ADCRESULT10>>4);
	ILC_temp = (AdcRegs.ADCRESULT11>>4);

    Time0Count=CpuTimer0.InterruptCount+110;                     //��λͬ��
	
	UAB = ((float)UAB_temp - 2067)*0.53267045;               //����ϵ������ݴ������Ͳ��������·����
	UBC		= ( 2074 - (float) UCB_temp )*0.53267045;
	Udc		= ( (float) Udc_temp ) * 0.46875;	
    ICOM1	= ( (float) ICOM1_temp - 2070 ) * 0.1148897;
	ICOM2	= ( (float) ICOM2_temp - 2068 ) * 0.1148897;
	ICOM3	= ( (float) ICOM3_temp - 2069 ) * 0.1148897;
	UCA=-UAB-UBC;   

	Id=0.6667*(ILA*SinTable[Time0Count]+ILB*SinTable[Time0Count+267]+ILC*SinTable[Time0Count+133]);
	Iq=0.6667*(ILA*SinTable[Time0Count+100]+ILB*SinTable[Time0Count+367]+ILC*SinTable[Time0Count+233]);
    
	LPFCAL(Id,Iq);                                       //��ͨ�˲���
    
    IHam_ref1=Idav*SinTable[Time0Count]+Iqav*SinTable[Time0Count+100];         //��������
	IHam_ref2=Idav*SinTable[Time0Count+267]+Iqav*SinTable[Time0Count+367];
	IHam_ref3=-IHam_ref1-IHam_ref2;

	IAHam=ILA-IHam_ref1;                                 //г������
	IBHam=ILB-IHam_ref2;
	ICHam=ILC-IHam_ref3;
	
	yucea[yucecount]=IAHam;
	yuceb[yucecount]=IBHam;
	yucec[yucecount]=ICHam;
	
	yucea[400]=yucea[0];
	yuceb[400]=yuceb[0];
	yucec[400]=yucec[0];
	
	yucea[401]=yucea[1];
	yuceb[401]=yuceb[1];
	yucec[401]=yucec[1];
	
	yucea[402]=yucea[2];
	yuceb[402]=yuceb[2];
	yucec[402]=yucec[2];
	

	switch(Operamode)                                  //���ݴ�������ָ��ѡ����Ӧ��ָ�����
	{
		case 1:
            	   Iref1se=0;                          
		           Iref2se=0;
		           Iref3se=0;
				   break;
		case 2:
            	   Iref1se=Indval*SinTable[Time0Count+100];                          
		           Iref2se=Indval*SinTable[Time0Count+367];
		           Iref3se=(-Indval*SinTable[Time0Count+100]-Indval*SinTable[Time0Count+367]);
				   break;
		case 3:
            	   Iref1se=Capval*SinTable[Time0Count+300];                          
		           Iref2se=Capval*SinTable[Time0Count+567];
		           Iref3se=(-Capval*SinTable[Time0Count+300]-Capval*SinTable[Time0Count+567]);
 				   break;
		case 4:
            	   Iref1se=Ham1val*SinTable[Time0Count]+/*Ham3val*Sin3Table[Time0Count]+*/Ham5val*Sin5Table[Time0Count]+Ham7val*Sin7Table[Time0Count];/*+Ham9val*Sin9Table[Time0Count]*///+Ham11val*Sin11Table[Time0Count]+Ham13val*Sin13Table[Time0Count];                     
		           Iref2se=Ham1val*SinTable[Time0Count+267]/*+Ham3val*Sin3Table[Time0Count+89]*/+Ham5val*Sin5Table[Time0Count+53]+Ham7val*Sin7Table[Time0Count+38];/*+Ham9val*Sin9Table[Time0Count+30]*///+Ham11val*Sin11Table[Time0Count+24]+Ham13val*Sin13Table[Time0Count+21];
		           Iref3se=(-Ham1val*SinTable[Time0Count]-Ham1val*SinTable[Time0Count+267])/*+(-Ham3val*Sin3Table[Time0Count]-Ham3val*Sin3Table[Time0Count+89])*/+(-Ham5val*Sin5Table[Time0Count]-Ham5val*Sin5Table[Time0Count+53])+(-Ham7val*Sin7Table[Time0Count]-Ham7val*Sin7Table[Time0Count+38]);/*+(-Ham9val*Sin9Table[Time0Count]-Ham9val*Sin9Table[Time0Count+30])*///+(-Ham11val*Sin11Table[Time0Count]-Ham11val*Sin11Table[Time0Count+24]);//+(-Ham13val*Sin13Table[Time0Count]-Ham13val*Sin13Table[Time0Count+21]);
 				   break;
		case 5:
            	   Iref1se=yucea[yucecount+1]*0.33333;//IAHam*0.33333;                          
		           Iref2se=yuceb[yucecount+1]*0.33333;//IBHam*0.33333;
		           Iref3se=yucec[yucecount+1]*0.33333;//ICHam*0.33333;
				   break;
		default:   
            	   Iref1se=0;                          
		           Iref2se=0;
		           Iref3se=0;
				   break;					       
	}


	Udcav=0.996*Udcav+0.004*Udc;                         //ֱ��ĸ�ߵ�ѹ�˲�

	fcheck2();                                           //ֱ��ĸ���Ƿ��ѹ���������������Ƿ�������

    UABsim=UAB*10/UdcTarget;                             //��ʵ��ֵת�ɷ����е�ֵ
	UBCsim=UBC*10/UdcTarget;
	Udcavsim=0.003*Udcav;
	ICOM1sim=0.015*ICOM1;
	ICOM2sim=0.015*ICOM2;
	ICOM3sim=0.015*ICOM3;

	if(j<400)
	{
	   ceshi1[j]=SinTable[Time0Count];
       ceshi2[j]=(2*UAB+UBC)*0.33333;	   
	   j++;
	}
    else j=0;

    Udct=UdcTarget*0.003-Udcavsim;

	if(state==0||Udct<-0.03)                            //��������
	{
	    Udci=0;
	}
    
	Udci=Udci+UdcKi*Udct;                                  

	if(Udci<-0.06)
		Udci=-0.06;												  //�����޷�
	else if(Udci>0.05)
		Udci=0.05;

	Udcpi=Udci+UdcKp*Udct;

	if(Udcpi>0.05) 
		Udcpi=0.05;												  //�޷�
	else if(Udcpi<-0.08)
		Udcpi=-0.08;

	Iref1dc=2.29*Udcpi*SinTable[Time0Count];                //��ѹָ��
	Iref2dc=2.29*Udcpi*SinTable[Time0Count+267];
	Iref3dc=-Iref1dc-Iref2dc;

	if(Udcavsim>2.07)                                 //ֱ����ѹ����690V����ָ���м���г��ָ��
	{
		Iref1=Iref1dc-0.015*Iref1se;                               //ָ�����
		Iref2=Iref2dc-0.015*Iref2se;
		Iref3=Iref3dc-0.015*Iref3se;

	}
	else
	{
	    Iref1=Iref1dc;
		Iref2=Iref2dc;
		Iref3=Iref3dc;
	}

	Iref1=-Iref1-ICOM1sim;                                  //����������������
	Iref2=-Iref2-ICOM2sim;
	Iref3=-Iref3-ICOM3sim;

	Iref1pi=7*Iref1;                                      //����������
	Iref2pi=7*Iref2;
	Iref3pi=7*Iref3;
	

	Uref1=Iref1pi+(2*UABsim+UBCsim)*0.33333;   
	Uref2=Iref2pi+(UBCsim-UABsim)*0.33333;
	Uref3=Iref3pi+(-UABsim-2*UBCsim)*0.33333;

	if(Uref1>5) Uref1=5;
	else if(Uref1<-5) Uref1=-5;
    if(Uref2>5) Uref2=5;
	else if(Uref2<-5) Uref2=-5;
	if(Uref3>5) Uref3=5;
	else if(Uref3<-5) Uref3=-5;

	//��FPGA������Ʋ�
//	*(Cmp1Write) = (Uint16)(Uref1 * 80+500);                                //��ֵ����ת��:�Ƚ�ֵ��������180����ƫ��֮��ȡ�������FPGA
//	*(Cmp2Write) = (Uint16)(Uref2 * 80+500);
//	*(Cmp3Write) = (Uint16)(Uref3 * 80+500);
    *(Cmp1Write)=(Uint16)(SinTable[k] * 400+500);
   *(Cmp2Write)=(Uint16)(SinTable[k+200] * 400+500);
   *(Cmp3Write)=(Uint16)(SinTable[k+133] * 400+500);
   k++;
   if(k>=400) k=0;    
    yucecount++;
    if(yucecount>=400) yucecount=0;
                                 
    dataflag=1;

    AdcRegs.ADCTRL2.bit.RST_SEQ1=1;
	PieCtrlRegs.PIEACK.bit.ACK1=1;
	AdcRegs.ADCST.bit.INT_SEQ1_CLR=1;
	EINT;
                                             //������־λ
}

void start(void)                                                //����PWM
{
    *(CommandStart)=0x0001;
	state=1;
	rsstate=2;
}

void stop(void)                                                 //����PWM
{
    *(CommandStop)=0x0002;
	state=0;
	if(chargflag==0) rsstate=0;
	else rsstate=3;
}

void fcheck1(void)
{
   //if(Udcav<Udcbvval)      
   if(600<Udcbvval)                                       //ֱ��ĸ�ߵ�ѹʵ��ֵС��ֱ��ĸ�ߵ�ѹ����ֵ��ΪǷѹ
    {
        stop();
//      contactorK2=0;                                         //���ֽӴ����Ͽ���������
        contactor2=0;
		ubvf=1;
	}
    else
	{
//	    if(faultf==0) contactorK2=0;
//      else contactorK2=1;
        contactor2=1;
		ubvf=0;
	}

//	if(Temax>Totval)
//	{
//	    fanK3=1;
//		fan=1;
//		otf=1;
//	}
//	else
//	{
//	    fanK3=0;
//		fan=0;
//		otf=0;
//	}

	igbtfault.all=*(IgbtFaultRead);                             //IGBT���ϼ��
	if(igbtfault.bit.Aup==1||igbtfault.bit.Ado==1||igbtfault.bit.Bup==1||igbtfault.bit.Bdo==1||igbtfault.bit.Cup==1||igbtfault.bit.Cdo==1)
	{
	    igbtf=1;
	    stop();
        faultf=1;
//		contactorK1=0;
//		contactorK2=0;
	    contactor1=0;
		contactor2=0;
	}


	faultstasec.bit.faultf=faultf;
	faultstasec.bit.contactor1=contactor1;
	faultstasec.bit.contactor2=contactor2;
	faultstasec.bit.fan=fan;
	faultstasec.bit.igbtf=igbtf;
	faultstasec.bit.otf=otf;
	faultstasec.bit.rsstate=rsstate;
	faultstasec.bit.rxf_foef=0;
	faultstasec.bit.socf=socf;
	faultstasec.bit.ubvf=ubvf;
	faultstasec.bit.uovf=uovf;

//	*(FaultStaWrite) = faultsta.all;
}

void fcheck2(void)
{
    if(Udcav>Udcovval)    
  //  if(Udcav>760)                                        //ֱ��ĸ�ߵ�ѹ�Ƿ��ѹ
	{
	    stop();                                      //��FPGA������PWM�ź�
	    faultf=1;                                              //�ô���λ
//      contactorK1=0;
//		contactorK2=0;
		contactor1=0;
		contactor2=0;
		uovf=1;
	}

	if((ICOM1>Isocval)||(ICOM1<-Isocval)||(ICOM2>Isocval)||(ICOM2<-Isocval)||(ICOM3>Isocval)||(ICOM3<-Isocval))      //�������������Ƿ����
	{
	    stop();                                                //��FPGA������PWM�ź�
	    faultf=1;                                              //�ô���λ
//      contactorK1=0;
//		contactorK2=0;
		contactor1=0;
		contactor2=0;
		socf=1;
	}  
}

interrupt void Zero_cap_isr(void)
{
    pllt = CpuTimer0.InterruptCount;                            //��ȡ�ϸ���Ƶ���ڵ��жϴ���
if(pllt>250){
	prdtemp = CpuTimer0Regs.PRD.all * pllt / 400; 
	 CpuTimer0Regs.PRD.all = prdtemp;              //�µļ������ڼ���
	if(prdtemp>7350&&prdtemp<7650)                              //prdtemp��׼ֵ��7500(150M*50us)(����Ƶ�ʵ�ƫ����49Hz~51Hz֮�䣩
	{                       //����ֵ���������µļ���ֵ
		faultsta.bit.sff=0;
	}
	else 
	{
	    stop();
	    faultsta.bit.sff=1;                                                
	}
	UBCPLL = UBC;                                               //�ж�����
	pllflag=1;
	CpuTimer0.InterruptCount = 0;
	}
	PieCtrlRegs.PIEACK.bit.ACK1=1;
}
	    
void fcheck(void)
{
	if((prdtemp * CpuTimer0.InterruptCount)>3080000)           //����������
	{
        stop();
		CpuTimer0.InterruptCount = 500;                        //����400�Ϳ�����  �ô���һֱ��?
		faultsta.bit.sff=1;
	}

	if(UABAV>Usovval||UBCAV>Usovval)
	{
	    stop();
		faultsta.bit.sovf=1;
	}
	else
	{
	    faultsta.bit.sovf=0;
	}
		
	if(UABAV<Usbvval||UBCAV<Usbvval)
	{
	    stop();
    	faultsta.bit.sbvf=1;
	}
	else
	{
	    faultsta.bit.sbvf=0;
	}
	
	if(faultsta.bit.sovf==1||faultsta.bit.sbvf==1||faultsta.bit.sff==1)   
	      fmainflag=1;
	else  fmainflag=0;
}

void controlset(void)
{
    Operamode=(Uint16)BytleValue[14];                          //���з�ʽ
	Indval   =(float)BytleValue[15];                           //��ֵ�����޹�
	Capval   =(float)BytleValue[16];                           //��ֵ�����޹�
	Ham3val  =(float)BytleValue[17];
	Ham5val  =(float)BytleValue[18];
	Ham7val  =(float)BytleValue[19];
	Ham9val  =(float)BytleValue[20];
	Ham11val =(float)BytleValue[21];
	Ham13val =(float)BytleValue[22];
	Ham1val  =(float)BytleValue[23];

	DynHam3  =(Uint16)BytleValue[25];                          //ѡ���Զ�̬����г��
	DynHam5  =(Uint16)BytleValue[26];
	DynHam7  =(Uint16)BytleValue[27];
	DynHam9  =(Uint16)BytleValue[28];
	DynHam11 =(Uint16)BytleValue[29];
	DynHam13 =(Uint16)BytleValue[30];
	DynHamHi =(Uint16)BytleValue[31];
	DynHamAll=(Uint16)BytleValue[32];
}
   
void maindataset(void)
{
    Wiring   =(Uint16)BytleValue[36];
	Controlmode=(Uint16)BytleValue[37];
	L1val    =0.01*(float)BytleValue[38];
	L2val    =0.01*(float)BytleValue[39];
	CT1Prop  =(float)BytleValue[40];
    CT2Prop  =(float)BytleValue[41];
	Hall1Prop=(float)BytleValue[42];
	Hall2Prop=(float)BytleValue[43];                     
	UdcTarget=(float)BytleValue[44];
	UdcKp    =0.01*(float)BytleValue[45];                      //��������Kp��������100����������
	UdcKi    =0.00005*(float)BytleValue[46];                   //����Ƶ����20000Hz����������������KiҪ����20000
	SwitchFre=(Uint16)BytleValue[47];

    Udcovval =(float)BytleValue[49];
	Udcbvval =(float)BytleValue[50];
	Isocval  =(float)BytleValue[51];
	Usovval  =(float)BytleValue[52];
	Usbvval  =(float)BytleValue[53];
	Totval   =(float)BytleValue[54];

	IKp=(L1val+L2val)*20;                                     //(0.1mH+0.2mH)*20KHz
}

//void secdataset(void)
//{   
 //   IKp  = (Uint16)IKp*0.1;
//    CTProp = (Uint16)(CT1Prop/CT2Prop);
//    HallProp = (Uint16)(Hall1Prop/Hall2Prop);
//    UdcTarget= (Uint16)UdcTarget;
//    UdcKp  = BytleValue[45]*0.01;
//    UdcKi  = BytleValue[46]*0.00005;
//    SwitchFre = SwitchFre;
 //   Udcovval  = BytleValue[49];
 //   Udcbvval   = BytleValue[50];
 //   Isocval   = BytleValue[51];
 //   Totval     = BytleValue[54];
//}

void secdeal(void)
{
//    faultsta.all = *(faultstaRead);
//    Udcsec          = *(UdcsecRead);
//    ICOM1sec        = *(ICOM1secRead);
//    ICOM2sec        = *(ICOM2secRead);
//    ICOM3sec        = *(ICOM3secRead);
    Tesecmax        = 80;//*(TesecmaxRead);

    faultstasecall.bit.uovf = faultstasec.bit.uovf;
	faultstasecall.bit.ubvf = faultstasec.bit.ubvf;
	faultstasecall.bit.socf = faultstasec.bit.socf;
	faultstasecall.bit.igbtf= faultstasec.bit.igbtf;
	faultstasecall.bit.otf  = faultstasec.bit.otf;

	if(faultstasec.bit.rxf_foef==1)
	    faultstasecall.bit.rxf_foef=1;
	else faultstasecall.bit.rxf_foef=0; 
	
	if(faultstasec.bit.uovf==1||faultstasec.bit.socf==1||faultstasec.bit.igbtf==1||faultstasec.bit.otf==1||faultstasecall.bit.rxf_foef==1)
	    faultstasecall.bit.fsecflag=1;
	else faultstasecall.bit.fsecflag=0;
	
	if(fmainflag==1||faultstasecall.bit.fsecflag==1)
	    faultsta.bit.fallflag=1;
	else faultsta.bit.fallflag=0;    

	faultstasecall.bit.rsstate = faultstasec.bit.rsstate;
	faultstasecall.bit.contactor1 = faultstasec.bit.contactor1;
	faultstasecall.bit.contactor2 = faultstasec.bit.contactor2;
	faultstasecall.bit.fan     = faultstasec.bit.fan;
}

void datadeal(void)
{
    if(dataflag)
	{
	    UABSUM=UABSUM+UAB*UAB;
        UBCSUM=UBCSUM+UBC*UBC;
		UCASUM=UCASUM+UCA*UCA;
		ILASUM=ILASUM+ILA*ILA;
		ILBSUM=ILBSUM+ILB*ILB;
		ILCSUM=ILCSUM+ILC*ILC;
	    ICOM1SUM=ICOM1SUM+ICOM1*ICOM1;
	    ICOM2SUM=ICOM2SUM+ICOM2*ICOM2;
	    ICOM3SUM=ICOM3SUM+ICOM3*ICOM3;

		dataflag=0;
	}
    if(pllflag)
	{  
	    UABAV=sqrt(UABSUM/400);
		UBCAV=sqrt(UBCSUM/400);
		UCAAV=sqrt(UCASUM/400);
		ILAAV=sqrt(ILASUM/400);
		ILBAV=sqrt(ILBSUM/400);
		ILCAV=sqrt(ILCSUM/400);
	    ICOM1AV=sqrt(ICOM1SUM/400);
	    ICOM2AV=sqrt(ICOM2SUM/400);
	    ICOM3AV=sqrt(ICOM3SUM/400);

		UABSUM=0;
		UBCSUM=0;
		UCASUM=0;
		ILASUM=0;
		ILBSUM=0;
		ILCSUM=0;
	    ICOM1SUM=0;
	    ICOM2SUM=0;
	    ICOM3SUM=0;

		pllflag=0;
	} 
}
void LPFCAL(float Id_in,float Iq_in)
{
   Iddata[lpfcount]=Id_in;
   Iqdata[lpfcount]=Iq_in;
   if(lpfcount==399)
   {
      Idav=Idavtemp+(Iddata[399]-Iddata[0])*0.0025;
      Iqav=Iqavtemp+(Iqdata[399]-Iqdata[0])*0.0025;
   }
   else
   {  
      Idav=Idavtemp+(Iddata[lpfcount]-Iddata[lpfcount+1])*0.0025;
      Iqav=Iqavtemp+(Iqdata[lpfcount]-Iqdata[lpfcount+1])*0.0025;
   }
   lpfcount++;
   if(lpfcount==400) lpfcount=0;      
   Idavtemp=Idav;
   Iqavtemp=Iqav;
}

