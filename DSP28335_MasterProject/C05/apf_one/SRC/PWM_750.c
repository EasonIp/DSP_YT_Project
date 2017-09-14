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

//////////重复控制中的低通滤波器参数
#define a0_lp 1*0.01//1*0.067455
#define a1_lp 2*0.01//2*0.067455   
#define a2_lp 1*0.01//1*0.067455
#define b1_lp -1.14298     
#define b2_lp  0.4128

//////////（5250-5500）重复控制中的带阻滤波器参数//////////
#define a0_bd  1*0.0893567
#define a1_bd  0.250945*0.0893567
#define a2_bd  1*0.0893567
#define b1_bd  0.239644
#define b2_bd  0.90993      

#pragma CODE_SECTION(adcisr,"ramfuncs")
#pragma CODE_SECTION(Zero_cap_isr,"ramfuncs")
#pragma CODE_SECTION(cpu_timer0_isr,"ramfuncs")
#pragma CODE_SECTION(LPFCAL,"ramfuncs")

float U1_lp[3] = {0,0,0};     //低通滤波器中所需的变量
float U2_lp[3] = {0,0,0};
float U3_lp[3] = {0,0,0};
float U1_bd[3] = {0,0,0};     //带阻滤波器中所需的变量
float U2_bd[3] = {0,0,0};
float U3_bd[3] = {0,0,0};

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

extern int BytleValue[];

Uint16 i=0,j=0,k=0;                                //一些常用的变量
float SinTable[1400];										  //正弦表
//float Sin5Table[700];
//float Sin7Table[700];
//float Sin11Table[700];
//float Sin13Table[700];
float Iddata[400];
float Iqdata[400];
//float yucea[405];
//float yuceb[405];
//float yucec[405];
float rctablea[400];
float rctableb[400];
float rctablec[400];
float ceshi1[400];
float ceshi2[400];
float ceshi3[400];
float Iref1av,Iref2av,Iref3av,Kref=1;
float Iref1sum=0,Iref2sum=0,Iref3sum=0;
//float  UBCAV=380,UBCSUM=0,UABAV=380,UABSUM=0,UCAAV=380,UCASUM=0;
float  UANAV=220,UANSUM=0,UBNAV=220,UBNSUM=0,UCNAV=220,UCNSUM=0;
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
Uint16 Udc_temp1,Udc_temp2,ICOM1_temp,ICOM2_temp,ICOM3_temp;                 //负载电流和补偿电流的采样值
//Uint16 IAref_temp,IBref_temp,ICref_temp; 

float  UAB,UBC,UAN,UBN,UCN,ILA,ILB,ILC,Udc,Udc1,Udc2,Tem1,Tem2,Temax=25,ICOM1,ICOM2,ICOM3;      //电网电压、直流侧电压和温度的实际值和负载电流和补偿电流的实际值                                  //
//float  IAref,IBref,ICref;
float  Tesecmax;
float  UANsim,UBNsim,UCNsim,ILAsim,ILBsim,ILCsim,ICOM1sim,ICOM2sim,ICOM3sim,Udcavsim,Udc1sim,Udc2sim;//实际值转换到仿真中的值
//float UANsimago1, UANsimago2, UANsimago3, UBNsimago1, UBNsimago2, UBNsimago3, UCNsimago1, UCNsimago2, UCNsimago3;
//float  IAHamsim,IBHamsim,ICHamsim;
//float  Uabsim=0,Ubcsim=0,Uabpsim,Ubcpsim;

float Iocak=0,Iocbk=0,Iocck=0;
float Idsys,Iqsys,Idsysav=0,Iqsysav=0;
float Id,Iq;                                                  //abc-dq0中的dq分量
float Idav=0,Iqav=0;                                          //对dq分量进行低通滤波
float IAHam,IBHam,ICHam;
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
float UdcTarget=300;
float KUdcT=1/300;
float UdcKp=3.96,UdcKi=0.00335,IKp=6;                                                    //电流环比例系数;
Uint16 SwitchFre=10000;
float Udcovval=400,Udcbvval=490,Isocval=220,Usovval=240,Usbvval=180,Totval=80;
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
//float cmp1,cmp2,cmp3;
//float Di1,Di2,Di3;                                            //预测电流指令

Uint16 Time0Count=0;
Uint16 faultf  = 0;											  //错误（故障）标志
Uint16 state = 0;                                           //运行状态
Uint16 ftime   = 0;								              //开关防抖计数（机械开关用）
Uint16 runcount= 0;                                           //运行灯闪烁功能所需计数变量
Uint16 dataflag= 0;											  //采样数据标志
Uint16 chargflag=0;                                           //充断电标疚�
Uint16 rctcount=0;                                           //重复控制数组计数
Uint16 chargcount=0;
Uint16 runorcount = 0;

interrupt void cpu_timer0_isr(void);                          //cpu定时器0中断服务子程序
interrupt void scibrx(void);                                  //SCI通信接受中断
interrupt void adcisr(void);                                  //AD采样服务子程序
interrupt void Zero_cap_isr(void);                            //电网电压过零捕捉中断

void maindataset(void);
//void secdataset(void);
void controlset(void);
void tempdeal(void);
void datadeal(void);
void fcheck(void);                                           //检测系统电压和频率
void secdeal(void);
void fcheck1(void);                                           //检测直流母线是否欠压，过温，IGBT是否故障
void fcheck2(void);                                           //检测直流母线是否过压，过流
void start(void);
void stop(void);
void contactordeal(void);
float lowpass1(float);                                        //重复控制中的低通滤波器
float lowpass2(float);
float lowpass3(float);
float bandstop1(float);                                       //重复控制中的带阻滤波器
float bandstop2(float);
float bandstop3(float);
void LPFCAL(float Id_in,float Iq_in);

float Idavtemp=0;
float Iqavtemp=0;
Uint16 lpfcount=0;

void main(){
    InitSysCtrl();                                            //初始化系统配置
    EALLOW;
    SysCtrlRegs.HISPCP.all=adcclk;                            //25M
    EDIS;                                           
	memcpy(&ramfuncsRunStart, &ramfuncsLoadStart, &ramfuncsLoadEnd-&ramfuncsLoadStart);
	InitFlash();                                              //初始化FLASH
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
    AdcRegs.ADCMAXCONV.all = 0x000F;           //16对通道转换
	
	BytleValue[76]=iniflag;

	for(i=0;i<1400;i++)
    {
        SinTable[i]=sin(2*pi*i/400);										//构造正弦表
	}

/*	for(i=0;i<405;i++)
	{
	    yucea[i]=0;
	    yuceb[i]=0;
	    yucec[i]=0;
	}*/
/*	for(i=0;i<600;i++)
	{
	    Sin9Table[i]=sin(2*pi*i/44);
	}*/
/*	for(i=0;i<700;i++)
	{
	    Sin11Table[i]=sin(2*pi*i/36);
	}
	for(i=0;i<700;i++)
	{
	    Sin13Table[i]=sin(2*pi*i/31);
	}*/
	for(i=0;i<400;i++) Iddata[i]=0;
	for(i=0;i<400;i++) Iqdata[i]=0;
	for(i=0;i<400;i++)
	{
	    rctablea[i]=0;
		rctableb[i]=0;
		rctablec[i]=0;
	}
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
		
		contactordeal();

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
			if(state==0&&!runorcount&&contactor2&&iniflag==0) start();
			if(state==1&&faultstasec.bit.faultf==1)
			{
			   stop();
			   BytleValue[79]=0;                                      //装置运行过程中出现致命故障需对逆变运行指令清零
			}
		}
		else if(state==1) stop();

/*		if(controlstasec.bit.reset==1)                        //复位
		{
		    faultf=0;
			uovf=0;
			socf=0;
			igbtf=0;
		}
		*/

/*		if(controlstasec.bit.charg_chargof==1&&faultf==0&&chargflag==0)  //模块充电
		{
//		    contactorK1=1;
//			contactorK2=0;
            contactor1=1;
			contactor2=0;
//			chargflag=1;
			rsstate=1;
		}

		if(controlstasec.bit.charg_chargof==2&&chargflag==1||faultf==1)  //模块断电
		{
//		    chargflag=0;
			stop();
//		    contactorK1=0;
//			contactorK2=0;
            contactor1=0;
   			contactor2=0;
		}*/

        fcheck1();                                                    
        
		datadeal();

		if(crcflag==1) modbusdeal();

		cmptxdeal();
		tempdeal();
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
	if(chargcount) chargcount--;
	if(runorcount) runorcount--;
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
	*(Commandtongbu)=0x0003;                                                     
    AdcRegs.ADCTRL2.bit.SOC_SEQ1=1;
	CpuTimer0.InterruptCount++;	  
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;
	CpuTimer0Regs.TCR.bit.TIF   = 1;                                               
}

interrupt void adcisr(void)
{
    UAB_temp	= (AdcRegs.ADCRESULT0>>4);
	UCB_temp	= (AdcRegs.ADCRESULT1>>4);
//  UCN_temp	= (AdcRegs.ADCRESULT2>>4);
	Udc_temp1	= (AdcRegs.ADCRESULT3>>4);
	Udc_temp2	= (AdcRegs.ADCRESULT4>>4);
	ICOM1_temp	= (AdcRegs.ADCRESULT9>>4);
	ICOM2_temp	= (AdcRegs.ADCRESULT10>>4);
	ICOM3_temp	= (AdcRegs.ADCRESULT11>>4);
	Tem1_temp	= (AdcRegs.ADCRESULT8>>4);
	ILA_temp = (AdcRegs.ADCRESULT5>>4);
	ILB_temp = (AdcRegs.ADCRESULT6>>4);
	ILC_temp = (AdcRegs.ADCRESULT7>>4);

//    Time0Count=CpuTimer0.InterruptCount+163;                     //相位同步
	
	UAB     = ((float)UAB_temp - 2070)*0.53267045;      //  ((float)UAB_temp - 2051)*0.53267045;       //比例系数需根据传感器和采样调理电路来定
	UBC		= ((float) 2060-UCB_temp)*0.53267045;//( 2072 - (float) UCB_temp )*0.53267045;
//  UCN		= ( (float) UCN_temp-2057 )*0.53267045;  
	Udc1	= ( (float) Udc_temp1 - 8 )*0.3197;//0.351648;//0.293;	    //1956    1927( (float) Udc_temp - 23 ) * 0.30528;
    Udc2	= 0; //0.351648;//0.293;
    ICOM1	= ( (float) ICOM1_temp - 2074 )*0.24414;  //( (float) ICOM1_temp - 2070 ) * 0.2441406; 
	ICOM2	= ( (float) ICOM2_temp - 2064 )*0.24414;  // ( (float) ICOM2_temp - 2064 ) * 0.2441406;
	ICOM3	= ( (float) ICOM3_temp - 2074 )*0.24414;  //( (float) ICOM3_temp - 2069 ) * 0.2441406; 
    Tem1    = ( (float) Tem1_temp - 2070 ) * 0.00293 ;
	ILA	    =  ( (float) ILA_temp - 2077 ) * coefficient;// 0.1057;//*0.5;   //*ct/10
	ILB  	= ( (float) ILB_temp - 2058 ) * coefficient;// 0.1057;//*0.5; 
	ILC	    = ( (float) ILC_temp - 2075 ) * coefficient;// 0.1057;//*0.5;
	Isysa   =0.9*Isysa+0.1*(-ICOM1+ILA);
	Isysb   =0.9*Isysb+0.1*(-ICOM2+ILB);
	Isysc   =0.9*Isysc+0.1*(-ICOM3+ILC);
//	UAN=
    UAN=(2*UAB+UBC)*0.3333;
	UBN=(UBC-UAB)*0.3333;
	UCN=(-UAB-2*UBC)*0.3333;

    if(state){
	if(j<400)
	{
	   
      // ceshi1[j]=Iref1se;//Udcavsim>UdcTarget*0.0027;
      // ceshi2[j]=Iref2se;
      ceshi1[j]= UAN;//(float) ICOM1_temp - 2070;
      ceshi2[j]= UBN;//(float) ICOM2_temp - 2060 ;
      ceshi3[j]= UCN;//(float) ICOM3_temp - 2071;   
	   j++;
	}
    else j=0;
}

	us_alpha=0.8165*(UAN-0.5*UBN-0.5*UCN);
    us_beta=0.7071*(UBN-UCN);
	Time0Count=atan2(us_beta,us_alpha)*63.69+500;
	   
	Id=0.6667*(ILA*SinTable[Time0Count]+ILB*SinTable[Time0Count+267]+ILC*SinTable[Time0Count+133]);
	Iq=0.6667*(ILA*SinTable[Time0Count+100]+ILB*SinTable[Time0Count+367]+ILC*SinTable[Time0Count+233]);
    
	LPFCAL(Id,Iq);                                       //低通滤波器
    
    IHam_ref1=Idav*SinTable[Time0Count]+Iqav*SinTable[Time0Count+100];         //基波电流
	IHam_ref2=Idav*SinTable[Time0Count+267]+Iqav*SinTable[Time0Count+367];
	IHam_ref3=-IHam_ref1-IHam_ref2;

	IAHam=ILA-IHam_ref1;                                 //谐波电流
	IBHam=ILB-IHam_ref2;
	ICHam=ILC-IHam_ref3;
	
/*	yucea[yucecount]=IAHam;
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
	yucec[402]=yucec[2];*/
	
                                 
//   Operamode=1;
	switch(Operamode)                                  //根据触摸屏的指令选择相应的指令电流
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
/*		case 4:
            	   Iref1se=Ham1val*SinTable[Time0Count]+Ham5val*Sin5Table[Time0Count]+Ham7val*Sin7Table[Time0Count];//+Ham11val*Sin11Table[Time0Count]+Ham13val*Sin13Table[Time0Count];                     
		           Iref2se=Ham1val*SinTable[Time0Count+267]+Ham5val*Sin5Table[Time0Count+53]+Ham7val*Sin7Table[Time0Count+38];//+Ham11val*Sin11Table[Time0Count+24]+Ham13val*Sin13Table[Time0Count+21];
		           Iref3se=(-Ham1val*SinTable[Time0Count]-Ham1val*SinTable[Time0Count+267])+(-Ham5val*Sin5Table[Time0Count]-Ham5val*Sin5Table[Time0Count+53])+(-Ham7val*Sin7Table[Time0Count]-Ham7val*Sin7Table[Time0Count+38]);//+(-Ham11val*Sin11Table[Time0Count]-Ham11val*Sin11Table[Time0Count+24])+(-Ham13val*Sin13Table[Time0Count]-Ham13val*Sin13Table[Time0Count+21]);
 				   break;*/

		case 6:
            	   Iref1se=IAHam;//yucea[yucecount+1]*1.1;//IAHam*0.33333;                          
		           Iref2se=IBHam;//yuceb[yucecount+1]*1.1;//IBHam*0.33333;
		           Iref3se=ICHam;//yucec[yucecount+1]*1.1;//ICHam*0.33333;
				   break;
		case 7:
            	   Iref1se=Iqav*SinTable[Time0Count+100];//yucea[yucecount+1]*1.1;//IAHam*0.33333;                          
		           Iref2se=Iqav*SinTable[Time0Count+367];//yuceb[yucecount+1]*1.1;//IBHam*0.33333;
		           Iref3se=-Iqav*SinTable[Time0Count+100]-Iqav*SinTable[Time0Count+367];//yucec[yucecount+1]*1.1;//ICHam*0.33333;
				   break;
		case 8:
            	   Iref1se=IAHam+Iqav*SinTable[Time0Count+100];//yucea[yucecount+1]*1.1;//IAHam*0.33333;                          
		           Iref2se=IBHam+Iqav*SinTable[Time0Count+367];//yuceb[yucecount+1]*1.1;//IBHam*0.33333;
		           Iref3se=ICHam-Iqav*SinTable[Time0Count+100]-Iqav*SinTable[Time0Count+367];//yucec[yucecount+1]*1.1;//ICHam*0.33333;
				   break;

		default:   Iref1se=0;                          
		           Iref2se=0;
		           Iref3se=0;
				   break;					       
	}
	Iref1se*=Kref;
	Iref2se*=Kref;
	Iref3se*=Kref;
	Udc1av=0.996*Udc1av+0.004*Udc1;
	Udc2av=0.996*Udc2av+0.004*Udc2; 
	Udcav=Udc1av+Udc2av;                         //直流母线电压滤波

	fcheck2();                                           //直流母线是否过压和逆变器输出电流是否过流检测

    UANsim=UAN*10*KUdcT;                             //将实际值转成仿真中的值
	UBNsim=UBN*10*KUdcT;
    UCNsim=UCN*10*KUdcT;
	Udcavsim=0.003*Udcav;
    Udc1sim=0.003*Udc1av;
    Udc2sim=0.003*Udc2av;
	ICOM1sim=0.015*ICOM1;
	ICOM2sim=0.015*ICOM2;
	ICOM3sim=0.015*ICOM3;
    
    Udct=UdcTarget*0.003-Udcavsim;

	if(state==0)                            //积分清零
	{
	    Udci=0;
	}
    
	Udci=Udci+UdcKi*Udct; 
	                                 
    if(Udci<-1)
		Udci=-1;												  //积分限幅
	else if(Udci>0.1)
		Udci=0.1;

	Udcpi=Udci+UdcKp*Udct;

	if(Udcpi>0.05) 
		Udcpi=0.05;												  //限幅
	else if(Udcpi<-1.5)
		Udcpi=-1.5;
    
    
    Udccat=Udc1sim-Udc2sim;
    Udccai=Udccai+0.02*Udccat;
	if(Udccai>=0.05)
	{
	   Udccai=0.05;
	}
	else if(Udccai<=-0.05)
	{
	   Udccai=-0.05;
	}
    Udccapi=Udccai;//+8.3*Udccat;
    if(Udccapi>=0.1)
	{
	   Udccapi=0.1;
	}
	else if(Udccapi<=-0.1)
	{
	   Udccapi=-0.1;
	}

	Iref1dc=2*Udcpi*SinTable[Time0Count];                //稳压指令
	Iref2dc=2*Udcpi*SinTable[Time0Count+267];
	Iref3dc=-Iref1dc-Iref2dc;

    Iref1dc=Iref1dc;//-0.3333*Udccapi;
	Iref2dc=Iref2dc;//-0.3333*Udccapi;
	Iref3dc=Iref3dc;//-0.3333*Udccapi;

/*		UANsimago3=UANsimago2;
		UANsimago2=UANsimago1;
		UANsimago1=UANsim;

		UBNsimago3=UBNsimago2;
		UBNsimago2=UBNsimago1;
		UBNsimago1=UBNsim;

		UCNsimago3=UCNsimago2;
		UCNsimago2=UCNsimago1;
		UCNsimago1=UCNsim;*/

	if(Udcavsim>UdcTarget*0.00285)                                 //直流电压大于690V，在指令中加入谐波指令
	{
		Iref1=Iref1dc-0.015*Iref1se;                               //指令电流
		Iref2=Iref2dc-0.015*Iref2se;
		Iref3=Iref3dc-0.015*Iref3se;


		Iocak=0.5*UANsim;
		Iocbk=0.5*UBNsim;
		Iocck=0.5*UCNsim;

	}
	else
	{
	    Iref1=Iref1dc;
		Iref2=Iref2dc;
		Iref3=Iref3dc;

		Iocak=0.5*(10*KUdcT)*UANAV*1.414*SinTable[Time0Count];
		Iocbk=0.5*(10*KUdcT)*UBNAV*1.414*SinTable[Time0Count+267];
 		Iocck=0.5*(10*KUdcT)*UCNAV*1.414*SinTable[Time0Count+133];

	}

	Iref1=-Iref1-ICOM1sim;                                  //逆变器输出电流反馈
	Iref2=-Iref2-ICOM2sim;
	Iref3=-Iref3-ICOM3sim;

	Iref1pi=3*Iref1;                                      //电流环比例
	Iref2pi=3*Iref2;
	Iref3pi=3*Iref3;
	
	if(state){
	rctablea[rctcount]=0.9*rctablea[rctcount]+Iref1;                    //重复控制数组累积
	if(rctablea[rctcount]>2) rctablea[rctcount]=2;
	else if(rctablea[rctcount]<-2) rctablea[rctcount]=-2;
	rctableb[rctcount]=0.9*rctableb[rctcount]+Iref2;
	if(rctableb[rctcount]>2) rctableb[rctcount]=2;
	else if(rctableb[rctcount]<-2) rctableb[rctcount]=-2;
	rctablec[rctcount]=0.9*rctablec[rctcount]+Iref3;
	if(rctablec[rctcount]>2) rctablec[rctcount]=2;
	else if(rctablec[rctcount]<-2) rctablec[rctcount]=-2;
    rctcount++;
    if(rctcount>=400) rctcount=0;
    }

	Uref1=Iref1pi+UANsim;//1.5*(10*KUdcT)*UANAV*1.414*SinTable[Time0Count]-Iocak;//+lowpass1(rctablea[(rctcount+3)%400]); //  //在电流环比例前馈中加入重复控制输出                   //重复控制写入调制波
	Uref2=Iref2pi+UBNsim;//1.5*(10*KUdcT)*UBNAV*1.414*SinTable[Time0Count+267]-Iocbk;//+lowpass2(rctableb[(rctcount+3)%400]);//-Iocbk
	Uref3=Iref3pi+UCNsim;//1.5*(10*KUdcT)*UCNAV*1.414*SinTable[Time0Count+133]-Iocck;//+lowpass3(rctablec[(rctcount+3)%400]);//-Iocck

	if(Uref1>5) Uref1=5;
	else if(Uref1<-5) Uref1=-5;
    if(Uref2>5) Uref2=5;
	else if(Uref2<-5) Uref2=-5;
	if(Uref3>5) Uref3=5;
	else if(Uref3<-5) Uref3=-5;

	//向FPGA传输调制波
	*(Cmp1Write) = (Uint16)(Uref1 * 200+1000);                                //数值类型转化:比较值经过扩大180倍加偏置之后取整传输给FPGA
	*(Cmp2Write) = (Uint16)(Uref2 * 200+1000);
	*(Cmp3Write) = (Uint16)(Uref3 * 200+1000);
 //  *(Cmp1Write)=(Uint16)(SinTable[Time0Count] * 800+1000);
 //  *(Cmp2Write)=(Uint16)(SinTable[Time0Count+267] * 800+1000);
 //  *(Cmp3Write)=(Uint16)(SinTable[Time0Count+133] * 800+1000);
    yucecount++;
    if(yucecount>=400) yucecount=0;
    	UANSUM=UANSUM+UAN*UAN*0.0025;
		UBNSUM=UBNSUM+UBN*UBN*0.0025;
		UCNSUM=UCNSUM+UCN*UCN*0.0025;
        
		ILASUM=ILASUM+ILA*ILA*0.0025;
		ILBSUM=ILBSUM+ILB*ILB*0.0025;
		ILCSUM=ILCSUM+ILC*ILC*0.0025;
	    ICOM1SUM=ICOM1SUM+ICOM1*ICOM1*0.0025;
	    ICOM2SUM=ICOM2SUM+ICOM2*ICOM2*0.0025;
	    ICOM3SUM=ICOM3SUM+ICOM3*ICOM3*0.0025;
		Isysasum=Isysasum+Isysa*Isysa*0.0025;
		Isysbsum=Isysbsum+Isysb*Isysb*0.0025;
		Isyscsum=Isyscsum+Isysc*Isysc*0.0025;
		Iref1sum+=Iref1se*Iref1se*0.0025;
		Iref2sum+=Iref2se*Iref2se*0.0025;
		Iref3sum+=Iref3se*Iref3se*0.0025;
	Idsys=0.6667*(Isysa*SinTable[Time0Count]+Isysb*SinTable[Time0Count+267]+Isysc*SinTable[Time0Count+133]);
	Iqsys=0.6667*(Isysa*SinTable[Time0Count+100]+Isysb*SinTable[Time0Count+367]+Isysc*SinTable[Time0Count+233]);
	Idsysav=Idsysav*0.999+Idsys*0.001;
	Iqsysav=Iqsysav*0.999+Iqsys*0.001;
    dataflag=1;
    AdcRegs.ADCTRL2.bit.RST_SEQ1=1;
	PieCtrlRegs.PIEACK.bit.ACK1=1;
	AdcRegs.ADCST.bit.INT_SEQ1_CLR=1;
	EINT;                                        //采样标志位
}

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
	if(chargflag==0) rsstate=0;
	else rsstate=3;
}

void fcheck1(void)
{
   if(Udcav<540/*Udcbvval*/)                                            //直流母线电压实际值小于直流母线电压下限值，为欠压
    {
        stop();
//      contactorK2=0;                                         //保持接触器断开，软启动
  //      contactor2=0;
		ubvf=1;
	}
    else
	{
//	    if(faultf==0) contactorK2=0;
//      else contactorK2=1;
//        contactor2=1;
		ubvf=0;
	}

/*	if(Temax>=Totval)
	{
	    stop();
//	    fanK3=1;
//		fan=1;
		otf=1;
	}
	else
	{
//	    fanK3=0;
//		fan=0;
		//otf=0;
	}
	*/
	if(chargcount>40000||runorcount>10000);
	else{
	igbtfault.all=*(IgbtFaultRead);                             //IGBT故障检测
	if(igbtfault.bit.Aup==1||igbtfault.bit.Ado==1||igbtfault.bit.Bup==1||igbtfault.bit.Bdo==1||igbtfault.bit.Cup==1||igbtfault.bit.Cdo==1)
	{
	    igbtf=1;
	    stop();
        faultf=1;
//		contactorK1=0;
//		contactorK2=0;
//	    contactor1=0;
//		contactor2=0;
	}
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
	if(chargcount>40000||runorcount>10000);
	else{
    if(Udcav>800/*Udcovval*/)     
	{
	    stop();                                      //给FPGA发封锁PWM信号
	    faultf=1;                                              //置错误位
//      contactorK1=0;
//		contactorK2=0;
//		contactor1=0;
//		contactor2=0;
		uovf=1;
	}
	Isocval=200;
	if((ICOM1>Isocval)||(ICOM1<-Isocval)||(ICOM2>Isocval)||(ICOM2<-Isocval)||(ICOM3>Isocval)||(ICOM3<-Isocval))      //逆变器输出电流是否过流
	{
	    stop();                                                //给FPGA发封锁PWM信号
	    faultf=1;                                              //置错误位
//      contactorK1=0;
//		contactorK2=0;
//		contactor1=0;
//		contactor2=0;
		socf=1;
	}  
	}
}

interrupt void Zero_cap_isr(void)
{
    pllt = CpuTimer0.InterruptCount;                            //读取上个工频周期的中断次数
if(pllt>250){
	prdtemp = CpuTimer0Regs.PRD.all * pllt / 400; 
	 CpuTimer0Regs.PRD.all = prdtemp;              //新的计数周期计算
	if(prdtemp>7350&&prdtemp<7650)                              //prdtemp标准值是7500(150M*50us)(电网频率的偏差在49Hz~51Hz之间）
	{                       //计数值正常加载新的计数值
		faultsta.bit.sff=0;
	}
	else 
	{
//	    stop();
	    faultsta.bit.sff=1;                                                
	}
//	UBCPLL = UBC;                                               //判断相序
	CpuTimer0.InterruptCount = 0;
	    UANAV=sqrt(UANSUM);
		UBNAV=sqrt(UBNSUM);
		UCNAV=sqrt(UCNSUM);
		ILAAV=sqrt(ILASUM);
		ILBAV=sqrt(ILBSUM);
		ILCAV=sqrt(ILCSUM);
	    ICOM1AV=sqrt(ICOM1SUM);
	    ICOM2AV=sqrt(ICOM2SUM);
	    ICOM3AV=sqrt(ICOM3SUM);
		Isysaav=sqrt(Isysasum);
		Isysbav=sqrt(Isysbsum);
		Isyscav=sqrt(Isyscsum);
		Iref1av=sqrt(Iref1sum);
		Iref2av=sqrt(Iref2sum);
		Iref3av=sqrt(Iref3sum);
		if(Iref1av>100||Iref2av>100||Iref3av>100) Kref-=0.01;
		else if(Kref<1) Kref+=0.01;
		UANSUM=0;
		UBNSUM=0;
		UCNSUM=0;
		ILASUM=0;
		ILBSUM=0;
		ILCSUM=0;
	    ICOM1SUM=0;
	    ICOM2SUM=0;
	    ICOM3SUM=0;
		Isysasum=0;
		Isysbsum=0;
		Isyscsum=0;
		Iref1sum=0;
		Iref2sum=0;
		Iref3sum=0;
	}
	pllflag=1;
	PieCtrlRegs.PIEACK.bit.ACK1=1;
}
	    
void fcheck(void)
{
	if((prdtemp * CpuTimer0.InterruptCount)>3080000)           //检查电网周期
	{
        stop();
		CpuTimer0.InterruptCount = 500;                        //大于400就可以了
		faultsta.bit.sff=1;
	}


	if(UANAV>250||UBNAV>250||UCNAV>250)
	{
	    stop();
		faultsta.bit.sovf=1;
	}
	else
	{
	    faultsta.bit.sovf=0;
	}
		
/*	if(UANAV<170||UBNAV<170||UCNAV<170)
	{
	    stop();
		faultsta.bit.sbvf=1;
	}
	else
	{
	    faultsta.bit.sbvf=0;
	}
*/	
	if(faultsta.bit.sovf==1||faultsta.bit.sbvf==1||faultsta.bit.sff==1)   
	      fmainflag=1;
	else  fmainflag=0;
}

void controlset(void)
{
    Operamode=(Uint16)BytleValue[14];                          //运行方式
	Indval   =(float)BytleValue[15];                           //定值感性无功
	Capval   =(float)BytleValue[16];                           //定值容性无功
/*	Ham3val  =(float)BytleValue[17];
	Ham5val  =(float)BytleValue[18];
	Ham7val  =(float)BytleValue[19];
	Ham9val  =(float)BytleValue[20];
	Ham11val =(float)BytleValue[21];
	Ham13val =(float)BytleValue[22];
	Ham1val  =(float)BytleValue[23];

	DynHam3  =(Uint16)BytleValue[25];                          //选性承巢�
	DynHam5  =(Uint16)BytleValue[26];
	DynHam7  =(Uint16)BytleValue[27];
	DynHam9  =(Uint16)BytleValue[28];
	DynHam11 =(Uint16)BytleValue[29];
	DynHam13 =(Uint16)BytleValue[30];
	DynHamHi =(Uint16)BytleValue[31];
	DynHamAll=(Uint16)BytleValue[32];*/
}
   
void maindataset(void)
{
    Operamode=(Uint16)BytleValue[14]; 
//    Wiring   =(Uint16)BytleValue[36];
//	Controlmode=(Uint16)BytleValue[37];
//	L1val    =0.01*(float)BytleValue[38];
//	L2val    =0.01*(float)BytleValue[39];
	CT1Prop  =(float)BytleValue[40];
    CT2Prop  =(float)BytleValue[41];
//	Hall1Prop=(float)BytleValue[42];
//	Hall2Prop=(float)BytleValue[43];
	if(UdcTarget!=BytleValue[48]) {                    
	UdcTarget=(float)BytleValue[48];
	UdcTarget=730;
	KUdcT=1/UdcTarget;
	}


//	UdcKp    =0.01*(float)BytleValue[45];                      //触摸屏的Kp是扩大了100倍传过来的
//	UdcKi    =0.00005*(float)BytleValue[46];                   //采样频率是20000Hz，触摸屏传过来的Ki要除以20000
//	SwitchFre=(Uint16)BytleValue[47];

    Udcovval =(float)BytleValue[49];
	Udcbvval =(float)BytleValue[50];
	Isocval  =(float)BytleValue[51];
	Usovval  =(float)BytleValue[52];
	Usbvval  =(float)BytleValue[53];
	Totval   =(float)BytleValue[54];
	number   =(float)BytleValue[55];
	coefficient=0;//(float)0.00266*(CT1Prop/CT2Prop)*(1/number);

//	IKp=(L1val+L2val)*20;                                     //(0.1mH+0.2mH)*20KHz
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
//    Tesecmax        = 80;//*(TesecmaxRead);

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

void datadeal(void)
{
	if(pllflag){
	THDLoad=sqrt(1-(Idav*Idav+Iqav*Iqav)/(0.666667*(ILAAV*ILAAV+ILBAV*ILBAV+ILCAV*ILCAV)));
	THDGrid=0.9*THDGrid+0.1*sqrt(1-(Idsysav*Idsysav+Iqsysav*Iqsysav)/(0.666667*(Isysaav*Isysaav+Isysbav*Isysbav+Isyscav*Isyscav)));
	pllflag=0;
	Gridpowfac=sqrt(Idsysav*Idsysav/(Idsysav*Idsysav+Iqsysav*Iqsysav));
	Loadpowfac=sqrt(Idav*Idav/(Idsysav*Idsysav+Iqsysav*Iqsysav));

	}


   /* if(dataflag)
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
	    COUNT=COUNT+1;
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
	} */
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

float bandstop1(float x1)
{
     float temp = 0,result = 0;
	 U1_bd[2] = x1 - b1_bd * U1_bd[1] - b2_bd * U1_bd[0];
	 result = a0_bd * U1_bd[2] + a1_bd * U1_bd[1] + a2_bd * U1_bd[0];
	 temp = U1_bd[1];
	 U1_bd[1] = U1_bd[2];
	 U1_bd[0] = temp;
	 return result;
}

float bandstop2(float x2)
{
     float temp = 0,result = 0;
	 U2_bd[2] = x2 - b1_bd * U2_bd[1] - b2_bd * U2_bd[0];
	 result = a0_bd * U2_bd[2] + a1_bd * U2_bd[1] + a2_bd * U2_bd[0];
	 temp = U2_bd[1];
	 U2_bd[1] = U2_bd[2];
	 U2_bd[0] = temp;
	 return result;
}

float bandstop3(float x3)
{
     float temp = 0,result = 0;
	 U3_bd[2] = x3 - b1_bd * U3_bd[1] - b2_bd * U3_bd[0];
	 result = a0_bd * U3_bd[2] + a1_bd * U3_bd[1] + a2_bd * U3_bd[0];
	 temp = U3_bd[1];
	 U3_bd[1] = U3_bd[2];
	 U3_bd[0] = temp;
	 return result;
}

void contactordeal(void){
	if(chargcount>40000||runorcount>10000) {
		igbtf=0;
		faultstasec.bit.igbtf=0;
		uovf=0;
		faultstasec.bit.uovf=0;
		socf=0;
		faultstasec.bit.socf=0;
		faultf=0;
		faultstasec.bit.faultf=0;
	}
	else{
	if(faultstasec.all&0x003b) {rsorder=0;BytleValue[79]=0;}
	else{
	if(rsorder&&!contactor1&&!contactor2){
		contactorK1=1;
		contactorK2=0;
		chargflag=1;
		chargcount=60000;
		contactor1=1;
		contactor2=0;
		fan=0;
	}
	}
	if(chargflag){
	if((chargcount<20000)&&(!((faultstasec.all&0x003f)||(faultsta.all&0x000e)))){
		contactorK1=0;
		contactorK2=1;
		contactor1=0;
		contactor2=1;
		fan=1;
		chargflag=0;
		runorcount=20000;
	}
	else if(chargcount==0){
		rsorder=0;
		BytleValue[79]=0;
	}
	}
	}
	if(!rsorder){
		contactorK1=0;
		contactorK2=0;
		chargflag=0;
		contactor1=0;
		contactor2=0;
		fan=0;
	}
}
void tempdeal(void)
{
    if(Tem1<0.7279)
	Temax=25;
	else if((Tem1>=0.7279)&&(Tem1<1.0904))
	Temax=27.5817*(Tem1-0.7279)+25;
	else if((Tem1>=1.0904)&&(Tem1<1.5747))
	Temax=20.6510*(Tem1-1.0904)+35;
    else if((Tem1>=1.5747)&&(Tem1<2.1926))
	Temax=16.1831*(Tem1-1.5747)+45;
	else if((Tem1>=2.1926)&&(Tem1<2.9464))
	Temax=13.2658*(Tem1-2.1926)+55;
	else if((Tem1>=2.9464)&&(Tem1<3.8218))
	Temax=11.4238*(Tem1-2.9464)+65;
	else if(Tem1>=3.8218)
	Temax=80;	
}













