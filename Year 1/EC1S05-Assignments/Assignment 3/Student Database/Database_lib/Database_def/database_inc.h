#ifndef DATABASE_STRUCT_H_
#define DATABASE_STRUCT_H_

// Definition of the max sizes of the strings:
#define FIRST_NAME_MAXSIZE	15
#define LAST_NAME_MAXSIZE	15
#define ID_MAXSIZE			10
#define DAY_MAXSIZE			3
#define MONTH_MAXSIZE		3
#define YEAR_MAXSIZE		5
#define WHOLEBIRTH_MAXSIZE DAY_MAXSIZE+MONTH_MAXSIZE+YEAR_MAXSIZE+3 // +3 because of the '/'

// Structs definitions:
typedef struct{
	char day	[DAY_MAXSIZE];
	char month	[MONTH_MAXSIZE];
	char year	[YEAR_MAXSIZE];
} Date;

typedef struct {
	char firstName	[FIRST_NAME_MAXSIZE];
	char lastName	[LAST_NAME_MAXSIZE];
	char id			[ID_MAXSIZE];
	Date birthDate;
} Database_Student;

#endif