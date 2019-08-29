// SERVER
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PORT 80
#define MAX 1024
#define SMALL 10
#define LARGE 4096
#define LINES 40

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
#include <sys/stat.h>
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
        char timeTakenChar[MAX];
        clock_t t;
        float timeTaken;

        // Recieve which command should be executed
        recv(clientSocket, &request, sizeof(request), 0);
        
        // PUT (Recieving file)
        if(strcmp(request, "put") == 0){
            char character;
            char fileName[MAX];
            char filePath[MAX] = "serverprogs/";
            int charCount;
            FILE *serverFilePointer;

            // Get file name & No. of characters in file.
            recv(clientSocket, &fileName, sizeof(fileName), 0);
            recv(clientSocket, &charCount, sizeof(charCount), 0);

            t = clock(); // Start runtime timer

            // Add file name to the file path (so it will store in correct dir)
            strcat(filePath, fileName);
            serverFilePointer = fopen(filePath, "w+");

            // Get each character from client and parse them into new file
            int counter = 0;
            while(counter < charCount) {
                recv(clientSocket, &character, sizeof(character), 0);
                fputc(character, serverFilePointer);
                counter++;
            }
            fclose(serverFilePointer);

            // End runtime timer & calculate time taken
            t = clock() - t;
            timeTaken = ( (double) t ) / CLOCKS_PER_SEC;

            // Format success message with timer result
            char success[MAX] = "Successfully saved (in ";

            gcvt(timeTaken, 8, timeTakenChar);
            strcat(success, timeTakenChar);
            strcat(success, " secs)");

            send(clientSocket, success, sizeof(success), 0);
        }

        else if(strcmp(request, "get") == 0){

        }

        // RUN (Execute a file)
        else if(strcmp(request, "run") == 0){
            char fileName[MAX] = "serverprogs/";
            char progName[MAX];
            char makeTxt[MAX] = "make ";
            char runTxt[MAX] = "./";
            char line[MAX];
            char args[MAX];
            int wordCount = 0;
            char pause[SMALL]; // Temp variable for the 40 page per line limit.

            FILE* exeFile;
            FILE* exeWordCount;

            recv(clientSocket, &progName, sizeof(progName), 0);
            recv(clientSocket, &args, sizeof(args), 0);

            // Format strings to be run
            strcat(fileName, progName);
            strcat(makeTxt, fileName);
            strcat(runTxt, fileName);
            strcat(runTxt, " ");
            strcat(runTxt, args);

            system(makeTxt); // Compile file
            
            // Get number of lines in the file (that's whats meant by word count)
            exeWordCount = popen(runTxt, "r");
            while (fgets(line, sizeof(line), exeWordCount) != NULL) {
                wordCount++;
            }
            pclose(exeWordCount);

            send(clientSocket, &wordCount, sizeof(int), 0);

            // Execute the file and read its contents.
            exeFile = popen(runTxt, "r");
            int lineNumber = 1;
            while (fgets(line, sizeof(line), exeFile) != NULL) {
                send(clientSocket, line, sizeof(line), 0);
                // If we get to line 40 -> wait for user input
                if((lineNumber % LINES) == 0){
                    recv(clientSocket, &pause, sizeof(pause), 0);
                }
                lineNumber++;
            }
            pclose(exeFile);
            
            char success[MAX] = "Successfully executed program";
            send(clientSocket, success, sizeof(success), 0);
        }

        // LIST (Listing out programs in serverprogs)
        else if(strcmp(request, "list") == 0){

            char dirName[MAX] = "serverprogs";
            char progName[MAX];
            char longFlag[SMALL];
            char progNameFlag[SMALL];
            struct dirent *pDirent;
            DIR *dirPointer;

            // Get optional flags
            recv(clientSocket, &longFlag, sizeof(longFlag), 0);
            recv(clientSocket, &progNameFlag, sizeof(longFlag), 0);

            // [progname] provided - Update directory path
            if(strcmp(progNameFlag, "yes") == 0) {
                recv(clientSocket, &progName, sizeof(progName), 0);
                strcat(dirName, "/");
                strcat(dirName, progName);
            }

            t = clock(); // Start runtime timer

            // Open directory
            dirPointer = opendir(dirName);

            // Error message if failed.
            if (dirPointer == NULL) {
                printf ("Cannot open '%s' directory\n", dirName);
                exit(1);
            }

            // Getting amount of files within directory
            /* Since rewind() was not working on the dirPointer
            a new countPointer was used to get the number of files */ 
            DIR *dirPointerFileCount;
            int fileCount = 0;
            dirPointerFileCount = opendir(dirName);
            while ((pDirent = readdir(dirPointerFileCount)) != NULL) {
                fileCount++;
            }

            // Send number of files to client
            send(clientSocket, &fileCount, sizeof(int), 0);
        
            // [-l] provided - list out more details about each file
            if(strcmp(longFlag, "yes") == 0) {
                while ((pDirent = readdir(dirPointer)) != NULL) {
                    struct stat st;
                    struct tm* clock;
                    char nameOfFile[MAX] = "";
                    char creationOfFile[MAX] = "";
                    char permissionOfFile[MAX] = "";
                    int sizeOfFile;

                    stat(pDirent->d_name, &st);

                    // Get name of file
                    strcpy(nameOfFile, pDirent->d_name);

                    // Get creation date of file
                    strcpy(creationOfFile, ctime(&st.st_birthtime));

                    // Get permissions
                    if(st.st_mode & S_IRUSR) {
                        strcat(permissionOfFile, "r");
                    } else {
                        strcat(permissionOfFile, "-");
                    }

                    if(st.st_mode & S_IWUSR) {
                        strcat(permissionOfFile, "w");
                    } else {
                        strcat(permissionOfFile, "-");
                    }

                    if(st.st_mode & S_IXUSR) {
                        strcat(permissionOfFile, "x");
                    } else {
                        strcat(permissionOfFile, "-");
                    }

                    // Send info the client
                    send(clientSocket, nameOfFile, sizeof(nameOfFile), 0);
                    send(clientSocket, &sizeOfFile, sizeof(int), 0);
                    send(clientSocket, creationOfFile, sizeof(creationOfFile), 0);
                    send(clientSocket, permissionOfFile, sizeof(permissionOfFile), 0);
                }
            }

            // Otherwise just list out all file names
            else {
                while ((pDirent = readdir(dirPointer)) != NULL) {
                    char nameOfFile[MAX] = "";

                    // Get file name
                    strcpy(nameOfFile, pDirent->d_name);

                    // Send info to the client
                    send(clientSocket, nameOfFile, sizeof(nameOfFile), 0);
                }
            }

            // End runtime timer & calculate time taken
            t = clock() - t;
            timeTaken = ( (double) t ) / CLOCKS_PER_SEC;

            // Format success message with timer result
            char success[MAX] = "Successfully found (in ";

            gcvt(timeTaken, 8, timeTakenChar);
            strcat(success, timeTakenChar);
            strcat(success, " secs)");
            
            send(clientSocket, success, sizeof(success), 0);
            closedir (dirPointer);
        }

        // SYS (Displaying system information)
        else if(strcmp(request, "sys") == 0){
            t = clock(); // Start runtime timer

            // Creating a systemInfo variable to read the system info from
            struct utsname systemInfo;
            uname(&systemInfo);

            char systemName[MAX] = "";
            char systemVersion[MAX] = "";
            int systemProcessors = 0;
            systemProcessors = get_nprocs_conf(); // Calls function which returns no. of processors

            // Copies the system info details into strings
            strcpy(systemName, systemInfo.sysname);
            strcpy(systemVersion, systemInfo.version);

            // End runtime timer & calculate time taken
            t = clock() - t;
            timeTaken = ( (double) t ) / CLOCKS_PER_SEC;

            // Format success message with timer result
            char success[MAX] = "Found info (in ";
            gcvt(timeTaken, 8, timeTakenChar); 
            strcat(success, timeTakenChar);
            strcat(success, " secs)");

            // Send data to client
            send(clientSocket, success, sizeof(success), 0);
            send(clientSocket, systemName, sizeof(systemName), 0);
            send(clientSocket, systemVersion, sizeof(systemVersion), 0);
            send(clientSocket, &systemProcessors, sizeof(int), 0);

        } else {
            // Do nothing
            // [Added to fix a bug]
        }
    }
    /* Close the socket */
    close(serverSocket);

    return 0;
}