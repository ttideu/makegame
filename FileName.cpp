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
    printf("*                            1. 게임 시작                         *\n");
    printf("*                            2. 도움말                            *\n");
    printf("*                            q. 종료                              *\n");
    printf("*                                                                 *\n");
    printf("*                                                                 *\n");
    printf("*                                                                 *\n");
    printf("*                                                                 *\n");
    printf("*                                                                 *\n");
    printf("*                                                                 *\n");
    printf("*******************************************************************\n");
    printf("\n");
    printf("원하는 옵션을 선택하세요: ");
    printf("\n");
}

void Render() {
    printf("\n게임이 진행 중입니다...\n");
}

void WaitFor3Seconds() {
    clock_t start_time = clock();
    while ((clock() - start_time) < 3 * CLOCKS_PER_SEC) {
        // 대기
    }
}

void Update() {
    if (_kbhit()) { // 키가 눌렸는지 확인
        char key = _getch(); // 키 입력을 받음
        printf("입력된 키: %c\n", key);

        if (key == 'q') {
            printf("3초 뒤에 게임 종료...\n");
            WaitFor3Seconds();
            printf("게임 종료...\n");
            exit(0);
        }

        if (key == '1') {
            printf("\n게임을 시작합니다!\n");
            ClearScreen();

            clock_t start_time = clock(); // 시작 시간을 초기화

            while (1) { // 무한 루프

                if ((clock() - start_time) >= CLOCKS_PER_SEC) { // 1초마다 렌더링
                    Render();
                    start_time = clock(); // 렌더링 후 시작 시간 초기화
                }

                if (_kbhit()) { // 키가 눌렸을 때 처리
                    char game_key = _getch();
                    if (game_key == 'q') {
                        printf("3초 뒤에 게임 종료...\n");
                        WaitFor3Seconds();
                        printf("게임 종료...\n");
                        exit(0);
                    }
                }
            }
        }

        if (key == '2') {
            printf("\n게임 도움말:\n");
            printf("- 'q' 키를 눌러 게임을 종료할 수 있습니다.\n");
        }
        else {
            printf("\n잘못된 입력입니다. 다시 선택하세요.\n");
        }
    }
}

void MainMenu() {
    int screenWidth = 60;  // 화면 가로 크기
    int cactusX = screenWidth;  // 선인장의 초기 위치 (화면 오른쪽)
    int cactusY = 13;  // 선인장의 Y 좌표

    clock_t start_time = clock(); // 현재 시간을 기록

    while (1) {
        if (_kbhit()) {
            Update();
            printf("\n계속하려면 아무 키나 누르세요...\n");
            _getch();  // 사용자 입력 대기
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