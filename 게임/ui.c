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
int hasKey = 0; // 열쇠를 획득했는지 여부 (0: 미획득, 1: 획득)
int enemyMoveTimer = 0; // 이동 타이머
int enemySpeed = 5; // 5 프레임마다 이동
int firstRun = 1; // 게임 시작 시 처음 실행 여부를 판단하는 변수
int warningTimer = 0; // 경고 타이머

char buffer[MAP_HEIGHT][MAP_WIDTH + 1]; // +1 -> null 문자

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

int movingEnemyCount = 0; // 초기화

void InitializeMovingEnemies() {
    movingEnemyCount = 0; // 초기화

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == 7 && movingEnemyCount < MAX_MOVING_ENEMIES) {
                movingEnemyList[movingEnemyCount] = (MovingEnemy){ x, y, 1, y }; // 초기 값 설정
                map[y][x] = 0; // 맵에서 적 제거 (맵 데이터가 중복되지 않도록)
                movingEnemyCount++;
            }
        }
    }
}


void UpdateAllMovingEnemies() {
    for (int i = 0; i < movingEnemyCount; i++) {
        MovingEnemy* enemy = &movingEnemyList[i];

        // 이전 위치를 0으로 초기화하여 잔상 제거
        map[enemy->y][enemy->x] = 0;

        int nextY = enemy->y + enemy->direction;

        // 이동 범위를 초과하거나 장애물에 부딪히면 방향 변경
        if (nextY < enemy->initialY || nextY > enemy->initialY + 8 || map[nextY][enemy->x] == 1) {
            enemy->direction *= -1;  // 방향 전환
        }
        else {
            enemy->y = nextY; // 적 이동
        }

        // 새로운 위치에 적을 반영
        map[enemy->y][enemy->x] = 7;
    }
}

int calculateDistance(int x1, int y1, int x2, int y2) {
    return (int)sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

int isInEnemyArea = 0; // 플레이어가 적의 영역에 있는지 여부
void walkSounds() {
    if (isInEnemyArea) {
        // 적의 영역 내에서는 발걸음 소리 호출 안 함
        return;
    }

    int nearTrap = 0;
    int nearKey = 0;
    int nearHatch = 0;

    // 플레이어 주변의 함정과 열쇠를 확인
    for (int dx = -3; dx <= 3; dx++) {
        for (int dy = -3; dy <= 3; dy++) {
            int checkX = playerX + dx;
            int checkY = playerY + dy;

            // 맵 범위 내인지 확인
            if (checkX >= 0 && checkX < MAP_WIDTH && checkY >= 0 && checkY < MAP_HEIGHT) {
                if (map[checkY][checkX] == 2) { // 2는 함정(절벽)
                    nearTrap = 1;
                }
                else if (map[checkY][checkX] == 3) { // 3은 열쇠
                    nearKey = 1;
                }

                else if (map[checkY][checkX] == 4 || map[checkY][checkX] == 5) {
                    nearHatch = 1;
                }

                // 두 조건을 모두 만족하면 탐색 중단
                if (nearTrap && nearKey && nearHatch) {
                    break;
                }
            }
        }
    }

    // 소리 재생: 함정 > 열쇠 > 일반 발소리 우선순위
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


int isSoundPlayed = 0; // 소리가 이미 재생 중인지 확인하는 플래그

int isPlayerNearEnemy = 0;
int wasPlayerNearEnemy = 0; // 이전에 적 근처에 있었는지 추적하는 변수

// 적의 4x4 범위에 들어왔는지 확인하고 소리 재생
void checkEnemyProximity() {
    int playerIsNearEnemy = 0; // 현재 플레이어가 적 근처에 있는지 여부

    // 맵 전체를 순회하여 고정형 적의 위치를 확인
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == 6 || map[y][x] == 7) { // 고정형 적
                int distanceToFixedEnemy = calculateDistance(playerX, playerY, x, y);

                // 적의 범위 내에 플레이어가 들어왔는지 확인
                if (distanceToFixedEnemy <= 4) {
                    playerIsNearEnemy = 1;
                    break;
                }
            }
        }
        if (playerIsNearEnemy) break; // 이미 적 근처에 있으면 탐색 중단
    }

    // 플레이어가 적 근처에 들어왔을 때만 소리 재생
    if (playerIsNearEnemy && !wasPlayerNearEnemy) {
        PlaySound(TEXT("enemy.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }

    // 적 영역에 들어갔으면 발걸음 소리 끄기
    if (playerIsNearEnemy) {
        isInEnemyArea = 1; // 적 영역에 들어갔음
    }
    else {
        isInEnemyArea = 0; // 적 영역을 벗어났음
    }

    // 이전 상태를 업데이트
    wasPlayerNearEnemy = playerIsNearEnemy;
}

void SetConsoleSize(int width, int height) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // 버퍼 크기를 먼저 조정
    COORD coord;
    coord.X = width;
    coord.Y = height;
    SetConsoleScreenBufferSize(hConsole, coord);

    // 창 크기를 조정
    SMALL_RECT sr;
    sr.Left = 0;
    sr.Top = 0;
    sr.Right = width - 1;
    sr.Bottom = height - 1;
    SetConsoleWindowInfo(hConsole, TRUE, &sr);
}

void ResetGame() {
    playerX = 1; // 초기 플레이어 위치
    playerY = 1;
    hasKey = 0; // 열쇠 상태 초기화
}

void ClearBuffer() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            buffer[y][x] = ' '; // 빈칸으로 초기화
        }
    }
}

void DrawVisibleMap() {
    // 플레이어 주변의 가시 거리
    int viewDistance = 3;

    // 가시 범위 내 요소를 다시 그리기
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            char currentChar = ' ';

            // 플레이어 주변 5x5 영역 표시
            if (x >= playerX - viewDistance && x <= playerX + viewDistance && y >= playerY - viewDistance && y <= playerY + viewDistance) {

            //if(1){
                if (map[y][x] == 1) {
                    currentChar = '#'; // 벽
                }
                else if (map[y][x] == 2) {
                    currentChar = '&'; // 함정
                }
                else if (map[y][x] == 3 && !hasKey) {
                    currentChar = 'K'; // 열쇠
                }
                else if (map[y][x] == 4) {
                    currentChar = 'E'; // 탈출구
                }
                else if (map[y][x] == 5) {
                    currentChar = 'E'; // 마지막 탈출구
                }
                else if (map[y][x] == 6) {
                    currentChar = 'M'; // 고정형 적
                }
                else if (map[y][x] == 7) {
                    currentChar = 'M'; // 움직이는 적
                }
                else if (x == playerX && y == playerY) {
                    currentChar = 'P'; // 플레이어
                }
            }

            // 이전 상태와 비교하여 화면 갱신
            if (buffer[y][x] != currentChar) {
                buffer[y][x] = currentChar; // 버퍼 업데이트
                gotoxy(x, y);
                switch (currentChar) {
                case '#': printf("#"); break; // 벽

                //case '&': printf("\033[31m&\033[0m"); break; // 함정
                //case 'K': printf("\033[33mK\033[0m"); break; // 열쇠
                //case 'E': printf("\033[34mE\033[0m"); break; // 탈출구
                //case 'M': printf("\033[31mM\033[0m"); break; // 적

                case 'P': printf("P"); break; // 플레이어
                default: printf(" "); break; // 빈 칸
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