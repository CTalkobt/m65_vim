//#include <stdlib.h>
#include <stdio.h>
#include <mega65.h>
#include <stdlib.h>
#include <string.h>
#include <mega65/conio.h>

#include "state.h"
#include "cmd.h"
#include "render.h"

#define MAX_CMD 78

void cmdRead(tsState *psState, char *pzCmdReminder) {
    static const int size=254;
*((unsigned char *)53280)=1;
    // @@TODO
//
//
//     strcpy(zFilename, pzCmdReminder);
//     strcat(zFilename,",s,r"); // ,r");
//
// #ifdef __MEGA65__
//     krnio_setbnk(0,0);
// #endif
//     krnio_setnam(zFilename);
//
//     char zBuffer[size+1];
//
//     if (krnio_open(1, 8, 8)) {
//
//         krnio_read(1, zBuffer, sizeof(char)*size);
//         zBuffer[size] = 0;
//         puts(zBuffer);
//
//         krnio_close(1);
//     }
//
//     krnio_clrchn();

}

void editCommand(tsState *psState) {
    static char zCmd[MAX_CMD+1];
    zCmd[0] = '\0';

    pcputs("Starting...");

    unsigned int kar;
    gotoxy(0,screenY-1); 
    // txtEraseEOS();
    putchar(':'); 
    // cursor_on();
    bool escape=false; 
    do {
        uint8_t l = strlen(zCmd); 
        gotoxy(1, screenY-1); 
        puts(zCmd); 
        gotoxy(l+1, screenY-1); 
        kar = cgetc();
        switch (kar) {
            case 20:    // ins/del
                if (l>0) {
                    l--;
                    zCmd[l]='\0';
                    cputc(kar);
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
                    cputc(kar);
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
    draw_screen(psState);

    // cursor_on();
    do {
        gotoxy(psState->xPos+ psState->screenStart.xPos, psState->lineY+psState->screenStart.yPos);
        const int kar = cgetc();
        tpfnCmd cmdFn;
        switch(psState->editMode) {
            case Default:
                 cmdFn = getcmd(psState->editMode, kar);
                if (NULL != cmdFn) {
                    // cursor_off();
                    cmdFn(psState);
                    // cursor_on();
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
    // cursor_off();
}


void test_cgetc(void) {
    while (true) {
        unsigned char c = cgetc();

        clrscr();
        gohome();

        cputdec( (unsigned long) c, 0, 0);
    }
}

int main(void) {
    // Setup screen.
    conioinit();
    flushkeybuf();
    setextendedattrib(true);
    clrscr();
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
    textcolor(COLOR_WHITE);

    // test_cgetc();

    getscreensize(&screenX, &screenY);

    tsState *state = malloc(sizeof(tsState));
    state->lines=1;
    state->lineY=0; 
    state->xPos=0;
    strcpy(state->zFilename, "tespt.txt,s");
    state->doExit = false; 
    state->screenStart.xPos = 0;
    state->screenStart.yPos = 0;
    state->screenEnd.yPos=screenY-3;
    state->screenEnd.xPos=screenX;
    state->editMode = Default;

#ifdef __MEGA65__
    pcputsxy(1,1,"version: 0.000001 (this is very alpha)");
    pcputsxy(1,4,"current functionality / limitations:");
    pcputsxy(1,6," :r<filename>    will attempt to read in memory however odd bug with");
    pcputsxy(1,7,"                 drive error.");
    pcputsxy(1,8," :q              will exit.");
#endif

    edit(state);
    while (true) {
        for (int i=0; i<16; i++) {
            bgcolor(i);
            bordercolor(15-i);
        }
    }

    return 0; 
}
