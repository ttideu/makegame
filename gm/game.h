#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include "ui.h"
extern int playerX, playerY, exitX, exitY, map[MAP_HEIGHT][MAP_WIDTH];

void Update();
void GameLoop();
void TitleLoop();

#endif