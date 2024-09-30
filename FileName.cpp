#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

void gotoxy(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(hConsole, coord);
}

void ClearScreen() {
    system("cls");
}

void DrawCactus(int x, int y) {
    gotoxy(x + 2, y);     printf("##\n");
    gotoxy(x, y + 1);     printf("# ## #\n");
    gotoxy(x, y + 2);     printf("######\n");
    gotoxy(x + 2, y + 3); printf("##\n");
    gotoxy(x + 2, y + 4); printf("##\n");
    printf("\n\n\n");
}

void DrawTitleScreen() {
    printf("*******************************************************************\n");
    printf("*                                                                 *\n");
    printf("*                                                 ##              *\n");
    printf("*                                                ####             *\n");
    printf("*                         ESCAPE FROM DARKROOM    ##              *\n");
    printf("*                                                                 *\n");
    printf("*                                                                 *\n");
    printf("*                                                                 *\n");
    printf("*                                                                 *\n");
    printf("*                            1. ���� ����                         *\n");
    printf("*                            2. ����                            *\n");
    printf("*                            q. ����                              *\n");
    printf("*                                                                 *\n");
    printf("*                                                                 *\n");
    printf("*                                                                 *\n");
    printf("*                                                                 *\n");
    printf("*                                                                 *\n");
    printf("*                                                                 *\n");
    printf("*******************************************************************\n");
    printf("\n");
    printf("���ϴ� �ɼ��� �����ϼ���: ");
    printf("\n");
}

void Render() {
    printf("\n������ ���� ���Դϴ�...\n");
}

void WaitFor3Seconds() {
    clock_t start_time = clock();
    while ((clock() - start_time) < 3 * CLOCKS_PER_SEC) {
        // ���
    }
}

void Update() {
    if (_kbhit()) { // Ű�� ���ȴ��� Ȯ��
        char key = _getch(); // Ű �Է��� ����
        printf("�Էµ� Ű: %c\n", key);

        if (key == 'q') {
            printf("3�� �ڿ� ���� ����...\n");
            WaitFor3Seconds();
            printf("���� ����...\n");
            exit(0);
        }

        if (key == '1') {
            printf("\n������ �����մϴ�!\n");
            ClearScreen();

            clock_t start_time = clock(); // ���� �ð��� �ʱ�ȭ

            while (1) { // ���� ����

                if ((clock() - start_time) >= CLOCKS_PER_SEC) { // 1�ʸ��� ������
                    Render();
                    start_time = clock(); // ������ �� ���� �ð� �ʱ�ȭ
                }

                if (_kbhit()) { // Ű�� ������ �� ó��
                    char game_key = _getch();
                    if (game_key == 'q') {
                        printf("3�� �ڿ� ���� ����...\n");
                        WaitFor3Seconds();
                        printf("���� ����...\n");
                        exit(0);
                    }
                }
            }
        }

        if (key == '2') {
            printf("\n���� ����:\n");
            printf("- 'q' Ű�� ���� ������ ������ �� �ֽ��ϴ�.\n");
        }
        else {
            printf("\n�߸��� �Է��Դϴ�. �ٽ� �����ϼ���.\n");
        }
    }
}

void MainMenu() {
    int screenWidth = 60;  // ȭ�� ���� ũ��
    int cactusX = screenWidth;  // �������� �ʱ� ��ġ (ȭ�� ������)
    int cactusY = 13;  // �������� Y ��ǥ

    clock_t start_time = clock(); // ���� �ð��� ���

    while (1) {
        if (_kbhit()) {
            Update();
            printf("\n����Ϸ��� �ƹ� Ű�� ��������...\n");
            _getch();  // ����� �Է� ���
            DrawTitleScreen();
        }

        if ((clock() - start_time) >= CLOCKS_PER_SEC / 2) {
            ClearScreen();
            DrawTitleScreen();
            DrawCactus(cactusX, cactusY);

            cactusX -= 1;
            if (cactusX < 0) {
                cactusX = screenWidth;
            }

            start_time = clock();
        }
    }
}

void GameLoop() {
    MainMenu();
}

int main() {
    GameLoop();
    return 0;
}