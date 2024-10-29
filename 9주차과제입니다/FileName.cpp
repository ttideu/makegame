#include <stdio.h>
#include <math.h>
#include <windows.h>

#define WIDTH 60
#define HEIGHT 60

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    float m[3][3]; // 3x3 행렬
} Matrix3x3;

typedef struct {
    float x;
    float y;
    float w; // 동차 좌표계에서의 w
} vec3; // 이름을 vec3로 변경

// 3x3 행렬 초기화
void initIdentityMatrix(Matrix3x3* mat) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mat->m[i][j] = (i == j) ? 1 : 0;
        }
    }
}

// 이동 행렬 생성
Matrix3x3 createTranslationMatrix(float tx, float ty) {
    Matrix3x3 mat;
    initIdentityMatrix(&mat);
    mat.m[0][2] = tx; // X축 이동
    mat.m[1][2] = ty; // Y축 이동
    return mat;
}

// 회전 행렬 생성
Matrix3x3 createRotationMatrix(float angle) {
    Matrix3x3 mat;
    initIdentityMatrix(&mat);
    float radian = angle * M_PI / 180;
    mat.m[0][0] = cos(radian); // cos(θ)
    mat.m[0][1] = -sin(radian); // -sin(θ)
    mat.m[1][0] = sin(radian); // sin(θ)
    mat.m[1][1] = cos(radian); // cos(θ)
    return mat;
}

// 행렬 곱셈
Matrix3x3 multiplyMatrices(Matrix3x3 a, Matrix3x3 b) {
    Matrix3x3 result;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j];
        }
    }
    return result;
}

// 벡터와 행렬 곱셈
vec3 Mul(vec3 a, Matrix3x3 b) {
    vec3 result;
    result.x = a.x * b.m[0][0] + a.y * b.m[0][1] + a.w * b.m[0][2];
    result.y = a.x * b.m[1][0] + a.y * b.m[1][1] + a.w * b.m[1][2];
    result.w = a.x * b.m[2][0] + a.y * b.m[2][1] + a.w * b.m[2][2];
    return result;
}

// 여러 행렬 곱셈
Matrix3x3 MulMatrices(Matrix3x3 a, Matrix3x3 b) {
    return multiplyMatrices(a, b);
}

// 콘솔 화면 지우기
void clearScreen() {
    system("cls");
}

// 화면 그리기
void draw(int angle) {
    clearScreen();

    // P 위치 (오른쪽 아래로 이동)
    int pX = WIDTH / 2 + 15; // P의 X 좌표를 오른쪽으로 이동
    int pY = HEIGHT / 2 + 3; // P의 Y 좌표를 아래로 이동

    // O의 위치 계산
    float oRadius = 11.0f; // O가 P로부터 떨어진 거리
    vec3 oPos = { oRadius * cos(angle * M_PI / 180), oRadius * sin(angle * M_PI / 180), 1.0f };

    // M의 초기 위치 (O 주위 거리)
    float mRadius = 3.0f; // M가 O로부터 떨어진 거리
    vec3 mOffset = { mRadius, 0, 1 }; // M의 O에 대한 초기 위치 (x축 기준)

    // M의 회전 행렬 생성
    Matrix3x3 rotation = createRotationMatrix(angle * 2); // M는 O보다 2배 빠르게 회전

    // M의 위치 계산
    vec3 mPos = Mul(mOffset, rotation);
    mPos.x += oPos.x; // O 위치에 M의 위치 더하기
    mPos.y += oPos.y;

    // 위치 변환을 위한 행렬 설정
    Matrix3x3 translationToP = createTranslationMatrix(pX, pY);

    // O와 M의 위치 변환
    oPos = Mul(oPos, translationToP);
    mPos = Mul(mPos, translationToP);

    // e = Mul(A, Mul(b, Mul(c, d))); 형식의 수식
    Matrix3x3 A = createTranslationMatrix(pX, pY);
    Matrix3x3 b = createRotationMatrix(angle);
    Matrix3x3 c = createTranslationMatrix(oRadius, 0);
    Matrix3x3 d = createTranslationMatrix(0, 0);

    // 최종 변환 행렬
    Matrix3x3 e = MulMatrices(A, MulMatrices(b, MulMatrices(c, d)));

    // 화면 그리기
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == pX && y == pY) {
                printf("P"); // P 출력
            }
            else if ((int)(oPos.x) == x && (int)(oPos.y) == y) {
                printf("O"); // O 출력
            }
            else if ((int)(mPos.x) == x && (int)(mPos.y) == y) {
                printf("M"); // M 출력
            }
            else {
                printf(" "); // 빈 공간 출력
            }
        }
        printf("\n");
    }
}

int main() {
    int angle = 0; // 각도 초기화

    while (1) { // 무한 루프
        draw(angle); // 회전 각도에 따라 그리기
        angle += 5; // 각도를 증가
        if (angle >= 360) angle = 0; // 각도가 360을 넘으면 0으로 초기화
        Sleep(100); // 0.1초 대기
    }
    return 0;
}