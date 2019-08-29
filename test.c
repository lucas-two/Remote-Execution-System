#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX 100
#define LINE 50

int main() {
    char fileName[MAX];
    char makeTxt[MAX] = "make ";
    char runTxt[MAX] = "./";
    char line[LINE];
    char args[MAX] = " boo";
    printf("Enter filename: ");
    gets(fileName);

    strcat(makeTxt, fileName);
    strcat(runTxt, fileName);
    strcat(runTxt, args);

    system(makeTxt);

    FILE* test_outputfile = popen(runTxt, "r");

    int wordCount = 0;
    while (fgets(line, sizeof(line), test_outputfile) != NULL) {
        wordCount++;
    }
    printf("%d words", wordCount);

    FILE* outputfile = popen(runTxt, "r");

    while (fgets(line, sizeof(line), outputfile) != NULL) {
        printf("%s", line);
    }

    pclose(outputfile);
}