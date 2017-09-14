/* 
   * ±êÌâ£ºÐýÒí·À³ý±ù¿ØÖÆÆ÷Èí¼þ
   * ÎÄ¼þÃû£ºXZ_73.pit
   * Ê±¼ä£º2013.10.17
   * °æ±¾£ºµÚV2.00°æ
   * °æÈ¨£º181³§ 
   * ×÷Õß£ºÖ£»ª  
*/

#include "DSP28_Device.h"

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart; 

#pragma CODE_SECTION(InitFlash, "ramfuncs");

////////////////±äÁ¿ÉêÃ÷/////////////////////////////////     
Uchar Count_sample;            //²É¼¯Êý¾Ý¸öÊý
Uchar Sample_Result;           //²ÉÑùÖµ 
Uchar Count_MBIT_Receive;      //Á¬Ðø½ÓÊÕÎ¬»¤BITÃüÁî´ÎÊý
Uchar Count_WDI;               //Î¹¹·Ê±¼ä±äÁ¿
Uchar Count_SCI_Send;          //SCI·¢ËÍÊ±¼ä±äÁ¿
Uint16 Count_MRB_Heat;         //Ö÷½°¼ÓÈÈÊ±¼ä±äÁ¿
Uchar Count_TRB_Heat;          //Î²½°¼ÓÈÈÊ±¼ä±äÁ¿
Uchar Time_Test;               //¶ÏÂ·²éÑ¯Ê±¼ä¼ä¸ô   
Uchar Count_Pulse;             //¼ÓÈÈÇø¸öÊý
Uchar Count_MRB_Test;		   //Ö÷½°¼à²âÊ±¼ä±äÁ¿
Uchar Count_TRB_Test;		   //Î²½°¼à²âÊ±¼ä±äÁ¿
Uchar Count_Light;             //½á¹ûµÆÉÁË¸±äÁ¿
Uchar Count_E422_Check;        //²éÑ¯422Êý¾ÝÊ±¼ä±äÁ¿
Uchar Count_E422_Error;        //Á¬ÐøÎ´ÊÕµ½422Êý¾Ý´ÎÊý
Uint16 TRBSensor_Temperature;  //Î²½°ÎÂ¶È´«¸ÐÆ÷ÎÂ¶È
Uint16 Current_MRB[3];         //Ö÷½°¼ÓÈÈÈýÏàµçÁ÷
Uint16 Current_TRB[3];         //Î²½°¼ÓÈÈÈýÏàµçÁ÷
Uchar Count_Open_MRB[3];       //Ö÷½°¼ÓÈÈÈýÏà¶ÏÂ·´ÎÊý 
Uchar Count_Open_TRB[3];       //Î²½°¼ÓÈÈÈýÏà¶ÏÂ·´ÎÊý 
Uint16 ONT_Real;               //Ö÷½°Ã¿¸öÇøÊµ¼Ê¼ÓÈÈÊ±¼ä
Uint16 ONT_Vary;               //Ö÷½°Ã¿¸öÇø¼ÆËã¼ÓÈÈÊ±¼ä 
Uint16 OFT_Real;               //Ö÷½°Ã¿¸öÇøÊµ¼ÊÍ£Ö¹Ê±¼ä
Uint16 OFT_Vary;               //Ö÷½°Ã¿¸öÇø¼ÆËãÍ£Ö¹Ê±¼ä
Uchar LWC;                     //½ÓÊÕµÄÒºÌ¬Ë®º¬Á¿Êý¾Ý
Uchar Ice_Degree;              //½á±ùÇ¿¶ÈÊý¾Ý
Uchar Number1_Error;           //¹ÊÕÏ×Ö1
Uchar Number2_Error;           //¹ÊÕÏ×Ö2
Uchar SJB[9];                  //½ÓÊÕµ½µÄSJB5µÄÊý¾Ý
Uchar m;                       //Ã¿´Î½Óµ½Êý¾Ý¸öÊý
Uchar Flag_PowerOnBIT_Finish;  //ÉÏµç×Ô¼ìÍê³É±êÖ¾1Íê³É  
Uchar Flag_MBIT_Start;         //Î¬»¤BIT¿ªÊ¼±êÖ¾=1            
Uchar Flag_MBIT_Finish;        //Î¬»¤BIT×Ô¼ìÍê³É±êÖ¾
Uchar Flag_MBIT_MRB;          //Î¬»¤BITÖÐÖ÷½°¼ÓÈÈ±êÖ¾
Uchar Flag_MBIT_TRB;          //Î¬»¤BITÖÐÎ²½°¼ÓÈÈ±êÖ¾
Uchar Flag_Switch_Check;      //½Ó´¥Æ÷¼ì²éÍê³É±êÖ¾1Íê³É 
Uchar Flag_Controller_OK;      //¿ØÖÆÆ÷Õý³£±êÖ¾
Uchar Flag_MRB_OK;             //Ö÷½°Õý³£±êÖ¾
Uchar Flag_TRB_OK;             //Î²½°Õý³£±êÖ¾
Uchar Flag_TRBSensor1_OK;     //Î²½°ÎÂ¶È´«¸ÐÆ÷1Õý³£
Uchar Flag_TRBSensor2_OK;     //Î²½°ÎÂ¶È´«¸ÐÆ÷2Õý³£
Uchar Flag_MRB_Heat;          //Ö÷½°ÕýÔÚ¼ÓÈÈ±êÖ¾
Uchar Flag_E422_Receive;      //½ÓÊÕµ½422Êý¾Ý±êÖ¾
Uchar Flag_MRB_Check;         //µØÃæÎ¬»¤BITÖÐÖ÷½°¼ì²éÍê±êÖ¾
Uchar Flag_TRB_Check;         //µØÃæÎ¬»¤BITÖÐÎ²½°¼ì²éÍê±êÖ¾
Uchar Flag_Heat_Start;        //Ö÷½°¿ªÊ¼¼ÓÈÈ±êÖ¾

Uchar gs=0;
Uchar fk[7]={0,0,0,0,0,0,0};    //·´À¡Ê±Ðò±àÂë
Uchar ZZ[7]={1,1,1,0,0,1,0};    //·´À¡Ê±ÐòÕæÖµ±í
Uchar Flag_FK_Test=0;           //·´À¡Ê±Ðò¼ì²é±êÖ¾
Uchar fk_error=0;               //·´À¡Ê±Ðò¹ÊÕÏ´ÎÊý
Uchar flag=0;
Uchar Flag_Air=0;              //·É»úÔÚµØÃæ
Uchar Flag_ICE=0;              //½á±ù=1 
Uchar Flag_Pattern=0;        //³ý±ù·½Ê½
Uchar Flag_Vary=0;

/////////////////º¯ÊýÉêÃ÷/////////////////////////////////
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


/////////////////Flash³õÊ¼»¯///////////////////////////
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

/////////////////È«¾Ö±äÁ¿³õÊ¼»¯/////////////////////////////////
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

//////////////////////ÏµÍ³³õÊ¼»¯//////////////////////////////////
void InitSysCtrl(void)
{    
  EALLOW;   
  DevEmuRegs.M0RAMDFT = 0x0300;
  DevEmuRegs.M1RAMDFT = 0x0300;
  DevEmuRegs.L0RAMDFT = 0x0300;
  DevEmuRegs.L1RAMDFT = 0x0300;
  DevEmuRegs.H0RAMDFT = 0x0300;
        
  SysCtrlRegs.WDCR= 0x0068;   //½ûÖ¹¿´ÃÅ¹·
  SysCtrlRegs.PLLCR = 0x8;    //Ö÷Æµ=30*8/2=120MHz  
  Delay(5000);                 //ÎªÁËÆµÂÊÎÈ¶¨ÑÓÊ±Ò»¶ÎÊ±¼ä      
  SysCtrlRegs.HISPCP.all = 0x0002;  //¸ßËÙÍâÉèÊ±ÖÓ=120/4=30MHz
  SysCtrlRegs.LOSPCP.all = 0x0002;	 //µÍËÙÍâÉèÊ±ÖÓ=120/4=30MHz
 
  SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;   //Ê¹ÄÜEVAÄ£¿éµÄ¸ßËÙÍâÉèÊ±ÖÓ
  SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;   //Ê¹ÄÜEVBÄ£¿éµÄ¸ßËÙÍâÉèÊ±ÖÓ
  SysCtrlRegs.PCLKCR.bit.ADCENCLK=1;   //Ê¹ÄÜADCÄ£¿éµÄ¸ßËÙÍâÉèÊ±ÖÓ
  SysCtrlRegs.PCLKCR.bit.SCIENCLKA=1;  //Ê¹ÄÜSCIAÄ£¿éµÄ¸ßËÙÍâÉèÊ±ÖÓ
  SysCtrlRegs.PCLKCR.bit.SCIENCLKB=1;  //Ê¹ÄÜSCIBÄ£¿éµÄ¸ßËÙÍâÉèÊ±ÖÓ				
  EDIS;	
}

///////////////////GPIO¿Ú³õÊ¼»¯//////////////////////////////////
void InitGpio(void)
{
  EALLOW;
  GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOAÉèÖÃÎªI/O¿Ú 
  GpioMuxRegs.GPADIR.all=0x0000;    //A0-A8ÉèÖÃÎªÊäÈë	

  GpioMuxRegs.GPBMUX.all=0x0000;   //GPIOBÉèÖÃÎªI/O¿Ú 
  GpioMuxRegs.GPBDIR.all=0xffff;   //B0-B15ÉèÖÃÎªÊä³ö	
  GpioDataRegs.GPBDAT.all=0x1c4; 
  
  GpioMuxRegs.GPFMUX.all=0x0030;   //GPIOF0-F3ÉèÖÃÎªI/O¿Ú
  GpioMuxRegs.GPFDIR.all=0xf;      //F0-F3ÉèÖÃÎªÊä³ö 
  GpioDataRegs.GPFDAT.all=0x0;       
  EDIS;
}	

///////////////////Íâ²¿ÖÐ¶Ï1³õÊ¼»¯//////////////////////////////////
void InitXINT1(void)
{  
  GpioMuxRegs.GPEMUX.bit.XINT1_XBIO_GPIOE0=1; //ÉèÖÃÖÐ¶Ï1¹¦ÄÜ 
  XIntruptRegs.XINT1CR.all=0x5;      //ÉÏÉýÑØÉú³ÉÖÐ¶Ï
}

///////////////////Íâ²¿ÖÐ¶Ï2³õÊ¼»¯//////////////////////////////////
void InitXINT2(void)
{  
  GpioMuxRegs.GPEMUX.bit.XINT2_ADCSOC_GPIOE1=1; //ÉèÖÃÍâ²¿ÖÐ¶Ï2¹¦ÄÜ 
  XIntruptRegs.XINT2CR.all=0x5;      //ÉÏÉýÑØÉú³ÉÖÐ¶Ï
}

///////////////////¶¨Ê±Æ÷1³õÊ¼»¯£¨25ms)/////////////////////////////////
void Init_eva_timer1(void)
{  
  EvaRegs.GPTCONA.all = 0x0;   //³õÊ¼»¯¶¨Ê±Æ÷1²¢ÉèÖÃ¶¨Ê±Æ÷1¼Ä´æÆ÷
  EvaRegs.EVAIMRA.bit.T1PINT = 1;  //ÔÊÐí¶¨Ê±Æ÷1ÖÜÆÚÖÐ¶Ï
  EvaRegs.EVAIFRA.bit.T1PINT = 1;  //Çå³ý¶¨Ê±Æ÷1ÖÜÆÚÖÐ¶Ï±êÖ¾
             
  EvaRegs.T1CON.all = 0x170C;  //Timer1ÎªÁ¬ÐøÔö¼ÆÊýÄ£Ê½£¬Ô¤·ÖÆµÖµ128
	                             //Ê¹ÓÃÄÚ²¿¸ßËÙÍâÉèÊ±ÖÓ
  EvaRegs.T1PR = 0x16e3;   //Timer1µÄÆÚ¼Ä´æÆ÷=0.025*30MHz/128
  EvaRegs.T1CNT = 0x0000;  //Timer1µÄýÇåÁã
}

///////////////////¶¨Ê±Æ÷4³õÊ¼»¯£¨25ms£©//////////////////////////////////
void Init_evb_timer4(void)
{ 
  EvbRegs.GPTCONB.all=0x0;     //³õÊ¼»¯¶¨Ê±Æ÷4²¢ÉèÖÃ¶¨Ê±Æ÷4¼Ä´æÆ÷
  EvbRegs.EVBIMRB.bit.T4PINT = 1;  //ÔÊÐí¶¨Ê±4ÖÜÆÚÖÐ¶Ï
  EvbRegs.EVBIFRB.bit.T4PINT = 1;  //Çå³ý¶¨Ê±Æ÷4ÖÜÆÚêÖ¾
             
  EvbRegs.T4CON.all = 0x170C;  //Timer4ÎªÁ¬ÐøÔö¼ÆÊýÄ£Ê½£¬Ô¤·ÖÆµÖµ128£¬
	                           //Ê¹ÓÃÄÚ²¿¸ßËÙÍâÉèÊ±ÖÓ
  EvbRegs.T4PR = 0x16e3;     //Timer4µÄÖÜÆÚ¼Ä´æÆ÷=0.025*30MHz/128
  EvbRegs.T4CNT = 0x0000;  //Timer4µÄ¼ÆÊýÆ÷ÇåÁã
}

/////////////////´®¿Ú³õÊ¼»¯//////////////////////////////////////
void InitSci(void)
{	
  EALLOW;
  GpioMuxRegs.GPFMUX.all = 0x0030;     //ÉèÖÃSCIA¹¦ÄÜ
  GpioMuxRegs.GPGMUX.all = 0x0030;     //ÉèÖÃSCIB¹¦ÄÜ
  EDIS;

  //////////////SCIA³õÊ¼»¯//////////////////////////////
  SciaRegs.SCIFFTX.all=0xe040;    //ÔÊÐí½ÓÊÕ,Ê¹ÄÜFIFO
  SciaRegs.SCIFFRX.all=0x2021;    //Ê¹ÄÜFIFO½ÓÊÕ
  SciaRegs.SCIFFCT.all=0x0000;    //½ûÖ¹²¨ÌØÂÊÐ£Ñé
  SciaRegs.SCICCR.all = 0x27;     //1¸öÍ£Ö¹Î»,ÆæÐ£Ñé,ÓÐÐ§Êý¾Ý³¤¶È8Î»	
  SciaRegs.SCICTL1.all = 0x03;    //¸´Î»
  SciaRegs.SCICTL2.all = 0x03;    //¸´Î»  	
  SciaRegs.SCIHBAUD = 0x01;     //ÉèÖÃ²¨ÌØÂÊ9600bps
  SciaRegs.SCILBAUD = 0x85;	
  SciaRegs.SCICTL1.all = 0x23;   //Ê¹ÄÜ·¢ËÍ½ÓÊÕ¹¦ÄÜ
  
  //////////////SCIB³õÊ¼»¯//////////////////////////////
  ScibRegs.SCIFFTX.all=0xe040;    //ÔÊÐí½ÓÊÕ,Ê¹ÄÜFIFO
  ScibRegs.SCIFFRX.all=0x2021;    //Ê¹ÄÜFIFO½ÓÊÕ
  ScibRegs.SCIFFCT.all=0x0000;    //½ûÖ¹²¨ÌØÂÊÐ£Ñé
  ScibRegs.SCICCR.all = 0x07;     //1¸öÍ£Ö¹Î»,ÆæÐ£Ñé,ÓÐÐ§Êý¾Ý³¤¶È8Î»	
  ScibRegs.SCICTL1.all = 0x03;    //¸´Î»
  ScibRegs.SCICTL2.all = 0x03;    //¸´Î»  	
  ScibRegs.SCIHBAUD = 0x01;     //ÉèÖÃ²¨ÌØÂÊ9600bps
  ScibRegs.SCILBAUD = 0x85;	
  ScibRegs.SCICTL1.all = 0x23;   //Ê¹ÄÜ·¢ËÍ½ÓÊÕ¹¦ÄÜ  	
} 

///////////////////AD³õÊ¼»¯//////////////////////////////////
void InitAdc(void)
{	
  AdcRegs.ADCTRL1.bit.RESET=1;  //¸´Î»ADCÄ£¿é
  Delay(5000);	 
  AdcRegs.ADCTRL1.bit.RESET=0;
  AdcRegs.ADCTRL1.bit.SUSMOD=3;
  AdcRegs.ADCTRL1.bit.ACQ_PS=0;
  AdcRegs.ADCTRL1.bit.CPS=0;     
  AdcRegs.ADCTRL1.bit.CONT_RUN=0; //ADÎªÆô¶¯/Í£Ö¹Ä£Ê½
  AdcRegs.ADCTRL1.bit.SEQ_CASC=1; //16¼¶ÁªÄ£Ê½

  AdcRegs.ADCTRL3.bit.ADCEXTREF = 0;    //Ê¹ÓÃÄÚ²¿²Î¿¼µçÑ¹     	
  AdcRegs.ADCTRL3.bit.ADCBGRFDN=3; //´ø¼äÏ¶²Î¿¼µçÂ·ÉÏµç	
  Delay(5000); 	                  //µÈ´ýÎÈ¶¨          
  AdcRegs.ADCTRL3.bit.ADCPWDN=1;  //ÆäËüµçÂ·ÉÏµç
  Delay(5000);                    //µÈ´ýÎÈ¶¨
  AdcRegs.ADCTRL3.bit.ADCCLKPS=15;  
  AdcRegs.ADCTRL3.bit.SMODE_SEL=0;  //Ë³Ðò²ÉÑù
	
  AdcRegs.MAX_CONV.bit.MAX_CONV=15;  //²ÉÑùÍ¨µÀÎª16   
  AdcRegs.ADCTRL2.all=0x8c04;
  				
  AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;
  AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;				
} 

/////////////ÑÓÊ±º¯Êý/////////////////////////////////
void Delay(Uint32 xms)
{
  Uint32 i=0;
  for(i=0;i<xms;i++)
  {
    asm("NOP");
  } 
}

//////////////ÐÅºÅ²É¼¯///////////////////////////////
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

////////////CPU¶ÁÐ´¼ì²é////////////////////////
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
      Flag_Controller_OK=1;   //¿ØÖÆÆ÷¹ÊÕÏ
      Number1_Error=Number1_Error|0x40;   //ÉÏ±¨¿ØÖÆÆ÷Ê§Ð§    
	  break;
    }
    else
    {
	  Flag_Controller_OK=0;   //¿ØÖÆÆ÷Õý³£
      Number1_Error=Number1_Error&0xbf;   //ÉÏ±¨¿ØÖÆÆ÷Õý³£
	  i++;
	  if(i>19)
	  {
	    break;
	  }
    }
  }
}

////////////CPU¶¨Ê±Æ÷¼ì²é////////////////////////
void CPU_Timer_Check(void)
{
  EvaRegs.T1CON.bit.TENABLE=1;   //´ò¿ª¶¨Ê±Æ÷1
  EvbRegs.T4CON.bit.TENABLE=1;   //´ò¿ª¶¨Ê±Æ÷4
  Delay(0x9fff);
  EvaRegs.T1CON.bit.TENABLE=0;    //¹Ø±Õ¶¨Ê±Æ÷1
  EvbRegs.T4CON.bit.TENABLE=0;    //¹Ø±Õ¶¨Ê±Æ÷4
  
  if(abs(EvaRegs.T1CNT-EvbRegs.T4CNT)>=5)
  {
    Flag_Controller_OK=1;    //¿ØÖÆÆ÷¹ÊÕÏ
    Number1_Error=Number1_Error|0x40;   //ÉÏ±¨¿ØÖÆÆ÷Ê§Ð§
  }
  else
  {
    Flag_Controller_OK=0;   //ÏµÍ³Õý³£
    Number1_Error=Number1_Error&0xbf;   //ÉÏ±¨¿ØÖÆÆ÷Õý³£
	EvaRegs.T1CNT=0x0000;
	EvbRegs.T4CNT=0x0000;
  }

  /////////////////////////////////////////////////////
  if(Flag_Controller_OK==0)   //¿ØÖÆÆ÷Õý³£
  {
    EvaRegs.T1CON.bit.TENABLE=1;   //´ò¿ª¶¨Ê±Æ÷1
    /////////½«¶¨Ê±Æ÷4¹¦ÄÜ¸ÄÎªÆô¶¯AD×ª»»¹¦ÄÜ///////////// 
    PieCtrl.PIEIER5.bit.INTx1=0;   //¹Ø±Õ¶¨Ê±Æ÷4ÖÐ¶Ï
    EvbRegs.GPTCONB.all=0x400;    //¶¨Ê±Æ÷4ÖÜÆÚÖÐ¶ÏÆô¶¯AD×ª»»
    EvbRegs.T4PR = 0xea;          //Timer4µÄÖÜÆÚ¼Ä´æÆ÷=0.001*30MHz/128 
  }    
}

///////// ´óÆøÎÂ¶È´«¸ÐÆ÷¼ì²é///////////////////////

void AirSensor_Check(void)
{      
  ADC_sample(0x0000);   //ADCINA0²É¼¯´óÆøÎÂ¶È 
  if((Sample_Result<100)||(Sample_Result>2900))
  { 
    Delay(0xffff);
    ADC_sample(0x0000);   //ADCINA0²É¼¯´óÆøÎÂ¶È
	if((Sample_Result<100)||(Sample_Result>2900))   //Á¬ÐøÅÐ¶Ï2´Î
	{
	  Number1_Error=Number1_Error|0x24;   //ÉÏ±¨´óÆøÎÂ¶È´«¸ÐÆ÷Î²½°·À±ùÊ§Ð§
      GpioDataRegs.GPBDAT.bit.GPIOB10=0;  //ÎÂ¶È²»×èÈû
	  Flag_TRB_OK=1;        //Î²½°¹ÊÕÏ
	  if(Flag_MRB_Heat==0)
	  {
	    Flag_MRB_OK=1;        //Ö÷½°¹ÊÕÏ        
	  }
	  Number1_Error=Number1_Error|0x2;  //ÉÏ±¨Ö÷½°³ý±ùÊ§Ð§
	  GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //µãÁÁÖ÷½°³ý±ùÊ§Ð§µÆ 
      GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //µãÁÁÎ²½°³ý±ùÊ§Ð§µÆ		
	}
  }
  else
  {
	if(Sample_Result<=1271)       //´óÆøÎÂ¶È>=20¶È
	{
	  GpioDataRegs.GPBDAT.bit.GPIOB10=1;  //Êä³öÎÂ¶È×èÈû
	}
	else
	{ 
	  GpioDataRegs.GPBDAT.bit.GPIOB10=0;  //ÎÂ¶È²»×èÈû
	}
  }
} 

//////////¼ì²éÎ²½°ÎÂ¶È´«¸ÐÆ÷¼ì²é//////////////////////
void TRBSensor_Check(void)
{   
  Uchar i=0; 

  if(Flag_TRBSensor1_OK==0)    //Î²½°ÎÂ¶È´«¸ÐÆ÷1Õý³£
  {
	i=0;			   //ADCINA1²É¼¯Î²½°ÎÂ¶È´«¸ÐÆ÷1
  }
  else
  {
	i=1;            //ADCINA2²É¼¯Î²½°ÎÂ¶È´«¸ÐÆ÷2
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
	    Flag_TRBSensor1_OK=1;     //Î²½°ÎÂ¶È´«¸ÐÆ÷1¹ÊÕÏ
	  }
	  else if(i==1)
	  {
	    Flag_TRBSensor2_OK=1;     //Î²½°ÎÂ¶È´«¸ÐÆ÷2¹ÊÕÏ
	  }
      Number2_Error=Number2_Error|0x2;  //Î²½°ÎÂ¶È´«¸ÐÆ÷¹ÊÕÏ
    }       
  }
  if((Flag_TRBSensor1_OK==1)&&(Flag_TRBSensor2_OK==1))
  { 
    Flag_TRB_OK=1;                    //Î²½°¹ÊÕÏ
    Number2_Error=Number2_Error|0x1;  //Î²½°ÎÂ¶È´«¸ÐÆ÷Ê§Ð§ 
    Number1_Error=Number1_Error|0x4;  //Î²½°·À±ùÊ§Ð§ 
    GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //¹Ø±ÕÎ²½°¼ÓÈÈ
    GpioDataRegs.GPBDAT.bit.GPIOB0=0;   //¹Ø±ÕÎ²½°¿ØÖÆ¿ª¹Ø
    GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //µãÁÁÎ²½°³ý±ùÊ§Æ       
  }    
}

/////////////Ö÷½°¸ÐÓ¦¼ì²é/////////////////////////////////////////
void MRB_QXJC(void)
{   
  Uchar i=0;

  for(i=0;i<3;i++) 	
  { 
	ADC_sample(0x3333+i*0x1111); 
    Current_MRB[i]=Sample_Result;  	          
    if((Current_MRB[i]<2450)||(Current_MRB[i]>3000))	//Ö÷½°È±ÏàµçÁ÷
    { 
      Delay(0xffff);
      ADC_sample(0x3333+i*0x1111); 
      Current_MRB[i]=Sample_Result;        
      if((Current_MRB[i]<2450)||(Current_MRB[i]>3000))
      { 
        Flag_MRB_OK=1;   //Ö÷½°³ý±ù¹ÊÕÏ
      }
    } 	
  }      
  if((abs(Current_MRB[0]-Current_MRB[1])>350)||(abs(Current_MRB[0]-Current_MRB[2])>350)
      ||(abs(Current_MRB[1]-Current_MRB[2])>350))
  { 
    Flag_MRB_OK=1;     //Ö÷½°³ý±ù¹ÊÕÏ
  }
  //////////////////////////////////////////////	
  if(Flag_MRB_OK==1)  //Ö÷½°³ý±ù¹ÊÕÏ
  {
	Number1_Error=Number1_Error|0x10;   //ÉÏ±¨Ö÷½°³ý±ù¹ÊÕÏ
    GpioDataRegs.GPBDAT.bit.GPIOB11=0;  //¹Ø±ÕÖ÷½°¼ÓÈÈ
    GpioDataRegs.GPBDAT.bit.GPIOB2=1;   //¹Ø±ÕÖ÷½°¿ØÖÆ¿ª¹Ø
	if(Flag_MBIT_Start==0)
    {
      GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //µãÁÁÖ÷½°³ý±ùÊ§Ð§µÆ
	}
  }
} 

/////////////Î²½°¸ÐÓ¦¼ì²é/////////////////////////////////////////
void TRB_QXJC(void)
{   
  Uchar i=0;
  
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
        Flag_TRB_OK=1;                    //Î²½°¹ÊÕÏ
      }
    }
  }      
  if((abs(Current_TRB[0]-Current_TRB[1])>360)||(abs(Current_TRB[0]-Current_TRB[2])>360)
      ||(abs(Current_TRB[1]-Current_TRB[2])>360))
  { 
    Flag_TRB_OK=1;                    //Î²½°¹ÊÕÏ
  }
  //////////////////////////////////////////
  if(Flag_TRB_OK==1)  //Î²½°·À±ùÊ§Ð§
  {
	Number1_Error=Number1_Error|0x4;   //ÉÏ±¨Î²½°·À±ùÊ§Ð§
    GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //¹Ø±ÕÎ²½°¼ÓÈÈ
    GpioDataRegs.GPBDAT.bit.GPIOB0=0;   //¹Ø±ÕÎ²½°¿ØÖÆ¿ª¹Ø
	if(Flag_MBIT_Start==0)
    {
      GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //µãÁÁÎ²½°³ý±ùÊ§Ð§µÆ
	}
  } 
} 

/////////////ÉÏµçBIT//////////////////////////////////
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
  ////////////////////////////////////////////////
  if(Flag_Controller_OK==0)     //ÉÏµçBITÕý³£
  {
    GpioDataRegs.GPBDAT.bit.GPIOB9=1;  //µãÁÁ¿ØÖÆÆ÷Ö¸Ê¾µÆ
	Flag_PowerOnBIT_Finish=1;   //ÉÏµçBITÍê³É 
  }        
}  

//////////½Ó´¥Æ÷¼ì²é////////////////////////////
void Switch_Check(void)
{    
  Count_MRB_Heat=0;
  Count_TRB_Heat=0;
  Flag_MRB_Heat=0;
  Flag_Heat_Start=0;
  Count_MRB_Test=0;
  Count_TRB_Test=0;
  Count_Pulse=0;
  /////////////////¼ì²éÖ÷½°½Ó´¥Æ÷¶Ï¿ª×´Ì¬ÊÇ·ñÕý³£//////////////////
  Delay(0x3ffff);
  if(GpioDataRegs.GPADAT.bit.GPIOA0==0)	//Ö÷½°½Ó´¥Æ÷×Ô¼ìÐÅºÅ0¹ÊÕÏ
  {
    Delay(0xffff);
    if(GpioDataRegs.GPADAT.bit.GPIOA0==0)
	{
	  Flag_MRB_OK=1;      //Ö÷½°¹ÊÕÏ
	}
  }
  /////////////////¼ì²éÎ²½°½Ó´¥Æ÷¶Ï¿ª×´Ì¬ÊÇ·ñÕý³£//////////////////
  Delay(0x3ffff); 
  if(GpioDataRegs.GPADAT.bit.GPIOA1==0)	//Î²½°½Ó´¥Æ÷×Ô¼ìÐÅºÅ0¹ÊÕÏ
  {
    Delay(0xffff);
    if(GpioDataRegs.GPADAT.bit.GPIOA1==0)
	{
	  Flag_TRB_OK=1;     //Î²½°¹ÊÕÏ
	}
  }
}

/////////½á±ùÌ½²â×ÓÏµÍ³¼ì²é//////////////////////
void Detector_Check(void)
{  
  if(GpioDataRegs.GPADAT.bit.GPIOA6==0)   
  { 
    Delay(0xffff);
	if(GpioDataRegs.GPADAT.bit.GPIOA6==0)
    {
      Number2_Error=Number2_Error|0x4;  //ÉÏ±¨½á±ùÌ½²âÊ§Ð§	  
      if(Flag_MRB_Heat==0)
	  {
	    Flag_MRB_OK=1;      //Ö÷½°¹ÊÕÏ
		PieCtrl.PIEIER9.bit.INTx3=0;     //²»½ÓÊÕLWC
	  } 
    }
  }
}

/////////////µØÃæÎ¬»¤BIT//////////////////////////////////
void MBIT(void)
{     
  if(Flag_MBIT_Finish==0)
  {
    if(Flag_Switch_Check==0)
    {
      Switch_Check();
      Flag_Switch_Check=1;    //½Ó´¥Æ÷¼ì²éÍê³É
    }
    /////////////Ö÷½°½Ó´¥Æ÷¼ì²éÕý³£ºó½øÐÐ¼ÓÈÈ///////////////////  
    if(Flag_MRB_OK==0)
    {
	  if((Flag_MBIT_MRB==0)&&(Flag_MRB_Check==0))
      { 
        ONT_Real=34;     //Î¬»¤BIT¼ÓÈÈÊ±¼äÎª0.85s    
        Time_Test=16;   //0.4sÍê³ÉÒ»´Î¼à¿Ø            
	    Flag_MBIT_MRB=1;   //×¼±¸½øÐÐÎ¬»¤BITÖ÷½°¼ÓÈÈ
      }
	  /////////ÏÈ¼ì²éÖ÷½°¼ÓÈÈÊÇ·ñÕý³£////////////////////
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
    ///////////////ÔÙ¼ì²éÎ²½°¼ÓÈÈý³////////
    if(Flag_TRB_OK==0)
    {
	  if((Flag_MBIT_TRB==0)&&(Flag_MRB_Check==1)&&(Flag_TRB_Check==0))
	  {
        Flag_MBIT_TRB=1;  //×¼±¸½øÐÐÎ¬»¤BITÎ²½°¼ÓÈÈ
	  }            	  
	  if(Flag_MBIT_TRB==1)
	  {
	    TRB_Heat();
		if(Flag_MBIT_TRB==0)
		{
          Flag_TRB_Check=1;
          Flag_MBIT_Finish=1;  //¼ì²âÍê³É×¼±¸ÉÏ±¨½á¹û
		}
	  }
    }
    if((Flag_TRB_OK==1)&&(Flag_MRB_Check==1))
	{
      Flag_MBIT_TRB=0;
      Flag_TRB_Check=1;
      Flag_MBIT_Finish=1;  //¼ì²âÍê³É×¼±¸ÉÏ±¨½áû
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
    GpioDataRegs.GPBDAT.bit.GPIOB3=0;      //ÃðµÆ
    GpioDataRegs.GPBDAT.bit.GPIOB4=0;
    Flag_Switch_Check=0;
    Flag_MBIT_Finish=0;
    Flag_MBIT_Start=0;   //×¼±¸ÏÂÒ»´ÎÎ¬»¤BIT
	Time_Test=80;
	Flag_MRB_Check=0;
    Flag_TRB_Check=0;
  }
}

///////////////Ö÷½°¼ÓÈÈ¿ØÖÆ////////////////////////////////////
void MRB_Heat(void)
{  
  if(GpioDataRegs.GPADAT.bit.GPIOA7==0)  //²éÑ¯ÏµÍ³µçÔ´ÊÇ·ñÆô¶¯
  {    
    if(Flag_Pattern==0x0)
    {
      if(Flag_MBIT_MRB==1)     //Î¬»¤BITÊ±
      {
        if(GpioDataRegs.GPBDAT.bit.GPIOB2==1)   
        {       
          GpioDataRegs.GPBDAT.bit.GPIOB2=0;    //½ÓÍ¨Ö÷½°¿ØÖÆ¿ª¹Ø 	   
	      Delay(0x8fffff);
          /////////Î¬»¤BITÊ±¼ì²éÖ÷½°½Ó´¥Æ÷½ÓÍ¨×´Ì¬ÊÇ·ñÕý³£//////////////
          if(GpioDataRegs.GPADAT.bit.GPIOA0==1)	//Ö÷½°½Ó´¥Æ÷×Ô¼ìÐÅºÅ1¹ÊÕÏ
          {
            Delay(0xffff);
            if(GpioDataRegs.GPADAT.bit.GPIOA0==1)
	        {
	          Flag_MRB_OK=1;      //Ö÷½°¹ÊÕÏ
	          GpioDataRegs.GPBDAT.bit.GPIOB11=0;   //¹Ø±ÕÖ÷½°¼ÓÈÈ
              GpioDataRegs.GPBDAT.bit.GPIOB2=1;    //¶Ï¿ªÖ÷½°µçÔ´
	        }
		  }
        }	  
	  }
      else
      {
	    if(GpioDataRegs.GPBDAT.bit.GPIOB2==1)
	    {
	      if((GpioDataRegs.GPFDAT.all&0xf)==0xf)  
	      {
            Delay(0xffff);
            if((GpioDataRegs.GPFDAT.all&0xf)==0xf)
            {
              Flag_Air=1;
              GpioDataRegs.GPBDAT.bit.GPIOB2=0;    //½ÓÍ¨Ö÷½°¿ØÖÆ¿ª¹Ø 	   
	          Delay(0x8fffff);                       	  
            }
		  }
	      else
		  {
            Flag_Air=0;	  //·É»úÔÚµØÃæ
	  	  } 
	    }	  
	  }
	} 
	else
	{
      if(GpioDataRegs.GPBDAT.bit.GPIOB2==1)
	  {
        GpioDataRegs.GPBDAT.bit.GPIOB2=0;    //½ÓÍ¨Ö÷½°¿ØÖÆ¿ª¹Ø 	   
	    Delay(0x8fffff);
	  } 
	}
	////////////////////////////////////////////////////////////
	if((GpioDataRegs.GPBDAT.bit.GPIOB2==0)&&(Count_MRB_Heat<=0))
	{
	  Count_Pulse=1;
	  Flag_Heat_Start=1;
	}
    /////////////////////¼ÓÈÈÊ±Ðò///////////////////// 
    if(((Count_Pulse-1)*(ONT_Real+6)<Count_MRB_Heat)
       &&(Count_MRB_Heat<=((Count_Pulse-1)*(ONT_Real+6)+6)))
	{ 
	  GpioDataRegs.GPBDAT.bit.GPIOB11=1;
	  Count_MRB_Test=0;
	  Flag_FK_Test=0;
	  flag=0;
	}
    if(((Count_Pulse-1)*(ONT_Real+6)+6<Count_MRB_Heat)
       &&(Count_MRB_Heat<=Count_Pulse*(ONT_Real+6)))
	{ 
	  GpioDataRegs.GPBDAT.bit.GPIOB11=0;
	  if(flag==0)
	  {
	    Count_Pulse++;		
	    if(Count_Pulse>7)
	    { 
	      Count_Pulse=0;
	    }
		flag=1;
	  }	   
    }
    if((7*ONT_Real+42<Count_MRB_Heat)&&(Count_MRB_Heat<=7*ONT_Real+48))
	{ 
	  GpioDataRegs.GPBDAT.bit.GPIOB11=1;
	  Count_MRB_Test=0;
	  Flag_FK_Test=0;
	  switch(Flag_Pattern)
	  {
		case 0:
		OFT_Real=OFT_Vary;    //½«¼ÆËãµÄ¶Ï¿ªÊ±¼ä¸³Öµ 
		break;
		case 1:
        OFT_Real=8960;
		break;
		case 2:
        OFT_Real=4560;
		break;
		case 3:
        OFT_Real=3360;
		break;
		default:break;		  		  
	  }
	}
	if(Flag_MBIT_MRB==1)    //Î¬»¤BITÖ÷½°¼ÓÈÈÊ±
	{
	  if(Count_MRB_Heat>(7*ONT_Real+48))
	  { 
	    GpioDataRegs.GPBDAT.bit.GPIOB11=0;   //¹Ø±ÕÖ÷½°¼ÓÈÈ
        GpioDataRegs.GPBDAT.bit.GPIOB2=1;    //¶Ï¿ªÖ÷½°µçÔ´ 
        Flag_Heat_Start=0; 
	    Count_MRB_Heat=0;
	    Flag_MBIT_MRB=0;           //Î¬»¤BITÖ÷½°¼ÓÈÈ½áÊø
	    Count_MRB_Test=0;
	    Flag_FK_Test=0;      
	  }
	}
	else    //×Ô¶¯³ý±ù
	{	  
	  if((ONT_Real+OFT_Real)<(7*ONT_Real+48))   //¼ÓÈÈÖÜÆÚT<(7*Ton+8*0.15)s
	  {
	    if(((7*ONT_Real+48)<Count_MRB_Heat)&&(Count_MRB_Heat<=(7*ONT_Real+54)))
	    { 
	      GpioDataRegs.GPBDAT.bit.GPIOB11=0;
	    } 
	    if(Count_MRB_Heat>(7*ONT_Real+54))
	    { 
	      Count_MRB_Heat=0;
		  Flag_Heat_Start=0;
	      Flag_MRB_Heat=0;           //Ò»¸ö¼ÓÈÈÖÜÆÚ½áÊø
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
	      Flag_MRB_Heat=0;           //Ò»¸ö¼ÓÈÈÖÜÆÚ½áÊø
		  Count_MRB_Test=0; 
		  Flag_FK_Test=0;
	    }
      }
	}
  }
  else
  { 
    GpioDataRegs.GPBDAT.bit.GPIOB11=0;   //¶Ï¿ª¼ÓÈÈÊ±Ðò
    GpioDataRegs.GPBDAT.bit.GPIOB2=1;    //¶Ï¿ªÖ÷½°µçÔ´
    Flag_Heat_Start=0; 
	Flag_MRB_Heat=0;
    Count_MRB_Heat=0;
    Count_MRB_Test=0;
    Flag_FK_Test=0; 
  }   
 	
  //////////////È±Ïà¼°·´À¡Ê±Ðò¼à¿Ø/////////////////////////////////////////////  	
  if(Flag_Pattern==0x0)
  {
    if(((GpioDataRegs.GPBDAT.bit.GPIOB11==0)&&(Count_MRB_Test>=Time_Test))
       &&(Count_Pulse>0))             //Ã¿1s²éÑ¯Ò»´Î
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
} 

///////////////·´À¡Ê±Ðò¼ì²é//////////////////////////////////
void FK_Check(void)
{
  if(GpioDataRegs.GPADAT.bit.GPIOA4==1)   //¼ÓÈÈÊ±Ðò·´À¡¼ì²é
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
        GpioDataRegs.GPBDAT.bit.GPIOB11=0;   //¶Ï¿ª¼ÓÈÈÊ±Ðò
        GpioDataRegs.GPBDAT.bit.GPIOB2=1;    //¶Ï¿ªÖ÷½°µçÔ´		
        Number1_Error=Number1_Error|0xa;  //ÉÏ±¨Ö÷½°Í¬²½Ê§Ð§,Ö÷½°¹ÊÕÏ
        if(Flag_MBIT_Start==0)
        {
         GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //µãÁÁÖ÷½°³ý±ùÊ§Ð§µÆ
        }
		Flag_FK_Test=1;			  //±¾Çø·´À¡Ê±Ðò¼ì²éÍê³É
      }
	}
  }

  if(fk_error==0)
  {
    Flag_FK_Test=1;          //±¾Çø·´À¡Ê±Ðò¼ì²éÍê³É
  }
}

///////////////Î²½°¼ÓÈÈ¿ØÖÆ////////////////////////////////////
void TRB_Heat(void)
{  
  if(GpioDataRegs.GPADAT.bit.GPIOA7==0)  //²éÑ¯¼ÓÈÈµçÔ´ÊÇ·ñ½ÓÍ¨
  { 
    if(Flag_Pattern==0x0)
    {
      if(Flag_MBIT_TRB==1)     //Î¬»¤BITÊ±¼ì²éÎ²½°½Ó´¥Æ÷½ÓÍ¨×´Ì¬ÊÇ·ñÕý³£
      { 
        GpioDataRegs.GPBDAT.bit.GPIOB0=1;    //½ÓÍ¨Î²½°¿ØÖÆ¿ª¹Ø
        Delay(0x3ffff);
        if(GpioDataRegs.GPADAT.bit.GPIOA1==1)	//Î²½°½Ó´¥Æ÷×Ô¼ìÐÅºÅ1¹ÊÕÏ
        {
          Delay(0xffff);
          if(GpioDataRegs.GPADAT.bit.GPIOA1==1)
	      {
	        Flag_TRB_OK=1;     //Î²½°¹ÊÕÏ
	        Count_TRB_Heat=0;
	        GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //Î²½°Í£Ö¹¼ÓÈÈ	
	        GpioDataRegs.GPBDAT.bit.GPIOB0=0;    //¶Ï¿ªÎ²½°µçÔ´ 
	        Flag_MBIT_TRB=0;          //µØÃæÎ¬»¤Î²½°¼ÓÈÈÍê±Ï
	      }
        } 
        //////////////////////////////////////////////////      
        if((Count_TRB_Heat>0)&&(Count_TRB_Heat<=34))
	    { 	    	    
	      GpioDataRegs.GPBDAT.bit.GPIOB1=1;   //Î²½°¼ÓÈÈ
	    }
	    if(Count_TRB_Heat>34)
	    { 
	      Count_TRB_Heat=0;
	      GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //Î²½°Í£Ö¹¼ÓÈÈ	
	      GpioDataRegs.GPBDAT.bit.GPIOB0=0;    //¶Ï¿ªÎ²½°µçÔ´ 
	      Flag_MBIT_TRB=0;          //µØÃæÎ¬»¤Î²½°¼ÓÈÈÍê±Ï     
	    }
      }
	  else
	  { 
	    if(GpioDataRegs.GPBDAT.bit.GPIOB0==0)
	    {
	      if((GpioDataRegs.GPFDAT.all&0xf)==0xf)
	      {
            Delay(0xffff);
            if((GpioDataRegs.GPFDAT.all&0xf)==0xf)
            {
              Flag_Air=1;
            }	   
		  }
	      else
		  {
            Flag_Air=0;	 //·É»úÔÚµØÃæ
		  } 
	    } 
	    ////////////////////////////////////////////////	  
	    if(Flag_Air==1)
	    {
          if((Flag_TRBSensor1_OK==0)||(Flag_TRBSensor2_OK==0))
		  {
            if(TRBSensor_Temperature<1970)
	        { 
	          GpioDataRegs.GPBDAT.bit.GPIOB1=1;   //Î²½°¼ÓÈÈ
	          GpioDataRegs.GPBDAT.bit.GPIOB0=1;    //½ÓÍ¨Î²½°µçÔ´	      
		    }
            if(TRBSensor_Temperature>2284)
		    { 
		      GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //Î²½°Í£Ö¹¼ÓÈÈ		  	 
		    }
		  }
          else
		  {
	        GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //Î²½°Í£Ö¹¼ÓÈÈ
		    GpioDataRegs.GPBDAT.bit.GPIOB0=0;    //¶Ï¿ªÎ²½°µçÔ´
		  }
	    }	 	  
      }
      //////////////È±Ïà¼à¿Ø/////////////////////////////////////////////  	
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
      if((Flag_TRBSensor1_OK==0)||(Flag_TRBSensor2_OK==0))
	  {
        if(TRBSensor_Temperature<1970)
	    { 
	      GpioDataRegs.GPBDAT.bit.GPIOB1=1;   //Î²½°¼ÓÈÈ
	      GpioDataRegs.GPBDAT.bit.GPIOB0=1;    //½ÓÍ¨Î²½°µçÔ´	      
		}
        if(TRBSensor_Temperature>2284)
		{ 
		  GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //Î²½°Í£Ö¹¼ÓÈÈ		  	 
		}
	  }
      else
	  {
	    GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //Î²½°Í£Ö¹¼ÓÈÈ
		GpioDataRegs.GPBDAT.bit.GPIOB0=0;    //¶Ï¿ªÎ²½°µçÔ´
	  } 
	}
  }
  else
  {
    Count_TRB_Heat=0;
	Count_TRB_Test=0;
	GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //Î²½°Í£Ö¹¼ÓÈÈ
    GpioDataRegs.GPBDAT.bit.GPIOB0=0;    //¶Ï¿ªÎ²½°µçÔ´
  }
}

/////////////////¼ÆËã¼ÓÈÈÊ±¼ä////////////////////////////////////////
void Heat_Time_Count(void)
{           
  float Air_Temperature1=0.0;     //´óÆø¾²ÎÂ
  int Air_Temperature2=0;
  float Ontime1=0.0;
  float Ontime2=0.0;
 
  ADC_sample(0x0000); 	  //ADCINA0²É¼¯´óÆøÎÂ¶È
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
    GpioDataRegs.GPBDAT.bit.GPIOB10=1;  //Êä³öÎÂ¶È×èÈû
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
 
///////////////////¶¨Ê±Æ÷1ÖÐ¶Ï//////////////////////////////////

interrupt void eva_timer1_isr(void)
{  
  DINT;
  if(PieCtrl.PIEACK.bit.ACK2==1)
  { 
    PieCtrl.PIEACK.bit.ACK2=1; 
    Count_WDI++;   
    Count_SCI_Send++;      //Î¹¹·¼°´®¿Ú·¢ËÍÊ±ä±äÁ¿
	Count_E422_Check++;
	/////////////////////////////////////////////////
	if(Count_WDI>=20)
	{
	  GpioDataRegs.GPBDAT.bit.GPIOB12=~GpioDataRegs.GPBDAT.bit.GPIOB12;  //Î¹¹·         
	}
	////////////////²éÑ¯³ý±ù·½Ê½/////////////////////
    if((GpioDataRegs.GPADAT.bit.GPIOA9==0)&&(GpioDataRegs.GPADAT.bit.GPIOA8==0))
    {      
      Flag_Vary=0x0;       //×Ô¶¯³ý±ù
	}
    else if((GpioDataRegs.GPADAT.bit.GPIOA9==0)&&(GpioDataRegs.GPADAT.bit.GPIOA8==1))
	{
      Flag_Vary=0x1;       //ÊÖ¶¯Î¢³ý±ù
	}
    else if((GpioDataRegs.GPADAT.bit.GPIOA9==1)&&(GpioDataRegs.GPADAT.bit.GPIOA8==0))
	{
      Flag_Vary=0x2;       //ÊÖ¶¯Çá³ý±ù
	} 
    else if((GpioDataRegs.GPADAT.bit.GPIOA9==1)&&(GpioDataRegs.GPADAT.bit.GPIOA8==1))
	{
      Flag_Vary=0x3;       //ÊÖ¶¯ÖÐ³ý±ù
	}
	if(((Flag_Pattern==0x0)&&(Flag_Vary!=0x0))
	    ||((Flag_Pattern!=0x0)&&(Flag_Vary==0x0)))
	{
      GpioDataRegs.GPBDAT.bit.GPIOB11=0;   //¶Ï¿ª¼ÓÈÈÊ±Ðò
      GpioDataRegs.GPBDAT.bit.GPIOB2=0;    //¶Ï¿ªÖ÷½°µçÔ´ 
	  Flag_Heat_Start=0;
      Count_MRB_Heat=0;
      Count_MRB_Test=0;
      Flag_FK_Test=0;
      Count_Pulse=0;
	  GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //Î²½°Í£Ö¹¼ÓÈÈ
      GpioDataRegs.GPBDAT.bit.GPIOB0=0;    //¶Ï¿ªÎ²½°µçÔ´
      Count_TRB_Heat=0;
	  Count_TRB_Test=0;     
	}
	Flag_Pattern=Flag_Vary;		     
    ///////////////////////////////////////////////
    if(Flag_PowerOnBIT_Finish==0)    //ÉÏµçBITÊ±
	{ 
	  if(Flag_Controller_OK==1)
	  {
	    Count_Light++;
	  }
	}
    else
    {              	    
	  if(Flag_Pattern==0)
	  {
	    if((GpioDataRegs.GPBDAT.bit.GPIOB2==1)&&(GpioDataRegs.GPBDAT.bit.GPIOB0==0))   
	    {	      
		  if((GpioDataRegs.GPFDAT.all&0xf)==0xf)
		  {
	        Flag_Air=1;
	      }
		  else
		  {
	        Flag_Air=0;	   //·É»úÔÚµØÃæ
		  }	      
	      ////////////////////////////////////////////////////
	      if((Flag_Air==0)&&(GpioDataRegs.GPADAT.bit.GPIOA3==0)) //½ÓÊÕµ½Î¬»¤×Ô¼ìÃüÁî
	      { 
	        Count_MBIT_Receive++;
	        if(Count_MBIT_Receive>=2)
            {	
		      Count_MBIT_Receive=0; 
	          Flag_MBIT_Start=1;         //×¼±¸Æô¶¯Î¬»¤BIT
	        }
	      }
	      else
	      {
	        Count_MBIT_Receive=0;
	      }
	    }	  
	    /////////////////////////////////////////////////////////////	  
	    if(Flag_MBIT_Start==1)    //µØÃæÎ¬»¤BITÊ±
	    {
	      if((Flag_MBIT_MRB==1)&&(Flag_Heat_Start==1))    //Ö÷½°
	      {
	        Count_MRB_Heat++;	    
	      }
	      if((GpioDataRegs.GPBDAT.bit.GPIOB0==1)&&(Flag_MBIT_TRB==1))  //Î²½°
	      {
	        Count_TRB_Heat++;
	      }
	      if(Flag_MBIT_Finish==1)   //¼ì²âÍê³É
	      {
	        if((Flag_TRB_OK==1)||(Flag_MRB_OK==1))
	        {
	          Count_Light++;
	        }
		  }
	    }        
        else  //Ñ²¼ìÊ±
	    {
          if(Flag_Heat_Start==1)    //Ö÷½°
	      {
	        Count_MRB_Heat++;	    
	      }
	    }
        ////////////////////////////////////////////////////
        if(GpioDataRegs.GPBDAT.bit.GPIOB2==0)    //Ö÷½°
	    { 
	      if((Flag_MRB_Heat==0)&&(Flag_MBIT_MRB==0))   //Ò»¸öÖÜÆÚ½áÊøÊ±
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
	    if(GpioDataRegs.GPBDAT.bit.GPIOB1==1)  //Î²½°
        { 
		  Count_TRB_Test++;
	    } 
	  }
	  else
	  {
        if(Flag_Heat_Start==1)    //Ö÷½°
	    {
	      Count_MRB_Heat++;	    
	    }	     
	  }
    }   
    EvaRegs.EVAIMRA.bit.T1PINT = 1;
    EvaRegs.EVAIFRA.bit.T1PINT = 1;
  }
  EINT;
} 


//////////////½ÓÊÕLWC/////////////////////////////////
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
		    Flag_E422_Receive=1;  //ÊÕµ½Êý¾Ý
            Count_E422_Error=0; 
            LWC=SJB[5];  
            if(LWC>0xc8)
            {
              LWC=0xc8;
            }         
            if(LWC>0)
		    {
		      /////////¼ÆËã¶Ï¿ªÊ±¼ä////////////////////                     		  
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
              /////////¼ÆËã½á±ùÇ¿¶È////////////////////
              if((LWC>=0x01)&&(LWC<=0x14))
		      {
		        Ice_Degree=0x0;    //ÇáÎ¢½á±ù
		      }
              if((LWC>=0x15)&&(LWC<=0x32))
		      {
		        Ice_Degree=0x1;    //Çá¶È½á±ù
		      }
              if((LWC>=0x33)&&(LWC<=0x64))
		      {
		        Ice_Degree=0x2;    //ÖÐ¶È½á±ù
		      }
              if((LWC>=0x65)&&(LWC<=0xc8))
		      {
		        Ice_Degree=0x3;    //ÖØ¶È½á±ù
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


///////////////////ADCÖÐ¶Ï//////////////////////////////////
interrupt void ad_isr(void)
{ 
  Uint16 Result[16];          //±£´æADÊý¾Ý
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
 		
	///////////16¸öÊý¾ÝÓÉÐ¡µ½´óÅÅÁÐ//////////////////////
	for(Count_sample=0;Count_sample<16;Count_sample++)
	{ for(i=0;i<16-Count_sample;i++)                      
	  { if(Result[i]>Result[i+1])
	    { max=Result[i];
		  Result[i]=Result[i+1];
          Result[i+1]=max;
		}
	  }
	}    	
	//////////È¥Í·È¥Î²È¡ÖÐ¼ä14¸öÓÐÐ§Êý¾ÝÇóÆ½¾ù////////////	
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


///////////////////Î²½°¶ÌÂ·ÖÐ¶Ï//////////////////////////////////
interrupt void XINT1_isr(void)
{ 
  DINT;
  if(PieCtrl.PIEACK.bit.ACK1==1)
  { 
    PieCtrl.PIEACK.bit.ACK1=1;  
    if(Flag_PowerOnBIT_Finish==1)   //ÉÏµçBITÍê³É
    {
      GpioMuxRegs.GPEMUX.bit.XINT1_XBIO_GPIOE0=1;
	  GpioMuxRegs.GPEDIR.bit.GPIOE0=0;
	  Delay(0x3ffff);
      if(GpioDataRegs.GPEDAT.bit.GPIOE0==1)
	  { 
	    Flag_TRB_OK=1;          //Î²½°¼ÓÈÈ¹ÊÕÏ
	    Number1_Error=Number1_Error|0x4;   //ÉÏ±¨Î²½°·À±ùÊ§Ð§
        GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //¹Ø±ÕÎ²½°¼ÓÈÈ
        GpioDataRegs.GPBDAT.bit.GPIOB0=0;   //¹Ø±ÕÎ²½°¿ØÖÆ¿ª¹Ø
		if(Flag_Pattern==0x0)
		{
		  if(Flag_MBIT_Start==0)
          {
            GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //µãÁÁÎ²½°³ý±ùÊ§Ð§µÆ
          }
        }
        else
        {
          GpioDataRegs.GPBDAT.bit.GPIOB15=0;
        } 
	  }
	}
  }
  EINT;
}

///////////////////Ö÷½°¶ÌÂ·ÖÐ¶Ï//////////////////////////////////
interrupt void XINT2_isr(void)
{ 
  DINT;
  if(PieCtrl.PIEACK.bit.ACK1==1)
  { 
    PieCtrl.PIEACK.bit.ACK1=1;  
    if(Flag_PowerOnBIT_Finish==1)   //ÉÏµçBITÍê³É
    {
      GpioMuxRegs.GPEMUX.bit.XINT2_ADCSOC_GPIOE1=1;
	  GpioMuxRegs.GPEDIR.bit.GPIOE1=0;
	  Delay(0x3ffff);
	  if(GpioDataRegs.GPEDAT.bit.GPIOE1==1)
	  { 
	    Flag_MRB_OK=1;        //Ö÷½°¼ÓÈÈ¹ÊÕÏ
        GpioDataRegs.GPBDAT.bit.GPIOB11=0;  //¹Ø±ÕÖ÷½°¼ÓÈÈ
        GpioDataRegs.GPBDAT.bit.GPIOB2=1;   //¹Ø±ÕÖ÷½°¿ØÖÆ¿ª¹Ø       
	    Number1_Error=Number1_Error|0x2;   //ÉÏ±¨Ö÷½°³ý±ùÊ§Ð§
		if(Flag_Pattern==0x0)
		{
		  if(Flag_MBIT_Start==0)
		  {
		    GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //µãÁÁÖ÷½°³ý±ùÊ§Ð§µÆ
		  }
		}
		else
		{
          GpioDataRegs.GPBDAT.bit.GPIOB14=0;
		}
	  }
	} 
  }
  EINT;
}


///////////Ö±½Ó·µ»ØÖÐ¶Ï·þÎñ³ÌÐò//////////////
interrupt void nothing()
{ 
  EINT;
  return;
}

/////////////Ö÷³ÌÐò//////////////////////////////////////

main()
{  
  Init_Variable();
  InitSysCtrl();
  //°ÑInitFlash£¨£©º¯Êý°áÒÆµ½ramÖÐÔËÐÐ
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
  
  //¿ªÖÐ¶Ï   
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
	if(Flag_Pattern==0)   //×Ô¶¯³ý±ù
	{
	  if(EvaRegs.T1CON.bit.TENABLE==1)  //¶¨Ê±Æ÷Õý³£
	  {
	    if(Count_SCI_Send>=40)     //1sµ½·¢ËÍ422¼à¿ØÐÅÏ¢
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
          SciaRegs.SCITXBUF=0xff-(0xaa+0x55+0x0c+0x01+LWC+Ice_Degree+Number1_Error+Number2_Error)+0x01;
	    }
        //////////////////////////////////////////////       
        if(Count_E422_Check>=54)  
        {  
          Count_E422_Check=0;
          if(Flag_E422_Receive==1)  //ÊÕµ½LWCÊý¾Ý
	      {
            Flag_E422_Receive=0;
	      }
	      else
	      {
            Count_E422_Error++;
		    if(Count_E422_Error>=5)    //Á¬Ðø5´ÎÎ´ÊÕµ½LWC
		    {          
              Number2_Error=Number2_Error|0x4;   //ÉÏ±¨½á±ùÌ½²âÊ§Ð§ 
		    }
	      }
        }
      }
      else
      {
	    GpioDataRegs.GPBDAT.bit.GPIOB12=~GpioDataRegs.GPBDAT.bit.GPIOB12;  //Î¹¹·         
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
    }     
    ///////////////////////////////////////////////////////////  
    if(Flag_PowerOnBIT_Finish==1)   //ÉÏµçBITÍê³É
    {
      if(Flag_Pattern==0)   //×Ô¶¯³ý±ù
      {
        if(Flag_MBIT_Start==1)   //×¼±¸½øÐÐÎ¬»¤BIT
        { 
	      Number2_Error=Number2_Error|0x10;  //ÕýÔÚ×Ô¼ì
          MBIT();   //µØÃæÎ¬»¤BIT
        }
        else      //Ñ²¼ìÊ±
        {
          Number2_Error=Number2_Error&0xef;   //×Ô¼ìÍê³É
          ////////////////////////////////////
          if((Number2_Error&0x4)==0x0)
          {
            Detector_Check();   //½á±ùÌ½²â¼ì²é
		  }
		  else	   //½á±ùÌ½²âÊ§Ð§ÉÏ±¨Ö÷½°¹ÊÕÏ
		  {
	        if((Flag_MBIT_Start==0)&&(Flag_MRB_Heat==0))
		    {
			  Flag_MRB_OK=1;     //Ö÷½°¹ÊÕÏ
		    }
		  } 
          /////////////////////////////////////////////
          if((Number1_Error&0x20)==0x0)
          {
            AirSensor_Check();  //´óÆøÎÂ¶È´«¸ÐÆ÷¼ì²é
		  }
		   ////////////////////////////////////////////////
		  if(Flag_TRBSensor1_OK==0)
		  {
		    TRBSensor_Check();  //Î²½°ÎÂ¶È´«¸ÐÆ÷¼ì²é
		  }
		  if((Flag_TRBSensor1_OK==1)&&(Flag_TRBSensor2_OK==0))
		  {
		    TRBSensor_Check();  //1¹ÊÕÏÊ±Á¢¼´¼ì²é2
		  } 			
		  /////////////////////////////////////////////////////
          if(GpioDataRegs.GPBDAT.bit.GPIOB10==0)  //ÎÂ¶È²»×èÈû
          {		  
            //////////////////²éÑ¯½á±ù¸æ¾¯//////////////
            if(GpioDataRegs.GPADAT.bit.GPIOA5==0)  //½á±ù
            {
              Delay(0xffff);
              if(GpioDataRegs.GPADAT.bit.GPIOA5==0)			  
              {
                Flag_ICE=1;          
                Number1_Error=Number1_Error|0x1;
              }		  
		    }
		    else
		    {
              Flag_ICE=0;          
              Number1_Error=Number1_Error&0xfe;
		    }
		    ////////////////////////////////////////////		    		    
		    if(Flag_TRB_OK==0)     //Î²½°Õý³£
		    {
		      if(((Number2_Error&0x4)==0x0)&&(Flag_ICE==1))
		      {
		        TRB_Heat();         //Î²½°¼ÓÈÈ
			  }
			  else
			  {
                GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //¹Ø±ÕÎ²½°¼ÓÈÈ
                GpioDataRegs.GPBDAT.bit.GPIOB0=0;  //¹Ø±ÕÎ²½°¿ØÖÆ¿ª¹Ø			  
			  }
		    }
			///////////////////////////////////////////////
            if(((Number2_Error&0x4)==0x0)&&((Number1_Error&0x20)==0x0))
            {
              if(Flag_ICE==1)
              {
                if((Flag_MRB_Heat==0)&&(Flag_MRB_OK==0))
                {
                  Heat_Time_Count();  //¼ÆËã¼ÓÈÈÊ±¼ä
				  ONT_Real=ONT_Vary;
                  Flag_MRB_Heat=1;     //Ö÷½°×¼±¸¼ÓÈÈ
                }                            
			  }
		    }
		    else
		    {
              Number1_Error=Number1_Error&0xfe;
		    }
		  }
          ////////////////////////////////////////////////
          if((Flag_MRB_OK==0)&&(Flag_MRB_Heat==1))
		  {
            MRB_Heat();	  //Ö÷½°ÕýÔÚ¼ÓÈÈ
		  }
		  else
		  {
            GpioDataRegs.GPBDAT.bit.GPIOB11=0;  //¹Ø±ÕÖ÷½°¼ÓÈÈ
            GpioDataRegs.GPBDAT.bit.GPIOB2=1;   //¹Ø±ÕÖ÷½°¿ØÖÆ¿ª¹Ø
		  }
           ////////////////////////////////////////////////////////////
          if((GpioDataRegs.GPBDAT.bit.GPIOB10==1)||(Flag_TRB_OK==1))
		  {
            GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //¹Ø±ÕÎ²½°¼ÓÈÈ
            GpioDataRegs.GPBDAT.bit.GPIOB0=0;  //¹Ø±ÕÎ²½°¿ØÖÆ¿ª¹Ø
		  }
	    } 
	    ////////////////////////////////////////////////////////////
	    if((GpioDataRegs.GPBDAT.bit.GPIOB2==0)||(GpioDataRegs.GPBDAT.bit.GPIOB1==1))
	    {
	      GpioDataRegs.GPBDAT.bit.GPIOB13=1;    //µãÁÁ·À³ý±ùÏµÍ³µÆ
	      Number2_Error=Number2_Error|0x8;
	    } 
	    else
	    {
	      GpioDataRegs.GPBDAT.bit.GPIOB13=0;    //·À³ý±ùÏµÍ³µÆÃðµô
          Number2_Error=Number2_Error&0xf7;
	    }
	  }
	  else
	  {
	    ONT_Real=640;        
        if(GpioDataRegs.GPBDAT.bit.GPIOB2==0)    //Ö÷½°
	    { 
	      if(Flag_MRB_Heat==0)   //Ò»¸öÖÜÆÚ½áÊøÊ±
		  {
		    Count_Pulse=1;
		  }
	    }
	    else
	    {
	      Count_Pulse=0;
		  if(Flag_MRB_Heat==0)
		  {
		    Flag_MRB_Heat=1;
		  }
	    } 
		//////////Ö÷½°¼ÓÈÈ/////////////////////////////
	    if((Flag_MRB_OK==0)&&(Flag_MRB_Heat==1))
		{
          MRB_Heat();	  //Ö÷½°ÕýÔÚ¼ÓÈÈ
		}
		else
		{
          GpioDataRegs.GPBDAT.bit.GPIOB11=0;  //¹Ø±ÕÖ÷½°¼ÓÈÈ
          GpioDataRegs.GPBDAT.bit.GPIOB2=1;   //¹Ø±ÕÖ÷½°¿ØÖÆ¿ª¹Ø
		}
		/////////////Î²½°¼ÓÈÈ//////////////////////////////
		if(Flag_TRBSensor1_OK==0)
		{
		  TRBSensor_Check();  //Î²½°ÎÂ¶È´«¸ÐÆ÷¼ì²é
		}
		if((Flag_TRBSensor1_OK==1)&&(Flag_TRBSensor2_OK==0))
		{
		  TRBSensor_Check();  //1¹ÊÕÏÊ±Á¢¼´¼ì²é2
		} 
		if(Flag_TRB_OK==0)     //Î²½°Õý³£
		{
		  TRB_Heat();         //Î²½°¼ÓÈÈ
	    }
	    else
	    {
          GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //¹Ø±ÕÎ²½°¼ÓÈÈ
          GpioDataRegs.GPBDAT.bit.GPIOB0=0;  //¹Ø±ÕÎ²½°¿ØÖÆ¿ª¹Ø			  
		}
	    ////////////////////////////////////////////////////////////
	    if((GpioDataRegs.GPBDAT.bit.GPIOB2==0)||(GpioDataRegs.GPBDAT.bit.GPIOB1==1))
	    {
	      GpioDataRegs.GPBDAT.bit.GPIOB13=1;    //µãÁÁ·À³ý±ùÏµÍ³µÆ
	      Number2_Error=Number2_Error|0x8;
	    } 
	    else
	    {
	      GpioDataRegs.GPBDAT.bit.GPIOB13=0;    //·À³ý±ùÏµÍ³µÆÃðµô
          Number2_Error=Number2_Error&0xf7;
	    }		 
	  }   
    }  
    else   //ÉÏµçBIT¹ÊÕÏ
    {
	  GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //µãÁÁÖ÷½°³ý±ùÊ§Ð§µÆ 
      GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //µãÁÁÎ²½°³ý±ùÊ§Ð§µÆ      
      if(Count_Light>=5)   //ÉÏµçBIT¹ÊÕÏ¿ØÖÆÆ÷Ö¸Ê¾µÆ4HzÉÁË¸
      {
        Count_Light=0;
        GpioDataRegs.GPBDAT.bit.GPIOB9=~GpioDataRegs.GPBDAT.bit.GPIOB9;
	  }  
    }  
  }          
}  

