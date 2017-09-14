/* ==================================================================
File name       : FIR.H                     
                    
Originator      : Advanced Embeeded Control (AEC)
                  Texas Instruments Inc.
Description:  
Header file containing  object definitions, proto type declaration and 
default object initializers for FIR Filter.
===================================================================
 History:
-------------------------------------------------------------------
12-05-2002   Release Rev 1.0                                                  
-------------------------------------------------------------------*/

#ifndef __FIR_H__
#define __FIR_H__
#define NULL    0
 
  
/*----------------------------------------------------------------
Define the structure of the FIRFILT_GEN Filter Module 
-----------------------------------------------------------------*/
typedef struct { 
    long *coeff_ptr;        /* Pointer to Filter coefficient */
    long * dbuffer_ptr;     /* Delay buffer ptr              */
    int	cbindex;			/* Circular Buffer Index         */
    int order;              /* Order of the Filter           */
    int input;              /* Latest Input sample           */ 
    int output;             /* Filter Output                 */
    void (*init)(void *);   /* Ptr to Init funtion           */
    void (*calc)(void *);   /* Ptr to calc fn                */  
    }FIR16;    



           
/*---------------------------------------------------------------
Define a Handles for the Filter Modules
-----------------------------------------------------------------*/
typedef FIR16 	*FIR16_handle;

          
#define FIR16_DEFAULTS { (long *)NULL, \
             (long *)NULL,   \
             0,            \
             50,             \
             0,				\
             0,				\
             (void (*)(void *))FIR16_init,\
             (void (*)(void *))FIR16_calc}    


                                                       
/*-------------------------------------------------------------
 Prototypes for the functions
---------------------------------------------------------------*/
void FIR16_calc(void *);
void FIR16_init(void *);


/*********** Sample FIR Co-efficients **************************/
/* Even Order (50): LPF co-efficients for FIR16 module	*/
#define FIR16_LPF50 {\
			8198,1514682,-60393,-2029236,-3538944,-3147075,64013,5242001,9437183,8323675,\
			-64824,-12844636,-22216704,-19005731,65197,27590460,46727167,39518335,-65393,-57606065,\
			-99811328,-88211505,65482,156041185,337117183,481951767}
#endif
