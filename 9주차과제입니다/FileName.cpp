#include <stdio.h>
#include <math.h>
#include <windows.h>

#define WIDTH 60
#define HEIGHT 60

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    float m[3][3]; // 3x3 ���
} Matrix3x3;

typedef struct {
    float x;
    float y;
    float w; // ���� ��ǥ�迡���� w
} vec3; // �̸��� vec3�� ����

// 3x3 ��� �ʱ�ȭ
void initIdentityMatrix(Matrix3x3* mat) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mat->m[i][j] = (i == j) ? 1 : 0;
        }
    }
}

// �̵� ��� ����
Matrix3x3 createTranslationMatrix(float tx, float ty) {
    Matrix3x3 mat;
    initIdentityMatrix(&mat);
    mat.m[0][2] = tx; // X�� �̵�
    mat.m[1][2] = ty; // Y�� �̵�
    return mat;
}

// ȸ�� ��� ����
Matrix3x3 createRotationMatrix(float angle) {
    Matrix3x3 mat;
    initIdentityMatrix(&mat);
    float radian = angle * M_PI / 180;
    mat.m[0][0] = cos(radian); // cos(��)
    mat.m[0][1] = -sin(radian); // -sin(��)
    mat.m[1][0] = sin(radian); // sin(��)
    mat.m[1][1] = cos(radian); // cos(��)
    return mat;
}

// ��� ����
Matrix3x3 multiplyMatrices(Matrix3x3 a, Matrix3x3 b) {
    Matrix3x3 result;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j];
        }
    }
    return result;
}

// ���Ϳ� ��� ����
vec3 Mul(vec3 a, Matrix3x3 b) {
    vec3 result;
    result.x = a.x * b.m[0][0] + a.y * b.m[0][1] + a.w * b.m[0][2];
    result.y = a.x * b.m[1][0] + a.y * b.m[1][1] + a.w * b.m[1][2];
    result.w = a.x * b.m[2][0] + a.y * b.m[2][1] + a.w * b.m[2][2];
    return result;
}

// ���� ��� ����
Matrix3x3 MulMatrices(Matrix3x3 a, Matrix3x3 b) {
    return multiplyMatrices(a, b);
}

// �ܼ� ȭ�� �����
void clearScreen() {
    system("cls");
}

// ȭ�� �׸���
void draw(int angle) {
    clearScreen();

    // P ��ġ (������ �Ʒ��� �̵�)
    int pX = WIDTH / 2 + 15; // P�� X ��ǥ�� ���������� �̵�
    int pY = HEIGHT / 2 + 3; // P�� Y ��ǥ�� �Ʒ��� �̵�

    // O�� ��ġ ���
    float oRadius = 11.0f; // O�� P�κ��� ������ �Ÿ�
    vec3 oPos = { oRadius * cos(angle * M_PI / 180), oRadius * sin(angle * M_PI / 180), 1.0f };

    // M�� �ʱ� ��ġ (O ���� �Ÿ�)
    float mRadius = 3.0f; // M�� O�κ��� ������ �Ÿ�
    vec3 mOffset = { mRadius, 0, 1 }; // M�� O�� ���� �ʱ� ��ġ (x�� ����)

    // M�� ȸ�� ��� ����
    Matrix3x3 rotation = createRotationMatrix(angle * 2); // M�� O���� 2�� ������ ȸ��

    // M�� ��ġ ���
    vec3 mPos = Mul(mOffset, rotation);
    mPos.x += oPos.x; // O ��ġ�� M�� ��ġ ���ϱ�
    mPos.y += oPos.y;

    // ��ġ ��ȯ�� ���� ��� ����
    Matrix3x3 translationToP = createTranslationMatrix(pX, pY);

    // O�� M�� ��ġ ��ȯ
    oPos = Mul(oPos, translationToP);
    mPos = Mul(mPos, translationToP);

    // e = Mul(A, Mul(b, Mul(c, d))); ������ ����
    Matrix3x3 A = createTranslationMatrix(pX, pY);
    Matrix3x3 b = createRotationMatrix(angle);
    Matrix3x3 c = createTranslationMatrix(oRadius, 0);
    Matrix3x3 d = createTranslationMatrix(0, 0);

    // ���� ��ȯ ���
    Matrix3x3 e = MulMatrices(A, MulMatrices(b, MulMatrices(c, d)));

    // ȭ�� �׸���
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == pX && y == pY) {
                printf("P"); // P ���
            }
            else if ((int)(oPos.x) == x && (int)(oPos.y) == y) {
                printf("O"); // O ���
            }
            else if ((int)(mPos.x) == x && (int)(mPos.y) == y) {
                printf("M"); // M ���
            }
            else {
                printf(" "); // �� ���� ���
            }
        }
        printf("\n");
    }
}

int main() {
    int angle = 0; // ���� �ʱ�ȭ

    while (1) { // ���� ����
        draw(angle); // ȸ�� ������ ���� �׸���
        angle += 5; // ������ ����
        if (angle >= 360) angle = 0; // ������ 360�� ������ 0���� �ʱ�ȭ
        Sleep(100); // 0.1�� ���
    }
    return 0;
}