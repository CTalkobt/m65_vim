#include <stdio.h>

#include "lib/m65/txtio.h"


#include "render.h"
#include "state.h"
#include "lib/itoa.h"

// Screen size. (assume 80x50)
#ifdef __MEGA65__
uint8_t screenX = 80; 
uint8_t screenY = 50;
#else /**Assume c64 */
uint8_t screenX = 40;
uint8_t screenY = 24;
#endif


char zTemp[32+1];

void inline cursor_on(void) {
    __asm__ volatile (
        "clc\n"
        "jsr\t$ff35\n" : /* no output */ : /*no input*/ : /*clobbers */ "a","p");
}

void inline cursor_off(void) {
    __asm__ volatile (
    "sec\n"
    "jsr\t$ff35\n" : /*out*/ :/*in*/ :/*clobber */"a","p");
}

void draw_screen(tsState *psState) {
    drawStatus(psState); 
}

void drawStatus(tsState *psState) {
    // Status line: 
    // "filename" <count>L, <count>B          x,ypos  n%
    // "text.txt" 20L, 400B                     5,6 10%
    // 
    gotoxy(0, screenY-2);
    for(unsigned char x=0;x<screenX;x++) {
        putchar('-');
    }

    gotoxy(0, screenY-1);
    txtEraseEOS(); // @@TODO: Need to correct for screenEnd/ screenStart
    putchar('\"');puts(psState->zFilename);putchar('\"'); 

    gotoxy(screenX-16,screenY-1);
    itoa(psState->xPos,zTemp,10);
    puts(zTemp); putchar(',');
    itoa(psState->lineY,zTemp,10);
    puts(zTemp);     

    gotoxy(screenX-5,screenY-1); 
    int percent = (psState->lines == 0) ? 0 : ((psState->lineY+0.0)/(psState->lines+0.0))*100; 
    if (percent>100) {
        percent=100; 
    }
    itoa(percent,zTemp,10);
    puts(zTemp); putchar('%'); 
}


