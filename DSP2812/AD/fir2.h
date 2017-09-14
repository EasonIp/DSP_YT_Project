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
			7881,-58407,-1633260,-2815451,-2752204,-525409,3406374,7273445,7929600,2883995,\
                        -7142746,-17300966,-20119370,-9896088,11992781,35323640,44498835,26935340,-16777096,-68878226,\
                        -98172877,-73400329,20250582,170262485,336920551,467271679}
#endif
