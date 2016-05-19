//Aron Lawrence
// Shell Assingment
//


// This code is intended to demo small pieces of code relevant to a shell
// dup2, execv()  in particular
// In addition, the most useful makeargv() method will be used
//
//NOT FULLY FUNCTIONAL
// pipes work
//built ins work
// backgrounding works
//redirecting...well thats a different story
// i can redirect out if ther are no pipes i.e ls > temp.txt will work
// everything else not so much...i feel like i am close...but just misisng something with file descriptors


//just using system calls from DupExecDemo.c for now
#include <stdio.h>
#include "apue.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>   // OverKill 
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h> //added this for the kill method
#include <string.h>
#include <unistd.h>
//
int makeargv(char *, char * , char ***);
#define INBUFFER 1024

const int PIPE_READ = 0;
const int PIPE_WRITE = 0;
void plush(void);
char *plushReadLine(void);
int plush_launch(char **args);
int plush_cd(char **);
int plush_kill(char **);
int plush_exit(char **);
int plush_num_builtins();
int plush_execute(char **);
int fork_pipes(int, char**);
int spawn_proc(int, int, char**);
//int directIn(char *);

//list of builtin strings
//and corrisponding functions
//may be a bit over kill for this assingmnet but will make it easier to add
//builtins if desired

char *builtin_str[] = {
	"cd",
	"kill",
	"exit"
};

//corresponding commands
int (*builtin_func[]) (char **) = {
	&plush_cd,
	&plush_kill,
	&plush_exit
};

int numtokens; //lets make this a global var  for now
//int fdl[2]; //these too
//int fdr[2];
// int fork_pipes(int n, char ***pipe_splits);
//first following a tutorial
// http://stephen-brennan.com/2015/01/16/write-a-shell-in-c/
//to get a basic shell working, does not implement pipes or redirects
//need to edit it to work with makeargv

int main(int argc, char **argv) {

	plush();

	return 0;
}
//steps:
// 0: come up with a cool prompt ...for now ">  "
// 1: read in a line to pass to makeargv
// 2: now to figure out how mageargv works
// 2.1 splitting on pipes work, on to starting a process...
// 3:  pid and fork time i suppose ...probs have a bit more to do with this
// 3.2 doing builtins
// 4. lets pipe
// 5. and 5 days later i can pipe
// 6. time to do redirecting...
// 7. so i skippedrediercting, moved on to backgrounding
// 8. and back to riderecting
// 8.1 (5 am tuesday the 23rd) ">" works with no pipes, thats something right



//*****************************
//*****main loop of shell******
void plush(void) {
	printf("\n\n\n\nWelcome to PLUSH: Pluggles' Lame  Ugly SHell\n");
	printf("you were not planning on doing any of that new age redirection in right?\n\n\n\n\n");

	char *line;
	int execStatus = 1;

	int i;

	//these may be needed (referenceing PipeScience.c)

	pid_t pid;
	char **pipe_splits;
	//char cwd[1024];
	while (execStatus) {

		printf(">>> ");
		line = plushReadLine();

		//printf("%s\n",line);
		//	printf("here\n");
		//split on pipes first

		numtokens = makeargv(line, "|", &pipe_splits);
		//test to make sure splitting on pipes...is splittin on pipes
		/*
		for(i=0; i < numtokens; i++){
			printf("%s\n", pipe_splits[i]);
		}
		*/
		char **commands;
		//	fork_pipes(numtokens, pipe_splits);
		//	if (numtokens != 1 ) {
		execStatus = fork_pipes(numtokens, pipe_splits);
		//	}

		//	else {
//
//			makeargv(pipe_splits[i], "\t \n&", &commands);
//				execStatus = plush_execute(commands);
//		}

		/*	for (i = 0; i < numtokens; i++) {
				//if (numtokens = 1) {  //no need to pipe stuff here...i hope
				char **commands;
				makeargv(pipe_splits[i], "\t \n&", &commands);
				execStatus = plush_execute(commands);
				//}

				//i guess we should pipe stuff

			}
			*/

		free(commands);
		free(line);
		free(pipe_splits);
	}

}

//****************************************************
//*********reads a line from stream******************
//         dynamically allocates size
// ...probably should have just used getline() -_-
//also from that tutorial listed, tried to use error.c erro calls
char *plushReadLine(void) {
	int bufsize = INBUFFER;
	int posistion = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c;

	//make sure malloc worked
	if (!buffer) {
		//look at me using error calls...probably incorectly
		//is there a way to check if error checking is erroring?
		err_sys("malloc failed\n");
	}

	while (1) {

		c = getchar();

		if (c == EOF || c == '\n') {
			buffer[posistion] = '\0';
			return buffer;
		}
		else {
			buffer[posistion] = c;
		}
		posistion++;


		//reallocate if needed
		if (posistion > - bufsize) {
			bufsize += INBUFFER;
			buffer = realloc(buffer, bufsize);
			if (!buffer) {
				err_sys("malloc failed\n");
			}
		}
	}
}
// l0oking a a stackoverflow question: this is about pipe_forks and spawn_procs at the bottom
// http://stackoverflow.com/questions/8082932/connecting-n-commands-with-pipes-in-a-shell


//ended up not using this exact function, but i did use a lot of the logic

//gonna gomment it out
/*
int plush_launch(char **args) {
	pid_t pid, wpid;
	int status;

	pid = fork();
	//child process
	if (pid == 0) {


		//execvp takes name then vector of arguments, shouldnt return any value
		if (execvp(args[0], args) == -1) {
			err_msg("plush exec failed");

			exit(EXIT_FAILURE);
		}
	}


	else if (pid < 0) {
		//error while forking
		err_sys("fork error\n");
		//continue anyways, but let user know an error happened
	}

//parent process
	else {
		do {
			//know the child has to execute first so we wait for it
			wpid = waitpid(pid, &status, WUNTRACED);
			//process can change state w/out process actually end
			//so we  are checking to see if it actually ended
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1; //so we know we can continue
}

*/
//get number of builtins
int plush_num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}


//this works, it may be nice to edit my prompt to show cwd
int plush_cd(char** args) {
	//home dir isnt implemented so make sure args1 != null
	if (args[1] == NULL) {
		fprintf(stderr, "plush: expected argument to \"cd\"\n");
	}
	else {
		if (chdir(args[1]) != 0) {
			err_msg("plush: not a directory");
		}
	}
	return 1;
}


// defualt works if you just send pid
//you can also do kill -9 pid
// or kiill SIGKILL pid ...which is the same of -9
//if i read my man pages correctly...
int plush_kill(char** args) {
	int returnVal = -1;
	int count = 0;
	while (args[count]) count++;
	pid_t pid;
	int sig;
	if (count < 2 && count ) {
		fprintf(stderr, "plush: expected an argument kill\n");
		return 1;
	}
	else {

		// sig = args[2];
		if (args[2] == NULL) {
			pid = atoi(args[1]);
			if (kill(pid, SIGTERM) != 0) {
				err_ret("plush: error");
			}
		}
		else if (strcmp(args[1], "SIGKILL") == 0 || strcmp(args[1], "-9") == 0 ) {
			pid = atoi(args[2]);
			if (kill(pid, SIGKILL) != 0) {
				err_ret("plush: error");
			}
		}
	}
	return 1;
}

int plush_exit(char **args) {
	return 0;
}

//so i changed some stuff and dont call this method
//but imma keep it because it was part of that main tutorial i followed and i used its logic else where
/*
int plush_execute(char **args) {
	int i;
	if (args[0] == NULL) {
		return 1;
	}
	for (i = 0; i < plush_num_builtins(); i++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
		}
	}


	return plush_launch(args);
}
*/
int fork_pipes(int n, char** tokens) {

	//6 am i am having an issue where i can redierct out if there are no pipes, but
	//as soon as i pipe and try to redierct out i get an error (standard input): Bad file descriptor
	// so what i think is happening is i am not correctly passing my last file descripter f[1], or not
	// dupping it right, which is wierd becuase the right file descripter gets passed fine when just piping
	// idk, but my code has turned into a bunch of spaghetti, so imma try and clear some stuff up, or explain my
	// resoning for what is quite cleary some shitty code
	int i, j;
	pid_t pid;
	int in;
	int fd[2];
	int status;
	int wpid;
	int backgrounding = 0;
	int outIndex;
	in = 0;
	//so i have to search the last string for a >
	//and first string for a <
	//unless if there is only one argument ( 0 pipes) then arg one can have both
	// make a flag or two perhaps?
	int directOut = 0;
	int directInFlag = 0;

	if (n == 0) {  //so if there are no commands, just go back to main loop
		return 1;
	}
	//char **directIn;
	//saving stdin or else i had a weird infinite loop, restore at end of method
	int saved_stdin = dup(0);
	int saved_stdout = dup(1);
	int tempfd1;

	//start checking for redierct in
/*	if (strstr(tokens[0], "<") != NULL) // checkin if containts <
	{
		//	printf("look mah i can string look up\n");
		directInFlag = 1;  //if the > was found now we know
		in = directIn(tokens[0]);


	}
*/

	//

	for (i = 0; i < n - 1; ++i) {  //main loop, will loop through all but last argument

		pipe(fd);  //gotta pipe

		char **commands;

		makeargv(tokens[i], "\t \n&", &commands);   //split each token on white space
		if (i == 0) {
			for (j = 0; j < plush_num_builtins(); j++) {   //here i am checking for builtins
				if (strcmp(commands[0], builtin_str[i]) == 0) { //i end up doing this later on the last argument too

					return (*builtin_func[j])(commands); //there was probably a better way of checking this
				}
			}
		}

		spawn_proc(in, fd[1], commands);  //if not builtins lets fork, and dup, and hopefully exec

		//tempfd1 = dup(fd[1]);
		//in = dup(fd[0]);
		close(fd[1]);
		//close the write file descriptor
		in = fd[0];
		//set the in value to the in file descriptor this is where next child will read from...
		//free(commands);  //free commands (good time to mention that i have so much memory leaking going on in
		// valgrind, it is pretty bad, i would advise staying away from it for now)

	}



	if (in != 0) {
	//	printf("here mate and in is '%d' and n is '%d'\n", in, n); //if in is not the standard read we set stdin to the in file descript for the last exec
		if ((dup2(in, 0)) == -1){
			printf("dup2 had an error\n");
		} //this should be the file descriptor for in on grep if i run $ ls | grep a > temp.txt
		// yet i get a bad descriptor
		//do i close it?

	}

	//so here is where things start to go really really bad

	char **lastcommand; //or possibly last command too

	char **splitOnOut;
	//decided to first check if there is just a redirct out, figured this would be the easiest to implement
	// mistakes were made...
	if (strstr(tokens[i], ">") != NULL) // checkin if containts >
	{
		//	printf("look mah i can string look up\n");
		directOut = 1;  //if the > was found now we know

	}
	int numOfArgs = makeargv(tokens[i], "\t \n", &lastcommand);  //split on white space



	if (directOut == 1) { //if the redierct was found it would help to know where:
		for (j = 0; j < numOfArgs; ++j) {
			if (strcmp(lastcommand[j], ">") == 0) {
				outIndex = j + 1;
				//printf("this should be a carrot '%s'\n", lastcommand[j]);

			}

		}
	//	if(n==1){
		close(fd[1]);
	//}
	}
	//printf(" and this should be outfile '%s'\n", lastcommand[outIndex]);

	int out;
	if (n == 1) {

		for (j = 0; j < plush_num_builtins(); j++) {  //checking for builtins again
			if (strcmp(lastcommand[0], builtin_str[j]) == 0) {
				return (*builtin_func[j])(lastcommand);

			}
		}
		//	printf("what does n equal?\n");
		 //if only the one argueement i close in
		//dup2(fd[1], 1);	//as i type these comments i feel i may wanna go back to this?
		//close(fd[1]); //close in

	}
	else {
	//	printf("fd[1] is '%d'\n", fd[1]);
	//	if (dup2(fd[1], 0) < 0 ) {
	//		printf("dup error\n");
	//	}
		//close(fd[1]);
	}
	
	//get fd for the open file
	//are all these options needed and useful?
	//idk but jglenn from cs.loyola thought they were:
	// http://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html
	if (directOut == 1) {
		if ((out = open(lastcommand[outIndex], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR)) == -1) {
			err_sys("Plush: ");
		}
		else { //if open worked i do stuff

			//printf("about to get crazy and out is '%d'\n", out);
			

			if((dup2(out, 1)) == -1){
				printf("error in dup 2");
			}  //this is the magic that redierct the output to the file
			//close(out);	//once duped i can close...i hope
			makeargv(tokens[i], ">", &splitOnOut);  //so now i split the command on the pipe
			free(lastcommand);  //and free lastcommand...i feel this is needed here
			numOfArgs = makeargv(splitOnOut[0], "\t \n", &lastcommand); //and then just pass everything up to the > to exec
			//printf("lastcommand of 0 id '%s'\n", lastcommand[0]);
		}
	}


	if (strcmp(lastcommand[numOfArgs - 1], "&") == 0) {  //here i check for backgrounding
		//printf("stuff is here man\n");
		backgrounding = 1;  //have a flag so i know if i can not wait later
		lastcommand[numOfArgs - 1] = NULL; //change & to a null so exec works
		
		signal(SIGCHLD, SIG_IGN); //hopefully ignore //it works sphinx++
	}





//if (n == 1) {   //if there was only one arg i have yet to fork
	//fork refers to same file descripters so this isn't the issue right?
// and ...i just broke my piping ...im going to always fork here i guess...
//
pid = fork();
//}
	

	if (pid == 0) {  //pid == 0 is the child so i can exec


		if ( execvp(lastcommand[0], lastcommand) == -1) {
			err_msg("plush exec failed");

			exit(EXIT_FAILURE);
		}
	}
	else if (pid > 0 && backgrounding == 0) {  //if backgrounding dont call wait...but wait
		// ..now i get zombie procs....
		// heres hoping trenary is a fan of the walking dead...
		do {

			//know the child has to execute first so we wait for it
			wpid = waitpid(pid, &status, WUNTRACED);
			//process can change state w/out process actually end
			//so we  are checking to see if it actually ended
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	
	free(lastcommand);

//restoring stdin
	dup2(saved_stdin, 0);
	//close(saved_stdin);
	dup2(saved_stdout, 1);
//	close(saved_stdout);
	return 1;
}


//this is pretty much stright out of that stack over flow response, i added that else
int spawn_proc(int in, int out, char **args) {

	pid_t pid;
	int wpid;
	int status;
	if ((pid = fork()) == 0)  //child
	{
		if ( in != 0) {  //these two ifs handle the passing of the file descripters
			dup2(in, 0);
			close(in);
		}
		if (out != 1) {
			dup2(out, 1);
			close(out);
		}
		return execvp(args[0], args);
	}
	else {
		do {

			//know the child has to execute first so we wait for it
			wpid = waitpid(pid, &status, WUNTRACED);
			//process can change state w/out process actually end
			//so we  are checking to see if it actually ended
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return pid;
}
/*
int directIn(char * firstTok){
printf("firstTok is %s\n", firstTok);

char **fisrcommand

int numOfArgs1 = makeargv(tokens[i], "\t \n", firstcommand);  //split on white space


return 0;
}
*/