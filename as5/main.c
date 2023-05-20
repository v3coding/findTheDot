#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ftd.h"

int main(){
    threadController* threadArguments = (threadController*) malloc(sizeof(threadController));
    pthread_t* threadIDs = (pthread_t*) malloc(sizeof(pthread_t) * 10);
    threadArguments->threadIDs = threadIDs;
    startProgram(threadArguments);
    free(threadIDs);
    free(threadArguments);
    return 0;
}