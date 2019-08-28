// CLIENT
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define PORT 80
#define MAX 1024
#define SMALL 10
#define LARGE 4096

#ifdef WIN32
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
        // Main variables.
        char request[MAX]; // What we ask the server (e.g. put or sys)
        char response[MAX]; // What we get back from server (e.g. time taken msg)
        char progname[MAX]; // Name of program file (Not used a lot)
        char buffer[LARGE]; // Storing information in a buffer

        printf("Enter Command: ");
        gets(request);

        // Exit if quit is called.
        if(strcmp(request, "quit") == 0) {
            break;
        }

        // --- PUT [Attempted Implementation] ---
        /*
        There appears to be a bug in this code in which
        the server is not printing the contents to a file.

        In addition, writing to a file of the same name as
        fileName caused a 'stack dump' error on the server end.
        As such, a placeholder 'savedFile' was used.

        This implementation is missing an actual checker for already
        created files and is unable to process multiple files at once in
        and send the entire directory of a program.
         */
        if(strcmp(request, "put") == 0) {
            // Tell server what command to use
            send(clientSocket, request, sizeof(request), 0);

            char fileName[MAX];
            FILE *filePtr;
            char overwriteFlag[SMALL];
            char charCount;
            char charWords;
            int wordCount = 0;

            // Get file name
            printf("Enter file name: ");
            gets(fileName);

            // Send the file name to the server
            send(clientSocket, fileName, sizeof(fileName), 0);

            // Get optional [-f] flag
            printf("Would you like to overwrite? [-f] (yes/no):");
            gets(overwriteFlag);

            // Send the [-f] flag to the server
            send(clientSocket, overwriteFlag, sizeof(char), 0);
            
            // Open the file
            filePtr = fopen(fileName, "r");

            // Find how many words are in the file
            while(( charCount = getc(filePtr)) != EOF) {
                fscanf(filePtr, "%s", buffer);
                if(isspace(charCount) || charCount == '\t')
                wordCount++;
            }

            // Send the amount of words to the server
            send(clientSocket, &wordCount, sizeof(int), 0);

            // Go back to first char
            rewind(filePtr);

            // Read each character in and send it to the server
            while(charWords != EOF) {
                fscanf(filePtr, "%s", buffer);
                send(clientSocket, buffer, sizeof(buffer), 0);
                charWords = fgetc(filePtr);
            }
            
            printf("File has been sent\n");
            recv(clientSocket, &response, sizeof(response), 0);
            printf("Server Response: %s\n",response);
        }

        // --- GET ---
        else if(strcmp(request, "get") == 0) {
            send(clientSocket, request, sizeof(request), 0);
            recv(clientSocket, &response, sizeof(response), 0);
            printf("Server Response: %s\n",response);
        }

        // --- RUN ---
        else if(strcmp(request, "run") == 0) {
            send(clientSocket, request, sizeof(request), 0);
            recv(clientSocket, &response, sizeof(response), 0); 
            printf("Server Response: %s\n",response);

        }
        // --- LIST ---
        else if(strcmp(request, "list") == 0) {
            send(clientSocket, request, sizeof(request), 0);
            recv(clientSocket, &response, sizeof(response), 0); 
            printf("Server Response: %s\n",response);
        }

        // --- SYS [Working] ---
        else if(strcmp(request, "sys") == 0) {
            // Tell server what command to use
            send(clientSocket, request, sizeof(request), 0);

            char systemName[MAX];
            char systemVersion[MAX];
            int systemProcessors;
            
            // Recieving data from server
            recv(clientSocket, &response, sizeof(response), 0);
            recv(clientSocket, &systemName, sizeof(systemName), 0); 
            recv(clientSocket, &systemVersion, sizeof(systemVersion), 0); 
            recv(clientSocket, &systemProcessors, sizeof(systemVersion), 0); 
            
            // Displaying data
            printf("Server Response: %s \n\n", response);
            printf(" [-] System Name: %s\n", systemName);
            printf(" [-] System Version: %s\n", systemVersion);
            printf(" [-] Number of prosessors: %d\n\n", systemProcessors);
        }

        // INVALID
        else{
            printf("Invalid Command");
        }

    }
    /* Close the socket */
    close(clientSocket);
    return 0;
}