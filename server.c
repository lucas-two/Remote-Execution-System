// SERVER
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define PORT 9002
#define MAX 1024

#ifdef __WIN32__
# include <winsock2.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h> 
#endif

int main() {

    char serverMsg[MAX] = "Hello, this is a server msg";

    /* Create the server socket */
    int serverSocket;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    /* Specifying an address */
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    /* Binding socket to our specific IP and Port */
    bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    /* Listen for connections */
    listen(serverSocket, 5);

    /* Accepting connection to client */
    int clientSocket;
    clientSocket = accept(serverSocket, NULL, NULL); // These are null since only looking at local machine

    /* Sending message to client */
    send(clientSocket, serverMsg, sizeof(serverMsg), 0);

    /* Close the socket */
    close(serverSocket);
}