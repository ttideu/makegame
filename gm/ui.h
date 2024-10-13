#ifndef UI_H
#define UI_H

#define MAP_WIDTH 80
#define MAP_HEIGHT 40
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 40
#define NUM_DROPS 3
#define PLAYER 'P'
#define EXIT_POINT 'E'
#define ENEMY 'M'  // 적을 'M'로 표시
#define KEY 'K' // 열쇠
#define CLIFF '&'  // 함정을 표시할 문자

typedef struct {
    int x;           // X 좌표
    int y;           // Y 좌표
    int direction;   // 방향 (1: 아래, -1: 위)
    int initialY;    // 초기 Y좌표
} MovingEnemy;

extern int playerX, playerY, exitX, exitY, map[MAP_HEIGHT][MAP_WIDTH];
extern int dropX[], dropY[], waveRadius[], waveActive[];
extern int playerX, playerY, exitX, exitY;
extern int enemyX, enemyY;  // 적의 좌표
extern int keyX, keyY, hasKey;
extern int map[MAP_HEIGHT][MAP_WIDTH];
extern MovingEnemy movingEnemy; // 움직이는 적 변수

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
void UpdateMovingEnemy();
void SetConsoleSize(int width, int height);
//void checkCliff(int playerX, int playerY);
void walkSounds();
int calculateDistance(int x1, int y1, int x2, int y2);
void checkEnemyProximity();

#endif