#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "math.h"
#define adcclk 0x03
#define EN_485        GpioDataRegs.GPADAT.bit.GPIO10          //485使能信号
#define DDOG          GpioDataRegs.GPBDAT.bit.GPIO55          //DSP正常运行指示灯
#define led_run_d     GpioDataRegs.GPADAT.bit.GPIO25          //运行指示灯
#define led_stop_d    GpioDataRegs.GPADAT.bit.GPIO24          //停止指示灯
//#define led_err_d     GpioDataRegs.GPCDAT.bit.GPIO82          //故障指示灯
//#define  anniu        GpioDataRegs.GPADAT.bit.GPIO30
#define contactorK1		GpioDataRegs.GPBDAT.bit.GPIO39   
#define contactorK2		GpioDataRegs.GPADAT.bit.GPIO31
#define FK2				GpioDataRegs.GPADAT.bit.GPIO4
#define FK1				GpioDataRegs.GPADAT.bit.GPIO5
//#define fanK3       
#define pi 3.14159265359

//yt
float xn[3] = {0,0,0};
float Pac = 0;
float yn[3] = {0,0,0};
float A=0,B=0,C=0,D=0,temp=0;
float Wc=5,Ts=0.00005,W0=314;
/*
#define a0_lp 1*0.01//1*0.067455
#define a1_lp 2*0.01//2*0.067455   
#define a2_lp 1*0.01//1*0.067455
#define b1_lp -1.14298     
#define b2_lp  0.4128


float U1_lp[3] = {0,0,0};     //低通滤波器中所需的变量
float U2_lp[3] = {0,0,0};
float U3_lp[3] = {0,0,0};
*/

//void s_delay(Uint16 i);
//void ms_delay(Uint16 i)

float InputWave(float input);
float IIR();

float fSum;
#define IIRNUMBER 3
float fAn[IIRNUMBER]={0.0007866,0.0007866,0.0};               //num //0.001568+0.001568(z^-1))		                                                //-----------------
float fBn[IIRNUMBER]={0,0.99842,0};             //den //1-0.996863(z^-1)
float fXn[IIRNUMBER]={0,0,0};
float fYn[IIRNUMBER]={0,0,0};
float fInput, fOutput;
//float fSignal1, fSignal2;
//float fStepSignal1,fStepSignal2;

//YT

     
#pragma CODE_SECTION(adcisr,"ramfuncs")
#pragma CODE_SECTION(Zero_cap_isr,"ramfuncs")
#pragma CODE_SECTION(cpu_timer0_isr,"ramfuncs")
//#pragma CODE_SECTION(LPFCAL,"ramfuncs")

Uint16    *CommandStart = (Uint16 *)0x100001;       //Start Command address
Uint16    *CommandStop  = (Uint16 *)0x100002;       //Stop  Command address
Uint16    *Commandtongbu  = (Uint16 *)0x100003;
 
Uint16     *Cmp1Write   = (Uint16 *)0x100011;       //调制波信号
Uint16     *Cmp2Write   = (Uint16 *)0x100012;
Uint16     *Cmp3Write   = (Uint16 *)0x100013;

Uint16    *IgbtFaultRead = (Uint16 *)0x100021;       //IGBT故障信号地址

struct IGBTFault_BITS{      //bits  description     //故障信号寄存器定义
    Uint16  Aup:1;     //0  Aup fault  
	Uint16  Ado:1;     //1  Adown fault
	Uint16  Bup:1;     //2  Bup fault
	Uint16  Bdo:1;     //3  Bdown fault
	Uint16  Cup:1;     //4  Cup fault
	Uint16  Cdo:1;     //5  Cdown fault
	Uint16  foef:1;    //6  奇偶校验故障
	Uint16  rxf:1;     //7  光纤通信故障
	Uint16  rsvd:8;   //15:8 reserved
};
union IGBTFault_REG{
    Uint16            all;
	struct IGBTFault_BITS bit;
};
union IGBTFault_REG igbtfault;

struct FaultStaSec_BITS{
    Uint16  faultf:1;      //0  致命故障
    Uint16  uovf:1;        //1  母线过压
	Uint16  ubvf:1;        //2  母线欠压
	Uint16  igbtf:1;       //3  IGBT故障
	Uint16  otf:1;         //4  过温故障
	Uint16  socf:1;        //5  模块过流
	Uint16  contactor1:1;  //6  接触器1状态
	Uint16  contactor2:1;  //7  接触器2状态
	Uint16  fan:1;         //8  风扇状态
	Uint16  rxf_foef:1;    //9  光纤接受故障(从板)
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
    Uint16  start_stop:2;    //0:1 逆变启动1 逆变停止2
    Uint16  charg_chargof:2; //2:3 充电1 断电2
    Uint16  reset:1;         //4   复位1
    Uint16  rsvd:11;         //保留
};
union ControlSta_REG{
    Uint16  all;
    struct  ControlSta_BITS bit;
};
union ControlSta_REG controlstasec;                             //控制指令

extern int BytleValue[];         //

Uint16 i=0,j=0,k=0,g=0;  
//YT
Uint16 ConversionCount=0;
//YT                              //一些常用的变量
float SinTable[1400];										  //正弦表  两个周期及以上就行
//float Sin5Table[700];
//float Sin7Table[700];
//float Sin11Table[700];
//float Sin13Table[700];
/*
float Iddata[400];
float Iqdata[400]; */
//float yucea[405];
//float yuceb[405];
//float yucec[405];
float rctablea[400];
float rctableb[400];
float rctablec[400];
float ceshi1[400];
float ceshi2[400];
float ceshi3[400];
float ceshi4[400];
float UBNcon[400];
float Iref1av,Iref2av,Iref3av,Kref=1;  //查找，有些只是定义了。
float Iref1sum=0,Iref2sum=0,Iref3sum=0;
//float  UBCAV=380,UBCSUM=0,UABAV=380,UABSUM=0,UCAAV=380,UCASUM=0;
//float  UANAV=220,UANSUM=0,UBNAV=220,UBNSUM=0,UCNAV=220,UCNSUM=0;
float  UANAV=110,UANSUM=0,UBNAV=110,UBNSUM=0,UCNAV=110,UCNSUM=0;
float  ILASUM=0,ILBSUM=0,ILCSUM=0;
float  ILAAV=0,ILBAV=0,ILCAV=0;
float  ICOM1SUM=0,ICOM2SUM=0,ICOM3SUM=0;
float  ICOM1AV=0,ICOM2AV=0,ICOM3AV=0,Udcav=0;
float  Isysa=0,Isysb=0,Isysc=0,Isysasum=0,Isysbsum=0,Isyscsum=0,Isysaav=0,Isysbav=0,Isyscav=0;
float  Udc1av=0,Udc2av=0;
float  THDIL=0,THDIsys=0;
float  coefficient=0;
Uint16 erchk=0;
Uint16 UAB_temp,UCB_temp,ILA_temp,ILB_temp,ILC_temp,Tem1_temp,Tem2_temp;   //电网电压、直流侧电压和温度的采样值
Uint16 UAN_temp,UBN_temp,UCN_temp;
Uint16 Udc_temp1,Udc_temp2,ICOM1_temp,ICOM2_temp,ICOM3_temp;                 //负载电流和补偿电流的采样值

Uint16 ms=0,s=0;
float Ps=0;

float  UAB,UBC,UAN,UBN,UCN,ILA,ILB,ILC,Udc,Udc1,Udc2,Tem1,Tem2,Temax=25,ICOM1,ICOM2,ICOM3,ICOM4;      //电网电压、直流侧电压和温度的实际值和负载电流和补偿电流的实际值                                  //
float  Tesecmax;
float  UANsim,UBNsim,UCNsim,ILAsim,ILBsim,ILCsim,ICOM1sim,ICOM2sim,ICOM3sim,Udcavsim,Udc1sim,Udc2sim;//实际值转换到仿真中的值

float Iocak=0,Iocbk=0,Iocck=0;
float Idsys,Iqsys,Idsysav=0,Iqsysav=0;
float Id,Iq;                                                  //abc-dq0中的dq分量
float Idav=0,Iqav=0;                                          //对dq分量进行低通滤波
float IAHam=0,IBHam=0,ICHam=0;
float IHam_ref1,IHam_ref2,IHam_ref3;  
float Iref1se,Iref2se,Iref3se;                                //通过功能选择得到的指令电流
float us_alpha,us_beta;

Uint16 DelayCount=0;                                          //防抖延时计数
Uint16 rsorder = 0;
Uint16 iniflag =1;                                          //模块逆变运行指令 	              
Uint16 pllt,prdtemp=0;										  //PLL中断服务子程序里的中断次数，cputimer0的计数值
Uint16 pllflag=0;											  //PLL标志                                           
Uint16 fmainflag=0;
Uint16 resetcount=0,resetflag=0;
float UBCPLL;
Uint16 yucecount=0;

Uint16 Operamode=6;//1表示稳压 ，2定量感性，3定量容性，4定量谐波，5动态补谐波
float number=2;
float Capval=0,Indval=0;
float Ham1val=0,Ham3val=0,Ham5val=0,Ham7val=0,Ham9val=0,Ham11val=0,Ham13val=0;
Uint16 DynHam3=0,DynHam5=0,DynHam7=0,DynHam9=0,DynHam11=0,DynHam13=0,DynHamHi=0,DynHamAll=0;
Uint16 Wiring=1;// 1表示三相三线 2表示三相四线  暂时没使用
Uint16 Controlmode=1;//1表示检测负载电流，2表示检测电网电流，3表示复合控制方式   暂时没使用
float L1val=0.1,L2val=0.2;
float CT1Prop,CT2Prop,Hall1Prop,Hall2Prop;
float UdcTarget=78;//350  DCdianya;
//float KUdcT=1/200;
//float UdcKp=3.96,UdcKi=0.00335,IKp=6;                                                    //电流环比例系数;
Uint16 SwitchFre=10000;
//float Udcovval=400,Udcbvval=490,Isocval=220,Usovval=240,Usbvval=180,Totval=80;
float Isocval=110;
float Gridpowfac=0,Loadpowfac=0,THDGrid=0,THDLoad=0;
Uint16 contactor1,contactor2,fan;

float Udci=0,Udcpi=0;
float Udct;    
float Udccat,Udccai=0,Udccapi=0;                                               //直流电压误差
float Iref1dc,Iref2dc,Iref3dc;                                      //稳压电流指令
float Iref1se,Iref2se,Iref3se;
float Iref1,Iref2,Iref3;   
float Iref1pi,Iref2pi,Iref3pi;                                //电流环比例积分输出
float Uref1,Uref2,Uref3;                                      //调制波电压指令
float Uavt=0,Uavi=0,Uavpi=0;
//float cmp1,cmp2,cmp3;
//float Di1,Di2,Di3; 

//YT
  float UANALP,ICOM1ALP,UANBTA,ICOM1BTA;
  float UANALP_DELAY,UANBTA_DELAY,ICOM1ALP_DELAY,ICOM1BTA_DELAY;
  float UANALP_DREF[101],UANBTA_DREF[101],ICOM1ALP_DREF[101],ICOM1BTA_DREF[101];


  float PLALP,QLALP;
  float PLALP_DC,PLALP_AC;
  float PCALP,QCALP,PCBTA,QCBTA;
  float NUMA,DENA,ICALP_REF;

  float ER_IC =0,ER_ICP=0,ER_ICR=0,ER_IC1=0,ER_IC2=0,ER_IC3=0,ER_IC4=0,ER_ICR1=0,UREF=0,limtz=0,limtf=0,limtz2=0,limtf2=0;
  float HB=0.1;
 //YT

Uint16 Time0Count=0;
Uint16 faultf  = 0;											  //错误（故障）标志
Uint16 state = 0;                                           //运行状态
Uint16 ftime   = 0;								              //开关防抖计数（机械开关用）
Uint16 runcount= 0;                                           //运行灯闪烁功能所需计数变量
Uint16 dataflag= 0;											  //采样数据标志
Uint16 chargflag=0;                                           //充断电标疚?
Uint16 rctcount=0;                                           //重复控制数组
//Uint16 chargcount=0;
//Uint16 runorcount = 0;
Uint16 ICOMcount=0;
Uint16 Temcount=0;
Uint16 Qcount=0;
Uint16 m=0;
float UBNA=0,UBNB=0,UBNC=0,UBNS=0;

interrupt void cpu_timer0_isr(void);                          //cpu定时器0中断服务子程序
interrupt void scibrx(void);                                  //SCI通信接受中断
interrupt void adcisr(void);                                  //AD采样服务子程序
interrupt void Zero_cap_isr(void);                            //电网电压过零捕捉中断

void maindataset(void);
void controlset(void);
void datadeal(void);
void fcheck(void);                                           //检测系统电压和频率
void secdeal(void);
void fcheck1(void);                                           //检测直流母线是否欠压，过温，IGBT是否故障
void fcheck2(void);                                           //检测直流母线是否过压，过流
void start(void);
void stop(void);
//void LPFCAL(float Id_in,float Iq_in);

//YT
/*
float lowpass1(float);                                        //重复控制中的低通滤波器
float lowpass2(float);
float lowpass3(float); */
//YT

float Idavtemp=0;
float Iqavtemp=0;
Uint16 lpfcount=0;

void main(){
    InitSysCtrl();                                            //初始化系统配置
    EALLOW;
    SysCtrlRegs.HISPCP.all=adcclk;                            //25M
    EDIS;                                           
	//memcpy(&ramfuncsRunStart, &ramfuncsLoadStart, &ramfuncsLoadEnd-&ramfuncsLoadStart);
	//InitFlash();                                              //初始化FLASH
    InitXintf16Gpio();                                        //配置GPIO
	InitScibGpio();
	DINT;
	InitPieCtrl();											  //初始化中断
	IER=0x0000;
	IFR=0x0000;																
	InitPieVectTable();                                       //初始化中断向量表
	EALLOW;
	PieVectTable.TINT0  = &cpu_timer0_isr;                    //CPU定时器0中断服务程序
	PieVectTable.SCIRXINTB=&scibrx;                           //SCI通讯接受中断
	PieVectTable.ADCINT = &adcisr;                            //AD采样中断服务程序
	PieVectTable.XINT2  = &Zero_cap_isr;                      //电网电压过零捕捉中断
	EDIS;
	InitCpuTimers();										  //初始化cpu定时器
	ConfigCpuTimer(&CpuTimer0,150,50);                        //CPU定时器0配置
    scib_fifo_init();                                         // 初始化SCI FIFO
	SciInit();
	Initcmptx();
	InitAdc();
	//以下几句初始化AD转换
	AdcRegs.ADCTRL1.bit.ACQ_PS=0x5;
	AdcRegs.ADCTRL2.bit.RST_SEQ1=1;
	AdcRegs.ADCTRL1.bit.SEQ_CASC=1;//yt 选择级联模式
	AdcRegs.ADCTRL1.bit.CONT_RUN=0; //yt stop/dtart mode
	AdcRegs.ADCTRL1.bit.SEQ_OVRD=0;
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1=1;
	AdcRegs.ADCCHSELSEQ1.all = 0x0;
	AdcRegs.ADCCHSELSEQ2.all = 0x0;
	AdcRegs.ADCCHSELSEQ3.all = 0x0;
	AdcRegs.ADCCHSELSEQ4.all = 0x0;


	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x7;     //UAN//
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x6;     //UBN//
	AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x5;     //UCN
	//	AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x5;     //UCN
	AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x4;     //UDC1//

	AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0x3;     //UDC2
	AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 0x2;     //IASYS
	AdcRegs.ADCCHSELSEQ2.bit.CONV06 = 0x1;     //IBSYS
	AdcRegs.ADCCHSELSEQ2.bit.CONV07 = 0x0;     //ICSYS
	AdcRegs.ADCCHSELSEQ3.bit.CONV08 = 0x8;     //TEM
	AdcRegs.ADCCHSELSEQ3.bit.CONV09 = 0x9;     //I1
//	AdcRegs.ADCCHSELSEQ3.bit.CONV09 = 0x9;     //I1
	AdcRegs.ADCCHSELSEQ3.bit.CONV10 = 0xa;     //I2
	AdcRegs.ADCCHSELSEQ3.bit.CONV11 = 0xb;     //I3
	AdcRegs.ADCCHSELSEQ4.bit.CONV12 = 0xc;     //I4
	AdcRegs.ADCCHSELSEQ4.bit.CONV13 = 0xd;     //I5
	AdcRegs.ADCCHSELSEQ4.bit.CONV14 = 0xe;     //I6
	AdcRegs.ADCCHSELSEQ4.bit.CONV15 = 0xf;     //I7
    AdcRegs.ADCMAXCONV.all = 0x000F;           //16对通道转换
	
	BytleValue[76]=iniflag;

	for(i=0;i<1400;i++)
    {
        SinTable[i]=sin(2*pi*i/400);										//构造正弦表
	} /**/
/*
	for(i=0;i<400;i++) Iddata[i]=0;
	for(i=0;i<400;i++) Iqdata[i]=0;	 */
//	for(i=0;i<400;i++) UBNcon[i]=0;
//	for(i=0;i<400;i++)
//	{
//	    rctablea[i]=0;
//		rctableb[i]=0;
//		rctablec[i]=0;
//	}
	i=0;
	EnableDog();
	IER|=M_INT1;
 	IER|=M_INT9;
	PieCtrlRegs.PIEIER1.bit.INTx7  = 1;
	PieCtrlRegs.PIEIER9.bit.INTx3  = 1;
	PieCtrlRegs.PIEIER1.bit.INTx6  = 1;
	PieCtrlRegs.PIEIER1.bit.INTx5  = 1;
	EINT;													 //以上几句开中断
	ERTM;
	StartCpuTimer0();                                         //启动定时器0
    stop();
	EN_485=0;
	DELAY_US(1E6);                
	igbtfault.all = 0x0000;
	faultsta.all = 0x0000;
	faultstasec.all=0x0000;
	faultstasecall.all=0x0000;
	controlstasec.all = 0x0000;
	faultf=0;
	igbtf=0;
    for(;;)
	{
	    if(runcount>=10000)                                        //DSP运行闪烁灯一秒钟闪两次
 	    {
	       DDOG=~DDOG;
		   runcount=0;
	    }
		if(state) DDOG=1;
        fcheck();                                                 //故障检测

		secdeal();
		

		if(BytleValue[77]==1)                                     //对模块故障信号进行复位
		{
			controlstasec.bit.reset=1;
			resetflag=1;
		}
				     
	    if(resetcount==4)                                        //复位结束处理 留200us的复位时间                 
		{
		    controlstasec.bit.reset=0;
	        BytleValue[77]=0;
		    resetflag=0;
		}

		if(BytleValue[78]==1)                                       // 模块充电
		    controlstasec.bit.charg_chargof=1;
		else  controlstasec.bit.charg_chargof=2;

		if(faultstasec.bit.faultf==1) BytleValue[78]=0;
		   
		if(rsorder==1)                                                //逆变启动指令
		{
			if(state==0&&ftime>=30000&&iniflag==0) start();    //ftime>=30000 延迟1.5s
			if(state==1&&faultstasec.bit.faultf==1)
			{
			   stop();
			   BytleValue[79]=0;                                      //装置运行过程中出现致命故障需对逆变运行指令清零
			}
		}
		else if(state==1) stop();

		if(controlstasec.bit.reset==1)                        //复位
		{
		    faultf=0;
			uovf=0;
			socf=0;
			igbtf=0;
		}

        fcheck1();                                                    
        

		if(crcflag==1) modbusdeal();

		cmptxdeal();
    }
}

interrupt void cpu_timer0_isr(void)
{   
    ServiceDog();
	runcount++;
	
	if(rsorder==1) ftime++;                                          //启动延时
	else ftime=0;
	if(resetflag==1) resetcount++;                                   //复位延时
	else resetcount=0;
	if(ftime>30000) ftime=30000;
	txcount++;
	if(txcount>10000)txcount=10000;
	txfree++;
	if(txfree>10000) txfree=10000;

	if(state==1)  Qcount++;
	else Qcount=0;

	if(Qcount>=20000)
	Qcount=20000;

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
		BytleValue[83]=0;
	} 
	if(BytleValue[84]==1)
	{
	    controlset();
		BytleValue[84]=0;
	}
	*(Commandtongbu)=0x0003;                                                     
    AdcRegs.ADCTRL2.bit.SOC_SEQ1=1;
	CpuTimer0.InterruptCount++;	  
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;
	CpuTimer0Regs.TCR.bit.TIF   = 1;                                               
}
//////////*************************************************************************************************************
interrupt void adcisr(void)
{
    UAN_temp	= (AdcRegs.ADCRESULT0>>4); 
	UBN_temp	= (AdcRegs.ADCRESULT1>>4);
	Udc_temp1	= (AdcRegs.ADCRESULT3>>4);
    //Result9=>I1-2070  Result10=>I2-2048  Result11=>I3--2048
	ICOM1_temp	= (AdcRegs.ADCRESULT10>>4); // ICOM1_temp--Ic
	ICOM2_temp	= (AdcRegs.ADCRESULT11>>4);//  ICOM2-temp--iload
	ICOM3_temp	= (AdcRegs.ADCRESULT9>>4);//  ICOM3_temp--is

//偏置之后的实际值
	UAN     = ((float)UAN_temp-2085)*0.53267045;   
	UBN     = ((float)UBN_temp-2058)*0.53267045;  
	Udc1	= ((float)Udc_temp1)*0.3197;
   // ICOM1	= ((float)ICOM1_temp-2062)*0.24414;   
   	//ICOM2	= ((float)ICOM1_temp-2048)*0.02857;//0.24414*0.125/8                      //iload
//I3-2062   I2-2070  I6-2222   I1-2048
	//ICOM1 = ICOM3 - ICOM2;
	ICOM1	= ((float)ICOM1_temp-2070)*0.02857;  //*0.24414    0.24414*0.125/1.068                                       // ic
	ICOM2	= ((float)ICOM2_temp-2062)*0.02857;//0.24414*0.125/8                      //iload
	ICOM3	= ((float)ICOM3_temp-2048)*0.02857;                                     //is

 //	ICOM3	= ((float)ICOM3_temp-2062);    //*0.24414    0.24414*0.125/1.068
	//ICOM4 = ICOM3 - ICOM2;
    Time0Count=CpuTimer0.InterruptCount+164;  //  164  调相位167
               
	switch(Operamode)                                  
	{
		case 1:
            	   Iref1se=0;                          
				   break;
		case 2:
            	   Iref1se=Indval*SinTable[Time0Count+100];   // 电流超前1/4 周期                        
				   break;
		case 3:
            	   Iref1se=Capval*SinTable[Time0Count+300];   //电流滞后1/4周期                                  
 				   break;
		case 6:
            	   Iref1se=IAHam;        //还没写功能                    
				   break;
		default:   
		           Iref1se=0;                          
				   break;					       
	}

/*********YT   **************/


	UANALP = UAN;
	ICOM1ALP = ICOM2;
//******delay function
	if(ConversionCount>100)
		ConversionCount =0;
	if((ConversionCount+1)==101)
	{
	UANALP_DELAY = UANALP_DREF[0];
	ICOM1ALP_DELAY = ICOM1ALP_DREF[0];	
	}
	else
	{
	UANALP_DELAY = UANALP_DREF[ConversionCount+1];
	ICOM1ALP_DELAY = ICOM1ALP_DREF[ConversionCount+1];
	}

	UANALP_DREF[ConversionCount] = UANALP;
	ICOM1ALP_DREF[ConversionCount] = ICOM1ALP;

//load active power 
	PLALP = UANALP*ICOM1ALP;
	PLALP+=(UANALP_DELAY*ICOM1ALP_DELAY);
	PLALP = PLALP*0.5;
//load imaginary power
	QLALP = UANALP_DELAY*ICOM1ALP;
	QLALP -= (UANALP*ICOM1ALP_DELAY);
	QLALP = QLALP*0.5;
//load active power AC part
	fInput=InputWave(PLALP);   //input filter signal
    fOutput=IIR();			//output filter signal
	//PLALP_DC =577;
	//PLALP_DC = lowpass1(PLALP);
	PLALP_DC = fOutput;
	//PLALP_DC=0.996*PLALP_DC+0.004*fOutput;
	PLALP_AC = PLALP-PLALP_DC;
////稳定DC值
   	Udcav=0.996*Udcav+0.004*Udc1;   //一阶低通滤波                      
   //	Udcav=Udc1; 
	fcheck2();    // 电流和电压保护 
	                                      
    Udct=70-Udcav;   //Udct=UdcTarget-Udcav;             //  DC Target  90

	Pac = ICOM3*Udcav;

	if(state==0)                            
	{
	    Udci=0;
		Uavi=0;
	}
    
	Udci=Udci+0.007*Udct;     //实现I，0.00025 是I的参数   //      PI   0.007                         

	// 限幅
	/*
	if(Udci<-100)
		Udci=-100;												  
	else if(Udci>100)
		Udci=100;
	*/
   	Udcpi=Udci+5*Udct;    // 实现P，参数为1  0.06      //       PI
   //	Udcpi=5.007*Udct; 
	//Udcpi = 0;
	/*
	if(Udcpi>100) 
		Udcpi=100;												  
	else if(Udcpi<-100)
		Udcpi=-100;
    */
	Iref1dc=Udcpi*SinTable[Time0Count];                
   //稳定DC值




//required active power and reactive power for compensation(alpha)
   /*PCALP = PLALP_DC*0.5+PLALP_AC;

   QCALP = 0.2887*PLALP_DC+QLALP;

   PCBTA = -0.5*PLALP_DC;

   QCBTA = -0.2887*PLALP_DC;*/
//	Udcpi = 0;

   	Ps= 30;
	if(state==1)
	{ 
	
	    if(ms<=20000)
		  {
			ms++;
		  	
		  	
		  }
		  else 
		  {
			 ms = 0;
		 	 s++;
		  }
	} 
  	if(s>=25)
		Ps=90;
   PCALP = PLALP_AC + Ps;
   	PCALP -= 8;   //1
   	//PCALP -= 14;  //2
   QCALP = QLALP;//+500;//+Udcpi;
   //QCALP -= 30;
  // PCALP = PLALP_AC;

  // QCALP = QLALP;

//current reference(alpha)
   NUMA = UANALP*PCALP+UANALP_DELAY*QCALP;//????
   DENA =  UANALP*UANALP+UANALP_DELAY*UANALP_DELAY;
   DENA =  DENA*0.5;
   ICALP_REF = NUMA/DENA;                                              // ic_ref
	//ICOM1ALP = ICOM1ALP+50;
  	ICOM4 = ICOM2- ICALP_REF;


 
    //  VOC
//   	ER_IC = +ICOM1-ICALP_REF;      
   	ER_IC = ICOM1-ICALP_REF;                                       //ic_error
	
   	///**/ER_ICP = ER_IC*62;   //Kp=50  62  50
//	if(ER_ICP>100) ER_ICP=100;
	   // else if(ER_ICP<-100) ER_ICP=-100;

   ////////PI controller integral function///////////////////
   /*  	ER_ICP = ER_IC*62;	  
		 if(state==0)                            
	{
	   // ER_IC = 0;
		ER_IC1 =0;
	} 
   	ER_IC1 = ER_IC1 + 2*ER_IC;

	ER_ICR = ER_IC1; */
	
   //	ER_ICR = ER_IC*3;
//////////////////////////////////////////////
//////////Quasi-PR controller resonant function
//Quasi_PR  双线性变换 PR
/* */ 	

	ER_ICP = ER_IC*55; 

	if(state==0)			
	{xn[0]=0;
	 xn[1]=0;
	 xn[2]=0;
	 yn[0]=0;
	 yn[1]=0;
	 yn[2]=0;
	}
	xn[2]= xn[1];
	xn[1] = xn[0];
	xn[0] = ER_IC;
	yn[2] = yn[1];
	yn[1] = yn[0];
	yn[0] = xn[0]-xn[2];
	yn[0] = 0.00025*yn[0];
	yn[0] = 5300*yn[0];//4500
	yn[0] += 1.999315*yn[1];
	yn[0] -= 0.999500*yn[2];
	limtz =120;
	limtf =-120;
	if(yn[0]>limtz)
		yn[0] = limtz;
	if(yn[0]<limtf)
		yn[0] =limtf;
	
	ER_ICR = yn[0];	

// Quasi_PR  双线性I

	limtz2 = 200;
	limtf2 = -200;
////////////////////////////////////////////////////////////////////////////
	UREF = ER_ICP + ER_ICR;	//  ;// 
  			if(UREF>limtz2) UREF=limtz2;
	        else if(UREF<limtf2) UREF=limtf2;


//	UREF = 50*ER_IC;
//*********  yt
	ConversionCount++;
/*  	

	if(ER_IC>HB)
		 Uref1=-400;
   else	if(ER_IC<-HB)
		 Uref1=400; 
		 Uref2=-Uref1;	*/ 
 	 
		  
   	
	Uref1 =-1*UREF;
	if(Uref1>limtz2) Uref1=limtz2;
	else if(Uref1<limtf2) Uref1=limtf2;
    Uref2=-Uref1; /**/   //另一桥臂
	   
	//  state == 1 时关闭画图                                //     ceshi1   ceshi2  ceshi3
 if(state==1)  
    {
	  if(g<400)
	  {	  
	  // step1 
	   /*  	
	   ceshi1[g]= ICALP_REF;   //Vs
           ceshi2[g]= ICOM2;  //il
           ceshi3[g]= ICOM4; //is_ideal
	   	   ceshi4[g]= UAN;	*/ 
	//step2
		/*		
  		ceshi1[g]= Udct;
    	ceshi2[g]= Udcpi;
   		ceshi3[g]= ER_IC;
		ceshi4[g]= UREF;   */
	//step3
	   /*  			
  		ceshi1[g]= Udcpi;
    	ceshi2[g]= ER_IC;
   		ceshi3[g]= ER_IC2;
		ceshi4[g]= UREF;  */ 
	//step4
	   /*   */
	    ceshi1[g]= Udcav;
	    //ceshi1[g]= PLALP;
    	ceshi2[g]= Pac;		
  		/*ceshi1[g]= ER_IC;
    	ceshi2[g]=ER_ICP;*/
   		ceshi3[g]= ICOM3;
		ceshi4[g]= UREF;  
		//ceshi4[g]= 311*SinTable[g];  
	 //step2
				
  			// ceshi1[g]= (AdcRegs.ADCRESULT9>>4);
      		// ceshi2[g]= (AdcRegs.ADCRESULT10>>4);
       		//ceshi3[g]= (AdcRegs.ADCRESULT14>>4);
			//ceshi4[g] = (AdcRegs.ADCRESULT15>>4);

	    g++; 
       // ces.hi1[j]= -Iref1dc+Uavpi;          
        //ceshi2[j]= UANAV*1.414*SinTable[Time0Count]-UBNS;
       // ceshi3[j]= ICOM3;
	   // j++;
	  }
      else g=0;
   }
 
	*(Cmp1Write) = (Uint16)(Uref1*5+1000);                                
	*(Cmp2Write) = (Uint16)(Uref2*5+1000);	 
/*	*(Cmp1Write) = (Uint16)(Uref1*5+1000);                                
	*(Cmp2Write) = (Uint16)(Uref2*5+1000); */

//  *(Cmp1Write)=(Uint16)( SinTable[Time0Count]*800+1000);
//  *(Cmp2Write)=(Uint16)(-SinTable[Time0Count]*800+1000);

//计算整体有效值
    UANSUM=UANSUM+UAN*UAN*0.0025;   //400点，采样400点的平均值
	UBNSUM=UBNSUM+UBN*UBN*0.0025;
	ILASUM=ILASUM+ILA*ILA*0.0025;
	ICOM1SUM=ICOM1SUM+ICOM1*ICOM1*0.0025;

    dataflag=1;
    AdcRegs.ADCTRL2.bit.RST_SEQ1=1;
	PieCtrlRegs.PIEACK.bit.ACK1=1;
	AdcRegs.ADCST.bit.INT_SEQ1_CLR=1;
	EINT;                                        //裳曛疚�
}
//////////////////endendendendendendendendendendendendendendendendendendendendendendendendendendendendendendendend
void start(void)                                                //启动PWM
{
    *(CommandStart)=0x0005;
	state=1;
	rsstate=2;
}

void stop(void)                                                 //封锁PWM
{
    *(CommandStop)=0x0002;
	state=0;
	ICOMcount=0;
	if(chargflag==0) rsstate=0;
	else rsstate=3;
}


// DC 电压欠压保护   150V
void fcheck1(void)
{
    if(Udcav<50)                                            //直流母线电压实际值小于直流母线电压下限值，为欠压
    {
        stop();
		ubvf=1;
	}
    else
	{
		ubvf=0;
	}

	igbtfault.all=*(IgbtFaultRead);                             //IGBT故障检测
	if(igbtfault.bit.Aup==1||igbtfault.bit.Ado==1||igbtfault.bit.Bup==1||igbtfault.bit.Bdo==1||igbtfault.bit.Cup==1||igbtfault.bit.Cdo==1)
	{
	    igbtf=1;
	    stop();
        faultf=1;
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

}
 
// DC电压  和输入电流保护检测
void fcheck2(void)
{

    if((Udcav>280)||(UBN>320)||(UBN<-320))     
	{
	    stop();                                      
	    faultf=1;                                              
		uovf=1;
	}
	Isocval=50;
	if((ICOM1>Isocval)||(ICOM1<-Isocval))      //逆变器输出电流是否过流
	{
	    ICOMcount=ICOMcount+1;
		if(ICOMcount==2)
		{
	      stop();                                                
	      faultf=1;                                             
	      socf=1;
		}
	} 
	else
	{
	   ICOMcount=0;
	} 
	
}
      //过零捕捉，提取相位
interrupt void Zero_cap_isr(void)
{
    pllt = CpuTimer0.InterruptCount;                            //读取上个工频周期的中断次数
    if(pllt>250)
   {
	prdtemp = CpuTimer0Regs.PRD.all * pllt / 400; 
	 CpuTimer0Regs.PRD.all = prdtemp;              //新的计数周期计算
	if(prdtemp>7350&&prdtemp<7650)                               //prdtemp标准值是7500(150M*50us)(电网频率的偏差在49Hz~51Hz之间）
	{                       //计数值正常加载新的计数值
		faultsta.bit.sff=0;
	}
	else 
	{
	    stop();
	    faultsta.bit.sff=1;                                                
	}

	CpuTimer0.InterruptCount = 0;
	UANAV=sqrt(UANSUM);
	//UBNAV=sqrt(UBNSUM);
    ILAAV=sqrt(ILASUM);	
	ICOM1AV=sqrt(ICOM1SUM);	

	UANSUM=0;
	UBNSUM=0;	
	ILASUM=0;
	ICOM1SUM=0;
	}
	pllflag=1;
	PieCtrlRegs.PIEACK.bit.ACK1=1;
}

// 系统频率故障	    
void fcheck(void)
{
	if((prdtemp * CpuTimer0.InterruptCount)>3080000)           //检查电网周期  3080000？
	{
        stop();
		CpuTimer0.InterruptCount = 500;                        //大于400就可以了
		faultsta.bit.sff=1;
	}

	if(UANAV>150)
	{
	    stop();
		faultsta.bit.sovf=1;
	}
	else
	{
	    faultsta.bit.sovf=0;
	}
		
	if(UANAV<40)
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

void controlset(void)    // 触摸屏模式
{
    Operamode=(Uint16)BytleValue[14];                          //运行方式
	Indval   =(float)BytleValue[15];                           //定值感性无功
	Capval   =(float)BytleValue[16];                           //ㄖ等菪晕薰�
}
   
void maindataset(void)
{
    Operamode=(Uint16)BytleValue[14]; 
}

//  故障信号赋值   传到触摸屏
void secdeal(void)
{


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

	if(rsorder)	
    faultsta.bit.fallflag=fmainflag||faultstasecall.bit.fsecflag||faultstasec.bit.ubvf;
   	else
    faultsta.bit.fallflag=faultstasecall.bit.fsecflag;
	
	faultstasecall.bit.rsstate = faultstasec.bit.rsstate;
	faultstasecall.bit.contactor1 = faultstasec.bit.contactor1;
	faultstasecall.bit.contactor2 = faultstasec.bit.contactor2;
	faultstasecall.bit.fan     = faultstasec.bit.fan;

}

/*void LPFCAL(float Id_in,float Iq_in)
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

*/

//YT
/*
float lowpass1(float x1)
{
   float temp = 0,result=0;
   U1_lp[2] = x1 - b1_lp * U1_lp[1] - b2_lp * U1_lp[0];
   result = a0_lp * U1_lp[2] + a1_lp * U1_lp[1] + a2_lp * U1_lp[0];
   temp = U1_lp[1];
   U1_lp[1] = U1_lp[2];
   U1_lp[0] = temp;  
   return result;
}

float lowpass2(float x2)
{
   float temp = 0,result=0;
   U2_lp[2] = x2 - b1_lp * U2_lp[1] - b2_lp * U2_lp[0];
   result = a0_lp * U2_lp[2] + a1_lp * U2_lp[1] + a2_lp * U2_lp[0];
   temp = U2_lp[1];
   U2_lp[1] = U2_lp[2];
   U2_lp[0] = temp;   
   return result;
}

float lowpass3(float x3)
{
   float temp = 0,result=0;
   U3_lp[2] = x3 - b1_lp * U3_lp[1] - b2_lp * U3_lp[0];
   result = a0_lp * U3_lp[2] + a1_lp * U3_lp[1] + a2_lp * U3_lp[0];
   temp = U3_lp[1];
   U3_lp[1] = U3_lp[2];
   U3_lp[0] = temp;   
   return result;
}
*/
//*********  yt

// iir filter subfuncation, input signal
float InputWave(float input)
{
	for (j=IIRNUMBER-1;j>0;j--)
	{
		fXn[j]=fXn[j-1];
		fYn[j]=fYn[j-1];

	}
    fXn[0]=input;
	fYn[0]=0.0;
	return(fXn[0]);
}
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// iir filter subfuncation, filter function 
float IIR()
{
	float fSum;
	fSum=0.0;
	for ( j=0;j<IIRNUMBER;j++ )
	{
		fSum+=(fXn[j]*fAn[j]);
		fSum+=(fYn[j]*fBn[j]);
	}
		fYn[0]=fSum;    //important  revised (no this command in example)
 	return(fSum);

}
//--------------------------------------------------------------------------------------

/*
void ms_delay(Uint16 i)
{

        Uint16 j=0;

  for(;i>0;i--)
  {

          for(j=0;j<=20;j++){}

  }

}

 

void s_delay(Uint16 i)
{
      
   Uint16 j=0;

  for(;i>0;i--)
  {

          for(j=0;j<=1000;j++)
                        {
								
                  ms_delay(1);

                          }

  }

}
 */






