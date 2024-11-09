#include <fileioc.h>
#include <graphx.h>
#include <keypadc.h>
#include <ti/getcsc.h>
#include "game.h"
#include "drawing.h"
#include "map.h"

bool toExit = false;

view_t currentView = VIEW_TITLE;

uint8_t cursorRow = 0;
uint8_t cursorCol = 0;

uint8_t *rowTargets;
uint8_t *colTargets;

uint8_t numLevelPacks;
char *levelPackNames[MAX_PACKS];

void *packStart;
char *packTitle;
uint8_t *packNumLevels;
uint16_t *packToc;

uint8_t levelIndex;
char *levelTitle;

void doInput() {
    bool changed = false;

    uint8_t key = os_GetCSC();

    switch(currentView) {
        case VIEW_CREDITS:
            if (key == sk_2nd || key == sk_Clear) {
                setView(VIEW_TITLE);
            }
            break;

        case VIEW_GAME:
            if (map[cursorRow][cursorCol].contents != ENTITY_MONSTER && map[cursorRow][cursorCol].contents != ENTITY_CHEST) {
                if (key == sk_2nd) {
                    map[cursorRow][cursorCol].contents = map[cursorRow][cursorCol].contents == ENTITY_WALL ? ENTITY_NONE : ENTITY_WALL;
                    changed = true;
                }
                else if (key == sk_Alpha) {
                    map[cursorRow][cursorCol].contents = map[cursorRow][cursorCol].contents == ENTITY_FLAG ? ENTITY_NONE : ENTITY_FLAG;
                    changed = true;
                }
            }

            if (key == sk_Left && cursorCol > 0) {
                cursorCol--;
                changed = true;
            }
            if (key == sk_Right && cursorCol < MAP_WIDTH - 1) {
                cursorCol++;
                changed = true;
            }
            if (key == sk_Up && cursorRow > 0) {
                cursorRow--;
                changed = true;
            }
            if (key == sk_Down && cursorRow < MAP_HEIGHT - 1) {
                cursorRow++;
                changed = true;
            }

            if (key == sk_Clear) {
                saveLevel();
                setView(VIEW_LEVELS);
            }

            break;
        
        case VIEW_LEVELS:
            if (key == sk_Up && levelIndex >= LEVELS_PER_ROW) {
                levelIndex -= LEVELS_PER_ROW;
                changed = true;
            }
            if (key == sk_Down && levelIndex + LEVELS_PER_ROW < *packNumLevels) {
                levelIndex += LEVELS_PER_ROW;
                changed = true;
            }
            if (key == sk_Left && levelIndex > 0) {
                levelIndex--;
                changed = true;
            }
            if (key == sk_Right && levelIndex + 1 < *packNumLevels) {
                levelIndex++;
                changed = true;
            }

            if (key == sk_Clear) {
                if (numLevelPacks > 0) setView(VIEW_TITLE);
                else toExit = true;
                return;
            }

            if (key == sk_2nd) {
                loadLevel(levelIndex);
                setView(VIEW_GAME);
            }

            break;

        case VIEW_TITLE:
            switch (key) {
                case sk_1:
                    loadLevelPack(0);
                    return;
                case sk_2:
                    loadLevelPack(1);
                    return;
                case sk_3:
                    loadLevelPack(2);
                    return;
                case sk_4:
                    loadLevelPack(3);
                    return;
                case sk_5:
                    loadLevelPack(4);
                    return;
                case sk_6:
                    loadLevelPack(5);
                    return;
                case sk_7:
                    loadLevelPack(6);
                    return;
                case sk_8:
                    loadLevelPack(7);
                    return;
                case sk_Clear:
                    toExit = true;
                    return;
                default: break;
            }
            break;

        default:
            break;
    }

    if (changed) drawView();
}

void setView(view_t newView) {
    currentView = newView;

    switch (newView) {
        case VIEW_TITLE:
            findLevelPacks();
            if (numLevelPacks == 1) {
                loadLevelPack(0);
                return;
            }
            break;
        case VIEW_LEVELS:
            loadLevel(levelIndex);
            break;
        case VIEW_GAME:
            cursorRow = 0;
            cursorCol = 0;
            break;
        default:
            break;
    }

    drawView();
}

void findLevelPacks() {
    numLevelPacks = 0;
    char *var_name;
    void *vat_ptr = NULL;

    while (true) {
        var_name = ti_Detect(&vat_ptr, "\xEC\x08\xF9");
        if (var_name == NULL) break;

        uint8_t packHandle = ti_Open(var_name, "r");
        ti_Seek(3, SEEK_SET, packHandle);
        levelPackNames[numLevelPacks] = ti_GetDataPtr(packHandle);
        numLevelPacks++;

        ti_Close(packHandle);
    }
}

void listLevelPacks() {
    for (uint8_t i = 0; i < numLevelPacks; i++) {
        char * packName = levelPackNames[i];
        gfx_SetTextXY(LIST_X, LIST_Y + LIST_DY * i);
        gfx_PrintUInt(i + 1, 1);
        gfx_PrintString(". ");
        gfx_PrintString(packName);
    }
}

void loadLevelPack(uint8_t index) {
    if (index >= numLevelPacks) return;

    char *var_name;
    void *vat_ptr = NULL;

    for (uint8_t i = 0; i <= index; i++) {
        var_name = ti_Detect(&vat_ptr, "\xEC\x08\xF9");
    }

    uint8_t packHandle = ti_Open(var_name, "r");
    packStart = ti_GetDataPtr(packHandle);
    ti_Seek(3, SEEK_SET, packHandle);
    packTitle = ti_GetDataPtr(packHandle);

    for (packNumLevels = packTitle; *packNumLevels != 0; packNumLevels++);
    packNumLevels++;

    packToc = (uint16_t *)(packNumLevels + 1);

    ti_Close(packHandle);

    levelIndex = 0;
    setView(VIEW_LEVELS);
}

void loadLevel(uint8_t index) {
    levelTitle = packStart + packToc[index];

    // load previously saved wall configuration
    uint8_t *levelMap = levelTitle;
    while (*levelMap != 0x00) levelMap++;
    levelMap++;
    for (uint8_t row = 0; row < MAP_HEIGHT; row++) {
        uint8_t loadByte = levelMap[row];
        for (uint8_t col = 0; col < MAP_WIDTH; col++) {
            map[row][col] = (loadByte & 0x01) ? (cell_t){ENTITY_WALL, 0} : (cell_t){ENTITY_NONE, 0};
            loadByte >>= 1;
        }
    }

    colTargets = levelMap + MAP_HEIGHT;
    rowTargets = colTargets + MAP_WIDTH;

    // load monsters and chests
    uint8_t *read_ptr;
    for (read_ptr = rowTargets + MAP_HEIGHT; *read_ptr != 0xff; read_ptr += 3) {
        map[read_ptr[0]][read_ptr[1]] = (cell_t){ENTITY_MONSTER, read_ptr[2]};
    }
    for (read_ptr++; *read_ptr != 0xff; read_ptr += 2) {
        map[read_ptr[0]][read_ptr[1]] = (cell_t){ENTITY_CHEST, 0};
    }
}

void saveLevel() {
    uint8_t *levelMap = packStart + packToc[levelIndex];
    while (*levelMap != 0x00) levelMap++;
    levelMap++;
    for (uint8_t row = 0; row < MAP_HEIGHT; row++) {
        levelMap[row] = 0x00;
        for (uint8_t col = 0; col < MAP_WIDTH; col++) {
            levelMap[row] >>= 1;
            if (map[row][col].contents == ENTITY_WALL) levelMap[row] |= 0x80;
        }
    }
}