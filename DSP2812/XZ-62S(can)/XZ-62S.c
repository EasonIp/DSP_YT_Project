

#include "DSP28_Device.h"
#include "math.h"


#define RAMBASE1 0x080000      //×ÜÏßµØÖ·,¶ÔÓ¦ÓÚÆ¬Ñ¡ÐÅºÅXZCS2 

#define ID1 0x01200000       //×ó¿ØÖÆÆ÷·¢ËÍµ½ÓÒ¿ØÖÆÆ÷MSGID
#define ID2 0x01300000       //×ó¿ØÖÆÆ÷·¢ËÍµ½ÊÔÑéÆ÷MSGID
#define ID3 0x02100000       //ÓÒ¿ØÖÆÆ÷·¢ËÍµ½×ó¿ØÖÆÆ÷MSGID
#define ID4 0x03100000       //ÊÔÑéÆ÷·¢ËÍµ½×ó¿ØÖÆÆ÷MSGID


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


////////////////±äÁ¿ÉêÃ÷/////////////////////////////////               
Uint32 i=0;                    //ÑÓÊ±±äÁ¿
Uint32 j[6]={0,0,0,0,0,0};     
Uint16 Write_tmp=0;			    //¶¨ÒåÍâ²¿½Ó¿ÚÐ´²Ù×÷±äÁ¿
Uint16 Read_tmp=0;              //¶¨ÒåÍâ²¿½Ó¿Ú¶Á²Ù×÷±äÁ¿
Uint16 *rambase1;              //¶¨ÒåZone6Íâ²¿½Ó¿ÚÖ¸Õë±äÁ¿
Uint16 LowWord_271_Send;        //·¢ËÍLABLE271µÍ×Ö½Ú
Uint16 HighWord_271_Send;       //·¢ËÍLABLE271¸ß×Ö½Ú
Uint16 LowWord_272_Send;        //·¢ËÍLABLE272µÍ×Ö½Ú
Uint16 HighWord_272_Send;        //·¢ËÍLABLE272¸ß×Ö½Ú
Uint16 LowWord_273_Send;         //·¢ËÍLABLE273µÍ×Ö½Ú
Uint16 HighWord_273_Send;        //·¢ËÍLABLE273¸ß×Ö½Ú
Uint16 LowWord_300_Send;          //·¢ËÍÈí¼þ°æ±¾ºÅ300µÍ×Ö½Ú
Uint16 HighWord_300_Send;         //·¢ËÍÈí¼þ°æ±¾ºÅ300¸ß×Ö½Ú
Uint16 LowWord_301_Send;          //·¢ËÍÈí¼þ°æ±¾ºÅ301µÍ×Ö½Ú
Uint16 HighWord_301_Send;         //·¢ËÍÈí¼þ°æ±¾ºÅ301¸ß×Ö½Ú
Uint16 LowWord_302_Send;         //·¢ËÍÈí¼þ°æ±¾ºÅ302µÍ×Ö½Ú
Uint16 HighWord_302_Send;         //·¢ËÍÈí¼þ°æ±¾ºÅ302¸ß×Ö½Ú
Uint16 LowWord_303_Send;          //·¢ËÍÈí¼þ°æ±¾ºÅ303µÍ×Ö½Ú
Uint16 HighWord_303_Send;         //·¢ËÍÈí¼þ°æ±¾ºÅ303¸ß×Ö½Ú
Uint16 LowWord_304_Send;           //·¢ËÍÓ²¼þ°æ±¾ºÅ304µÍ×Ö½Ú
Uint16 HighWord_304_Send;          //·¢ËÍÓ²¼þ°æ±¾ºÅ304¸ß×Ö½Ú
Uint16 LowWord_305_Send;           //·¢ËÍÓ²¼þ°æ±¾ºÅ305µÍ×Ö½Ú
Uint16 HighWord_305_Send;          //·¢ËÍÓ²¼þ°æ±¾ºÅ305¸ß×Ö½Ú
Uint16 LowWord_306_Send;           //·¢ËÍÓ²¼þ°æ±¾ºÅ306µÍ×Ö½Ú
Uint16 HighWord_306_Send;          //·¢ËÍÓ²¼þ°æ±¾ºÅ306¸ß×Ö½Ú
Uint16 LowWord_307_Send;           //·¢ËÍÓ²¼þ°æ±¾ºÅ307µÍ×Ö½Ú
Uint16 HighWord_307_Send;          //·¢ËÍÓ²¼þ°æ±¾ºÅ307¸ß×Ö½Ú
Uint16 LowWord_310_Send;           //·¢ËÍÓ²¼þ°æ±¾ºÅ310µÍ×Ö½Ú
Uint16 HighWord_310_Send;          //·¢ËÍÓ²¼þ°æ±¾ºÅ310¸ß×Ö½Ú
Uint32 TestMbox1=0x0;			   //ÓÊÏä2½ÓÊÕµ½µÄÐÅÏ¢
Uint32 TestMbox2=0x0;     
Uint32 TestMbox3=0x0;			   //ÓÊÏä3½ÓÊÕµ½µÄÐÅÏ¢
Uint32 TestMbox4=0x0;  
    
char  Flag_Controller_Judge=0;
/////////////////º¯ÊýÉêÃ÷/////////////////////////////////

void InitSysCtrl(void);
void InitGpio(void);
void InitXintf(void);
void InitECan(void);
void VersionNumber_Hardware_Get(void);
void Data1_Save(void);
void Data1_Read(void);
interrupt void ECAN0_ISR(void);


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
   for(i=0; i<5000; i++){}   // ÎªÁËÆµÂÊÎÈ¶¨¬±ØÐëÑÓ»
      
   SysCtrlRegs.HISPCP.all = 0x0002;  //¸ßËÙÍâÉèÊ±ÖÓ=120/4=30MHz
   SysCtrlRegs.LOSPCP.all = 0x0002;	 //µÍËÙÍâÉèÊ±ÖÓ=120/4=30MHz
 
   SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;  //Ê¹ÄÜEVAÄ£¿éµÄ¸ßËÙÍâÉèÊ±ÖÓ
   SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;  //Ê¹ÄÜEVBÄ£¿éµÄ¸ßËÙÍâÉèÊ±ÖÓ
   SysCtrlRegs.PCLKCR.bit.ADCENCLK=1;  //Ê¹ÄÜADCÄ£¿éµÄ¸ßËÙÍâÉèÊ±ÖÓ
   SysCtrlRegs.PCLKCR.bit.ECANENCLK=1; //Ê¹ÄÜCANÍâÉèµÄÏµÍ³Ê±ÖÓ
				
   EDIS;	
}

///////////////////GPIO¿Ú³õÊ¼»¯//////////////////////////////////
void InitGpio(void)
{
   EALLOW;
   GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOAÉèÖÃÎªI/O¿Ú 
   GpioMuxRegs.GPADIR.all=0xffff;   //A0-A15ÉèÖÃÎªÊä³ö	
   GpioDataRegs.GPADAT.all=0x23e; 

   GpioMuxRegs.GPBMUX.all=0x0000;    //GPIOBÉèÖÃÎªI/O¿Ú 
   GpioMuxRegs.GPBDIR.all=0xefdf;    //B5,B12ÉèÖÃÎªÊäÈë£¬ÆäÓàÉèÖÃÎªÊä³ö	
   GpioDataRegs.GPBDAT.all=0x8221;   

    
   GpioMuxRegs.GPFMUX.all=0x00c0;  //ÉèÖÃGPIOF6/7ÎªeCAN¿Ú,ÆäÓàÉèÖÃÎªI/O¿Ú
   GpioMuxRegs.GPFDIR.all=0xa00;
   GpioDataRegs.GPFDAT.bit.GPIOF9=1;
   GpioDataRegs.GPFDAT.bit.GPIOF11=0;
   EDIS;
}	

	
////////////////////Íâ²¿½Ó¿ÚXINTF³õÊ¼»¯/////////////////////////
void InitXintf(void)
{  
    XintfRegs.XINTCNF2.bit.XTIMCLK = 0x0001;  //ËùÓÐzoneµÄÊ±ÐòÊÇ»ùÓÚXTIMCLK = SYSCLKOUT/2 
    XintfRegs.XINTCNF2.bit.WRBUFF = 3;   //ÉèÖÃ3¸öÐ´»º³å
	XintfRegs.XINTCNF2.bit.CLKOFF = 0;  //Ê¹ÄÜXCLKOUT
	XintfRegs.XINTCNF2.bit.CLKMODE = 0;  //ÉèÖÃXCLKOUT=XTIMCLK
    	
	XintfRegs.XTIMING2.bit.X2TIMING=1;
	//Zone2Ð´Ê±Ðò
	XintfRegs.XTIMING2.bit.XWRLEAD = 1;    //Ð´²Ù×÷Òýµ¼½×¶ÎµÄÖÜÆÚ=2*XTIMCLK
	XintfRegs.XTIMING2.bit.XWRACTIVE = 7;   //Ð´²Ù×÷¼¤»î½×¶ÎµÈ´ý×´Ì¬µÄÖÜÆÚ=14*XTIMCLK
	XintfRegs.XTIMING2.bit.XWRTRAIL = 2;    //Ð´²Ù×÷¸ú×Ù½×¶ÎµÄÖÜÆÚ=4*XTIMCLK
   //Zone2¶ÁÊ±Ðò
	XintfRegs.XTIMING2.bit.XRDLEAD = 1;    //¶Á²Ù×÷Òýµ¼½×¶ÎµÄÖÜÆÚ=2*XTIMCLK
	XintfRegs.XTIMING2.bit.XRDACTIVE=7;	    //¶Á²Ù×÷¼¤»î½×¶ÎµÈ´ý×´Ì¬µÄÖÜÆÚ=14*XTIMCLK
	XintfRegs.XTIMING2.bit.XRDTRAIL = 2;   //¶Á²Ù×÷¸ú×Ù½×¶ÎµÄÖÜÆÚ=4*XTIMCLK
	//²»¼Ó±¶Ê±Ðò
	XintfRegs.XTIMING2.bit.X2TIMING = 0; 
   //Zone2²»¶ÔREADYÐÅºÅ²ÉÑù
    XintfRegs.XTIMING2.bit.USEREADY = 0;
    XintfRegs.XTIMING2.bit.READYMODE = 0;
    XintfRegs.XTIMING2.bit.XSIZE = 3;   //16Î»½Ó¿Ú£¬Î¨Ò»ÓÐÐ§µÄÉèÖÃ

    XintfRegs.XBANK.bit.BANK = 2;   //Ê¹ÄÜÄÚ²¿ÇøÓò2
    XintfRegs.XBANK.bit.BCYC = 3;  //Á¬Ðø·ÃÎÊÖ®¼äÌí¼ÓÁË3¸öXTIMCLKÖÜÆÚ

	//Zone6Ð´Ê±Ðò
	XintfRegs.XTIMING6.bit.XWRLEAD = 1;    //Ð´²Ù×÷Òýµ¼½×¶ÎµÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING6.bit.XWRACTIVE = 7;   //Ð´²Ù×÷¼¤»î½×¶ÎµÈ´ý×´Ì¬µÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING6.bit.XWRTRAIL = 3;    //Ð´²Ù×÷¸ú×Ù½×¶ÎµÄÖÜÆÚ=XTIMCLK
   //Zone6¶ÁÊ±Ðò
	XintfRegs.XTIMING6.bit.XRDLEAD = 1;    //¶Á²Ù×÷Òýµ¼½×¶ÎµÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING6.bit.XRDACTIVE=1;	    //¶Á²Ù×÷¼¤»î½×¶ÎµÈ´ý×´Ì¬µÄÖÜÆÚ=XTIMCLK
	XintfRegs.XTIMING6.bit.XRDTRAIL = 1;   //¶Á²Ù×÷¸ú×Ù½×¶ÎµÄÖÜÆÚ=XTIMCLK
	//²»¼Ó±¶Ê±Ðò
	XintfRegs.XTIMING6.bit.X2TIMING = 0; 
   //Zone6²»¶ÔREADYÐÅºÅ²ÉÑù
    XintfRegs.XTIMING6.bit.USEREADY = 0;
    XintfRegs.XTIMING6.bit.READYMODE = 0;
    XintfRegs.XTIMING6.bit.XSIZE = 3;   //16Î»½Ó¿Ú£¬Î¨Ò»ÓÐÐ§µÄÉèÖÃ

    XintfRegs.XBANK.bit.BANK = 6;   //Ê¹ÄÜÄÚ²¿ÇøÓò6
    XintfRegs.XBANK.bit.BCYC = 3;  //Á¬Ðø·ÃÎÊÖ®¼äÌí¼ÓÁË3¸öXTIMCLKÖÜÆÚ
}


////////////////////ECAN³õÊ¼»¯/////////////////////////
void InitECan(void)
{
	EALLOW;
	ECanaRegs.CANTIOC.bit.TXFUNC = 1;
	ECanaRegs.CANRIOC.bit.RXFUNC = 1;
	EDIS;
		
	ECanaRegs.CANME.all = 0;  //½ûÖ¹ùÓÐÓÊÏä

      ECanaRegs.CANMD.bit.MD0=0;  //ÓÊÏä0·¢ËÍµ½ÓÒ¿ØÖÆÆ÷
      ECanaRegs.CANMD.bit.MD1=0;  //ÓÊÏä1·¢ËÍµ½ÊÔÑéÆ÷
      ECanaRegs.CANMD.bit.MD2=1;  //ÓÊÏä2½ÓÊÕÓÒ¿ØÖÆÆ÷ÐÅÏ¢
      ECanaRegs.CANMD.bit.MD3=1;  //ÓÊÏä3½ÓÊÕÊÔÑéÆ÷ÐÅÏ¢
     	
	if(Flag_Controller_Judge==0)
	{ ECanaRegs.CANMD.bit.MD0=0;  //ÓÊÏä0·¢ËÍµ½ÓÒ¿ØÖÆÆ÷
      ECanaRegs.CANMD.bit.MD1=0;  //ÓÊÏä1·¢ËÍµ½ÊÔÑéÆ÷
      ECanaRegs.CANMD.bit.MD2=1;  //ÓÊÏä2½ÓÊÕÓÒ¿ØÖÆÆ÷ÐÅÏ¢
      ECanaRegs.CANMD.bit.MD3=1;  //ÓÊÏä3½ÓÊÕÊÔÑéÆ÷ÐÅÏ¢
     	
	  ECanaMboxes.MBOX0.MID.all = ID1;	
	  ECanaMboxes.MBOX1.MID.all = ID2;
	  ECanaMboxes.MBOX2.MID.all = ID3;
	  ECanaMboxes.MBOX3.MID.all = ID4;
	}
    else
	{ ECanaRegs.CANMD.bit.MD0=0;  //ÓÊÏä0·¢ËÍµ½×ó¿ØÖÆÆ÷
      ECanaRegs.CANMD.bit.MD2=1;  //ÓÊÏä2½ÓÊÕ×ó¿ØÖÆÆ÷ÐÅÏ¢
    	
	  ECanaMboxes.MBOX0.MID.all = ID3;	
	  ECanaMboxes.MBOX2.MID.all = ID1;
	}
			
	ECanaRegs.CANME.bit.ME0=1;	//ÓÊÏäÊ¹ÄÜ
    ECanaRegs.CANME.bit.ME1=1;
    ECanaRegs.CANME.bit.ME2=1;
	ECanaRegs.CANME.bit.ME3=1;
		
	ECanaMboxes.MBOX0.MCF.bit.DLC = 8;	//Êý¾Ý³¤¶È 8¸öBYTE
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
	ECanaRegs.CANMC.bit.STM = 0; //Õý³£¹¤×÷Ä£Ê½ 
	ECanaRegs.CANMC.bit.SCM = 0; //sccÄ£Ê½
    ECanaRegs.CANMC.bit.DBO = 0; //Ê×ÏÈ½ÓÊÕ»ò·¢ËÍÊý¾ÝµÄ×î¸ß×Ö½Ú
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


/////////////Í¨¹ý×ÜÏßÏòÍâ²¿´æ´¢Æ÷Ð´Ó²¼þ°æ±¾ºÅ////////////////////
void Data1_Save(void)
{  
   *rambase1=Write_tmp&0xff; 	//½«±äÁ¿Read_tmpÐ´ÈëÊý¾Ý×ÜÏßÉÏ
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


/////////////Í¨¹ý×ÜÏß¶ÁÈ¡ÉÏ´Î¹ÊÕÏ´ÎÊý////////////////////
void Data1_Read(void)
{  
   Read_tmp=(*rambase1)&0xff;       //½«Êý¾Ý×ÜÏßÉÏµÄÊý¾Ý¶Áµ½±äÁ¿Read_tmpÖÐ
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

///////////////¶ÁÈ¡Ó²¼þ°æ±¾ºÅ///////////////////////////////
void VersionNumber_Hardware_Get(void)
{  
   Uint16 Version_Number1=0;         //Ô¤´æÓ²¼þ°æ±¾ºÅµÚ1Î»
   Uint16 Version_Number2=0;         //Ô¤´æÓ²¼þ°æ±¾ºÅµÚ2Î»
   Uint16 Version_Number3=0;         //Ô¤´æÓ²¼þ°æ±¾ºÅµÚ3Î»
   Uint16 Version_Number4=0;         //Ô¤´æÓ²¼þ°æ±¾ºÅµÚ4Î»
   Uint16 Version_Number5=0;         //Ô¤´æÓ²¼þ°æ±¾ºÅµÚ5Î»
   Uint16 Version_Number6=0;         //Ô¤´æÓ²¼þ°æ±¾ºÅµÚ6Î»
   Uint16 Version_Number7=0;         //Ô¤´æÓ²¼þ°æ±¾ºÅµÚ7Î»
   
   DINT;
   GpioDataRegs.GPBDAT.bit.GPIOB8=1;
   GpioDataRegs.GPBDAT.bit.GPIOB9=0;   //´ò¿ª¶ÁÐ´²Ù×÷B9=0
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
   GpioDataRegs.GPBDAT.bit.GPIOB9=1;   //¹Ø±Õ¶ÁÐ´²Ù×÷B9=1
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



///////////////////CANÖÐ¶ÏÓÊÕÐÅÏ?//////////////////////////////
interrupt void ECAN0_ISR(void)
{   
  DINT;
  if(PieCtrl.PIEACK.bit.ACK9==1)
  { PieCtrl.PIEACK.bit.ACK9=1;

   //////////////////////////////////////////////////////////////
    if(ECanaRegs.CANRMP.bit.RMP3==1)  //½ÓÊÕÊÔÑéÆ÷ÐÅÏ¢
    { ECanaRegs.CANRMP.bit.RMP3=1;
     
       TestMbox3=ECanaMboxes.MBOX3.MDRL.all;
       TestMbox4=ECanaMboxes.MBOX3.MDRH.all;
        
         //////½ÓÊÕµ½Ó²¼þ°æ±¾ºÅ//////////////
        if((TestMbox4&0xff000000)==0x58000000)
		{ //////////Ó²¼þ°æ±¾ºÅ±£´æ///////////////////
		  GpioDataRegs.GPBDAT.bit.GPIOB8=1;
		  GpioDataRegs.GPBDAT.bit.GPIOB9=0;   //´ò¿ª¶ÁÐ´²Ù×÷B9=0		  
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
          	  
		  //GpioDataRegs.GPBDAT.bit.GPIOB9=1;   //´ò¿ª¶ÁÐ´²Ù×÷B9=0 
		  //GpioDataRegs.GPBDAT.bit.GPIOB8=0;           
		}	    
    }	
   }
   EINT;
}



/////////////Ö÷³ÌÐò//////////////////////////////////////

main()
{  InitSysCtrl();
	//°ÑInitFlash£¨£©º¯Êý°áÒÆµ½ramÖÐÔËÐÐ
   MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);	
   InitFlash();

   DINT;
   IER = 0x0000;
   IFR = 0x0000;

   InitPieCtrl();
   InitPieVectTable();
   InitGpio();	
   InitXintf();
   //×ó/ÓÒ¿ØÖÆÅÐ¶Ï//////////////////////
   if(GpioDataRegs.GPFDAT.bit.GPIOF13==1)  
   { Flag_Controller_Judge=1;         //ÓÒ¿ØÖÆÆ÷
   }
   else
   { Flag_Controller_Judge=0;        //×ó¿ØÖÆÆ÷	 
   } 


   InitECan(); 
    		
   EALLOW;
   PieVectTable.ECAN0INTA = &ECAN0_ISR;
   EDIS;  
  
   //¿ªÖÐ¶Ï
   IER |= M_INT9;
   PieCtrl.PIEIER9.bit.INTx5=1;
   EINT;  
   ERTM;

   LowWord_271_Send=0x00b9;   //³õÊ¼»¯429ÐÅºÅ
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
     
    //////¶ÁÈ¡Ô¤ÖÃÓ²¼þ°æ±¾ºÅ///////////////
   VersionNumber_Hardware_Get();
         
   for(;;){}

}