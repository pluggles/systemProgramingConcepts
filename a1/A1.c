/************************************************************
* Aron Lawrence CS2240
* This is part one of A1,
* this program readin in the data file, creates a sorted
* struct of the wanted information, and a matching index array
* then it writes out to two binary files
*
*************************************************************/



#include "apue.h" // Includes most standard libraries 
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>	

#define BUFFSIZE 512
#define FALSE 0
#define OFFSET 





/*
     Define structs
*/
typedef struct
{
	char   code[4]; //made this size four to help with strncpy and strncmp
	char*   name;
	int     pop;
	float   life_e;
}country;

typedef struct{
	char 	code[4];
	int 	offset;
}countryindex;

//*************Prototypes************************************

char *GimmeALine( int FileDes);
country* splitALine(char* record);
void printStruct(country** countryStruct, int numCountries);
void printOneRecord(country** countryStruct, int indexLoc);
void cmpObject(country** countries, int numCountries);
int BinarySearch(country** countries, int numCountries, char* key);  //used for testing
void UserInput(country** countries, int totalcountries);  //used for testing
void writeRecordstoFile(country** countries, int countrySize);
void makeIndexArray(country** countries, int countrySize);
void writeIndextoFile(countryindex** countryIndex, int countrySize);

//***************globals****************************************
//i dont know if i eneded up neededing these to be globals
int offset = 0;  //global
countryindex** countryIndex;

/************main***********************************************/
int main(int argc, char *argv[])
{
	int fd, n, i, LineCount=0, AllotedCountries=50; // file descriptor, count, index, i may have used all these at one point...
	
	//char *Tok;
	char* LinePtr;
	
	country** countries = malloc(50*sizeof(country*)); // memory for you array
	country tempCountry; //static struct to be used later
	fd = open("AllCountries.dat", O_RDONLY);
	if(fd < 0) //males sure your data fil is here
	{
		err_sys("failed open!");
	} else{
		printf("\nFile AllCountries.dat opened\n");
	}

	while((LinePtr = GimmeALine(fd)) !=NULL){ //if there are lines left add them to your array


		if(LineCount >= AllotedCountries){ //if u run out of space give it more
			AllotedCountries += 50;
			countries = realloc(countries, AllotedCountries*sizeof(country*)); //realloc reallocates memory..go figure
		}

		
		countries[LineCount] = splitALine(LinePtr); //takes line, splits it, and adds the temp struct to the end of the countries array

        
        LineCount++;
        if(LineCount >= 1){
        cmpObject(countries, LineCount); //sorts on the fly, once there are at least 2 records
    	}
	} 
		int closing = close(fd);

		printf("Array of records made and sorted\n"); //At one point i deemed this new line neccassary
		//print struct used for testing
		//printStruct(countries, LineCount); //prints all the records


		writeRecordstoFile(countries, LineCount);
		printf("Records written to binary file\n"); //writes all the records to a binary file (I hope)
		
		//user input not needed for this part, used for testing though
		//UserInput(countries, LineCount);  //enters user input loop


		makeIndexArray(countries, LineCount);
		printf("Index array made and sorted\n");

		writeIndextoFile(countryIndex, LineCount);
		printf("Indexarray written to file\n");


		//free your memory
		for(i=0; i < LineCount; i++){
			free(countries[i]->name);
			free(countries[i]);

			free(countryIndex[i]);
		}

		free(countryIndex);
	   free(countries); //since we malloc, we have to free

	   printf("memory has been freed, goodbye.\n\n");
	   exit(0);
}
//this is 100 percent Trenarys GimmeALine method
//
//**********************************************
//**********************************************
/*
Sent a file to read, returns a a single record
*/
char *GimmeALine( int FD)  // GimmeALine is sent open descriptor
// and returns a pointer to a newline terminated
// substring of filled buffer. All reads are done
// within GimmeALine, including the first.
// GimmeAline assumes the last character in file is '\n'
{
    static char Buffer[BUFFSIZE + 1];
    static int MyEof = FALSE;
    static int LineFound;
    static int ReadCount = 0; // initialize to trigger first read
    static int BufferNdx = BUFFSIZE + 1;
    static int TokenNdx;
    static char *ReturnPointer;
    off_t offset; // for lseek() with buffer fragment

    if (BufferNdx >= ReadCount) // No data ?
    {
        ReadCount = read(FD, Buffer, BUFFSIZE);
        BufferNdx = 0;
        MyEof = (ReadCount == 0);
    }
    if (ReadCount < 0) err_sys("Read Less than 0\n");
    //       Here Buffer has data -- or Not

    if (! MyEof) // Now we have some data in Buffer
    {
        // So look for lines
        TokenNdx = BufferNdx; // First line character
        LineFound = FALSE;
        do // Find a Line
        {
            while ((BufferNdx < ReadCount)
                    && (Buffer[BufferNdx] != '\n')) BufferNdx++;
            // Move Ptr Forward To Eoln or BufferEnd ?
            LineFound = (BufferNdx < ReadCount);

            if (! LineFound) // refill buffer
            {
                offset = (off_t)(TokenNdx - BufferNdx);
                lseek(FD, offset, SEEK_CUR);
                ReadCount = read(FD, Buffer, BUFFSIZE);
                BufferNdx = 0; TokenNdx = 0; // reset
                MyEof = (ReadCount == 0);
                if (MyEof) break; // from FindALine
            }
        }
        while (! LineFound);   // find line in filled buffer
    }
    // Here EOF or eoln
    if (MyEof) ReturnPointer = NULL;
    else
    {
        ReturnPointer = &Buffer[TokenNdx];
        Buffer[BufferNdx] = (char) 0; // make "string"
        BufferNdx++; // Get Ready For Next Token
    }

    return ReturnPointer;
}
/*
gets a record string and splits it
tokens of inters are:
1: code
2: name
7: population
8: life expectancy

passed in a record returns a country
*/

country* splitALine(char* record)
{
	char*   token;
	char* 	ptToken;
	
	int     tokenNum = 1;
	country* returnPointer = malloc(sizeof(country));  //creates the temp country struct
	token = strtok_r(record, ",", &ptToken);
 	
	//printf("in SplitALine()\n");
 
	while(strlen(token)!=0 && tokenNum < 10) //cycle through important tokens
	{
		token = strtok_r(NULL, ",", &ptToken);
		switch(tokenNum){ //figures out the important tokens and places them in the struct
			case 1: 
				strncpy(returnPointer->code, token, sizeof(returnPointer->code) + 1);
				break;
			case 2: 
				returnPointer->name=malloc(strlen(token)+1);
				strncpy(returnPointer->name, token, strlen(token) + 1);
				break;
			case 7:
				returnPointer->pop = atoi(token);
				break;
			case 8:
				returnPointer->life_e = atof(token);
				break;
		}
		
		tokenNum++;
	}
	//returnPointer = &returnCountry;
 
	return returnPointer;
}
/*
this will print the whole struct to the terminal

passed in the country array and the size of the array
used for testing 
*/
void printStruct(country** countryStruct, int numCountries)
{
	int i;
	for(i = 0; i < numCountries; i++)
	{
		printf("%c%c%c, %s, %i, %f \n",countryStruct[i]->code[0], countryStruct[i]->code[1], countryStruct[i]->code[2],
			countryStruct[i]->name, countryStruct[i]->pop,countryStruct[i]->life_e);
	}
}


//this prints one reocrd, go figure also used for testing
void printOneRecord(country** countryStruct, int indexLoc)
{
	int i = indexLoc;;
	
		printf("%c%c%c, %s, %i, %f \n",countryStruct[i]->code[0], countryStruct[i]->code[1], countryStruct[i]->code[2],
			countryStruct[i]->name, countryStruct[i]->pop,countryStruct[i]->life_e);
	
}
/*
bit misnamed method
this compares and does the swap, so i guess it should really be named
sort() or something like that

sorts alphebetically by comparing the code
gets called after each new record gets added

gets passed the country array and the size
*/
void cmpObject(country** countries, int numCountries){
int i, j;
int count = numCountries;
country *temp; 


for(i=0; i < count; i++){
	for(j = i+1; j < count; j++){
		if (strcmp(countries[i]->code, countries[j]->code) > 0) {
			//basic swap seems to work well
			temp = countries[i];
			countries[i] = countries[j]; //move j to i
			countries[j] = temp;

			}
		}
	}

}
/*
searces the sorted array for the country code, eventuall will search directory
but for now just searches actual array of records

gets passed the country array, the size of the array the the country code as a string
returns an int that is the index of the countries place in the array
or -1 if the code was not found
*/
int BinarySearch(country** countries, int numCountries, char* key) {
	int indexLocation = -1;
	int count = numCountries-1;
	int min = 0, max = count, mid;
	//printf("the key is %c%c%c\n", key[0], key[1],key[2]); //checking to see if the right key got passes
	
	//basic BinarySearch but checks the str as opposed to an int
	while(min <= max){
		mid = ( min + max)/2;
		if(strncmp(countries[mid]->code, key, 3) < 0){
			min = mid + 1;
		}
		else if(strncmp(countries[mid]->code, key, 3) == 0){
			return mid;  //i know we shouldnt return twice form a method but this made more sense then:
			// min = max+1;
			
		}
		else if(strncmp(countries[mid]->code, key, 3) > 0){
			max = mid - 1;
		}
	}
	//printf("index location is %d\n", indexLocation);
	return indexLocation; //index location gets returned and eventually snet to readOneRecord
}
/*
Infinite loop asking for user input, allows for user to enter country code
and returns the record, if the code is not found -1 is returned
to quit the loop and exit the program enter -1

passes in Country array, and amount of records
in retrospect i should probably have made my record count a global varible
but i dont feel like changing that now

*/
void UserInput(country** countries, int totalcountries){
	char *wantedKey = malloc(3 * sizeof(char*));    //allocates space for the users input
	int returnedkey = -1;
	char inbuf[1000];
	int n;

			printf("Enter a country code to see its information (three letters all caps, enter q to quit)\n");
			//printf("\n");
			printf("User Input:    \n");
			//fsync(1);
			read(0,inbuf,sizeof(inbuf));
			printf("\n");
			while(inbuf[0] != 'q' || inbuf[1]!= '\n'){
				if(inbuf[3] == '\n'){
				returnedkey = BinarySearch(countries, totalcountries, inbuf);
			}
					if (returnedkey != -1){  //if the record was found prints the record
							printf("\n");	
							printf("Printing returned struct\n");
							printOneRecord(countries, returnedkey);
			} else{
				printf("Country Not Found\n");
			}	
				returnedkey = -1;
                printf("USER INPUT:  \n"); 
      
                read(0,inbuf,sizeof(inbuf));    
            }

            free(wantedKey);

}
/*
Should write the country array to a binary file,


Passes in the country array, and the country size
*/
void writeRecordstoFile(country** countries, int countrySize){
	int outFile;
	//outFile = open("records.bin", O_TRUNC | O_WRONLY, S_IWUSR);
	outFile = open("records.bin", O_WRONLY | O_CREAT | O_TRUNC,  S_IRUSR | S_IRGRP | S_IROTH);
	int i;
	int length;
	for(i=0; i<countrySize; i++){
		length = strlen(countries[i]->name);
		write(outFile, countries[i]->code, 4);
		write(outFile, &length, sizeof(int));
		write(outFile, countries[i]->name, strlen(countries[i]->name));
		write(outFile, &countries[i]->pop, sizeof(int));
		write(outFile, &countries[i]->life_e, sizeof(float));

	}
}
//*****Makes the index array, for direct addressing*********
void makeIndexArray(country** countries, int countrySize){
	int i=0;


	countryIndex = malloc(countrySize*sizeof(countryindex*));
	int currentoffset = 0;


	//printf("offset is %d\n", currentoffset);
	

	countryIndex[0] = malloc(sizeof(countryIndex));
		

		countryIndex[0]->offset = 0;
		//printf("Offset is: %d", currentoffset);
		strncpy(countryIndex[0]->code, countries[0]->code, 4);
		currentoffset = (4*sizeof(char)+
			sizeof(int)+strlen(countries[0]->name)+sizeof(int)+sizeof(float));


	for(i=1; i <countrySize; i++){
		countryIndex[i] = malloc(sizeof(countryIndex));
	//	printf("count %d\n", i);

		countryIndex[i]->offset = currentoffset;
		currentoffset += (4*sizeof(char)+
			sizeof(int)+strlen(countries[i]->name)+sizeof(int)+sizeof(float));
		//printf("count %d\n", i);
	//	printf("Offset is: %d", currentoffset);
		strncpy(countryIndex[i]->code, countries[i]->code, 4);
//str

		//printf("%c%c%c, %d \n", countryIndex[i]->code[0], countryIndex[i]->code[1], countryIndex[i]->code[2], countryIndex[i]->offset);
		
	}


	
}
//****writes the inendex array to a bin file
void writeIndextoFile(countryindex** countryIndex, int countrySize){
	int outIndexFile;
	//outIndexFile = open("index.bin", O_TRUNC | O_WRONLY, S_IWUSR);
	outIndexFile = open("index.bin", O_WRONLY | O_CREAT | O_TRUNC,  S_IRUSR | S_IRGRP | S_IROTH);
	int i = 0;
	for(i=0; i<countrySize; i++){
		write(outIndexFile, countryIndex[i]->code, 4);
		write(outIndexFile, &countryIndex[i]->offset, sizeof(int));
	}
}