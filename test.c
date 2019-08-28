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
#include <sys/sysinfo.h>



int main(void){

    int systemProcessors = 0;
    systemProcessors = get_nprocs_conf();

    printf("%d", systemProcessors);

    return 0;
}