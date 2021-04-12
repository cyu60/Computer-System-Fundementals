#include <stdio.h>      /* for snprintf */
#include "csapp.h"
#include "calc.h"
#include "stdbool.h"

#define LINEBUF_SIZE 1024

bool chat_with_client(struct Calc *calc, int infd, int outfd) {
	rio_t in;
	char linebuf[LINEBUF_SIZE];

	/* wrap standard input (which is file descriptor 0) */
	rio_readinitb(&in, infd);

	/*
	 * Read lines of input, evaluate them as calculator expressions,
	 * and (if evaluation was successful) print the result of each
	 * expression.  Quit when "quit" command is received.
	 */
	int done = 0;
	while (!done) {
		ssize_t n = rio_readlineb(&in, linebuf, LINEBUF_SIZE);
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
				rio_writen(outfd, "Error\n", 6);
			} else {
				/* output result */
				int len = snprintf(linebuf, LINEBUF_SIZE, "%d\n", result);
				if (len < LINEBUF_SIZE) {
					rio_writen(outfd, linebuf, len);
				}
			}
		}
	}
	return true;
}

int main(int argc, char **argv) {
	/* TODO: implement this program */
	if (argc != 2) {
		fprintf("%s\n", "Invalid number of arguments");
		return 1;
	}
	if (argv[1] < 1024) {
		fprintf("%s\n", "Invalid number for server port");
		return 1;	
	} else {
		int serverNumber = argv[1];
	}
	int open_indicator = Open_listenfd(argv[1]);
	if (open_indicator < 0) {
		fprintf("%s\m", "Error opening the server.");
		return 1;
	}
	struct calc* calc_server = calc_create();
	while (true) {
		int client_indicator = Accept(open_indicator, NULL, NULL); 
		if (client_indicator < 0){
      		fprintf("%s\n", "Couldn't create a connection with the client.");
    	}
		int result = chat_with_client(calc_server, client_indicator, client_indicator);
		if (!result) {
			return 0;
		}
		close(client_indicator);
	}
	close(open_indicator);
	calc_destroy(calc_server);
	return 0;
}
