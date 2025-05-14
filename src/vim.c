#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mega65/txtio.h>
#include <conio.h>

#include "state.h"
#include "cmd.h"
#include "render.h"


// Vim is exiting. 
int exitVim = 0;

// Max x,y coordinates for screen. 
unsigned char maxX; 
unsigned char maxY; 

// Current x,y coordinates into text. 
unsigned char posX, posY; 
unsigned char maxPosY; 

// Current filename. 
EditMode mode = Default;

void edit(tsState *psState) {
    // Screen/Border color = black.
    *((char *)53280) = (char) 0;
    *((char *)53281) = (char) 0;

    maxX=80; maxY=50; 

    draw_screen(psState); 

    cursor_on(); 
    do {
        gotoxy(psState->xPos+ psState->screenStart.xPos, psState->lineY+psState->screenStart.yPos); 
        int kar = getch();
        tpfnCmd cmdFn = getcmd(mode, kar);
        if (NULL != cmdFn) {
            cursor_off(); 
            cmdFn(psState);
            cursor_on(); 
        } else {
            // @@TODO:Handle non command keypresses.
        }
    } while (!exitVim);

    // reset to default state. 
    cursor_off(); 
}


int main(void) {
    txtScreen80x50(); 

    tsState *state = malloc(tsState); 
    state->lines=1;
    state->lineY=0; 
    state->xPos=0;
    strcpy(state->zFilename, "test.txt");
    state->doExit = 0; 
    state->screenStart.xPos = 0;
    state->screenStart.yPos = 0;
    state->screenEnd.yPos=48;
    state->screenEnd.xPos=80;
    state->editMode = Default; 

    edit(state);
    return 0; 
}
