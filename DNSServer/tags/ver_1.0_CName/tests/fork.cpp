#include <stdio.h>   /* printf, stderr, fprintf */
#include <unistd.h>  /* _exit, fork */
#include <stdlib.h>  /* exit */
#include <errno.h>   /* errno */
 
int main(void)
{
   pid_t  pid;
 
   /* Output from both the child and the parent process
    * will be written to the standard output,
    * as they both run at the same time.
    */
   pid = fork();
   if (pid == 0)
   {
      /* Child process:
       * When fork() returns 0, we are in
       * the child process.
       * Here we count up to ten, one each second.
       */
      int j;
      for (j = 0; j < 10; j++)
      {
         printf("child: %d\n", j);
         sleep(1);
      }
      _exit(0);  /* Note that we do not use exit() */
   }
   else if (pid > 0)
   { 
      int i;
      /*for (i = 0; i < 10; i++)
      {
         printf("parent: %d\n", i);
        sleep(0.1);
      }*/
    //  exit(0);
   }
   else
   {   
      /* Error:
       * When fork() returns a negative number, an error happened
       * (for example, number of processes reached the limit).
       */
      fprintf(stderr, "can't fork, error %d\n", errno);
      exit(EXIT_FAILURE);
   }
	int v = 0;
	while(v< 10){
		printf("F1234\n");
		sleep(1);
		v++;
	}


}
