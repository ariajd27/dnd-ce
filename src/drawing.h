#ifndef DRAWING_H
#define DRAWING_H

#define COLOR_MAGENTA 0
#define COLOR_BLACK 1
#define COLOR_WHITE 2
#define COLOR_RED 3
#define COLOR_YELLOW 4

#define BG_X 40
#define BG_Y 0
#define PACKS_TITLE_X ((GFX_LCD_WIDTH - title_width) / 2)
#define PACKS_TITLE_Y ((80 - title_height) / 2)
#define LIST_X 40
#define LIST_Y 80
#define LIST_DY 14
#define LIST_WIDTH ((GFX_LCD_WIDTH - LIST_X) / 2)

#define LEVELS_TITLE_X 73
#define LEVELS_TITLE_Y 10
#define LEVEL_PACK_NAME_X 78
#define LEVEL_PACK_NAME_Y 48
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

#define LEVEL_TITLE_Y 16
#define MAP_X 88
#define MAP_Y 64
#define TILE_WIDTH 16
#define TILE_HEIGHT 16
#define DIGIT_HOFFSET 12
#define DIGIT_VOFFSET 12

void drawView();

typedef enum {
    SIDE_BACK,
    SIDE_FRONT
} side_t;

void drawTiles();
void drawEntities();
void drawWalls(side_t side);
void drawErrors();
void drawTotals();

void printCentered(char *str, uint8_t y);

#endif