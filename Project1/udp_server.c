/* 
 * Code to start Socket Programming in IECE/ICSI 516/416
 * Instructor: Prof. Dola Saha
 *
 * udp_server.c - A simple UDP echo server 
 *
 * Compile: gcc udp_server.c -o udp_server
 * 
 * usage: udp_server <port>
 */

/*
 * Code edited for Project 1 in ICSI 516
 * Author: Dan Yee
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024
#define PACKET_SIZE 1042                // maximum size of the "message" packet with the header, length, and payload combined
#define CLIENT_HEADER_SIZE 12           // assume we know the length of the header
#define MESSAGE_LENGTH 4                // length of the message will always be represented as 4 characters

/*
 * error - wrapper for perror
 */
void error(char *msg) {
  perror(msg);
  exit(1);
}

/*
 * Function that converts a String to uppercase letters and returns it.
 */
char* toUpper(char* string)
{
    for (int i = 0; i < strlen(string); i++)
        if (string[i] >= 97 && string[i] <= 122)
            string[i] = string[i] - 32;
    return string;
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
    strncpy(length, packet + CLIENT_HEADER_SIZE, MESSAGE_LENGTH);
    strcat(length, "\0");
    return atoi(length);
}

int main(int argc, char **argv) {
    int sockfd; /* socket */
    int portno; /* port to listen on */
    socklen_t clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    struct hostent *hostp; /* client host info */
    char buf[BUFSIZE]; /* message buf */
    char *hostaddrp; /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */
    int n; /* message byte size */

    char recvPacket[PACKET_SIZE];               // stores the received packet
    char sendtoPacket[PACKET_SIZE];             // used to store the packet that is echoed back to the client
    int length;                                 // length of the message in the received packet
    char lengthStr[5];                          // used to store the length of the message as a char* of 4 digit characters

    /* 
    * check command line arguments 
    */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    portno = atoi(argv[1]);

    /* 
    * socket: create the parent socket 
    */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* setsockopt: Handy debugging trick that lets 
    * us rerun the server immediately after we kill it; 
    * otherwise we have to wait about 20 secs. 
    * Eliminates "ERROR on binding: Address already in use" error. 
    */
    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    /*
    * build the server's Internet address
    */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)portno);

    /* 
    * bind: associate the parent socket with a port 
    */
    if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) 
        error("ERROR on binding");

    /* 
    * main loop: wait for a datagram, then echo it
    */
    clientlen = (socklen_t) sizeof(clientaddr);
    while (1) {

    /*
     * recvfrom: receive a UDP datagram from a client
     */
    bzero(buf, BUFSIZE);
    n = recvfrom(sockfd, recvPacket, PACKET_SIZE, 0, (struct sockaddr *) &clientaddr, &clientlen);
    if (n < 0)
        error("ERROR in recvfrom");

    /* 
     * gethostbyaddr: determine who sent the datagram
     */
    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    if (hostp == NULL)
        error("ERROR on gethostbyaddr");
    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL)
        error("ERROR on inet_ntoa\n");

    /* Display the message length and message on the Server side */
    printf("Server Received Datagram From %s (%s)\n", hostp->h_name, hostaddrp);
    length = getIntLength(recvPacket);
    printf("Message Length: %d\n", length);
    strncpy(buf, recvPacket + CLIENT_HEADER_SIZE + MESSAGE_LENGTH, length);         // strip the header and message length off, leaving only the payload
    printf("Message: %s\n\n", buf);

    /* Craft the packet to be sent back to the client */
    strcpy(sendtoPacket, "UDP Server\0");
    strcpy(lengthStr, "\0");
    getLength(length, lengthStr);                   
    strcat(sendtoPacket, lengthStr);                // append the length to the "text message" as a char* of 4 digits
    strcat(sendtoPacket, "\0");
    strcat(sendtoPacket, toUpper(buf));             // append the uppercased version of the received message
    strcat(sendtoPacket, "\0");

    /* 
     * sendto: echo the input back to the client 
     */
    n = sendto(sockfd, sendtoPacket, strlen(sendtoPacket), 0, (struct sockaddr *) &clientaddr, clientlen);
    if (n < 0) 
        error("ERROR in sendto");
    }
    close(sockfd);
}



