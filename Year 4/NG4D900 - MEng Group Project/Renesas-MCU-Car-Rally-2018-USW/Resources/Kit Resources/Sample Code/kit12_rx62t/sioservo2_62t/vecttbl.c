/***********************************************************************/
/*                                                                     */
/*  FILE        :vecttbl.c                                             */
/*  DATE        :Tue, Sep 03, 2013                                     */
/*  DESCRIPTION :Initialize of Vector Table                            */
/*  CPU TYPE    :RX62T                                                 */
/*                                                                     */
/*  This file is generated by Renesas Project Generator (Ver.4.53).    */
/*  NOTE:THIS IS A TYPICAL EXAMPLE.                                    */
/*                                                                     */
/***********************************************************************/
/************************************************************************
*
* Device     : RX/RX600/RX62T
*
* File Name  : vecttbl.c
*
* Abstract   : Initialize of Vector Table.
*
* History    : 1.00 (2010-06-08)  [Hardware Manual Revision : 0.20]
*            : 1.10 (2011-02-21)  [Hardware Manual Revision : 0.20]
*            : 1.11 (2011-06-20)  [Hardware Manual Revision : 1.00]
*
* NOTE       : THIS IS A TYPICAL EXAMPLE.
*
* Copyright (C) 2010 (2011) Renesas Electronics Corporation.
* and Renesas Solutions Corp.
*
************************************************************************/

#include "vect.h"

#pragma section C FIXEDVECT

void (*const Fixed_Vectors[])(void) = {
//;0xffffffd0  Exception(Supervisor Instruction)
    Excep_SuperVisorInst,
//;0xffffffd4  Reserved
    Dummy,
//;0xffffffd8  Reserved
    Dummy,
//;0xffffffdc  Exception(Undefined Instruction)
    Excep_UndefinedInst,
//;0xffffffe0  Reserved
    Dummy,
//;0xffffffe4  Exception(Floating Point)
    Excep_FloatingPoint,
//;0xffffffe8  Reserved
    Dummy,
//;0xffffffec  Reserved
    Dummy,
//;0xfffffff0  Reserved
    Dummy,
//;0xfffffff4  Reserved
    Dummy,
//;0xfffffff8  NMI
    NonMaskableInterrupt,
//;0xfffffffc  RESET
//;<<VECTOR DATA START (POWER ON RESET)>>
//;Power On Reset PC
PowerON_Reset_PC                                                                                                                             
//;<<VECTOR DATA END (POWER ON RESET)>>
};
