
#include "DSP28_Device.h"

#pragma DATA_SECTION(AdcRegs,"AdcRegsFile");
volatile struct ADC_REGS AdcRegs;

#pragma DATA_SECTION(EvaRegs,"EvaRegsFile");
volatile struct EVA_REGS EvaRegs;

#pragma DATA_SECTION(EvbRegs,"EvbRegsFile");
volatile struct EVB_REGS EvbRegs;

#pragma DATA_SECTION(GpioDataRegs,"GpioDataRegsFile");
volatile struct GPIO_DATA_REGS GpioDataRegs;

#pragma DATA_SECTION(GpioMuxRegs,"GpioMuxRegsFile");
volatile struct GPIO_MUX_REGS GpioMuxRegs;

#pragma DATA_SECTION(PieCtrl,"PieCtrlRegsFile");
volatile struct PIE_CTRL_REGS PieCtrl;

#pragma DATA_SECTION(PieVectTable,"PieVectTable");
struct PIE_VECT_TABLE PieVectTable;

#pragma DATA_SECTION(SciaRegs,"SciaRegsFile");
volatile struct SCI_REGS SciaRegs;

#pragma DATA_SECTION(ScibRegs,"ScibRegsFile");
volatile struct SCI_REGS ScibRegs;

#pragma DATA_SECTION(SysCtrlRegs,"SysCtrlRegsFile");
volatile struct SYS_CTRL_REGS SysCtrlRegs;

#pragma DATA_SECTION(DevEmuRegs,"DevEmuRegsFile");
volatile struct DEV_EMU_REGS DevEmuRegs;

#pragma DATA_SECTION(CsmRegs,"CsmRegsFile");
volatile struct CSM_REGS CsmRegs;

#pragma DATA_SECTION(CsmPwl,"CsmPwlFile");
volatile struct CSM_PWL CsmPwl;

#pragma DATA_SECTION(FlashRegs,"FlashRegsFile");
volatile struct FLASH_REGS FlashRegs;

#pragma DATA_SECTION(XIntruptRegs,"XIntruptRegsFile");
volatile struct XINTRUPT_REGS XIntruptRegs;

