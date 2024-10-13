#include "game.h"
#include "ui.h"

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
        }

        // �̵��� ��ġ�� ��ȿ���� Ȯ��
        if (map[newPlayerY][newPlayerX] == 0 || map[newPlayerY][newPlayerX] == 2) {
            // ���� ��ġ �����
            ClearPlayer(playerX, playerY); // ���� ��ġ �����

            // �÷��̾� ��ġ ������Ʈ
            playerX = newPlayerX;
            playerY = newPlayerY;
        }

        // �÷��̾ ���� �浹�ߴ��� Ȯ��
        if (playerX == enemyX && playerY == enemyY) {
            gameRunning = 0;
            gotoxy(0, MAP_HEIGHT + 1);
            printf("���� ����!                                \n");
            ResetGame();
            Sleep(1000);
        }

        if (playerX == movingEnemy.x && playerY == movingEnemy.y) {
            gameRunning = 0;
            gotoxy(0, MAP_HEIGHT + 1);
            printf("���� ����!                                \n");
            ResetGame();
            Sleep(1000);
        }

        // ���� üũ
        //checkCliff(playerX, playerY);

        //������ ������ ���� ���� ó��
        if (map[playerY][playerX] == 2) {
            gameRunning = 0;
            gotoxy(0, MAP_HEIGHT + 1); // ȭ�� �ϴ����� �̵�
            printf("���� ����! ������ ���߽��ϴ�.                  \n");
            ResetGame();
            Sleep(1000);
        }

        // ���踦 ȹ���ߴ��� Ȯ��
        if (playerX == keyX && playerY == keyY && !hasKey) {
            hasKey = 1;
            gotoxy(0, MAP_HEIGHT + 1);
            printf("���踦 ȹ���߽��ϴ�!                           \n");
            PlaySound(TEXT("key.wav"), NULL, SND_FILENAME | SND_ASYNC);
            Sleep(500);
        }

        // Ż�ⱸ�� �����ߴ��� Ȯ��
        if (playerX == exitX && playerY == exitY) {
            if (hasKey) { // ���踦 ������ ���� ���� Ż�� ����
                gameRunning = 0;
                gotoxy(0, MAP_HEIGHT + 1);
                printf("Ż���߽��ϴ�!                    \n");
                PlaySound(TEXT("exit.wav"), NULL, SND_FILENAME | SND_ASYNC);
                ResetGame();
                Sleep(1500);
            }
            else {
                gotoxy(0, MAP_HEIGHT + 1);
                printf("���谡 �ʿ��մϴ�!               \n");
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
    gameRunning = 1;
    while (gameRunning) {
        DrawVisibleMap(); // �÷��̾� �ֺ� ������ ��Ÿ ��� �׸���
        Update();
        UpdateMovingEnemy();
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
                printf("ESC: ���� ����\n");
                printf("���� ������ ���� Ż�� ������ �����ؾ� �մϴ�.\n");
                printf("�̵�: W, A, S, D\n");
                Sleep(2000);
            }
        }
    }
}