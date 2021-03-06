// purpose: comparing Apache Benchmark tool vs. this code against nginx web server
// author: Dehqanpour
//To compile: gcc -O2 tcp_cli_ab.c -lpthread
// To run: ./a.out <# of total reqs> <# of parallel threads>
//#include <stdlib.h>
//#include <time.h>
#include<stdio.h>
#include<string.h>    
#include<sys/socket.h>
#include<arpa/inet.h> //for inet_addr
#include <pthread.h>
#include "timer.h"
void* handle_client(void*);
struct sockaddr_in server;
unsigned short n=0; 
char message[45] = "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n"; //simple GET request to fetch home page


int main(int argc , char *argv[])
{
	int N; //Total requests will be send to server

	if (argc != 3) {
		fprintf(stderr, "We need two positional parameter (n,c)");
		return 1;
	 }
	unsigned short c; // number of concurrent clients (# of parallel threads)
	int j;
	double t1,t2;
	N=atoi(argv[1]);
	c=atoi(argv[2]);
	n=N/c; // each thread make n request
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 80 ); 
    
	pthread_t* handles = (pthread_t*) malloc (c*sizeof(pthread_t));
	
	
	//clock_t t1 = clock();
	GET_TIME(t1);
	for (j=0; j<c; j++)  
    pthread_create( &handles[j], NULL, handle_client, (void*)j );  


	for (j=0; j<c; j++) {
      pthread_join(handles[j], NULL);
		}

	GET_TIME(t2);
	printf("\nResult = %g #/sec \n", n/(t2-t1));
    return 0;
}





void* handle_client(void* rank) {

	int socket_desc;
	char  server_reply[2000];
	unsigned short i;

  	//Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }

    //Connect to remote server
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("connect error");
        return NULL;
    }
   //Send some data
  for(i=0; i<n; i++){
    if( send(socket_desc , message , strlen(message) , 0) < 0)
    {
        puts("Send failed");
        return NULL;
    }
    //Receive a reply from the server
    if( recv(socket_desc, server_reply , 2000 , 0) < 0)
    {
        puts("recv failed");
    }
  
	
}
	close(socket_desc);
   return NULL;
}