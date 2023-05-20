#include <stdint.h>
#include <stdbool.h>
#include <pru_cfg.h>
#include "resource_table_empty.h"
#include "sharedDataStruct.h"

#define STR_LEN         8       // # LEDs in our string
#define oneCyclesOn     700/5   // Stay on 700ns
#define oneCyclesOff    800/5
#define zeroCyclesOn    350/5
#define zeroCyclesOff   600/5
#define resetCycles     60000/5 // Must be at least 50u, use 60u
#define DELAY_250_MS 50000000

// P8_11 for output (on R30), PRU0
#define DATA_PIN 15       // Bit number to output on

volatile register uint32_t __R30;
volatile register uint32_t __R31;

// Shared Memory Configuration
// -----------------------------------------------------------
#define THIS_PRU_DRAM 0x00000 // Address of DRAM
#define OFFSET 0x200 // Skip 0x100 for Stack,
// 0x100 for Heap (from makefile)
#define THIS_PRU_DRAM_USABLE (THIS_PRU_DRAM + OFFSET)

// This works for both PRU0 and PRU1 as both map their own memory to 0x0000000
volatile sharedMemStruct_t *pSharedMemStruct = (volatile void *)THIS_PRU_DRAM_USABLE;

// GPIO Input: P8_15 = pru0_pru_r31_15
// = JSRT (Joystick Right) on Zen Cape
#define JOYSTICK_RIGHT_MASK (1 << 15)
#define JOYSTICK_QUIT_MASK (1 << 14)

void main(void)
{
    pSharedMemStruct->isButtonPressed = false;
    pSharedMemStruct->quit = 1;
    uint32_t brightColor = 0;
    uint32_t dimColor = 0;

    // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

    // COLOURS
    // - 1st element in array is 1st (bottom) on LED strip; last element is last on strip (top)
    // - Bits: {Green/8 bits} {Red/8 bits} {Blue/8 bits} {White/8 bits}
    uint32_t color[STR_LEN] = {
        // 0x0f000000, // Green
        // 0x000f0000, // Red
        // 0x00000f00, // Blue
        // 0x0000000f, // White
        // 0x0f0f0f00, // White (via RGB)
        // 0x0f0f0000, // Yellow
        // 0x000f0f00, // Purple
        0x0f000f00, // Teal
        0x0f000f00,
        0x0f000f00,
        0x0f000f00,
        0x0f000f00,
        0x0f000f00,
        0x0f000f00,
        0x0f000f00,
    };    

    __delay_cycles(resetCycles);

    while(pSharedMemStruct->quit){


    for(int j = 0; j < STR_LEN; j++) {
        for(int i = 31; i >= 0; i--) {
            if(color[j] & ((uint32_t)0x1 << i)) {
                __R30 |= 0x1<<DATA_PIN;      // Set the GPIO pin to 1
                __delay_cycles(oneCyclesOn-1);
                __R30 &= ~(0x1<<DATA_PIN);   // Clear the GPIO pin
                __delay_cycles(oneCyclesOff-2);
            } else {
                __R30 |= 0x1<<DATA_PIN;      // Set the GPIO pin to 1
                __delay_cycles(zeroCyclesOn-1);
                __R30 &= ~(0x1<<DATA_PIN);   // Clear the GPIO pin
                __delay_cycles(zeroCyclesOff-2);
            }
        }
    }



        // if (!(__R31 & JOYSTICK_RIGHT_MASK)) {
        //     pSharedMemStruct->isButtonPressed = true;
        // }
        pSharedMemStruct->isButtonPressed = (__R31 & JOYSTICK_QUIT_MASK) != 0;

        pSharedMemStruct->quit = (__R31 & JOYSTICK_RIGHT_MASK) != 0;

    if(pSharedMemStruct->left){
        //red
        brightColor = 0x00ff0000;
        dimColor = 0x000f0000;
    }
    if(pSharedMemStruct->right){
        //green
        brightColor = 0xff0000ff;
        dimColor = 0x0f000000;
    }

     for(int i = 0; i < STR_LEN; i++){
         color[i] = 0;
     }

    switch (pSharedMemStruct->height){
        case 0: 
            color[0] = brightColor;
            color[1] = dimColor;
            break;
        case 1:
            color[2] = dimColor;
            color[1] = brightColor;
            color[0] = dimColor;
            break;
        case 2:
            color[3] = dimColor;
            color[2] = brightColor;
            color[1] = dimColor;
            break;
        case 3:
            color[4] = dimColor;
            color[3] = brightColor;
            color[2] = dimColor;
            break;  
        case 4:
            color[5] = dimColor;
            color[4] = brightColor;
            color[3] = dimColor;
            break;
        case 5: 
            color[6] = dimColor;
            color[5] = brightColor;
            color[4] = dimColor;
            break;
        case 6:
            color[7] = dimColor;
            color[6] = brightColor;
            color[5] = dimColor;
            break;
        case 7: 
            color[7] = brightColor;
            color[6] = dimColor;
            break;
        case 8:
            for(int i = 0; i < STR_LEN; i++){
                color[i] = 0x00000f00;
            }
            break;
    }

    __R30 &= ~(0x1<<DATA_PIN);   // Clear the GPIO pin
    __delay_cycles(resetCycles);
    __delay_cycles(DELAY_250_MS);

    }



    // Send Reset
    // __R30 &= ~(0x1<<DATA_PIN);   // Clear the GPIO pin
    // __delay_cycles(resetCycles);

    __halt();
}