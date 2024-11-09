#include <stdlib.h>
#include <graphx.h>
#include "map.h"
#include "drawing.h"
#include "game.h"
#include "gfx/gfx.h"
#include "asm/blitBuffer.h"

#define TILE_CURSOR tileset_tiles[0]
#define TILE_FLAG tileset_tiles[1]
#define TILE_CHEST tileset_tiles[2]
#define TILE_ERROR tileset_tiles[3]
#define TILE_MONSTER tileset_tiles[4]

const uint8_t tileIds[] = {12, 15, 8, 9, 0, 11, 14, 7, 13, 4, 1, 10, 3, 2, 5, 6};
#define getTileIndexFromNeighbors(tileNeighbors) tileIds[tileNeighbors]

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
            // draw the background and banner
            gfx_Sprite(levels, BG_X, BG_Y);
            gfx_RLETSprite(title, LEVELS_TITLE_X, LEVELS_TITLE_Y);
            gfx_SetTextFGColor(COLOR_BLACK);
            gfx_PrintStringXY(packTitle, LEVEL_PACK_NAME_X, LEVEL_PACK_NAME_Y);

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
                gfx_PrintStringXY(levelTitle, LEVEL_SELECT_NAME_X, LEVEL_SELECT_NAME_Y);
                gfx_SetColor(COLOR_RED);
                gfx_Rectangle(levelX - LEVELS_CURSOR_HOFFSET, levelY - LEVELS_CURSOR_VOFFSET, LEVELS_DX, LEVELS_DY);
            }

            break;

        case VIEW_GAME:
            gfx_Sprite(level, BG_X, BG_Y);
            gfx_SetTextFGColor(COLOR_WHITE);
            printCentered(levelTitle, LEVEL_TITLE_Y);
            drawTiles();

            break;

        default:
            break;
    }

    blitBuffer();
}

void drawTiles() {
    drawWalls(SIDE_BACK);
    drawEntities();
    drawWalls(SIDE_FRONT);
    drawTotals();
    drawErrors();
    gfx_RLETSprite(TILE_CURSOR, MAP_X + cursorCol * TILE_WIDTH + TILE_WIDTH / 2, MAP_Y + cursorRow * TILE_HEIGHT + TILE_HEIGHT / 2);
}

void drawEntities() {
    for (uint8_t col = 0; col < MAP_WIDTH; col++) {
        for (uint8_t row = 0; row < MAP_HEIGHT; row++) {
            switch (map[row][col].contents) {
                case ENTITY_FLAG:
                    gfx_RLETSprite(TILE_FLAG, MAP_X + col * TILE_WIDTH + TILE_WIDTH / 2, MAP_Y + row * TILE_HEIGHT + TILE_HEIGHT / 2);
                    break;
                case ENTITY_CHEST:
                    gfx_RLETSprite(TILE_CHEST, MAP_X + col * TILE_WIDTH + TILE_WIDTH / 2, MAP_Y + row * TILE_HEIGHT + TILE_HEIGHT / 2);
                    break;
                case ENTITY_MONSTER:
                    gfx_RLETSprite(TILE_MONSTER, MAP_X + col * TILE_WIDTH + TILE_WIDTH / 2, MAP_Y + row * TILE_HEIGHT + TILE_HEIGHT / 2);
                    break;
                default: break;
            }
        }
    }
}

void drawWalls(side_t side) {
    for (uint8_t col = 0; col <= MAP_WIDTH; col++) {
        for (uint8_t row = 0; row <= MAP_HEIGHT; row++) {
            uint8_t tileNeighbors = getCell(row - 1, col - 1).contents == ENTITY_WALL ? 1 : 0;
            tileNeighbors += 2 * (getCell(row - 1, col).contents == ENTITY_WALL ? 1 : 0);
            tileNeighbors += 4 * (getCell(row, col - 1).contents == ENTITY_WALL ? 1 : 0);
            tileNeighbors += 8 * (getCell(row, col).contents == ENTITY_WALL ? 1 : 0);

            if (side == SIDE_FRONT) {
                if (!(tileNeighbors & 0x0c)) continue;
            }
            else {
                if (tileNeighbors & 0x0c) continue;
            }

            const uint8_t tileId = getTileIndexFromNeighbors(tileNeighbors);
            gfx_RLETSprite(walls_tiles[tileId], MAP_X + col * TILE_WIDTH, MAP_Y + row * TILE_HEIGHT);
        }
    }
}

void drawErrors() {
    for (uint8_t col = 0; col < MAP_WIDTH; col++) {
        if (wallsInCol(col) > colTargets[col]) {
            for (uint8_t row = 0; row < MAP_HEIGHT; row++) {
                gfx_RLETSprite(TILE_ERROR, MAP_X + col * TILE_WIDTH + TILE_WIDTH / 2, MAP_Y + row * TILE_HEIGHT + TILE_HEIGHT / 2);
            }
        }
    }
    for (uint8_t row = 0; row < MAP_HEIGHT; row++) {
        if (wallsInRow(row) > rowTargets[row]) {
            for (uint8_t col = 0; col < MAP_WIDTH; col++) {
                gfx_RLETSprite(TILE_ERROR, MAP_X + col * TILE_WIDTH + TILE_WIDTH / 2, MAP_Y + row * TILE_HEIGHT + TILE_HEIGHT / 2);
            }
        }
    }
}

void drawTotals() {
    for (uint8_t col = 0; col < MAP_WIDTH; col++) {
        const uint8_t spriteIndex = colTargets[col] + (wallsInCol(col) == colTargets[col] ? 9 : 0);
        gfx_RLETSprite(digits_tiles[spriteIndex], MAP_X + col * TILE_WIDTH + TILE_WIDTH / 2, MAP_Y - DIGIT_VOFFSET);
    }
    for (uint8_t row = 0; row < MAP_HEIGHT; row++) {
        const uint8_t spriteIndex = rowTargets[row] + (wallsInRow(row) == rowTargets[row] ? 9 : 0);
        gfx_RLETSprite(digits_tiles[spriteIndex], MAP_X - DIGIT_HOFFSET, MAP_Y + row * TILE_HEIGHT + TILE_HEIGHT / 2);
    }
}

void printCentered(char *str, uint8_t y) {
    uint8_t length;
    for (length = 0; str[length] != 0x00; length++);
    gfx_PrintStringXY(str, GFX_LCD_WIDTH / 2 - 4 * length, y);
}