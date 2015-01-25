#include  <stdio.h>
#include  <sys/types.h>
#include <unistd.h>  /* _exit, fork */
#include <stdlib.h>  /* exit */
#include <errno.h> 
#include <iostream>
#include <string>
#include <string.h>

#define   MAX_COUNT  200

void  ChildProcess(std::string Ident);                /* child process prototype  */
void  ParentProcess(void);               /* parent process prototype */

int  main(void)
{
     pid_t  pid;

     pid = fork();
     if (pid == 0) 
          ChildProcess("Main Child");
     else 
          ParentProcess();
}

void  ChildProcess(std::string Ident)
{
     int   i;

     for (i = 1; i <= MAX_COUNT; i++){
		sleep(1);        
	  	std::cout << "Child: " << Ident << std::endl;
	}	
     printf("   *** Child process is done ***\n");
	
}

void  ParentProcess(void)
{
	pid_t  pid;

     pid = fork();
     if (pid == 0) 
          ChildProcess("Second Child");
     else 
          ChildProcess("Second Parent");
	
}
