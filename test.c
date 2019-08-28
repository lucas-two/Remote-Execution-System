// Tests
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#define MAX 100

int main () {
    int li = 1;
    char progName[MAX];
    char dirName[MAX];


	printf("Enter a directory name: ");
    gets(dirName);

    struct dirent *pDirent;
    DIR *dir_ptr; // Pointer to the directory


    // If flags were set
    if(li == 1) {
        printf("Enter progname name: ");
        gets(progName);
        strcat(dirName, "/");
        strcat(dirName, progName);
    }

    dir_ptr = opendir(dirName); // Open input directory

    if (dir_ptr == NULL) {
        printf ("Cannot open '%s' directory\n", dirName);
    }

    if(li == 1) {
        struct stat st;
        struct tm* clock; 

        while ((pDirent = readdir(dir_ptr)) != NULL) {
            printf ("[-] %s\n", pDirent->d_name);
            stat(pDirent->d_name, &st);
            printf("\n -- File Size: %lld bytes\n", st.st_size);
            printf(" -- Creation Date: %s", ctime(&st.st_birthtime));
            printf(" -- Permissions: ");
            printf((st.st_mode & S_IRUSR)? "r":"-");
            printf((st.st_mode & S_IWUSR)? "w":"-");
            printf((st.st_mode & S_IXUSR)? "x":"-");
            printf("\n\n");
        } 
    }
    closedir (dir_ptr);
    return 0;
}