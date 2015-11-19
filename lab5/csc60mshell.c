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

#define MAXLINE 80
#define MAXARGS 20

void process_input(int argc, char **argv) {
  /* Problem 1: perform system call execvp to execute command     */ 
  /*            No special operator(s) detected                   */
  /* Hint: Please be sure to review execvp.c sample program       */
  /* if (........ == -1) {                                        */  
  /*  perror("Shell Program");                                    */
  /*  _exit(-1);                                                  */
  /* }                                                            */
  /* Problem 2: Handle redirection operators: < , or  >, or both  */
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
  if(execvp(*argv, argv) < 0)
	printf("***ERROR: exec failed\n");
  exit(1);
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

//**********************************************************
//	Check if first argument is a special character
//	returns 0 if no special characters
//	returns -1 if first argument is a special char
//*********************************************************
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
/*                  The main program starts here                     */
/* ----------------------------------------------------------------- */
int main(void)
{
 char cmdline[MAXLINE];
 char *argv[MAXARGS];
 int argc;
 int status;
 char *builtIn[] = {"exit","pwd","cd","clear"};
 char *cwd;
 char buff[PATH_MAX + 1];
 char *path;
 pid_t pid;

 /* Loop forever to wait and process commands */
 while (1) {
  printf("csc60mshell> ");
  fgets(cmdline, MAXLINE, stdin);
  if(cmdline[0] == '\n')
	continue;
  argc = parseline(cmdline, argv);
  
  /* Step 1: If user hits enter key without a command, continue to loop again at the beginning */
  /*         Hint: look up for they keyword "continue" in C */
  /* Step 1: Call parseline to build argc/argv: argc/argv parameters declared above */ 
  /* Step 1: Handle build-in command: exit, pwd, or cd - if detect one              */
  /* Step 1: Else, fork off a process */

  /******BUILT IN COMMANDS**************/
  //exit command
  if(strcmp(builtIn[0],argv[0]) == 0)
  	exit(EXIT_SUCCESS);
  else{
  	//pwd command
  	if(strcmp(builtIn[1],argv[0]) == 0){
		cwd = getcwd(buff, PATH_MAX + 1);
		if(cwd != NULL){
     			printf("%s\n",cwd);
			continue;
		}
	}
  	else
  		//cd
  		if(strcmp(builtIn[2],argv[0]) == 0){
			//cd with no argument...cd $HOME
			if(argc ==  1){
				char *home;
				home = getenv("HOME");
				if(chdir(home) != 0)
					perror("Error ");	
				continue;	
			}
			path = argv[1];
			if(chdir(path) != 0)
				perror("Invalid");
			continue;	
		}
		else
			if(strcmp(builtIn[3],argv[0]) == 0){
				system("clear");
				continue;
			}
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
  else if (pid == 0)
    //execute execvp...
    process_input(argc, argv);
  else 
    /* I am parent process */
    if (wait(&status) == -1)
      perror("Shell Program error");
    //else
     // printf("Child returned status: %d\n",status);
 }
}
