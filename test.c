// Tests
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include<sys/utsname.h>
#include <dirent.h>


int main(void){
    struct utsname detect;

    uname(&detect);

    printf("System name - %s \n", detect.sysname);
    printf("Nodename    - %s \n", detect.nodename);
    printf("Release     - %s \n", detect.release);
    printf("Version     - %s \n", detect.version);
    printf("Machine     - %s \n", detect.machine);
    printf("Domain name - %s \n", detect.__domainname);

    return 0;
}