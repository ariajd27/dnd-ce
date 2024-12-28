#include <stdlib.h>
#include <graphx.h>
#include <keypadc.h>
#include <sys/rtc.h>
#include "gfx/gfx.h"
#include "drawing.h"
#include "game.h"

int main(void) {
    gfx_Begin();
    gfx_SetPalette(default_palette, sizeof_default_palette, 0);
    gfx_SetDrawBuffer();
    gfx_FillScreen(1);

    kb_EnableOnLatch();
    kb_ClearOnLatch();

    setView(VIEW_TITLE);

    while (!toExit) {
        doInput();
        if (kb_On) break;
    }

    kb_ClearOnLatch();

    gfx_End();
    return 0;
}