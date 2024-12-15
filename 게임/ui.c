#include "ui.h"
#include "map.h"
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <math.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")


int dropX[NUM_DROPS] = { 9, 24, 51 };
int dropY[NUM_DROPS] = { 3, 1, 6 };
int waveRadius[NUM_DROPS] = { 0, 0, 0 };
int waveActive[NUM_DROPS] = { 0, 0, 0 };

int playerX = 1;
int playerY = 1;
int hasKey = 0; // ���踦 ȹ���ߴ��� ���� (0: ��ȹ��, 1: ȹ��)
int enemyMoveTimer = 0; // �̵� Ÿ�̸�
int enemySpeed = 5; // 5 �����Ӹ��� �̵�
int firstRun = 1; // ���� ���� �� ó�� ���� ���θ� �Ǵ��ϴ� ����
int warningTimer = 0; // ��� Ÿ�̸�

char buffer[MAP_HEIGHT][MAP_WIDTH + 1]; // +1 -> null ����

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
    printf("          s. ���� ����          h. ����          esc. ����          \n");
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
    char rain = '*';  // ������ '*'���� ǥ��
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

int movingEnemyCount = 0; // �ʱ�ȭ

void InitializeMovingEnemies() {
    movingEnemyCount = 0; // �ʱ�ȭ

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == 7 && movingEnemyCount < MAX_MOVING_ENEMIES) {
                movingEnemyList[movingEnemyCount] = (MovingEnemy){ x, y, 1, y }; // �ʱ� �� ����
                map[y][x] = 0; // �ʿ��� �� ���� (�� �����Ͱ� �ߺ����� �ʵ���)
                movingEnemyCount++;
            }
        }
    }
}


void UpdateAllMovingEnemies() {
    for (int i = 0; i < movingEnemyCount; i++) {
        MovingEnemy* enemy = &movingEnemyList[i];

        // ���� ��ġ�� 0���� �ʱ�ȭ�Ͽ� �ܻ� ����
        map[enemy->y][enemy->x] = 0;

        int nextY = enemy->y + enemy->direction;

        // �̵� ������ �ʰ��ϰų� ��ֹ��� �ε����� ���� ����
        if (nextY < enemy->initialY || nextY > enemy->initialY + 8 || map[nextY][enemy->x] == 1) {
            enemy->direction *= -1;  // ���� ��ȯ
        }
        else {
            enemy->y = nextY; // �� �̵�
        }

        // ���ο� ��ġ�� ���� �ݿ�
        map[enemy->y][enemy->x] = 7;
    }
}

int calculateDistance(int x1, int y1, int x2, int y2) {
    return (int)sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

int isInEnemyArea = 0; // �÷��̾ ���� ������ �ִ��� ����
void walkSounds() {
    if (isInEnemyArea) {
        // ���� ���� �������� �߰��� �Ҹ� ȣ�� �� ��
        return;
    }

    int nearTrap = 0;
    int nearKey = 0;
    int nearHatch = 0;

    // �÷��̾� �ֺ��� ������ ���踦 Ȯ��
    for (int dx = -3; dx <= 3; dx++) {
        for (int dy = -3; dy <= 3; dy++) {
            int checkX = playerX + dx;
            int checkY = playerY + dy;

            // �� ���� ������ Ȯ��
            if (checkX >= 0 && checkX < MAP_WIDTH && checkY >= 0 && checkY < MAP_HEIGHT) {
                if (map[checkY][checkX] == 2) { // 2�� ����(����)
                    nearTrap = 1;
                }
                else if (map[checkY][checkX] == 3) { // 3�� ����
                    nearKey = 1;
                }

                else if (map[checkY][checkX] == 4 || map[checkY][checkX] == 5) {
                    nearHatch = 1;
                }

                // �� ������ ��� �����ϸ� Ž�� �ߴ�
                if (nearTrap && nearKey && nearHatch) {
                    break;
                }
            }
        }
    }

    // �Ҹ� ���: ���� > ���� > �Ϲ� �߼Ҹ� �켱����
    if (nearTrap) {
        PlaySound(TEXT("trap.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
    else if (nearKey && !hasKey) {
        PlaySound(TEXT("nearKey.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
    else if (nearHatch) {
        PlaySound(TEXT("hatch.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
    else {
        PlaySound(TEXT("walk.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
}


int isSoundPlayed = 0; // �Ҹ��� �̹� ��� ������ Ȯ���ϴ� �÷���

int isPlayerNearEnemy = 0;
int wasPlayerNearEnemy = 0; // ������ �� ��ó�� �־����� �����ϴ� ����

// ���� 4x4 ������ ���Դ��� Ȯ���ϰ� �Ҹ� ���
void checkEnemyProximity() {
    int playerIsNearEnemy = 0; // ���� �÷��̾ �� ��ó�� �ִ��� ����

    // �� ��ü�� ��ȸ�Ͽ� ������ ���� ��ġ�� Ȯ��
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == 6 || map[y][x] == 7) { // ������ ��
                int distanceToFixedEnemy = calculateDistance(playerX, playerY, x, y);

                // ���� ���� ���� �÷��̾ ���Դ��� Ȯ��
                if (distanceToFixedEnemy <= 4) {
                    playerIsNearEnemy = 1;
                    break;
                }
            }
        }
        if (playerIsNearEnemy) break; // �̹� �� ��ó�� ������ Ž�� �ߴ�
    }

    // �÷��̾ �� ��ó�� ������ ���� �Ҹ� ���
    if (playerIsNearEnemy && !wasPlayerNearEnemy) {
        PlaySound(TEXT("enemy.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }

    // �� ������ ������ �߰��� �Ҹ� ����
    if (playerIsNearEnemy) {
        isInEnemyArea = 1; // �� ������ ����
    }
    else {
        isInEnemyArea = 0; // �� ������ �����
    }

    // ���� ���¸� ������Ʈ
    wasPlayerNearEnemy = playerIsNearEnemy;
}

void SetConsoleSize(int width, int height) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // ���� ũ�⸦ ���� ����
    COORD coord;
    coord.X = width;
    coord.Y = height;
    SetConsoleScreenBufferSize(hConsole, coord);

    // â ũ�⸦ ����
    SMALL_RECT sr;
    sr.Left = 0;
    sr.Top = 0;
    sr.Right = width - 1;
    sr.Bottom = height - 1;
    SetConsoleWindowInfo(hConsole, TRUE, &sr);
}

void ResetGame() {
    playerX = 1; // �ʱ� �÷��̾� ��ġ
    playerY = 1;
    hasKey = 0; // ���� ���� �ʱ�ȭ
}

void ClearBuffer() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            buffer[y][x] = ' '; // ��ĭ���� �ʱ�ȭ
        }
    }
}

void DrawVisibleMap() {
    // �÷��̾� �ֺ��� ���� �Ÿ�
    int viewDistance = 3;

    // ���� ���� �� ��Ҹ� �ٽ� �׸���
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            char currentChar = ' ';

            // �÷��̾� �ֺ� 5x5 ���� ǥ��
            if (x >= playerX - viewDistance && x <= playerX + viewDistance && y >= playerY - viewDistance && y <= playerY + viewDistance) {

            //if(1){
                if (map[y][x] == 1) {
                    currentChar = '#'; // ��
                }
                else if (map[y][x] == 2) {
                    currentChar = '&'; // ����
                }
                else if (map[y][x] == 3 && !hasKey) {
                    currentChar = 'K'; // ����
                }
                else if (map[y][x] == 4) {
                    currentChar = 'E'; // Ż�ⱸ
                }
                else if (map[y][x] == 5) {
                    currentChar = 'E'; // ������ Ż�ⱸ
                }
                else if (map[y][x] == 6) {
                    currentChar = 'M'; // ������ ��
                }
                else if (map[y][x] == 7) {
                    currentChar = 'M'; // �����̴� ��
                }
                else if (x == playerX && y == playerY) {
                    currentChar = 'P'; // �÷��̾�
                }
            }

            // ���� ���¿� ���Ͽ� ȭ�� ����
            if (buffer[y][x] != currentChar) {
                buffer[y][x] = currentChar; // ���� ������Ʈ
                gotoxy(x, y);
                switch (currentChar) {
                case '#': printf("#"); break; // ��

                //case '&': printf("\033[31m&\033[0m"); break; // ����
                //case 'K': printf("\033[33mK\033[0m"); break; // ����
                //case 'E': printf("\033[34mE\033[0m"); break; // Ż�ⱸ
                //case 'M': printf("\033[31mM\033[0m"); break; // ��

                case 'P': printf("P"); break; // �÷��̾�
                default: printf(" "); break; // �� ĭ
                }
            }
        }
    }
}


void wlsWkRmx() {
    printf("\n");
    printf("                                                                            \n");
    printf("                                                                            \n");
    printf("                                                                            \n");
    printf("                                                                            \n");
    printf("                                                                            \n");
    printf("                                                                            \n");
    printf("                                        \\   |   /                          \n");
    printf("                                      \".   ' '   .\"                       \n");
    printf("                                        '\\     /'                          \n");
    printf("                                      - - -( )- - -                         \n");
    printf("                                        .'     '.                           \n");
    printf("                                      .'         '.                         \n");
    printf("                                        /   |   \\                          \n");
    printf("============================================================================\n");
    printf("                                                                            \n");
    printf("                                     CLEAR                                  \n");
    printf("                                                                            \n");
    printf("============================================================================\n");
    printf("                     ^                    /   \\                            \n");
    printf("                    / \\_    ^            /     \\                          \n");
    printf("                   /    \\__/ \\_       __/       \\__                      \n");
    printf("                __/            \\   __/               \\__                  \n");
    printf("               /                 \\/                     \\                 \n");
    printf("              /                                           \\                \n");
    printf("           __/                                              \\__            \n");
    printf("          /                                                     \\          \n");
    printf("             ____                                     ____                  \n");
    printf("            |    |                                   |    |                 \n");
    printf("            |    |               ____                |    |   ____          \n");
    printf("  ____      |    |__    ____    |    |____      ____ |    |  |    |         \n");
    printf(" |    |_____|    |  |  |    |   |         |    |    ||    |__|    |         \n");
    printf(" |         _     |  |  |    |   |         |____|    ||                 ____ \n");
    printf(" |   ____ | |    |  |  |    |   |     ____      ____||   ____         |    |\n");
    printf(" |___|  |_|_|____|__|__|____|___|____|    |____|    ||___|  |_________|____|\n");
    printf("\n");
}