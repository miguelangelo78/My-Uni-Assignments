/*
 * utils.h
 *
 *  Created on: 15/10/2017
 *      Author: Miguel
 */

#ifndef LIB_CLIB_UTILS_H_
#define LIB_CLIB_UTILS_H_

#include <math/mathutils.h>

#define strfmt(buffer, fmt) do { va_list args; va_start(args, fmt); vsprintf(buffer, fmt, args); va_end(args); } while(0)

#define arraysize(array) (sizeof(array) / sizeof(array[0]))

//////////////////////////////////////////////////////////////////////////
/////////////////////////// Common Bit Masks /////////////////////////////
//////////////////////////////////////////////////////////////////////////
//   X:masked (disabled)  ////////////////////  O:not masked (enabled)  //
//////////////////////////////////////////////////////////////////////////
#define MASK2_2         0x66            //		X O O X  X O O X
#define MASK2_0         0x60            //		X O O X  X X X X
#define MASK0_2         0x06            //		X X X X  X O O X
#define MASK3_3         0xE7            //		O O O X  X O O O
#define MASK0_3         0x07            //		X X X X  X O O O
#define MASK3_0         0xE0            //		O O O X  X X X X
#define MASK4_0         0xF0            //		O O O O  X X X X
#define MASK0_4         0x0F            //		X X X X  O O O O
#define MASK4_4         0xFF            //		O O O O  O O O O
#define MASK4_1         0x1F            //		X X X O  O O O O
#define MASK3_2			0x19			//		X X X O  O X X O
#define MASK1_1         0x18            //		X X X O  O X X X
#define MASKL2_0        0x30            //		X X O O  X X X X
#define MASKL0_2        0x0C            //		X X X X  O O X X

#define b00000000 0x00
#define b11111111 0xFF
#define b00011000 0x18
#define b00001100 0x0C
#define b00000110 0x06
#define b00000011 0x03
#define b00000001 0x01
#define b00110000 0x30
#define b01100000 0x60
#define b11000000 0xC0
#define b10000000 0x80
#define b00011100 0x1C
#define b00011110 0x1E
#define b00011111 0x1F
#define b00111000 0x38
#define b01111000 0x78
#define b11111000 0xF8
#define b10101010 0xAA
#define b01010101 0x55
#define b10000001 0x81
#define b10000011 0x83
#define b11000001 0xC1
#define b11000011 0xC3
#define b11011011 0xDB
#define b00001110 0x0E
#define b00000111 0x07
#define b01110000 0x70
#define b11100000 0xE0
#define b00111111 0x3F
#define b00001111 0x0F
#define b11100111 0xE7
#define b11111100 0xFC
#define b11111110 0xFE
#define b11110000 0xF0

#endif /* LIB_CLIB_UTILS_H_ */
