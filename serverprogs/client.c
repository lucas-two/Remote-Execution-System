// CLIENT
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define PORT 80
#define MAX 1024
#define SMALL 10
#define LARGE 4096
#define LINES 40

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

            char character;
            char overwriteFlag[SMALL];
            char fileName[MAX];
            FILE *filePointer;
            FILE *countFilePointer; 

            // Get file name
            printf("Enter source file name: ");
            gets(fileName);
            send(clientSocket, fileName, sizeof(fileName), 0);

            // NOTE: Not yet implemented
            printf("Would you like to overwrite existing files [-f]? (yes/no): ");
            gets(overwriteFlag);
            // send(clientSocket, &localflag, sizeof(localflag), 0);

            // Count number of characters in file
            int charCount = 0;
            countFilePointer = fopen(fileName, "r");

            while((character = fgetc(countFilePointer)) != EOF) {
                charCount++;
            }
            fclose(countFilePointer);

            send(clientSocket, &charCount, sizeof(int), 0);

            filePointer = fopen(fileName, "r");
            while((character = fgetc(filePointer)) != EOF) {
                send(clientSocket, &character, sizeof(character), 0);
            }
            fclose(filePointer);

            recv(clientSocket, &response, sizeof(response), 0);
            printf("Server Respose: %s\n", response);
            
        }

        // --- GET ---
        else if(strcmp(request, "GET") == 0) {
            send(clientSocket, request, sizeof(request), 0);
            recv(clientSocket, &response, sizeof(response), 0); 
            printf("Server Response: %s\n", response);
        }

        // --- RUN [Working in some ways]---
        /* 
        We're able to execute and compile a file with command args.
        We get a display and can run through it in sets of 40 lines.
        Nevertheless, there is no checking if files already exist
        and hence shouldn't be copied over. Neither is there a way
        of storing this file locally with [-f local].
        */
        else if(strcmp(request, "run") == 0) {
            // Tell server what command to use
            send(clientSocket, request, sizeof(request), 0);

            char progName[MAX];
            char line[MAX];
            char args[MAX];
            int wordCount;
            char pause[SMALL];
            char localFlag[SMALL];

            // Actual sourcefile, not a dir holding programs
            printf("Enter program name: ");
            gets(progName);
            send(clientSocket, &progName, sizeof(progName), 0);

            printf("Enter program arguments (leave blank if none): ");
            gets(args);
            send(clientSocket, &args, sizeof(args), 0);

            // NOTE: Not yet implemented
            printf("Would you like to save locally [-f local]? (yes/no): ");
            gets(localFlag);
            // send(clientSocket, &localflag, sizeof(localflag), 0);

            recv(clientSocket, &wordCount, sizeof(int), 0);

            int counter = 1;
            while(counter != wordCount + 1) {
                recv(clientSocket, &line, sizeof(line), 0);
                printf("%s", line);
                
                if((counter % LINES) == 0) {
                    printf(" - - - press enter to continue - - -");
                    gets(pause);
                    send(clientSocket, pause, sizeof(pause), 0);
                }
                counter++;
            }

            recv(clientSocket, &response, sizeof(response), 0);
            printf("Server Response: %s\n",response);
        }


        // --- LIST [Working with tiny bug]---
        /*
        It appears when we use [-l], the stat info being
        recorded may be related to the source file rather 
        than the files within the source file. Attempts at fixing
        this were made but did not fix the issue.
        */
        else if(strcmp(request, "list") == 0) {
             // Tell server what command to use
            send(clientSocket, request, sizeof(request), 0);

            char dirName[MAX] = "serverprogs";
            char longFlag[SMALL];
            char progNameFlag[SMALL];
            char progName[MAX];
            int fileCount;
            char nameOfFile[MAX] = "";
            char creationOfFile[MAX] = "";
            char permissionOfFile[MAX] = "";
            int sizeOfFile;
            
            printf("Would you like to see long list [-l]? (yes/no): ");
            gets(longFlag);
            send(clientSocket, longFlag, sizeof(longFlag), 0);

            printf("Would you like to look at a [progname]? (yes/no): ");
            gets(progNameFlag);
            send(clientSocket, progNameFlag, sizeof(progNameFlag), 0);

            // [progname] provided
            if(strcmp(progNameFlag, "yes") == 0) {
                printf("Enter progname name: ");
                gets(progName);
                send(clientSocket, progName, sizeof(progName), 0);
            }

            recv(clientSocket, &fileCount, sizeof(fileCount), 0); 

            //[-l] provided
            int counter = 0;
            if(strcmp(longFlag, "yes") == 0) {
                while(counter != fileCount) {
                    recv(clientSocket, &nameOfFile, sizeof(nameOfFile), 0); 
                    recv(clientSocket, &sizeOfFile, sizeof(sizeOfFile), 0); 
                    recv(clientSocket, &creationOfFile, sizeof(creationOfFile), 0); 
                    recv(clientSocket, &permissionOfFile, sizeof(permissionOfFile), 0);
                    printf ("[-] %s\n", nameOfFile);
                    printf(" ~ File Size: %d bytes\n", sizeOfFile);
                    printf(" ~ Creation Date: %s", creationOfFile);
                    printf(" ~ Permissions: %s", permissionOfFile);
                    printf("\n\n");
                    counter++;
                }
            }
            // [-l] not provided
            else {
                 while(counter != fileCount) {
                    recv(clientSocket, &nameOfFile, sizeof(nameOfFile), 0); 
                    printf ("[-] %s\n", nameOfFile);
                    counter++;
                 }
            }

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