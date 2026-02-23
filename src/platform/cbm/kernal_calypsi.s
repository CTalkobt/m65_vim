    .section code
    .extern _Zp

    .public kBsout
kBsout: jmp 0xffd2


    .public kSetBank
kSetBank:
    ldx zp:_Zp+0
    clc
    jsr 0xff6b
    rts

    .public kSetnam
kSetnam:
    ldx zp:_Zp+0
    ldy zp:_Zp+1
    clc
    jsr 0xffbd
    rts

    .public kSetlfs
kSetlfs:
    ldx zp:_Zp+0
    ldy zp:_Zp+1
    jsr 0xffba
    rts

    .public kOpen
kOpen:
    clc
    jsr 0xffc0
    rts

    .public kClose
kClose:
    sec
    jsr 0xffc3
    rts

    .public kChkin
kChkin:
    tax
    jsr 0xffc6
    rts

    .public kCkout
kCkout:
    tax
    jsr 0xffc9
    rts

    .public kClrchn
kClrchn:
    jsr 0xffcc
    rts

    .public kBasin
kBasin:
    jsr 0xffcf
    rts

    .public kGetin
kGetin:
    jsr 0xffe4
    rts

    .public kReset
kReset:
    jmp 0xff32

    .public kPlotXY
kPlotXY:
    tay
    ldx zp:_Zp+0
    clc
    jmp 0xfff0

    .public scrClear
scrClear:
    lda #147
    jmp 0xffd2

    .public kFnKeyMacros
kFnKeyMacros:
    beq disable$
enable$:
    sec
    jsr 0xff47
    and #0xff-32
    clc
    jsr 0xff47
    jmp done$
disable$:
    sec
    jsr 0xff47
    ora #32
    clc
    jsr 0xff47
done$:
    rts

    .public scrCursorOn
    .public plShowCursor
plShowCursor:
scrCursorOn:
    clc
    jmp 0xff35

    .public scrCursorOff
    .public plHideCursor
plHideCursor:
scrCursorOff:
    sec
    jmp 0xff35
