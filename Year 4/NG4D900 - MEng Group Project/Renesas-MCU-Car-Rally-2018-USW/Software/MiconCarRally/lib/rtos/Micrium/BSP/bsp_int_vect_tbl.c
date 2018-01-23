/*
*********************************************************************************************************
*                                         BOARD SUPPORT PACKAGE
*
*                                           Renesas RX62N
*                                               on the
*                                     YRDKRX62N Evaluation Board
*
*                                 (c) Copyright 2013, Micrium, Weston, FL
*                                          All Rights Reserved
*
* Filename      : bsp_int_vect_tbl.c
* Version       : V1.00
* Programmer(s) : JPB
*                 AA
*                 JJL
*                 JM
*********************************************************************************************************
*/

#include  "bsp_cfg.h"

#include  <cpu.h>

#if       BSP_CFG_OS3_EN   > 0u
#include  <os.h>
#endif

#if       BSP_CFG_OS2_EN   > 0u
#include  <ucos_ii.h>
#endif

#include  "..\..\..\..\..\uc-cpu\cpu_core.h"
#include  "..\..\..\..\..\uc-lib\lib_def.h"
#include  "..\bsp\bsp_int_vect_tbl.h"

#if BSP_CFG_NET_EN > 0u
#include  "..\bsp\net_bsp.h"
#endif

#if BSP_CFG_CAN_EN > 0u
#include  "..\bsp\can_bsp.h"
#endif

#if BSP_CFG_SER_EN > 0u
#include  "..\bsp\bsp_ser.h"
#endif

#if BSP_CFG_USBD_EN > 0u
#include  "..\..\..\..\..\uC-USB-Device-V4\Source\usbd_core.h"
#include  "..\..\..\..\..\uC-USB-Device-V4\Drivers\RX600\BSP\YRDKRX62N\usbd_bsp_yrdkrx62n.h"
#endif

#if BSP_CFG_USBH_EN > 0u
#include  "..\..\..\..\..\uC-USB-Host-V3\HCD\RX600\BSP\YRDKRX62N\usbh_bsp_rx600.h"
#endif


/*
*********************************************************************************************************
*                                       INTERRUPT VECTOR TABLE
*
* Note(s): 1) Could be in RAM if BSP_CFG_INT_VECT_RAM_EN is set to 1 in 'bsp_cfg.h'
*          2) In either case BSP_IntVectSet() must becalled to boint to BSP_IntVectTbl[]
*
*********************************************************************************************************
*/

#if     BSP_CFG_INT_VECT_TBL_RAM_EN > 0u
static  CPU_FNCT_VOID  BSP_IntVectTbl[] =
#else
const   CPU_FNCT_VOID  BSP_IntVectTbl[] =
#endif
{
    (CPU_FNCT_VOID)BSP_IntHandler_000,          /*  00, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_001,          /*  01, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_002,          /*  02, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_003,          /*  03, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_004,          /*  04, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_005,          /*  05, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_006,          /*  06, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_007,          /*  07, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_008,          /*  08, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_009,          /*  09, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_010,          /*  10, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_011,          /*  11, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_012,          /*  12, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_013,          /*  13, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_014,          /*  14, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_015,          /*  15, Reserved                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_016,          /*  16, BUSERR                                         */

    (CPU_FNCT_VOID)BSP_IntHandler_017,          /*  17, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_018,          /*  18, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_019,          /*  19, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_020,          /*  20, Reserved                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_021,          /*  21, FCU_FCUERR                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_022,          /*  22, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_023,          /*  23, FCU_FRDYI                                      */

    (CPU_FNCT_VOID)BSP_IntHandler_024,          /*  24, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_025,          /*  25, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_026,          /*  26, Reserved                                       */

    (CPU_FNCT_VOID)OSCtxSwISR,                  /*  27, ICU_SWINT - uC/OS-xx Context Switch            */

    (CPU_FNCT_VOID)OS_BSP_TickISR,              /*  28, CMTU0_CMI0 - uC/OS-xx Tick interrupt handler   */
    (CPU_FNCT_VOID)BSP_IntHandler_029,          /*  29, CMTU0_CMI1                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_030,          /*  30, CMTU1_CMI2                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_031,          /*  31, CMTU1_CMI3                                     */

#if BSP_CFG_NET_EN > 0u
    (CPU_FNCT_VOID)NetDev_ISR,                  /*  32, Ether_EINT - Ethernet Controller               */
#else
    (CPU_FNCT_VOID)BSP_IntHandler_032,          /*      32 ------------------------------------------- */
#endif

    (CPU_FNCT_VOID)BSP_IntHandler_033,          /*  33, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_034,          /*  34, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_035,          /*  35, Reserved                                       */

#if ((BSP_CFG_USBD_EN > 0u) || (BSP_CFG_USBH_EN > 0u))
#if BSP_CFG_USBD_EN > 0u
    (CPU_FNCT_VOID)USBD_BSP_RX62N_IntHandler,   /*  36, USB0_D0FIFO0                                   */
    (CPU_FNCT_VOID)USBD_BSP_RX62N_IntHandler,   /*  37, USB0_D1FIFO0                                   */
    (CPU_FNCT_VOID)USBD_BSP_RX62N_IntHandler,   /*  38, USB0_USBI0                                     */
#endif

#if BSP_CFG_USBH_EN > 0u
    (CPU_FNCT_VOID)USBH_RX600_BSP_IntHandler,   /*  36, USB0_D0FIFO0                                   */
    (CPU_FNCT_VOID)USBH_RX600_BSP_IntHandler,   /*  37, USB0_D1FIFO0                                   */
    (CPU_FNCT_VOID)USBH_RX600_BSP_IntHandler,   /*  38, USB0_USBI0                                     */
#endif

#else
    (CPU_FNCT_VOID)BSP_IntHandler_036,          /*      36 ------------------------------------------- */
    (CPU_FNCT_VOID)BSP_IntHandler_037,          /*      37 ------------------------------------------- */
    (CPU_FNCT_VOID)BSP_IntHandler_038,          /*      38 ------------------------------------------- */
#endif
    (CPU_FNCT_VOID)BSP_IntHandler_039,          /*  39, Reserved                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_040,          /*  40, USB1_D0FIFO1                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_041,          /*  41, USB1_D1FIFO1                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_042,          /*  42, USB1_USBI1                                     */

    (CPU_FNCT_VOID)BSP_IntHandler_043,          /*  43, Reserved                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_044,          /*  44, RSPI0_SPEI0                                    */
    (CPU_FNCT_VOID)BSP_IntHandler_045,          /*  45, RSPI0_SPRI0                                    */
    (CPU_FNCT_VOID)BSP_IntHandler_046,          /*  46, RSPI0_SPTI0                                    */
    (CPU_FNCT_VOID)BSP_IntHandler_047,          /*  47, RSPI0_SPII0                                    */

    (CPU_FNCT_VOID)BSP_IntHandler_048,          /*  48, RSPI1_SPEI1                                    */
    (CPU_FNCT_VOID)BSP_IntHandler_049,          /*  49, RSPI1_SPRI1                                    */
    (CPU_FNCT_VOID)BSP_IntHandler_050,          /*  50, RSPI1_SPTI1                                    */
    (CPU_FNCT_VOID)BSP_IntHandler_051,          /*  51, RSPI1_SPII1                                    */

    (CPU_FNCT_VOID)BSP_IntHandler_052,          /*  52, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_053,          /*  53, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_054,          /*  54, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_055,          /*  55, Reserved                                       */
#if BSP_CFG_CAN_EN > 0u
    (CPU_FNCT_VOID)RX600_CAN_NSHandler,         /*  56, CAN0 ERS0                                      */
    (CPU_FNCT_VOID)RX600_CAN_RxHandler,         /*  57, CAN0 RXF0                                      */
    (CPU_FNCT_VOID)RX600_CAN_TxHandler,         /*  58, CAN0 TXF0                                      */
    (CPU_FNCT_VOID)RX600_CAN_RxHandler,         /*  59, CAN0 RXM0                                      */    
    (CPU_FNCT_VOID)RX600_CAN_TxHandler,         /*  60, CAN0 TXM0                                      */
#else
    (CPU_FNCT_VOID)BSP_IntHandler_056,          /*      56 ------------------------------------------- */
    (CPU_FNCT_VOID)BSP_IntHandler_057,          /*      57 ------------------------------------------- */
    (CPU_FNCT_VOID)BSP_IntHandler_058,          /*      58 ------------------------------------------- */
    (CPU_FNCT_VOID)BSP_IntHandler_059,          /*      59 ------------------------------------------- */
    (CPU_FNCT_VOID)BSP_IntHandler_060,          /*      60 ------------------------------------------- */
#endif
    (CPU_FNCT_VOID)BSP_IntHandler_061,          /*  61, Reserved                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_062,          /*  62, RTC_PRD                                        */
    (CPU_FNCT_VOID)BSP_IntHandler_063,          /*  63, RTC_CUP                                        */

    (CPU_FNCT_VOID)BSP_IntHandler_064,          /*  64, IRQ0                                           */
    (CPU_FNCT_VOID)BSP_IntHandler_065,          /*  65, IRQ1                                           */
    (CPU_FNCT_VOID)BSP_IntHandler_066,          /*  66, IRQ2                                           */
    (CPU_FNCT_VOID)BSP_IntHandler_067,          /*  67, IRQ3                                           */
    (CPU_FNCT_VOID)BSP_IntHandler_068,          /*  68, IRQ4                                           */
    (CPU_FNCT_VOID)BSP_IntHandler_069,          /*  69, IRQ5                                           */
    (CPU_FNCT_VOID)BSP_IntHandler_070,          /*  70, IRQ6                                           */
    (CPU_FNCT_VOID)BSP_IntHandler_071,          /*  71, IRQ7                                           */
    (CPU_FNCT_VOID)BSP_IntHandler_072,          /*  72, IRQ8                                           */
    (CPU_FNCT_VOID)BSP_IntHandler_073,          /*  73, IRQ9                                           */
    (CPU_FNCT_VOID)BSP_IntHandler_074,          /*  74, IRQ10                                          */
    (CPU_FNCT_VOID)BSP_IntHandler_075,          /*  75, IRQ11                                          */
    (CPU_FNCT_VOID)BSP_IntHandler_076,          /*  76, IRQ12                                          */
    (CPU_FNCT_VOID)BSP_IntHandler_077,          /*  77, IRQ13                                          */
    (CPU_FNCT_VOID)BSP_IntHandler_078,          /*  78, IRQ14                                          */
    (CPU_FNCT_VOID)BSP_IntHandler_079,          /*  79, IRQ15                                          */

    (CPU_FNCT_VOID)BSP_IntHandler_080,          /*  80, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_081,          /*  81, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_082,          /*  82, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_083,          /*  83, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_084,          /*  84, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_085,          /*  85, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_086,          /*  86, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_087,          /*  87, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_088,          /*  88, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_089,          /*  89, Reserved                                       */

#if ((BSP_CFG_USBD_EN > 0u) || (BSP_CFG_USBH_EN > 0u))
#if BSP_CFG_USBD_EN > 0u
    (CPU_FNCT_VOID)USBD_BSP_RX62N_IntHandler,   /*  90, USB_USBR0                                      */
#endif

#if BSP_CFG_USBH_EN > 0u
    (CPU_FNCT_VOID)USBH_RX600_BSP_IntHandler,   /*  90, USB_USBR0                                      */
#endif

#else
    (CPU_FNCT_VOID)BSP_IntHandler_090,          /*      90 ------------------------------------------- */
#endif

    (CPU_FNCT_VOID)BSP_IntHandler_091,          /*  91, USB_USBR1                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_092,          /*  92, RTC_ALM                                        */

    (CPU_FNCT_VOID)BSP_IntHandler_093,          /*  93, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_094,          /*  94, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_095,          /*  95, Reserved                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_096,          /*  96, WDT_WOVI                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_097,          /*  97, Reserved                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_098,          /*  98, AD0_ADI0                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_099,          /*  99, AD1_ADI1                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_100,          /* 100, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_101,          /* 101, Reserved                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_102,          /* 102, S12AD_S12ADI0                                  */

    (CPU_FNCT_VOID)BSP_IntHandler_103,          /* 103, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_104,          /* 104, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_105,          /* 105, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_106,          /* 106, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_107,          /* 107, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_108,          /* 108, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_109,          /* 109, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_110,          /* 110, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_111,          /* 111, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_112,          /* 112, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_113,          /* 113, Reserved                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_114,          /* 114, MTU0_TGIA0                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_115,          /* 115, MTU0_TGIB0                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_116,          /* 116, MTU0_TGIC0                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_117,          /* 117, MTU0_TGID0                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_118,          /* 118, MTU0_TGIV0                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_119,          /* 119, MTU0_TGIE0                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_120,          /* 120, MTU0_TGIF0                                     */

    (CPU_FNCT_VOID)BSP_IntHandler_121,          /* 121, MTU1_TGIA1                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_122,          /* 122, MTU1_TGIB1                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_123,          /* 123, MTU1_TGIV1                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_124,          /* 124, MTU1_TGIU1                                     */

    (CPU_FNCT_VOID)BSP_IntHandler_125,          /* 125, MTU2_TGIA2                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_126,          /* 126, MTU2_TGIB2                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_127,          /* 127, MTU2_TGIV2                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_128,          /* 128, MTU2_TGIU2                                     */

    (CPU_FNCT_VOID)BSP_IntHandler_129,          /* 129, MTU3_TGIA3                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_130,          /* 130, MTU3_TGIB3                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_131,          /* 131, MTU3_TGIC3                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_132,          /* 132, MTU3_TGID3                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_133,          /* 133, MTU3_TGIV3                                     */

    (CPU_FNCT_VOID)BSP_IntHandler_134,          /* 134, MTU4_TGIA4                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_135,          /* 135, MTU4_TGIB4                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_136,          /* 136, MTU4_TGIC4                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_137,          /* 137, MTU4_TGID4                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_138,          /* 138, MTU4_TGIV4                                     */

    (CPU_FNCT_VOID)BSP_IntHandler_139,          /* 139, MTU5_TGIU5                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_140,          /* 140, MTU5_TGIV5                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_141,          /* 141, MTU5_TGIW5                                     */

    (CPU_FNCT_VOID)BSP_IntHandler_142,          /* 142, MTU6_TGIA6                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_143,          /* 143, MTU6_TGIB6                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_144,          /* 144, MTU6_TGIC6                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_145,          /* 145, MTU6_TGID6                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_146,          /* 146, MTU6_TGIV6                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_147,          /* 147, MTU6_TGIE6                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_148,          /* 148, MTU6_TGIF6                                     */

    (CPU_FNCT_VOID)BSP_IntHandler_149,          /* 149, MTU7_TGIA7                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_150,          /* 150, MTU7_TGIB7                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_151,          /* 151, MTU7_TGIV7                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_152,          /* 152, MTU7_TGIU7                                     */

    (CPU_FNCT_VOID)BSP_IntHandler_153,          /* 153, MTU8_TGIA8                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_154,          /* 154, MTU8_TGIB8                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_155,          /* 155, MTU8_TGIV8                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_156,          /* 156, MTU8_TGIU8                                     */

    (CPU_FNCT_VOID)BSP_IntHandler_157,          /* 157, MTU9_TGIA9                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_158,          /* 158, MTU9_TGIB9                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_159,          /* 159, MTU9_TGIC9                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_160,          /* 160, MTU9_TGID9                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_161,          /* 161, MTU9_TGIV9                                     */

    (CPU_FNCT_VOID)BSP_IntHandler_162,          /* 162, MTU10_TGIA10                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_163,          /* 163, MTU10_TGIB10                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_164,          /* 164, MTU10_TGIC10                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_165,          /* 165, MTU10_TGID10                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_166,          /* 166, MTU10_TGIV10                                   */

    (CPU_FNCT_VOID)BSP_IntHandler_167,          /* 167, MTU11_TGIU11                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_168,          /* 168, MTU11_TGIV11                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_169,          /* 169, MTU11_TGIW11                                   */

    (CPU_FNCT_VOID)BSP_IntHandler_170,          /* 170, POE_OEI1                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_171,          /* 171, POE_OEI2                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_172,          /* 172, POE_OEI3                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_173,          /* 173, POE_OEI4                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_174,          /* 174, TMR0_CMI0A                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_175,          /* 175, TMR0_CMI0B                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_176,          /* 176, TMR0_OV0I                                      */

    (CPU_FNCT_VOID)BSP_IntHandler_177,          /* 177, TMR1_CMI1A                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_178,          /* 178, TMR1_CMI1B                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_179,          /* 179, TMR1_OV1I                                      */

    (CPU_FNCT_VOID)BSP_IntHandler_180,          /* 180, TMR2_CMI2A                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_181,          /* 181, TMR2_CMI2B                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_182,          /* 182, TMR2_OV2I                                      */

    (CPU_FNCT_VOID)BSP_IntHandler_183,          /* 183, TMR3_CMI3A                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_184,          /* 184, TMR3_CMI3B                                     */
    (CPU_FNCT_VOID)BSP_IntHandler_185,          /* 185, TMR3_OV3I                                      */

    (CPU_FNCT_VOID)BSP_IntHandler_186,          /* 186, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_187,          /* 187, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_188,          /* 188, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_189,          /* 189, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_190,          /* 190, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_191,          /* 191, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_192,          /* 192, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_193,          /* 193, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_194,          /* 194, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_195,          /* 195, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_196,          /* 196, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_197,          /* 197, Reserved                                       */

#if BSP_CFG_NET_EN > 0u
    (CPU_FNCT_VOID)NetDev_ISR,                  /* 198, DMACA_DMACI0 - DMA Controller for Ethernet     */
#else
    (CPU_FNCT_VOID)BSP_IntHandler_198,          /*      198 ------------------------------------------ */
#endif

    (CPU_FNCT_VOID)BSP_IntHandler_199,          /* 199, DMACA_DMACI1                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_200,          /* 200, DMACA_DMACI2                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_201,          /* 201, DMACA_DMACI3                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_202,          /* 202, EXDMAC_EXDMAC0I                                */
    (CPU_FNCT_VOID)BSP_IntHandler_203,          /* 203, EXDMAC_EXDMAC0I                                */

    (CPU_FNCT_VOID)BSP_IntHandler_204,          /* 204, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_205,          /* 205, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_206,          /* 206, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_207,          /* 207, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_208,          /* 208, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_209,          /* 209, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_210,          /* 210, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_211,          /* 211, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_212,          /* 212, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_213,          /* 213, Reserved                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_214,          /* 214, SCI0_ERI0                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_215,          /* 215, SCI0_RXI0                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_216,          /* 216, SCI0_TXI0                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_217,          /* 217, SCI0_TEI0                                      */

    (CPU_FNCT_VOID)BSP_IntHandler_218,          /* 218, SCI1_ERI1                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_219,          /* 219, SCI1_RXI1                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_220,          /* 220, SCI1_TXI1                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_221,          /* 221, SCI1_TEI1                                      */

#if BSP_CFG_SER_EN > 0u
    (CPU_FNCT_VOID)BSP_Ser_ISR_Err_Handler,     /* 222, SCI2_ERI2 - UART Rx Error Interrupts           */
    (CPU_FNCT_VOID)BSP_Ser_ISR_Rx_Handler,      /* 223, SCI2_RXI2 - UART Rx Interrupts                 */
    (CPU_FNCT_VOID)BSP_Ser_ISR_Tx_Handler,      /* 224, SCI2_TXI2 - UART Tx Interrupts                 */
    (CPU_FNCT_VOID)BSP_Ser_ISR_Tx_Handler,      /* 225, SCI2_TEI2 - UART Tx Error Interrupts           */
#else
    (CPU_FNCT_VOID)BSP_IntHandler_222,          /*      222 ------------------------------------------ */
    (CPU_FNCT_VOID)BSP_IntHandler_223,          /*      223 ------------------------------------------ */
    (CPU_FNCT_VOID)BSP_IntHandler_224,          /*      224 ------------------------------------------ */
    (CPU_FNCT_VOID)BSP_IntHandler_225,          /*      225 ------------------------------------------ */
#endif
    (CPU_FNCT_VOID)BSP_IntHandler_226,          /* 226, SCI3_ERI3                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_227,          /* 227, SCI3_RXI3                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_228,          /* 228, SCI3_TXI3                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_229,          /* 229, SCI3_TEI3                                      */

    (CPU_FNCT_VOID)BSP_IntHandler_230,          /* 230, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_231,          /* 231, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_232,          /* 232, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_233,          /* 233, Reserved                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_234,          /* 234, SCI5_ERI5                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_235,          /* 235, SCI5_RXI5                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_236,          /* 236, SCI5_TXI5                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_237,          /* 237, SCI5_TEI5                                      */

    (CPU_FNCT_VOID)BSP_IntHandler_238,          /* 238, SCI6_ERI6                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_239,          /* 239, SCI6_RXI6                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_240,          /* 240, SCI6_TXI6                                      */
    (CPU_FNCT_VOID)BSP_IntHandler_241,          /* 241, SCI6_TEI6                                      */

    (CPU_FNCT_VOID)BSP_IntHandler_242,          /* 242, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_243,          /* 243, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_244,          /* 244, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_245,          /* 245, Reserved                                       */

    (CPU_FNCT_VOID)BSP_IntHandler_246,          /* 246, RIIC0_ICEEI0                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_247,          /* 247, RIIC0_ICRXI0                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_248,          /* 248, RIIC0_ICTXI0                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_249,          /* 249, RIIC0_ICTEI0                                   */

    (CPU_FNCT_VOID)BSP_IntHandler_250,          /* 250, RIIC1_ICEEI1                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_251,          /* 251, RIIC1_ICRXI1                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_252,          /* 252, RIIC1_ICTXI1                                   */
    (CPU_FNCT_VOID)BSP_IntHandler_253,          /* 253, RIIC1_ICTEI1                                   */

    (CPU_FNCT_VOID)BSP_IntHandler_254,          /* 254, Reserved                                       */
    (CPU_FNCT_VOID)BSP_IntHandler_255,          /* 255, Reserved                                       */
};


/*
*********************************************************************************************************
*                                            BSP_IntVectSet()
*
* Description : Assign ISR handler.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               isr         Handler to assign.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void   BSP_IntVectSet          (CPU_INT16U     int_id,
                                CPU_FNCT_VOID  isr)
{
#if  BSP_CFG_INT_VECT_TBL_RAM_EN > 0u
    CPU_SR_ALLOC();


    CPU_INT_VECT_TBL_BASE_SET((CPU_INT32U)&BSP_IntVectTbl[0]);
    if (int_id < BSP_CFG_INT_VECT_TBL_SIZE) {
        CPU_CRITICAL_ENTER();
        BSP_IntVectTbl[int_id] = isr;
        CPU_CRITICAL_EXIT();
    }
#else
    (void)&int_id;
    (void)&isr;
    CPU_INT_VECT_TBL_BASE_SET((CPU_INT32U)&BSP_IntVectTbl[0]);
#endif
}


/*
*********************************************************************************************************
*                                          BSP_IntVectTblInit()
*
* Description : Sets up the interrupt vector table base address of the CPU.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntVectTblInit (void)
{
    CPU_INT_VECT_TBL_BASE_SET((CPU_INT32U)&BSP_IntVectTbl[0]);
}


/*
*********************************************************************************************************
*                                        BSP_IntHandler_xxx()
*
* Description : Dummy interrupt handlers.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntHandler_000 (void)
{
    while (1);
}


void  BSP_IntHandler_001 (void)
{
    while (1);
}


void  BSP_IntHandler_002 (void)
{
    while (1);
}


void  BSP_IntHandler_003 (void)
{
    while (1);
}


void  BSP_IntHandler_004 (void)
{
    while (1);
}


void  BSP_IntHandler_005 (void)
{
    while (1);
}


void  BSP_IntHandler_006 (void)
{
    while (1);
}


void  BSP_IntHandler_007 (void)
{
    while (1);
}


void  BSP_IntHandler_008 (void)
{
    while (1);
}


void  BSP_IntHandler_009 (void)
{
    while (1);
}


void  BSP_IntHandler_010 (void)
{
    while (1);
}


void  BSP_IntHandler_011 (void)
{
    while (1);
}


void  BSP_IntHandler_012 (void)
{
    while (1);
}


void  BSP_IntHandler_013 (void)
{
    while (1);
}


void  BSP_IntHandler_014 (void)
{
    while (1);
}


void  BSP_IntHandler_015 (void)
{
    while (1);
}


void  BSP_IntHandler_016 (void)
{
    while (1);
}


void  BSP_IntHandler_017 (void)
{
    while (1);
}


void  BSP_IntHandler_018 (void)
{
    while (1);
}


void  BSP_IntHandler_019 (void)
{
    while (1);
}


void  BSP_IntHandler_020 (void)
{
    while (1);
}


void  BSP_IntHandler_021 (void)
{
    while (1);
}


void  BSP_IntHandler_022 (void)
{
    while (1);
}


void  BSP_IntHandler_023 (void)
{
    while (1);
}


void  BSP_IntHandler_024 (void)
{
    while (1);
}


void  BSP_IntHandler_025 (void)
{
    while (1);
}


void  BSP_IntHandler_026 (void)
{
    while (1);
}


void  BSP_IntHandler_027 (void)
{
    while (1);
}


void  BSP_IntHandler_028 (void)
{
    while (1);
}


void  BSP_IntHandler_029 (void)
{
    while (1);
}


void  BSP_IntHandler_030 (void)
{
    while (1);
}


void  BSP_IntHandler_031 (void)
{
    while (1);
}


void  BSP_IntHandler_032 (void)
{
    while (1);
}


void  BSP_IntHandler_033 (void)
{
    while (1);
}


void  BSP_IntHandler_034 (void)
{
    while (1);
}


void  BSP_IntHandler_035 (void)
{
    while (1);
}


void  BSP_IntHandler_036 (void)
{
    while (1);
}


void  BSP_IntHandler_037 (void)
{
    while (1);
}


void  BSP_IntHandler_038 (void)
{
    while (1);
}


void  BSP_IntHandler_039 (void)
{
    while (1);
}


void  BSP_IntHandler_040 (void)
{
    while (1);
}


void  BSP_IntHandler_041 (void)
{
    while (1);
}


void  BSP_IntHandler_042 (void)
{
    while (1);
}


void  BSP_IntHandler_043 (void)
{
    while (1);
}


void  BSP_IntHandler_044 (void)
{
    while (1);
}


void  BSP_IntHandler_045 (void)
{
    while (1);
}


void  BSP_IntHandler_046 (void)
{
    while (1);
}


void  BSP_IntHandler_047 (void)
{
    while (1);
}


void  BSP_IntHandler_048 (void)
{
    while (1);
}


void  BSP_IntHandler_049 (void)
{
    while (1);
}


void  BSP_IntHandler_050 (void)
{
    while (1);
}


void  BSP_IntHandler_051 (void)
{
    while (1);
}


void  BSP_IntHandler_052 (void)
{
    while (1);
}


void  BSP_IntHandler_053 (void)
{
    while (1);
}


void  BSP_IntHandler_054 (void)
{
    while (1);
}


void  BSP_IntHandler_055 (void)
{
    while (1);
}


void  BSP_IntHandler_056 (void)
{
    while (1);
}


void  BSP_IntHandler_057 (void)
{
    while (1);
}


void  BSP_IntHandler_058 (void)
{
    while (1);
}


void  BSP_IntHandler_059 (void)
{
    while (1);
}


void  BSP_IntHandler_060 (void)
{
    while (1);
}


void  BSP_IntHandler_061 (void)
{
    while (1);
}


void  BSP_IntHandler_062 (void)
{
    while (1);
}


void  BSP_IntHandler_063 (void)
{
    while (1);
}


void  BSP_IntHandler_064 (void)
{
    while (1);
}


void  BSP_IntHandler_065 (void)
{
    while (1);
}


void  BSP_IntHandler_066 (void)
{
    while (1);
}


void  BSP_IntHandler_067 (void)
{
    while (1);
}


void  BSP_IntHandler_068 (void)
{
    while (1);
}


void  BSP_IntHandler_069 (void)
{
    while (1);
}


void  BSP_IntHandler_070 (void)
{
    while (1);
}


void  BSP_IntHandler_071 (void)
{
    while (1);
}


void  BSP_IntHandler_072 (void)
{
    while (1);
}


void  BSP_IntHandler_073 (void)
{
    while (1);
}


void  BSP_IntHandler_074 (void)
{
    while (1);
}


void  BSP_IntHandler_075 (void)
{
    while (1);
}


void  BSP_IntHandler_076 (void)
{
    while (1);
}


void  BSP_IntHandler_077 (void)
{
    while (1);
}


void  BSP_IntHandler_078 (void)
{
    while (1);
}


void  BSP_IntHandler_079 (void)
{
    while (1);
}


void  BSP_IntHandler_080 (void)
{
    while (1);
}


void  BSP_IntHandler_081 (void)
{
    while (1);
}


void  BSP_IntHandler_082 (void)
{
    while (1);
}


void  BSP_IntHandler_083 (void)
{
    while (1);
}


void  BSP_IntHandler_084 (void)
{
    while (1);
}


void  BSP_IntHandler_085 (void)
{
    while (1);
}


void  BSP_IntHandler_086 (void)
{
    while (1);
}


void  BSP_IntHandler_087 (void)
{
    while (1);
}


void  BSP_IntHandler_088 (void)
{
    while (1);
}


void  BSP_IntHandler_089 (void)
{
    while (1);
}


void  BSP_IntHandler_090 (void)
{
    while (1);
}


void  BSP_IntHandler_091 (void)
{
    while (1);
}


void  BSP_IntHandler_092 (void)
{
    while (1);
}


void  BSP_IntHandler_093 (void)
{
    while (1);
}


void  BSP_IntHandler_094 (void)
{
    while (1);
}


void  BSP_IntHandler_095 (void)
{
    while (1);
}


void  BSP_IntHandler_096 (void)
{
    while (1);
}


void  BSP_IntHandler_097 (void)
{
    while (1);
}


void  BSP_IntHandler_098 (void)
{
    while (1);
}


void  BSP_IntHandler_099 (void)
{
    while (1);
}


void  BSP_IntHandler_100 (void)
{
    while (1);
}


void  BSP_IntHandler_101 (void)
{
    while (1);
}


void  BSP_IntHandler_102 (void)
{
    while (1);
}


void  BSP_IntHandler_103 (void)
{
    while (1);
}


void  BSP_IntHandler_104 (void)
{
    while (1);
}


void  BSP_IntHandler_105 (void)
{
    while (1);
}


void  BSP_IntHandler_106 (void)
{
    while (1);
}


void  BSP_IntHandler_107 (void)
{
    while (1);
}


void  BSP_IntHandler_108 (void)
{
    while (1);
}


void  BSP_IntHandler_109 (void)
{
    while (1);
}


void  BSP_IntHandler_110 (void)
{
    while (1);
}


void  BSP_IntHandler_111 (void)
{
    while (1);
}


void  BSP_IntHandler_112 (void)
{
    while (1);
}


void  BSP_IntHandler_113 (void)
{
    while (1);
}


void  BSP_IntHandler_114 (void)
{
    while (1);
}


void  BSP_IntHandler_115 (void)
{
    while (1);
}


void  BSP_IntHandler_116 (void)
{
    while (1);
}


void  BSP_IntHandler_117 (void)
{
    while (1);
}


void  BSP_IntHandler_118 (void)
{
    while (1);
}


void  BSP_IntHandler_119 (void)
{
    while (1);
}


void  BSP_IntHandler_120 (void)
{
    while (1);
}


void  BSP_IntHandler_121 (void)
{
    while (1);
}


void  BSP_IntHandler_122 (void)
{
    while (1);
}


void  BSP_IntHandler_123 (void)
{
    while (1);
}


void  BSP_IntHandler_124 (void)
{
    while (1);
}


void  BSP_IntHandler_125 (void)
{
    while (1);
}


void  BSP_IntHandler_126 (void)
{
    while (1);
}


void  BSP_IntHandler_127 (void)
{
    while (1);
}


void  BSP_IntHandler_128 (void)
{
    while (1);
}


void  BSP_IntHandler_129 (void)
{
    while (1);
}


void  BSP_IntHandler_130 (void)
{
    while (1);
}


void  BSP_IntHandler_131 (void)
{
    while (1);
}


void  BSP_IntHandler_132 (void)
{
    while (1);
}


void  BSP_IntHandler_133 (void)
{
    while (1);
}


void  BSP_IntHandler_134 (void)
{
    while (1);
}


void  BSP_IntHandler_135 (void)
{
    while (1);
}


void  BSP_IntHandler_136 (void)
{
    while (1);
}


void  BSP_IntHandler_137 (void)
{
    while (1);
}


void  BSP_IntHandler_138 (void)
{
    while (1);
}


void  BSP_IntHandler_139 (void)
{
    while (1);
}


void  BSP_IntHandler_140 (void)
{
    while (1);
}


void  BSP_IntHandler_141 (void)
{
    while (1);
}


void  BSP_IntHandler_142 (void)
{
    while (1);
}


void  BSP_IntHandler_143 (void)
{
    while (1);
}


void  BSP_IntHandler_144 (void)
{
    while (1);
}


void  BSP_IntHandler_145 (void)
{
    while (1);
}


void  BSP_IntHandler_146 (void)
{
    while (1);
}


void  BSP_IntHandler_147 (void)
{
    while (1);
}


void  BSP_IntHandler_148 (void)
{
    while (1);
}


void  BSP_IntHandler_149 (void)
{
    while (1);
}


void  BSP_IntHandler_150 (void)
{
    while (1);
}


void  BSP_IntHandler_151 (void)
{
    while (1);
}


void  BSP_IntHandler_152 (void)
{
    while (1);
}


void  BSP_IntHandler_153 (void)
{
    while (1);
}


void  BSP_IntHandler_154 (void)
{
    while (1);
}


void  BSP_IntHandler_155 (void)
{
    while (1);
}


void  BSP_IntHandler_156 (void)
{
    while (1);
}


void  BSP_IntHandler_157 (void)
{
    while (1);
}


void  BSP_IntHandler_158 (void)
{
    while (1);
}


void  BSP_IntHandler_159 (void)
{
    while (1);
}


void  BSP_IntHandler_160 (void)
{
    while (1);
}


void  BSP_IntHandler_161 (void)
{
    while (1);
}


void  BSP_IntHandler_162 (void)
{
    while (1);
}


void  BSP_IntHandler_163 (void)
{
    while (1);
}


void  BSP_IntHandler_164 (void)
{
    while (1);
}


void  BSP_IntHandler_165 (void)
{
    while (1);
}


void  BSP_IntHandler_166 (void)
{
    while (1);
}


void  BSP_IntHandler_167 (void)
{
    while (1);
}


void  BSP_IntHandler_168 (void)
{
    while (1);
}


void  BSP_IntHandler_169 (void)
{
    while (1);
}


void  BSP_IntHandler_170 (void)
{
    while (1);
}


void  BSP_IntHandler_171 (void)
{
    while (1);
}


void  BSP_IntHandler_172 (void)
{
    while (1);
}


void  BSP_IntHandler_173 (void)
{
    while (1);
}


void  BSP_IntHandler_174 (void)
{
    while (1);
}


void  BSP_IntHandler_175 (void)
{
    while (1);
}


void  BSP_IntHandler_176 (void)
{
    while (1);
}


void  BSP_IntHandler_177 (void)
{
    while (1);
}


void  BSP_IntHandler_178 (void)
{
    while (1);
}


void  BSP_IntHandler_179 (void)
{
    while (1);
}


void  BSP_IntHandler_180 (void)
{
    while (1);
}


void  BSP_IntHandler_181 (void)
{
    while (1);
}


void  BSP_IntHandler_182 (void)
{
    while (1);
}


void  BSP_IntHandler_183 (void)
{
    while (1);
}


void  BSP_IntHandler_184 (void)
{
    while (1);
}


void  BSP_IntHandler_185 (void)
{
    while (1);
}


void  BSP_IntHandler_186 (void)
{
    while (1);
}


void  BSP_IntHandler_187 (void)
{
    while (1);
}


void  BSP_IntHandler_188 (void)
{
    while (1);
}


void  BSP_IntHandler_189 (void)
{
    while (1);
}


void  BSP_IntHandler_190 (void)
{
    while (1);
}


void  BSP_IntHandler_191 (void)
{
    while (1);
}


void  BSP_IntHandler_192 (void)
{
    while (1);
}


void  BSP_IntHandler_193 (void)
{
    while (1);
}


void  BSP_IntHandler_194 (void)
{
    while (1);
}


void  BSP_IntHandler_195 (void)
{
    while (1);
}


void  BSP_IntHandler_196 (void)
{
    while (1);
}


void  BSP_IntHandler_197 (void)
{
    while (1);
}


void  BSP_IntHandler_198 (void)
{
    while (1);
}


void  BSP_IntHandler_199 (void)
{
    while (1);
}


void  BSP_IntHandler_200 (void)
{
    while (1);
}


void  BSP_IntHandler_201 (void)
{
    while (1);
}


void  BSP_IntHandler_202 (void)
{
    while (1);
}


void  BSP_IntHandler_203 (void)
{
    while (1);
}


void  BSP_IntHandler_204 (void)
{
    while (1);
}


void  BSP_IntHandler_205 (void)
{
    while (1);
}


void  BSP_IntHandler_206 (void)
{
    while (1);
}


void  BSP_IntHandler_207 (void)
{
    while (1);
}


void  BSP_IntHandler_208 (void)
{
    while (1);
}


void  BSP_IntHandler_209 (void)
{
    while (1);
}


void  BSP_IntHandler_210 (void)
{
    while (1);
}


void  BSP_IntHandler_211 (void)
{
    while (1);
}


void  BSP_IntHandler_212 (void)
{
    while (1);
}


void  BSP_IntHandler_213 (void)
{
    while (1);
}


void  BSP_IntHandler_214 (void)
{
    while (1);
}


void  BSP_IntHandler_215 (void)
{
    while (1);
}


void  BSP_IntHandler_216 (void)
{
    while (1);
}


void  BSP_IntHandler_217 (void)
{
    while (1);
}


void  BSP_IntHandler_218 (void)
{
    while (1);
}


void  BSP_IntHandler_219 (void)
{
    while (1);
}


void  BSP_IntHandler_220 (void)
{
    while (1);
}


void  BSP_IntHandler_221 (void)
{
    while (1);
}


void  BSP_IntHandler_222 (void)
{
    while (1);
}


void  BSP_IntHandler_223 (void)
{
    while (1);
}


void  BSP_IntHandler_224 (void)
{
    while (1);
}


void  BSP_IntHandler_225 (void)
{
    while (1);
}


void  BSP_IntHandler_226 (void)
{
    while (1);
}


void  BSP_IntHandler_227 (void)
{
    while (1);
}


void  BSP_IntHandler_228 (void)
{
    while (1);
}


void  BSP_IntHandler_229 (void)
{
    while (1);
}


void  BSP_IntHandler_230 (void)
{
    while (1);
}


void  BSP_IntHandler_231 (void)
{
    while (1);
}


void  BSP_IntHandler_232 (void)
{
    while (1);
}


void  BSP_IntHandler_233 (void)
{
    while (1);
}


void  BSP_IntHandler_234 (void)
{
    while (1);
}


void  BSP_IntHandler_235 (void)
{
    while (1);
}


void  BSP_IntHandler_236 (void)
{
    while (1);
}


void  BSP_IntHandler_237 (void)
{
    while (1);
}


void  BSP_IntHandler_238 (void)
{
    while (1);
}


void  BSP_IntHandler_239 (void)
{
    while (1);
}


void  BSP_IntHandler_240 (void)
{
    while (1);
}


void  BSP_IntHandler_241 (void)
{
    while (1);
}


void  BSP_IntHandler_242 (void)
{
    while (1);
}


void  BSP_IntHandler_243 (void)
{
    while (1);
}


void  BSP_IntHandler_244 (void)
{
    while (1);
}


void  BSP_IntHandler_245 (void)
{
    while (1);
}


void  BSP_IntHandler_246 (void)
{
    while (1);
}


void  BSP_IntHandler_247 (void)
{
    while (1);
}


void  BSP_IntHandler_248 (void)
{
    while (1);
}


void  BSP_IntHandler_249 (void)
{
    while (1);
}


void  BSP_IntHandler_250 (void)
{
    while (1);
}


void  BSP_IntHandler_251 (void)
{
    while (1);
}


void  BSP_IntHandler_252 (void)
{
    while (1);
}


void  BSP_IntHandler_253 (void)
{
    while (1);
}


void  BSP_IntHandler_254 (void)
{
    while (1);
}


void  BSP_IntHandler_255 (void)
{
    while (1);
}





