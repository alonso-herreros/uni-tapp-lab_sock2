#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *print_message_function( void *ptr );

main()
{
     pthread_t thread1, thread2;
     const char *message0 = "This is written by parent ";
     const char *message1 = "This is written by Thread 1";
     const char *message2 = "This is written by Thread 2";
     int  iret1, iret2;

    /* Create independent threads each of which will execute function 
	#include <pthread.h>

	int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
	    void *(*start_routine)(void*), void *arg);

	 DESCRIPTION

	The pthread_create() function is used to create a new thread, with attributes specified by attr, within a process. If attr is NULL, 		the default attributes are used. If the attributes specified by attr are modified later, the thread's attributes are not affected. 		Upon successful completion, pthread_create() stores the ID of the created thread in the location referenced by thread.
	The thread is created executing start_routine with arg as its sole argument. If the start_routine returns, the effect is as if 		there was an implicit call to pthread_exit() using the return value of start_routine as the exit status. Note that the thread in 		which main() was originally invoked differs from this. When it returns from main(), the effect is as if there was an implicit call 		to exit() using the return value of main() as the exit status.*/

     iret1 = pthread_create( &thread1, NULL, print_message_function, (void*) message1);
     iret2 = pthread_create( &thread2, NULL, print_message_function, (void*) message2);

	 printf("%s \n", message0);
	
     /* Wait till threads are complete before main continues. Unless we  */
     /* wait we run the risk of executing an exit which will terminate   */
     /* the process and all threads before the threads have completed.   */

     pthread_join( thread1, NULL);
     pthread_join( thread2, NULL); 

     printf("Thread 1 returns: %d\n",iret1);
     printf("Thread 2 returns: %d\n",iret2);
     exit(0);
}

void *print_message_function( void *ptr )
{
     char *message;
     message = (char *) ptr;
     printf("%s \n", message);
}

