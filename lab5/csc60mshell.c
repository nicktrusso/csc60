/* csc60mshell.c
 * programmed by:
 * 	Nick Trusso
 * 	Ryan Robertson
 * 10/25/2015
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
//#include "tlpi_hdr.h"

#define MAXLINE 80
#define MAXARGS 20

struct job_array
{
	int process_id; // process id
	char command[80]; // previous command with & removed.
	int job_number; // job number
};
	
void process_input(int argc, char **argv) {
  if(argc == -1)
  {
	perror("Shell Program");
	_exit(-1);
  }
  
  char *cmd[] = {argv[0],0};
  int i;
  int outPos = 0; //Position of the output operator
  int inPos = 0;  //Position of the input operator
  int outFileId,inFileId;
  int outOperators = 0;
  int inOperators = 0;
  //check for ">" operator
  for(i=0;i<argc;i++){
	if(strcmp(argv[i],">") == 0){
		outPos = i;
		outOperators++;
	}
  }
  if(outOperators > 1){
	printf("***ERROR more than one operator\n");
	exit(-1);
  }
	
  //check for "<" operator
  for(i=0;i<argc;i++){
	if(strcmp(argv[i],"<") == 0){
		inPos = i;
		inOperators++;
	}
  }
  if(inOperators > 1){
	printf("***ERROR more than one operator\n");
	exit(-1);
  }
  //open/create file for writing
  //first check for redirection file
  if(outPos != 0){
      if(argv[outPos+1] == '\0'){
          printf("***ERROR no redirection file\n");
          exit(-1);
      }
      
	outFileId = open(argv[outPos+1],O_RDWR | O_CREAT | O_TRUNC,S_IRUSR | S_IWUSR);
        if(outFileId < 0){
		printf("***ERROR creating file\n");
		exit(-1);
	}	
  	dup2(outFileId,1); //copy outFileId to stdout
  	close(outFileId); //close it now that is has been copied
  	argv[outPos] = NULL;	
  }
  
  //open file for input
  //first check for redirection file
  if(inPos != 0){
	if(argv[inPos+1] == '\0'){
          printf("***ERROR no redirection file\n");
          exit(-1);
        }
	inFileId = open(argv[inPos+1],O_RDONLY);
	if(inFileId < 0){
		printf("***ERROR opening file\n");
		exit(-1);
	}
	dup2(inFileId,0); //copy inFileId to stdout
	close(inFileId);  //no longer needed
	argv[inPos] = NULL;  	
  }
  printf("running: %s\n",argv[0]);
  setpgid(0,0);
  if(execvp(*argv, argv) < 0)
	printf("***ERROR: exec failed\n");
  exit(EXIT_SUCCESS);
}

/* ----------------------------------------------------------------- */
/*                  parse input line into argc/argv format           */
/* ----------------------------------------------------------------- */
int parseline(char *cmdline, char **argv)
{
  int count = 0;
  char *separator = " \n\t";
  argv[count] = strtok(cmdline, separator);
  while ((argv[count] != NULL) && (count+1 < MAXARGS)) {
   argv[++count] = strtok((char *) 0, separator);
  }
  return count;
}

/* ----------------------------------------------------------------- */
/*	Check if first argument is a special character					 */
/*	returns 0 if no special characters								 */
/*	returns -1 if first argument is a special char					 */ 
/* ----------------------------------------------------------------- */
int checkSpecial(char **argv){
	char *special[] = {"!","@","#","%","$","^","&","*","(",")"
			   ,"-","+","=","<",">",",",".","?",":",";"
			   ,"'","{","}","/","\\","[","]","\0"};
	int hasSpecial = 0;
	int count = 0;
	while((special[count] != NULL) && (hasSpecial == 0)){
		if((strcmp(special[count],argv[0])) == 0){
			hasSpecial = 1;
			continue;
		}
		count++;
	}
        if(hasSpecial != 0)
		return -1;
	else
		return 0;
}		

/* ----------------------------------------------------------------- */
/*     returns 1 if "&" is the last command, else returns 0          */
/* ----------------------------------------------------------------- */
int isBackgroundJob(int argc,char **cmd){
	if(strcmp(cmd[argc-1],"&")==0)
		return 1;
	else
		return 0;
}

/* ----------------------------------------------------------------- */
/*     removes last string in argv                                   */
/* ----------------------------------------------------------------- */
void removeTail(int argc,char **argv){
	argv[--argc] = NULL;
}

/* ----------------------------------------------------------------- */
/*     SIGNAL handler - SIGINT                                       */
/* ----------------------------------------------------------------- */
static void signalIntHandler(int sig){
	printf("signal interrupt\n");
	return;
}

/* ----------------------------------------------------------------- */
/*     SIGNAL handler - SIGCHLD                                       */
/* ----------------------------------------------------------------- */
static void signalChldHandler(int sig){
	printf("child is term/stopped\n");
	//waitpid(pid,status,options);
	return;
}

/* ----------------------------------------------------------------- */
/*                  The main program starts here                     */
/* ----------------------------------------------------------------- */
int main(void)
{
	
 char cmdline[MAXLINE];
 char *argv[MAXARGS];
 int argc;
 int status;
 int size;
 char *builtIn[] = {"exit","pwd","cd","clear","jobs"};
 char *cwd;
 char buff[PATH_MAX + 1];
 char *path;
 pid_t pid;
 struct job_array jobs[20];
 int bckgrdProcesses = 0;
 int isBckgProc = 0; 
 
 //setup sigaction handler for interrupt
 struct sigaction handler;
 handler.sa_handler = signalIntHandler;
 sigemptyset(&handler.sa_mask);
 handler.sa_flags = 0;
 sigaction(SIGINT, &handler, NULL);
 
 //setup sigaction handler for SIGCHLD
 struct sigaction handler2;
 handler2.sa_handler = signalChldHandler;
 sigemptyset(&handler2.sa_mask);
 handler2.sa_flags = 0;
 sigaction(SIGCHLD, &handler2, NULL);
 

 /* Loop forever to wait and process commands */
 int i;
 for(i=0;i<10;++i){
  printf("csc60mshell> ");
  fgets(cmdline, MAXLINE, stdin);
  //if user presses enter with no arguments, continue
  if(cmdline[0] == '\n')
	continue;
  
  //process number of arguments 
  argc = parseline(cmdline, argv);
  
  isBckgProc = 0;
  //check if background process
  if(isBackgroundJob(argc,argv) == 1){
	  isBckgProc = 1;
	  removeTail(argc,argv);
	  --argc;
  }
  
  //see if the first argument is a built in command
  size = sizeof(builtIn)/sizeof(builtIn[0]);
  int i;
  int index = -1;
  for(i=0;i<size;++i){
	  if(strcmp(builtIn[i],argv[0]) == 0){
		  index = i;
	  }
  }
  //built in command found 
  if(index != -1){
	  switch(index){
		case 0:
			exit(EXIT_SUCCESS);
			break;
		case 1:
			cwd = getcwd(buff, PATH_MAX + 1);
			if(cwd != NULL)
     			printf("%s\n",cwd);
			break;
		case 2:
			//cd with no argument -> cd $HOME
			if(argc ==  1){
				char *home;
				home = getenv("HOME");
				if(chdir(home) != 0)
					perror("Error ");
				break;
			}
			path = argv[1];
			if(chdir(path) != 0)
				perror("Invalid");
			break;
		case 3:
			system("clear");
			break;
		case 4:
			for(i=0;i<bckgrdProcesses;++i)
				printf("[%i] %i\n",(i + 1),jobs[i].process_id);
			break;
	  }
	  continue;
  }
  
  
  int hasSpecial;
  hasSpecial = checkSpecial(argv);
  if(hasSpecial != 0){
	printf("***ERROR invalid input\n");
	continue;
  }
		  
  pid = fork();
  if (pid == -1) 
    perror("Shell Program fork error");
  else if (pid == 0){
    //child process
	process_input(argc, argv);
  }
	else if (wait(&status) == -1)
      perror("Shell Program error");
	  else if(isBckgProc == 1){
		  //record in list of background jobs
		  jobs[bckgrdProcesses++].process_id = pid;
	  }
	  else{
		  //parent process
		  wait(&status);
		  if(WIFEXITED(status))
			  printf("child exited with %d\n",WEXITSTATUS(status));
		  else if(WIFSIGNALED(status))
			  printf("child exited due to signal %d\n", WTERMSIG(status));
		  else if(WIFSTOPPED(status))
			  printf("child process was stopped by signal %d\n",WIFSTOPPED(status));
		  
	  }
	}
	
}
