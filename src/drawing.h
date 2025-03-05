#ifndef DRAWING_H
#define DRAWING_H

#define COLOR_MAGENTA 0
#define COLOR_BLACK 1
#define COLOR_WHITE 2
#define COLOR_RED 3
#define COLOR_YELLOW 4
#define COLOR_LIGHT_GREY 5
#define COLOR_DARK_GREY 6
#define COLOR_LIGHT_BROWN 8
#define COLOR_DARK_BROWN 7

#define BG_X 40
#define BG_WIDTH 240
#define BANNER_HEIGHT 64
#define PACKS_TITLE_X ((GFX_LCD_WIDTH - title_width) / 2)
#define PACKS_TITLE_Y ((80 - title_height) / 2)
#define LIST_X 40
#define LIST_Y 80
#define LIST_DY 14
#define LIST_WIDTH ((GFX_LCD_WIDTH - LIST_X) / 2)

#define LEVELS_TITLE_X 73
#define LEVELS_TITLE_Y 10
#define LEVEL_PACK_NAME_X 78
#define LEVEL_PACK_NAME_Y 49
#define LEVEL_SELECT_NAME_X 64
#define LEVEL_SELECT_NAME_Y 74
#define LEVELS_PER_ROW 6
#define LEVELS_MAX_ROWS 4
#define LEVELS_X 54
#define LEVELS_Y 91
#define LEVELS_DX 36
#define LEVELS_DY 36
#define LEVELS_CURSOR_HOFFSET 2
#define LEVELS_CURSOR_VOFFSET 2
#define LEVELS_SCALE 4

#define LEVEL_TITLE_Y LEVEL_PACK_NAME_Y
#define MAP_X 88
#define MAP_Y 86
#define TILE_WIDTH 16
#define TILE_HEIGHT 16
#define DIGIT_HOFFSET 12
#define DIGIT_VOFFSET 12

void drawView();

extern uint8_t levelWinAnimFrame;

void drawTiles();
void drawEntities();
void drawWalls();
void drawErrorsAndTotals();

void printCentered(char *str, uint8_t y);

#endif