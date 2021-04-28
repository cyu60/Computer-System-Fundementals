//magarw13 - Mehul Agarwal
//cyu60 - Chinat Yu

#include <stdio.h>      /* for snprintf */
#include "csapp.h"
#include "calc.h"
#include "stdbool.h"
#include "pthread.h"
#include <sys/types.h>

#define LINEBUF_SIZE 1024

//for each client connneciton, server will start a new thread. 
//with each thread that you're detaching, have a worker method that is called everytime a thread is detached
//worker method handles metadata that does the actual chatting with the client
//variables are consistent across client-connections
struct ThreadData {
	struct Calc *calc;
	int client_fd;
};

/* peer thread will change this variable to 0 upon shutdown command*/
volatile int accept_connections;
/* incremented when a connection starts and decremented when it ends */
sem_t numThreadsAlive; 

int chat_with_client(struct Calc *calc, int clientfd);
void wait_for_clients_exit();
void * worker_function(void *arg);
struct timeval getTimeout(int second);

int chat_with_client(struct Calc *calc, int clientfd) {
	rio_t in;
	char linebuf[LINEBUF_SIZE];

	/* wrap standard input (which is file descriptor 0) */
	Rio_readinitb(&in, clientfd);

	/*
	 * Read lines of input, evaluate them as calculator expressions,
	 * and (if evaluation was successful) print the result of each
	 * expression.  Quit when "quit" command is received.
	 */
	int done = 0;
	while (!done) {
		ssize_t n = Rio_readlineb(&in, linebuf, LINEBUF_SIZE);
		if (n <= 0) {
			/* error or end of input */
			done = 1;
		} else if (strcmp(linebuf, "quit\n") == 0 || strcmp(linebuf, "quit\r\n") == 0) {
			/* quit command */
			done = 1;
		} else if (strcmp(linebuf, "shutdown\n") == 0 || strcmp(linebuf, "shutdown\r\n") == 0) {
			// Shutdown command
			return false;
		} else {
			/* process input line */
			int result;
			if (calc_eval(calc, linebuf, &result) == 0) {
				/* expression couldn't be evaluated */
				Rio_writen(clientfd, "Error\n", 6);
			} else {
				/* output result */
				int len = snprintf(linebuf, LINEBUF_SIZE, "%d\n", result);
				if (len < LINEBUF_SIZE) {
					Rio_writen(clientfd, linebuf, len);
				}
			}
		}
	}
	return true;
}

void * worker_function(void *arg) {
	sem_post(&numThreadsAlive); // Increase semaphore
	struct ThreadData *info = arg;
	pthread_detach(pthread_self());
	if(!chat_with_client(info->calc, info->client_fd)) {
		accept_connections = 0; // check for shut down
	}
	Close(info->client_fd);
	free(info);
	sem_wait(&numThreadsAlive); // decrease semaphore
	return NULL;
}

int main(int argc, char **argv) {
	
	if (argc != 2) {
		printf("%s\n", "Invalid number of arguments");
		return 1;
	}

	int listenfd = Open_listenfd(argv[1]);
	if (listenfd < 0) {
		printf("%s\n", "Error opening the server.");
		return 1;
	}
	struct sockaddr addr;
	int addrlen, client_indicator;
	
	// Set up global vars
	accept_connections = 1;
	Sem_init(&numThreadsAlive, 0, 0);
	struct Calc* calc_server = calc_create();

	/* setup listenfd for select() */
	fd_set readfds;
	FD_ZERO (&readfds);
	FD_SET(listenfd, &readfds);

	while (accept_connections) {
		struct timeval timeout = getTimeout(1);
		FD_SET(listenfd, &readfds);
		Select(listenfd + 1, &readfds, NULL, NULL, &timeout); //checks if listenfd is ready

		if (FD_ISSET(listenfd, &readfds)) {  // connection req recieved
			addrlen = sizeof(addr);
			client_indicator = Accept(listenfd, &addr, (socklen_t *)&addrlen); 
			if (client_indicator < 0){
      			printf("%s\n", "Couldn't create a connection with the client.");
    		}
			// create thread
			struct ThreadData *info = malloc(sizeof(struct ThreadData));
			info->client_fd = client_indicator;
			info->calc = calc_server;
			pthread_t thr_id;
			Pthread_create(&thr_id, NULL, worker_function, info);
		}

	}
	Close(listenfd);
	wait_for_clients_exit();
	calc_destroy(calc_server);
	return 0;	

}

void wait_for_clients_exit() {
	int numConn = 0;
	sem_getvalue(&numThreadsAlive, &numConn);
	while(numConn) {
		sleep(1);
		sem_getvalue(&numThreadsAlive, &numConn);
	}
	sem_destroy(&numThreadsAlive);
}

/* return a timeval struct to periodically wake up selec()*/
struct timeval getTimeout(int second) {
	struct timeval tv;
	tv.tv_sec = second;
	tv.tv_usec = 0; //microsecond
	return tv;
}