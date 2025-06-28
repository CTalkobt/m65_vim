//
// Created by duck on 6/22/25.
//
#include <stdio.h>

#include "lib/m65/kbd.h"
#include "lib/m65/kernal.h"
#include "lib/m65/screen.h"
#include "mega65/hal.h"

char zBuffer[255+1];

/**
 * Read line from input up to n length.
 * Result in zBuffer.
 */
void read_line(char * str, int n) {
    unsigned char idx = 0;
    unsigned char input;
    bool exit = false;

    zBuffer[0] = '\0';

    do {
        kbdBufferClear();
        input = kbdHit();

        if (input == 0)
            continue;
*((unsigned char *)53280) = *((unsigned char *)53280) +1;

        if (input == 20) {
            if (idx > 0) {
                idx--;
                zBuffer[idx] = '\0';
                kBsout(20);
            }
            continue;
        } else
        if (input == 13) {
            exit = true;
        }

    } while (!exit);
    zBuffer[idx] = '\0';
}

int main(int argc, char *argv[]) {
    // test_screen();
    scrScreenMode(_80x50);
    scrClear();
    scrPuts("enter string:");
    read_line(zBuffer, 80);

    kPlotXY(4,5);
    scrPuts("Revision: "__DATE__ " " __TIME__ "\n");
    // if (kAddKey(3)) {
    //     scrscrBorder(2);
    //     *((unsigned char *)53280) = 2;
    // }
    // if (kAddKey('i')) {
    //     *((unsigned char *)53280) = 3;
    // }


    kPlotXY(0,48);
    scrDupeChar(80, '-');
    kPlotXY(0,49);
    scrDupeChar(78,'*');
    // scrClearLine(49);

    kPlotXY(0,10);
    scrPuts("0,10");

    kPlotXY(60,10);
    scrPuts("60,10");

    kPlotXY(60,40);
    scrPuts("60,40");

    usleep(6553600);
    usleep(6553600);

    kReset(HypervisorReset);

    // asm("LDA #2");
    // asm("JSR $FF32");  // reset.
}
