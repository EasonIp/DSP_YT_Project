#ifndef DSP28_DEV_EMU_H
#define DSP28_DEV_EMU_H

// Device Configuration Register Bit Definitions
struct DEVICECNF_BITS  {   // bits  description
   Uint16 rsvd1:3;           // 2:0   reserved
   Uint16 VMAPS:1;           // 3     VMAP Status
   Uint16 rsvd2:1;           // 4     reserved
   Uint16 XRSn:1;            // 5     XRSn Signal Status
   Uint16 rsvd3:10;          // 15:6
   Uint16 rsvd4:3;           // 18:6
   Uint16 ENPROT:1;          // 19    Enable/Disable pipeline protection
   Uint16 rsvd5:12;          // 31:20 reserved
};

union DEVICECNF_REG {
   Uint32                all;
   struct DEVICECNF_BITS  bit;
};

// Device ID Register Bit Definitions
struct DEVICEID_BITS  {    // bits  description
   Uint16 PARTID:16;         // 15:0  Part ID
   Uint16 REVID:16;          // 31:16 Revision
};

union DEVICEID_REG {
   Uint32                   all;
   struct DEVICEID_BITS  bit;
};

struct DEV_EMU_REGS {
   union DEVICECNF_REG DEVICECNF;
   union DEVICEID_REG DEVICEID;
   Uint16  PROTSTART;
   Uint16  PROTRANGE;
   Uint16 rsvd[202];
   Uint16 M0RAMDFT;
   Uint16 M1RAMDFT;
   Uint16 L0RAMDFT;
   Uint16 L1RAMDFT;
   Uint16 H0RAMDFT;
};

extern volatile struct DEV_EMU_REGS DevEmuRegs;
#endif  // end of DSP28_DEV_EMU_H definition
