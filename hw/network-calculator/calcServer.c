#include <stdio.h> /* for fprintf, snprintf */
#include "csapp.h"
#include "calc.h"

/* buffer size for reading lines of input from user */
#define LINEBUF_SIZE 1024

/* 
 * Read client command and send back calculated result
 * through the connection descriptor.
 * Return 0 if client enters shutdown command,
 * return 1 if client indicates end of session(quit).
 */
int chat_with_client(struct Calc *calc, int connfd);
void check_error(int fd, char *message);

int main(int argc, char **argv)
{
	int listenfd, connfd, clientlen;
	struct sockaddr clientaddr;

	/* check command line argument */
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}

	/* create a listening server socket ready to accept connection requests */
	char *port = argv[1];
	listenfd = open_listenfd(port);
	check_error(listenfd, "Failed to create server socket.");
	printf("Server is listening on port %s.\n", port);

	/* Each loop iteration is a client session */
	int acceptConn = 1;
	struct Calc *calc = calc_create();
	while (acceptConn)
	{
		clientlen = sizeof(clientaddr);

		//wait for connection request
		printf("Looking for new client connection...\n");
		connfd = Accept(listenfd, &clientaddr, (socklen_t *)&clientlen);
		check_error(connfd, "Failed to accept connection request.");
		printf("Connection established!\n");

		//serve the client
		acceptConn = chat_with_client(calc, connfd);

		//service returned, terminate connection
		Close(connfd);
		printf("Connection with current client closed!\n");
	}
	calc_destroy(calc);

	printf("A client has asked to end the server process.\n");
	return 0;
}

int chat_with_client(struct Calc *calc, int connfd)
{
	rio_t conn;
	char linebuf[LINEBUF_SIZE];

	/* wrap connection descriptor to rio_t */
	rio_readinitb(&conn, connfd);

	/*
	 * Read lines of input, evaluate them as calculator expressions,
	 * and (if evaluation was successful) print the result of each
	 * expression.  Quit when "quit" or "shutdown" command is received.
	 */
	int done = 0;
	while (!done)
	{
		/* read client input */
		ssize_t n = rio_readlineb(&conn, linebuf, LINEBUF_SIZE);
		if (n <= 0)
		{
			/* error or end of input */
			done = 1;
		}
		else if (strcmp(linebuf, "quit\n") == 0 || strcmp(linebuf, "quit\r\n") == 0)
		{
			/* quit command, end session */
			done = 1;
		}
		else if (strcmp(linebuf, "shutdown\n") == 0 || strcmp(linebuf, "shutdown\r\n") == 0)
		{
			/* shutdown command, end server process */
			return 0;
		}
		else
		{
			/* process input line from client */
			int result;
			if (calc_eval(calc, linebuf, &result) == 0)
			{
				/* expression couldn't be evaluated */
				rio_writen(connfd, "Error\n", 6);
			}
			else
			{
				/* output result to client */
				int len = snprintf(linebuf, LINEBUF_SIZE, "%d\n", result);
				if (len < LINEBUF_SIZE)
				{
					rio_writen(connfd, linebuf, len);
				}
			}
		}
	}
	return 1; //keep accepting new clients
}

/* print error message if file descriptor is negative */
void check_error(int fd, char *message)
{
	if (fd < 0)
	{
		fprintf(stderr, "%s\n", message);
		exit(1);
	}
}
