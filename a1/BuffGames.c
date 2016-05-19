// This code is intended to demonstrate buffering
// a line oriented file, where line lengths vary
// The intent is to minimize reads using a buffer
// size that is appropriate for system factors such
// as block size etc.
// The module will be invoked as
//    char * GimmeALine(   ) returning a pointer
// to a line from the file being processed or NULL
// if no more data. All reads are done within this
// module. An attempt at encapsulation is made by
// declaring local names as static (and thus not
// 'automatic' and thus persistent over calls to GimmeALine
// A simple main driver is used for testing
// Caveat Emptor: this is a work in progress
// Robert Trenary, 9/21/15
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include "apue.h" // Stevens' error handling
#define BUFFSIZE 512
#define FALSE 0
#define TRUE -42
char *GimmeALine( int FileDes);
// will send current file descriptor

int main(int argc, char *argv[])
{
    int fd ;
    char *LinePtr; // Return from GimmeALine
    int LineCount = 0;

    fd =  open("AllCountries.dat", O_RDONLY);
    if (fd < 0) err_sys("Failed to open Countries File\n");

    while ((LinePtr = GimmeALine(fd)) != NULL)
    {
        fprintf(stderr, "Line %u is \n %s\n", LineCount, LinePtr);
        LineCount++;
    }
}

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
