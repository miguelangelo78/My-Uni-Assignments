/*
*********************************************************************************************************
*                                         BOARD SUPPORT PACKAGE
*
*                                 (c) Copyright 2013, Micrium, Weston, FL
*                                          All Rights Reserved
*
* Filename      : os_bsp.h
* Version       : V1.00
* Programmer(s) : JPB
*                 AA
*                 JJL
*                 JM
*********************************************************************************************************
*/

#ifndef  BSP_OS_TICK_H_
#define  BSP_OS_TICK_H_


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void     OS_BSP_TickInit (void);
CPU_ISR  OS_BSP_TickISR  (void);

#endif
