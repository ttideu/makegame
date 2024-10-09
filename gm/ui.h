#ifndef UI_H
#define UI_H

#define MAP_WIDTH 40
#define MAP_HEIGHT 20
#define SCREEN_WIDTH 60
#define SCREEN_HEIGHT 20
#define NUM_DROPS 3
#define PLAYER 'P'
#define EXIT_POINT 'E'

extern int dropX[], dropY[], waveRadius[], waveActive[];
extern int playerX, playerY, exitX, exitY;
extern int map[MAP_HEIGHT][MAP_WIDTH];

void gotoxy(int x, int y);
void ClearScreen();
void DrawTitleScreen();
void ClearPrints(int x, int y, int radius);
void DrawPrints(int x, int y, int radius);
void DrawDrop(int x, int y);
void DrawPlayer();
void ClearPlayer(int x, int y);
void DrawExitPoint();
void DrawMap();

#endif