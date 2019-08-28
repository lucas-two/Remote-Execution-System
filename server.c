// SERVER
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PORT 80
#define MAX 1024
#define SMALL 10
#define LARGE 4096

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include<sys/utsname.h>
#include <dirent.h>
#include <sys/sysinfo.h>

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
        char timeTakenChar[MAX];
        clock_t t;
        float timeTaken;

        // Recieve which command should be executed
        recv(clientSocket, &request, sizeof(request), 0);
        
        // PUT (Recieving file)
        if(strcmp(request, "put") == 0){
            char success[MAX] = "Successfully saved file (in ";
            FILE *serverFilePtr;
            int wordNo;
            int wordCount;
            char overwriteFlag[SMALL];
            char fileName[MAX];

            // Recieve name of file
            recv(clientSocket, &fileName, sizeof(fileName), 0);

            // Recieve wheather overwriting or not
            recv(clientSocket, &overwriteFlag, sizeof(fileName), 0);

            t = clock(); // Start runtime timer

            // NOTE: Should change 'savedFile' here to reflect the fileName.
            // However, this was causing a 'stack dump' and so SavedFile is used
            // as a placeholder.
            if(strcmp(overwriteFlag, "yes") == 0){
                serverFilePtr = fopen("SavedFile", "w+");
            }
            else {
                // NOT YET IMPLEMENTED
                // [Check if file exists]
                serverFilePtr = fopen("SavedFile", "w+");
                // IF does exist -> Error MSG
                }
            
            // Get number of words 
            recv(clientSocket, &wordCount, sizeof(int), 0);

            // Print each word recieved from the user to the new file.
            while(wordNo != wordCount) {
                recv(clientSocket, &buffer, sizeof(buffer), 0);
                printf("%s", buffer);
                fprintf(serverFilePtr, "%s ", buffer);
                wordNo++;
            }
             // End runtime timer & calculate time taken
            t = clock() - t;
            timeTaken = ( (double) t ) / CLOCKS_PER_SEC;

            // Format success message with timer result
            gcvt(timeTaken, 8, timeTakenChar); 
            strcat(success, timeTakenChar);
            strcat(success, " secs)");

            // Send success message to user
            send(clientSocket, success, sizeof(success), 0);
        }

        else if(strcmp(request, "get") == 0){
            char response[MAX] = "GET";
            send(clientSocket, response, sizeof(response), 0);
        }

        else if(strcmp(request, "run") == 0){
            char response[MAX] = "RUN";
            send(clientSocket, response, sizeof(response), 0);
        }

        else if(strcmp(request, "list") == 0){
            char response[MAX] = "LIST";
            send(clientSocket, response, sizeof(response), 0);
        }

        else if(strcmp(request, "sys") == 0){
            char success[MAX] = "Info found (in ";
            t = clock(); // Start runtime timer

            struct utsname systemInfo;
            uname(&systemInfo);

            char systemName[MAX] = "";
            char systemVersion[MAX] = "";
            int systemProcessors = 0;
            systemProcessors = get_nprocs_conf();

            strcpy(systemName, systemInfo.sysname);
            strcpy(systemVersion, systemInfo.version);

            // End runtime timer & calculate time taken
            t = clock() - t;
            timeTaken = ( (double) t ) / CLOCKS_PER_SEC;

            // Format success message with timer result
            gcvt(timeTaken, 8, timeTakenChar); 
            strcat(success, timeTakenChar);
            strcat(success, " secs)");

            send(clientSocket, systemName, sizeof(systemName), 0);
            send(clientSocket, systemVersion, sizeof(systemVersion), 0);
            send(clientSocket, &systemProcessors, sizeof(int), 0);
            send(clientSocket, success, sizeof(success), 0);
        }

        else{
            // Do nothing
        }
    }
    /* Close the socket */
    close(serverSocket);

    return 0;
}