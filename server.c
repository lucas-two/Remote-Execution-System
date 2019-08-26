// SERVER
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#define PORT 80
#define MAX 1024
#define LARGE 4096


#ifdef __WIN32__
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h> 
#endif

int main() {

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


    while(1) {
        char request[MAX];

        recv(clientSocket, &request, sizeof(request), 0);
        
        if(strcmp(request, "put") == 0){
            char response[MAX] = "PUT";
            send(clientSocket, response, sizeof(response), 0);
        }

        else if(strcmp(request, "get") == 0){
            char response[MAX] = "GET";
            send(clientSocket, response, sizeof(response), 0);
        }

        else {
            char response[MAX] = "?";
            send(clientSocket, response, sizeof(response), 0);
        }
    }
    /* Close the socket */
    close(serverSocket);

    return 0;
}