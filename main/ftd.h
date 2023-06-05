#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

typedef struct threadController{
    //Hit on X
    _Atomic float X;
    //Hit on Y
    _Atomic float Y;
    //Hit on Z
    _Atomic float Z;
    //i2c file desc
    int i2cFileDesc;
    //boolean int to control startup and shutdown of threads
    int programRunning;
    //array of thread ID's
    pthread_t* threadIDs;
    // has a hit occured on x
    int hitX;
    // has a hit occured on y
    int hitY;
    // complete hit has occured
    int completeHit;
    //point X generated at startup
    float pointX;
    // point Y generated at startup
    float pointY;
    //tilt up/down
    int tiltUp;
    int tiltDown;
    //tilt left/right
    int tiltLeft;
    int tiltRight;
    int hits;
    int hitSound;
    int missSound;
    
} threadController;

void startProgram(threadController* threadArgument);

void waitForProgramEnd(threadController* threadArgument);

void* monitorAccelerometer(void* args);

void* printData(void* args);