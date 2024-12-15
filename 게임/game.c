#include "game.h"
#include "ui.h"
#include "map.h"

int gameRunning = 0;

void Update() {
    

    if (_kbhit()) {
        char input = _getch();
        int newPlayerX = playerX;
        int newPlayerY = playerY;

        
        // �÷��̾� �̵� ���� ����
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
            gameRunning = 0; // ESC Ű�� ���� ����
            break;
        //case 'n':
        //    gotoxy(0, MAP_HEIGHT + 1);
        //    printf("                                             \n");
        //    Sleep(500);
        //    NextMap(); // ���� ������ �̵�
        //    ResetGame();
        //    break;
        }

        // �̵��� ��ġ�� ��ȿ���� Ȯ��
        if (   map[newPlayerY][newPlayerX] == 0 || map[newPlayerY][newPlayerX] == 2
            || map[newPlayerY][newPlayerX] == 3 || map[newPlayerY][newPlayerX] == 4
            || map[newPlayerY][newPlayerX] == 5 || map[newPlayerY][newPlayerX] == 6
            || map[newPlayerY][newPlayerX] == 7) {
            
            // ���� ��ġ �����
            ClearPlayer(playerX, playerY); // ���� ��ġ �����

            // �÷��̾� ��ġ ������Ʈ
            playerX = newPlayerX;
            playerY = newPlayerY;
        }

        // �÷��̾ ���� �浹�ߴ��� Ȯ��
        if (map[playerY][playerX] == 6) {
            gameRunning = 0;
            gotoxy(0, MAP_HEIGHT + 1);
            printf("���� ����!                                \n");
            ResetGame();
            Sleep(1500);
        }

        if (map[playerY][playerX] == 7) {
            gameRunning = 0;
            gotoxy(0, MAP_HEIGHT + 1);
            printf("���� ����!                                \n");
            ResetGame();
            Sleep(1500);
        }

        //������ ������ ���� ���� ó��
        if (map[playerY][playerX] == 2) {
            gameRunning = 0;
            gotoxy(0, MAP_HEIGHT + 1); // ȭ�� �ϴ����� �̵�
            printf("���� ����! ������ ���߽��ϴ�.                  \n");
            ResetGame();
            Sleep(1500);
        }

        // ���踦 ȹ���ߴ��� Ȯ��
        if (map[playerY][playerX] == 3 && !hasKey) {
            hasKey = 1;
            gotoxy(0, MAP_HEIGHT + 1);
            printf("���踦 ȹ���߽��ϴ�!                           \n");
            PlaySound(TEXT("key.wav"), NULL, SND_FILENAME | SND_ASYNC);
            Sleep(500);
        }

        // Ż�ⱸ�� �����ߴ��� Ȯ��
        if (map[playerY][playerX] == 4) {
            if (hasKey) { // ���踦 ������ ���� ���� Ż�� ����
                gotoxy(0, MAP_HEIGHT + 1);
                printf("                                             \n");
                Sleep(500);
                NextMap(); // ���� ������ �̵�
                PlaySound(TEXT("exit.wav"), NULL, SND_FILENAME | SND_ASYNC);
                ResetGame();
                Sleep(2000);
            }
            else {
                gotoxy(0, MAP_HEIGHT + 1);
                printf("���谡 �ʿ��մϴ�!                           \n");
                PlaySound(TEXT("cant_exit.wav"), NULL, SND_FILENAME | SND_ASYNC);
                Sleep(1000);
            }
        }

        // ������ Ż�ⱸ �浹
        if (map[playerY][playerX] == 5) {
            if (hasKey) { // ���踦 ������ ���� ���� Ż�� ����
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
                printf("���谡 �ʿ��մϴ�!                           \n");
                PlaySound(TEXT("cant_exit.wav"), NULL, SND_FILENAME | SND_ASYNC);
                Sleep(1000);
            }
        }

        checkEnemyProximity(); // �� �Ҹ� ���
        // ���ο� ��ġ�� �÷��̾� �׸���
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
                printf("������ ����Ǿ����ϴ�.       \n");
                break;
            }
            else if (key == 'h') {
                gotoxy(0, MAP_HEIGHT + 1);
                printf("���� ����:\n");
                printf("���� ���� : ESC\n");
                printf("���� ������ ���� Ż�� ������ �����ؾ� �մϴ�.\n");
                printf("��, ����, ����, Ż�ⱸ �������� ������ �Ҹ��� �鸳�ϴ�.�̰Ϳ� �������ּ���.\n");
                printf("�̵�: W, A, S, D\n");
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
                gameRunning = 0; // ESC Ű�� ���� ����
                break;
            }
        }

    }

}