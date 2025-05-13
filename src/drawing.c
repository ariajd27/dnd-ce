#include <stdlib.h>
#include <graphx.h>
#include <time.h>
#include "map.h"
#include "drawing.h"
#include "game.h"
#include "gfx/gfx.h"
#include "asm/blitBuffer.h"

#define TILE_CURSOR tileset_tiles[0]
#define TILE_FLAG tileset_tiles[1]
#define TILE_CHEST tileset_tiles[2]
#define TILE_ERROR tileset_tiles[3]

#define NUM_WIN_ANIM_FRAMES 4
#define WIN_ANIM_FRAME_TIME 10000

const uint8_t tileIds[] = {12, 15, 8, 9, 0, 11, 14, 7, 13, 4, 1, 10, 3, 2, 5, 6};
#define getTileIndexFromNeighbors(tileNeighbors) tileIds[tileNeighbors]

uint8_t levelWinAnimFrame = 0;
clock_t lastWinAnimFrame;

void drawBackground()
{
    gfx_SetColor(COLOR_LIGHT_GREY);
    gfx_FillRectangle(BG_X, 0, BG_WIDTH, BANNER_HEIGHT);
    gfx_SetColor(COLOR_DARK_GREY);
    gfx_FillRectangle(BG_X, BANNER_HEIGHT, BG_WIDTH, GFX_LCD_HEIGHT - BANNER_HEIGHT);
}

void drawView() {
    gfx_FillScreen(COLOR_BLACK);
    switch (currentView) {
        case VIEW_TITLE:
            gfx_SetTextFGColor(COLOR_WHITE);
            gfx_RLETSprite(title, PACKS_TITLE_X, PACKS_TITLE_Y);
            gfx_SetTextXY(LIST_X, LIST_Y - LIST_DY);

            if (numLevelPacks > 0) {
                gfx_PrintString("Select pack to load:");
                listLevelPacks();
            }
            else {
                gfx_PrintString("No level packs found!");
            }

            break;

        case VIEW_LEVELS:
            drawBackground();
            gfx_RLETSprite(title, LEVELS_TITLE_X, LEVELS_TITLE_Y);
            gfx_SetTextFGColor(COLOR_BLACK);
            printCentered(packTitle, LEVEL_PACK_NAME_Y);

            for (uint8_t i = 0; i < *packNumLevels; i++) {
                const uint24_t levelX = LEVELS_X + LEVELS_DX * (i % LEVELS_PER_ROW);
                const uint8_t levelY = LEVELS_Y + LEVELS_DY * (i / LEVELS_PER_ROW);
                loadLevel(i);

                // draw a preview of the level
                for (uint8_t row = 0; row < MAP_HEIGHT; row++) {
                    for (uint8_t col = 0; col < MAP_WIDTH; col++) {
                        const uint8_t cellColor = map[row][col].contents == ENTITY_WALL ? COLOR_BLACK
                                                : map[row][col].contents == ENTITY_CHEST ? COLOR_YELLOW
                                                : map[row][col].contents == ENTITY_MONSTER ? COLOR_RED
                                                : COLOR_WHITE;
                        gfx_SetColor(cellColor);
                        gfx_FillRectangle(levelX + col * LEVELS_SCALE, levelY + row * LEVELS_SCALE, LEVELS_SCALE, LEVELS_SCALE);
                    }
                }

                if (i != levelIndex) continue;

                // only get here if this is the selected level
                // draw the selection cursor and display the name of the level
                printCentered(levelTitle, LEVEL_SELECT_NAME_Y);
                gfx_SetColor(COLOR_RED);
                gfx_Rectangle(levelX - LEVELS_CURSOR_HOFFSET, levelY - LEVELS_CURSOR_VOFFSET, LEVELS_DX, LEVELS_DY);
            }

            break;

        case VIEW_GAME:
            drawBackground();
            gfx_RLETSprite(title, LEVELS_TITLE_X, LEVELS_TITLE_Y);
            gfx_SetTextFGColor(COLOR_BLACK);
            printCentered(levelTitle, LEVEL_TITLE_Y);
            drawTiles();

            break;

        default:
            break;
    }

    blitBuffer();
}

void drawTiles() {
    drawWalls();
    drawEntities();
    drawErrorsAndTotals();

    // no cursor if win animation playing (to tell user interaction is not permitted)
    if (levelWinAnimFrame != 0) return;
    
    gfx_RLETSprite(TILE_CURSOR, MAP_X + cursorCol * TILE_WIDTH + TILE_WIDTH / 2, MAP_Y + cursorRow * TILE_HEIGHT + TILE_HEIGHT / 2 + 1);
}

void drawEntities()
{
    for (uint8_t col = 0; col < MAP_WIDTH; col++)
    {
        for (uint8_t row = 0; row < MAP_HEIGHT; row++)
        {
            gfx_rletsprite_t *tileSprite;

            switch (map[row][col].contents) {
                case ENTITY_FLAG:
                    tileSprite = TILE_FLAG;
                    break;
                case ENTITY_CHEST:
                    tileSprite = TILE_CHEST;
                    break;
                case ENTITY_MONSTER:
                    tileSprite = tileset_tiles[4 * (map[row][col].variant + 1) + levelWinAnimFrame];
                    break;
                default: continue;
            }

            gfx_RLETSprite(tileSprite, MAP_X + col * TILE_WIDTH + TILE_WIDTH / 2, MAP_Y + row * TILE_HEIGHT + TILE_HEIGHT / 2);
        }
    }
}

void drawWalls() {
    for (uint8_t col = 0; col <= MAP_WIDTH; col++) {
        for (uint8_t row = 0; row <= MAP_HEIGHT; row++) {
            uint8_t tileNeighbors = getCell(row - 1, col - 1).contents == ENTITY_WALL ? 1 : 0;
            tileNeighbors += 2 * (getCell(row - 1, col).contents == ENTITY_WALL ? 1 : 0);
            tileNeighbors += 4 * (getCell(row, col - 1).contents == ENTITY_WALL ? 1 : 0);
            tileNeighbors += 8 * (getCell(row, col).contents == ENTITY_WALL ? 1 : 0);

            const uint8_t tileId = getTileIndexFromNeighbors(tileNeighbors);
            gfx_RLETSprite(walls_tiles[tileId], MAP_X + col * TILE_WIDTH, MAP_Y + row * TILE_HEIGHT);
        }
    }
}

void drawErrorsAndTotals()
{
    bool noExcess = true;

    // errors
    for (uint8_t col = 0; col < MAP_WIDTH; col++) {
        if (wallsInCol(col) > colTargets[col]) {
            noExcess = false;
            for (uint8_t row = 0; row < MAP_HEIGHT; row++) {
                gfx_RLETSprite(TILE_ERROR, MAP_X + col * TILE_WIDTH + TILE_WIDTH / 2, MAP_Y + row * TILE_HEIGHT + TILE_HEIGHT / 2);
            }
        }
    }
    for (uint8_t row = 0; row < MAP_HEIGHT; row++) {
        if (wallsInRow(row) > rowTargets[row]) {
            noExcess = false;
            for (uint8_t col = 0; col < MAP_WIDTH; col++) {
                gfx_RLETSprite(TILE_ERROR, MAP_X + col * TILE_WIDTH + TILE_WIDTH / 2, MAP_Y + row * TILE_HEIGHT + TILE_HEIGHT / 2);
            }
        }
    }

    bool noMissing = true;

    // totals
    for (uint8_t col = 0; col < MAP_WIDTH; col++) {
        bool thisSatisfied = wallsInCol(col) == colTargets[col];
        noMissing &= thisSatisfied;
        const uint8_t spriteIndex = colTargets[col] + (thisSatisfied ? 9 : 0);
        gfx_RLETSprite(digits_tiles[spriteIndex], MAP_X + col * TILE_WIDTH + TILE_WIDTH / 2, MAP_Y - DIGIT_VOFFSET);
    }
    for (uint8_t row = 0; row < MAP_HEIGHT; row++) {
        bool thisSatisfied = wallsInRow(row) == rowTargets[row];
        noMissing &= thisSatisfied;
        const uint8_t spriteIndex = rowTargets[row] + (thisSatisfied ? 9 : 0);
        gfx_RLETSprite(digits_tiles[spriteIndex], MAP_X - DIGIT_HOFFSET, MAP_Y + row * TILE_HEIGHT + TILE_HEIGHT / 2);
    }

    if (noMissing && noExcess)
    {
        if (levelWinAnimFrame == 0) {
            levelWinAnimFrame++;
            lastWinAnimFrame = clock();

            for (uint8_t row = 0; row < MAP_HEIGHT; row++)
            {
                for (uint8_t col = 0; col < MAP_WIDTH; col++)
                {
                    if (map[row][col].contents == ENTITY_FLAG) map[row][col].contents = ENTITY_NONE;
                }
            }
        }
        else if (clock() - lastWinAnimFrame > WIN_ANIM_FRAME_TIME)
        {
            lastWinAnimFrame = clock();
            if (levelWinAnimFrame < NUM_WIN_ANIM_FRAMES - 1) {
                levelWinAnimFrame++;
            }
            else {
                levelWinAnimFrame--;
            }
        }
    }
}

void printCentered(char *str, uint8_t y)
{
    uint8_t length = 0;
    for (char *ptr = str; *ptr != 0x00; ptr++)
    {
        const uint8_t dl = *ptr == ' ' ? 3 : 8;
        length += dl;
    }

    const uint24_t x0 = (GFX_LCD_WIDTH - (uint24_t) length) / 2;
    const uint24_t x1 = (GFX_LCD_WIDTH + (uint24_t) length) / 2;

    // draw scroll background
    gfx_SetColor(COLOR_DARK_BROWN);
    gfx_HorizLine_NoClip(x0 - 3, y - 3, length + 6);
    gfx_HorizLine_NoClip(x0 - 3, y + 9, length + 6);
    gfx_SetColor(COLOR_LIGHT_BROWN);
    gfx_FillRectangle_NoClip(x0 - 2, y - 2, length, 11);
    gfx_RLETSprite_NoClip(scroll, x0 - (scroll_width + 2), y - (scroll_height - 8)/2 - 1);
    gfx_RLETSprite_NoClip(scroll, x1 - 2, y - (scroll_height - 8)/2 - 1);

    // text
    gfx_SetColor(COLOR_BLACK);
    gfx_PrintStringXY(str, x0, y);
}