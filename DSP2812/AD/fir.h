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
#define FIR16_LPF50 {6538,6094,1381132,2624632,3278242,2555904,-893,-4261018,-8848333,-11141621,\
                        -8585217,348,12976597,25493893,30736582,22806528,-132,-32768171,-63701128,-77135938,\
                        -58458113,39,94502962,209190952,319553557,399376384}
#endif
