// CLIENT
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define PORT 80
#define MAX 1024
#define LARGE 4096

#ifdef __WIN32__
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#endif

int main(int argc, char *argv[]) {

    char *serverIP;
    serverIP = argv[1];

    /* Create a socket*/
    int clientSocket;
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    /* Specifying an address to connect to */
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    inet_aton(serverIP, &serverAddress.sin_addr.s_addr);

    /* Connecting to the socket */
    int connectionStatus = connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    //Checking if there was an error with connection
    if(connectionStatus == -1){
        printf("Error making connection");
    }

    printf("\nConnected to %s on port %d...\n", serverIP, PORT);
    printf("Server Commands\n");
    printf("-----------------\n");
    printf("[-] put\n");
    printf("[-] get\n");
    printf("[-] run\n");
    printf("[-] list\n");
    printf("[-] sys\n");
    printf("[-] quit\n");
    printf("-----------------\n");

    while(1) {
        char request[LARGE];
        char response[LARGE];

        printf("Enter Command: ");
        gets(request);

        if(strcmp(request, "quit") == 0) {
            break;
        }

        send(clientSocket, request, sizeof(request), 0);
        recv(clientSocket, &response, sizeof(response), 0);
        printf("Server Response: %s", response);
    }
    /* Close the socket */
    close(clientSocket);
    return 0;
}
