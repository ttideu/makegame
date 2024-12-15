#include "game.h"
#include "ui.h"
#include "map.h"

int gameRunning = 0;

void Update() {
    

    if (_kbhit()) {
        char input = _getch();
        int newPlayerX = playerX;
        int newPlayerY = playerY;

        
        // 플레이어 이동 방향 결정
        switch (input) {
        case 'w': 
            newPlayerY--;
            walkSounds();
            break;
        case 's': 
            newPlayerY++;
            walkSounds();
            break;
        case 'a': 
            newPlayerX--;
            walkSounds();
            break;
        case 'd': 
            newPlayerX++;
            walkSounds();
            break;
        case 27: 
            gameRunning = 0; // ESC 키로 게임 종료
            break;
        //case 'n':
        //    gotoxy(0, MAP_HEIGHT + 1);
        //    printf("                                             \n");
        //    Sleep(500);
        //    NextMap(); // 다음 맵으로 이동
        //    ResetGame();
        //    break;
        }

        // 이동할 위치가 유효한지 확인
        if (   map[newPlayerY][newPlayerX] == 0 || map[newPlayerY][newPlayerX] == 2
            || map[newPlayerY][newPlayerX] == 3 || map[newPlayerY][newPlayerX] == 4
            || map[newPlayerY][newPlayerX] == 5 || map[newPlayerY][newPlayerX] == 6
            || map[newPlayerY][newPlayerX] == 7) {
            
            // 이전 위치 지우기
            ClearPlayer(playerX, playerY); // 현재 위치 지우기

            // 플레이어 위치 업데이트
            playerX = newPlayerX;
            playerY = newPlayerY;
        }

        // 플레이어가 적과 충돌했는지 확인
        if (map[playerY][playerX] == 6) {
            gameRunning = 0;
            gotoxy(0, MAP_HEIGHT + 1);
            printf("게임 오버!                                \n");
            ResetGame();
            Sleep(1500);
        }

        if (map[playerY][playerX] == 7) {
            gameRunning = 0;
            gotoxy(0, MAP_HEIGHT + 1);
            printf("게임 오버!                                \n");
            ResetGame();
            Sleep(1500);
        }

        //함정에 닿으면 게임 오버 처리
        if (map[playerY][playerX] == 2) {
            gameRunning = 0;
            gotoxy(0, MAP_HEIGHT + 1); // 화면 하단으로 이동
            printf("게임 오버! 함정에 당했습니다.                  \n");
            ResetGame();
            Sleep(1500);
        }

        // 열쇠를 획득했는지 확인
        if (map[playerY][playerX] == 3 && !hasKey) {
            hasKey = 1;
            gotoxy(0, MAP_HEIGHT + 1);
            printf("열쇠를 획득했습니다!                           \n");
            PlaySound(TEXT("key.wav"), NULL, SND_FILENAME | SND_ASYNC);
            Sleep(500);
        }

        // 탈출구에 도달했는지 확인
        if (map[playerY][playerX] == 4) {
            if (hasKey) { // 열쇠를 가지고 있을 때만 탈출 가능
                gotoxy(0, MAP_HEIGHT + 1);
                printf("                                             \n");
                Sleep(500);
                NextMap(); // 다음 맵으로 이동
                PlaySound(TEXT("exit.wav"), NULL, SND_FILENAME | SND_ASYNC);
                ResetGame();
                Sleep(2000);
            }
            else {
                gotoxy(0, MAP_HEIGHT + 1);
                printf("열쇠가 필요합니다!                           \n");
                PlaySound(TEXT("cant_exit.wav"), NULL, SND_FILENAME | SND_ASYNC);
                Sleep(1000);
            }
        }

        // 마지막 탈출구 충돌
        if (map[playerY][playerX] == 5) {
            if (hasKey) { // 열쇠를 가지고 있을 때만 탈출 가능
                gotoxy(0, MAP_HEIGHT + 1);
                printf("                                             \n");
                
                PlaySound(TEXT("exit.wav"), NULL, SND_FILENAME | SND_ASYNC);
                Sleep(1000);
                currentMapIndex = 0;
                ResetGame();
                clearLoop();
            }
            else {
                gotoxy(0, MAP_HEIGHT + 1);
                printf("열쇠가 필요합니다!                           \n");
                PlaySound(TEXT("cant_exit.wav"), NULL, SND_FILENAME | SND_ASYNC);
                Sleep(1000);
            }
        }

        checkEnemyProximity(); // 적 소리 재생
        // 새로운 위치에 플레이어 그리기
        DrawPlayer();

    }
}

void GameLoop() {
    ClearBuffer();
    gameRunning = 1;
    LoadMap(currentMapIndex);
    InitializeMovingEnemies();

    while (gameRunning) {
        Update();
        UpdateAllMovingEnemies();
        DrawVisibleMap();
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
                if (dropY[i] < SCREEN_HEIGHT / 2) {
                    dropY[i]++;
                }
                else {
                    waveActive[i] = 1;
                    dropY[i] = 0;

                    PlaySound(TEXT("water_drop.wav"), NULL, SND_FILENAME | SND_ASYNC);
                }
            }

            if (waveActive[i]) {
                if (waveRadius[i] > 0) {
                    ClearPrints(dropX[i], SCREEN_HEIGHT / 2, waveRadius[i] - 1);
                }

                DrawPrints(dropX[i], SCREEN_HEIGHT / 2, waveRadius[i]);
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
                ClearScreen();
                gameRunning = 1;
                GameLoop();
            }
            else if (key == 27) {
                gotoxy(0, MAP_HEIGHT + 1);
                printf("게임이 종료되었습니다.       \n");
                break;
            }
            else if (key == 'h') {
                gotoxy(0, MAP_HEIGHT + 1);
                printf("게임 도움말:\n");
                printf("게임 종료 : ESC\n");
                printf("적과 함정을 피해 탈출 지점에 도달해야 합니다.\n");
                printf("적, 함정, 열쇠, 탈출구 주위에는 고유의 소리가 들립니다.이것에 집중해주세요.\n");
                printf("이동: W, A, S, D\n");
                Sleep(3000);
            }
            /*
            else if (key == 'm') {
                ClearScreen();
                clearLoop();
            }*/
        }
    }
}

void clearLoop() {
    gameRunning = 2;
    ClearScreen();

    while (gameRunning == 2) {
        ClearScreen();
        wlsWkRmx();
        Sleep(200);

        if (_kbhit()) {
            char input = _getch();

            switch (input) {
            case 27:
                gameRunning = 0; // ESC 키로 게임 종료
                break;
            }
        }

    }

}