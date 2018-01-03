#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define INDEX_CHARS 0
#define INDEX_LINES 1
#define INDEX_SPACE 2
#define INDEX_TABS  3
#define PROPERTY_COUNT 4

#define NEW_LINE 10
#define SPACE ' '
#define TAB '\t'

int * get_filedata(char * filename){
	FILE * f = fopen(filename,"r");
	int * data_container = NULL , i;
	char ch;

	if(f!=NULL){
		data_container = malloc(sizeof(int) * PROPERTY_COUNT);
		for(i=0;i<PROPERTY_COUNT;i++) data_container[i] = 0;

		data_container[INDEX_LINES]++;

		while((ch = fgetc(f))!=EOF){
			data_container[INDEX_CHARS]++;		
			if(ch==NEW_LINE) data_container[INDEX_LINES]++;
			if(ch==SPACE)	 data_container[INDEX_SPACE]++;
			if(ch==TAB)		 data_container[INDEX_TABS] ++;
		}
		fclose(f);
	}

	return data_container;
}

void main(){
	int * data;
	char filename[50];

	printf("Filename: ");
	scanf("%s",filename);

	if((data = get_filedata(filename))==NULL){
		printf("File '%s' does not exist\n",filename);
	}else{
		// TODO: Show data
		printf("Character count: %d\n", data[INDEX_CHARS]);
		printf("Line count: %d\n",	    data[INDEX_LINES]);
		printf("Space count: %d\n",     data[INDEX_SPACE]);
		printf("Tab count: %d\n",       data[INDEX_TABS]);
	}

	if(data!=NULL)
		free(data);
	getch();
}
