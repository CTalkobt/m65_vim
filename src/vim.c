#include <stdio.h>
#include <mega65.h>
#include <stdlib.h>
#include <string.h>

#include "lib/m65/debug.h"
#include "lib/m65/screen.h"

#include "state.h"
#include "cmd.h"
#include "render.h"
#include "lib/m65/kbd.h"
#include "mega65/memory.h"

#define MAX_CMD 78



void editCommand(tsState *psState) {
    static char zCmd[MAX_CMD+1];
    zCmd[0] = 0;

    DEBUG("mode:command start");

    unsigned int kar;
    kPlotXY(0,psState->screenEnd.yPos-1);
    // txtEraseEOS();
    kBsout(':');
    scrCursorOn();

    bool escape=false; 
    do {
        uint8_t l = strlen(zCmd);

        DEBUG("zCmd:");
        DEBUG(zCmd);DEBUG("\n");

{
    char zDebug[80];
    sprintf(zDebug, "X,Y1: %d,%d '%s':%d (%d,%d)\n", 1, psState->screenEnd.yPos - 1, zCmd, strlen(zCmd),
        l+1, psState->screenEnd.yPos-1);
    DEBUG(zDebug);
}
        kPlotXY(1, psState->screenEnd.yPos-1);
        if (strlen(zCmd) > 0)
            scrPuts(zCmd);
        kar = kbdGetKey();
{
    char zDebug[80];
    sprintf(zDebug, "X,Y2: %d,%d\n", l+1, psState->screenEnd.yPos - 1);
    DEBUG(zDebug);
}

        kPlotXY(l+1, psState->screenEnd.yPos-1);

        switch (kar) {
            case 20:    // ins/del
                if (l>0) {
                    l--;
                    zCmd[l]='\0';
                    kBsout(kar);
                }
                escape=false;
                break;
            case 27:    // escape
            case '\n':
            case 13:
debug_msg("13 or \\n or esc pressed\n");
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
                    kBsout(kar);
                    escape=false;
                }
                break;
        }
    } while (kar != 27 && kar != '\n' && kar != 13);
debug_msg("Exit cmd loop");
debug_msg("Command:"); debug_msg(zCmd);debug_msg("\n");

    switch(zCmd[0]){
        case 'r': // Read
            // cmdRead(psState, zCmd+1);
            break;
        case 'Q':
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

    scrCursorOn();
    do {
        kPlotXY(psState->xPos+ psState->screenStart.xPos, psState->lineY+psState->screenStart.yPos);
        const int kar = kbdGetKey();
        tpfnCmd cmdFn;
DEBUG("poll");
        switch(psState->editMode) {
            case Default:
                 cmdFn = getcmd(psState->editMode, kar);
                if (NULL != cmdFn) {
                    scrCursorOff();
                    cmdFn(psState);
                    scrCursorOn();
                } else {
                    // @@TODO:Handle non command keypresses.

                }
                if (psState->editMode == Default)
                    break;
            case Command:
                editCommand(psState); 
                draw_screen(psState); 
                break;
            default:
                psState->doExit = true;
                break;
        }
    } while (!psState->doExit);
DEBUG("edit() - exit:\n");
    // reset to default state
    scrCursorOff();
}

#define VIC_BASE 0xD000UL
#define SET_H640() POKE(VIC_BASE + 0x31, PEEK(VIC_BASE + 0x31) | 128)
#define CLEAR_H640() POKE(VIC_BASE + 0x31, PEEK(VIC_BASE + 0x31) & 127)
#define CLEAR_V400() POKE(VIC_BASE + 0x31, PEEK(VIC_BASE + 0x31) & 0xF7)
#define SET_V400() POKE(VIC_BASE + 0x31, PEEK(VIC_BASE + 0x31) | 8)
extern unsigned char g_curScreenW;
extern unsigned char g_curScreenH;
void mysetscreensize(unsigned  char w, unsigned char h)
{
    // if (w == 80) {
    //     SET_H640();
    //     // compensate for vic-iii h640 horizontal positioning bug
    //     POKE(0xd04cUL, (unsigned char)0x50);
    // }
    // else if (w == 40) {
    //     CLEAR_H640();
    //     POKE(0xd04cUL, (unsigned char)0x4e);
    // }
    //
    // if (h == 50) {
    //     SET_V400();
    // }
    // else if (h == 25) {
    //     CLEAR_V400();
    // }

    POKE(VIC_BASE+0x31, (uint8_t) 232);
    POKE(VIC_BASE+0x4c, (uint8_t) 80);

    // Cache values.
    if (w == 40 || w == 80) {
        g_curScreenW = w;
    }
    if (h == 25 || h == 50) {
        g_curScreenH = h;
    }
}


int main(void) {
    screenX=80;
    screenY=50;

    // mytest();

    // Setup screen.
    scrScreenMode(_80x50);
    // conioinit();
    // setscreensize(screenX,screenY);
    // esc-5   TODO-Find a way around this ESC-5 "hack". Without the screen doesn't shift to 80x50.
    // putchar(27); putchar(53);
    kbdBufferClear();


    scrClear();
    scrColor(COLOR_BLACK, COLOR_BLACK);

    tsState *state = malloc(sizeof(tsState));
    state->lines=1;
    state->lineY=0;
    state->xPos=0;
    strcpy(state->zFilename, "test.txt,s");
    state->doExit = false;
    state->screenStart.xPos = 0;
    state->screenStart.yPos = 0;
    state->screenEnd.yPos=screenY-3;
    state->screenEnd.xPos=screenX;
    state->editMode = Default;

    scrPutsXY(1,1, "version: 0.000001 (this is very alpha)");
    scrPutsXY(1,4,"current functionality / limitations:");
    scrPutsXY(1,6," :r<filename>    will attempt to read in memory however odd bug with");
    scrPutsXY(1,7,"                 drive error.");
    scrPutsXY(1,8," :q              will exit.");
    scrPutsXY(1,10, "screen y max:");  scrPutDec(screenY, 0);
    scrPutsXY(1,11, "screen x max:");  scrPutDec(screenX, 0);

    edit(state);

    // TODO: Eventually implement proper end to basic from mos-llvm.
    kReset(WarmBoot);

    return 0;
}
