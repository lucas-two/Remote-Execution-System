// SERVER
#include <stdio.h>
#include <stdlib.h>
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
#include <sys/types.h>
#include<sys/utsname.h>
#include <dirent.h>
#endif

int main(int argc, char *argv[]) {

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
        char buffer[LARGE];
        char request[MAX];
        
        // Recieve which command should be executed
        recv(clientSocket, &request, sizeof(request), 0);
        
        if(strcmp(request, "put") == 0){
            char success[MAX] = "Successfully recieved file";
            FILE *filePtr;
            int wordNo;
            int wordCount;
            char fileName[MAX];

            recv(clientSocket, &fileName, sizeof(fileName), 0);

            filePtr = fopen(strcat(fileName), "w+");

            recv(clientSocket, &wordCount, sizeof(int), 0);

            while(wordNo != wordCount) {
                recv(clientSocket, buffer, sizeof(buffer), 0);
                fprintf(filePtr, "%s ", buffer);
                wordNo++;
            }

            send(clientSocket, success, sizeof(success), 0);
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