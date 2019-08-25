// CLIENT
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

    /* Create a socket*/
    int networkSocket;
    networkSocket = socket(AF_INET, SOCK_STREAM, 0);

    /* Specifying an address to connect to */
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    /* Connecting to the socket */
    int connectionStatus = connect(networkSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    //Checking if there was an error with connection
    if(connectionStatus == -1){
        printf("Error making connection");
    }

    /* Recieving data from server */
    char serverResponse[MAX];
    recv(networkSocket, &serverResponse, sizeof(serverResponse), 0);

    // Printing data we got back
    printf("Server sent: %s", serverResponse);

    /* Close the socket */
    close(networkSocket);

    return 0;
}