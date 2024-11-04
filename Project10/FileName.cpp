#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>
#include <windows.h>

const double PI = 3.14159265358979323846; // PI ��� ����

void clearConsole() {
    // �ܼ� ȭ�� �����
    system("cls");
}

struct Vector2 {
    float x, y;

    // ���� ȸ�� �Լ�
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

// ���� �ٰ��� ���ο� �ִ��� üũ�ϴ� �Լ�
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

// �簢���� �������� ȸ����Ű�� ���θ� ä��� �Լ�
void drawFilledRotatingRectangle(int centerX, int centerY, int width, int height, float angle) {
    clearConsole();
    char rectangleChar = '*';

    // �簢���� �������� �����մϴ�.
    Vector2 vertices[4] = {
        { width / 2.0f, height / 2.0f },   // ������ ��
        { width / 2.0f, -height / 2.0f },  // ������ �Ʒ�
        { -width / 2.0f, -height / 2.0f }, // ���� �Ʒ�
        { -width / 2.0f, height / 2.0f }   // ���� ��
    };

    // ȸ���� ������ ����
    Vector2 rotatedVertices[4];
    for (int i = 0; i < 4; ++i) {
        rotatedVertices[i] = vertices[i].rotate(angle);
    }

    // �簢�� ���� ä���
    for (int y = -height; y <= height; ++y) {
        for (int x = -width; x <= width; ++x) {
            Vector2 currentPoint = { static_cast<float>(x), static_cast<float>(y) };

            // ���� ��ǥ�� �ܼ��� �߽����� ����
            int drawX = static_cast<int>(centerX + currentPoint.x);
            int drawY = static_cast<int>(centerY - currentPoint.y); // Y�� ����

            // ���� ȸ���� �簢�� ���ο� �ִ��� Ȯ��
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
    const int consoleWidth = 80;  // �ܼ� ���� ũ��
    const int consoleHeight = 25; // �ܼ� ���� ũ��
    const int rectWidth = 11;      // �簢���� ���� ����
    const int rectHeight = 5;      // �簢���� ���� ����
    const int centerX = consoleWidth / 2;
    const int centerY = consoleHeight / 2;

    float angle = 0.0f;
    const float rotationSpeed = 2 * PI / 3; // 3�ʿ� 1���� ȸ�� (360�� = 2�� ����)
    const int fps = 30; // ������ �ӵ�
    const float angleIncrement = rotationSpeed / fps; // ���� ������

    while (true) {
        drawFilledRotatingRectangle(centerX, centerY, rectWidth, rectHeight, angle);
        angle += angleIncrement; // ���� ����

        // ��� ���
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps)); // �� 30fps
    }

    return 0;
}
