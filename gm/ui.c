#include "ui.h"
#include <windows.h>
#include <conio.h>

int dropX[NUM_DROPS] = { 9, 24, 51 };
int dropY[NUM_DROPS] = { 3, 1, 6 };
int waveRadius[NUM_DROPS] = { 0, 0, 0 };
int waveActive[NUM_DROPS] = { 0, 0, 0 };

int playerX = 1;
int playerY = 1;
int exitX = 38;
int exitY = 13;

int map[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,0,1},
    {1,0,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1},
    {1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1},
    {1,0,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1},
    {1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,1},
    {1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1},
    {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1},
    {1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void gotoxy(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hConsole, coord);
}

void ClearScreen() {
    system("cls");
}

void DrawTitleScreen() {
    printf("\n\n\n");
    printf("                         ESCAPE FROM DARKROOM                         \n");
    printf("\n\n\n\n\n");
    printf("\n\n\n\n\n");
    printf("\n\n\n");
    printf("          s. 게임 시작          h. 도움말          esc. 종료          \n");
}

void ClearPrints(int x, int y, int radius) {
    for (int i = 1; i <= radius; i++) {
        gotoxy(x - i, y);         printf(" ");
        gotoxy(x + i, y);         printf(" ");
        gotoxy(x, y - i);         printf(" ");
        gotoxy(x, y + i);         printf(" ");
        gotoxy(x - i, y - i);     printf(" ");
        gotoxy(x + i, y - i);     printf(" ");
        gotoxy(x - i, y + i);     printf(" ");
        gotoxy(x + i, y + i);     printf(" ");
    }
}

void DrawPrints(int x, int y, int radius) {
    char rain = '*';  // 파장을 '*'으로 표시
    for (int i = 1; i <= radius; i++) {
        gotoxy(x - i, y);         printf("%c", rain);
        gotoxy(x + i, y);         printf("%c", rain);
        gotoxy(x, y - i);         printf("%c", rain);
        gotoxy(x, y + i);         printf("%c", rain);
        gotoxy(x - i, y - i);     printf("%c", rain);
        gotoxy(x + i, y - i);     printf("%c", rain);
        gotoxy(x - i, y + i);     printf("%c", rain);
        gotoxy(x + i, y + i);     printf("%c", rain);
    }
}

void DrawDrop(int x, int y) {
    gotoxy(x, y);
    printf("o");
}

void DrawPlayer() {
    gotoxy(playerX, playerY);
    printf("%c", PLAYER);
}

void ClearPlayer(int x, int y) {
    gotoxy(x, y);
    printf(" ");
}

void DrawExitPoint() {
    gotoxy(exitX, exitY);
    printf("%c", EXIT_POINT);
}

void DrawMap() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == 1) {
                gotoxy(x, y);
                printf("#");
            }
        }
    }
}