#ifndef GAME_H
#define GAME_H

#include <graphx.h>

#define MAX_PACKS 8

extern bool toExit;

typedef enum {
    VIEW_TITLE,
    VIEW_CREDITS,
    VIEW_LEVELS,
    VIEW_GAME
} view_t;

extern view_t currentView;

extern uint8_t cursorRow;
extern uint8_t cursorCol;

extern uint8_t *rowTargets;
extern uint8_t *colTargets;

extern uint8_t numLevelPacks;
extern char *levelPackNames[MAX_PACKS];

extern char *packTitle;
extern uint8_t *packNumLevels;
extern uint8_t levelIndex;
extern char *levelTitle;

void doInput();

void setView(view_t newView);

void findLevelPacks();
void listLevelPacks();
void loadLevelPack(uint8_t index);
void loadLevel(uint8_t index);
void saveLevel();

#endif