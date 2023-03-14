 /* 
 * Code to start Socket Programming in IECE/ICSI 416
 * Instructor: Prof. Dola Saha
 *
 * tcp_client.c - A simple TCP client 
 *
 * Compile: gcc tcp_client.c -o tcp_client
 * 
 * usage: tcp_client <host> <port>
 */

/*
 * Code edited for Project 1 in ICSI 516
 * Author: Dan Yee
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>

#define BUFSIZE 1024

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char **argv) {
    int sockfd, portno, n;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];
	struct timespec start, end;

    FILE* output = fopen("output.txt", "w");            // stores the output returned from the server
    char* splitStr;
    char recvCountBuf[BUFSIZE];
    int recvCount = 0;
    int writeAmount;
	
    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* connect: create a connection with the server */
    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) 
        error("ERROR connecting");

    /* get message line from the user */
    printf("Enter file name and path: ");
    bzero(buf, BUFSIZE);
    fgets(buf, BUFSIZE, stdin);
    
    /* send the file name and path to the server */
    n = write(sockfd, buf, strlen(buf));
    if (n < 0) 
        error("ERROR writing to socket");

    /* print the server's reply */
    // loop until the special char "$" is received
    while (strcmp(buf, "$") != 0)
    {
        bzero(buf, BUFSIZE);
        n = read(sockfd, buf, BUFSIZE);
        if (n < 0) 
            error("ERROR reading from socket");

        // need to split the read buffer by \n because read() reads more than each line sent by the server
        splitStr = strtok(buf, "\n");
        while (splitStr != NULL && strcmp(splitStr, "$") != 0)
        {
            writeAmount = fwrite(splitStr, sizeof(char), strlen(splitStr), output);     // write the read line to file
            if (writeAmount < 0)
                error("ERROR writing to file");
            writeAmount = fwrite("\n\0", sizeof(char), strlen("\n\0"), output);         // write the \n char, which was removed by strtok()
            if (writeAmount < 0)
                error("ERROR writing to file");
            splitStr = strtok(NULL, "\n");                                              // get the next instance of \n
            recvCount++;

            if (recvCount % 4 == 0)                                                     // for every 4 messages received, send an "ACK"-like message to the server
            {
                sprintf(recvCountBuf, "%d", recvCount);
                strcpy(buf, "Received messages up to # \0");
                strcat(buf, recvCountBuf);
                strcat(buf, "\0");

                n = write(sockfd, buf, strlen(buf));
                if (n < 0)
                    error("ERROR writing to socket");
            }
        }
    }
    fclose(output);                                         // close the output file
    close(sockfd);                                          // close the socket
    
    if (recvCount == 0)
        printf("Error: Requested file not found on Server.\n");
    else
        printf("File data transfer complete.\n");
    return 0;
}