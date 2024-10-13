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
int enemyX = 10;  // 고정형 적의 X 좌표
int enemyY = 5;   // 고정형 적의 Y 좌표
int keyX = 10; // 열쇠의 X 좌표
int keyY = 10; // 열쇠의 Y 좌표
int hasKey = 0; // 열쇠를 획득했는지 여부 (0: 미획득, 1: 획득)
int enemyMoveTimer = 0; // 이동 타이머
int enemySpeed = 5; // 5 프레임마다 이동
int firstRun = 1; // 게임 시작 시 처음 실행 여부를 판단하는 변수
int warningTimer = 0; // 경고 타이머

MovingEnemy movingEnemy = { 20, 8, 1 }; // 초기 위치 (20, 8) / 방향 (아래로)

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

/*
void checkCliff(int playerX, int playerY) {
    int warning = 0; // 경고 메시지가 출력되었는지 여부를 추적하는 변수
    for (int dx = -3; dx <= 3; dx++) {
        for (int dy = -3; dy <= 3; dy++) {
            int checkX = playerX + dx;
            int checkY = playerY + dy;

            // 맵 범위를 벗어나지 않는지 확인
            if (checkX >= 0 && checkX < MAP_WIDTH && checkY >= 0 && checkY < MAP_HEIGHT) {
                if (map[checkY][checkX] == 2) { // 함정에 가까이 가면 경고 메시지 출력
                    warning = 1; // 경고 메시지 출력 플래그 설정
                    gotoxy(0, MAP_HEIGHT + 2);
                    printf("근처에 함정이 있습니다!            "); // 메시지 출력, 덮어쓰기 방지
                    break; // 경고 메시지를 출력하고 반복문 종료
                }
            }
        }
    }

    // 경고 메시지가 출력되지 않았다면, 이전 메시지를 지움
    if (warning == 0) {
        gotoxy(0, MAP_HEIGHT + 2); // 화면 하단으로 이동
        printf("                                           \r"); 
        // 공백을 출력하여 이전 메시지를 지움
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
    enemyMoveTimer++; // 타이머 증가

    if (enemyMoveTimer >= enemySpeed) { // 설정한 속도에 따라 이동
        // 적이 다음으로 이동할 위치 계산
        int nextY = movingEnemy.y + movingEnemy.direction;

        // 적이 초기 위치에서 8칸 범위 내에 있는지 확인하고 벽이 있는지 확인
        if ((nextY < movingEnemy.initialY || nextY > movingEnemy.initialY + 8) || map[nextY][movingEnemy.x] == 1) {
            // 범위를 벗어나거나 벽에 부딪히면 방향 변경
            movingEnemy.direction *= -1;
        }
        else {
            // 범위 내에 있고 벽이 없으면 이동
            movingEnemy.y = nextY;
        }

        enemyMoveTimer = 0; // 타이머 초기화
    }
}

int calculateDistance(int x1, int y1, int x2, int y2) {
    return (int)sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}



void walkSounds() {
    int nearTrap = 0;

    // 플레이어가 함정 근처에 있는지 확인
    for (int dx = -3; dx <= 3; dx++) {
        for (int dy = -3; dy <= 3; dy++) {
            int checkX = playerX + dx;
            int checkY = playerY + dy;

            // 맵 범위 내인지 확인
            if (checkX >= 0 && checkX < MAP_WIDTH && checkY >= 0 && checkY < MAP_HEIGHT) {
                if (map[checkY][checkX] == 2) { // 2는 함정(절벽)
                    nearTrap = 1;
                    break;
                }
            }
        }
    }

    // 함정 근처에 있으면 함정 소리, 아니면 일반 발소리 재생
    if (nearTrap) {
        PlaySound(TEXT("trap.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
    else {
        PlaySound(TEXT("walk.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
}


int isSoundPlayed = 0; // 소리가 이미 재생 중인지 확인하는 플래그
// 적의 5x5 범위에 들어왔는지 확인하고 소리 재생
void checkEnemyProximity() {
    int distanceToFixedEnemy = calculateDistance(playerX, playerY, enemyX, enemyY);
    int distanceToMovingEnemy = calculateDistance(playerX, playerY, movingEnemy.x, movingEnemy.y);

    // 적의 범위 내에 플레이어가 들어왔는지 확인
    if ((distanceToFixedEnemy <= 5 || distanceToMovingEnemy <= 5) && !isSoundPlayed) {
        PlaySound(TEXT("enemy.wav"), NULL, SND_FILENAME | SND_ASYNC);
        isSoundPlayed = 1;  // 소리가 재생되면 플래그 설정
    }
    // 적의 범위를 벗어났다면 플래그를 초기화
    else if (distanceToFixedEnemy > 5 && distanceToMovingEnemy > 5) {
        isSoundPlayed = 0;  // 적의 범위 밖으로 나가면 플래그 초기화
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

    // 버퍼 크기도 조정
    COORD coord;
    coord.X = width;
    coord.Y = height;
    SetConsoleScreenBufferSize(hConsole, coord);
}

void ResetGame() {
    playerX = 1; // 초기 플레이어 위치
    playerY = 1;
    hasKey = 0; // 열쇠 상태 초기화
}

void DrawVisibleMap() {

    // 이전 상태와 비교해 달라진 부분만 출력
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            char currentChar = ' ';

            // 플레이어 주변 5x5 영역만 표시
            if (x >= playerX - 2 && x <= playerX + 2 && y >= playerY - 2 && y <= playerY + 2) {
            //if (1) {
                if (map[y][x] == 1) {
                    currentChar = '#'; // 벽
                }
                else if (map[y][x] == 2) {
                    currentChar = '&'; // 함정
                }
                else if (x == keyX && y == keyY && !hasKey) {
                    currentChar = 'K'; // 열쇠 (획득하지 않았을 때만 보임)
                }
                else if (x == exitX && y == exitY) {
                    currentChar = 'E'; // 탈출구
                }
                else if (x == enemyX && y == enemyY) {
                    currentChar = 'M'; // 적
                }

                // 움직이는 적의 위치를 체크하여 currentChar에 반영
                if (x == movingEnemy.x && y == movingEnemy.y) {
                    currentChar = 'M'; // 움직이는 적은 'M'로 표시
                }

                // 플레이어 위치
                if (x == playerX && y == playerY) {
                    currentChar = 'P';
                }
            }

            // 이전 상태와 비교하여 달라진 부분만 출력
            if (buffer[y][x] != currentChar) {
                buffer[y][x] = currentChar; // 버퍼 업데이트
                gotoxy(x, y); // 해당 좌표로 이동하여
                switch (currentChar) {
                case '#':
                    printf("#"); // 벽
                    break;
                case '&':
                    printf("\033[31m&\033[0m"); // 함정
                    break;
                case 'K':
                    printf("\033[33mK\033[0m"); // 열쇠는 노란색
                    break;
                case 'E':
                    printf("\033[34mE\033[0m"); // 탈출구는 파란색
                    break;
                case 'M':
                    printf("\033[31mM\033[0m"); // 적은 빨간색
                    break;
                case 'P':
                    printf("P"); // 플레이어
                    break;
                default:
                    printf(" "); // 빈 공간
                    break;
                }
            }
        }
    }
}