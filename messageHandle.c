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

#define BUFFER_SIZE 1024
void displayError(char *msg)
{
    perror(msg);
    exit(1);
}

void messageHandle(int newClientSocketFD)
{
    regex_t getReg;
    regex_t postReg;
    char buffer[256];

    int numberOfElements = read(newClientSocketFD, buffer, 255);
    if (numberOfElements < 0)
        displayError("Error happened while reading incoming message");
    if (regcomp(&getReg, "^GET", 0) < 0)
        displayError("Error while compiling get reg expression");
    if (regcomp(&postReg, "^POST", 0) < 0)
        displayError("Error while compiling post reg expression");
    // checking for type of request
    short i;
    char **linesOfText;
    linesOfText = (char **)malloc(5 * sizeof(char *));
    if (regexec(&getReg, buffer, 0, NULL, 0) == 0)
    {
        printf("Oldu bişeyler\n");
        // seperating the client send data as lines
        short startPoint = 0;
        short count = 0;
        for (i = 0; i <= strlen(buffer); i++)
        {
            if (buffer[i] == '\n' || buffer[i] == '\0')
            {
                // for(int j = 0; j < i - startPoint; j++) {
                //     *(linesOfText[count] + j) = *(buffer + startPoint + j);
                // }
                linesOfText[count] = (char *)malloc((i - startPoint) * sizeof(char));
                strncpy(linesOfText[count], buffer + startPoint, i - startPoint);
                // making the string being null terminated
                linesOfText[count][i - startPoint] = '\0';
                count++;
                startPoint = i + 1;
            }
        }
        printf("Here is the first line: %s\n", linesOfText[0]);
        printf("Here is the length: %ld\n", strlen(linesOfText[0]));
        // the starting point is 4. index
        char *urlEncoded = (char *)malloc((strlen(linesOfText[0]) - 14) * sizeof(char));
        if (urlEncoded == NULL)
            displayError("Error while allocation memory for urlencoded");

        strncpy(urlEncoded, linesOfText[0] + 4, strlen(linesOfText[0]) - 14);
        for (i = 0; i < 5; i++)
        {
            free(linesOfText[i]);
        }
        free(linesOfText);
        printf("Here is the urlEncoded:%s\n", urlEncoded);
        printf("Here is the length:%ld\n", strlen(urlEncoded));
        //-------------------------------------------------------------------------------------------
        // sending the file
        struct binaryTreeStruct *root = NULL;
        root = addRoute(root, "/", "index.html");
        addRoute(root, "/about", "html/about.html");
        addRoute(root, "/bruh", "html/bruh.html");
        postOrder(root);
        int fileHandle;
        size_t fileSize;
        char header[1024];
        char fileBuffer[1024];
        int s;
        struct binaryTreeStruct *selectedRoute = NULL;
        if ((selectedRoute = searchRoute(root, urlEncoded)) != NULL)
        {
            printf("ROUTE FOUND, here is the route: %s\n", selectedRoute->routePath);
            
            if ((fileHandle = open(selectedRoute->routePath, O_RDONLY)) == -1)
                displayError("Error happened while opening file");
            // getting file size by moving pointer to end
            fileSize = lseek(fileHandle, (off_t)0, SEEK_END);
            // moving the pointer to beginning
            lseek(fileHandle, (off_t)0, SEEK_SET);
            printf("File size is: %lu\n", fileSize);

            // constructing header
            sprintf(header, "HTTP/1.1 200 OK\r\n"
                            "Content-type: text/html\r\n"
                            "Server: Diablo sikerim belani he yavsak\r\n"
                            "Content-length: %lu\r\n\r\n",
                    fileSize);
            // sending header
            if (write(newClientSocketFD, header, strlen(header)) < 0)
                displayError("error happened while writing the header");

            // sending the file
            if ((s = read(fileHandle, fileBuffer, fileSize)) == -1)
                displayError("error happened while reading the file of 1024byte");
            if ((write(newClientSocketFD, fileBuffer, s)) < 0)
                displayError("error happened while writing the file data to client");
            // bzero(fileBuffer, 1024);
        }
        else
        {    
            printf("ROUTE NOT FOUND\n");
            if ((fileHandle = open("html/404.html", O_RDONLY)) == -1)
                displayError("Error happened while openning error file");
            fileSize = lseek(fileHandle, (off_t)0, SEEK_END);
            lseek(fileHandle, (off_t)0, SEEK_SET);
            printf("File size is: %lu\n", fileSize);
            sprintf(header, "HTTP/1.1 404 NOT FOUND\r\n"
                                 "Content-type: text/html\r\n"
                                 "Server: Diablo sikerim belani he yavsak\r\n"
                                 "Content-length: %lu\r\n\r\n",
                    fileSize);
            if (write(newClientSocketFD, header, strlen(header)) < 0)
                displayError("error happened while writing the header");
            if ((s = read(fileHandle, fileBuffer, 1024)) == -1)
                displayError("error happened while reading the file of 1024byte");
            if ((write(newClientSocketFD, fileBuffer, s)) < 0)
                displayError("error happened while writing the file data to client");
        }
        free(urlEncoded);
    }
    else if (regexec(&postReg, buffer, 0, NULL, 0) == 0)
    {
        printf("this is post");
    }
    else
    {
        displayError("Error happened while checking for type of request");
    }

    // printf("Here is the message: %s", buffer);
}
