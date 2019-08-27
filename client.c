// CLIENT
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define PORT 80
#define MAX 1024
#define LARGE 4096

#ifdef __WIN32__
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/types.h>
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
        char request[MAX];
        char response[MAX];
        time_t startTime;
        time_t endTime;
        int timeTaken;
        char progname[MAX];
        char buffer[LARGE];

        printf("Enter Command: ");
        gets(request);

        if(strcmp(request, "quit") == 0) {
            break;
        }

        send(clientSocket, request, sizeof(request), 0);


        if(strcmp(request, "put") == 0) {
            char fileName[MAX];
            FILE *filePtr;
            char charCount;
            char charWords;
            int wordCount = 0;

            printf("Enter file name: ");
            gets(fileName);
            startTime = clock();

            send(clientSocket, fileName, sizeof(fileName), 0);


            filePtr = fopen(fileName, "r");

            while(( charCount = getc(filePtr)) != EOF) {
                fscanf(filePtr, "%s", buffer);
                if(isspace(charCount) || charCount == '\t')
                wordCount++;
            }
            send(clientSocket, &wordCount, sizeof(int), 0);
            rewind(filePtr); // Go back to first char

            while(charWords != EOF) {
                fscanf(filePtr, "%s", buffer);
                send(clientSocket, buffer, sizeof(buffer), 0);
                charWords = fgetc(filePtr);
            }

            printf("File has been sent");
            recv(clientSocket, &response, sizeof(response), 0);
            endTime = clock();
            timeTaken = (endTime - startTime) / CLOCKS_PER_SEC;
            printf("Server Response (in %d secs): %s\n", timeTaken,response);
        }

    }
    /* Close the socket */
    close(clientSocket);
    return 0;
}
