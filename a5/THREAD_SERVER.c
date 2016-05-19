/* R. Trenary, 4/22/15
 An example of a primitive server I can stand to show in public
 A socket is set up in the internet domain using TCP/IP 
 The port number is passed as an argument to executable
 Each accept forks a process, which parses the request
 (we are handling only GET requests)and then constructs 
 a so-called response header (which is echoed in this demo).
 Note that this header is built using sprintf and thus formatting
 is available. The header can then be written back across socket
 in single write. 
 Usage: sudo <serverexecutablename> <port #>
 then to test on your own machine type URL in browser 
    localhost:port/Koala.jpg 
 to serve the file to yourself.  
 Here the file size is found, memory for it all is malloc()ed
 and a single write serves it up. I do free() that memory. 

 Alec Carpenter has managed to send audio, text, (video?)
 but the browser and mime type supported can vary  

 Here we handle more than, using system calls socket, bind, listen accept. 
 No 404 messages are returned on failure.
****************************************************************************************************
 so basically just going to use trenarys code, gonna make it thread, serve audio, and make a 404 page

 Did just that, used treanrys code, changed it to thread instead of fork
 added an if sturcture to see if 404, img, or mp3, and that was it

****************************************************************************************************
*/
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>    // the usual suspects 
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>

#define BufferSize 1024 // for parsing request
#define BIG_ENUF 4096 // For request header

void error(char *); // prototype for quick error exit
void * threadMeth(void *);


static char* not_found =
"HTTP/1.0 404 Not Found\r\n"
"Content-type: text/html\r\n\r\n"
"\n"
"<html>\n"
" <body>\n"
"  <h1>404 Not Found</h1>\n"
"  <p>The requested URL %s was not found on this server.</p>\n"
" </body>\n"
"</html>\n";


int main(int argc, char *argv[]) // arv[1] has port #
{
    // We Have Ourselves Some Declarations Old School
 int sockfd, newsockfd, portno, clilen,Connect_Count=0;
 char buffer[BufferSize]; // for communicating with client
  char * BigBuffer; // for serving file to client
  int BufferNdx,n, err ;// workaday subscripts 
    char * TmpBuffer, *SavePtr, *FileName, *GetToken;
    // for processing tokens from client request
 pid_t pid; // for forks;
    FILE * F;  // for streaming file when GET served
    struct stat S;// to find file length 
  struct sockaddr_in serv_addr, cli_addr; 
// structs for client and server

// =======================================//
//   And Now To Work 
// =======================================//

     if (argc < 2) {  // looking for port # 
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     // specifies TCP IP flow 
     if (sockfd < 0) 
        error("ERROR opening socket");
     memset( (char *) &serv_addr, 0, sizeof(serv_addr));
     // Now fill the zeroed server structure // 
     // consistent with socket setup 
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;  
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno); // proper byte order
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
                    error("ERROR on binding");
      GetToken = strtok_r(TmpBuffer," ",&SavePtr); 
 // And Now port is bound to socket for TCP / IP

      //***Every thing above this is pretty much trenarys*****////
      //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//


      pthread_t tid;
    while (1) // Limit on Number of Connections
    {
        
        listen(sockfd,5);
      clilen = sizeof(cli_addr);
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    // listen blocks until someone knocks and when we accept 
  
  // the client structure is filled by accept 
     if (newsockfd < 0) // exit server if bad accept 
          error( "ERROR on accept");
     // otherwise let's fork a process to do that work 
      int *tempSock;
      *tempSock = newsockfd;
    err = pthread_create(&tid, NULL, threadMeth, tempSock);
 
     // to handle talking with client below
//if parent, loop again to listen if more connections ? 
    }
   
 // ===================================================  
// THIS IS THE FORKED CHILD WHO WILL HANDLE THE REQUEST
// First prove we can get a request parsed 

}
// Poor man's error routine 
void error(char *msg)
{
    perror(msg);
    exit(1);
}


void * threadMeth(void *x){

    //***This whole thing is also pretty much all trenrys, i just duplicated some stuff,
    // added a 404 page and changed the content type to audio/mpeg for for the mp3
    // got the 404 stuff (including the char* from: http://www.advancedlinuxprogramming.com/listings/chapter-11/server.c)
    
    char buffer[BufferSize];
    int newsockfd = *(int*)x;
     char * BigBuffer; // for serving file to client
  int BufferNdx,n, err ;// workaday subscripts 
    char * TmpBuffer, *SavePtr, *FileName, *GetToken;
    struct stat S;
    FILE * F;
    memset(buffer, 0,BufferSize);
     n = read(newsockfd,buffer,BufferSize-1); // This leaves null at end
    if (n < 0) error("ERROR reading from socket");
// Much of below debug is commented out .. use for science
// So we got something from socket -- let's echo it
// if debugging
//     printf("Here is the message: %s\n",buffer);
// Can we parse the request ? 
     printf("%s\n",(TmpBuffer=strtok_r(buffer,"\n",&SavePtr)));
     GetToken = strtok_r(TmpBuffer," ",&SavePtr); 
     printf("%s\n",GetToken); 
     GetToken = strtok_r(NULL," ",&SavePtr); 
     printf("%s After Get\n",GetToken); // file name token begins '/'
     GetToken++; // Point to first character of actual file name 
    // now open the file and send it to client ? 
     if ((F =  fopen(GetToken,"r")) == NULL){
        char Response[BIG_ENUF];
        snprintf(Response, sizeof(Response), not_found, GetToken);
        write(STDERR_FILENO, Response, strlen(Response));
         write(newsockfd,Response,strlen(Response));
         close(newsockfd);
         pthread_exit(NULL);
     }
    else printf("Good Dog\n"); 
        int FileSize;
    if(strstr(GetToken, ".jpg") != NULL){
    if ((fstat(fileno(F),&S)==-1)) error("failed fstat\n"); // Need file size 
    FileSize = S.st_size;
// Looks ok -- now let's write the request header
 // Let's just fill a buffer with header info using sprintf()           
    char Response[BIG_ENUF];
    int HeaderCount=0;
    HeaderCount=0;//Use to know where to fill buffer with sprintf 
        HeaderCount+=sprintf( Response+HeaderCount,"HTTP/1.0 200 OK\r\n");
        HeaderCount+=sprintf( Response+HeaderCount,"Server: Flaky Server/1.0.0\r\n");
        HeaderCount+=sprintf( Response+HeaderCount,"Content-Type: image/jpeg\r\n");
        HeaderCount+=sprintf( Response+HeaderCount,"Content-Length:%d\r\n",FileSize);
 // And finally to delimit header
        HeaderCount+=sprintf( Response+HeaderCount,"\r\n"); 
        // Let's echo that to stderr to be sure ! 
        fprintf(stderr,"HeaderCount %d and Header\n",HeaderCount);
        write(STDERR_FILENO, Response, HeaderCount);
        write(newsockfd,Response,HeaderCount); // and send to client
// Now Serve That File in one write to socket
        BigBuffer = malloc(FileSize+2);// Just being OCD -- Slack is 2
        fread(BigBuffer,1,FileSize,F);
        write(newsockfd,BigBuffer,FileSize);
        free(BigBuffer); 
// Now close up this client's shop 
     close(newsockfd);
     pthread_exit(NULL);
 }
 
 else if(strstr(GetToken, ".mp3") != NULL){
    if ((fstat(fileno(F),&S)==-1)) error("failed fstat\n"); // Need file size 
    FileSize = S.st_size;
// Looks ok -- now let's write the request header
 // Let's just fill a buffer with header info using sprintf()           
    char Response[BIG_ENUF];
    int HeaderCount=0;
    HeaderCount=0;//Use to know where to fill buffer with sprintf 
        HeaderCount+=sprintf( Response+HeaderCount,"HTTP/1.0 200 OK\r\n");
        HeaderCount+=sprintf( Response+HeaderCount,"Server: Flaky Server/1.0.0\r\n");
        HeaderCount+=sprintf( Response+HeaderCount,"Content-Type: audio/mpeg\r\n");
        HeaderCount+=sprintf( Response+HeaderCount,"Content-Length:%d\r\n",FileSize);
 // And finally to delimit header
        HeaderCount+=sprintf( Response+HeaderCount,"\r\n"); 
        // Let's echo that to stderr to be sure ! 
        fprintf(stderr,"HeaderCount %d and Header\n",HeaderCount);
        write(STDERR_FILENO, Response, HeaderCount);
        write(newsockfd,Response,HeaderCount); // and send to client
// Now Serve That File in one write to socket
        BigBuffer = malloc(FileSize+2);// Just being OCD -- Slack is 2
        fread(BigBuffer,1,FileSize,F);
        write(newsockfd,BigBuffer,FileSize);
        free(BigBuffer); 
// Now close up this client's shop 
     close(newsockfd);
     pthread_exit(NULL);

 }
 

    pthread_exit(NULL);
}

