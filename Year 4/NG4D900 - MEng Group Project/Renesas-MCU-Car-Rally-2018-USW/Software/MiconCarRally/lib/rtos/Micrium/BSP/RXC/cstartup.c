/*********************************************************************
*
* Device     : RX/RX600
*
* File Name  : resetprg.c
*
* Abstract   : Reset Program.
*
* History    : 1.00  (2009-08-07)
*            : 1.10  (2011-02-21)
*            : 1.11  (2011-06-20)
*
* NOTE       : THIS IS A TYPICAL EXAMPLE.
*
* Copyright (C) 2009 (2011) Renesas Electronics Corporation.
* and Renesas Solutions Corp.
*
*********************************************************************/

#include <machine.h>
#include <_h_c_lib.h>
#include "typedefine.h" // Define Types
#include "stacksct.h"   // Stack Sizes (Interrupt and User)

void PowerON_Reset(void);
void main(void);

#define PSW_init  0x00010000 // PSW bit pattern
#define FPSW_init 0x00000000 // FPSW bit base pattern

#pragma section ResetPRG // Output PowerON_Reset to PResetPRG section

#pragma entry PowerON_Reset

void PowerON_Reset(void)
{ 
	set_intb(__sectop("C$VECT"));

#ifdef __ROZ                    // Initialize FPSW
#define _ROUND 0x00000001            // Let FPSW RMbits=01 (round to zero)
#else
#define _ROUND 0x00000000            // Let FPSW RMbits=00 (round to nearest)
#endif
#ifdef __DOFF
#define _DENOM 0x00000100            // Let FPSW DNbit=1 (denormal as zero)
#else
#define _DENOM 0x00000000            // Let FPSW DNbit=0 (denormal as is)
#endif

    set_fpsw(FPSW_init | _ROUND | _DENOM);

    _INITSCT();

//    _INIT_IOLIB();                    // Use SIM I/O

//    errno=0;                        // Remove the comment when you use errno
//    srand((_UINT)1);                    // Remove the comment when you use rand()
//    _s1ptr=NULL;                    // Remove the comment when you use strtok()

//    HardwareSetup();                // Use Hardware Setup
    nop();

//    _CALL_INIT();                    // Remove the comment when you use global class object

    set_psw(PSW_init);                // Set Ubit & Ibit for PSW
//    chg_pmusr();                    // Remove the comment when you need to change PSW PMbit (SuperVisor->User)

    main();

//    _CLOSEALL();                    // Use SIM I/O
    
//    _CALL_END();                    // Remove the comment when you use global class object

    brk();
}

