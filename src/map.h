#ifndef MAP_H
#define MAP_H

#define MAP_WIDTH 8
#define MAP_HEIGHT 8

typedef enum {
    ENTITY_NONE,
    ENTITY_WALL,
    ENTITY_FLAG,
    ENTITY_CHEST,
    ENTITY_MONSTER
} entity_t;

typedef struct {
    entity_t contents : 3;
    uint8_t variant : 5;
} cell_t;

extern cell_t map[MAP_HEIGHT][MAP_WIDTH];

cell_t getCell(uint8_t row, uint8_t col);

uint8_t wallsInCol(uint8_t col);
uint8_t wallsInRow(uint8_t row);

#endif