 /* 
   * Î²Òí·À³ı±ù¿ØÖÆÆ÷
   * ÎÄ¼şÃû£ºXZ-62.pit
   * Ê±¼ä£º2011.1.1.10.27
   * °æ±¾£ºµÚ1.01°æ
   
*/

#include "DSP28_Device.h"
#include "math.h"

#define RAMBASE1 0x0080000      //±£´æ°æ±¾ºÅÊ×µØÖ·
#define RAMBASE2 0x0080005      //±£´æ¹ÊÕÏ´ÎÊıÊ×µØÖ·
#define RAMBASE3 0x0080009      //±£´æ×´Ì¬ĞÅÏ¢ÂëÊ×µØÖ·
#define RAMBASE6 0x0100000      //×ÜÏßµØÖ·,¶ÔÓ¦ÓÚÆ¬Ñ¡ĞÅºÅXZCS6 
#define ID1 0x01200000       //×ó¿ØÖÆÆ÷·¢ËÍµ½ÓÒ¿ØÖÆÆ÷MSGID
#define ID2 0x01300000       //×ó¿ØÖÆÆ÷·¢ËÍµ½ÊÔÑéÆ÷MSGID
#define ID3 0x02100000       //ÓÒ¿ØÖÆÆ÷·¢ËÍµ½×ó¿ØÖÆÆ÷MSGID
#define ID4 0x03100000       //ÊÔÑéÆ÷·¢ËÍµ½×ó¿ØÖÆÆ÷MSGID
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


////////////////±äÁ¿ÉêÃ÷/////////////////////////////////
Uint32 i;                  //±äÁ¿
Uint16 channel;           //¶¨ÒåÒ»¸ö²É¼¯Í¨µÀ±äÁ¿
int a[16];                //¶¨ÒåÒ»¸öÊı×éÓÃÓÚ±£´æAD×ª»»µÄ½á¹û
int n=0;
float jrwd;                //²É¼¯ÎÂ¶È´«¸ĞÆ÷Êı¾İ
float qxdl;                //²É¼¯¶ÏÏàµçÁ÷
Uint16 a1,a2;              //²É¼¯Ğ£ÕıµçÑ¹
Uint16 a3;     
float gain,offest;         //¶¨ÒåÔöÒæºÍÆ«ÖÃÏµÊı
Uint16 *rambase1,*rambase2,*rambase3; //¶¨ÒåÍâ²¿½Ó¿ÚÖ¸Õë
Uint16 tmp,tmp1;             //¶¨ÒåÍâ²¿½Ó¿Ú±äÁ¿
Uint16 *rambase6;      
Uint16 CSH;               //429³õÊ¼»¯
Uint16 FSDZJ_271;         //·¢ËÍLABLE271µÍ×Ö½Ú
Uint16 FSGZJ_271;         //·¢ËÍLABLE271¸ß×Ö½Ú
Uint16 FSDZJ_272;
Uint16 FSGZJ_272; 
Uint16 FSDZJ_273;
Uint16 FSGZJ_273; 
Uint16 FSDZJ_300;          //·¢ËÍÈí¼ş°æ±¾ºÅ
Uint16 FSGZJ_300;
Uint16 FSDZJ_301;       
Uint16 FSGZJ_301;
Uint16 FSDZJ_302;
Uint16 FSGZJ_302;
Uint16 FSDZJ_303;
Uint16 FSGZJ_303;
Uint16 FSDZJ_304;           //·¢ËÍÓ²¼ş°æ±¾ºÅ
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
Uint16 JSDZJ;			     //½ÓÊÕµÍ×Ö½Ú
Uint16 JSGZJ;			     //½ÓÊÕ¸ß×Ö½Ú
Uint32 gz=0;                   //¹ÊÕÏ´ÎÊı¼ÇÂ¼
char m=0;
char sd[10]={0,0,0,0,0,0,0,0,0,0};
char j=0;                      //³ı±ù¿ØÖÆ°´Å¤°´Ñ¹´ÎÊı
char count1=0;                //1s¼ÆÊı±äÁ¿ 
char count2=0;                //0.5s¼ÆÊı±äÁ¿CPLD¼à¿ØDSPÊ±ÖÓ
Uint16 count3=0;              //¼ÓÈÈ¼à¿ØÊ±¼ä±äÁ¿
char count4=0;                //È±Ïà¼à¿ØÊ±¼ä±äÁ¿
char count5=0;                //ÈÈµ¶ÎÂ¶È¼à²âÊ±¼ä±äÁ¿
char count6=0;                //4ÇøÓòÎÂ¶È¼à²âÊ±¼ä±äÁ¿
char count7=0;                 //3ÇøÓòÎÂ¶È¼à²âÊ±¼ä±äÁ¿
char count8=0;                //2ÇøÓòÎÂ¶È¼à²âÊ±¼ä±äÁ¿
char count9=0;                 //1ÇøÓòÎÂ¶È¼à²âÊ±¼ä±äÁ¿
char count10=0;                //ÅĞ¶Ï¶ÌÂ·ÑÓÊ±±äÁ¿
int t1=250,t2=250;                   //¼ÓÈÈÊ±¼ä
char k[32]={0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0
           };                //Ã¿Â·ÎÂ¶È³¬ÎÂÊ±¼ä 
Uint16 lz=0;                 //½ÓÊÕµ½ÂÖÔØĞÅºÅµÄ´ÎÊı
int error1=0;                 //Î´ÊÕµ½ÂÖÔØĞÅºÅµÄ´ÎÊı»ò´íÎóĞÅÏ¢
float wd;                     //´óÆø¾²ÎÂ
Uint16 wd1=0;                 //½ÓÊÕµ½´óÆøÎÂ¶ÈĞÅºÅµÄ´ÎÊı
int error2=0;                 //Î´ÊÕµ½´óÆøÎÂ¶ÈĞÅºÅµÄ´ÎÊı»ò´íÎóĞÅÏ¢
Uint16 can1=0,can2=0;
char EvaTimer1InterruptCount=0,EvaTimer2InterruptCount=0;                
char flag1;                   //×ó/ÓÒ¿ØÖÆÆ÷±êÖ¾
char flag2=0;                 //ADCĞ£Õı±êÖ¾flag2=1Íê³É
char flag3=0;                 //ÉÏµç×Ô¼ìÕı³£                
char flag4=0;                  //µØÃæÎ¬»¤Õı³£
char flag5=0;                 //È±Ïà¼à¿Ø0.5ÑÓÊ±±êÖ¾flag5=1Íê³É
char flag6=0;                 //·É»úÔÚ¿ÕÖĞ±êÖ¾flag6=1
char flag6a=0;
char flag7=0;                  //Ö÷¿ØÖÆÆ÷×ó»úÒí¶ÏÂ·´«¸ĞÆ÷Õı³£±êÖ¾
char flag8=0;                  //Ö÷¿ØÖÆÆ÷ÓÒ»úÒí¶ÏÂ·´«¸ĞÆ÷Õı³£±êÖ¾
char flag9=0;                  //ÉÏµç×Ô¼ìÍê³É±êÖ¾
char flag10=0;                  //µØÃæ×Ô¼ìÍê³É±êÖ¾
char flag11=0;                  //Ö÷¿ØÖÆÆ÷Õı³£
char flag12=0;                  //³ı±ùÆô¶¯±êÖ¾flag12=1
char flag13=0;                  //°´Å¦Ö÷/¸¨ÇĞ»»±êÖ¾
char flag14=0;                  //¹ÊÕÏÊ±ÇĞ»»±êÖ¾
char flag15=0;                  //¿ªÊ¼½ÓÊÜ429±êÖ¾flag15=1
char flag16=0;                  //274ÂÖÔØ½ÓÊÕ±êÖ¾flag16=1
char flag17=0;                    //274ÂÖÔØ½ÓÊÕÅĞ¶Ï±êÖ¾
char flag18=0;                  //213¾²ÎÂ½ÓÊÕ±êÖ¾flag18=1
char flag19=0;                    //213¾²ÎÂ½ÓÊÕÅĞ¶Ï±êÖ¾
char flag20=0;                  //´óÆø¾²ÎÂ<5
char flag21=0;                  //¶ÌÂ·ÅĞ¶Ï±êÖ¾flag21=1¶ÌÂ·
char flag22=0;
char flag23=0;
Uint32 TestMbox1,TestMbox2;   //ÓÊÏä2½ÓÊÕµ½µÄĞÅÏ¢
Uint32 TestMbox3,TestMbox4;   //ÓÊÏä3½ÓÊÕµ½µÄĞÅÏ¢


/////////////////º¯ÊıÉêÃ÷/////////////////////////////////

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

   SysCtrlRegs.PLLCR = 0x8;   //Ö÷Æµ=30*8/2=120MHz
  
   for(i=0; i<5000; i++){}   // ÎªÁËÆµÂÊÎÈ¶¨£¬±ØĞëÑÓÊ±Ò»¶ÎÊ±¼ä
      
   SysCtrlRegs.HISPCP.all = 0x0002;  //¸ßËÙÍâÉèÊ±ÖÓ=120/4=30
   SysCtrlRegs.LOSPCP.all = 0x0002;	 //µÍËÙÍâÉèÊ±ÖÓ=120/4=30
 
   SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;  //Ê¹ÄÜEVAÄ£¿éµÄ¸ßËÙÍâÉèÊ±ÖÓ
   SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;  //Ê¹ÄÜEVBÄ£¿éµÄ¸ßËÙÍâÉèÊ±ÖÓ
   SysCtrlRegs.PCLKCR.bit.ADCENCLK=1;  //Ê¹ÄÜADCÄ£¿éµÄ¸ßËÙÍâÉèÊ±ÖÓ
   SysCtrlRegs.PCLKCR.bit.ECANENCLK=1; //Ê¹ÄÜCANÍâÉèµÄÏµÍ³Ê±ÖÓ
				
   EDIS;
	
}


///////////////////IO¿Ú³õÊ¼»¯//////////////////////////////////

void InitGpio(void)
{

   EALLOW;
   GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOAÉèÖÃÎªI/O¿Ú 
   GpioMuxRegs.GPADIR.all=0xFFFF;   //A0-A15ÉèÖÃÎªÊä³ö	
   GpioDataRegs.GPADAT.all=0x23f; 

   GpioMuxRegs.GPBMUX.all=0x0000;    //GPIOBÉèÖÃÎªI/O¿Ú 
   GpioMuxRegs.GPBDIR.all=0xFFDF;    //B5ÉèÖÃÎªÊäÈë£¬ÆäÓàÉèÖÃÎªÊä³ö	
   GpioDataRegs.GPBDAT.all=0x8010;   
     
   GpioMuxRegs.GPFMUX.all=0x00c0;  //ÉèÖÃGPIOF6/7ÎªeCAN¿Ú,ÆäÓàÉèÖÃÎªI/O¿Ú
   GpioMuxRegs.GPFDIR.bit.GPIOF9=1;
   GpioMuxRegs.GPFDIR.bit.GPIOF11=1;
   GpioMuxRegs.GPFDIR.bit.GPIOF12=0;
   GpioMuxRegs.GPFDIR.bit.GPIOF13=0;
   GpioDataRegs.GPFDAT.bit.GPIOF9=1;
   GpioDataRegs.GPFDAT.bit.GPIOF11=0;
   EDIS;

}	

///////////////////Íâ²¿ÖĞ¶Ï13³õÊ¼»¯//////////////////////////////////

void InitXINT13(void)
{  GpioMuxRegs.GPEMUX.bit.XNMI_XINT13_GPIOE2=1; //ÉèÖÃÖĞ¶Ï13¹¦ÄÜ 
   XIntruptRegs.XNMICR.all=0x2;	 //ÏÂ½µÑØÉú³ÉÖĞ¶Ï
}

///////////////////Íâ²¿ÖĞ¶Ï1³õÊ¼»¯//////////////////////////////////

void InitXINT1(void)
{  GpioMuxRegs.GPEMUX.bit.XINT1_XBIO_GPIOE0=1; //ÉèÖÃÍâ¿ÖĞ¶Ï1¹¦ÄÜ 
   XIntruptRegs.XINT1CR.all=0x5;      //ÉÏÉıÑØÉú³ÉÖĞ¶Ï
}

///////////////////Íâ²¿ÖĞ¶Ï2³õÊ¼»¯//////////////////////////////////

void InitXINT2(void)
{  GpioMuxRegs.GPEMUX.bit.XINT2_ADCSOC_GPIOE1=1; //ÉèÖÃÍâ²¿ÖĞ¶Ï13¹¦ÄÜ 
   XIntruptRegs.XINT2CR.all=0x5;      //ÉÏÉıÑØÉú³ÉÖĞ¶Ï
}

///////////////////PDPINTBÖĞ¶Ï³õÊ¼»¯//////////////////////////////////

void InitPDPINTB(void)
{  GpioMuxRegs.GPDMUX.bit.T3CTRIP_PDPB_GPIOD5=1; //ÖÃÏ¹¦ÄÜ
   EvbRegs.EVBIMRA.bit.PDPINTB=1;   //ÊĞíPDPINTBÖĞ
   EvbRegs.EVBIFRA.bit.PDPINTB=1;   //Çå³ıPDPINTBÖĞ¶Ï±êÖ¾              
}

///////////////////¶¨Ê±Æ÷1³õÊ¼»¯£¨100mS©//////////////////////////////////

void init_eva_timer1(void)
{
  
   EvaRegs.GPTCONA.all = 0;   //³õÊ¼»¯¶¨Ê±Æ÷1²¢ÉèÖÃ¶¨Ê±Æ÷1¼Ä´æÆ÷
   EvaRegs.EVAIMRA.bit.T1PINT = 1;  //ÔÊĞí¶¨±Æ÷1ÖÜÆÚÖĞ¶Ï
   EvaRegs.EVAIFRA.bit.T1PINT = 1;  //Çå¶¨÷1ÖÜÆÚÖĞ¶Ï±êÖ¾
             
   EvaRegs.T1CON.all = 0x170C;  //Timer1ÎªÁ¬ĞøÔö¼ÆÊıÄ£Ê½£¬Ô¤·ÖÆµÖµ128
	                             //Ê¹ÓÃÄÚ²¿¸ßËÙÍâÉèÊ±ÖÓ
   EvaRegs.T1PR = 0x5b8d;   //Timer1µÄÆÚ¼Ä´æÆ÷=0.1*30MHz/128
   EvaRegs.T1CNT = 0x0000;  //Timer1µÄ¼ÆÊıã

}


///////////////////¶¨Ê±Æ÷2³õÊ¼»¯£¨100mS£©//////////////////////////////////

void init_eva_timer2(void)
{
  
    EvaRegs.GPTCONA.all = 0;   //³õÊ¼»¯¶¨±Æ÷2²¢ÉèÖÃ¶¨Ê±Æ÷2¼Ä´æÆ÷
    EvaRegs.EVAIMRB.bit.T2PINT = 1;  //ÔÊĞí¶¨Ê±Æ÷2ÖÜÆÚÖĞ¶Ï
    EvaRegs.EVAIFRB.bit.T2PINT = 1;  //Çå³ı¶¨Ê±Æ÷2ÖÜÆÚÖĞ¶Ï±êÖ¾
             
    EvaRegs.T2CON.all = 0x170C;  //Timer2ªÁ¬Ôö¼ÆÊÄ£Ê½£¬Ô¤·ÖÆµÖµÎª128£¬
	                             //Ê¹ÓÃÄÚ²¿¸ßËÙÍâÉèÊ±ÖÓ
	EvaRegs.T2PR = 0x5b8d;   //Timer2µÄÖÜÆÚ¼Ä´æÆ÷=0.1*30MHz/128
	EvaRegs.T2CNT = 0x0000;  //Timer2µÄ¼ÆÊıÆ÷Çå

}

///////////////////AD³õÊ¼»¯//////////////////////////////////
void InitAdc(void)
{
		
	AdcRegs.ADCTRL1.bit.RESET=1;  //¸´Î»ADCÄ£¿é
	for(i=0;i<5000;i++)	 {} 
	AdcRegs.ADCTRL1.bit.RESET=0;
	AdcRegs.ADCTRL1.bit.SUSMOD=3;
	AdcRegs.ADCTRL1.bit.ACQ_PS=0;
	AdcRegs.ADCTRL1.bit.CPS=0;     
	AdcRegs.ADCTRL1.bit.CONT_RUN=0; //ADÎªÆô¶¯/Í£Ö¹Ä£Ê½
	AdcRegs.ADCTRL1.bit.SEQ_CASC=1; //16¼¶ÁªÄ£Ê½
     	
	AdcRegs.ADCTRL3.bit.ADCBGRFDN=3; //´ø¼äÏ¶²Î¿¼µçÂ·ÉÏµç
	for(i=0;i<10000;i++) {}           //µÈ´ıÎÈ¶¨
	AdcRegs.ADCTRL3.bit.ADCPWDN=1;  //ÆäËüµçÂ·ÉÏµç
	for(i=0;i<5000;i++)	 {}           //µÈ´ıÎÈ¶¨
	AdcRegs.ADCTRL3.bit.ADCCLKPS=15;  
	AdcRegs.ADCTRL3.bit.SMODE_SEL=0;  //Ë³Ğò²ÉÑù

	
	AdcRegs.MAX_CONV.bit.MAX_CONV=15;  //²ÉÑùÍ¨µÀÎª16
  
    AdcRegs.CHSELSEQ1.all= channel;   //Í¬Ò»¸öÍ¨µÀ²É16´Î
    AdcRegs.CHSELSEQ2.all= channel;
    AdcRegs.CHSELSEQ3.all= channel;
    AdcRegs.CHSELSEQ4.all= channel;

    AdcRegs.ADCTRL2.all=0x8c04;

	////////////²ÉÓÃ¶¨Ê±Æ÷4Æô¶¯AD×ª»»/////////////////////
    EvbRegs.GPTCONB.all=0x400;    //¶¨Ê±Æ÷4ÖÜÆÚÖĞ¶ÏÆô¶¯AD×ª»»
    EvbRegs.EVBIMRB.bit.T4PINT = 1;  //ÔÊĞí¶¨Ê±Æ÷4ÖÜÆÚÖĞ¶Ï
    EvbRegs.EVBIFRB.bit.T4PINT = 1;  //Çå³ı¶¨Ê±Æ÷4ÖÜÆÚÖĞ¶Ï±êÖ¾
             
    EvbRegs.T4CON.all = 0x170C;  //Timer4ÎªÁ¬ĞøÔö¼ÆÊıÄ£Ê½£¬Ô¤·ÖÆµÖµÎª128£¬
	                             //Ê¹ÓÃÄÚ²¿¸ßËÙÍâÉèÊ±ÖÓ
	EvbRegs.T4PR = 0xea;     //Timer4µÄÖÜÆÚ¼Ä´æÆ÷=0.001*30MHz/128
	EvbRegs.T4CNT = 0x0000;  //Timer4µÄ¼ÆÊıÆ÷ÇåÁã
				
	AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;
	AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;	
			
}	
	
////////////////////Íâ²¿½Ó¿Ú³õÊ¼»¯/////////////////////////
void InitXintf(void)
{  
    XintfRegs.XINTCNF2.bit.XTIMCLK = 0x0001;  //ËùÓĞzoneµÄÊ±ĞòÊÇ»ùÓÚXTIMCLK = SYSCLKOUT/2 
    XintfRegs.XINTCNF2.bit.WRBUFF = 3;   //ÉèÖÃ3¸öĞ´»º³å
	XintfRegs.XINTCNF2.bit.CLKOFF = 0;  //Ê¹ÄÜXCLKOUT
	XintfRegs.XINTCNF2.bit.CLKMODE = 0;  //ÉèÖÃXCLKOUT=XTIMCLK
    	
	XintfRegs.XTIMING2.bit.X2TIMING=1;
	//Zone2Ğ´Ê±Ğò
	XintfRegs.XTIMING2.bit.XWRLEAD = 1;    //Ğ´²Ù×÷Òıµ¼½×¶ÎµÄÖÜÆÚ=2*XTIMCLK
	XintfRegs.XTIMING2.bit.XWRACTIVE = 7;   //Ğ´²Ù×÷¼¤»î½×¶ÎµÈ´ı×´Ì¬µÄÖÜÆÚ=14*XTIMCLK
	XintfRegs.XTIMING2.bit.XWRTRAIL = 2;    //Ğ´²Ù×÷¸ú×Ù½×¶ÎµÄÖÜÆÚ=4*XTIMCLK
   //Zone2¶ÁÊ±Ğò
	XintfRegs.XTIMING2.bit.XRDLEAD = 1;    //¶Á²Ù×÷Òıµ¼½×¶ÎµÄÖÜÆÚ=2*XTIMCLK
	XintfRegs.XTIMING2.bit.XRDACTIVE=7;	    //¶Á²Ù×÷¼¤»î½×¶ÎµÈ´ı×´Ì¬µÄÖÜÆÚ=14*XTIMCLK
	XintfRegs.XTIMING2.bit.XRDTRAIL = 2;   //¶Á²Ù×÷¸ú×Ù½×¶ÎµÄÖÜÆÚ=4*XTIMCLK
	//²»¼Ó±¶Ê±Ğò
	XintfRegs.XTIMING2.bit.X2TIMING = 0; 
   //Zone2²»¶ÔREADYĞÅºÅ²ÉÑù
    XintfRegs.XTIMING2.bit.USEREADY = 0;
    XintfRegs.XTIMING2.bit.READYMODE = 0;
    XintfRegs.XTIMING2.bit.XSIZE = 3;   //16Î»½Ó¿Ú£¬Î¨Ò»ÓĞĞ§µÄÉèÖÃ

    XintfRegs.XBANK.bit.BANK = 2;   //Ê¹ÄÜÄÚ²¿ÇøÓò2
    XintfRegs.XBANK.bit.BCYC = 3;  //Á¬Ğø·ÃÎÊÖ®¼äÌí¼ÓÁË3¸öXTIMCLKÖÜÆÚ

	//Zone6Ğ´Ê±Ğò
	XintfRegs.XTIMING6.bit.XWRLEAD = 1;    //Ğ´²Ù×÷Òıµ¼½×¶ÎµÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING6.bit.XWRACTIVE = 7;   //Ğ´²Ù×÷¼¤»î½×¶ÎµÈ´ı×´Ì¬µÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING6.bit.XWRTRAIL = 2;    //Ğ´²Ù×÷¸ú×Ù½×¶ÎµÄÖÜÆÚ=XTIMCLK
   //Zone6¶ÁÊ±Ğò
	XintfRegs.XTIMING6.bit.XRDLEAD = 1;    //¶Á²Ù×÷Òıµ¼½×¶ÎµÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING6.bit.XRDACTIVE=1;	    //¶Á²Ù×÷¼¤»î½×¶ÎµÈ´ı×´Ì¬µÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING6.bit.XRDTRAIL = 1;   //¶Á²Ù×÷¸ú×Ù½×¶ÎµÄÖÜÆÚ=XTIMCLK
	//²»¼Ó±¶Ê±Ğò
	XintfRegs.XTIMING6.bit.X2TIMING = 0; 
   //Zone6²»¶ÔREADYĞÅºÅ²ÉÑù
    XintfRegs.XTIMING6.bit.USEREADY = 0;
    XintfRegs.XTIMING6.bit.READYMODE = 0;
    XintfRegs.XTIMING6.bit.XSIZE = 3;   //16Î»½Ó¿Ú£¬Î¨Ò»ÓĞĞ§µÄÉèÖÃ

    XintfRegs.XBANK.bit.BANK = 6;   //Ê¹ÄÜÄÚ²¿ÇøÓò6
    XintfRegs.XBANK.bit.BCYC = 3;  //Á¬Ğø·ÃÎÊÖ®¼äÌí¼ÓÁË3¸öXTIMCLKÖÜÆÚ
	
}

////////////////////ECAN³õÊ¼»¯/////////////////////////
void InitECan(void)
{
	EALLOW;
	ECanaRegs.CANTIOC.bit.TXFUNC = 1;
	ECanaRegs.CANRIOC.bit.RXFUNC = 1;
	EDIS;
		
	ECanaRegs.CANME.all = 0;  //½ûÖ¹ËùÓĞÓÊÏä

	if(flag1==0)
	{ ECanaRegs.CANMD.bit.MD0=0;  //ÓÊÏä0·¢ËÍµ½ÓÒ¿ØÖÆÆ÷
      ECanaRegs.CANMD.bit.MD1=0;  //ÓÊÏä1·¢ËÍµ½ÊÔÑéÆ÷
      ECanaRegs.CANMD.bit.MD2=1;  //ÓÊÏä2½ÓÊÕÓÒ¿ØÖÆÆ÷ĞÅÏ¢
      ECanaRegs.CANMD.bit.MD3=1;  //ÓÊÏä3½ÓÊÕÊÔÑéÆ÷ĞÅÏ¢
     	
	  ECanaMboxes.MBOX0.MID.all = ID1;	
	  ECanaMboxes.MBOX1.MID.all = ID2;
	  ECanaMboxes.MBOX2.MID.all = ID3;
	  ECanaMboxes.MBOX3.MID.all = ID4;
	}
    else
	{ ECanaRegs.CANMD.bit.MD0=0;  //ÓÊÏä0·¢ËÍµ½×ó¿ØÖÆÆ÷
      ECanaRegs.CANMD.bit.MD2=1;  //ÓÊÏä2½ÓÊÕ×ó¿ØÖÆÆ÷ĞÅÏ¢
    	
	  ECanaMboxes.MBOX0.MID.all = ID3;	
	  ECanaMboxes.MBOX2.MID.all = ID1;
	}
			
	ECanaRegs.CANME.bit.ME0=1;	//ÓÊÏäÊ¹ÄÜ
    ECanaRegs.CANME.bit.ME1=1;
    ECanaRegs.CANME.bit.ME2=1;
	ECanaRegs.CANME.bit.ME3=1;
		
	ECanaMboxes.MBOX0.MCF.bit.DLC = 8;	//Êı¾İ³¤¶È 8¸öBYTE
	ECanaMboxes.MBOX1.MCF.bit.DLC = 8;
	ECanaMboxes.MBOX2.MCF.bit.DLC = 8;
	ECanaMboxes.MBOX3.MCF.bit.DLC = 8;
	
	ECanaMboxes.MBOX0.MCF.bit.RTR = 0;	
	ECanaMboxes.MBOX1.MCF.bit.RTR = 0;  
	ECanaMboxes.MBOX2.MCF.bit.RTR = 0;	
	ECanaMboxes.MBOX3.MCF.bit.RTR = 0; 

	EALLOW;	
	ECanaRegs.CANMIM.all = 0xFFFFFFFF;	//ÓÊÏäÆÁ±ÎÊ¹ÄÜ
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
	ECanaRegs.CANMC.bit.STM = 0; //Õı³£¹¤×÷Ä£Ê½ 
	ECanaRegs.CANMC.bit.SCM = 0; //sccÄ£Ê½
    ECanaRegs.CANMC.bit.DBO = 0; //Ê×ÏÈ½ÓÊÕ»ò·¢ËÍÊı¾İµÄ×î¸ßÓĞĞ§×Ö½Ú
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

///////////////¿ØÖÆÆ÷´úÂë½»»»///////////////////////////////////
void MBOX0Send(void)
{  
    ECanaMboxes.MBOX0.MDRL.all = ((Uint32)FSGZJ_271<<16)+(Uint32)FSDZJ_271;
	ECanaMboxes.MBOX0.MDRH.all = 0x444d3143;
	ECanaRegs.CANTRS.bit.TRS0=1;   //Æô¶¯·¢ËÍ	
    for(i=0;i<0xffff;i++) {}
    ECanaRegs.CANTA.bit.TA0=1;
    ECanaRegs.CANTRS.bit.TRS0=0;
	for(i=0;i<0xffff;i++) {}

    ECanaMboxes.MBOX0.MDRL.all = ((Uint32)FSGZJ_272<<16)+(Uint32)FSDZJ_272;
	ECanaMboxes.MBOX0.MDRH.all = 0x444d3243;
	ECanaRegs.CANTRS.bit.TRS0=1;   //Æô¶¯·¢ËÍ	
	for(i=0;i<0xffff;i++) {}
    ECanaRegs.CANTA.bit.TA0=1;
    ECanaRegs.CANTRS.bit.TRS0=0;
    for(i=0;i<0xffff;i++){}
 
    ECanaMboxes.MBOX0.MDRL.all = ((Uint32)FSGZJ_273<<16)+(Uint32)FSDZJ_273;
	ECanaMboxes.MBOX0.MDRH.all = 0x444d3343;
	ECanaRegs.CANTRS.bit.TRS0=1;   //Æô¶¯·¢ËÍ
	for(i=0;i<0xffff;i++) {}
    ECanaRegs.CANTA.bit.TA0=1;
    ECanaRegs.CANTRS.bit.TRS0=0;
    for(i=0;i<0xffff;i++){}

    if((JSDZJ&0xff)==0x8b)
    { ECanaMboxes.MBOX0.MDRL.all = ((Uint32)JSGZJ<<16)+(Uint32)JSDZJ;
      ECanaMboxes.MBOX0.MDRH.all = 0x444d3443;
	  ECanaRegs.CANTRS.bit.TRS0=1;   //ô¯·¢ËÍ	
      for(i=0;i<0xffff;i++) {}
      ECanaRegs.CANTA.bit.TA0=1;
      ECanaRegs.CANTRS.bit.TRS0=0;
    }
	can1++;
}

///////////////¿ØÖÆÆ÷·¢ËÍµ½ÊÔÑéÆ÷///////////////////////////////////
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

/////////////Í¨¹ı×ÜÏßÏòÍâ²¿´æ´¢Æ÷Ğ´Ó²¼ş°æ±¾ºÅ////////////////////
void saveDA1(void)
{  
   *rambase1=tmp&0xff; 	
   rambase1++;    
}

/////////////Í¨¹ı×ÜÏßÏòÍâ²¿´æ´¢Æ÷Ğ´×Ü¹ÊÕÏ´ÎÊı ////////////////////
void saveDA2(void)
{  
   *rambase2=tmp&0xff; 	
   rambase2++;    
}

/////////////Í¨¹ı×ÜÏßÏòÍâ²¿´æ´¢Æ÷Ğ´×´Ì¬ĞÅÏ¢Âë////////////////////
void saveDA3(void)
{  
   *rambase3=tmp&0xff; 	
   rambase3++;    
}

/////////////Í¨¹ı×ÜÏß¶ÁÈ¡ÉÏ´Î¹ÊÕÏ´ÎÊı////////////////////
void readDA1(void)
{ 
   tmp1=(*rambase1)&0xff;       //½«Êı¾İ×ÜÏßÉÏµÄÊı¾İ¶Áµ½±äÁ¿tmp1ÖĞ
   rambase1++;   
}

/////////////Í¨¹ı×ÜÏß¶ÁÈ¡ÉÏ´Î¹ÊÕÏ´ÎÊı////////////////////
void readDA2(void)
{ 
   tmp1=(*rambase2)&0xff;       //½«Êı¾İ×ÜÏßÉÏµÄÊı¾İ¶Áµ½±äÁ¿tmpÖĞ
   rambase2++;   
}

/////////////Í¨¹ı×ÜÏß¶ÁÈ¡ÉÏ´Î×´Ì¬ĞÅÏ¢Âë½áÊø´¦////////////////////
void readDA3(void)
{ 
   tmp1=(*rambase3)&0xff;       //½«Êı¾İ×ÜÏßÉÏµÄÊı¾İ¶Áµ½±äÁ¿tmpÖĞ 
   rambase3++; 
}

//////////////Çå³ı´æ´¢Æ÷Êı¾İ/////////////////////////////
void QCdata(void)
{
  rambase3=(Uint16 *)RAMBASE3;
  rambase6=(Uint16 *)RAMBASE6;
  for(;rambase3<rambase6;rambase3++)
  { *rambase3=0xff;
  } 
  
}

////////////ADCĞ£Õı////////////////////////////////////

void ADC_adjust(void)
{    
   ///////²É¼¯ADCINB6(2.5V)/////////
   channel=0xeded; 
   ADC_sample();
   a1=a3;      
   ///////²É¼¯ADCINB7(1.25V)///////////   
   channel=0xfdfd; 
   ADC_sample();  
   a2=a3;       
   //////////¼ÆËãÔöÒæºÍÆ«ÖÃ/////////////////
   gain=(float)(HIGH_COUNT-LOW_COUNT)/(a1-a2); 
   offest=a2*gain-1733.0;
   flag2=1;     //ADCĞ£ÕıÍê±Ï
}

///////////////Èí¼ş´úÂë¼ì²é////////////////////////////////
void DMJC(void)
{ /////////¼ì²é´æ´¢Æ÷¶ÁĞ´ÊÇ·ñÕı³£//////////////////////  
   tmp=0x11;
   *rambase1=tmp;
   for(i=0;i<10000;i++){}
   tmp1=*rambase1;
   rambase1=(Uint16 *)RAMBASE1; 
   if(tmp1!=tmp)  //´æ´¢Æ÷¹ÊÕÏ
   { GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU·¢³ö¹ÊÕÏĞÅºÅ  
     gz++;   
   }
   /////////¼ì²é¶¨Ê±Æ÷ÊÇ·ñÕı³£//////////////////////////
   EvaRegs.T1CON.bit.TENABLE=1;   //´ò¿ª¶¨Ê±Æ÷
   EvaRegs.T2CON.bit.TENABLE=1;
   while(1)         
   { if(EvaTimer2InterruptCount==10)  //ÑÓÊ±1S
     break;	 
   }
   EvaRegs.T1CON.bit.TENABLE=0;   //¹Ø±Õ¶¨Ê±Æ÷
   EvaRegs.T2CON.bit.TENABLE=0;
   if(EvaTimer1InterruptCount!=10)  //¶¨Ê±Æ÷¹ÊÕÏ
   { GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU·¢³ö¹ÊÕÏĞÅºÅ
     gz++; 
   }
   EvaTimer1InterruptCount=0;
   EvaTimer2InterruptCount=0;

}

/////////////////ÈÈµ¶¶ÌÂ·´«¸ĞÆ÷¼ì²é///////////////////////
void redao_DLJC(void)
{  GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0xa; //×ó»úÒíÈÈµ¶
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))  //ÉÏµç×Ô¼ì»òµØÃæÎ¬»¤Ê±
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[0]++;
	 }
     if(sd[0]==2)
	 { sd[0]=0;
	   FSGZJ_272=FSGZJ_272|0x1;    //×ó1/2ÅäµçºĞ¹ÊÕÏ
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU·¢³ö¹ÊÕÏĞÅºÅ
	 }
   } 
   
   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all|0xf;  //ÓÒ»úÒíÈÈµ¶
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))
   {  if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[1]++;
	 }
     if(sd[1]==2)
	 { sd[1]=0;	 
	   FSDZJ_272=FSDZJ_272|0x8000;  //ÓÒ1/2ÅäµçºĞ¹ÊÕÏ
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU·¢³ö¹ÊÕÏĞÅºÅ
	 }
   }
 
}

/////////////////1ºÅ¶Î¶ÌÂ·´«¸ĞÆ÷¼ì²é///////////////////////
void abc1_DLJC(void)
{  GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0x6; //×ó»úÒí1ºÅ¶Î
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[2]++;
	 }
     if(sd[2]==2)
	 { sd[2]=0;
	   FSGZJ_272=FSGZJ_272|0x1;    //×ó1/2ÅäµçºĞ¹ÊÕÏ
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU·¢³ö¹ÊÕÏĞÅºÅ
	 }
   }
   
   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0xb; //ÓÒ»úÒí1ºÅ¶Î
   for(i=0;i<0x5000;i++) {}  
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[3]++;
	 }
     if(sd[3]==2)
	 { sd[3]=0;
	   FSDZJ_272=FSDZJ_272|0x8000;  //ÓÒ1/2ÅäµçºĞ¹ÊÕÏ
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU·¢³ö¹ÊÕÏĞÅºÅ
	 }
   }

}

/////////////////2ºÅ¶Î¶ÌÂ·´«¸ĞÆ÷¼ì²é///////////////////////  
void abc2_DLJC(void) 
{  GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0x7;  //×ó»úÒí2ºÅ¶Î
   for(i=0;i<0x5000;i++) {}  
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[4]++;
	 }
     if(sd[4]==2)
	 { sd[4]=0;
	   FSGZJ_272=FSGZJ_272|0x1;    //×ó1/2ÅäµçºĞ¹ÊÕÏ
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU·¢³ö¹ÊÕÏĞÅºÅ
	 }
   }
      
   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0xc; //ÓÒ»úÒí2ºÅ¶Î
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[5]++;
	 }
     if(sd[5]==2)
	 { sd[5]=0;
	   FSDZJ_272=FSDZJ_272|0x8000;  //ÓÒ1/2ÅäµçºĞ¹ÊÕÏ
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU·¢³ö¹ÊÕÏĞÅºÅ
	 }
   }
}

/////////////////3ºÅ¶Î¶ÌÂ·´«¸ĞÆ÷¼ì²é///////////////////////  
void abc3_DLJC(void)
{  GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0x8;  //×ó»úÒí3ºÅ¶Î
   for(i=0;i<0x5000;i++) {}   
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[6]++;
	 }
     if(sd[6]==2)
	 { sd[6]=0; 
	   FSGZJ_272=FSGZJ_272|0x1;    //×ó1/2ÅäµçºĞ¹ÊÕÏ
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU·¢³ö¹ÊÕÏĞÅºÅ
	 }
   }

   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0xd;    //ÓÒ»úÒí3ºÅ¶Î
   for(i=0;i<0x5000;i++) {}  
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[7]++;
	 }
     if(sd[7]==2)
	 { sd[7]=0; 
	   FSDZJ_272=FSDZJ_272|0x8000;  //ÓÒ1/2ÅäµçºĞ¹ÊÕÏ
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU·¢³ö¹ÊÕÏĞÅºÅ
	 }
   }

}

/////////////////4ºÅ¶Î¶ÌÂ·´«¸ĞÆ÷¼ì²é///////////////////////  
void abc4_DLJC(void)
{  GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0x9;    //×ó»úÒí4ºÅ¶Î
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[8]++;
	 }
     if(sd[8]==2)
	 { sd[8]=0; 
	   FSGZJ_272=FSGZJ_272|0x1;    //×ó1/2ÅäµçºĞ¹ÊÕÏ
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU·¢³ö¹ÊÕÏĞÅºÅ
	 }
   }

   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xfff0|0xe; //ÓÒ»úÒí4ºÅ¶Î
   for(i=0;i<0x5000;i++) {} 
   if((flag9==1)||(flag10==1))
   { if(GpioDataRegs.GPBDAT.bit.GPIOB5==1)
	 { sd[9]++;
	 }
     if(sd[9]==2)
	 { sd[9]=0;
	   FSDZJ_272=FSDZJ_272|0x8000;  //ÓÒ1/2ÅäµçºĞ¹ÊÕÏ
	   gz++; 
	   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU³ö¹ÊÕÏĞÅºÅ
	 }
   }
}

/////////////ÉÏµç×Ô¼ì//////////////////////////////////

void poweron_BIT(void)
{  
   flag9=1;
   FSDZJ_273=FSDZJ_273|0x2000;     //ÕıÔÚ×Ô¼ì
   DMJC();

   //////////¼ì²é¶ÌÂ·´«¸ĞÆ÷ÊÇ·ñÕı³£///////////////
   /*GpioDataRegs.GPFDAT.bit.GPIOF9=0;  //¶ÌÂ·´«¸ĞÆ÷×Ô¼ì
   GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //´ò¿ª¶ÌÂ·²âÊÔÍ¨µÀÑ¡Ôñ
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
   GpioDataRegs.GPFDAT.bit.GPIOF9=1;  //¹Ø±Õ×Ô¼ìµçÔ´*/

   if(GpioDataRegs.GPBDAT.bit.GPIOB15==0)
   { flag3=1;
   }
   FSDZJ_273=FSDZJ_273&0xdfff;   // ×Ô¼ìÍê³É 
   flag9=0;      

}

////////////////µØÃæÎ¬»¤BIT////////////////////////////
void DMWH_BIT(void)
{  flag10=1;
   FSDZJ_273=FSDZJ_273|0x2000;     //ÕıÔÚ×Ô¼ì  
   DMJC();

   for(i=0;i<40000;i++){}
   /*if(GpioDataRegs.GPFDAT.bit.GPIOF12==1) 
   { EvaRegs.T1CON.bit.TENABLE=1;  // ´ò¿ª¶¨Ê±Æ÷1
     GpioDataRegs.GPADAT.bit.GPIOA9=0;   //´ò¿ª°²È«¿ª¹Ø
     GpioDataRegs.GPADAT.bit.GPIOA15=1;   //ÈÈµ¶¼ÓÈÈ
     GpioDataRegs.GPADAT.bit.GPIOA14=1;   //4Çø¼ÓÈÈ,Ê±¿Ì1      
     GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //´ò¿ª¶ÌÂ·²âÊÔµÀ¡Ôñ
	 for(i=0;i<0xffff;i++){}
     redao_DLJC();     
     //redao_temperature();
     abc4_DLJC();
     //abc4_temperature();
     //QXJC();  
     
      ////////////µØÃæÎ¬»¤¼ÓÈÈÊ±Ğò//////////////////////////////
     if(EvaTimer1InterruptCount==8)          //Ê±¿Ì2              
     { GpioDataRegs.GPADAT.bit.GPIOA14=0; 
       GpioDataRegs.GPADAT.bit.GPIOA13=1;
       abc3_DLJC();
	   //QXJC();
       //abc3_temperature();
     }
     if(EvaTimer1InterruptCount==16)         //Ê±¿Ì3             
     { GpioDataRegs.GPADAT.bit.GPIOA13=0; 
       GpioDataRegs.GPADAT.bit.GPIOA12=1; 
	   abc2_DLJC();
	   //QXJC();
       //abc2_temperature();
     }
     if(EvaTimer1InterruptCount==24)          //Ê±¿Ì4             
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
	 GpioDataRegs.GPBDAT.bit.GPIOB4=1;  //¹Ø±Õ¶ÌÂ·²âÊÔÍ¨µÀÑ¡Ôñ 
     EvaRegs.T1CON.bit.TENABLE=0; 
   }*/
   
   if(GpioDataRegs.GPBDAT.bit.GPIOB15==0)
   { flag4=1;
   }
   FSDZJ_273=FSDZJ_273&0xdfff;   // ×Ô¼ìÍê³É 
   flag10=0; 
   arinc429fasong(); //429ÉÏ±¨×Ô¼ìĞÅÏ¢
   MBOX0Send();  

}


/////////////429·¢ËÍ////////////////////////////////
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

///////////////¼ÓÈÈ¿ØÖÆ////////////////////////////////////
void jiare(void)
{  if(flag13==0)
   { GpioDataRegs.GPADAT.bit.GPIOA9=0;   //´ò¿ª°²È«¿ª¹Ø 
	 FSGZJ_271=FSGZJ_271|0x2000; //Î²Òí·À³ı±ùÏµÍ³¿ª
	  ///////ÑÓÊ±2s////////////////////////////
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
     GpioDataRegs.GPADAT.bit.GPIOA15=1;   //ÈÈµ¶¼ÓÈÈ
	 FSDZJ_272=FSDZJ_272|0x2000;   //ÕıÔÚ½øĞĞ¼ÓÈÈ
   }

    ////////////¼ÓÈÈÊ±Ğò//////////////////////////////
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
      GpioDataRegs.GPBDAT.bit.GPIOB4=1;  //¹Ø±Õ¶ÌÂ·²âÊÔÍ¨µÀÑ¡Ôñ
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
    GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //´ò¿ª¶ÌÂ·²âÊÔÍ¨µÀÑ¡Ôñ

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

//////////////ĞÅºÅ²É¼¯///////////////////////////////
void ADC_sample(void)
{  InitAdc(); 
   EvbRegs.T4CON.bit.TENABLE=1;
   while(1)
   {if(n==8)
	break;
   } 
   n=0;	 
   if(flag2==1) //ÉÏµç×Ô¼ìÍêºóĞŞÕı²É¼¯µçÑ¹
   { a3=(int)(gain*a3-offest);
   }		
   EvbRegs.T4CON.bit.TENABLE=0;
}  

/////////////////ÈÈµ¶ÎÂ¶È¼à²â//////////////////////////////
/*void redao_temperature(void)
{ 
    GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1cc0;   //×ó1ÈÈµ¶      
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095; 
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[3]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x400;     //×ó1¶Î³¬ÎÂ
	  gz++; 
	  k[3]=k[3]+1;                     
	  if(k[3]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	  gz++; 
	  FSGZJ_272=FSGZJ_272|0x400;     //×ó1¶Î³¬ÎÂ
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1dc0;   //×ó2ÈÈµ¶     
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[7]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x200;     //×ó2¶Î³¬ÎÂ
	  gz++; 
	  k[7]=k[7]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[7]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x200;     //×ó2¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x16c0;   //×ó3ÈÈµ¶
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[11]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x100;     //×ó3¶Î³¬ÎÂ
	  gz++; 
	  k[11]=k[11]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[11]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x100;     //×ó3¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x17c0;   //×ó4ÈÈµ¶
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[15]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x80;     //×ó4¶Î³¬ÎÂ
	  gz++; 
	  k[15]=k[15]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[15]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x80;     //×ó4¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1ac0;   //ÓÒ1ÈÈµ¶ 
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[19]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x40;     //ÓÒ1¶Î³¬ÎÂ
	  gz++; 
	  k[19]=k[19]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[19]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x40;     //ÓÒ1¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1bc0;   //ÓÒ2ÈÈµ¶
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[23]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x20;     //ÓÒ2¶Î³¬ÎÂ
	  gz++; 
	  k[23]=k[23]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[23]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x20;     //ÓÒ2¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xec0;   //ÓÒ3ÈÈµ¶ 
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[27]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x10;     //ÓÒ3¶Î³¬ÎÂ
	  gz++; 
	  k[27]=k[27]+1;                      //¼ÇÂ¼¬ÎÂÊ±¼ä
	  if(k[27]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x10;     //ÓÒ3¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xfc0;   //ÓÒ4ÈÈµ¶ 
    channel=0x8d8d;       //ADCINB0²É¼¯Â¶È«¸ĞÆ÷çÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[31]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x8;     //ÓÒ4¶Î³¬ÎÂ
	  gz++; 
	  k[31]=k[15]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[31]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x8;     //ÓÒ4¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}
}

/////////////////×ó/ÓÒ4abcÎÂ¶È¼à²â//////////////////////////////
void abc4_temperature(void)
{   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1700;   //×ó4a
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[12]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x80;     //×ó4¶Î³¬ÎÂ
	  gz++; 
	  k[12]=k[12]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[12]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x80;     //×ó4¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1740;   //×ó4b
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[13]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x80;     //×ó4¶Î³¬ÎÂ
	  gz++; 
	  k[13]=k[13]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[13]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x80;     //×ó4¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1780;   //×ó4c 
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();	
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[14]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x80;     //×ó4¶Î³¬ÎÂ
	  gz++; 
	  k[14]=k[14]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[14]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x80;     //×ó4¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xf00;   //ÓÒ4a 
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[28]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x8;     //ÓÒ4¶Î³¬ÎÂ
	  gz++; 
	  k[28]=k[28]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[28]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x8;     //ÓÒ4¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xf40;   //ÓÒ4b  
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[29]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x8;     //ÓÒ4¶Î³¬ÎÂ
	  gz++; 
	  k[29]=k[29]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[29]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x8;     //ÓÒ4¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xf80;   //ÓÒ4c 
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[23]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x8;     //ÓÒ4¶Î³¬ÎÂ
	  gz++; 
	  k[31]=k[31]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[31]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x8;     //ÓÒ4¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	 
	}
}

/////////////////×ó/ÓÒ3abcÎÂ¶È¼à²â//////////////////////////////
void abc3_temperature(void)
{   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1600;   //×ó3a  
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂÈ¸Ğ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[8]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x100;     //×ó3¶Î³¬ÎÂ
	  gz++; 
	  k[8]=k[8]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[8]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x100;     //×ó3¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1640;   //×ó3b 
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[9]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x100;     //×ó3¶Î³¬ÎÂ
	  gz++; 
	  k[9]=k[9]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[9]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x100;     //×ó3¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1680;   //×ó3c 
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[10]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x100;     //×ó3¶Î³¬ÎÂ
	  gz++; 
	  k[10]=k[10]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[10]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x100;     //×ó3¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xe00;   //ÓÒ3a 
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷çÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[24]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x10;     //ÓÒ3¶Î¬ÎÂ
	  gz++; 
	  k[24]=k[24]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[24]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x10;     //ÓÒ3¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xe40;   //ÓÒ3b 
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[25]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x10;     //ÓÒ3¶Î³¬ÎÂ
	  gz++; 
	  k[25]=k[25]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[25]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x10;     //ÓÒ3¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0xe80;   //ÓÒ3c 
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[26]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x10;     //ÓÒ3¶Î³¬ÎÂ
	  gz++; 
	  k[26]=k[26]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[26]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x10;     //ÓÒ3¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}
}

/////////////////×ó/ÓÒ2abcÎÂ¶È¼à²â//////////////////////////////
void abc2_temperature(void)
{   GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1d00;   //×ó2a 
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[4]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x200;     //×ó2¶Î³¬ÎÂ
	  gz++; 
	  k[4]=k[4]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[4]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x200;     //×ó2¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1d40;   //×ó2b 
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[5]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x200;     //×ó2¶Î¬ÎÂ
	  gz++; 
	  k[5]=k[5]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[5]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x200;     //×ó2Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1d80;   //×ó2c 
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[6]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x200;     //×ó2¶Î³¬ÎÂ
	  gz++; 
	  k[6]=k[6]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[6]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x200;     //×ó2¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1b00;   //ÓÒ2a 
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[20]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x20;     //ÓÒ2¶Î³¬ÎÂ
	  gz++; 
	  k[20]=k[20]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[20]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x20;     //ÓÒ2¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1b40;   //ÓÒ2b  
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[21]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x20;     //ÓÒ2¶Î³¬ÎÂ
	  gz++; 
	  k[21]=k[21]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[21]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x20;     //2¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1b80;   //ÓÒ2c 
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[22]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x20;     //ÓÒ2¶Î³¬ÎÂ
	  gz++; 
	  k[22]=k[22]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[22]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x20;     //ÓÒ2¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}
}

/////////////////×ó/ÓÒ1abcÎÂ¶È¼à²â//////////////////////////////
void abc1_temperature(void)
{ 
    GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1c00;   //×ó1a 
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[0]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x400;     //×ó1¶Î³¬ÎÂ
	  gz++; 
	  k[0]=k[0]+1;                      //¼Ç³¬ÎÂÊä
	  if(k[0]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x400;     //×ó1¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}
    
	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1c40;   //×ó1b
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[1]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x400;     //×ó1¶Î³¬ÎÂ
	  gz++; 
	  k[1]=k[1]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[1]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x400;     //×ó1¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}
	
	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1c80;   //×ó1c
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[2]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x400;     //×ó1¶Î³¬ÎÂ
	  gz++; 
	  k[2]=k[2]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[2]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x400;     //×ó1¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

    GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1a00;   //ÓÒ1a  
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[20]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x40;     //ÓÒ1¶Î³¬ÎÂ
	  gz++; 
	  k[20]=k[20]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[20]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x40;     //ÓÒ1¶Î³¬ÎÂ 
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1a40;   //ÓÒ1b 
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[21]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x40;     //ÓÒ1¶Î³¬ÎÂ
	  gz++; 
	  k[21]=k[21]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[21]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x40;     //ÓÒ1¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}

	GpioDataRegs.GPBDAT.all=GpioDataRegs.GPBDAT.all&0xe03f|0x1a80;   //ÓÒ1c
    channel=0x8d8d;       //ADCINB0²É¼¯ÎÂ¶È´«¸ĞÆ÷µçÑ¹ĞÅºÅ
    ADC_sample();
    jrwd=(float)a3*3.0/4095;
	if((jrwd<=2.293)&&(jrwd>=1.1688))
	{ k[22]=0;
	}
    if((jrwd<2.6395)&&(jrwd>2.293))
	{ FSGZJ_272=FSGZJ_272|0x40;     //ÓÒ1Î³¬ÎÂ
	  gz++; 
	  k[22]=k[22]+1;                      //¼ÇÂ¼³¬ÎÂÊ±¼ä
	  if(k[22]>24)                    //³¬ÎÂ25s
	  { //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê
	  }
	}
    if((jrwd>=2.6395)&&(jrwd<=2.832))
	{ gz++; 
	  FSGZJ_272=FSGZJ_272|0x40;     //ÓÒ1¶Î³¬ÎÂ
	  //GpioDataRegs.GPBDAT.bit.GPIOB15=0;   //CPU±¨¹Ê	  
	}
}*/

//////////////×óÆ½Î²¼ÓÈÈµçÁ÷ÅĞ¶ÏÊÇ·ñ¿ªÂ·///////////////////
void ZDLJC(void)
{ 
  if((qxdl<15.0)||(qxdl>58.0))
  { 
	GpioDataRegs.GPADAT.bit.GPIOA9=1;
	if(GpioDataRegs.GPADAT.bit.GPIOA14==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x200;         //×ó4¶Î¶ÏÂ·
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA13==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x400;         //×óµÚ3¶Î¶ÏÂ·
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA12==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x800;         //×óµÚ2¶Î¶ÏÂ·
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA11==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x1000;         //×óµÚ1¶Î¶ÏÂ·
	}
	GpioDataRegs.GPBDAT.bit.GPIOB15=0;    //±¾¿ØÖÆÆ÷¹ÊÕÏ
	GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //¹Ø±ÕËùÓĞ¼ÓÈÈ
  } 
}

//////////////ÓÒÆ½Î²¼ÓÈÈµçÁ÷ÅĞ¶ÏÊÇ·ñ¿ªÂ·///////////////////
void YDLJC(void)
{  
  if((qxdl<15.0)||(qxdl>58.0))
  { 
	GpioDataRegs.GPADAT.bit.GPIOA9=1;
	if(GpioDataRegs.GPADAT.bit.GPIOA14==1)
    { gz++; 
	  FSGZJ_271=FSGZJ_271|0x20;         //ÓÒµÚ4¶Î¶ÏÂ·
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA13==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x40;         //ÓÒµÚ3¶Î¶ÏÂ·
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA12==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x80;         //ÓÒµÚ2¶Î¶ÏÂ·
	}
	if(GpioDataRegs.GPADAT.bit.GPIOA11==1)
	{ gz++; 
	  FSGZJ_271=FSGZJ_271|0x100;         //ÓÒµÚ1¶Î¶ÏÂ·	 
	}
    GpioDataRegs.GPBDAT.bit.GPIOB15=0;    //±¾¿ØÖÆÆ÷¹ÊÕÏ
	GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //¹Ø±ÕËùÓĞ¼ÓÈÈ	
  }
}

///////////////////×óÆ½Î²¿ªÂ·¼à¿Ø//////////////////////////////
void ZKLJC(void)
{  
   channel=0x0d0d;   //ADCINA0²É¼¯×ót
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5;   //²É¼¯µçÑ¹×ª»»³ÉµçÁ÷
   ZDLJC();
    
   channel=0x1d1d;   //ADCINA1²É¼¯×ós
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5;
   ZDLJC();
     
   channel=0x2d2d;   //ADCINA2²É¼¯×ór
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5;
   ZDLJC();    
}

///////////////////ÓÒÆ½Î²¿ªÂ·¼à¿Ø//////////////////////////////
void YKLJC(void)
{ 	 
   channel=0x3d3d;   //ADCINA3²É¼¯ÓÒt
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5;
   YDLJC();

   channel=0x4d4d;   //ADCINA4²É¼¯ÓÒs
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5; 
   YDLJC();

   channel=0x5d5d;   //ADCINA5²É¼¯ÓÒr
   ADC_sample();
   qxdl=(float)a3*3.0/4095;
   qxdl= qxdl*50/2.5; 
   YDLJC();   
}

///////////////×ó/ÓÒ¿ªÂ·È±Ïà¼à¿Ø////////////////////////////////
void QXJC(void)
{
   ZKLJC();
   if(GpioDataRegs.GPBDAT.bit.GPIOB15==0)
   { FSGZJ_272=FSGZJ_272|0x1;    //×óÅäµçºĞ¹ÊÕÏ	     
   }
   YKLJC();
   if(GpioDataRegs.GPBDAT.bit.GPIOB15==0)
   { FSDZJ_272=FSDZJ_272|0x8000;  //ÓÒÅäµçºĞ¹ÊÕÏ
   }
   if(flag7==1)
   { if((FSGZJ_272&0x1==0x1)&&(FSDZJ_272&0x8000==0x0000)) //×ó¹ÊÕÏÓÒÕı³£
     { FSGZJ_272=FSGZJ_272|0x1;    //×óÅäµçºĞ¹ÊÕÏ	
	 }
	 if((FSGZJ_272&0x1==0x0000)&&(FSDZJ_272&0x8000==0x8000)) //×óÕı³£ÓÒ¹ÊÕÏ
	 { if(flag1==0)
	   { FSGZJ_272=FSGZJ_272|0x2;    //2#µçÔ´¹ÊÕÏ
	   }
	   else
	   { FSGZJ_272=FSGZJ_272|0x4;    //1#µçÔ´¹ÊÕÏ
	   }
	 }
	 else
	 { FSGZJ_271=FSGZJ_271|0x4000;   //·À³ı±ùÏµÍ³Ê§Ğ§
	 }
   }
   if(flag8==1)
   { if((FSGZJ_272&0x1==0x0000)&&(FSDZJ_272&0x8000==0x8000)) //×óÕı³£ÓÒ¹ÊÕÏ
     { FSDZJ_272=FSDZJ_272|0x8000;  //ÓÒÅäµçºĞ¹ÊÕÏ
	 }
     if((FSGZJ_272&0x1==0x1)&&(FSDZJ_272&0x8000==0x0000)) //×ó¹ÊÕÏÓÒÕı³£
	 { if(flag1==0)
	   { FSGZJ_272=FSGZJ_272|0x4;    //1#µç¹ÊÕÏ
	   }
	   else
	   { FSGZJ_272=FSGZJ_272|0x2;    //2#µçÔ´¹ÊÕÏ
	   }
	 }
	 else
	 { FSGZJ_271=FSGZJ_271|0x4000;   //·À³ı±ùÏµÍ³Ê§Ğ§
	 }
   }
}

///////////////////¶¨Ê±Æ÷1ÖĞ¶Ï//////////////////////////////////

interrupt void eva_timer1_isr(void)
{  
  DINT;
  if(PieCtrl.PIEACK.bit.ACK2==1)
  { PieCtrl.PIEACK.bit.ACK2=1;
   
   /////////////////////////////////////////////////
   if((flag9==1)|(flag10==1))    //ÉÏµç×Ô¼ì»òµØÃæ×Ô¼ìÊ± 
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
          { FSGZJ_271=FSGZJ_271|0x1e;       //×ó±ß4¶ÎÈ«²¿¶ÌÂ·
	        gz++;	     
          }
	      if(GpioDataRegs.GPADAT.bit.GPIOA15==1)
          { FSGZJ_271=FSGZJ_271|0x1;	   //ÓÒ±ß4¶ÎÈ«²¿¶ÌÂ·
	        FSDZJ_271=FSDZJ_271|0xe000;       
	        gz++; 
          }
	      if(GpioDataRegs.GPADAT.bit.GPIOA11==1)
          {  FSGZJ_271=FSGZJ_271|0x10;     //×óµÚ1¶Î¶ÌÂ·
	         gz++; 
          }
          if(GpioDataRegs.GPADAT.bit.GPIOA11==1)
          {  FSGZJ_271=FSGZJ_271|0x1;     //ÓÒ1¶Î¶ÌÂ·
	         gz++; 
          }
          if(GpioDataRegs.GPADAT.bit.GPIOA12==1)
          {   FSGZJ_271=FSGZJ_271|0x8;     //×óµÚ2¶Î¶ÌÂ·
	          gz++; 
	      } 
	      if(GpioDataRegs.GPADAT.bit.GPIOA12==1)
	      { FSDZJ_271=FSDZJ_271|0x8000;     //ÓÒµÚ2¶Î¶ÌÂ·
		    gz++; 
	      }
	      if(GpioDataRegs.GPADAT.bit.GPIOA13==1)
	      { FSGZJ_271=FSGZJ_271|0x4;     //×óµÚ3¶Î¶ÌÂ·
		    gz++;
	      } 
	      if(GpioDataRegs.GPADAT.bit.GPIOA13==1)
	      { FSDZJ_271=FSDZJ_271|0x4000;     //ÓÒµÚ3¶Î¶ÌÂ·
		    gz++;
	      }
	      if(GpioDataRegs.GPADAT.bit.GPIOA14==1)
	      { FSGZJ_271=FSGZJ_271|0x1;     //×óµÚ4¶Î¶ÌÂ·
		    gz++; 
	      } 
	      if(GpioDataRegs.GPADAT.bit.GPIOA14==1)
	      { FSDZJ_271=FSDZJ_271|0x2000;     //ÓÒµÚ4¶Î¶ÌÂ·
		    gz++; 
	      }	 
          GpioDataRegs.GPADAT.bit.GPIOA9=1;		  
          GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //¹Ø±ÕËùÓĞ¼ÓÈÈ
	      //GpioDataRegs.GPBDAT.bit.GPIOB4=1;
		  GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU·¢³ö¹ÊÕÏĞÅºÅ
         }
		 else
		 {flag21=0;
		 }
	   }
	 }

     ///////////////////////////////////////////////////
     if(GpioDataRegs.GPFDAT.bit.GPIOF12==1) //³ı±ùÆô¶¯
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
	       GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //¹Ø±ÕËùÓĞ¼ÓÈÈ
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
	   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff; //¹Ø±ÕËùÓĞ¼ÓÈÈ
	   FSGZJ_271=FSGZJ_271&0xdfff; //Î²Òí·À³ı±ùÏµÍ³¹Ø±Õ
	   FSDZJ_272=FSDZJ_272&0xdfff;  //Ã»ÓĞ½øĞĞ¼ÓÈÈ¹¤×÷
	   if(flag12==0)
	   { m=0;
	   }
	   else
	   { flag13=1;   //×¼±¸ÇĞ»»
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

///////////////////¶¨Ê±Æ÷2ÖĞ¶Ï//////////////////////////////////

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

///////////////////ADCÖĞ¶Ï//////////////////////////////////
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
	for(n=0;n<8;n++)  //È¡8¸öÓĞĞ§Êı¾İÇóÆ½¾ù
	{ a3=a3+a[2*n+1];
	}
    a3=a3/8;

    AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;
    AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;
  }	
  EINT;
	
}

///////////////////ECANÖĞ¶ÏÓÊÕĞÅÏ¢///////////////////////////////
interrupt void ECAN0_ISR(void)
{ 
  DINT;
  if(PieCtrl.PIEACK.bit.ACK9==1)
  { PieCtrl.PIEACK.bit.ACK9=1;

   	if(ECanaRegs.CANRMP.bit.RMP2==1) //½ÓÊÕÁíÒ»¿ØÖÆÆ÷ĞÅÏ¢
    { TestMbox1=ECanaMboxes.MBOX2.MDRL.all;
      TestMbox2=ECanaMboxes.MBOX2.MDRH.all;
      if(TestMbox2==0x444d3143)      //²éÑ¯271
	  { TestMbox1=(TestMbox1&0x1e000000)>>28;
	    if(TestMbox1>=0x1)
		{ flag7=1;       //ÁíÒ»¿ØÖÆÆ÷×óÆ½Î²¿ªÂ·
		}
        TestMbox1=ECanaMboxes.MBOX2.MDRL.all;
        TestMbox1=(TestMbox1&0x1e00000)>>24;
	    if(TestMbox1>=0x1)
		{ flag8=1;       //ÁíÒ»¿ØÖÆÆ÷ÓÒÆ½Î²¿ªÂ·
		}
	  }
      ECanaRegs.CANRMP.bit.RMP2=1;
    }

    if(ECanaRegs.CANRMP.bit.RMP3==1)  //½ÓÊÕÊÔÑéÆ÷ĞÅÏ¢
    { if(flag6==0)  //·É»úÔÚµØÃæ
      { TestMbox3=ECanaMboxes.MBOX3.MDRL.all;
        TestMbox4=ECanaMboxes.MBOX3.MDRH.all;
        ECanaRegs.CANRMP.bit.RMP3=1;

         //////½ÓÊÕµ½Ó²¼ş°æ±¾ºÅ//////////////
        if((TestMbox4&0xffff0000)==0x58580000)
		{ 
		  //////////Ó²¼ş°æ±¾ºÅ±£´æ///////////////////
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
         ////////½ÓÊÕµ½¶Á´æ´¢Æ÷ÃüÁî////////////
        if((TestMbox4&0xffff0000)==0x44530000)
        { MBOX1Send();    
		}
         ////////½ÓÊÕµ½Çå³ı´æ´¢Æ÷ÃüÁî/////////////
        if((TestMbox4&0xffff0000)==0x51430000)
		{ QCdata(); 
		}
      }
    }
	can2++;
   }
   EINT;
}

///////////////////µôµç±£»¤ÖĞ¶Ï//////////////////////////////////

/*interrupt void XINT13_isr(void)
{  
   DINT;

   GpioDataRegs.GPBDAT.bit.GPIOB15=0;  //CPU·¢³ö¹ÊÕÏĞÅºÅ
   GpioDataRegs.GPADAT.bit.GPIOA9=1;
   flag22=0;
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //¹Ø±ÕËùÓĞ¼ÓÈÈ
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

///////////////////429½ÓÊÕÖĞ¶Ï//////////////////////////////////

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
    /////////½ÓÊÕLABEL274ĞÅºÅ/////////////////////////////
    if((JSDZJ&0xff)==0xbc)   
    { lz=0;
      flag16=1;
      if((JSDZJ&0x600)==0x600)  //274Í¨Ñ¶Õı³£
	  { if((JSDZJ&0x8000)==0x0000)  //·É»úÔÚµØÃæ
        { 
          flag6=flag6a;
          
          if((JSDZJ&0x4000)==0x4000)  //ÒªÇóµØÃæÎ¬»¤  
          { DMWH_BIT();
          }
	      else
	      { flag4=0;   //²»ÒªÇóµØÃæÎ¬»¤,Ä¬ÈÏÕı³£ 
	      }
        }
	    else
	    { flag6=1;     //·É»úÔÚ¿ÕÖĞ
	    }
	  }
    }
    //////////½ÓÊÕLABEL213¾²ÎÂĞÅºÅ//////////////////////////////////
    if((JSDZJ&0xff)==0x8b)
    { wd=0;
      flag18=1;
      if((JSDZJ&0x600)==0x600)   //213Í¨Ñ¶Õı³£ 
	  { if((JSGZJ&0x8000)==0x0000)  //ÎÂ¶ÈÎªÕı
        { JSGZJ=(JSGZJ&0x7ff0)>>4;
	      wd=(float)JSGZJ*0.25;
		  if((wd>=0.0)&&(wd<5.0))
		  { t2=250;
		    flag20=0; 
		  }
		  if(wd>=5.0)   
		  { GpioDataRegs.GPADAT.bit.GPIOA9=1;
            GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff;  //¹Ø±ÕËùÓĞ¼ÓÈÈ 
		    FSDZJ_272=FSDZJ_272&0xdfff; 
		    flag20=1; 
		  }              
	    }
	    else        //ÎÂ¶ÈÎª¸º
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
      tmp=JSDZJ&0xff;    //±£´æµ½Íâ²¿´æ´¢Æ÷ÖĞ
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

///////////////////¶ÌÂ·ÖĞ¶Ï//////////////////////////////////

interrupt void XINT2_isr(void)
{ 
  DINT;
  if(PieCtrl.PIEACK.bit.ACK1==1)
  { PieCtrl.PIEACK.bit.ACK1=1;   
    if((flag9==0)&&(flag10==0))  //ÖÜÆÚÑ²ì±
    { if(GpioDataRegs.GPADAT.bit.GPIOA9==0)
      { flag21=1;	     
	  }
    }
  }
  EINT;   
}

////////////////¹¤×÷/±¸·İÇĞ»//////////////////////////
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
   if((j==1)&&(flag1==0))  //±¾¿ØÖÆÆ÷Îª×óÖ÷,¾Ü¾øÇĞ»»
   { flag11=0;        //Ö÷¿ØÖÆÆ÷Õı³£
   }
   if((j==2)&&(flag1==0))  //±¾¿ØÖÆÆ÷Îª×ó¸¨
   { j=1;
     flag14=1;
   }
   
   if((j==2)&&(flag1==1))  //±¾¿ØÖÆÆ÷ÎªÓÒÖ÷,¾Ü¾øÇĞ»»
   { flag11=0;        //Ö÷¿ØÖÆÆ÷Õı³£
   }
   if((j==1)&&(flag1==1))  //±¾¿ØÖÆÆ÷ÎªÓÒ¸¨
   { j=2;
     flag14=1;
   }
    
   EvbRegs.EVBIMRA.bit.PDPINTB=1; 
   EvbRegs.EVBIFRA.bit.PDPINTB=1;
  } 
  EINT;
}

///////////Ö±½Ó·µ»ØÖĞ¶Ï·şÎñ³ÌĞò//////////////
interrupt void nothing()
{ EINT;
  return;
}

/////////////Ö÷³ÌĞò//////////////////////////////////////

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
  
   //¿ªÖĞ¶Ï
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

   FSDZJ_271=0x00b9;   //³õÊ¼»¯429ĞÅºÅ
   FSGZJ_271=0x0000;  
   FSDZJ_272=0x40ba;   //ÉÏµçÄ¬ÈÏ×óÖ÷¿Ø 
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
     
   ///////ÑÓÊ±2s////////////////////////////
   EvaRegs.T2CON.bit.TENABLE=1;
   while(1)
   { if(EvaTimer2InterruptCount>=20)
     {EvaTimer2InterruptCount=0;
	  break;
	 }
   } 
   EvaRegs.T2CON.bit.TENABLE=0;

   //////¶ÁÈ¡Ó²¼ş°æ±¾ºÅ/////////////////////////
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
     
   //////¶ÁÈ¡ÉÏ´Î×Ü¹ÊÕÏ´ÎÊı///////////////
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

   ///////¶ÁÈ¡ÉÏ´Î½áÊø±êÖ¾/////////////////////////   
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

   //×ó/ÓÒ¿ØÖÆÅĞ¶Ï//////////////////////
   if(GpioDataRegs.GPFDAT.bit.GPIOF13==1)  
   { flag1=1;         //ÓÒ¿ØÖÆÆ÷
   }
   else
   { flag1=0;        //×ó¿ØÖÆÆ÷	 
   }

   InitECan(); 
   ADC_adjust(); 
   poweron_BIT();  //ÉÏµç×Ô¼ì²â

   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0xffde;  //³õÊ¼»¯3282
   CSH=0x0020;
   rambase6=(Uint16 *)RAMBASE6;
   *rambase6=CSH;
   for(i=0;i<5;i++){}
   GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all|0x3f;
   arinc429fasong(); //429ÉÏ±¨×Ô¼ìĞÅÏ¢
   MBOX0Send();    //×óÓÒCAN½»Á÷
   
   if(flag3==0)  //ÉÏµç×Ô¼ìÕı³£
   {
      /////////Ó²¼ş°æ±¾ºÅ·¢ËÍ/////////////////////
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

      /////////Èí¼ş°æ±¾ºÅ·¢ËÍ//////////////////////
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

	  EvaRegs.T1CON.bit.TENABLE=1;  // ´ò¿ª¶¨Ê±Æ÷1,¿ÕÖĞ¹¤×÷

	  while(1)
	  { if(flag4==0) //µØÃæÎ¬»¤Õı³£
	    { if(count1>=18)     //429·¢ËÍ¼°CANÃ¿1.8s·¢ËÍÒ»´Î
		  { count1=0;
            arinc429fasong();  //429×ÜÏß·¢ËÍ
            MBOX0Send();	//CAN×ÜÏß·¢ËÍ  
			if(can1!=can2)
			{//////±£´æÎŞcanÊı¾İ´úÂë/////////////////
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

	        tmp=(char)gz&0xff;   //±£´æ×Ü¹ÊÕÏ´ÎÊı
			saveDA2();
            tmp=(char)gz&0xff00>>8;
            saveDA2();
            tmp=(char)gz&0xff0000>>16;
            saveDA2();
            tmp=(char)gz&0xff000000>>24;
            saveDA2();

	        tmp=FSDZJ_271&0xff;    //±£´æ429ĞÅºÅ
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
			{ //////²éÑ¯ÂÖÔØĞÅºÅ´ÎÊı/////////
		      if(flag16==1)
              { flag16=0;
			    lz=0;
			  }
			  else
			  {
			    error1++;
			    gz++;
			    lz++;	 
			    tmp=0x34;	 //±£´æÎŞÂÖÔØĞÅºÅ´úÂë
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
			  if(error1>=20)  //ÀÛ¼Æ20´ÎÎ´ÊÕµ½ÂÖÔØĞÅÏ¢,Ôò·É»úÔÚ¿ÕÖĞ
			  { flag6=1;
			    flag6a=1;	        
			  }
			  if(lz>=5)  //Á¬Ğø5´ÎÎ´ÊÕµ½ÂÖÔØĞÅÏ¢,Ôò·É»úÔÚ¿ÕÖĞ
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
			    tmp=0x33;	  //±£´æÎŞ213ĞÅºÅ´úÂë
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
			  if(error2>=20)  //ÀÛ¼Æ20´ÎÎ´ÊÕµ½¾²ÎÂĞÅÏ¢,Ôò·É»úÔÚÑÏ¿á»·¾³ÏÂ
			  { t2=380;	    
			  }
			  if(wd1>=5)  //Á¬Ğø5´ÎÎ´ÊÕµ½¾²ÎÂĞÅÏ¢,Ôò·É»úÔÚÑÏ¿á»·¾³ÏÂ
			  { t2=380;
			    wd1=0;
			  } 
	        }	        	   
	      }
		  		  
		  if(((GpioDataRegs.GPBDAT.bit.GPIOB15==1)&&(flag6==1))&&(flag20==0))                   
          { 
            if((j==1)&&(flag1==0))
		    { FSDZJ_272=FSDZJ_272|0x4000;       //×óÖ÷¿Ø
	          jiare();
		    }
		    if((j==2)&&(flag1==1))
		    { FSDZJ_272=FSDZJ_272&0xbfff;       //ÓÒÖ÷¿Ø
	          jiare();
		    }			
          }
          else
		  { GpioDataRegs.GPADAT.bit.GPIOA9=1;
		    flag22=0;
	        GpioDataRegs.GPADAT.all=GpioDataRegs.GPADAT.all&0x7ff; //¹Ø±ÕËùÓĞ¼ÓÈÈ
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





