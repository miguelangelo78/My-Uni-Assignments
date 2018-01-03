#ifndef DATABASE_PARSER_H_
#define DATABASE_PARSER_H_

#include <stdlib.h>
#include "../Database_def/database_inc.h"

#define BIRTHDATE_DELIMITER "/"
#define PROPERTY_DELIMITER "|"

// Parsing properties:
typedef enum {
	PROPERTY_ID, PROPERTY_FIRSTNAME, PROPERTY_LASTNAME, PROPERTY_BIRTHDATE,
	PROPERTY_BIRTH_DAY = 0, PROPERTY_BIRTH_MONTH = 1, PROPERTY_BIRTH_YEAR = 2
} Parsing_props_t;

// How many properties there are to be parsed:
#define PROPERTY_COUNT 4

extern Database_Student ** parse_database(char ** database_content, int file_linecount);

#endif