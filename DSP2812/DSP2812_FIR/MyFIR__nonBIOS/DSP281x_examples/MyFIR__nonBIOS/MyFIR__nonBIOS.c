
#include "DSP281x_Device.h"     
#include "DSP281x_Examples.h"   
#include "IQmathLib.h"
#include "fir.h"
#include <math.h>
#include <stdio.h>

#define FIR_ORDER 50
#pragma DATA_SECTION(lpf, "firfilt");
FIR16 lpf=FIR16_DEFAULTS;
#pragma DATA_SECTION(dbuffer,"firldb");
long dbuffer[(FIR_ORDER+2)/2];
const long coeff[(FIR_ORDER+2)/2]=FIR16_LPF50; 
int xn[200];
int	index,in,in1,input,output;
float yn[200];
float out;
Uint16 j=0;

void main(void)
{

   InitSysCtrl();
   DINT;
   InitPieCtrl();
   IER = 0x0000;
   IFR = 0x0000;
   InitPieVectTable();

	for(index=0;index<200;index++)
	{
		//xn[index]=(int)((sin(0.02454*500*index)*32768)+(0.8*sin(0.02454*8888*index)*32768)+(0.5*sin(0.02454*888*index)*32768)+(0.3*sin(0.02454*2500*index)*32768));
        xn[index]=(int)((sin(0.02454*500*index)*32768)+(0.8*sin(0.02454*8888*index)*32768));
	}
	lpf.dbuffer_ptr=dbuffer;
	lpf.coeff_ptr=(long *)coeff;
	lpf.order=FIR_ORDER;
	lpf.init(&lpf);
	index=0;
	for(;;)
	{
		index=0;
		for(index=0;index<200;index++)
		{
			xn[index]=xn[index+1];
		}
		xn[199]=xn[0];
		for(index=0;index<200;index++)
		{
			input=xn[index];
			lpf.input=input;
			lpf.calc(&lpf);
			output=lpf.output;
			out=(float)output/32768;
			yn[index]=out;
		}
        
	}
} 



