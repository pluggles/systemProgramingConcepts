/**
* Aron Lawrence
* Threading homework
* this program should "open" a dir get the file names
* sort the contents of all the files using threads
* then sort all the sorted stuff into one file
**/

#include "apue.h"
#include <dirent.h>
#include <stdio.h>
#include <pthread.h>
void * sortFile(void *x);

#define BUFFER 512

typedef struct {
	char *name;
	char *pass;
	char *blood;
	char *domain;
	int index;
	int fileSize;  //allows me to nicely pass around my size of arrays useful for sorting

} records;

typedef struct {
	records sortedRecord;
} collectedArrays;


records* splitALine(char* line);
void printStruct(records** recordStruct, int numRecords);
int compare(const void *ind1, const void *ind2);
void mergeArrays(records ***record, int count);
records** mergeTwo(records **array1, records** array2);
void writeStruct(records** recordStruct, int numRecords);

//***********************************************
//*****Main function, handles more then it should....
//************************************************
int main(int argc, char **argv) {


	DIR *d;
	struct dirent *dir;

	d = opendir(argv[1]);
	int counter = 0; //number of files...which means number of threads...which means size of 2d array
	int i, err;
	//** handles getting the files names **
	char **files = malloc(255 * sizeof(char*));
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (strstr(dir->d_name, ".") == NULL) {
				counter++;
				char temp[512];
				temp[0] = '\0';
				strcpy(temp, argv[1]);
				strcat(temp, "/");  //segfaults if this inst here id dir is entered w/out last slash
				strcat(temp, dir->d_name); //makes sure i can pass full fill path to thread

				files[counter - 1] = malloc(255 * sizeof(char*));
				strcpy(files[counter - 1], temp);

			}
		}


		closedir(d);
	}
	else {
		printf("that was not a valid dir, please enter the directory path as a command line arg\n");
	}

	//*** start of threading stuff *********
	pthread_t tid[counter];
	for (i = 0; i < counter; i++) {

		err = pthread_create(&tid[i], NULL, sortFile, files[i]);
	}

	records **data[counter];

	for (i = 0; i < counter; i++) {
		err = pthread_join(tid[i], (void**) &data[i]);
	}
	//end of threading stuff

	mergeArrays(data, counter);
	for(i=0; i <counter; i++){
		free(files[i]);
	}
	free(files);

}
//********************************************************
//*****This will open, read, populate and sort a struct***
//********************************************************
void * sortFile(void *x) {
	char* file;
	file = (char*)x;
	FILE *fp;
	fp = fopen(file, "r");

	int indexSpot = 0;
	int arraySize = BUFFER;
	records** recordArray = malloc(arraySize * sizeof(records*));


	char line[512];
	while (fgets(line, 512, fp ) != NULL) {
		if (indexSpot >= arraySize ) {
			arraySize += BUFFER;
			recordArray = realloc(recordArray, arraySize * sizeof(records*));
		}

		recordArray[indexSpot] = splitALine(line);
		indexSpot++;
	}

	int closing = fclose(fp);


	qsort(recordArray, indexSpot, sizeof(records*), compare);
	recordArray[0]->fileSize = indexSpot;


	pthread_exit(recordArray);



}
//****************************************
//***This takes a line and splits it into a struct
//************************************************
records* splitALine(char* line) {
	char* token;
	char* ptToken;

	int tokenNum = 1;
	records* returnPointer = malloc(sizeof(records));
	token = strtok_r(line, ",", &ptToken);
	char* temp;

	while (token != NULL && tokenNum < 6) {

		switch (tokenNum) {
		case 1: //name
			returnPointer->name = malloc(strlen(token) + 1);
			strncpy(returnPointer->name, token, strlen(token) + 1);
			break;

		case 2: //pass
			returnPointer->pass = malloc(strlen(token) + 1);
			strncpy(returnPointer->pass, token, strlen(token) + 1);
			break;
		case 3:  //blood
			returnPointer->blood = malloc(strlen(token) + 1);
			strncpy(returnPointer->blood, token, strlen(token) + 1);
			break;
		case 4:  //domain
			returnPointer->domain = malloc(strlen(token) + 1);
			strncpy(returnPointer->domain, token, strlen(token) + 1);
			break;
		case 5: //int index

			returnPointer->index = atoi(token);
			break;
		default:
			break;

		}

		tokenNum++;
		token = strtok_r(NULL, ",", &ptToken);
	}
	return returnPointer;
}
//******************************************************
//****This will print the entire struct*****************
//******************************************************
void printStruct(records** recordStruct, int numRecords)
{
	int i;
	for (i = 0; i < numRecords; i++)
	{
		printf("%s, %s, %s, %s, %d \n", recordStruct[i]->name, recordStruct[i]->pass,
		       recordStruct[i]->blood, recordStruct[i]->domain, recordStruct[i]->index);
	}
}
//*************************************************************
//**********compare function to be used with qsort, sphinx++***
//*************************************************************
int compare(const void *ind1, const void *ind2) {
	records *elem1 = *(records **) ind1;
	records *elem2 = *(records **) ind2;

	if (elem1->index < elem2->index) {
		return -1;
	}
	else if (elem1->index > elem2->index ) {
		return 1;
	}
	else return 0;
}
//*****************************************************************
//*****gets two arrays from 2d array and sends then to merge sort**
//******************************************************************
void mergeArrays(records ***record, int count) {
	int i;
	//there was probably a faster way of doing this using a K-way merge sort
	//...but i didnt want to implement that
	records **merged = malloc(sizeof(record));
	//do the base case
	merged = mergeTwo(record[0], record[1]);
	int mergeSize = 0;


	for (i = 2; i < count; i++) {
		//merge final array with rest of the smaller arrays
		merged = mergeTwo(merged, record[i]);

	}
	int structSize = merged[0]->fileSize;
	writeStruct(merged, merged[0]->fileSize);
	for(i=0; i <structSize; i++){
		free(merged[i]->name);
		free(merged[i]->pass);
		free(merged[i]->blood);
		free(merged[i]->domain);
		free(merged[i]);
	}
	free(merged);
}

//**************************************************
//*****Merge sort two arrays of my struct************
//*****************************************************
records** mergeTwo(records **array1, records** array2) {
	int i, j, k;
	j = 0;
	k = 0;
	int arraySize1 = array1[0]->fileSize;
	int arraySize2 = array2[0]->fileSize;
	records** combinedArray = malloc(sizeof(records) * (arraySize1 + arraySize2));

	//printf("\narray size one is: %d\n", arraySize1);
	//printf("array size two is: %d\n", arraySize2);

	//found a sorting algoritm for this here:
	// http://www.programmingsimplified.com/c/source-code/c-program-merge-two-arrays

	for (i = 0; i < arraySize1 + arraySize2;) {
		if (j < arraySize1 && k < arraySize2) {
			if (array1[j]->index < array2[k]->index) {
				combinedArray[i] = array1[j];
				j++;
			}
			else {
				combinedArray[i] = array2[k];
				k++;
			}
			i++;
		}
		else if (j == arraySize1) {
			for (; i < arraySize1 + arraySize2;) {
				combinedArray[i] = array2[k];
				k++;
				i++;
			}
		}
		else {
			for ( ; i < arraySize1 + arraySize2;) {
				combinedArray[i] = array1[j];
				j++;
				i++;
			}
		}
	}
	combinedArray[0]->fileSize = arraySize1 + arraySize2;
	return combinedArray;

}
//******************************************************
//*******Just writes the sorted array to the file*******
//******************************************************
void writeStruct(records** recordStruct, int numRecords)
{
	FILE *f = fopen("sorted.yay", "w");
	if (f == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}
	int i;
	for (i = 0; i < numRecords; i++)
	{
		fprintf(f, "%s, %s, %s, %s, %d \n", recordStruct[i]->name, recordStruct[i]->pass,
		        recordStruct[i]->blood, recordStruct[i]->domain, recordStruct[i]->index);
	}

	fclose(f);
}