#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
//for importing internet address structure
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "messageHandle.c"
#include <arpa/inet.h>


int main(int argc, char **argv) {
    int socketFD, newClientSocketFD;
    struct sockaddr_in serverAdress, clientAdress;
    if(argc < 2) {
        printf("Man please provide a host to init\n");
        exit(1);
    }
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if(socketFD < 0 || socketFD == 2) displayError("Socket init failed");
    
    serverAdress.sin_family = AF_INET; //uses tcp instead of udp
    serverAdress.sin_port = htons(atoi(argv[1])); //necessary to convert host byte order to network
    serverAdress.sin_addr.s_addr = INADDR_ANY; //for accepting all connections
    printf("WebSocketServer listening on port %d...\n", atoi(argv[1]));

    //for making port resusable even though it may be busy (generally for test purposes)
    int reuse = 1;
	if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) displayError("Making port reusable even though it is busy");

    if(bind(socketFD, (struct sockaddr *) &serverAdress, sizeof(serverAdress)) < 0) displayError("Binding to port failed");
    listen(socketFD, 5);
    int clientLength = sizeof(clientAdress);
    short forkId;
    short connectionCounts;
    while(1) {
        newClientSocketFD = accept(socketFD, (struct sockaddr *) &clientAdress, &clientLength);
        if(newClientSocketFD < 0) displayError("Error while connecting");
        connectionCounts++;
        printf("Connection accepted from %s:%d with number of: %d\n",inet_ntoa(clientAdress.sin_addr), ntohs(clientAdress.sin_port), connectionCounts);
        if(forkId < 0 ) displayError("Error while forking");
        if((forkId == fork()) == 0) {
            messageHandle(newClientSocketFD);
        }
    }
    close(newClientSocketFD);
return 0;
}