#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <c64/kernalio.h>

#include <mega65/txtio.h>
#include <conio.h>

#include "state.h"
#include "cmd.h"
#include "render.h"


// Max x,y coordinates for screen. 
unsigned char maxX; 
unsigned char maxY; 

// Current x,y coordinates into text. 
unsigned char posX, posY; 
unsigned char maxPosY; 

extern uint8_t screenX;
extern uint8_t screenY;

#define MAX_CMD 78

#include <c64/kernalio.h>

void cmdRead(tsState *psState, char *pzCmdReminder) {
    static const int size=254;
    char zFilename[size];
*((unsigned char *)53280)=1;


    strcpy(zFilename, pzCmdReminder);
    strcat(zFilename,",s,r"); // ,r");

#ifdef __MEGA65__
    krnio_setbnk(0,0);
#endif
    krnio_setnam(zFilename);

    char zBuffer[size+1];

    if (krnio_open(1, 8, 8)) {

        krnio_read(1, zBuffer, sizeof(char)*size);
        zBuffer[size] = 0;
        puts(zBuffer);

        krnio_close(1);
    }

    krnio_clrchn();

}

void editCommand(tsState *psState) {
    static char zCmd[MAX_CMD+1];
    zCmd[0] = '\0';

    int kar; 
    gotoxy(0,screenY-1); 
    txtEraseEOS(); 
    putchar(':'); 
    cursor_on(); 
    bool escape=false; 
    do {
        uint8_t l = strlen(zCmd); 
        gotoxy(1, screenY-1); 
        puts(zCmd); 
        gotoxy(l+1, screenY-1); 
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
                if (l<MAX_CMD) {
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
                }
                break;
        }

    } while (kar != 27 && kar != '\n');     

    switch(zCmd[0]){
        case 'r': // Read
            cmdRead(psState, zCmd+1);
            break;
        case 'q': // Quit
            psState->doExit=true;
            break;
        default:
            break;
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
    } while (!psState->doExit);

    // reset to default state. 
    cursor_off(); 
}


int main(void) {
#ifdef __MEGA65__
    txtScreen80x50();
#endif

    tsState *state = malloc(tsState); 

    state->lines=1;
    state->lineY=0; 
    state->xPos=0;
    strcpy(state->zFilename, "test.txt,s");
    state->doExit = false; 
    state->screenStart.xPos = 0;
    state->screenStart.yPos = 0;
#ifdef __MEGA65__
    state->screenEnd.yPos=48;
    state->screenEnd.xPos=80;
#else /**assume c64 */
    state->screenEnd.yPos=22;
    state->screenEnd.xPos=40;
#endif
    state->editMode = Default; 

    putch(147);
#ifdef __MEGA65__
    puts("version: 0.000001 (this is very alpha)\n\n");
    puts("current functionality / limitations:\n\n");
    puts(" :r<filename>    will attempt to read in memory however odd bug with\n");
    puts("                 drive error.\n\n");
    puts(" :q              will exit.");
#endif

    edit(state);
    return 0; 
}
