#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
#include <windows.h>

const double PI = 3.14159265358979323846; // PI 상수 정의

void clearConsole() {
    // 콘솔 화면 지우기
    system("cls");
}

struct Vector2 {
    float x, y;

    // 벡터 회전 함수
    Vector2 rotate(float angle) const {
        float radians = angle;
        float cosAngle = cos(radians);
        float sinAngle = sin(radians);
        return {
            x * cosAngle - y * sinAngle,
            x * sinAngle + y * cosAngle
        };
    }
};

// 점이 다각형 내부에 있는지 체크하는 함수
bool isInside(Vector2 p, const Vector2 vertices[], int vertexCount) {
    int crossings = 0;

    for (int i = 0; i < vertexCount; i++) {
        Vector2 a = vertices[i];
        Vector2 b = vertices[(i + 1) % vertexCount];

        if ((a.y > p.y) != (b.y > p.y) &&
            (p.x < (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x)) {
            crossings++;
        }
    }

    return crossings % 2 == 1;
}

// 사각형의 꼭짓점을 회전시키고 내부를 채우는 함수
void drawFilledRotatingRectangle(int centerX, int centerY, int width, int height, float angle) {
    clearConsole();
    char rectangleChar = '*';

    // 사각형의 꼭짓점을 정의합니다.
    Vector2 vertices[4] = {
        { width / 2.0f, height / 2.0f },   // 오른쪽 위
        { width / 2.0f, -height / 2.0f },  // 오른쪽 아래
        { -width / 2.0f, -height / 2.0f }, // 왼쪽 아래
        { -width / 2.0f, height / 2.0f }   // 왼쪽 위
    };

    // 회전된 꼭짓점 저장
    Vector2 rotatedVertices[4];
    for (int i = 0; i < 4; ++i) {
        rotatedVertices[i] = vertices[i].rotate(angle);
    }

    // 사각형 내부 채우기
    for (int y = -height; y <= height; ++y) {
        for (int x = -width; x <= width; ++x) {
            Vector2 currentPoint = { static_cast<float>(x), static_cast<float>(y) };

            // 최종 좌표를 콘솔의 중심으로 맞춤
            int drawX = static_cast<int>(centerX + currentPoint.x);
            int drawY = static_cast<int>(centerY - currentPoint.y); // Y축 반전

            // 점이 회전된 사각형 내부에 있는지 확인
            if (isInside(currentPoint, rotatedVertices, 4)) {
                if (drawX >= 0 && drawX < 80 && drawY >= 0 && drawY < 25) {
                    COORD coord = { static_cast<SHORT>(drawX), static_cast<SHORT>(drawY) };
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
                    std::cout << rectangleChar;
                }
            }
        }
    }
}

int main() {
    const int consoleWidth = 80;  // 콘솔 가로 크기
    const int consoleHeight = 25; // 콘솔 세로 크기
    const int rectWidth = 11;      // 사각형의 가로 길이
    const int rectHeight = 5;      // 사각형의 세로 길이
    const int centerX = consoleWidth / 2;
    const int centerY = consoleHeight / 2;

    float angle = 0.0f;
    const float rotationSpeed = 2 * PI / 3; // 3초에 1바퀴 회전 (360도 = 2π 라디안)
    const int fps = 30; // 프레임 속도
    const float angleIncrement = rotationSpeed / fps; // 각도 증가량

    while (true) {
        drawFilledRotatingRectangle(centerX, centerY, rectWidth, rectHeight, angle);
        angle += angleIncrement; // 각도 증가

        // 잠시 대기
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps)); // 약 30fps
    }

    return 0;
}
