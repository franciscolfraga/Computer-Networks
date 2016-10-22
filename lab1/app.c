#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <strings.h>
#include "app.h"
/* app layer, way how I access service*/

void startapp(char* file_name){
    if(appinfo.status == SENDER)
        send(file_name);
    else if(appinfo.status == RECEIVER)
        rcv();
}

void send(char* file_name){
    struct stat st;
    stat(file_name, &st);
    appinfo.file_size = st.st_size;
}
void rcv(){
    
}