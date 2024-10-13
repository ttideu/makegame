#include "ui.h"
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
int exitX = 78;
int exitY = 38;
int enemyX = 10;  // ������ ���� X ��ǥ
int enemyY = 5;   // ������ ���� Y ��ǥ
int keyX = 10; // ������ X ��ǥ
int keyY = 10; // ������ Y ��ǥ
int hasKey = 0; // ���踦 ȹ���ߴ��� ���� (0: ��ȹ��, 1: ȹ��)
int enemyMoveTimer = 0; // �̵� Ÿ�̸�
int enemySpeed = 5; // 5 �����Ӹ��� �̵�
int firstRun = 1; // ���� ���� �� ó�� ���� ���θ� �Ǵ��ϴ� ����
int warningTimer = 0; // ��� Ÿ�̸�

MovingEnemy movingEnemy = { 20, 8, 1 }; // �ʱ� ��ġ (20, 8) / ���� (�Ʒ���)

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

/*
void checkCliff(int playerX, int playerY) {
    int warning = 0; // ��� �޽����� ��µǾ����� ���θ� �����ϴ� ����
    for (int dx = -3; dx <= 3; dx++) {
        for (int dy = -3; dy <= 3; dy++) {
            int checkX = playerX + dx;
            int checkY = playerY + dy;

            // �� ������ ����� �ʴ��� Ȯ��
            if (checkX >= 0 && checkX < MAP_WIDTH && checkY >= 0 && checkY < MAP_HEIGHT) {
                if (map[checkY][checkX] == 2) { // ������ ������ ���� ��� �޽��� ���
                    warning = 1; // ��� �޽��� ��� �÷��� ����
                    gotoxy(0, MAP_HEIGHT + 2);
                    printf("��ó�� ������ �ֽ��ϴ�!            "); // �޽��� ���, ����� ����
                    break; // ��� �޽����� ����ϰ� �ݺ��� ����
                }
            }
        }
    }

    // ��� �޽����� ��µ��� �ʾҴٸ�, ���� �޽����� ����
    if (warning == 0) {
        gotoxy(0, MAP_HEIGHT + 2); // ȭ�� �ϴ����� �̵�
        printf("                                           \r"); 
        // ������ ����Ͽ� ���� �޽����� ����
    }
}
*/

void DrawPlayer() {
    gotoxy(playerX, playerY);
    printf("%c", PLAYER);
}

void ClearPlayer(int x, int y) {
    gotoxy(x, y);
    printf(" ");
}

void UpdateMovingEnemy() {
    enemyMoveTimer++; // Ÿ�̸� ����

    if (enemyMoveTimer >= enemySpeed) { // ������ �ӵ��� ���� �̵�
        // ���� �������� �̵��� ��ġ ���
        int nextY = movingEnemy.y + movingEnemy.direction;

        // ���� �ʱ� ��ġ���� 8ĭ ���� ���� �ִ��� Ȯ���ϰ� ���� �ִ��� Ȯ��
        if ((nextY < movingEnemy.initialY || nextY > movingEnemy.initialY + 8) || map[nextY][movingEnemy.x] == 1) {
            // ������ ����ų� ���� �ε����� ���� ����
            movingEnemy.direction *= -1;
        }
        else {
            // ���� ���� �ְ� ���� ������ �̵�
            movingEnemy.y = nextY;
        }

        enemyMoveTimer = 0; // Ÿ�̸� �ʱ�ȭ
    }
}

int calculateDistance(int x1, int y1, int x2, int y2) {
    return (int)sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}



void walkSounds() {
    int nearTrap = 0;

    // �÷��̾ ���� ��ó�� �ִ��� Ȯ��
    for (int dx = -3; dx <= 3; dx++) {
        for (int dy = -3; dy <= 3; dy++) {
            int checkX = playerX + dx;
            int checkY = playerY + dy;

            // �� ���� ������ Ȯ��
            if (checkX >= 0 && checkX < MAP_WIDTH && checkY >= 0 && checkY < MAP_HEIGHT) {
                if (map[checkY][checkX] == 2) { // 2�� ����(����)
                    nearTrap = 1;
                    break;
                }
            }
        }
    }

    // ���� ��ó�� ������ ���� �Ҹ�, �ƴϸ� �Ϲ� �߼Ҹ� ���
    if (nearTrap) {
        PlaySound(TEXT("trap.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
    else {
        PlaySound(TEXT("walk.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
}


int isSoundPlayed = 0; // �Ҹ��� �̹� ��� ������ Ȯ���ϴ� �÷���
// ���� 5x5 ������ ���Դ��� Ȯ���ϰ� �Ҹ� ���
void checkEnemyProximity() {
    int distanceToFixedEnemy = calculateDistance(playerX, playerY, enemyX, enemyY);
    int distanceToMovingEnemy = calculateDistance(playerX, playerY, movingEnemy.x, movingEnemy.y);

    // ���� ���� ���� �÷��̾ ���Դ��� Ȯ��
    if ((distanceToFixedEnemy <= 5 || distanceToMovingEnemy <= 5) && !isSoundPlayed) {
        PlaySound(TEXT("enemy.wav"), NULL, SND_FILENAME | SND_ASYNC);
        isSoundPlayed = 1;  // �Ҹ��� ����Ǹ� �÷��� ����
    }
    // ���� ������ ����ٸ� �÷��׸� �ʱ�ȭ
    else if (distanceToFixedEnemy > 5 && distanceToMovingEnemy > 5) {
        isSoundPlayed = 0;  // ���� ���� ������ ������ �÷��� �ʱ�ȭ
    }
}

void SetConsoleSize(int width, int height) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT sr;
    sr.Left = 0;
    sr.Top = 0;
    sr.Right = width - 1;
    sr.Bottom = height - 1;
    SetConsoleWindowInfo(hConsole, TRUE, &sr);

    // ���� ũ�⵵ ����
    COORD coord;
    coord.X = width;
    coord.Y = height;
    SetConsoleScreenBufferSize(hConsole, coord);
}

void ResetGame() {
    playerX = 1; // �ʱ� �÷��̾� ��ġ
    playerY = 1;
    hasKey = 0; // ���� ���� �ʱ�ȭ
}

void DrawVisibleMap() {

    // ���� ���¿� ���� �޶��� �κи� ���
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            char currentChar = ' ';

            // �÷��̾� �ֺ� 5x5 ������ ǥ��
            if (x >= playerX - 2 && x <= playerX + 2 && y >= playerY - 2 && y <= playerY + 2) {
            //if (1) {
                if (map[y][x] == 1) {
                    currentChar = '#'; // ��
                }
                else if (map[y][x] == 2) {
                    currentChar = '&'; // ����
                }
                else if (x == keyX && y == keyY && !hasKey) {
                    currentChar = 'K'; // ���� (ȹ������ �ʾ��� ���� ����)
                }
                else if (x == exitX && y == exitY) {
                    currentChar = 'E'; // Ż�ⱸ
                }
                else if (x == enemyX && y == enemyY) {
                    currentChar = 'M'; // ��
                }

                // �����̴� ���� ��ġ�� üũ�Ͽ� currentChar�� �ݿ�
                if (x == movingEnemy.x && y == movingEnemy.y) {
                    currentChar = 'M'; // �����̴� ���� 'M'�� ǥ��
                }

                // �÷��̾� ��ġ
                if (x == playerX && y == playerY) {
                    currentChar = 'P';
                }
            }

            // ���� ���¿� ���Ͽ� �޶��� �κи� ���
            if (buffer[y][x] != currentChar) {
                buffer[y][x] = currentChar; // ���� ������Ʈ
                gotoxy(x, y); // �ش� ��ǥ�� �̵��Ͽ�
                switch (currentChar) {
                case '#':
                    printf("#"); // ��
                    break;
                case '&':
                    printf("\033[31m&\033[0m"); // ����
                    break;
                case 'K':
                    printf("\033[33mK\033[0m"); // ����� �����
                    break;
                case 'E':
                    printf("\033[34mE\033[0m"); // Ż�ⱸ�� �Ķ���
                    break;
                case 'M':
                    printf("\033[31mM\033[0m"); // ���� ������
                    break;
                case 'P':
                    printf("P"); // �÷��̾�
                    break;
                default:
                    printf(" "); // �� ����
                    break;
                }
            }
        }
    }
}