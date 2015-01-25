#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <string.h>


void *print_message_function( void *ptr );
void *print_second(void *ptr);
void* QueryRBLNameserver(void* ptr);

     
char* arraytest(unsigned char* DNSResponse){

//	char DNSResponse[512];
	*(DNSResponse + 1) = 100;
*(DNSResponse + 0) = 122;
*(DNSResponse + 2) = 121;
std::cout << "helo: " <<  (int)DNSResponse[0] << std::endl;
//	return DNSResponse;
//	std::cout << "withinhelo: " <<  (int)DNSResponse[0] << std::endl;
//	printf("%02x", DNSResponse[0] );
}

main()
{
	char test[5] = {'v','e'};
 	int nu = 2;
     pthread_t thread[nu];
char *message1 = "Thread 1";
     char *message2 = "Thread 2";
     int  iret1, iret2;

	int delay = 2; 
	std::cout << "sizeof: " << strlen(test) << std::endl;
	std::cout << "test[0]: " << test[0] << std::endl;
std::cout << "test[1]: " << test[1] << std::endl;
	unsigned char helo[512];
	arraytest(helo);
	std::cout << "helo: " <<  helo[0] << std::endl;
    /* Create independent threads each of which will execute function */
	for(int i = 1;i < 3; i++){
		if(i==1){
			delay = 10000000;
		}else{
			delay = 10000000;		
		}
	    if(pthread_create( &thread[i], NULL, print_message_function, (void*) delay)){
		perror("Creation of Thread failed");
		throw;
		}
	printf("test\n");
	}
   //  iret2 = pthread_create( &thread2, NULL, print_second, (void*) message2);

     /* Wait till threads are complete before main continues. Unless we  */
     /* wait we run the risk of executing an exit which will terminate   */
     /* the process and all threads before the threads have completed.   */
	void *status1, *status2;
	void *status[2];
	for(int i = 1;i < 3; i++){   
		if(pthread_join( thread[i], &status[i])){
			perror("Join thread failed.");
		}//end if statement
	}//end for loop
/*	pthread_join( thread[1], &status1);
	pthread_join( thread[2], &status2); 
*/	std::cout << "Message1: " <<  (char*)status[1] << std::endl;
	std::cout << "Message2: " <<  (char*)status[2] << std::endl;

     exit(0);
}

void *print_message_function( void *ptr )
{

/*     char *message;
     message = (char *) ptr;
     printf("%s \n", message);
*/	int i = 0;
std::cout << "ptr: " << (long long)ptr << std::endl;
	while(i < (long long)ptr){
	//	printf("%d\n", i );
//sleep(1);
		i++;
	}
std::cout << "test" << std::endl;
 //const char *ret = (char*) malloc(sizeof(char));
const char *ret = "hello";
	return((void*)ret);
	
}

//query the RBL NameServer
void* QueryRBLNameserver(void* ptr){


}//end querying RBL Nameserver

void *print_second(void *ptr){
	int i = 0;	
	while(i < 10){
//		printf("%d\n", i );
//sleep(1);
		i++;
	}
}
