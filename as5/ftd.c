#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include "ftd.h"
#include <stdatomic.h>
#include <linux/i2c.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <sys/mman.h>
#include "sharedDataStruct.h"

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"
#define I2C_DEVICE_ADDRESS 0x18
#define I2C_DEVICE2_ADDRESS 0x20

#define REG_TURN_ON_ACCEL 0x20
#define READADDR 0xA8
#define AxL 0x28
#define AxH 0x29
#define AyL 0x2A
#define AyH 0x2B
#define AzL 0x2C
#define AzH 0x2D
#define REG_DIRA 0x00 // Zen Red uses: 0x02
#define REG_DIRB 0x01 // Zen Red uses: 0x03
#define REG_OUTA 0x14 // Zen Red uses: 0x00
#define REG_OUTB 0x15 // Zen Red uses: 0x01

// General PRU Memomry Sharing Routine
// ----------------------------------------------------------------
#define PRU_ADDR 0x4A300000 // Start of PRU memory Page 184 am335x TRM
#define PRU_LEN 0x80000 // Length of PRU memory
#define PRU0_DRAM 0x00000 // Offset to DRAM
#define PRU1_DRAM 0x02000
#define PRU_SHAREDMEM 0x10000 // Offset to shared memory
#define PRU_MEM_RESERVED 0x200 // Amount used by stack and heap

// Convert base address to each memory section
#define PRU0_MEM_FROM_BASE(base) ( (base) + PRU0_DRAM + PRU_MEM_RESERVED)
#define PRU1_MEM_FROM_BASE(base) ( (base) + PRU1_DRAM + PRU_MEM_RESERVED)
#define PRUSHARED_MEM_FROM_BASE(base) ( (base) + PRU_SHAREDMEM)

volatile void* getPruMmapAddr(void) {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("ERROR: could not open /dev/mem");
        exit(EXIT_FAILURE);
    }
// Points to start of PRU memory.
    volatile void* pPruBase = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU_ADDR);
    if (pPruBase == MAP_FAILED) {
        perror("ERROR: could not map memory");
        exit(EXIT_FAILURE);
    }
    close(fd);
    return pPruBase;
}

void freePruMmapAddr(volatile void* pPruBase){
    if (munmap((void*) pPruBase, PRU_LEN)) {
    perror("PRU munmap failed");
    exit(EXIT_FAILURE);
    }
}

void sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}


void* playSounds(void* args){
    threadController* threadData = (threadController*) args;
    FILE *pFile;
    while(threadData->programRunning){
        if(threadData->hitSound){

            pFile = fopen("/dev/bone/pwm/0/a/period", "w");
	        fprintf(pFile, "%d", 1000000);
	        fclose(pFile);
            pFile = fopen("/dev/bone/pwm/0/a/duty_cycle", "w");
            fprintf(pFile, "%d", 500000);
            fclose(pFile);
            pFile = fopen("/dev/bone/pwm/0/a/enable", "w");
            fprintf(pFile, "%d", 1);
            fclose(pFile);
            sleepForMs(200);
            pFile = fopen("/dev/bone/pwm/0/a/enable", "w");
            fprintf(pFile, "%d", 0);
            fclose(pFile);
            pFile = fopen("/dev/bone/pwm/0/a/period", "w");
	        fprintf(pFile, "%d", 750000);
	        fclose(pFile);
            pFile = fopen("/dev/bone/pwm/0/a/duty_cycle", "w");
            fprintf(pFile, "%d", 500000);
            fclose(pFile);
            pFile = fopen("/dev/bone/pwm/0/a/enable", "w");
            fprintf(pFile, "%d", 1);
            fclose(pFile);
            sleepForMs(200);
            pFile = fopen("/dev/bone/pwm/0/a/enable", "w");
            fprintf(pFile, "%d", 0);
            fclose(pFile);
            pFile = fopen("/dev/bone/pwm/0/a/period", "w");
	        fprintf(pFile, "%d", 550000);
	        fclose(pFile);
            pFile = fopen("/dev/bone/pwm/0/a/duty_cycle", "w");
            fprintf(pFile, "%d", 500000);
            fclose(pFile);
            pFile = fopen("/dev/bone/pwm/0/a/enable", "w");
            fprintf(pFile, "%d", 1);
            fclose(pFile);
            sleepForMs(100);
            pFile = fopen("/dev/bone/pwm/0/a/enable", "w");
            fprintf(pFile, "%d", 0);
            fclose(pFile);

            threadData->hitSound = 0;
        }
        if(threadData->missSound){

            pFile = fopen("/dev/bone/pwm/0/a/period", "w");
	        fprintf(pFile, "%d", 1000000);
	        fclose(pFile);
            pFile = fopen("/dev/bone/pwm/0/a/duty_cycle", "w");
            fprintf(pFile, "%d", 500000);
            fclose(pFile);
            pFile = fopen("/dev/bone/pwm/0/a/enable", "w");
            fprintf(pFile, "%d", 1);
            fclose(pFile);
            sleepForMs(200);
            pFile = fopen("/dev/bone/pwm/0/a/enable", "w");
            fprintf(pFile, "%d", 0);
            fclose(pFile);
            pFile = fopen("/dev/bone/pwm/0/a/period", "w");
	        fprintf(pFile, "%d", 1500000);
	        fclose(pFile);
            pFile = fopen("/dev/bone/pwm/0/a/duty_cycle", "w");
            fprintf(pFile, "%d", 500000);
            fclose(pFile);
            pFile = fopen("/dev/bone/pwm/0/a/enable", "w");
            fprintf(pFile, "%d", 1);
            fclose(pFile);
            sleepForMs(200);
            pFile = fopen("/dev/bone/pwm/0/a/enable", "w");
            fprintf(pFile, "%d", 0);
            fclose(pFile);

            threadData->missSound = 0;
        }
        sleepForMs(50);
    }
    pthread_exit(0);
}

void initGPIO(void) {
	//Export both pins
	FILE *pFile = fopen("/sys/class/gpio/export", "w");
	if(pFile == NULL) {
		printf("ERROR: Unable to open export file.\n");
		exit(1);
	}

	fprintf(pFile, "%d", 61);
	fprintf(pFile, "%d", 44);
	fclose(pFile);
	
	//Set direction of both pins
	pFile = fopen("/sys/class/gpio/gpio61/direction", "w");
	if(pFile == NULL) {
		printf("ERROR: Unable to open gpio61/direction\n");
		exit(1);
	}
	fprintf(pFile, "%s", "out");
	fclose(pFile);
	pFile = fopen("/sys/class/gpio/gpio44/direction", "w");
	if(pFile == NULL) {
		printf("ERROR: Unable to open gpio44/direction\n");
		exit(1);
	}
	fprintf(pFile, "%s", "out");
	fclose(pFile);

	//Turn on both digits
	pFile = fopen("/sys/class/gpio/gpio61/value", "w");
	if(pFile == NULL) {
		printf("ERROR: Unable to open gpio61/value\n");
		exit(1);
	}
	fprintf(pFile, "%d", 1);
	fclose(pFile);
	pFile = fopen("/sys/class/gpio/gpio44/value", "w");
	if(pFile == NULL) {
		printf("ERROR: Unable to open gpio44/value\n");
		exit(1);
	}
	fprintf(pFile, "%d", 1);
	fclose(pFile);


}

static int initI2cBus(char* bus, int address){
    int i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}

static void writeI2cReg(int i2cFileDesc, unsigned char regAddr,unsigned char value){
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    if (res != 2) {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

void runCommand(char* command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
    if (fgets(buffer, sizeof(buffer), pipe) == NULL)
    break;
    // printf("--> %s", buffer); // Uncomment for debugging
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

static unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr){
// To read a register, must first write the address
    int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
    if (res != sizeof(regAddr)) {
        perror("I2C: Unable to write to i2c register.");
        exit(1);
    }
// Now read the value and return it
    char value = 0;
    res = read(i2cFileDesc, &value, sizeof(value));
    if (res != sizeof(value)) {
        perror("I2C: Unable to read from i2c register");
        exit(1);
    }
    return value;
}

void configureInput(){
    FILE *pFile;
    pFile = fopen("/sys/class/gpio/gpio26/direction", "w");
    fprintf(pFile,"in");
    pFile = fopen("/sys/class/gpio/gpio46/direction", "w");
    fprintf(pFile,"in");
    pFile = fopen("/sys/class/gpio/gpio65/direction", "w");
    fprintf(pFile,"in");
    pFile = fopen("/sys/class/gpio/gpio47/direction", "w");
    fprintf(pFile,"in");
    pFile = fopen("/sys/class/gpio/gpio27/direction", "w");
    fprintf(pFile,"in");
}

int readJoystick(int joystick){
    //1 is up, 2 is down
    FILE *pFile;

    if(joystick == 1){
        pFile = fopen("/sys/class/gpio/gpio26/value", "r");
    }
    if(joystick == 2){
        pFile = fopen("/sys/class/gpio/gpio46/value", "r");
    }
    if(joystick == 3){
        pFile = fopen("/sys/class/gpio/gpio65/value", "r");
    }
    if(joystick == 4){
        pFile = fopen("/sys/class/gpio/gpio47/value", "r");
    }
    if(joystick == 5){
        pFile = fopen("/sys/class/gpio/gpio27/value", "r");
    }
    char buff[1024];
    fgets(buff, 1024, pFile);
    //printf("Read : '%s'",buff);
    //printf("First Bit = %c\n",buff[0]);
    fclose(pFile);
    if(buff[0] == '0'){
        return 1;
    }
    return 0;
}

void* printData(void* args){
    threadController* threadData = (threadController*) args;
    volatile void *pPruBase = getPruMmapAddr();
    volatile sharedMemStruct_t *pSharedPru0 = PRU0_MEM_FROM_BASE(pPruBase);

    while(threadData->programRunning){
        printf("X%f Y%f |TOTAL SCORE %d| HIT:%d HITX:%d HITY:%d | TiltLeft: %d TiltRight: %d | TiltUp: %d TiltDown: %d |  PointX: %f PointY: %f\n",threadData->X,threadData->Y,threadData->hits, threadData->completeHit,threadData->hitX,threadData->hitY,threadData->tiltLeft,threadData->tiltRight,threadData->tiltUp,threadData->tiltDown,threadData->pointX,threadData->pointY);
        sleepForMs(500);
        if(!pSharedPru0->isButtonPressed){
            printf("Joystick Was Pressed!\n");
        }
    }
    freePruMmapAddr(pPruBase);
    pthread_exit(0);
}

void* monitorAccelerometerX(void* args){
    volatile void *pPruBase = getPruMmapAddr();
    volatile sharedMemStruct_t *pSharedPru0 = PRU0_MEM_FROM_BASE(pPruBase);
    threadController* threadData = (threadController*) args;
    float xPoint = threadData->pointX;
    threadData->tiltLeft = 0;
    threadData->tiltRight = 0;

    unsigned char buffx[9];
    for(int i = 0; i < 9; i++){
        buffx[i] = 0;
    }

    //left x is negative -1, right x is positive 1
    while(threadData->programRunning){
        
       *buffx = readI2cReg(threadData->i2cFileDesc,0x28);
       *(buffx + 4) = readI2cReg(threadData->i2cFileDesc,0x29);
        int16_t x = (buffx[4] << 8) | (buffx[0]);

        float intermediateX = (float) x;
        float xscaled = (float) (intermediateX / 16000);
        if(xscaled > 1){
            xscaled = 1;
        }
        if(xscaled < -1){
            xscaled = -1;
        }

        float xDiff = fabs(xscaled - xPoint);
        if(xDiff <= 0.1f){
            threadData->hitX = 1;
        }
        else{
            threadData->hitX = 0;
        }
        if(threadData->hitX && threadData->hitY){
            threadData->completeHit = 1;
        }
        else{
            threadData->completeHit = 0;
        }

        if(xscaled < xPoint){
            threadData->tiltLeft = 0;
            threadData->tiltRight = 1;
            pSharedPru0->left = 0;
            pSharedPru0->right = 1;
        }else{
            threadData->tiltRight = 0;
            threadData->tiltLeft = 1;
            pSharedPru0->left = 1;
            pSharedPru0->right = 0;
        }

        atomic_store(&threadData->X,xscaled);
        sleepForMs(100);
    }
    pthread_exit(0);
}

//up is -1 down is 1
void* monitorAccelerometerY(void* args){
    volatile void *pPruBase = getPruMmapAddr();
    volatile sharedMemStruct_t *pSharedPru0 = PRU0_MEM_FROM_BASE(pPruBase);
    threadController* threadData = (threadController*) args;
    float yPoint = threadData->pointY;
    
    threadData->tiltUp = 0;
    threadData->tiltDown = 0;

    unsigned char buffy[9];
        for(int i = 0; i < 9; i++){
        buffy[i] = 0;
    }

    while(threadData->programRunning){
       *buffy = readI2cReg(threadData->i2cFileDesc,0x2A);
       *(buffy + 4) = readI2cReg(threadData->i2cFileDesc,0x2B);
        int16_t y = (buffy[4] << 8) | (buffy[0]);

        float intermediateY = (float) y;
        float yscaled = (float) (intermediateY / 16000);
        if(yscaled > 1){
            yscaled = 1;
        }
        if(yscaled < -1){
            yscaled = -1;
        }

        float yDiff = fabs(yscaled - yPoint);
        if(yDiff <= 0.1f){
            threadData->hitY = 1;
        }
        else{
            threadData->hitY = 0;
        }
        if(threadData->hitX && threadData->hitY){
            threadData->completeHit = 1;
        }
        else{
            threadData->completeHit = 0;
        }
        
        if(threadData->completeHit == 0){

        if(yscaled < yPoint){
            threadData->tiltUp = 0;
            threadData->tiltDown = 1;
            pSharedPru0->height = 0;

            if(yDiff < 0.25f){
                pSharedPru0->height = 3;
            }
            if((yDiff < 0.50f) && (yDiff > 0.25f)){
                pSharedPru0->height = 2;
            }
            if((yDiff < 0.75f) && (yDiff > 0.50f)){
                pSharedPru0->height = 1;
            }
            if(yDiff > 0.75f){
                pSharedPru0->height = 0;
            }
        }else{
            threadData->tiltDown = 0;
            threadData->tiltUp = 1;

            if(yDiff < 0.25f){
                pSharedPru0->height = 4;
            }
            if((yDiff < 0.50f) && (yDiff > 0.25f)){
                pSharedPru0->height = 5;
            }
            if((yDiff < 0.75f) && (yDiff > 0.50f)){
                pSharedPru0->height = 6;
            }
            if(yDiff > 0.75f){
                pSharedPru0->height = 7;
            }
        }

        }
        else{
            pSharedPru0->height = 8;
            if(!pSharedPru0->isButtonPressed){
                printf("SCORE!!\n");
                threadData->hits++;
                threadData->hitSound = 1;
            }
        }
        if(!pSharedPru0->isButtonPressed){
            if(threadData->completeHit == 0){
                threadData->missSound = 1;
            }
        }

        atomic_store(&threadData->Y,yscaled);
        //threadData->programRunning = pSharedPru0->quit;
        if(!pSharedPru0->quit){
            threadData->programRunning = 0;
        }
        sleepForMs(100);
    }
    pthread_exit(0);
}


void startProgram(threadController* threadArgument){
    //Configure I2C Pins
    srand(time(NULL));
    runCommand("config-pin p9_18 i2c");
	runCommand("config-pin p9_17 i2c");
    runCommand("config-pin P8.11 pruout");
    runCommand("config-pin p8_15 pruin");
    runCommand("config-pin p8_16 pruin");
    runCommand("config-pin p9_22 pwm");

    initGPIO();

    threadArgument->i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
   // unsigned char outputValue = readI2cReg(threadArgument->i2cFileDesc,0x20);
    writeI2cReg(threadArgument->i2cFileDesc,REG_TURN_ON_ACCEL,0x00);
   //printf("Validating accelerometer memory value at 0x20 is set to 0x00 which is 'off', value = %u\n",outputValue);
    writeI2cReg(threadArgument->i2cFileDesc,REG_TURN_ON_ACCEL,0x27);
   // outputValue = readI2cReg(threadArgument->i2cFileDesc,0x20);
   // printf("Validating accelerometer memory value at 0x20 is set to 0x01 which is 'on', value = %u\n",outputValue);
   threadArgument->X = 0;
   threadArgument->Y = 0;
   threadArgument->Z = 0;
   threadArgument->hits = 0;
   threadArgument->missSound = 0;
   threadArgument->hitSound = 0;

    threadArgument->pointX = ((float)rand()/(float)(RAND_MAX)) - 0.5f;
    threadArgument->pointY = ((float)rand()/(float)(RAND_MAX)) - 0.5f;

    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    //Set "Running" boolean to indicate program is to run
    threadArgument->programRunning = 1;

    //Start acceleromter monitoring threads
    pthread_create(&tid, &attr, monitorAccelerometerX, threadArgument);
    threadArgument->threadIDs[0] = tid;

    pthread_create(&tid, &attr, monitorAccelerometerY, threadArgument);
    threadArgument->threadIDs[1] = tid;

     pthread_create(&tid, &attr, playSounds, threadArgument);
     threadArgument->threadIDs[2] = tid;

    //Start data printing thread
    pthread_create(&tid, &attr, printData, threadArgument);
    threadArgument->threadIDs[3] = tid;

    // //monitorJoystick thread
    // pthread_create(&tid, &attr, monitorJoystick, threadArgument);
    // threadArgument->threadIDs[5] = tid;

    //Wait for threads to gracefully return
    waitForProgramEnd(threadArgument);
}

void waitForProgramEnd(threadController* threadArgument){

    //Wait for accelerometer thread to join gracefully
    pthread_join(threadArgument->threadIDs[0],NULL);

    //Wait for printing thread to join gracefully
    pthread_join(threadArgument->threadIDs[1],NULL);

    // //Wait for printing thread to join gracefully
     pthread_join(threadArgument->threadIDs[2],NULL);

    //Wait for printing thread to join gracefully
    pthread_join(threadArgument->threadIDs[3],NULL);
}

