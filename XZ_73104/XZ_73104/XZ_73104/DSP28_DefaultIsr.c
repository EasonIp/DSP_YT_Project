#include "DSP28_Device.h"

interrupt void INT13_ISR(void)     // INT13 or CPU-Timer1
{
     EINT;
     return;
}

interrupt void INT14_ISR(void)     // CPU-Timer2
{
     EINT;
     return;
}

interrupt void NMI_ISR(void)       // Non-maskable interrupt
{
   EINT;
   return;
}

interrupt void  XINT1_ISR(void)
{
   EINT;
   return;
}     

interrupt void  XINT2_ISR(void)
{
   EINT;
   return;
}

interrupt void DATALOG_ISR(void)   // Datalogging interrupt
{
   EINT;
   return;
}

interrupt void RTOSINT_ISR(void)   // RTOS interrupt
{
   EINT;
   return;
}

interrupt void EMUINT_ISR(void)    // Emulation interrupt
{
   EINT;
   return;
}


interrupt void ILLEGAL_ISR(void)   // Illegal operation TRAP
{
  asm("          ESTOP0");
  return;

}

interrupt void USER0_ISR(void)     // User Defined trap 0
{
   EINT;
   return;
}

interrupt void USER1_ISR(void)     // User Defined trap 1
{
   EINT;
   return;
}

interrupt void USER2_ISR(void)     // User Defined trap 2
{
   EINT;
   return;
}

interrupt void USER3_ISR(void)     // User Defined trap 3
{
   EINT;
   return;
}

interrupt void USER4_ISR(void)     // User Defined trap 4
{
   EINT;
   return;
}

interrupt void USER5_ISR(void)     // User Defined trap 5
{
   EINT;
   return;
}

interrupt void USER6_ISR(void)     // User Defined trap 6
{
   EINT;
   return;
}

interrupt void USER7_ISR(void)     // User Defined trap 7
{
   EINT;
   return;
}

interrupt void USER8_ISR(void)     // User Defined trap 8
{
   EINT;
   return;
}

interrupt void USER9_ISR(void)     // User Defined trap 9
{
   EINT;
   return;
}

interrupt void USER10_ISR(void)    // User Defined trap 10
{
   EINT;
   return;
}

interrupt void USER11_ISR(void)    // User Defined trap 11
{
   EINT;
   return;
}

interrupt void  ADCINT_ISR(void)     // ADC
{
   EINT;
   return;
}

interrupt void  TINT0_ISR(void)      // CPU-Timer 0
{
     EINT;
     return;
}

interrupt void  WAKEINT_ISR(void)    // WD
{
   EINT;
   return;
}

interrupt void PDPINTA_ISR( void )    // EV-A
{
   EINT;
   return;
}

interrupt void CMP1INT_ISR(void)    // EV-A
{
   EINT;
   return;
}

interrupt void CMP2INT_ISR(void)    // EV-A
{
   EINT;
   return;
}

interrupt void CMP3INT_ISR(void)    // EV-A
{
   EINT;
   return;
}

interrupt void T1PINT_ISR(void)    // EV-A
{
   EINT;
   return;
}

interrupt void T1CINT_ISR(void)    // EV-A
{
   EINT;
   return;
}

interrupt void T1UFINT_ISR(void)   // EV-A
{
   EINT;
   return;
}

interrupt void T1OFINT_ISR(void)   // EV-A
{
   EINT;
   return;
}
      
interrupt void T2PINT_ISR(void)     // EV-A
{
   EINT;
   return;
}

interrupt void T2CINT_ISR(void)     // EV-A
{
   EINT;
   return;
}

interrupt void T2UFINT_ISR(void)    // EV-A
{
   EINT;
   return;
}

interrupt void T2OFINT_ISR(void)    // EV-A
{
   EINT;
   return;
}

interrupt void CAPINT1_ISR(void)    // EV-A
{
   EINT;
   return;
}

interrupt void CAPINT2_ISR(void)    // EV-A
{
   EINT;
   return;
}

interrupt void CAPINT3_ISR(void)    // EV-A
{
   EINT;
   return;
}
      
interrupt void  PDPINTB_ISR(void)   // EV-B
{
   EINT;
   return;
}

interrupt void CMP4INT_ISR(void)    // EV-B
{
   EINT;
   return;
}

interrupt void CMP5INT_ISR(void)    // EV-B
{
   EINT;
   return;
}

interrupt void CMP6INT_ISR(void)    // EV-B
{
   EINT;
   return;
}

interrupt void T3PINT_ISR(void)     // EV-B
{
   EINT;
   return;
}

interrupt void T3CINT_ISR(void)     // EV-B
{
   EINT;
   return;
}

interrupt void T3UFINT_ISR(void)    // EV-B
{
   EINT;
   return;
}

interrupt void T3OFINT_ISR(void)    // EV-B
{
   EINT;
   return;
}
     
interrupt void T4PINT_ISR(void)    // EV-B
{
   EINT;
   return;
}

interrupt void T4CINT_ISR(void)    // EV-B
{
   EINT;
   return;
}

interrupt void T4UFINT_ISR(void)   // EV-B
{
   EINT;
   return;
}

interrupt void T4OFINT_ISR(void)   // EV-B
{
   EINT;
   return;
}

interrupt void CAPINT4_ISR(void)   // EV-B
{
   EINT;
   return;
}

interrupt void CAPINT5_ISR(void)   // EV-B
{
   EINT;
   return;
}

interrupt void CAPINT6_ISR(void)   // EV-B
{
   EINT;
   return;
}


interrupt void MRINTA_ISR(void)     // McBSP-A
{
   EINT;
   return;
}

interrupt void MXINTA_ISR(void)     // McBSP-A
{
   EINT;
   return;
}

interrupt void SPIRXINTA_ISR(void)    // SPI-A
{
   EINT;
   return;
}

interrupt void SPITXINTA_ISR(void)     // SPI-A
{
   EINT;
   return;
}

interrupt void SCIRXINTA_ISR(void)     // SCI-A
{
   EINT;
   return;
}

interrupt void SCITXINTA_ISR(void)     // SCI-A
{
   EINT;
   return;
}

interrupt void SCIRXINTB_ISR(void)     // SCI-B
{
   EINT;
   return;
}

interrupt void SCITXINTB_ISR(void)     // SCI-B
{
   EINT;
   return;
}

interrupt void ECAN0INTA_ISR(void)  // eCAN-A
{
   EINT;
   return;
}

interrupt void ECAN1INTA_ISR(void)  // eCAN-A
{
   EINT;
   return;
}

interrupt void PIE_RESERVED(void)  // Reserved space.  For test.
{
  EINT;
  return;
}

interrupt void rsvd_ISR(void)          // for test
{
  EINT;
  return;
}


