#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h>  // _kbhit(), _getch() 함수 사용을 위한 헤더
#include <windows.h>

#include "ElfMath.h"

#define WIDTH 80
#define HEIGHT 24

// 상태를 추적하는 변수
int state = 1;

// 전역 변수로 스크린 버퍼 선언
char screenBuffer[(WIDTH + 1) * HEIGHT];
int screenWidth = WIDTH;
int screenHeight = HEIGHT;

// 화면 초기화 함수
void ClearScreen() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            screenBuffer[i * (WIDTH + 1) + j] = ' ';  // 화면 초기화
        }
        screenBuffer[i * (WIDTH + 1) + WIDTH] = '\n';  // 개행 문자
    }

}


void FillPolygon(Vector3* vertices, int vertexCount, char fillChar) {
    for (int y = 0; y < HEIGHT; y++) {
        int nodeX[1000];  // 현재 스캔라인에서 교차하는 x 좌표들
        int nodeCount = 0;

        for (int i = 0; i < vertexCount; i++) {
            Vector3 v1 = vertices[i];
            Vector3 v2 = vertices[(i + 1) % vertexCount];  // 다음 꼭짓점

            // 스캔라인과 교차 여부 확인
            if ((v1.y <= y && v2.y > y) || (v2.y <= y && v1.y > y)) {
                // 교차점 x 좌표 계산 (더 높은 정밀도)
                double slope = (double)(v2.x - v1.x) / (v2.y - v1.y);  // 기울기
                int x = (int)(v1.x + (y - v1.y) * slope + 0.5);        // 반올림 처리
                nodeX[nodeCount++] = x;
            }
        }

        // 교차점 정렬
        for (int i = 0; i < nodeCount - 1; i++) {
            for (int j = i + 1; j < nodeCount; j++) {
                if (nodeX[i] > nodeX[j]) {
                    int temp = nodeX[i];
                    nodeX[i] = nodeX[j];
                    nodeX[j] = temp;
                }
            }
        }

        // 교차점들 사이를 채움
        for (int i = 0; i < nodeCount; i += 2) {
            if (nodeX[i] >= WIDTH) break;  // 화면 범위 밖이면 무시
            if (nodeX[i + 1] > 0) {       // 유효한 구간이면 채움
                int startX = (nodeX[i] < 0) ? 0 : nodeX[i];                      // 왼쪽 경계
                int endX = (nodeX[i + 1] >= WIDTH) ? WIDTH - 1 : nodeX[i + 1];  // 오른쪽 경계
                for (int x = startX; x <= endX; x++) {
                    screenBuffer[y * (WIDTH + 1) + x] = fillChar;
                }
            }
        }
    }
}

void DrawLine(int x1, int y1, int x2, int y2, char fillChar) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        if (x1 >= 0 && x1 < WIDTH && y1 >= 0 && y1 < HEIGHT) {
            screenBuffer[y1 * (WIDTH + 1) + x1] = fillChar;  // 화면에 전달된 문자로 채움
        }

        if (x1 == x2 && y1 == y2) break;
        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void Render(float angle) {
    ClearScreen();

    // 태양 계산 및 그리기
    Vector3 sun_center = { 40.0f, 12.0f, 1.0f };
    Matrix3x3 sun_rotation = rotation_matrix(-angle / 3000);
    Vector3 sun_vertices[3] = {
        { 0.0f, -5.0f, 1.0f },
        { -4.0f, 2.5f, 1.0f },
        { 4.0f, 2.5f, 1.0f }
    };
    for (int i = 0; i < 3; i++) {
        sun_vertices[i] = multiply_matrix_vector(sun_rotation, sun_vertices[i]);
        sun_vertices[i].x += sun_center.x;
        sun_vertices[i].y += sun_center.y;
    }
    FillPolygon(sun_vertices, 3, 'S');

    // 지구 계산 및 그리기
    Matrix3x3 earth_rotation = rotation_matrix(angle / 3000);
    Vector3 earth_center = multiply_matrix_vector(earth_rotation, (Vector3) { 24.0f, 0.0f, 1.0f });
    earth_center.x += sun_center.x;
    earth_center.y += sun_center.y;
    Matrix3x3 earth_self_rotation = rotation_matrix(angle / 2000);
    Vector3 earth_vertices[4] = {
        { -3.0f, -3.0f, 1.0f },
        { 3.0f, -3.0f, 1.0f },
        { 3.0f, 3.0f, 1.0f },
        { -3.0f, 3.0f, 1.0f }
    };
    for (int i = 0; i < 4; i++) {
        earth_vertices[i] = multiply_matrix_vector(earth_self_rotation, earth_vertices[i]);
        earth_vertices[i].x += earth_center.x;
        earth_vertices[i].y += earth_center.y;
    }
    FillPolygon(earth_vertices, 4, 'E');  // 지구 내부 채우기

    // 달 계산 및 그리기
    Matrix3x3 moon_rotation = rotation_matrix(-angle / 2000);
    Vector3 moon_center = multiply_matrix_vector(moon_rotation, (Vector3) { 8.0f, 0.0f, 1.0f });
    moon_center.x += earth_center.x;
    moon_center.y += earth_center.y;
    Matrix3x3 moon_self_rotation = rotation_matrix(angle / 1000);
    Vector3 moon_vertices[16];
    for (int i = 0; i < 16; i++) {
        float theta = i * (2 * M_PI / 16);
        moon_vertices[i] = multiply_matrix_vector(
            moon_self_rotation,
            (Vector3) {
            cosf(theta) * 1.5f, sinf(theta) * 1.5f, 1.0f
        }
        );
        moon_vertices[i].x += moon_center.x;
        moon_vertices[i].y += moon_center.y;
    }
    FillPolygon(moon_vertices, 16, 'M');  // 달 내부 채우기

}


// 게임 초기화 함수
void Init() {

    ClearScreen();

    //커서 숨기기
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;  // 커서를 숨김
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// 화면을 그리는 함수
void Draw(char* Buffer) {
    //0,0으로 이동
    COORD coord = { 0, 0 };  // 좌표 (0, 0)
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

    // 스크린 버퍼 출력
    printf("%s", screenBuffer);  // 화면 출력
}

float angle = 0;

// 화면 업데이트 함수
void Update() {
    if (state == 3) {
        ClearScreen();
        Render(angle);
        angle += 10;
    }
    else {
        angle = 0; // 상태 변경 시 angle 초기화
    }
}

void RenderTitleScreen() {
    // 화면 전체를 공백으로 초기화
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            screenBuffer[y * (WIDTH + 1) + x] = ' ';  // 공백으로 채우기
        }
        screenBuffer[y * (WIDTH + 1) + WIDTH] = '\n';  // 각 줄 끝에 개행 추가
    }
    screenBuffer[HEIGHT * (WIDTH + 1) - 1] = '\0';  // 버퍼의 마지막에 널 문자 추가

    // 출력할 문자열
    const char* text = "20211429 황기환";
    int textX = WIDTH / 2 - strlen(text) / 2;  // 텍스트의 X 위치 (가운데 정렬)
    int textY = HEIGHT / 2;  // 텍스트의 Y 위치 (가운데 정렬)

    // 문자열을 화면 버퍼에 저장
    for (int i = 0; i < strlen(text); i++) {
        screenBuffer[textY * (WIDTH + 1) + (textX + i)] = text[i];  // 각 문자 출력
    }

    Draw(screenBuffer);  // 화면에 출력
}

void AlignPlanets() {
    ClearScreen();
    // 태양 위치
    Vector3 sun_center = { 40.0f, 12.0f, 1.0f };

    // 태양: 고정 (삼각형 그리기)
    Vector3 sun_vertices[3] = {
        { 0.0f, -5.0f, 1.0f },  // 위쪽 꼭짓점
        { -4.0f, 2.5f, 1.0f },  // 왼쪽 꼭짓점
        { 4.0f, 2.5f, 1.0f }    // 오른쪽 꼭짓점
    };

    for (int i = 0; i < 3; i++) {
        sun_vertices[i].x += sun_center.x;
        sun_vertices[i].y += sun_center.y;
    }

    // 태양 그리기
    FillPolygon(sun_vertices, 3, 'S');

    ////////////////////////
    // 지구 중심 위치 (태양 중심 기준)
    Vector3 earth_center = { 24.0f, 0.0f, 1.0f };
    earth_center.x += sun_center.x;  // 태양 중심으로 이동
    earth_center.y += sun_center.y;

    // 지구 꼭짓점
    Vector3 earth_vertices[4] = {
        { -3.0f, -3.0f, 1.0f },
        { 3.0f, -3.0f, 1.0f },
        { 3.0f, 3.0f, 1.0f },
        { -3.0f, 3.0f, 1.0f }
    };

    for (int i = 0; i < 4; i++) {
        earth_vertices[i].x += earth_center.x;  // 지구 중심으로 이동
        earth_vertices[i].y += earth_center.y;
    }

    // 지구 그리기
    FillPolygon(earth_vertices, 4, 'E');  // 지구 내부 채우기

    /////////////////////////////////

    // 달의 고정된 위치를 계산
    Vector3 moon_center = (Vector3){ 8.0f, 0.0f, 1.0f }; // 달 중심 좌표 (지구로부터 거리 8)
    moon_center.x += earth_center.x;                    // 지구 중심 좌표로 이동
    moon_center.y += earth_center.y;

    // 달의 원 형태 계산
    Vector3 moon_vertices[16]; // 점 개수로 원처럼 보이게
    float radius = 1.5f;       // 달의 반지름

    for (int i = 0; i < 16; i++) { // 12개의 점
        float theta = i * (2 * M_PI / 16); // 원의 점 각도 계산
        moon_vertices[i] = (Vector3){
            cosf(theta) * radius, // x 좌표
            sinf(theta) * radius, // y 좌표
            1.0f
        };
        moon_vertices[i].x += moon_center.x; // 달 중심으로 이동
        moon_vertices[i].y += moon_center.y;
    }

    // 달 그리기
    FillPolygon(moon_vertices, 16, 'M');  // 달 내부 채우기
    

}

int main() {
    // 게임 초기화
    Init();

    clock_t lastTime = clock();
    int fps = 60;
    double frameTime = 1000.0 / fps;

    while (1) {
        // ESC 키를 눌렀는지 확인
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 27) {  // ESC 키가 눌리면 게임 종료
                break;
            }

            if (ch == ' ' || ch == 13) {  
                state = (state % 3) + 1;

                if (state == 3) {
                    angle = 0; // 상태가 3으로 바뀔 때 각도 초기화
                }
            }
        }

        // 상태별 동작
        switch (state) {
        case 1:
            RenderTitleScreen();  // 첫 화면을 계속 출력
            break;
        case 2:
            AlignPlanets();  // 행성들이 가로로 정렬
            break;
        case 3:
            Update();  // 행성 공전 및 자전 애니메이션
            break;
        }

        // 프레임 제한 (60fps 유지)
        clock_t now = clock();
        double deltaTime = (double)(now - lastTime) / CLOCKS_PER_SEC * 1000.0;

        if (deltaTime >= frameTime) {
            lastTime = now;

            // 화면 그리기 (상태에 맞는 화면 그리기)
            if (state != 1) {
                Draw(screenBuffer);  // 화면에 출력
            }
        }
    }

    return 0;
}