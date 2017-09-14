/* 
   * Ö±Éý»úÐýÒí·À³ý±ù¿ØÖÆÆ÷
   * ÎÄ¼þÃû£ºXZ-XX.pit
   * Ê±¼ä£º2010.6.1
   * °æ±¾£ºµÚ¶þ°æ
   
*/

#include "DSP28_Device.h"
#include "math.h"

///////±äÁ¿ÉêÃ÷///////////////
int i,j;
Uint32  count1=0;
int channel;
int	a[16];
float AD0=0.0,AD1=0.0,adclo=0.0,cesi;
float LWC,Temperature,ONTIMER,OFFTIMER,ONTIMER1,OFFTIMER1;
int ONT,OFT;
int n,max,a1;
char flag=0;  


////////º¯ÊýÉêÃ÷/////////////
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

   SysCtrlRegs.PLLCR = 0x3;   //Ö÷Æµ=30*3/2=45MHz
  
   for(i= 0; i< 5000; i++){}   // ÎªÁËÆµÂÊÎÈ¶¨£¬±ØÐëÑÓÊ±Ò»¶ÎÊ±¼ä
       
   SysCtrlRegs.HISPCP.all = 0x0002;  //¸ßËÙÍâÉèÊ±ÖÓ=45/4
   SysCtrlRegs.LOSPCP.all = 0x0002;	 //µÍËÙÍâÉèÊ±ÖÓ=45/4
 
   SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;  //Ê¹ÄÜEVAÄ£¿éµÄ¸ßËÙÍâÉèÊ±ÖÓ
   SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;  //Ê¹ÄÜEVBÄ£¿éµÄ¸ßËÙÍâÉèÊ±ÖÓ
   SysCtrlRegs.PCLKCR.bit.ADCENCLK=1;  //Ê¹ÄÜADCÄ£¿éµÄ¸ßËÙÍâÉèÊ±ÖÓ
   SysCtrlRegs.PCLKCR.bit.SCIENCLKA=1; //Ê¹ÄÜSCIAÄ£¿éµÄµÍËÙÍâÉèÊ±ÖÓ
				
   EDIS;
	
}

///////////////////IO¿Ú³õÊ¼»¯//////////////////////////////////

void InitGpio(void)
{

     EALLOW;
     GpioMuxRegs.GPAMUX.all=0x0000;   //GPIOAÉèÖÃÎªI/O¿Ú 
     GpioMuxRegs.GPADIR.all=0x3F;   //A0-A5ÉèÖÃÎªÊä³ö£¬A6-A15ÉèÖÃÎªÊäÈë	
     //GpioDataRegs.GPADAT.bit.GPIOA0=1;
     //GpioDataRegs.GPADAT.bit.GPIOA1=1;
     //GpioDataRegs.GPADAT.bit.GPIOA2=1;
     //GpioDataRegs.GPADAT.bit.GPIOA5=1;

     GpioMuxRegs.GPBMUX.all=0x0000;    //GPIOBÉèÖÃÎªI/O¿Ú 
     GpioMuxRegs.GPBDIR.all=0x3F;   //B0-B5ÉèÖÃÎªÊä³ö£¬B6-B7ÉèÖÃÎªÊäÈë	
     GpioDataRegs.GPBDAT.bit.GPIOB0=1;
     GpioDataRegs.GPBDAT.bit.GPIOB1=1;
	 GpioDataRegs.GPBDAT.bit.GPIOB2=1;
	 GpioDataRegs.GPBDAT.bit.GPIOB3=1;
	 //GpioDataRegs.GPBDAT.bit.GPIOB4=1;
	 GpioDataRegs.GPBDAT.bit.GPIOB5=1;
     EDIS;

}	


///////////////////¶¨Ê±Æ÷1³õÊ¼»¯£¨50mS£©//////////////////////////////////

void init_eva_timer1(void)
{
  
    EvaRegs.GPTCONA.all = 0;   //³õÊ¼»¯¶¨Ê±Æ÷1²¢ÉèÖÃ¶¨Ê±Æ÷1¼Ä´æÆ÷
    EvaRegs.EVAIMRA.bit.T1PINT = 1;  //ÔÊÐí¶¨Ê±Æ÷1ÖÜÆÚÖÐ¶Ï
    EvaRegs.EVAIFRA.bit.T1PINT = 1;  //Çå³ý¶¨Ê±Æ÷1ÖÜÆÚÖÐ¶Ï±êÖ¾
             
    EvaRegs.T1CON.all = 0x170C;  //Timer1ÎªÁ¬ÐøÔö¼ÆÊýÄ£Ê½£¬Ô¤·ÖÆµÖµÎª128£¬
	                             //Ê¹ÓÃÄÚ²¿¸ßËÙÍâÉèÊ±ÖÓ
	EvaRegs.T1PR = 0x112a;   //Timer1µÄÖÜÆÚ¼Ä´æÆ÷=0.05*45MHz/4/128
	EvaRegs.T1CNT = 0x0000;  //Timer1µÄ¼ÆÊýÆ÷ÇåÁã

}

///////////////////¶¨Ê±Æ÷1ÖÐ¶Ï//////////////////////////////////

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


///////////////////AD³õÊ¼»¯//////////////////////////////////
void InitAdc(void)
{
		
	AdcRegs.ADCTRL1.bit.RESET=1;  //¸´Î»ADCÄ£¿é
    for(i=0;i<5000;i++)	{}
	AdcRegs.ADCTRL1.bit.RESET=0;
	AdcRegs.ADCTRL1.bit.SUSMOD=3;
	AdcRegs.ADCTRL1.bit.ACQ_PS=0;
	AdcRegs.ADCTRL1.bit.CPS=0;     //ADCCLK=HSPCLK/2
	AdcRegs.ADCTRL1.bit.CONT_RUN=0; //ADÎªÆô¶¯/Í£Ö¹Ä£Ê½
	AdcRegs.ADCTRL1.bit.SEQ_CASC=1; //16¼¶ÁªÄ£Ê½
  	
	AdcRegs.ADCTRL3.bit.ADCBGRFDN=3; //´ø¼äÏ¶²Î¿¼µçÂ·ÉÏµç
	for(i=0;i<10000;i++)  {}          //µÈ´ýÎÈ¶¨
	AdcRegs.ADCTRL3.bit.ADCPWDN=1;  //ÆäËüµçÂ·ÉÏµç
	for(i=0;i<5000;i++)	 {}           //µÈ´ýÎÈ¶¨
	AdcRegs.ADCTRL3.bit.ADCCLKPS=15;
	AdcRegs.ADCTRL3.bit.SMODE_SEL=0;  //Ë³Ðò²ÉÑù

	
	AdcRegs.MAX_CONV.bit.MAX_CONV=15;  //²ÉÑùÍ¨µÀÎª16
  
    AdcRegs.CHSELSEQ1.all= channel;   //Í¬Ò»¸öÍ¨µÀ²É16´Î
    AdcRegs.CHSELSEQ2.all= channel;
    AdcRegs.CHSELSEQ3.all= channel;
    AdcRegs.CHSELSEQ4.all= channel;

	AdcRegs.ADCTRL2.all=0x8c04;   //¿ÉÓÃEVBµÄÒ»¸öÊÂ¼þÐÅºÅ´¥·¢AD×ª»»

	////////////²ÉÓÃ¶¨Ê±Æ÷4Æô¶¯AD×ª»»/////////////////////
    EvbRegs.GPTCONB.all=0x400;    //¶¨Ê±Æ÷4ÖÜÆÚÖÐ¶ÏÆô¶¯AD×ª»»
    EvbRegs.EVBIMRB.bit.T4PINT = 1;  //ÔÊÐí¶¨Ê±Æ÷4ÖÜÆÚÖÐ¶Ï
    EvbRegs.EVBIFRB.bit.T4PINT = 1;  //Çå³ý¶¨Ê±Æ÷4ÖÜÆÚÖÐ¶Ï±êÖ¾
             
    EvbRegs.T4CON.all = 0x170C;  //Timer4ÎªÁ¬ÐøÔö¼ÆÊýÄ£Ê½£¬Ô¤·ÖÆµÖµÎª128£¬
	                             //Ê¹ÓÃÄÚ²¿¸ßËÙÍâÉèÊ±ÖÓ
	EvbRegs.T4PR = 0x57;     //Timer4µÄÖÜÆÚ¼Ä´æÆ÷=0.001*45MHz/4/128
	EvbRegs.T4CNT = 0x0000;  //Timer4µÄ¼ÆÊýÆ÷ÇåÁã
				
	AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;
	AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;	
			
}	

///////////////////ADÖÐ¶Ï/////////////////////////////////

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
	 for(i=0;i<12;i++)  //È¥Í·Î²¸÷2¸öÈ¡Æ½¾ù
	 { a1=a1+a[i+2];
	 }
     a1=a1/12;
	 cesi=a1*3.0/4095+adclo;

	/////////·¢ËÍ´®¿ÚÊý¾Ý//////////////////////
     SciaRegs.SCITXBUF= a1&0xff00;  //·¢ËÍ¸ß8Î»
     SciaRegs.SCITXBUF= a1&0xff;   //·¢ËÍµÍ8Î»
    ///////////////////////////////////////////

	 AdcRegs.ADC_ST_FLAG.bit.INT_SEQ1_CLR=1;  //Çå³ýÖÐ¶Ï±êÖ¾
	 AdcRegs.ADC_ST_FLAG.bit.INT_SEQ2_CLR=1;	
   }
   EINT;	
}

/////////////////Ê±¼ä¼ÆËã////////////////////////////////////////

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
  {  GpioDataRegs.GPADAT.bit.GPIOA1=0;  //Êä³öÎÂ¶È¿ØÖÆÐÅºÅ
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

/////////////////Ê±Ðò¼ÓÈÈ////////////////////////////////////////

void PWM()
{ 
   //GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //´ò¿ª¼ÓÈÈ¿ØÖÆ
   j=0;
   EvaRegs.T1CON.bit.TENABLE=1;   //´ò¿ª¶¨Ê±Æ÷1
   ONT=40;
   OFT=600;
   while(1)
   {if((0<count1)&&(count1<=ONT))
    { GpioDataRegs.GPBDAT.bit.GPIOB3=0;	   //ÐýÒí¼ÓÈÈ¿ØÖÆ
	  GpioDataRegs.GPBDAT.bit.GPIOB5=0;	   //Î²½°¼ÓÈÈ¿ØÖÆ
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
    
    EvaRegs.T1CON.bit.TENABLE=0;   //¹Ø±Õ¶¨Ê±Æ÷1

}


///////////////////â²¿ÖÐ¶Ï1³õÊ¼»¯//////////////////////////////////

void XINT1(void)
{ GpioMuxRegs.GPEMUX.bit.XINT1_XBIO_GPIOE0=1;   //ÉèÖÃÍâ²¿ÖÐ¶Ï1¹¦ÄÜ
  XIntruptRegs.XINT1CR.all=0x0;   //ÏÂ½µÑØÉú³ÉÖÐ¶Ï
}


///////////////////Íâ²¿ÖÐ¶Ï1ÖÐ¶Ï//////////////////////////////////

interrupt void XINT1_isr(void)
{ 
  DINT;
  PieCtrl.PIEACK.bit.ACK1=1;   //ÔÊÐí½øÈëÍ¬×éÆäËüÖÐ¶Ï
  EINT;

  /////////ÎÂ¶È´«¸ÐÆ÷¹ÊÕÏ£¨¸ø¶¨ÎÂ¶È-20£©//////////////////////////
  /*if( GpioDataRegs.GPADAT.bit.GPIOA15==1)
  { while(1)
	{ if(GpioDataRegs.GPADAT.bit.GPIOA6==1) //½á±ù¸æ¾¯
	  {if(GpioDataRegs.GPADAT.bit.GPIOA12==1)  //²éÑ¯¼ÓÈÈµçÔ´ÊÇ·ñ½ÓÍ¨
	   {  
         channel=0x1111;    //ADCINA1²É¼¯½á±ùËÙÂÊ
         InitAdc();

	     EvbRegs.T4CON.bit.TENABLE=1;  //´ò¿ª¶¨Ê±Æ÷4¿ªÊ¼²ÉÑù

	     while(1)
	     {if(i==12)
	       break;
	     }
         i=0;
         AD1=cesi;
         EvbRegs.T4CON.bit.TENABLE=0;  //¹Ø±Õ¶¨Ê±Æ÷4

         Count();
		 ONT=13;
		 PWM();
       }
      }
     }    
   }
  
   /////////½á±ùËÙÂÊÌ½²â¹ÊÕÏ£¨ÊÖ¶¯Èýµµ£©/////////////////////////
   if( GpioDataRegs.GPADAT.bit.GPIOA7==1)
   { while(1)
	{ if(GpioDataRegs.GPADAT.bit.GPIOA6==1) //½á±ù¸æ¾¯
	  {if(GpioDataRegs.GPADAT.bit.GPIOA12==1)  //²éÑ¯¼ÓÈÈµçÔ´ÊÇ·ñ½ÓÍ¨
	   { 
	     channel=0x0000;    //ADCINA0²É¼¯´óÆøÎÂ¶È
         InitAdc();

	     EvbRegs.T4CON.bit.TENABLE=1;  //´ò¿ª¶¨Ê±Æ÷4¿ªÊ¼²ÉÑù

	     while(1)
	     {if(i==12)
	       break;
	     }
         i=0;
         AD1=cesi;
         EvbRegs.T4CON.bit.TENABLE=0;  //¹Ø±Õ¶¨Ê±Æ÷4
         
         channel=0x1111;    //ADCINA1²É¼¯ÊÖ¶¯ÈýµµµçÑ¹
         InitAdc();

	     EvbRegs.T4CON.bit.TENABLE=1;  //´ò¿ª¶¨Ê±Æ÷4¿ªÊ¼²ÉÑù

	     while(1)
	     {if(i==12)
	       break;
	     }
         i=0;
         AD1=cesi;
         EvbRegs.T4CON.bit.TENABLE=0;  //¹Ø±Õ¶¨Ê±Æ÷4

         Count();
		 PWM();
       }
      }
     }    
   }
  
  //////////ÐýÒí¼ÓÈÈÊ±Ðò¹ÊÕÏ»òÐýÒí¼ÓÈÈµçÁ÷¹ÊÕÏ////////////////////////////
   if((GpioDataRegs.GPBDAT.bit.GPIOB7==1)|(GpioDataRegs.GPADAT.bit.GPIOA2==0))
   { while(1)
     { GpioDataRegs.GPBDAT.bit.GPIOB3=1;  //¶Ï¿ªÐýÒí¼ÓÈÈ¿ØÖÆ 
      
	  ///////////Ì½²â×ÓÏµÍ³Õý³£¹¤×÷///////////////////////
        channel=0x0000;    //ADCINA0²É¼¯´óÆøÎÂ¶È
         InitAdc();

	     EvbRegs.T4CON.bit.TENABLE=1;  //´ò¿ª¶¨Ê±Æ÷4¿ªÊ¼²ÉÑù

	     while(1)
	     {if(i==12)
	       break;
	     }
         i=0;
         AD1=cesi;
         EvbRegs.T4CON.bit.TENABLE=0;  //¹Ø±Õ¶¨Ê±Æ÷4
         
         channel=0x1111;    //ADCINA1²É¼¯ÊÖ¶¯ÈýµµµçÑ¹
         InitAdc();

	     EvbRegs.T4CON.bit.TENABLE=1;  //´ò¿ª¶¨Ê±Æ÷4¿ªÊ¼²ÉÑù

	     while(1)
	     {if(i==12)
	       break;
	     }
         i=0;
         AD1=cesi;
         EvbRegs.T4CON.bit.TENABLE=0;  //¹Ø±Õ¶¨Ê±Æ÷4
	 }
   }
  
  //////////ÎÂ¶È´óÓÚ20¶È£¨Í£Ö¹¹¤×÷£©////////////////////////////////
  if(GpioDataRegs.GPADAT.bit.GPIOA1==0)
  { while(1) 
    { GpioDataRegs.GPBDAT.bit.GPIOB4=1;  //¶Ï¿ª¼ÓÈÈ¿ØÖÆµçÔ´ 
    }
  }
 
 //////////Î²½°¼ÓÈÈµçÁ÷¹ÊÕÏ///////////////////////////////////////
  if(GpioDataRegs.GPBDAT.bit.GPIOB6==1)
  { while(1)
    {GpioDataRegs.GPBDAT.bit.GPIOB5=1;  //¶Ï¿ªÎ²½°¼ÓÈÈ¿ØÖÆ 
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
  
  /*¿ªÖÐ¶Ï*/
  IER |= M_INT1;
  PieCtrl.PIEIER1.bit.INTx4=1;

  IER |= M_INT1;
  PieCtrl.PIEIER1.bit.INTx6=1;

  IER |= M_INT2;
  PieCtrl.PIEIER2.bit.INTx4=1;

  EINT;  
  ERTM;	

  ////////ÉÏµç×Ô¼ì²â//////////////////////////
 
   GpioDataRegs.GPADAT.bit.GPIOA0=0;  //½ÓÍ¨ÉÏµç×Ô¼ìµçÔ´

   EvaRegs.T1CON.bit.TENABLE=1;   //´ò¿ª¶¨Ê±Æ÷1

   while(1)         
   { if(count1>=240)  //ÑÓÊ±12Sºó×Ô¼ì
     {count1=0;
	  break;
	 }
   }
   
   EvaRegs.T1CON.bit.TENABLE=0;   //¹Ø±Õ¶¨Ê±Æ÷1

   if(GpioDataRegs.GPADAT.bit.GPIOA15==1)//²éÑ¯ÎÂ¶È´«¸ÐÆ÷ÊÇ·ñ¹ÊÕÏ
   { flag=1;
   }
   
   if(GpioDataRegs.GPADAT.bit.GPIOA7==1) //²éÑ¯½á±ùËÙÂÊÌ½²âÊÇ·ñ¹ÊÕÏ
   { flag=1;
   }
   
   if(GpioDataRegs.GPADAT.bit.GPIOA9!=1)  //²éÑ¯K61×Ô¼ìÊÇ·ñ¹ÊÕÏ
   { flag=1;
   }
   
    if(GpioDataRegs.GPADAT.bit.GPIOA10!=1)  //²éÑ¯K62×Ô¼ìÊÇ·ñ¹ÊÕÏ
   { flag=1;
   }

    if(GpioDataRegs.GPADAT.bit.GPIOA11!=1)  //²éÑ¯K63×Ô¼ìÊÇ·ñ¹ÊÕÏ
   { flag=1;
   }
   
   flag=0;
   if(flag==1)
   { while(1)
     { //GpioDataRegs.GPADAT.bit.GPIOA0=1;  //¶Ï¿ªÉÏµç×Ô¼ìµçÔ´
	 }     
   }
   else
   { flag=0;
     //GpioDataRegs.GPADAT.bit.GPIOA5=0;   //Êä³öÉÏµç×Ô¼ì²âÕý³£
     //GpioDataRegs.GPADAT.bit.GPIOA0=1;  //¶Ï¿ªÉÏµç×Ô¼ìµçÔ´

	 ////////ÈË¹¤×Ô¼ì²â//////////////////////////
	  EvaRegs.T1CON.bit.TENABLE=1;   //´ò¿ª¶¨Ê±Æ÷1

      while(1)         
      { if(count1>=120)  //ÑÓÊ±6SºóÈË¹¤¼ì²â
        {count1=0;
	     break;
	    }
      }
   
     EvaRegs.T1CON.bit.TENABLE=0;   //¹Ø±Õ¶¨Ê±Æ÷1

	 //if(GpioDataRegs.GPADAT.bit.GPIOA13==1)   //´ò¿ªÈË¹¤²âÊÔµçÔ´
     //{ 
       GpioDataRegs.GPBDAT.bit.GPIOB2=0;  //ÈË¹¤¼ì²â²âÊÔµÆÁÁ
	   //GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //´ò¿ª¼ÓÈÈ¿ØÖÆµçÔ´       
	   /*j=0;
       count1=0;
	   EvaRegs.T1CON.bit.TENABLE=1;   //´ò¿ª¶¨Ê±Æ÷1

	   while(1)
	   { if((0<count1)&&(count1<=17))
	     { GpioDataRegs.GPBDAT.bit.GPIOB3=0;  //´ò¿ªÐýÒí¼ÓÈÈ¿ØÖÆ
		   GpioDataRegs.GPBDAT.bit.GPIOB5=0;  //´ò¿ªÎ²½°¼ÓÈÈ¿ØÖÆ
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
       EvaRegs.T1CON.bit.TENABLE=0;   //¹Ø±Õ¶¨Ê±Æ÷1
       GpioDataRegs.GPBDAT.bit.GPIOB4=0;  //¶Ï¿ª¼ÓÈÈ¿ØÖÆµçÔ´*/
       
	   EvaRegs.T1CON.bit.TENABLE=1;   //´ò¿ª¶¨Ê±Æ÷1
       GpioDataRegs.GPBDAT.bit.GPIOB1=0; //´ò¿ªÐýÒí²âÊÔµÆ

       while(1)         
       { if(count1>=120)    //ÑÓÊ±6S
         { GpioDataRegs.GPBDAT.bit.GPIOB1=1; //¹Ø±ÕÐýÒí²âÊÔµÆ
           count1=0;		  
	       break;
	     }
       }

       GpioDataRegs.GPBDAT.bit.GPIOB0=0; //´ò¿ªÎ²½°²âÊÔµÆ

       while(1)         
       { if(count1>=80)    //ÑÓÊ±4S
         { GpioDataRegs.GPBDAT.bit.GPIOB0=1; //¹Ø±ÕÎ²½°²âÊÔµÆ
           count1=0;		  
	       break;
	     }
       }
       GpioDataRegs.GPBDAT.bit.GPIOB2=1;
       EvaRegs.T1CON.bit.TENABLE=0;   //¹Ø±Õ¶¨Ê±Æ÷1

	   /////////²éÑ¯½á±ù¸æ¾¯ÐÅºÅ///////////////////////

	   while(1)
	   { //if(GpioDataRegs.GPADAT.bit.GPIOA6==1) //½á±ù¸æ¾¯
	     //{ if(GpioDataRegs.GPADAT.bit.GPIOA12==1)  //²éÑ¯¼ÓÈÈµçÔ´ÊÇ·ñ½ÓÍ¨
		  // { 
		    /* channel=0x0000;    //ADCINA0²É¼¯´óÆøÎÂ¶È
             InitAdc();
			 EvbRegs.T4CON.bit.TENABLE=1;  //´ò¿ª¶¨Ê±Æ÷4¿ªÊ¼²ÉÑù
             i=0;
			 while(1)
			 {if(i==12)
			  break;
			 }
             i=0;
             AD0=cesi;
             EvbRegs.T4CON.bit.TENABLE=0;  //¹Ø±Õ¶¨Ê±Æ÷4
             
			 channel=0x1111;    //ADCINA1²É¼¯½á±ùËÙÂÊ
             InitAdc();
			 EvbRegs.T4CON.bit.TENABLE=1;  //´ò¿ª¶¨Ê±Æ÷4¿ªÊ¼²ÉÑù
			 while(1)
			 {if(i==12)
			  break;
			 }
             i=0;
             AD1=cesi;
             EvbRegs.T4CON.bit.TENABLE=0;  //¹Ø±Õ¶¨Ê±Æ÷4

             Count();*/
			 PWM();
		     
		 //  }
		// }
	  }
	//}
  }
}

