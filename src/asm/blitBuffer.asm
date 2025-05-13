    vbuffer := 0xD52C00
    vram := 0xD40000
    numBytes := 76800

    assume adl=1

    section .text
    public _blitBuffer
_blitBuffer:
    ld hl, vbuffer
    ld de, vram
    ld bc, numBytes
    ldir
    ret