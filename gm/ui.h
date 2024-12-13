#ifndef UI_H
#define UI_H

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 40
#define NUM_DROPS 3
#define PLAYER 'P'
#define EXIT_POINT 'E'
#define ENEMY 'M'  // ���� 'M'�� ǥ��
#define KEY 'K' // ����
#define CLIFF '&'  // ������ ǥ���� ����

#define MAX_MOVING_ENEMIES 10

typedef struct {
    int x, y;       // ���� ���� ��ġ
    int direction;  // �̵� ���� (1: �Ʒ�, -1: ��)
    int initialX;   // �ʱ� X��ǥ (���� ����)
    int initialY;   // �ʱ� Y ��ġ
} MovingEnemy;

MovingEnemy movingEnemyList[MAX_MOVING_ENEMIES]; // �����̴� �� �迭
extern MovingEnemy movingEnemy; // �����̴� �� ����

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