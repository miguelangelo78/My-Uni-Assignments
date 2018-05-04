/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         BOARD SUPPORT PACKAGE
*
*                                            Renesas RX62N
*                                                on the
*                                      YRPBRX62N Promotional Board
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : DC
*                 JJL
*                 SB
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  "bsp.h"

#include  "bsp_cfg.h"

#include  <cpu.h>

#include  "..\bsp\bsp_sys.h"

#if BSP_CFG_LED_EN         > 0u
#include  "..\bsp\bsp_led.h"
#endif

#if BSP_CFG_CAN_EN         > 0u    
#include  "..\bsp\bsp_can.h"
#endif

#if  BSP_CFG_FS_EN       > 0u
#endif

    
#if  BSP_CFG_GUI_EN      > 0u 
#endif

    
#if  BSP_CFG_MBM_EN      > 0u 
#endif

    
#if  BSP_CFG_MBS_EN      > 0u  
#endif

    
#if  BSP_CFG_NET_EN      > 0u  
#endif

    
#if  BSP_CFG_OS2_EN      > 0u
#include  <ucos_ii.h>
#endif

    
#if  BSP_CFG_OS3_EN      > 0u
#include  <os.h>
#endif

    
#if  BSP_CFG_USBD_EN     > 0u  
#endif

    
#if  BSP_CFG_USBH_EN     > 0u  
#endif


#if BSP_CFG_MISC_EN      > 0u
#include  "..\bsp\bsp_misc.h"
#endif


/*
*********************************************************************************************************
*                                         BSP INITIALIZATION
*
* Description : This function should be called by the application code before using any functions in this
*              module.
*
* Arguments   : none
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Init (void)
{
    BSP_SysInit();                                              /* Initialize system services, clocks, etc.             */

    OS_BSP_TickInit();                                          /* Init uC/OS-xx periodic time source                   */

#if BSP_CFG_LED_EN       > 0u    
    BSP_LED_Init();                                             /* Initialize LEDs                                      */
#endif 

    
#if  BSP_CFG_CAN_EN      > 0u
#endif

    
#if  BSP_CFG_CAN_OPEN_EN > 0u
#endif

    
#if  BSP_CFG_FS_EN       > 0u
#endif

    
#if  BSP_CFG_GUI_EN      > 0u 
#endif

    
#if  BSP_CFG_MBM_EN      > 0u 
#endif

    
#if  BSP_CFG_MBS_EN      > 0u  
#endif

    
#if  BSP_CFG_NET_EN      > 0u  
#endif

    
#if  BSP_CFG_OS2_EN      > 0u  
#endif

    
#if  BSP_CFG_OS3_EN      > 0u  
#endif

    
#if  BSP_CFG_USBD_EN     > 0u  
#endif

    
#if BSP_CFG_MISC_EN      > 0u    
    BSP_MiscInit();                                             /* Initialize miscellaneous services                    */
#endif
}

