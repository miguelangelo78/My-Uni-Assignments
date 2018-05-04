/*
*********************************************************************************************************
*                                         BOARD SUPPORT PACKAGE
*
*                            (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved. Protected by international copyright laws.
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
* Filename      : bsp_sys.c
* Version       : V1.00
* Programmer(s) : JPB
*                 AA
*                 JJL
*                 JM
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDES
*********************************************************************************************************
*/

#include  "bsp_cfg.h"

#include  <cpu.h>
#include  "..\..\..\..\..\uc-cpu\cpu_core.h"
#include  "..\..\..\..\..\uc-lib\lib_def.h"
#include  "..\bsp\bsp_sys.h"

#include  <iodefine.h>


/*
*********************************************************************************************************
*                                         BSP SYSTEM INITIALIZATION
*
* Description: This function should be called early in the BSP initialization process.
*
* Arguments  : none
*********************************************************************************************************
*/

void  BSP_SysInit (void)
{
    SYSTEM.SCKCR.BIT.ICK = 0; /* System Clock: 12.288 * 8 = 98.304MHz */
    SYSTEM.SCKCR.BIT.PCK = 1; /* Peripheral Clock: 12.288 * 4 = 49.152MHz */
}


/*
*********************************************************************************************************
*                                     PERIPHERAL CLOCK FREQUENCY
*
* Description: This function is used to retrieve peripheral clock frequency.
*
* Arguments  : none
*
* Return     : Peripheral clock frequency in cycles.
*********************************************************************************************************
*/

CPU_INT32U  BSP_SysPerClkFreqGet (void)
{
    CPU_INT32U  sys_clk;
    CPU_INT32U  per_clk;
    CPU_INT32U  per_div;


    sys_clk =  SYSTEM.SCKCR.LONG;
    per_div = (sys_clk >> 8u) & 0xFu;
    if (per_div > 3u) {
        return (0u);
    }

    per_clk =  BSP_CFG_SYS_EXT_CLK_FREQ << (3u - per_div);
    return (per_clk);
}
