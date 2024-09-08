#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <time.h>


#define BUFSIZE 65536 /* larger = more efficient */
#define CHUNK 16384
#define ERROR 31
#define LOG 32
#define FORBIDDEN 403
#define NOTFOUND 404



void logger(int type, char* message, int clientSocketFd, struct in_addr clientIp) {
    char logBuffer[BUFSIZE * 2];
    char header[256];
    char fileBuffer[1024];
    char timeBuffer[CHUNK];
    int byteRead;
    int fileDescriptor;
    int logFileDescriptor;
    int fileSize;
    char *clientIpStr = inet_ntoa(clientIp);
    
    switch (type)
    {
    case NOTFOUND:
        if((fileDescriptor = open("gzipHtml/404.html.gz", O_RDONLY)) < -1) sprintf(logBuffer, "NOT FOUND: Error while opening the 404 file");
        fileSize = lseek(fileDescriptor, SEEK_SET, SEEK_END);
        lseek(fileDescriptor, SEEK_SET, SEEK_SET);
        sprintf(header, "HTTP/2 404 NOT FOUND\r\n"
                                "Content-Type: text/html \r\n"
                                "Server: Diablo sikerim belani he yavsak\r\n"
                                "Content-Encoding: gzip\r\n"
                                "Content-Length: %d\r\n\r\n",
                        fileSize);
        if(write(clientSocketFd, header, strlen(header)) < 0) sprintf(logBuffer, "NOT FOUND: Error while sending the header");
        if((byteRead = read(fileDescriptor, fileBuffer, fileSize)) < 0) sprintf(logBuffer, "NOT FOUND: Error while reading the file");
        if(write(clientSocketFd, fileBuffer, byteRead) < 0) sprintf(logBuffer, "NOT FOUND: Error while writing the file");
        close(fileDescriptor);
        break;
    case ERROR:
        
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        sprintf(timeBuffer, "now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour + 3, tm.tm_min, tm.tm_sec);
        sprintf(logBuffer, "ERROR: message; %s, errno: %d, exiting pid: %d, ip adress: %s, time: %s", message, errno, getpid(), clientIpStr, timeBuffer );
        break;
    case LOG:
        sprintf(logBuffer, "LOG: info; %s, pid: %d ", message, getpid());
        break;
    default:
        break;
    }
    char breakLine[] = "----------------------------------\n";
    if((logFileDescriptor = open("diablo.log",  O_CREAT| O_WRONLY | O_APPEND)) >=0) {
        write(logFileDescriptor, logBuffer, strlen(logBuffer));
        write(logFileDescriptor,breakLine , strlen(breakLine));
        close(logFileDescriptor);
    }
    if(type == ERROR || type == NOTFOUND) {
        exit(3); 
    }
}
