/**
 * 	C program that implements a shell called utdash.
 *	Programs will be executed using fork/exec. 
 *	It allows command line which contain whitespace delimited arguments. These arguments will *  be passed to the command when it is executed. 
 *	It accepts redirection commands in one of three formats: 
 *	command arguments > filename 
 *	command arguments >> filename 
 *	command arguments < filename 
 *
 * @author Jagadish Kamisetti
 */

//Header files 
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

//This function splits the command and its arguments and initializes g_argv
void prepareArguments(char *);
//This Function to append full path to command
int AppendPathToCommand(char *);
//This function execute the command using execve syscall
void executeWithExecve(char *,int ,int ,char *);
//This function search the PATH envrionment to initialize searchPath array
void searchAndFillPath(char *);
//This function freeup the g_argv array
void freeupg_argv(void);

static char *g_argv[100]; //Array to hold command arguments
static char *g_envp[100]; //Array to hold envp settings for execve
static char *searchPath[20]; //Array to hold PATH variable entries

int main(int argc, char *argv[], char *envp[])
{
	char charcterRead;
	int redirection=0;//variable to count occurrence of '>' operator
	int fileinput=0;//variable to count occurrence of '<' operator
	int i, fd;
	//input or output filename
	char *filename = (char *)malloc(sizeof(char) * 100); 
	//temporary array to hold cmd+arguments
	char *tempCmdArgument = (char *)malloc(sizeof(char) * 100); 
	//Array to hold PATH varibale value
	char *pathString = (char *)malloc(sizeof(char) * 256);
	//variable to hold command
	char *cmd = (char *)malloc(sizeof(char) * 100); 
	
	FILE *input; //file descriptor for /dev/tty
    FILE *output;//file descriptor for /dev/tty
	
	//open the terminal keyboard to take input from /dev/tty
    input = fopen("/dev/tty", "r"); 
	//open the terminal keyboard to write output to /dev/tty
    output = fopen("/dev/tty", "w");

	if (!input || !output)
   {
      fprintf(stderr, "Unable to open /dev/tty\n");
      exit(1);
   }
   
	int indx = 0;
	//Initializing g_envp array
	for(;envp[indx] != NULL; indx++) {
		g_envp[indx] = (char *)malloc(sizeof(char) * (strlen(envp[indx]) + 1));
		memcpy(g_envp[indx], envp[indx], strlen(envp[indx]));
	}

	int count = 0;
	char *substring;
	//Initializing pathString with PATH environment variable value
	while(1) {
		substring = strstr(g_envp[count], "PATH");
		if(substring == NULL) {
			count++;
		} else {
			break;
		}
	}
     strncpy(pathString, substring, strlen(substring));
	 
	//Populating pathString value to searchPath array
	searchAndFillPath(pathString);
	
	
	if(fork() != 0) {
		wait(NULL);
	} else {	
		execve("/usr/bin/clear", argv, g_envp);
		exit(1);
	}
	fprintf(output,"%s","Utdash$");//Writing the output to /dev/tty
	printf("Utdash$");
	fflush(input);
	while(charcterRead != EOF) {
		//Reading the Input from /dev/tty
		charcterRead=fgetc(input); 
		if(charcterRead == '>') //Check for output redirection operator
		{
			redirection++;
		}else if(charcterRead == '<') //Check for input redirection operator
		{
			fileinput++;
		}else if(charcterRead == '\n')//Check for end of command
		{
			if(tempCmdArgument[0] == '\0') { //check for null command
				fprintf(output,"%s","Utdash$");//Writing the output to /dev/tty
				printf("Utdash$");
			}else{
				prepareArguments(tempCmdArgument);
				strncpy(cmd, g_argv[0], strlen(g_argv[0]));
				strncat(cmd, "\0", 1);
					   if(index(cmd, '/') == NULL) {
						   if(AppendPathToCommand(cmd) == 0) {
							   executeWithExecve(cmd,redirection,fileinput,filename);
						   } else {
							   printf("%s: command not found\n", cmd);
						   }
					   } else {
						   if((fd = open(cmd, O_RDONLY)) > 0) {
							   close(fd);
							   executeWithExecve(cmd,redirection,fileinput,filename);
						   } else {
							   printf("%s: command not found\n", cmd);
						   }
					   }
					   freeupg_argv();
					   fprintf(output,"%s","Utdash$");//Writing the output to /dev/tty
					   printf("Utdash$");
					   bzero(cmd, 100);
				   }
				   redirection=0;
				   fileinput = 0;
				   bzero(tempCmdArgument, 100);
				   bzero(filename,100);
		}else{
			if(redirection == 0 && fileinput == 0)
			{
				strncat(tempCmdArgument, &charcterRead, 1);
			}else if(charcterRead !=' ')
			{
				strncat(filename, &charcterRead, 1);
			}
		}
	}
	free(tempCmdArgument);
	free(pathString);
	
	for(i=0;g_envp[i]!=NULL;i++)
	{
		free(g_envp[i]);
	}
	for(i=0;i<20;i++)
	{
		free(searchPath[i]);
	}
	printf("\n");
	return 0;
}
/**
 * 	This function execute the command using execve syscall
 *	It will check input and output redirections to files and if any then It open the file
 *  for appropriate action(read/write). 
 */
void executeWithExecve(char *cmd,int redirection,int fileinput,char *filename)
{
	int status;
	if(fork() != 0) {
		wait(NULL);
	}else{
		if(redirection == 1 ) //check for single '>' symbol
		{
			//opening the file in Write Only mode
			int out = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			//replacing stdout with file descriptor 
			dup2(out, 1);
			close(out);

		}else if(redirection == 2) //check for single '>>' symbol
		{
			//opening the file in Append mode
			int out = open(filename, O_APPEND | O_WRONLY | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			//replacing stdout with file descriptor 
			dup2(out, 1);
			close(out);
		}else if (fileinput != 0) ////check for single '<' symbol
		{
			//opening the file in read Only mode
			int inp = open(filename, O_RDONLY);
			//replacing stdin with file descriptor 
			dup2(inp, 0);
			close(inp);
		}
		status = execve(cmd, g_argv, g_envp);
		if(status < 0)
		{
			printf("%s: %s\n", cmd, "command not found");
		}
		exit(1);		
	}
}

/**
 * 	This function splits the command and its arguments and initializes g_argv 
 */
void prepareArguments(char *commandAndArguments)
{
	char *foo = commandAndArguments;
	int index = 0;
	char strings[100];
	bzero(strings, 100);
	
	while(*foo != '\0') {
		if(*foo == ' ' || *foo == '\t') { //checking for white spaced delimiter
			foo++;
			if(*foo != '\0')
			{
				if(g_argv[index] == NULL)
					g_argv[index] = (char *)malloc(sizeof(char) * strlen(strings) + 1);
				else {
					bzero(g_argv[index], strlen(g_argv[index]));
				}
				strncpy(g_argv[index], strings, strlen(strings));
				strncat(g_argv[index], "\0", 1);
				bzero(strings, 100);
				index++;
				foo--;
			}
			
			if(index == 11)
			 break;
		} else {
			strncat(strings, foo, 1);
		}
		foo++;

	}
	
	 g_argv[index] = (char *)malloc(sizeof(char) * strlen(strings) + 1);
	 strncpy(g_argv[index], strings, strlen(strings));
	 strncat(g_argv[index], "\0", 1);
}
/**
 * 	This Function to append full path to command 
 */
int AppendPathToCommand(char *cmd)
{
	int i;
	char substring[100];
	int fd;
	bzero(substring, 100);
	for(i=0;searchPath[i]!=NULL;i++) {
		strcpy(substring, searchPath[i]);
		strncat(substring, cmd, strlen(cmd));
		if((fd = open(substring, O_RDONLY)) > 0) {
			strncpy(cmd, substring, strlen(substring));
			close(fd);
			return 0;
		}
	}
	return 0;
}

/**
 * 	This function search the PATH envrionment to initialize searchPath array 
 */
void searchAndFillPath(char *pathString) 
{
	int i=0;
	char *tempString = pathString;
	char substring[100];

	while(*tempString != '=')
	{
		tempString++;
	}
	tempString++;

	while(*tempString != '\0') {
		if(*tempString == ':') { //checking delimeter ":"
			strncat(substring, "/", 1); 
			searchPath[i] = (char *) malloc(sizeof(char) * (strlen(substring) + 1));
			strncat(searchPath[i], substring, strlen(substring));
			strncat(searchPath[i], "\0", 1);
			i++;
			bzero(substring, 100);
		} else {
			strncat(substring, tempString, 1);
		}
		tempString++;
	}
}

/**
 * 	This function freeup the g_argv array
 */
void freeupg_argv()
{
	int i=0;
	while(g_argv[i]!=NULL) {
		bzero(g_argv[i], strlen(g_argv[i])+1);
		g_argv[i] = NULL;
		free(g_argv[i]);
		i++;
	}
}

