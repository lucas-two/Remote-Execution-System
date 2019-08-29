// Tests
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#define MAX 100
#define SMALL 5

int main () {
    char progName[MAX];
    char dirName[MAX];
    char longFlag[SMALL];
    char progNameFlag[SMALL];
    struct dirent *pDirent;
    DIR *dir_ptr;

	printf("Enter a directory name: ");
    gets(dirName);

    printf("Would you like to see long list [-l]? (yes/no): ");
    gets(longFlag);

    printf("Would you like to look at a [progname]? (yes/no): ");
    gets(progNameFlag);

    // Set path to the program folder
    if(strcmp(progNameFlag, "yes") == 0) {
        printf("Enter progname name: ");
        gets(progName);
        strcat(dirName, "/");
        strcat(dirName, progName);
    }

    // Open directory
    dir_ptr = opendir(dirName);

    // Error if directory cannot be found
    if (dir_ptr == NULL) {
        printf ("Cannot open '%s' directory\n", dirName);
    }

    // (if [-l]) Print long list of each file
    if(strcmp(longFlag, "yes") == 0) {
        struct stat st;
        struct tm* clock; 

        // NOTE: There appears to be a slight bug in which info may
        // be relating to the actual directory these files are within.
        while ((pDirent = readdir(dir_ptr)) != NULL) {
            printf ("[-] %s\n", pDirent->d_name);
            stat(pDirent->d_name, &st);
            printf(" ~ File Size: %d bytes\n", st.st_size);
            printf(" ~ Creation Date: %s", ctime(&st.st_birthtime));
            printf(" ~ Permissions: ");
            printf((st.st_mode & S_IRUSR)? "r":"-");
            printf((st.st_mode & S_IWUSR)? "w":"-");
            printf((st.st_mode & S_IXUSR)? "x":"-");

            if(st.st_mode & S_IXUSR){
                printf("yay");
            }
            else{
                printf("nay");
            }
        }
    }

    // Otherwise just print all files in the directory
    else {
        while ((pDirent = readdir(dir_ptr)) != NULL) {
            printf ("[-] %s\n", pDirent->d_name);
        }
    }

    closedir (dir_ptr);
    return 0;
}