#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <strings.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include "Routing.c"
#include <sys/stat.h>
#include "ErrorHandling.c"
#include <netinet/in.h>
#include <errno.h>

#define BUFSIZE 65536 /* larger = more efficient */
#define CHUNK 16384
#define ERROR 31
#define LOG 32
#define FORBIDDEN 403
#define NOTFOUND 404

void displayError(int type,char *msg, int newClientSocketFD, struct in_addr clientIp)
{
    perror(msg);
    //same thing as perror with written in buffer
    char messageWithErrorBuffer[strlen(msg) + 256];
    sprintf(messageWithErrorBuffer, "%s : %s", msg, strerror(errno));
    logger(type, messageWithErrorBuffer, newClientSocketFD, clientIp);
}

char **seperatingByLine(char **linesOfText, char buffer[], short *lineNumber)
{
    int i;
    short startPoint = 0;
    short count = 0;
    for (i = 0; i <= strlen(buffer); i++)
    {
        if (buffer[i] == '\n' || buffer[i] == '\0')
        {
            linesOfText = (char **)realloc(linesOfText, (count + 1) * sizeof(char *));
            if (linesOfText == NULL)
            {
                printf("Memory allocation patladı");
                exit(1);
            }
            linesOfText[count] = (char *)malloc((i - startPoint) * sizeof(char));
            if (linesOfText[count] == NULL)
            {
                printf("Memory allocation patladı");
                exit(1);
            }
            strncpy(linesOfText[count], buffer + startPoint, i - startPoint);
            // making the string being null terminated
            linesOfText[count][i - startPoint] = '\0';
            startPoint = i + 1;
            count++;
        }
    }
    *lineNumber = count;
    return linesOfText;
};

char *extractFileRequest(char *request, char *filePath)
{
    short count = 0;
    short i;
    short start, end;
    for (i = 0; i < strlen(request); i++)
    {
        if (request[i] == ' ')
        {
            switch (count)
            {
            case 0:
                start = i;
                count++;
                break;
            case 1:
                end = i;
                break;
            default:
                break;
            }
        }
    }
    size_t length = end - start - 1;
    // printf("LENGTH is %lu",length);
    filePath = (char *)malloc((length + 1) * sizeof(char));
    if (filePath == NULL)
    {
        printf("Error for location memory for filePath");
    }
    for (i = 0; i < length; i++)
    {
        *(filePath + i) = *(request + start + 1 + i);
    }
    filePath[length] = '\0';
    return filePath;
}

char *getMimeType(char *line)
{
    char *mime_type;
    if (strstr(line, ".html") != NULL)
    {
        mime_type = "text/html";
    }
    else if (strstr(line, ".css") != NULL)
    {
        mime_type = "text/css";
    }
    return mime_type;
}

void messageHandle(int newClientSocketFD, struct in_addr clientIp)
{
    regex_t getReg;
    regex_t postReg;
    char readBuffer[CHUNK];
    // checking for type of request
    if (read(newClientSocketFD, readBuffer, CHUNK - 1) < 0){
        displayError(ERROR,"Error happened while reading incoming message", newClientSocketFD, clientIp);
    }
    if (regcomp(&getReg, "^GET", 0) < 0) {
        displayError(ERROR, "Error while compiling get reg expression", newClientSocketFD, clientIp);
    }
    if (regcomp(&postReg, "^POST", 0) < 0) {
        displayError(ERROR, "Error while compiling post reg expression", newClientSocketFD, clientIp);
    }
    short i;
    // seperating the client send data as lines
    char **linesOfText = NULL;
    //getting request line count
    short lineCount;
    printf("Oldu bişeyler\n");
    linesOfText = seperatingByLine(linesOfText, readBuffer, &lineCount);
    for (i = 0; i < lineCount; i++)
    {
        printf("\e[1;34mHere is the %d line: %s\e[m\n", i, linesOfText[i]);
    }
    printf("Here is the number of request lines made from browser: %d\n", lineCount);
    char *filePath = NULL;
    filePath = extractFileRequest(linesOfText[0], filePath);
    printf("Here is the filePath:%s\n", filePath);

    //-------------------------------------------------------------------------------------------
    // constructing the routes

    struct binaryTreeStruct *root = NULL;
    root = addRoute(root, "/", "index.html.gz");
    addRoute(root, "/about", "gzipHtml/about.html.gzz");
    addRoute(root, "/bruh", "gzipHtml/bruh.html.gz");
    addRoute(root, "/styles/styles.css", "styles/styles.css.gz");
    postOrder(root);

    //-------------------------------------------------------------------------------------------
    //GET method and sending the file

    if (regexec(&getReg, linesOfText[0], 0, NULL, 0) == 0)
    {   
        char header[1024];
        char fileBuffer[CHUNK];
        int fileSize;
        int fileDescriptor;
        int byteRead;
        struct binaryTreeStruct *selectedRoute = NULL;
        if ((selectedRoute = searchRoute(root, filePath)) != NULL)
        {
            printf("GET: ROUTE FOUND, here is the route: %s\n", selectedRoute->routePath);
            char *mime_type = getMimeType(selectedRoute->routePath);
            printf("Mime type is : %s\n", mime_type);
            if ((fileDescriptor = open(selectedRoute->routePath, O_RDONLY)) == -1){
                displayError(ERROR, "Error happened while opening the file", newClientSocketFD, clientIp);
            }
            char logMessageBuffer[23 + strlen(selectedRoute->routePath)];
            sprintf(logMessageBuffer, "GET: request made to %s", selectedRoute->routePath);
            // TO DO: fix this mf
            // freeBinaryTree(root);
            fileSize = lseek(fileDescriptor, (off_t)0, SEEK_END);
            // moving the pointer to beginning
            lseek(fileDescriptor, (off_t)0, SEEK_SET);
            printf("Requested file size is: %d\n", fileSize);
            sprintf(header, "HTTP/2 200 OK\r\n"
                            "Content-Type: %s\r\n"
                            "Server: Diablo sikerim belani he yavsak\r\n"
                            "Content-Encoding: gzip\r\n"
                            "Location: Annen\r\n"
                            "Cache-Control: max-age=100, must-revalidate\r\n"
                            "Content-Length: %d\r\n\r\n",
                    mime_type, fileSize);
            if (write(newClientSocketFD, header, strlen(header)) < 0){
                displayError(ERROR, "Error writing HTTP header", newClientSocketFD, clientIp);
            }
            if ((byteRead = read(fileDescriptor, fileBuffer, fileSize)) == -1) {
                displayError(ERROR,"error happened while reading the file of 1024byte", newClientSocketFD, clientIp);
            }
            printf("Byte read is %d\n", byteRead);
            if ((write(newClientSocketFD, fileBuffer, byteRead)) < 0)
                displayError(ERROR,"error happened while writing the file data to client", newClientSocketFD, clientIp);
            
            logger(LOG, logMessageBuffer, newClientSocketFD, clientIp);
        }
        else
        {
            printf("NO ROUTE FOUND, giving 404 error. ");
            logger(NOTFOUND, "", newClientSocketFD, clientIp);
            freeBinaryTree(root);
        }
        free(filePath);
    }
    else if (regexec(&postReg, linesOfText[0], 0, NULL, 0) == 0)
    {
        printf("this is post");
    }
    else
    {
        displayError(ERROR,"Error happened while checking for type of request", newClientSocketFD, clientIp);
    }
    free(linesOfText);

}

/*
TO DO:
-supporting json files

--creating a site that supports mysql / dynamic routes 
-supporting https (kind of)
*/