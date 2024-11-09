#include <stdlib.h>
#include <graphx.h>
#include "map.h"

cell_t map[MAP_HEIGHT][MAP_WIDTH];

cell_t getCell(uint8_t row, uint8_t col) {
    if (row >= MAP_HEIGHT || col >= MAP_WIDTH) {
        return (cell_t){ENTITY_WALL, 0};
    }
    else return map[row][col];
}

uint8_t wallsInRow(uint8_t row) {
    uint8_t numWalls = 0;
    for (uint8_t col = 0; col < MAP_WIDTH; col++) {
        if (map[row][col].contents == ENTITY_WALL) numWalls++;
    }
    return numWalls;
}

uint8_t wallsInCol(uint8_t col) {
    uint8_t numWalls = 0;
    for (uint8_t row = 0; row < MAP_HEIGHT; row++) {
        if (map[row][col].contents == ENTITY_WALL) numWalls++;
    }
    return numWalls;
}