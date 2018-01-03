#define MAX_RECORD_COUNT 100
#define CHOICE_ADD 1
#define CHOICE_DISPLAY_DATABASE 2
#define CHOICE_EXIT 5
int size = 0;

typedef struct{
	int day,month,year;
}Date;

typedef struct{
	char firstName[10], lastName[10], id[10];
	Date birth;
}Record;

Record create_record(int index){
	Date d;
	Record r;

	printf("\n________\nInserting record n. %d:\n________\n-ID: ",index); 
	scanf("%s",r.id);
	printf("-First Name: "); scanf("%s",r.firstName);
	printf("-Last Name: "); scanf("%s",r.lastName);
	printf("-Date of birth -\n   >Day: "); scanf("%d",&d.day);
	printf("   >Month: "); scanf("%d",&d.month);
	printf("   >Year: "); scanf("%d",&d.year);
	r.birth = d;
	return r;
}
void print_record(Record r){
	printf("\n________\nID: %s\nFirst Name: %s\nLast Name: %s\nBirth: %d/%d/%d",
	r.id,r.firstName,r.lastName,r.birth.day,r.birth.month,r.birth.year);
}

void fill_database(Record * records,int record_count){
	int i;
	for(i=0;i<record_count;i++)
		records[i] = create_record(i+1);
}

void add_record(Record * records){
	records[size] = create_record(size+1);
	size++;
}

void* allocate_database(int record_count){
	return malloc(sizeof(Record)*record_count);
}

void cleanup(Record * r){
	free(r);
}

void display_database(Record * recs,int start,int end){
	int i;
	printf("\nRecord display:");
	for(i=start;i<end;i++)
		print_record(recs[i]);
}

void main(){
	int choice = 0;
	Record * r = allocate_database(MAX_RECORD_COUNT);
	
	while(choice!=CHOICE_EXIT){
		printf("\n1 - Add record\n2 - Display database\n5 - Exit\nChoose: ");
		scanf("%d",&choice);
		
		switch(choice){
			case CHOICE_ADD: 
				add_record(r); break;
			case CHOICE_DISPLAY_DATABASE: 
				display_database(r,0,size); break;
			default: break;// Do nothing for now
		}
	}
	cleanup(r);
	getch();
}
