/**********************************************************************
* File: f2812.cmd -- Linker command file for Boot to Flash bootmode.
* History: 09/08/03 - original (based on DSP28 header files v1.00, D. Alter)
**********************************************************************/
-stack 400

MEMORY
{
PAGE 0:    /* Program Memory */
   RAML0       : origin = 0x008000, length = 0x001000     /* on-chip RAM block L0 */
   RAML1       : origin = 0x009000, length = 0x001000     /* on-chip RAM block L1 */
   OTP         : origin = 0x3D7800, length = 0x000800     /* on-chip OTP */
   FLASH_J     : origin = 0x3D8000, length = 0x002000     /* on-chip FLASH */
   FLASH_I     : origin = 0x3DA000, length = 0x002000     /* on-chip FLASH */
   FLASH_H     : origin = 0x3DC000, length = 0x004000     /* on-chip FLASH */
   FLASH_G     : origin = 0x3E0000, length = 0x004000     /* on-chip FLASH */
   FLASH_F     : origin = 0x3E4000, length = 0x004000     /* on-chip FLASH */
   FLASH_E     : origin = 0x3E8000, length = 0x004000     /* on-chip FLASH */
   FLASH_D     : origin = 0x3EC000, length = 0x004000     /* on-chip FLASH */
   FLASH_C     : origin = 0x3F0000, length = 0x004000     /* on-chip FLASH */
   FLASH_A     : origin = 0x3F7000, length = 0x000F80     /* on-chip FLASH */

   CSM_RSVD    : origin = 0x3F7F80, length = 0x000076
   BEGIN	   : origin = 0x3F7FF6, length = 0x000002  
   PASSWORDS   : origin = 0x3F7FF8, length = 0x000008
     
   ROM         : origin = 0x3FF000, length = 0x000FC0     /* boot ROM available if MP/MCn=0 */
   RESET       : origin = 0x3FFFC0, length = 0x000002     /* part of boot ROM (MP/MCn=0) or XINTF zone 7 (MP/MCn=1) */
   VECTORS     : origin = 0x3FFFC2, length = 0x00003E     /* part of boot ROM (MP/MCn=0) or XINTF zone 7 (MP/MCn=1) */

PAGE 1 :   /* Data Memory */
   RAMM01      : origin = 0x000000, length = 0x0000800     /* on-chip RAM block M0 & M1 */
   DEV_EMU     : origin = 0x000880, length = 0x000180     /* Device emulation registers */
   FLASH_REGS  : origin = 0x000A80, length = 0x000060     /* FLASH registers */
   CSM         : origin = 0x000AE0, length = 0x000010     /* Code security module registers */
   XINTF       : origin = 0x000B20, length = 0x000020     /* External interface registers */
   CPU_TIMER0  : origin = 0x000C00, length = 0x000008     /* CPU Timer0 registers (CPU Timer1 and Timer2 are reserved for BIOS)*/
   CPU_TIMER1  : origin = 0x000C08, length = 0x000008		 
   CPU_TIMER2  : origin = 0x000C10, length = 0x000008
   PIE_CTRL    : origin = 0x000CE0, length = 0x000020     /* PIE control registers */
   PIE_VECT    : origin = 0x000D00, length = 0x000100     /* PIE vector table */
   ECAN_A      : origin = 0x006000, length = 0x000100     /* eCAN registers */
   ECAN_AMBOX  : origin = 0x006100, length = 0x000100     /* eCAN mailboxes */
   SYSTEM      : origin = 0x007010, length = 0x000020     /* System control registers */
   SPI_A       : origin = 0x007040, length = 0x000010     /* SPI registers */
   SCI_A       : origin = 0x007050, length = 0x000010     /* SCI-A registers */
   XINTRUPT    : origin = 0x007070, length = 0x000010     /* External interrupt registers */
   GPIOMUX     : origin = 0x0070C0, length = 0x000020     /* GPIO mux registers */
   GPIODAT     : origin = 0x0070E0, length = 0x000020     /* GPIO data registers */
   ADC         : origin = 0x007100, length = 0x000020     /* ADC registers */
   EV_A        : origin = 0x007400, length = 0x000040     /* Event Manager A registers */
   EV_B        : origin = 0x007500, length = 0x000040     /* Event Manager B registers */
   SCI_B       : origin = 0x007750, length = 0x000010     /* SCI-B registers */
   MCBSP_A     : origin = 0x007800, length = 0x000040     /* McBSP registers */   
   RAML1       : origin = 0x009000, length = 0x001000
   FLASH_B     : origin = 0x3F4000, length = 0x003000
   CSM_PWL     : origin = 0x3F7FF8, length = 0x000008     /* CSM password locations in FLASHA */
   RAMH0       : origin = 0x3F8000, length = 0x002000     /* H0 SARAM */
}


SECTIONS
{
/*** Compiler Required Sections ***/
  /* Program memory (PAGE 0) sections */
   .text             : > FLASH_C,          PAGE = 0
   .cinit            : > FLASH_C,          PAGE = 0
   .pinit            : > FLASH_C,          PAGE = 0
   .reset            : > RESET,            PAGE = 0, TYPE = DSECT  /* We are not using the .reset section */
   vectors           : > VECTORS,          PAGE = 0, TYPE = DSECT
  /* Data Memory (PAGE 1) sections */
   .stack            : > RAMM01,           PAGE = 1
   .bss              : > RAML1,            PAGE = 1
   .ebss             : > RAML1,            PAGE = 1
   .sysmem           : > RAMH0,            PAGE = 1                /* Should be empty with large memory model */
   .esysmem          : > RAMH0,            PAGE = 1

  /* Initalized sections go in Flash */   
   .const            : > FLASH_B,          PAGE = 1
   .econst           : > FLASH_B,          PAGE = 1                /* Should be empty with large memory model */
   .switch           : > FLASH_B,          PAGE = 1      

/*** User Defined Sections ***/
  
   codestart         : > BEGIN,            PAGE = 0                /* Used by file CodeStartBranch.asm */
   csm_rsvd          : LOAD = CSM_RSVD,    PAGE = 0                /* Used by file passwords.asm */
   passwords         : LOAD = PASSWORDS,   PAGE = 0                /* Used by file passwords.asm */

   secureRamFuncs    : LOAD = FLASH_C,     PAGE = 0                /* Used by InitFlash() in SysCtrl.c */ 
                       RUN  = RAML0,       PAGE = 0
                       LOAD_START(_secureRamFuncs_loadstart)
                       LOAD_END(_secureRamFuncs_loadend)
					   RUN_START(_secureRamFuncs_runstart)  

   ramdata           : > RAMM01,            PAGE = 1         
   
/**********************************************/
/* Allocate Peripheral Frame 0 Register Structures:   */
   DevEmuRegsFile      : > DEV_EMU     PAGE = 1
   FlashRegsFile       : > FLASH_REGS  PAGE = 1
   CsmRegsFile         : > CSM         PAGE = 1
   XintfRegsFile       : > XINTF       PAGE = 1
   CpuTimer0RegsFile   : > CPU_TIMER0  PAGE = 1      
   CpuTimer1RegsFile   : > CPU_TIMER1  PAGE = 1      
   CpuTimer2RegsFile   : > CPU_TIMER2  PAGE = 1 
   PieCtrlRegsFile     : > PIE_CTRL    PAGE = 1      
   PieVectTable        : > PIE_VECT    PAGE = 1

   /* Allocate Peripheral Frame 2 Register Structures:   */
   ECanaRegsFile       : > ECAN_A      PAGE = 1   
   ECanaMboxesFile     : > ECAN_AMBOX  PAGE = 1

   /* Allocate Peripheral Frame 1 Register Structures:   */
   SysCtrlRegsFile     : > SYSTEM      PAGE = 1
   SpiaRegsFile        : > SPI_A       PAGE = 1
   SciaRegsFile        : > SCI_A       PAGE = 1
   XIntruptRegsFile    : > XINTRUPT    PAGE = 1
   GpioMuxRegsFile     : > GPIOMUX     PAGE = 1
   GpioDataRegsFile    : > GPIODAT     PAGE = 1
   AdcRegsFile         : > ADC         PAGE = 1
   EvaRegsFile         : > EV_A        PAGE = 1
   EvbRegsFile         : > EV_B        PAGE = 1
   ScibRegsFile        : > SCI_B       PAGE = 1
   McbspaRegsFile      : > MCBSP_A     PAGE = 1

   /* CSM Password Locations */
   CsmPwlFile          : > CSM_PWL     PAGE = 1
   
}
