#include <stdio.h>
#include <conio.h>
#include <mega65/txtio.h>

#include "state.h"
#include "render.h"

char zTemp[32+1];

void cursor_on() {
  *((unsigned char *)0x1120) = 0; 
}

void cursor_off() {
  *((unsigned char *)0x1120) = 16; 
}

void draw_screen(tsState *psState) {
    putch(147); 
    drawStatus(psState); 
}

void drawStatus(tsState *psState) {
    uint8_t maxX=psState->screenEnd.xPos- psState->screenStart.xPos; 
    uint8_t maxY=psState->screenEnd.yPos- psState->screenStart.yPos; 


    // Status line: 
    // "filename" <count>L, <count>B          x,ypos  n%
    // "text.txt" 20L, 400B                     5,6 10%
    // 
    gotoxy(psState->screenStart.xPos, psState->screenEnd.yPos-2); 
    for(unsigned char x=0;x<maxX;x++) {
        putch('-');
    }

    gotoxy(0,psState->screenEnd.yPos-1);
    txtEraseEOS(); // @@TODO: Need to correct for screenEnd/ screenStart
    putchar('\"');puts(psState->zFilename);putchar('\"'); 

    gotoxy(psState->screenEnd.xPos-16,psState->screenEnd.yPos-1);
    itoa(psState->xPos,zTemp,10);
    puts(zTemp); putchar(',');
    itoa(psState->lineY,zTemp,10);
    puts(zTemp);     

    gotoxy(maxX-4,maxY-1); 
    int percent = (psState->lines == 0) ? 0 : ((psState->lineY+0.0)/(psState->lines+0.0))*100; 
    itoa(percent,zTemp,10);
    puts(zTemp); putchar('%'); 
}


