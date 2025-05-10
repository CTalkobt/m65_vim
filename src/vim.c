#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mega65/txtio.h>
#include <conio.h>

#include "cmd.h"



// Temp storage. Only used within a fn "frame". 
char zTemp[32+1]; 

// Vim is exiting. 
int exitVim = 0;

// Max x,y coordinates for screen. 
unsigned char maxX; 
unsigned char maxY; 

// Current x,y coordinates into text. 
unsigned char posX, posY; 
unsigned char maxPosY; 

// Current filename. 
char zFilename[80+1];   

EditMode mode = Default;

void draw_screen() {
    putch(147); 
    gotoxy(0,maxY-2); 
    for(unsigned char x=0;x<maxX;x++) {
        putch('-');
    }
    // Status line: 
    // "filename" <count>L, <count>B          x,ypos  n%
    // "text.txt" 20L, 400B                     5,6 10%
    // 
    gotoxy(0,maxY-1);
    txtEraseEOS();
    putchar('\"');puts(zFilename);putchar('\"'); 

    gotoxy(maxX-16,maxY-1);
    itoa(posX,zTemp,10);
    puts(zTemp); putchar(',');
    itoa(posY,zTemp,10);
    puts(zTemp);     

    gotoxy(maxX-4,maxY-1); 
    int percent = (maxPosY == 0) ? 0 : ((posY+0.0)/(maxPosY+0.0))*100; 
    itoa(percent,zTemp,10);
    puts(zTemp); putchar('%'); 
}

void edit() {
    strcpy(zFilename, "filename.txt"); 

    // Screen/Border color = black.
    *((char *)53280) = (char) 0;
    *((char *)53281) = (char) 0;

    txtScreen80x50(); 
    maxX=80; maxY=50; 

    draw_screen(); 

    do {
        int kar = getch();
        tpfnCmd cmdFn = getcmd(mode, kar);
        if (NULL != cmdFn) {
            cmdFn();
        } else {
            // @@TODO:Handle non command keypresses.
        }
    } while (!exitVim);
};

void cursor_on() {
    *((unsigned char *)0x1120) = 0; 
}

void cursor_off() {
    *((unsigned char *)0x1120) = 16; 
}


int main(void) {
    edit();
    return 0; 
}
