/* 
   * ±êÌâ£ºĞıÒí·À³ı±ù¿ØÖÆÆ÷Èí¼ş
   * ÎÄ¼şÃû£ºXZ_73.pit
   * Ê±¼ä£º2013.6.28
   * °æ±¾£ºµÚV1.04°æ
   * °æÈ¨£º181³§ 
   * ×÷Õß£ºÖ£»ª  
*/

#include "DSP28_Device.h"

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart; 

#pragma CODE_SECTION(InitFlash, "ramfuncs");

////////////////±äÁ¿ÉêÃ÷/////////////////////////////////     
Uchar Count_sample;            //²É¼¯Êı¾İ¸öÊı
Uchar Sample_Result;           //²ÉÑùÖµ 
Uchar Count_MBIT_Receive;      //Á¬Ğø½ÓÊÕÎ¬»¤BITÃüÁî´ÎÊı
Uchar Count_WDI;               //Î¹¹·Ê±¼ä±äÁ¿
Uchar Count_SCI_Send;          //SCI·¢ËÍÊ±¼ä±äÁ¿
Uint16 Count_MRB_Heat;         //Ö÷½°¼ÓÈÈÊ±¼ä±äÁ¿
Uchar Count_TRB_Heat;          //Î²½°¼ÓÈÈÊ±¼ä±äÁ¿
Uchar Time_Test;               //¶ÏÂ·²éÑ¯Ê±¼ä¼ä¸ô   
Uchar Count_Pulse;             //¼ÓÈÈÇø¸öÊı
Uchar Count_MRB_Test;		   //Ö÷½°¼à²âÊ±¼ä±äÁ¿
Uchar Count_TRB_Test;		   //Î²½°¼à²âÊ±¼ä±äÁ¿
Uchar Count_Light;             //½á¹ûµÆÉÁË¸±äÁ¿
Uchar Count_E422_Check;        //²éÑ¯422Êı¾İÊ±¼ä±äÁ¿
Uchar Count_E422_Error;        //Á¬ĞøÎ´ÊÕµ½422Êı¾İ´ÎÊı
Uint16 TRBSensor_Temperature;  //Î²½°ÎÂ¶È´«¸ĞÆ÷ÎÂ¶È
Uint16 Current_MRB[3];         //Ö÷½°¼ÓÈÈÈıÏàµçÁ÷
Uint16 Current_TRB[3];         //Î²½°¼ÓÈÈÈıÏàµçÁ÷
Uchar Count_Open_MRB[3];       //Ö÷½°¼ÓÈÈÈıÏà¶ÏÂ·´ÎÊı 
Uchar Count_Open_TRB[3];       //Î²½°¼ÓÈÈÈıÏà¶ÏÂ·´ÎÊı 
Uint16 ONT_Real;               //Ö÷½°Ã¿¸öÇøÊµ¼Ê¼ÓÈÈÊ±¼ä
Uint16 ONT_Vary;               //Ö÷½°Ã¿¸öÇø¼ÆËã¼ÓÈÈÊ±¼ä 
Uint16 OFT_Real;               //Ö÷½°Ã¿¸öÇøÊµ¼ÊÍ£Ö¹Ê±¼ä
Uint16 OFT_Vary;               //Ö÷½°Ã¿¸öÇø¼ÆËãÍ£Ö¹Ê±¼ä
Uchar LWC;                     //½ÓÊÕµÄÒºÌ¬Ë®º¬Á¿Êı¾İ
Uchar Ice_Degree;              //½á±ùÇ¿¶ÈÊı¾İ
Uchar Number1_Error;           //¹ÊÕÏ×Ö1
Uchar Number2_Error;           //¹ÊÕÏ×Ö2
Uchar SJB[9];                  //½ÓÊÕµ½µÄSJB5µÄÊı¾İ
Uchar m;                       //Ã¿´Î½Óµ½Êı¾İ¸öÊı
Uchar Flag_PowerOnBIT_Finish;  //ÉÏµç×Ô¼ìÍê³É±êÖ¾1Íê³É  
Uchar Flag_MBIT_Start;         //Î¬»¤BIT¿ªÊ¼±êÖ¾=1            
Uchar Flag_MBIT_Finish;        //Î¬»¤BIT×Ô¼ìÍê³É±êÖ¾
Uchar Flag_MBIT_MRB;          //Î¬»¤BITÖĞÖ÷½°¼ÓÈÈ±êÖ¾
Uchar Flag_MBIT_TRB;          //Î¬»¤BITÖĞÎ²½°¼ÓÈÈ±êÖ¾
Uchar Flag_Switch_Check;      //½Ó´¥Æ÷¼ì²éÍê³É±êÖ¾1Íê³É 
Uchar Flag_Controller_OK;      //¿ØÖÆÆ÷Õı³£±êÖ¾
Uchar Flag_MRB_OK;             //Ö÷½°Õı³£±êÖ¾
Uchar Flag_TRB_OK;             //Î²½°Õı³£±êÖ¾
Uchar Flag_TRBSensor1_OK;     //Î²½°ÎÂ¶È´«¸ĞÆ÷1Õı³£
Uchar Flag_TRBSensor2_OK;     //Î²½°ÎÂ¶È´«¸ĞÆ÷2Õı³£
Uchar Flag_MRB_Heat;          //Ö÷½°ÕıÔÚ¼ÓÈÈ±êÖ¾
Uchar Flag_E422_Receive;      //½ÓÊÕµ½422Êı¾İ±êÖ¾
Uchar Flag_MRB_Check;         //µØÃæÎ¬»¤BITÖĞÖ÷½°¼ì²éÍê±êÖ¾
Uchar Flag_TRB_Check;         //µØÃæÎ¬»¤BITÖĞÎ²½°¼ì²éÍê±êÖ¾
Uchar Flag_Heat_Start;        //Ö÷½°¿ªÊ¼¼ÓÈÈ±êÖ¾

Uchar gs=0;
Uchar fk[7]={0,0,0,0,0,0,0};    //·´À¡Ê±Ğò±àÂë
Uchar ZZ[7]={1,1,1,0,0,1,0};    //·´À¡Ê±ĞòÕæÖµ±í
Uchar Flag_FK_Test=0;           //·´À¡Ê±Ğò¼ì²é±êÖ¾
Uchar fk_error=0;               //·´À¡Ê±Ğò¹ÊÕÏ´ÎÊı

/////////////////º¯ÊıÉêÃ÷/////////////////////////////////
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
  EDIS;
}	

///////////////////Íâ²¿ÖĞ¶Ï1³õÊ¼»¯//////////////////////////////////
void InitXINT1(void)
{  
  GpioMuxRegs.GPEMUX.bit.XINT1_XBIO_GPIOE0=1; //ÉèÖÃÖĞ¶Ï1¹¦ÄÜ 
  XIntruptRegs.XINT1CR.all=0x5;      //ÉÏÉıÑØÉú³ÉÖĞ¶Ï
}

///////////////////Íâ²¿ÖĞ¶Ï2³õÊ¼»¯//////////////////////////////////
void InitXINT2(void)
{  
  GpioMuxRegs.GPEMUX.bit.XINT2_ADCSOC_GPIOE1=1; //ÉèÖÃÍâ²¿ÖĞ¶Ï2¹¦ÄÜ 
  XIntruptRegs.XINT2CR.all=0x5;      //ÉÏÉıÑØÉú³ÉÖĞ¶Ï
}

///////////////////¶¨Ê±Æ÷1³õÊ¼»¯£¨25ms)/////////////////////////////////
void Init_eva_timer1(void)
{  
  EvaRegs.GPTCONA.all = 0x0;   //³õÊ¼»¯¶¨Ê±Æ÷1²¢ÉèÖÃ¶¨Ê±Æ÷1¼Ä´æÆ÷
  EvaRegs.EVAIMRA.bit.T1PINT = 1;  //ÔÊĞí¶¨Ê±Æ÷1ÖÜÆÚÖĞ¶Ï
  EvaRegs.EVAIFRA.bit.T1PINT = 1;  //Çå³ı¶¨Ê±Æ÷1ÖÜÆÚÖĞ¶Ï±êÖ¾
             
  EvaRegs.T1CON.all = 0x170C;  //Timer1ÎªÁ¬ĞøÔö¼ÆÊıÄ£Ê½£¬Ô¤·ÖÆµÖµ128
	                             //Ê¹ÓÃÄÚ²¿¸ßËÙÍâÉèÊ±ÖÓ
  EvaRegs.T1PR = 0x16e3;   //Timer1µÄÆÚ¼Ä´æÆ÷=0.025*30MHz/128
  EvaRegs.T1CNT = 0x0000;  //Timer1µÄ¼ÆÊıÇåÁã
}

///////////////////¶¨Ê±Æ÷4³õÊ¼»¯£¨25ms£©//////////////////////////////////
void Init_evb_timer4(void)
{ 
  EvbRegs.GPTCONB.all=0x0;     //³õÊ¼»¯¶¨Ê±Æ÷4²¢ÉèÖÃ¶¨Ê±Æ÷4¼Ä´æÆ÷
  EvbRegs.EVBIMRB.bit.T4PINT = 1;  //ÔÊĞí¶¨Ê±4ÖÜÆÚÖĞ¶Ï
  EvbRegs.EVBIFRB.bit.T4PINT = 1;  //Çå³ı¶¨Ê±Æ÷4ÖÜÆÚêÖ¾
             
  EvbRegs.T4CON.all = 0x170C;  //Timer4ÎªÁ¬ĞøÔö¼ÆÊıÄ£Ê½£¬Ô¤·ÖÆµÖµ128£¬
	                           //Ê¹ÓÃÄÚ²¿¸ßËÙÍâÉèÊ±ÖÓ
  EvbRegs.T4PR = 0x16e3;     //Timer4µÄÖÜÆÚ¼Ä´æÆ÷=0.025*30MHz/128
  EvbRegs.T4CNT = 0x0000;  //Timer4µÄ¼ÆÊıÆ÷ÇåÁã
}

/////////////////´®¿Ú³õÊ¼»¯//////////////////////////////////////
void InitSci(void)
{	
  EALLOW;
  GpioMuxRegs.GPFMUX.all = 0x0030;     //ÉèÖÃSCIA¹¦ÄÜ
  GpioMuxRegs.GPGMUX.all = 0x0030;     //ÉèÖÃSCIB¹¦ÄÜ
  EDIS;

  //////////////SCIA³õÊ¼»¯//////////////////////////////
  SciaRegs.SCIFFTX.all=0xe040;    //ÔÊĞí½ÓÊÕ,Ê¹ÄÜFIFO
  SciaRegs.SCIFFRX.all=0x2021;    //Ê¹ÄÜFIFO½ÓÊÕ
  SciaRegs.SCIFFCT.all=0x0000;    //½ûÖ¹²¨ÌØÂÊĞ£Ñé
  SciaRegs.SCICCR.all = 0x07;     //1¸öÍ£Ö¹Î»,ÆæĞ£Ñé,ÓĞĞ§Êı¾İ³¤¶È8Î»	
  SciaRegs.SCICTL1.all = 0x03;    //¸´Î»
  SciaRegs.SCICTL2.all = 0x03;    //¸´Î»  	
  SciaRegs.SCIHBAUD = 0x01;     //ÉèÖÃ²¨ÌØÂÊ9600bps
  SciaRegs.SCILBAUD = 0x85;	
  SciaRegs.SCICTL1.all = 0x23;   //Ê¹ÄÜ·¢ËÍ½ÓÊÕ¹¦ÄÜ
  
  //////////////SCIB³õÊ¼»¯//////////////////////////////
  ScibRegs.SCIFFTX.all=0xe040;    //ÔÊĞí½ÓÊÕ,Ê¹ÄÜFIFO
  ScibRegs.SCIFFRX.all=0x2021;    //Ê¹ÄÜFIFO½ÓÊÕ
  ScibRegs.SCIFFCT.all=0x0000;    //½ûÖ¹²¨ÌØÂÊĞ£Ñé
  ScibRegs.SCICCR.all = 0x07;     //1¸öÍ£Ö¹Î»,ÆæĞ£Ñé,ÓĞĞ§Êı¾İ³¤¶È8Î»	
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
  Delay(5000); 	                  //µÈ´ıÎÈ¶¨          
  AdcRegs.ADCTRL3.bit.ADCPWDN=1;  //ÆäËüµçÂ·ÉÏµç
  Delay(5000);                    //µÈ´ıÎÈ¶¨
  AdcRegs.ADCTRL3.bit.ADCCLKPS=15;  
  AdcRegs.ADCTRL3.bit.SMODE_SEL=0;  //Ë³Ğò²ÉÑù
	
  AdcRegs.MAX_CONV.bit.MAX_CONV=15;  //²ÉÑùÍ¨µÀÎª16   
  AdcRegs.ADCTRL2.all=0x8c04;
  				
  AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;
  AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;				
} 

/////////////ÑÓÊ±º¯Êı/////////////////////////////////
void Delay(Uint32 xms)
{
  Uint32 i=0;
  for(i=0;i<xms;i++)
  {
    asm("NOP");
  } 
}

//////////////ĞÅºÅ²É¼¯///////////////////////////////
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

////////////CPU¶ÁĞ´¼ì²é////////////////////////
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
      Number1_Error=Number1_Error|0x40;   //ÉÏ±¨¿ØÖÆÆ÷Ê§Ğ§    
	  break;
    }
    else
    {
	  Flag_Controller_OK=0;   //¿ØÖÆÆ÷Õı³£
      Number1_Error=Number1_Error&0xbf;   //ÉÏ±¨¿ØÖÆÆ÷Õı³£
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
    Number1_Error=Number1_Error|0x40;   //ÉÏ±¨¿ØÖÆÆ÷Ê§Ğ§
  }
  else
  {
    Flag_Controller_OK=0;   //ÏµÍ³Õı³£
    Number1_Error=Number1_Error&0xbf;   //ÉÏ±¨¿ØÖÆÆ÷Õı³£
	EvaRegs.T1CNT=0x0000;
	EvbRegs.T4CNT=0x0000;
  }

  /////////////////////////////////////////////////////
  if(Flag_Controller_OK==0)   //¿ØÖÆÆ÷Õı³£
  {
    EvaRegs.T1CON.bit.TENABLE=1;   //´ò¿ª¶¨Ê±Æ÷1
    /////////½«¶¨Ê±Æ÷4¹¦ÄÜ¸ÄÎªÆô¶¯AD×ª»»¹¦ÄÜ///////////// 
    PieCtrl.PIEIER5.bit.INTx1=0;   //¹Ø±Õ¶¨Ê±Æ÷4ÖĞ¶Ï
    EvbRegs.GPTCONB.all=0x400;    //¶¨Ê±Æ÷4ÖÜÆÚÖĞ¶ÏÆô¶¯AD×ª»»
    EvbRegs.T4PR = 0xea;          //Timer4µÄÖÜÆÚ¼Ä´æÆ÷=0.001*30MHz/128 
  }    
}

///////// ´óÆøÎÂ¶È´«¸ĞÆ÷¼ì²é///////////////////////

void AirSensor_Check(void)
{      
  ADC_sample(0x0000);   //ADCINA0²É¼¯´óÆøÎÂ¶È 
  if(Flag_PowerOnBIT_Finish==0)    //ÉÏµçBITÊ±	 
  { 
    if((Sample_Result<530)||(Sample_Result>630))
    { 
      Flag_Controller_OK=1;    //¿ØÖÆÆ÷¹ÊÕÏ
	  Number1_Error=Number1_Error|0x40;   //ÉÏ±¨¿ØÖÆÆ÷Ê§Ğ§
    }
	else
	{
	  Flag_Controller_OK=0;    //¿ØÖÆÆ÷Õı³£
	  Number1_Error=Number1_Error&0xbf;   //ÉÏ±¨¿ØÖÆÆ÷Õı³£
	}
  }
  else 
  { 
	if((Sample_Result<100)||(Sample_Result>2900))
    { 
      Delay(0xffff);
      ADC_sample(0x0000);   //ADCINA0²É¼¯´óÆøÎÂ¶È
	  if((Sample_Result<100)||(Sample_Result>2900))   //Á¬ĞøÅĞ¶Ï2´Î
	  {
	    Number1_Error=Number1_Error|0x24;   //ÉÏ±¨´óÆøÎÂ¶È´«¸ĞÆ÷Î²½°·À±ùÊ§Ğ§
        GpioDataRegs.GPBDAT.bit.GPIOB10=0;  //ÎÂ¶È²»×èÈû
	    Flag_TRB_OK=1;        //Î²½°¹ÊÕÏ
	    if(Flag_MRB_Heat==0)
	    {
	      Flag_MRB_OK=1;        //Ö÷½°¹ÊÕÏ        
	    }
	    Number1_Error=Number1_Error|0x2;  //ÉÏ±¨Ö÷½°³ı±ùÊ§Ğ§
	    GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //µãÁÁÖ÷½°³ı±ùÊ§Ğ§µÆ 
        GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //µãÁÁÎ²½°³ı±ùÊ§Ğ§µÆ		
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
} 

//////////¼ì²éÎ²½°ÎÂ¶È´«¸ĞÆ÷¼ì²é//////////////////////
void TRBSensor_Check(void)
{   
  Uchar i=0; 
  
  if(Flag_PowerOnBIT_Finish==0)    //ÉÏµçBITÊ±	    	 
  {
    while(i<2)
    {
	  ADC_sample(0x1111+i*0x1111);        
      if((Sample_Result<1175)||(Sample_Result>1275))
      { 
        Flag_Controller_OK=1;      //¿ØÖÆÆ÷¹ÊÕÏ
	    Number1_Error=Number1_Error|0x40;   //ÉÏ±¨¿ØÖÆÆ÷Ê§Ğ§
		break;
      }
      else
	  { 
	    Flag_Controller_OK=0;      //¿ØÖÆÆ÷Õı³£
	    Number1_Error=Number1_Error&0xbf;   //ÉÏ±¨¿ØÖÆÆ÷Õı³£
		i++;
	  }
    }
  }
  else
  { 
    if(Flag_TRBSensor1_OK==0)    //Î²½°ÎÂ¶È´«¸ĞÆ÷1Õı³£
    {
	  i=0;			   //ADCINA1²É¼¯Î²½°ÎÂ¶È´«¸ĞÆ÷1
	}
    else
	{
	  i=1;            //ADCINA2²É¼¯Î²½°ÎÂ¶È´«¸ĞÆ÷2
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
	      Flag_TRBSensor1_OK=1;     //Î²½°ÎÂ¶È´«¸ĞÆ÷1¹ÊÕÏ
	    }
	    else if(i==1)
	    {
	      Flag_TRBSensor2_OK=1;     //Î²½°ÎÂ¶È´«¸ĞÆ÷2¹ÊÕÏ
	    }
        Number2_Error=Number2_Error|0x2;  //Î²½°ÎÂ¶È´«¸ĞÆ÷¹ÊÕÏ
      }       
    }
    if((Flag_TRBSensor1_OK==1)&&(Flag_TRBSensor2_OK==1))
    { 
      Flag_TRB_OK=1;                    //Î²½°¹ÊÕÏ
      Number2_Error=Number2_Error|0x1;  //Î²½°ÎÂ¶È´«¸ĞÆ÷Ê§Ğ§ 
      Number1_Error=Number1_Error|0x4;  //Î²½°·À±ùÊ§Ğ§ 
      GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //¹Ø±ÕÎ²½°¼ÓÈÈ
      GpioDataRegs.GPBDAT.bit.GPIOB0=0;   //¹Ø±ÕÎ²½°¿ØÖÆ¿ª¹Ø
      GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //µãÁÁÎ²½°³ı±ùÊ§Æ       
	}   
  } 
}

/////////////Ö÷½°¸ĞÓ¦¼ì²é/////////////////////////////////////////
void MRB_QXJC(void)
{   
  Uchar i=0;
  
  if(Flag_PowerOnBIT_Finish==0)    //ÉÏµçBITÊ±
  {
    while(i<3)
    {
      ADC_sample(0x3333+i*0x1111);    
      if((Sample_Result<2450)||(Sample_Result>2550))
	  {
	    Flag_Controller_OK=1;      //¿ØÖÆÆ÷¹ÊÕÏ
	    Number1_Error=Number1_Error|0x40;   //ÉÏ±¨¿ØÖÆÆ÷Ê§
		break;
	  }
	  else
	  {
	    Flag_Controller_OK=0;      //¿ØÖÆÆ÷Õı³£
	    Number1_Error=Number1_Error&0xbf;   //ÉÏ±¨¿ØÖÆÆ÷Õı³£
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
      if((Current_MRB[i]<2450)||(Current_MRB[i]>3000))	//Ö÷½°È±ÏàµçÁ÷
      { 
        Delay(0xffff);
        ADC_sample(0x3333+i*0x1111); 
        Current_MRB[i]=Sample_Result;        
        if((Current_MRB[i]<2450)||(Current_MRB[i]>3000))
        { 
          Flag_MRB_OK=1;   //Ö÷½°³ı±ù¹ÊÕÏ
        }
      } 	
	}      
    if((abs(Current_MRB[0]-Current_MRB[1])>350)||(abs(Current_MRB[0]-Current_MRB[2])>350)
      ||(abs(Current_MRB[1]-Current_MRB[2])>350))
    { 
      Flag_MRB_OK=1;     //Ö÷½°³ı±ù¹ÊÕÏ
    }
	//////////////////////////////////////////////	
	if(Flag_MRB_OK==1)  //Ö÷½°³ı±ù¹ÊÕÏ
	{
	  Number1_Error=Number1_Error|0x10;   //ÉÏ±¨Ö÷½°³ı±ù¹ÊÕÏ
      GpioDataRegs.GPBDAT.bit.GPIOB11=0;  //¹Ø±ÕÖ÷½°¼ÓÈÈ
      GpioDataRegs.GPBDAT.bit.GPIOB2=1;   //¹Ø±ÕÖ÷½°¿ØÖÆ¿ª¹Ø
	  if(Flag_MBIT_Start==0)
      {
        GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //µãÁÁÖ÷½°³ı±ùÊ§Ğ§µÆ
	  }
	}
  }
} 

/////////////Î²½°¸ĞÓ¦¼ì²é/////////////////////////////////////////
void TRB_QXJC(void)
{   
  Uchar i=0;
  
  if(Flag_PowerOnBIT_Finish==0)    //ÉÏµçBITÊ±
  {
    while(i<3)
    {
      ADC_sample(0x6666+i*0x1111);
      if((Sample_Result<1540)||(Sample_Result>1640))
	  {
	    Flag_Controller_OK=1;      //¿ØÖÆÆ÷¹ÊÕÏ
	    Number1_Error=Number1_Error|0x40;   //ÉÏ±¨¿ØÖÆÆ÷Ê§Ğ§
		break;
	  }
	  else
	  {
	    Flag_Controller_OK=0;      //¿ØÖÆÆ÷Õı³£
	    Number1_Error=Number1_Error&0xbf;   //ÉÏ±¨¿ØÖÆÆ÷Õı³£
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
	if(Flag_TRB_OK==1)  //Î²½°·À±ùÊ§Ğ§
	{
	  Number1_Error=Number1_Error|0x4;   //ÉÏ±¨Î²½°·À±ùÊ§Ğ§
      GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //¹Ø±ÕÎ²½°¼ÓÈÈ
      GpioDataRegs.GPBDAT.bit.GPIOB0=0;   //¹Ø±ÕÎ²½°¿ØÖÆ¿ª¹Ø
	  if(Flag_MBIT_Start==0)
      {
        GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //µãÁÁÎ²½°³ı±ùÊ§Ğ§µÆ
	  }
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
  /////////¼ì²é×Ô¶¯¿ØÖÆµçÂ·ÊÇ·ñÕı³£//////////////////////////
  if(Flag_Controller_OK==0)
  { 
    GpioDataRegs.GPBDAT.bit.GPIOB8=0;    //Êä³ö"ÎÂ¶ÈÊäÈë×Ô¼ì"ĞÅºÅ
    Delay(0xffff);
    AirSensor_Check(); //ÅĞ¶Ï´óÆøÎÂ¶È´¦ÀíµçÂ·ÊÇ·ñÕı³£
    if(Flag_Controller_OK==1)
	{
	  AirSensor_Check();
	}

    TRBSensor_Check(); //ÅĞ¶ÏÎ²½°ÎÂ¶È´«¸ĞÆ÷´¦ÀíµçÂ·ÊÇ·ñÕı³£
    if(Flag_Controller_OK==1)
	{
	  TRBSensor_Check();
	}

    //////¼ì²éÖ÷½°ÈıÏà´¦ÀíµçÂ·//////////////////
	GpioDataRegs.GPBDAT.bit.GPIOB6=0;    //Æô¶¯"Ö÷½°¸ĞÓ¦×Ô¼ì"ĞÅºÅ
    Delay(0x3ffff);
    MRB_QXJC();
    if(Flag_Controller_OK==1)
	{
	  MRB_QXJC();
	}
    //////¼ì²éÎ²½°ÈıÏà´¦ÀíµçÂ·//////////////////
	GpioDataRegs.GPBDAT.bit.GPIOB7=0;    //Æô¶¯"Î²½°¸ĞÓ¦×Ô¼ì"ĞÅºÅ
    Delay(0x3ffff);
    TRB_QXJC();
    if(Flag_Controller_OK==1)
	{
	  TRB_QXJC();
	}

    GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all|0x1e0;  //¹Ø±ÕÈı¸ö×Ô¼ìÃüÁî
  }

  /////////¼ì²éÊÖ¶¯¿ØÖÆµçÂ·ÊÇ·ñÕı³£//////////////////////////
  if(Flag_Controller_OK==0)
  { 
    GpioDataRegs.GPBDAT.bit.GPIOB5=1;    //Æô¶¯"ÊÖ¶¯³ı±ù×Ô¼ì"ĞÅºÅ
    Delay(0xffff); 
    if(GpioDataRegs.GPADAT.bit.GPIOA8==0)
    {       
      Delay(0xffff);
      if(GpioDataRegs.GPADAT.bit.GPIOA8==0)
      {
	    Flag_Controller_OK=1;
	    Number1_Error=Number1_Error|0x40;   //ÉÏ±¨¿ØÖÆÆ÷Ê§Ğ§
      } 
	}
    GpioDataRegs.GPBDAT.bit.GPIOB5=0;
  }
  ////////////////////////////////////////////////
  if(Flag_Controller_OK==0)     //ÉÏµçBITÕı³£
  {
    GpioDataRegs.GPBDAT.bit.GPIOB9=1;  //µãÁÁ¿ØÖÆÆ÷Ö¸Ê¾µÆ
	Flag_PowerOnBIT_Finish=1;   //ÉÏµçBITÍê³É 
  }        
}  

//////////½Ó´¥Æ÷¼ì²é////////////////////////////
void Switch_Check(void)
{    
  ////////////¶Ï¿ªÖ÷Î²½°ËùÓĞ¼ÓÈÈ/////////////////////////
  GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //¹Ø±ÕÎ²½°¼ÓÈÈ
  GpioDataRegs.GPBDAT.bit.GPIOB0=0;  //¹Ø±ÕÎ²½°¿ØÖÆ¿ª¹Ø
  GpioDataRegs.GPBDAT.bit.GPIOB2=1;  //¹Ø±ÕÖ÷½°¼ÓÈÈ
  GpioDataRegs.GPBDAT.bit.GPIOB11=0;  //¹Ø±ÕÖ÷½°¿ØÖÆ¿ª¹Ø 
  Flag_MRB_OK=0;    //×Ô¶¯³ı±ù³öÏÖ¹ÊÕÏÊ±ÈÔ¿É½øÈëÎ¬»¤BIT
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
  /////////////////¼ì²éÖ÷½°½Ó´¥Æ÷¶Ï¿ª×´Ì¬ÊÇ·ñÕı³£//////////////////
  Delay(0x3ffff);
  if(GpioDataRegs.GPADAT.bit.GPIOA0==0)	//Ö÷½°½Ó´¥Æ÷×Ô¼ìĞÅºÅ0¹ÊÕÏ
  {
    Delay(0xffff);
    if(GpioDataRegs.GPADAT.bit.GPIOA0==0)
	{
	  Flag_MRB_OK=1;      //Ö÷½°¹ÊÕÏ
	}
  }
  /////////////////¼ì²éÎ²½°½Ó´¥Æ÷¶Ï¿ª×´Ì¬ÊÇ·ñÕı³£//////////////////
  Delay(0x3ffff); 
  if(GpioDataRegs.GPADAT.bit.GPIOA1==0)	//Î²½°½Ó´¥Æ÷×Ô¼ìĞÅºÅ0¹ÊÕÏ
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
      Number2_Error=Number2_Error|0x4;  //ÉÏ±¨½á±ùÌ½²âÊ§Ğ§	  
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
    /////////////Ö÷½°½Ó´¥Æ÷¼ì²éÕı³£ºó½øĞĞ¼ÓÈÈ///////////////////  
    if(Flag_MRB_OK==0)
    {
	  if((Flag_MBIT_MRB==0)&&(Flag_MRB_Check==0))
      { 
        ONT_Real=34;     //Î¬»¤BIT¼ÓÈÈÊ±¼äÎª0.85s    
        Time_Test=16;   //0.4sÍê³ÉÒ»´Î¼à¿Ø            
	    Flag_MBIT_MRB=1;   //×¼±¸½øĞĞÎ¬»¤BITÖ÷½°¼ÓÈÈ
      }
	  /////////ÏÈ¼ì²éÖ÷½°¼ÓÈÈÊÇ·ñÕı³£////////////////////
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
    ///////////////ÔÙ¼ì²éÎ²½°¼ÓÈÈı³////////
    if(Flag_TRB_OK==0)
    {
	  if((Flag_MBIT_TRB==0)&&(Flag_MRB_Check==1)&&(Flag_TRB_Check==0))
	  {
        Flag_MBIT_TRB=1;  //×¼±¸½øĞĞÎ¬»¤BITÎ²½°¼ÓÈÈ
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
    GpioDataRegs.GPBDAT.bit.GPIOB3=0;      //ÃğµÆ
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
    if(GpioDataRegs.GPBDAT.bit.GPIOB2==1)   
    {       
      GpioDataRegs.GPBDAT.bit.GPIOB2=0;    //½ÓÍ¨Ö÷½°¿ØÖÆ¿ª¹Ø 	   
	  Delay(0x8fffff);
      if(Flag_MBIT_MRB==1)    //Î¬»¤BITÊ±¼ì²éÖ÷½°½Ó´¥Æ÷½ÓÍ¨×´Ì¬ÊÇ·ñÕı³£
      {
        if(GpioDataRegs.GPADAT.bit.GPIOA0==1)	//Ö÷½°½Ó´¥Æ÷×Ô¼ìĞÅºÅ1¹ÊÕÏ
        {
          Delay(0xffff);
          if(GpioDataRegs.GPADAT.bit.GPIOA0==1)
	      {
	        Flag_MRB_OK=1;      //Ö÷½°¹ÊÕÏ
	      }
		}
      }	  
	}
	if((GpioDataRegs.GPBDAT.bit.GPIOB2==0)&&(Count_MRB_Heat<=0))
	{
	  Count_Pulse=1;
	  Flag_Heat_Start=1;
	}
    /////////////////////¼ÓÈÈÊ±Ğò///////////////////// 
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
      OFT_Real=OFT_Vary;    //½«¼ÆËãµÄ¶Ï¿ªÊ±¼ä¸³Öµ
	  Count_MRB_Test=0;
	  Flag_FK_Test=0;
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
	else    //×Ô¶¯³ı±ù
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
    GpioDataRegs.GPBDAT.bit.GPIOB11=0;   //¶Ï¿ª¼ÓÈÈÊ±Ğò
    GpioDataRegs.GPBDAT.bit.GPIOB2=1;    //¶Ï¿ªÖ÷½°µçÔ´
    Flag_Heat_Start=0; 
	Flag_Heat_Start=0;
    Count_MRB_Heat=0;
    Count_MRB_Test=0;
    Flag_FK_Test=0; 
  }   
 	
  //////////////È±Ïà¼°·´À¡Ê±Ğò¼à¿Ø/////////////////////////////////////////////  	
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

///////////////·´À¡Ê±Ğò¼ì²é//////////////////////////////////
void FK_Check(void)
{
  if(GpioDataRegs.GPADAT.bit.GPIOA4==1)   //¼ÓÈÈÊ±Ğò·´À¡¼ì²é
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
        GpioDataRegs.GPBDAT.bit.GPIOB11=0;   //¶Ï¿ª¼ÓÈÈÊ±Ğò
        GpioDataRegs.GPBDAT.bit.GPIOB2=1;    //¶Ï¿ªÖ÷½°µçÔ´		
        Number1_Error=Number1_Error|0xa;  //ÉÏ±¨Ö÷½°Í¬²½Ê§Ğ§,Ö÷½°¹ÊÕÏ
        if(Flag_MBIT_Start==0)
        {
         GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //µãÁÁÖ÷½°³ı±ùÊ§Ğ§µÆ
        }
		Flag_FK_Test=1;			  //±¾Çø·´À¡Ê±Ğò¼ì²éÍê³É
      }
	}
  }

  if(fk_error==0)
  {
    Flag_FK_Test=1;          //±¾Çø·´À¡Ê±Ğò¼ì²éÍê³É
  }
}

///////////////Î²½°¼ÓÈÈ¿ØÖÆ////////////////////////////////////
void TRB_Heat(void)
{  
  if(GpioDataRegs.GPADAT.bit.GPIOA7==0)  //²éÑ¯¼ÓÈÈµçÔ´ÊÇ·ñ½ÓÍ¨
  { 
    if(Flag_MBIT_TRB==1)     //Î¬»¤BITÊ±¼ì²éÎ²½°½Ó´¥Æ÷½ÓÍ¨×´Ì¬ÊÇ·ñÕı³£
    { 
      GpioDataRegs.GPBDAT.bit.GPIOB0=1;    //½ÓÍ¨Î²½°¿ØÖÆ¿ª¹Ø
      Delay(0x3ffff);
      if(GpioDataRegs.GPADAT.bit.GPIOA1==1)	//Î²½°½Ó´¥Æ÷×Ô¼ìĞÅºÅ1¹ÊÕÏ
      {
        Delay(0xffff);
        if(GpioDataRegs.GPADAT.bit.GPIOA1==1)
	    {
	      Flag_TRB_OK=1;     //Î²½°¹ÊÕÏ
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
	  if(Flag_TRBSensor1_OK==0)
	  { 	    	    
	    if(TRBSensor_Temperature>1060)
	    { 
	      GpioDataRegs.GPBDAT.bit.GPIOB1=1;   //Î²½°¼ÓÈÈ
	      GpioDataRegs.GPBDAT.bit.GPIOB0=1;    //½ÓÍ¨Î²½°µçÔ´	      
		}
        if(TRBSensor_Temperature<740)
		{ 
		  GpioDataRegs.GPBDAT.bit.GPIOB1=0;   //Î²½°Í£Ö¹¼ÓÈÈ		  	 
		}
	  }
      else
	  { 
	    if(Flag_TRBSensor2_OK==0)
	    { 
	      if(TRBSensor_Temperature>1060)
	      { 
	        GpioDataRegs.GPBDAT.bit.GPIOB1=1;   //Î²½°¼ÓÈÈ
	        GpioDataRegs.GPBDAT.bit.GPIOB0=1;    //½ÓÍ¨Î²½°µçÔ´
		  }
          if(TRBSensor_Temperature<740)
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
 
///////////////////¶¨Ê±Æ÷1ÖĞ¶Ï//////////////////////////////////

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
	  if((GpioDataRegs.GPBDAT.bit.GPIOB2==1)&&(GpioDataRegs.GPBDAT.bit.GPIOB0==0))   
	  {
	    if(GpioDataRegs.GPADAT.bit.GPIOA3==0) //½ÓÊÕµ½Î¬»¤×Ô¼ìÃüÁî
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
		    Flag_E422_Receive=1;  //ÊÕµ½Êı¾İ
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
		        Ice_Degree=0x2;    //ÖĞ¶È½á±ù
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


///////////////////ADCÖĞ¶Ï//////////////////////////////////
interrupt void ad_isr(void)
{ 
  Uint16 Result[16];          //±£´æADÊı¾İ
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
 		
	///////////16¸öÊı¾İÓÉĞ¡µ½´óÅÅÁĞ//////////////////////
	for(Count_sample=0;Count_sample<16;Count_sample++)
	{ for(i=0;i<16-Count_sample;i++)                      
	  { if(Result[i]>Result[i+1])
	    { max=Result[i];
		  Result[i]=Result[i+1];
          Result[i+1]=max;
		}
	  }
	}    	
	//////////È¥Í·È¥Î²È¡ÖĞ¼ä14¸öÓĞĞ§Êı¾İÇóÆ½¾ù////////////	
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


///////////////////Î²½°¶ÌÂ·ÖĞ¶Ï//////////////////////////////////
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
	    Number1_Error=Number1_Error|0x4;   //ÉÏ±¨Î²½°·À±ùÊ§Ğ§
        GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //¹Ø±ÕÎ²½°¼ÓÈÈ
        GpioDataRegs.GPBDAT.bit.GPIOB0=0;   //¹Ø±ÕÎ²½°¿ØÖÆ¿ª¹Ø
		if(Flag_MBIT_Start==0)
        {
          GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //µãÁÁÎ²½°³ı±ùÊ§Ğ§µÆ
        } 
	  }
	}
  }
  EINT;
}

///////////////////Ö÷½°¶ÌÂ·ÖĞ¶Ï//////////////////////////////////
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
	    Number1_Error=Number1_Error|0x2;   //ÉÏ±¨Ö÷½°³ı±ùÊ§Ğ§
		if(Flag_MBIT_Start==0)
		{
		  GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //µãÁÁÖ÷½°³ı±ùÊ§Ğ§µÆ
		}
	  }
	} 
  }
  EINT;
}


///////////Ö±½Ó·µ»ØÖĞ¶Ï·şÎñ³ÌĞò//////////////
interrupt void nothing()
{ 
  EINT;
  return;
}

/////////////Ö÷³ÌĞò//////////////////////////////////////

main()
{  
  Init_Variable();
  InitSysCtrl();
  //°ÑInitFlash£¨£©º¯Êı°áÒÆµ½ramÖĞÔËĞĞ
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
  
  //¿ªÖĞ¶Ï   
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
	if(EvaRegs.T1CON.bit.TENABLE==1)  //¶¨Ê±Æ÷Õı³£
	{
	  if(Count_SCI_Send>=40)     //1sµ½·¢ËÍ422¼à¿ØĞÅÏ¢
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
        if(Flag_E422_Receive==1)  //ÊÕµ½LWCÊı¾İ
	    {
          Flag_E422_Receive=0;
	    }
	    else
	    {
          Count_E422_Error++;
		  if(Count_E422_Error>=5)    //Á¬Ğø5´ÎÎ´ÊÕµ½LWC
		  {          
            Number2_Error=Number2_Error|0x4;   //ÉÏ±¨½á±ùÌ½²âÊ§Ğ§ 
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
    ///////////////////////////////////////////////////////////  
    if(Flag_PowerOnBIT_Finish==1)   //ÉÏµçBITÍê³É
    {
      if(Flag_MBIT_Start==1)   //×¼±¸½øĞĞÎ¬»¤BIT
      { 
	    Number2_Error=Number2_Error|0x10;  //ÕıÔÚ×Ô¼ì
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
		else	   //½á±ùÌ½²âÊ§Ğ§ÉÏ±¨Ö÷½°¹ÊÕÏ
		{
	      if((Flag_MBIT_Start==0)&&(Flag_MRB_Heat==0))
		  {
			Flag_MRB_OK=1;     //Ö÷½°¹ÊÕÏ
		  }
		} 
        /////////////////////////////////////////////
        if((Number1_Error&0x20)==0x0)
        {
          AirSensor_Check();  //´óÆøÎÂ¶È´«¸ĞÆ÷¼ì²é
		}
		////////////////////////////////////////////////
		if(Flag_TRBSensor1_OK==0)
		{
		  TRBSensor_Check();  //Î²½°ÎÂ¶È´«¸ĞÆ÷¼ì²é
		}
		if((Flag_TRBSensor1_OK==1)&&(Flag_TRBSensor2_OK==0))
		{
		  TRBSensor_Check();  //1¹ÊÕÏÊ±Á¢¼´¼ì²é2
		} 			
		/////////////////////////////////////////////////////
        if(GpioDataRegs.GPBDAT.bit.GPIOB10==0)  //ÎÂ¶È²»×èÈû
        {		  
		  if(Flag_TRB_OK==0)     //Î²½°Õı³£
		  {
		    if(((Number2_Error&0x4)==0x0)&&(GpioDataRegs.GPADAT.bit.GPIOA5==0))  //½á±ù
		    {
		      TRB_Heat();         //Î²½°¼ÓÈÈ
			}
			else
			{
              GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //¹Ø±ÕÎ²½°¼ÓÈÈ
              GpioDataRegs.GPBDAT.bit.GPIOB0=0;  //¹Ø±ÕÎ²½°¿ØÖÆ¿ª¹Ø			  
			}
		  }
          if(((Number2_Error&0x4)==0x0)&&((Number1_Error&0x20)==0x0))
          {
            if(GpioDataRegs.GPADAT.bit.GPIOA5==0)  //½á±ù
            {
              Delay(0xffff);
              if(GpioDataRegs.GPADAT.bit.GPIOA5==0)
			  {
                Number1_Error=Number1_Error|0x1;
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
		}
        ////////////////////////////////////////////////
        if((Flag_MRB_OK==0)&&(Flag_MRB_Heat==1))
		{
          MRB_Heat();	  //Ö÷½°ÕıÔÚ¼ÓÈÈ
		}
		else
		{
          GpioDataRegs.GPBDAT.bit.GPIOB11=0;  //¹Ø±ÕÖ÷½°¼ÓÈÈ
          GpioDataRegs.GPBDAT.bit.GPIOB2=1;   //¹Ø±ÕÖ÷½°¿ØÖÆ¿ª¹Ø
		}
        /////////////////////////////////////////////////////////////
        if((GpioDataRegs.GPBDAT.bit.GPIOB10==1)||(Flag_TRB_OK==1))
		{
          GpioDataRegs.GPBDAT.bit.GPIOB1=0;  //¹Ø±ÕÎ²½°¼ÓÈÈ
          GpioDataRegs.GPBDAT.bit.GPIOB0=0;  //¹Ø±ÕÎ²½°¿ØÖÆ¿ª¹Ø
		}
	  } 
	  ////////////////////////////////////////////////////////////
	  if((GpioDataRegs.GPBDAT.bit.GPIOB2==0)||(GpioDataRegs.GPBDAT.bit.GPIOB1==1))
	  {
	    GpioDataRegs.GPBDAT.bit.GPIOB13=1;    //µãÁÁ·À³ı±ùÏµÍ³µÆ
	    Number2_Error=Number2_Error|0x8;
	  } 
	  else
	  {
	    GpioDataRegs.GPBDAT.bit.GPIOB13=0;    //·À³ı±ùÏµÍ³µÆÃğµô
        Number2_Error=Number2_Error&0xf7;
	  }   
    }  
    else   //ÉÏµçBIT¹ÊÕÏ
    {
	  GpioDataRegs.GPBDAT.bit.GPIOB14=1;  //µãÁÁÖ÷½°³ı±ùÊ§Ğ§µÆ 
      GpioDataRegs.GPBDAT.bit.GPIOB15=1;  //µãÁÁÎ²½°³ı±ùÊ§Ğ§µÆ      
      if(Count_Light>=5)   //ÉÏµçBIT¹ÊÕÏ¿ØÖÆÆ÷Ö¸Ê¾µÆ4HzÉÁË¸
      {
        Count_Light=0;
        GpioDataRegs.GPBDAT.bit.GPIOB9=~GpioDataRegs.GPBDAT.bit.GPIOB9;
	  }  
    }  
  }          
}  

