//###########################################################################
#ifndef EXAMPLE_H
#define EXAMPLE_H

/*---- shared global function prototypes -----------------------------------*/
extern void InitAdc(void);
extern void InitDevEmu(void);
extern void InitDevice(void);
extern void InitECana(void);
extern void InitEv(void);
extern void InitGpio(void);
extern void InitPieCtrl(void);
extern void InitPieVectTable(void);
extern void InitSci(void);
extern void InitSysCtrl(void);
extern void InitXIntrupt(void);


extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;  


#endif   // - end of EXAMPLE_H

