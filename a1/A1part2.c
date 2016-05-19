/**
* Aron Lawrence 2240
* A1 Part 2
* this program reads in the index array from a bianry file
* allows the user to search for a record by country
* then if found open the records bin file and prints the wanted record

**/

#include "apue.h" // Includes most standard libraries 
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>	

#define NUMCOUNTRIES 239

typedef struct{
	char 	code[4];
	int 	offset;
}countryindex;

countryindex** countryIndex;

void *GimmeANode(int filDes);
int BinarySearch(int countryLines, char* key);
void UserInput(int totalcountries);
void printStruct(int LineCount);
void printOneRecord(int returnedKey);


int main(int argc, char *argv[])
{
	int i;
	countryIndex = malloc(239*sizeof(countryindex*));
	int fd  = open("index.bin", O_RDONLY);
	if(fd < 0){
		err_sys("failed open!");
	}
	GimmeANode(fd);
	for(i=0; i < NUMCOUNTRIES; i++){
		free(countryIndex[i]);
	}
	int closflie = close(fd);
	free(countryIndex);
	exit(0);
}

//****Reads in the index file and creates the index array**********
void *GimmeANode(int filDes){
		static char charBuf[4];
		static int offsetbuff;
		static int MyEOf = -1;
		static char *String;
		int i,j;
		static int readstuff = 0;
		for( i=0; i<239; i++){
			countryIndex[i]=malloc(sizeof(countryindex));
	
		readstuff =read(filDes, charBuf, 4);
	
		readstuff=read(filDes, &offsetbuff, sizeof(int));


		String = charBuf;

		strncpy(countryIndex[i]->code, String, strlen(String));
		countryIndex[i]->offset = offsetbuff;

		

	}
	//printStruct(NUMCOUNTRIES);
	UserInput(NUMCOUNTRIES);

}
//*****Basic binary search based off country code********************
int BinarySearch(int countryLines, char* key) {
	int offSet = -1;
	int count = countryLines-1;
	int min = 0, max = count, mid;

	//printf("the key is %c%c%c\n", key[0], key[1],key[2]); //checking to see if the right key got passes
	
	//basic BinarySearch but checks the str as opposed to an int
	while(min <= max){
		mid = ( min + max)/2;
		if(strncmp(countryIndex[mid]->code, key, 3) < 0){
			min = mid + 1;
		}
		else if(strncmp(countryIndex[mid]->code, key, 3) == 0){
			//printf("here here here");
			offSet = countryIndex[mid]->offset;  //i know we shouldnt return twice form a method but this made more sense then:
			 min = max+10;
			//offSet = 10;
			
		}
		else if(strncmp(countryIndex[mid]->code, key, 3) > 0){
			max = mid - 1;
		}
	}
	//printf("index location is %d\n", indexLocation);
	return offSet; //index location gets returned and eventually snet to readOneRecord
}
//******gets user input and loops********
void UserInput(int totalcountries){
	char *wantedKey = malloc(3 * sizeof(char*));   //allocates space for the users input
	int returnedkey = -1;
	char inbuf[5];
	int n;

			printf("Enter a country code to see its information.\n");
			printf("Enter a country code in all caps, or 000 to exit\n");
			//printf("\n");
			printf("User Input:    \n");
			//fsync(1);
			read(0,inbuf,sizeof(inbuf)-1);
			inbuf[4]= '\0';
			printf("\n");
			while((strncmp(inbuf, "000", 3) != 0  )|| inbuf[3]!= '\n'){
				//printf("here\n");
				if(inbuf[3] == '\n'){
				//	printf("Now here\n");
				returnedkey = BinarySearch(totalcountries, inbuf);
			}
					if (returnedkey != -1){  //if the record was found prints the record
							printf("\n");	
							printf("Printing returned struct\n");
							printOneRecord(returnedkey);
							printf("May I have another, sir?\n");
			} else{
				printf("Country Not Found. May I have another, sir?\n");
			}	
				returnedkey = -1;
                printf("USER INPUT:  \n"); 
      
                read(0,inbuf,sizeof(inbuf)-1); 
                inbuf[4]= '\0';   
            }
            free(wantedKey);

}
//*******print index array for testing************
void printStruct(int LineCount){

	int i;
	for(i = 0; i < LineCount; i++)
	{
		printf("%c%c%c, %i \n", countryIndex[i]->code[0], countryIndex[i]->code[1], countryIndex[i]->code[2],
			 countryIndex[i]->offset);
	}

}
//****opend the records file and seeks to correct offset, reads ina nd prints wanted info******
void printOneRecord(int returnedKey){
	
int fd  = open("records.bin", O_RDONLY);
	if(fd < 0){
		err_sys("failed open!");
	}
	int offset = returnedKey;
	 char charBuf[4];
	 int lengthBuf;
	
	 int popBuf;
	 float lifeBuf;


	lseek(fd, offset, 0);

	int readRecord = read(fd, charBuf, 4);
	readRecord = read(fd, &lengthBuf, sizeof(int));
	char* nameBuffer = malloc(4*lengthBuf*sizeof(char));
	readRecord = read(fd, nameBuffer, lengthBuf);
	//printf("nameBuffer is %s\n", nameBuffer);
	readRecord = read(fd, &popBuf, sizeof(int));
	readRecord = read(fd, &lifeBuf, sizeof(float));

	//char *CodeS = cha

	printf("%c%c%c, %s, %i, %f\n", charBuf[0], charBuf[1], charBuf[2], nameBuffer, popBuf, lifeBuf);
	//printf("Here here here\n");
	free(nameBuffer);
	//printf("Now i am here\n");
	int closing = close(fd);
}