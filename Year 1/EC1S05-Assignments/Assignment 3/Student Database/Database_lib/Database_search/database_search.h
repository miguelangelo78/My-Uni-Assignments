#ifndef DATABASE_SEARCH_H_
#define DATABASE_SEARCH_H_

#include "../Database_record_list/linked_list.h"
#include "../Misc/misc.h"

// Declare search masks:

// Filter Structure: 0b 0 0 0 0 0 0 0 0 0 (9 bits)
// BIT STRUCTURE:
	// 1st 3 bits (000_): Search mode
	// 4th (0_ 000): Case sensitive enable
	// 5th and 6th (00_ 0000): Search only for first or last letters (NOT both)
	// 7th and 8th: (00_ 000000): Match if keyword is equal, greater or less than
	// 9th (0_ 00000000): Invert match so it matches all opposites of the previous filters

// Search modes:
#define SEARCH_MODE_MASK 7 // 0b111
#define SEARCH_MODE_ID 0 //0b000
#define SEARCH_MODE_FIRSTNAME 1 //0b001
#define SEARCH_MODE_LASTNAME 2 //0b010
#define SEARCH_MODE_BIRTH_WHOLE 3 //0b011
#define SEARCH_MODE_BIRTH_DAY 4 //0b100
#define SEARCH_MODE_BIRTH_MONTH 5 //0b101
#define SEARCH_MODE_BIRTH_YEAR 6 //0b110
#define SEARCH_MODE_NULL_MSG "<NULL MODE>"

// Filters:
#define SEARCH_FILTER_CASE 8 //0b1000
#define SEARCH_FILTER_FIRSTLETTER 16 //0b010000
#define SEARCH_FILTER_LASTLETTER 32 //0b100000
#define SEARCH_FILTER_EQUAL 0 //0b00000000 // Turned on by default
#define SEARCH_FILTER_GREATER 64 // 0b01000000
#define SEARCH_FILTER_LESS 128 //0b10000000
#define SEARCH_FILTER_INV 256 //0b100000000
#define SEARCH_FILTER_DEL_DAY 512 // 0b1000000000
#define SEARCH_FILTER_DEL_MONTH 1024 // 0b10000000000
#define SEARCH_FILTER_DEL_YEAR 2048 // 0b100000000000

extern int * database_query(int search_props, char * keyword);

#endif
