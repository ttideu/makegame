#include "game.h"

int gameRunning = 0;

void Update() {
    if (_kbhit()) {
        char input = _getch();
        int newPlayerX = playerX;
        int newPlayerY = playerY;

        switch (input) {
        case 'w': newPlayerY--; break;
        case 's': newPlayerY++; break;
        case 'a': newPlayerX--; break;
        case 'd': newPlayerX++; break;
        case 27: gameRunning = 0; break;
        }

        if (map[newPlayerY][newPlayerX] == 0) {
            playerX = newPlayerX;
            playerY = newPlayerY;
        }

        if (playerX == exitX && playerY == exitY) {
            gameRunning = 0;
            gotoxy(0, MAP_HEIGHT + 1);
            printf("탈출했습니다!\n");
            Sleep(1000);
        }
    }

    DrawPlayer();
}

void GameLoop() {
    gameRunning = 1;
    while (gameRunning) {
        ClearScreen();
        DrawMap();
        DrawExitPoint();

        Update();
        Sleep(100);
    }
}

void TitleLoop() {
    while (1) {
        ClearScreen();
        DrawTitleScreen();

        for (int i = 0; i < NUM_DROPS; i++) {
            if (!waveActive[i]) {
                DrawDrop(dropX[i], dropY[i]);
                if (dropY[i] < SCREEN_HEIGHT) {
                    dropY[i]++;
                }
                else {
                    waveActive[i] = 1;
                    dropY[i] = 0;
                }
            }

            if (waveActive[i]) {
                if (waveRadius[i] > 0) {
                    ClearPrints(dropX[i], SCREEN_HEIGHT, waveRadius[i] - 1);
                }

                DrawPrints(dropX[i], SCREEN_HEIGHT, waveRadius[i]);
                waveRadius[i]++;

                if (waveRadius[i] > 3) {
                    waveActive[i] = 0;
                    waveRadius[i] = 0;
                }
            }
        }

        Sleep(200);

        if (_kbhit()) {
            char key = _getch();
            if (key == 's') {
                gameRunning = 1;
                GameLoop();
            }
            else if (key == 27) {
                printf("\n\n\n\n\n\n\n\n\n게임이 종료되었습니다.\n");
                break;
            }
            else if (key == 'h') {
                printf("\n\n\n\n\n\n\n\n\n\n게임 도움말:\n");
                printf("ESC: 게임 종료\n");
                printf("플레이어는 'P'로 표시되며,'E'로 표시된 탈출 지점에 도달해야 합니다.\n");
                printf("이동: W(위), S(아래), A(왼쪽), D(오른쪽)\n");
                Sleep(2000);
            }
        }
    }
}