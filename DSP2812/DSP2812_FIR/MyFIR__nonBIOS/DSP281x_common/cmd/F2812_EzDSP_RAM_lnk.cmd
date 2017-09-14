
MEMORY
{
PAGE 0 :
   RAMM0      : origin = 0x000000, length = 0x000400
   BEGIN      : origin = 0x3F8000, length = 0x000002             
   PRAMH0     : origin = 0x3F8002, length = 0x000FFE
   BOOTROM (RW)   : origin = 0x3ff000, length = 0x000fc0
   RESET      : origin = 0x3FFFC0, length = 0x000002                
PAGE 1 : 
   RAMM1      : origin = 0x000400, length = 0x000400
   L0L1RAM(RW) : origin = 0x008000, length = 0x2000
   DRAMH0     : origin = 0x3f9000, length = 0x002000         
}
 
 
SECTIONS
{
   codestart        : > BEGIN,       PAGE = 0
   ramfuncs         : > PRAMH0       PAGE = 0  
   .text            : > PRAMH0,      PAGE = 0
   .cinit           : > PRAMH0,      PAGE = 0
   .pinit           : > PRAMH0,      PAGE = 0
   .switch          : > RAMM0,       PAGE = 0
   .reset           : > RESET,       PAGE = 0, TYPE = DSECT /* not used, */
   
   .stack           : > RAMM1,       PAGE = 1
   .ebss            : > DRAMH0,      PAGE = 1
   .econst          : > DRAMH0,      PAGE = 1      
   .esysmem         : > DRAMH0,      PAGE = 1
   firldb           align(0x100)> DRAMH0	 PAGE = 1
   firfilt	        : >	DRAMH0	     PAGE = 1    
   IQmathTables     : load = BOOTROM, PAGE = 0                    
   IQmath           : load = PRAMH0,   PAGE = 0
     
}
