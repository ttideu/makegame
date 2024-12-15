#ifndef UI_H
#define UI_H

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 40
#define NUM_DROPS 3
#define PLAYER 'P'
#define EXIT_POINT 'E'
#define ENEMY 'M'  // 적을 'M'로 표시
#define KEY 'K' // 열쇠
#define CLIFF '&'  // 함정을 표시할 문자

#define MAX_MOVING_ENEMIES 10

typedef struct {
    int x, y;       // 적의 현재 위치
    int direction;  // 이동 방향 (1: 아래, -1: 위)
    int initialX;   // 초기 X좌표 (범위 계산용)
    int initialY;   // 초기 Y 위치
} MovingEnemy;

MovingEnemy movingEnemyList[MAX_MOVING_ENEMIES]; // 움직이는 적 배열
extern MovingEnemy movingEnemy; // 움직이는 적 변수

extern int playerX, playerY, exitX, exitY;
extern int dropX[], dropY[], waveRadius[], waveActive[];
extern int playerX, playerY, exitX, exitY;
extern int hasKey;
extern int currentMapIndex;
extern int movingEnemyCount;

void gotoxy(int x, int y);
void ClearScreen();
void DrawTitleScreen();
void ClearPrints(int x, int y, int radius);
void DrawPrints(int x, int y, int radius);
void DrawDrop(int x, int y);
void DrawPlayer();
void ClearPlayer(int x, int y);
void ResetGame();
void DrawVisibleMap();
void SetConsoleSize(int width, int height);
void walkSounds();
int calculateDistance(int x1, int y1, int x2, int y2);
void checkEnemyProximity();
void ClearBuffer();
void wlsWkRmx();

#endif