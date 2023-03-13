/* 
 * udp_client.c - A simple UDP client
 * usage: udp_client <host> <port>
 */
 /* 
 * Code to start Socket Programming in IECE/ICSI 516/416
 * Instructor: Prof. Dola Saha
 *
 * udp_client.c - A simple UDP client 
 *
 * Compile: gcc udp_client.c -o udp_client
 * 
 * usage: udp_client <host> <port>
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

#define BUFSIZE 1024
#define PACKET_SIZE 1042                // maximum size of the "message" packet with the header, length, and payload combined
#define SERVER_HEADER_SIZE 10           // assume we know the length of the header
#define MESSAGE_LENGTH 4                // length of the message will always be represented as 4 characters

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

/* Function that buffers out the length of the message, if necessary, to be 4 characters in length */
char* getLength(long length, char* msgLength)
{
    char lengthStr[5];
    sprintf(lengthStr, "%ld", length);
    strcat(lengthStr, "\0");

    if (length < 10)
        strcat(msgLength, "000\0");
    else if (length < 100)
        strcat(msgLength, "00\0");
    else if (length < 1000)
        strcat(msgLength, "0\0");
    strcat(msgLength, lengthStr);
    strcat(msgLength, "\0");
    return msgLength;
}

/* Function that strips the message length off the "packet" and returns it as an int */
int getIntLength(char* packet)
{
    char length[5];
    strncpy(length, packet + SERVER_HEADER_SIZE, MESSAGE_LENGTH);
    strcat(length, "\0");
    return atoi(length);
}

int main(int argc, char **argv) {
    int sockfd, portno, n;
    socklen_t serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];

    char msgLength[5];                  // stores the length of the msg as a 4 digit number, pre-pended with 0s if necessary
    strcpy(msgLength, "\0");
    char sendtoPacket[PACKET_SIZE];     // stores the entire packet (header, length, message) to be sent to the Server
    char recvPacket[PACKET_SIZE];
    int length;

    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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

    /* get a message from the user */
    bzero(buf, BUFSIZE);
    printf("Enter Message: ");
    fgets(buf, BUFSIZE, stdin);

    /* Craft the "text message" as a packet */
    strcpy(sendtoPacket, "UDP Client 1\0");
    getLength(strlen(buf) - 1, msgLength);              // subtract 1 because fgets() includes '\n' as one of the characters.
    strcat(sendtoPacket, msgLength);
    strcat(sendtoPacket, "\0");
    strcat(sendtoPacket, buf);
    strcat(sendtoPacket, "\0");

    /* send the message to the server */
    serverlen = (socklen_t) sizeof(serveraddr);
    n = sendto(sockfd, sendtoPacket, strlen(sendtoPacket), 0, (struct sockaddr*)&serveraddr, serverlen);
    if (n < 0) 
        error("ERROR in sendto");
    
    /* print the server's reply */
    n = recvfrom(sockfd, recvPacket, PACKET_SIZE, 0, (struct sockaddr*)&serveraddr, &serverlen);
    if (n < 0) 
        error("ERROR in recvfrom");
    
    length = getIntLength(recvPacket);
    printf("Response Length: %d\n", length);
    strncpy(buf, recvPacket + SERVER_HEADER_SIZE + MESSAGE_LENGTH, length);
    printf("Response From Server: %s", buf);
    
    close(sockfd);
    return 0;
}