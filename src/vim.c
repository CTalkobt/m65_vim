#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <mega65/txtio.h>
#include <conio.h>

#include "state.h"
#include "cmd.h"
#include "render.h"


// Vim is exiting. 
bool exitVim = false;

// Max x,y coordinates for screen. 
unsigned char maxX; 
unsigned char maxY; 

// Current x,y coordinates into text. 
unsigned char posX, posY; 
unsigned char maxPosY; 

extern uint8_t screenX;
extern uint8_t screenY;

void editCommand(tsState *psState) {
    static char zCmd[80+1];
    zCmd[0] = '\0';

    int kar; 
    gotoxy(0,screenY-1); 
    txtEraseEOS(); 
    cursor_on(); 
    bool escape=false; 
    do {
        uint8_t l = strlen(zCmd); 
        gotoxy(0, screenY-1); 
        puts(zCmd); 
        gotoxy(l, screenY-1); 
        kar = getch();
        switch (kar) {
            case 20:    // ins/del
                if (l>0) {
                    l--;
                    zCmd[l]='\0';
                    putchar(kar); 
                }
                escape=false;
                break;
            case 27:    // escape
            case '\n':
                break;
            case 22:    // Ctrl-V
                if(!escape) {
                    escape=true; 
                    break;
                }
            default:
                // @@TODO: Filter control codes unless prefixed by Ctrl-V. 
                // 
                if(escape) {
                    zCmd[l]='^';
                    l++; 
                }
                zCmd[l] = kar; 
                zCmd[l+1] = '\0';
                putch(kar); 
                escape=false;
                break;
        }

    } while (kar != 27 && kar != '\n');     

    if (strcmp(zCmd, "q") == 0) {
        exitVim=true; 
    }        

    psState->editMode = Default; 
}

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
        switch(psState->editMode) {
            case Default:
                tpfnCmd cmdFn = getcmd(psState->editMode, kar);
                 if (NULL != cmdFn) {
                    cursor_off(); 
                    cmdFn(psState);
                    cursor_on(); 
                } else {
                    // @@TODO:Handle non command keypresses.
                }
                if (psState->editMode == Default)
                    break;
            case Command:
//                (*(unsigned char *)53280)++; 
                editCommand(psState); 
                draw_screen(psState); 
                break;
            default:
                break;
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
