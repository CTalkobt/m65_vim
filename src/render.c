#include <stdio.h>
#include <mega65/conio.h>

#include "render.h"
#include "state.h"
#include "lib/itoa.h"


// Screen size. (assume 80x50)
unsigned char screenX;
unsigned char screenY;

char zTemp[32+1];

// void inline cursor_on(void) {
//     __asm__ volatile (
//         "clc\n"
//         "jsr\t$ff35\n" : /* no output */ : /*no input*/ : /*clobbers */ "a","c");
// }
//
// void inline cursor_off(void) {
//     __asm__ volatile (
//     "sec\n"
//     "jsr\t$ff35\n" : /*out*/ :/*in*/ :/*clobber */"a","c");
// }

void draw_screen(tsState *psState) {
    drawStatus(psState); 
}

void drawStatus(tsState *psState) {
    // Status line: 
    // "filename" <count>L, <count>B          x,ypos  n%
    // "text.txt" 20L, 400B                     5,6 10%
    // 
    gotoxy(0, screenY-2);
    highlight(1);
    for(unsigned char x=0;x<screenX;x++) {
        cputc('-');
    }
    highlight(0);

    gotoxy(0, screenY-1);
    // txtEraseEOS(); // @@TODO: Need to correct for screenEnd/ screenStart
    pcputc('\"');pcputs(psState->zFilename);pcputc('\"');

    gotoxy(screenX-16,screenY-1);
    itoa(psState->xPos,zTemp,10);
    pcputs(zTemp); putchar(',');
    itoa(psState->lineY,zTemp,10);
    pcputs(zTemp);

    gotoxy(screenX-5,screenY-1); 
    int percent = (psState->lines == 0) ? 0 : (int) ((psState->lineY+0.0)/(psState->lines+0.0))*100;
    if (percent>100) {
        percent=100; 
    }
    itoa(percent,zTemp,10);
    pcputs(zTemp); pcputc('%');

    // RE-position cursor.
    gotoxy(psState->xPos, psState->lineY);
}


