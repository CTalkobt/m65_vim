    .public _kSetnam
    .public _kSetlfs
    .public _kOpen
    .public _kClose
    .public _kChkin
    .public _kClrchn
    .public _kBasin
    .public _kGetin
    .public _kPlotXY
    .public _kFnKeyMacros
    .public _plHideCursor
    .public _plShowCursor
    .public _scrCursorOn
    .public _scrCursorOff

    .segment code

    .public _kBsout
_kBsout: 
    pha
    lda 2,sp
    jsr $ffd2
    pla
    rts


    .public _kSetBank
_kSetBank:
    pha
    lda 2,s
    ldx 3,s
    clc
    jsr $ff6b
    pla
    rts

_kSetnam:
    pha
    lda 2,s
    ldx 3,s
    ldy 4,s
    clc
    jsr $ffbd
    pla
    rts

_kSetlfs:
    pha
    lda 2,s
    ldx 3,s
    ldy 4,s
    jsr $ffba
    pla
    rts

_kOpen:
    clc
    jsr $ffc0
    rts

_kClose:
    pha
    lda 2,s
    sec
    jsr $ffc3
    pla
    rts

_kChkin:
    pha
    lda 2,s
    jsr $ffc6
    pla
    rts

_kClrchn:
    jsr $ffcc
    rts

_kBasin:
    jsr $ffcf
    rts

_kGetin:
    jsr $ffe4
    rts

_kPlotXY:
    pha
    ldx 2,s
    ldy 3,s
    clc
    jsr $fff0
    pla
    rts

_kFnKeyMacros:
    pha
    lda 2,s
    beq disable
enable:
    sec
    jsr $ff47
    and #$ff-32
    clc
    jsr $ff47
    jmp done
disable:
    sec
    jsr $ff47
    ora #32
    clc
    jsr $ff47
done:
    pla
    rts

_plHideCursor:
    sec
    jsr $ff35
    rts

_plShowCursor:
    clc
    jsr $ff35
    rts

_scrCursorOn:
    clc
    jsr $ff35
    rts

_scrCursorOff:
    sec
    jsr $ff35
    rts
