/* TCPechod.c - main, TCPechod */ 
		  
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/wait.h>


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



/*------------------------------------------------------------------------
 * main - Concurrent TCP server for ECHO service
 *------------------------------------------------------------------------
 */

#define MAX_CHILDREN 10

int main(int argc, char *argv[])
{
  	pid_t child_pid;
	pid_t tpid;	
	int count_children = 0;
	int count=0;
	int child_status;
	int test=100; 
		
	printf("P: Parent process running\n");
	printf("P: The parent test variable has the value of %d\n", test);
	child_pid = fork();
		
	switch (child_pid) {
		case 0:	/* child */
			printf("\tchild %d : The value of variable test, for me is... %d\n",getpid(),test);
			for(count = 0; count <10; count++)
			{
				test+=1;
				printf("\tchild %d : The value of variable test, for me is... %d\n",getpid(),test);
			}
			break;
		default:/* parent */
			printf("P: Waiting for my son to finish\n");
			tpid = wait(&child_status);
			printf("P: The parent test variable has the value of %d\n", test);
			printf("*******************\n");
			printf("P: So, my son copied my variables (test) and the values they have when\n I made the fork, but in a separate space of memory, that's why the value\n of my test var is not the same as his\n");
			
			break;
		case -1:
			printf("fork: returned error\n");
	}
 }



