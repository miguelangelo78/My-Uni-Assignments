/*
*********************************************************************************************************
*                                         BOARD SUPPORT PACKAGE
*
*                            (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               BSP is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                           EXCEPTION HANDLERS
*
*                                            Renesas RX62N
*                                                on the
*                                      YRPBRX62N Promotional Board
*
* Filename      : bsp_vector_tbl.c
* Version       : V1.00
* Programmer(s) : DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <lib_def.h>
#include  "bsp_cfg.h"

#if       BSP_CFG_OS3_EN   > 0u
#include  <os.h>
#endif

#if       BSP_CFG_OS2_EN   > 0u
#include  <ucos_ii.h>
#endif

static  void  Excep_SuperVisorInst(void);
static  void  Excep_UndefinedInst (void);
static  void  NonMaskableInterrupt(void);
extern  void  PowerON_Reset       (void);


/*
*********************************************************************************************************
*                                         LOCAL GLOBAL TABLES
*********************************************************************************************************
*/

#pragma section C FIXEDVECT

void*  const  Fixed_Vectors[] = {
    Excep_SuperVisorInst,                                       /* 0xffffffd0  Exception(Supervisor Instruction)        */
    0,                                                          /* 0xffffffd4  Reserved                                 */
    0,                                                          /* 0xffffffd8  Reserved                                 */
    Excep_UndefinedInst,                                        /* 0xffffffdc  Exception(Undefined Instruction)         */
    0,                                                          /* 0xffffffe0  Reserved                                 */
    0,                                                          /* 0xffffffe4  Reserved                                 */
    0,                                                          /* 0xffffffe8  Reserved                                 */
    0,                                                          /* 0xffffffec  Reserved                                 */
    0,                                                          /* 0xfffffff0  Reserved                                 */
    0,                                                          /* 0xfffffff4  Reserved                                 */
    NonMaskableInterrupt,                                       /* 0xfffffff8  NMI                                      */
    PowerON_Reset,                                              /* 0xffffffff  Reset                                    */
};

#pragma address _MDEreg=0xffffff80 // MDE register (Single Chip Mode)
#ifdef __BIG
    const unsigned long _MDEreg = 0xfffffff8; // big
#else
    const unsigned long _MDEreg = 0xffffffff; // little
#endif
#pragma section



/*
*********************************************************************************************************
*********************************************************************************************************
*                                         EXCEPTION HANDLERS
*********************************************************************************************************
*********************************************************************************************************
*/

#pragma section IntPRG

/*
*********************************************************************************************************
*                                            Excep_SuperVisorInst()
*
* Description : Handle SuperVisor Instruction Interrupt.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  void  Excep_SuperVisorInst(void)
{
    while (DEF_TRUE) {
        ;
    }
}


/*
*********************************************************************************************************
*                                            Excep_UndefinedInst()
*
* Description : Handle Undefined Instruction Interrupt.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  void  Excep_UndefinedInst(void)
{
    while (DEF_TRUE) {
        ;
    }
}


/*
*********************************************************************************************************
*                                            INT_NonMaskableInterrupt()
*
* Description : Handle Non-Maskable Interrupt (NMI).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  void  NonMaskableInterrupt(void)
{
    while (DEF_TRUE) {
        ;
    }
}


#pragma section

