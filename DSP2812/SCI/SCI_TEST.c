
#include "DSP28_Device.h"

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

///////////////////////////////////////////
unsigned long k=0;
unsigned int i=0;
unsigned int j=0;
unsigned int sj[9]={0,0,0,0,0,0,0,0,0};
unsigned int count=0;
char LWC=0x11;
char ICE=0x22;
char ERROR1=0x33;
char ERROR2=0x44;

void InitSci(void);
interrupt void SCIA_ISR(void);
void init_eva_timer1(void);
interrupt void eva_timer1_isr(void); 

///////////////////¶¨Ê±Æ÷1³õÊ¼»¯£¨25ms£©//////////////////////////////////

void init_eva_timer1(void)
{
  
   EvaRegs.GPTCONA.all = 0;   //³õÊ¼»¯¶¨Ê±Æ÷1²¢ÉèÖÃ¶¨Ê±Æ÷1¼Ä´æÆ÷
   EvaRegs.EVAIMRA.bit.T1PINT = 1;  //ÔÊÐí¶¨±Æ÷1ÖÜÆÚÖÐ¶Ï
   EvaRegs.EVAIFRA.bit.T1PINT = 1;  //Çå¶¨÷1ÖÜÆÚÖÐ¶Ï±êÖ¾
             
   EvaRegs.T1CON.all = 0x170C;  //Timer1ÎªÁ¬ÐøÔö¼ÆÊýÄ£Ê½£¬Ô¤·ÖÆµÖµ128
	                             //Ê¹ÓÃÄÚ²¿¸ßËÙÍâÉèÊ±ÖÓ
   EvaRegs.T1PR = 0x16e3;   //Timer1µÄÆÚ¼Ä´æÆ÷=0.025*30MHz/128
   EvaRegs.T1CNT = 0x0000;  //Timer1µÄ¼ÆÊýã

} 

/////////////////////////////////////////
void InitSci(void)
{
	
  EALLOW;
  GpioMuxRegs.GPFMUX.all = 0x0030;
  GpioMuxRegs.GPGMUX.all = 0x0030;
  EDIS;
    
    SciaRegs.SCIFFTX.all=0xe040;
    SciaRegs.SCIFFRX.all=0x2021;
	SciaRegs.SCIFFCT.all=0x0000;
	SciaRegs.SCICCR.all = 0x07;	
	SciaRegs.SCICTL1.all = 0x03;
	SciaRegs.SCICTL2.all = 0x03;
	
	SciaRegs.SCIHBAUD = 0x01;   //9600
	SciaRegs.SCILBAUD = 0x85;
	
	SciaRegs.SCICTL1.all = 0x23;
    
    ////////////////////////////////
    ScibRegs.SCIFFTX.all=0xe040;
    ScibRegs.SCIFFRX.all=0x2021;
	ScibRegs.SCIFFCT.all=0x0000;
	ScibRegs.SCICCR.all = 0x07;	
	ScibRegs.SCICTL1.all = 0x03;
	ScibRegs.SCICTL2.all = 0x03;
	
	ScibRegs.SCIHBAUD = 0x01;   //9600
	ScibRegs.SCILBAUD = 0x85;
	
	ScibRegs.SCICTL1.all = 0x23;	
}

////////////////////////////////////////
void E422_Send(char sj)
{
  SciaRegs.SCITXBUF=sj;
  for(i=0;i<0xff;i++);
}

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

//////////////////////////////////////////
/*interrupt void SCIA_ISR(void)
{   
   DINT;
   if(PieCtrl.PIEACK.bit.ACK9==1)
   { PieCtrl.PIEACK.bit.ACK9=1;
     sj[j]=SciaRegs.SCIRXBUF.all;
	 j++;
     if(j>8)
	 { 
	   j=0;
	 }
    SciaRegs.SCIFFRX.bit.RXFFINTCLR=1;
    SciaRegs.SCIFFRX.bit.RXFFOVF=1;
   }
   EINT;
}*/


void main(void)
{

   for(k=0;k<0xfffff;k++);
   for(k=0;k<0xfffff;k++);

	/*³õÊ¼»¯ÏµÍ³*/
	/*InitSysCtrl();
  //°ÑInitFlash£¨£©º¯Êý°áÒÆµ½ramÖÐÔËÐÐ
    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);	
    InitFlash();


	/*¹ØÖÐ¶Ï*/
	DINT;
	IER = 0x0000;
	IFR = 0x0000;

	/*³õÊ¼»¯PIEÖÐ¶Ï*/
	InitPieCtrl();

	/*³õÊ¼»¯PIEÖÐ¶ÏÊ¸Á¿±í*/
	InitPieVectTable();	

    init_eva_timer1();	
	/*³õÊ¼»¯SCIA¼Ä´æÆ÷*/
    InitSci();

	/*ÉèÖÃÖÐ¶Ï·þÎñ³ÌÐòÈë¿ÚµØÖ·*/
	EALLOW;	// This is needed to write to EALLOW protected registers
	PieVectTable.T1PINT=&eva_timer1_isr;
	//PieVectTable.RXAINT = &SCIA_ISR;
	EDIS;   // This is needed to disable write to EALLOW protected registers

	/*¿ªÖÐ¶Ï*/   
	IER |= (M_INT2|M_INT9);
	PieCtrl.PIEIER2.bit.INTx4=1;
	//PieCtrl.PIEIER9.bit.INTx1 = 1;

	EINT;   // Enable Global interrupt INTM
	ERTM;	// Enable Global realtime interrupt DBGM

	EvaRegs.T1CON.bit.TENABLE=1;
    
	for(;;)
	{
      if(count>=48)
	  {
        count=0;
        E422_Send(0xaa);
        E422_Send(0x55);
		E422_Send(0x00);
		E422_Send(0x09);
		E422_Send(0x02);
		E422_Send(0x7c);
		E422_Send(0x00);
		E422_Send(0x00);
		E422_Send(0xaa+0x55+0x09+0x02+0x7c);
      }	
	}*/
} 	


