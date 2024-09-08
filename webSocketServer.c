#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
// for importing internet address structure
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "messageHandle.c"
#include <signal.h>

// info to vds server:
// 185.48.183.186
// ubuntu
// c7Y?E%fyk9
int main(int argc, char **argv)
{
    int socketFD, newClientSocketFD;
    // assigning random value to client ip for log messages
    struct in_addr clientIp;
    clientIp.s_addr = 32;
    struct sockaddr_in serverAdress, clientAdress;
    if (argc < 2)
    {
        printf("Man please provide a host to init\n");
        exit(1);
    }
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0 || socketFD == 2)
        displayError(ERROR,"Socket init failed", 31, clientIp);
    // making process deamon by forking it from its parent and making it take init process
    //  if(fork() != 0){
    //      return 0;
    //  }
    //  (void)signal(SIGHUP, SIG_IGN); /* ignore terminal hangups */
    (void)signal(SIGCLD, SIG_IGN); /* ignore child death */

    serverAdress.sin_family = AF_INET;            // uses tcp instead of udp
    serverAdress.sin_port = htons(atoi(argv[1])); // necessary to convert host byte order to network
    serverAdress.sin_addr.s_addr = INADDR_ANY;    // for accepting all connections
    printf("WebSocketServer listening on port %d...\n", atoi(argv[1]));

    // for making port resusable even though it may be busy (generally for test purposes) *bruh still not works nvm
    int reuse = 1;
    if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        displayError(ERROR, "Making port reusable even though it is busy", 31, clientIp);
    }

    if (bind(socketFD, (struct sockaddr *)&serverAdress, sizeof(serverAdress)) < 0)
        displayError(ERROR, "Binding to port failed", 31, clientIp);
    if (listen(socketFD, 5) < 0)
        displayError(ERROR, "Listening on port caused some errors", 31, clientIp);
    int clientLength = sizeof(clientAdress);
    short connectionCounts;
    // making the current working directiory only accesible directiory chroot jail
    char workingDirecitoryBuffer[256];
    getcwd(workingDirecitoryBuffer, 256);
    if (chroot(workingDirecitoryBuffer) < 0)
        displayError(ERROR,"Error happened while changing to root", 31, clientIp);
    while (1)
    {
        newClientSocketFD = accept(socketFD, (struct sockaddr *)&clientAdress, &clientLength);
        if (newClientSocketFD < 0)
            displayError(ERROR,"Error while connecting", 31, clientIp);

        printf("Connection accepted from %s:%d\n",
               inet_ntoa(clientAdress.sin_addr), ntohs(clientAdress.sin_port));

        int forkId = fork();
        if (forkId < 0)
            displayError(ERROR,"Error while forking", 31, clientIp);

        if (forkId == 0)
        {
            // Child process
            close(socketFD); /*This needs to be done to give other connections our same socket and its subsequent
            file requests
            */
            messageHandle(newClientSocketFD, clientAdress.sin_addr);
            close(newClientSocketFD);
            exit(0);
        }
        else
        {
            // Parent process
            close(newClientSocketFD);
        }
    }

    close(socketFD);
    return 0;
}