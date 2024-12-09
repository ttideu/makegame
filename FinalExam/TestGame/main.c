#include "ElfMath.h"
#include "Elf2D.h"
#include "ElfTime.h"
#include "ElfObject.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WIDTH 80
#define HEIGHT 24

void Initialize(GameObject_Line* obj, int objNum)
{
    //객체 초기화(관절 3개)
    int i = 0;
    for (i = 0; i < 3; i++)
    {
        obj[i].Position.x = 0;
        obj[i].Position.y = 0;

        obj[i].Rotation = 0;


        //(0,0) ~ (3,0) 을 잇는 직선
        obj[i].Line[0].x = 0;
        obj[i].Line[0].y = 0;

        obj[i].Line[1].x = 3;
        obj[i].Line[1].y = 0;
    }

    obj[1].Rotation = 30;
    obj[2].Rotation = 30;

    //화면 출력 심볼 입력
    obj[0].Symbol = "11";
    obj[1].Symbol = "22";
    obj[2].Symbol = "33";

    //크기 입력
    obj[0].Scale.x = 1;
    obj[0].Scale.y = 1;
    obj[1].Scale.x = 2;
    obj[1].Scale.y = 2;
    obj[2].Scale.x = 2;
    obj[2].Scale.y = 2;
}

// 0: 입력없음
// 99 : 게임 종료
int Input()
{
    // ESC 키를 눌렀는지 확인
    if (_kbhit()) {
        char ch = _getch();
        if (ch == 27) {  // ESC 키가 눌리면 게임 종료
            return 99;
        }
        if (ch == 'a') {  // ESC 키가 눌리면 게임 종료
            // ang += 1;
        }
        if (ch == 'd') {  // ESC 키가 눌리면 게임 종료
            return 99;
        }
    }

    return 0;
}

void Update(GameObject_Line* obj, int objNum, int e)
{    
    
}

float ang = 0;

void Render(GameObject_Line* obj, int objNum, char* Buf, int width, int height)
{
    Vector3 sun_cntr = { 40.0f, 12.0f, 1.0f };

    Matrix3x3 sun_rotat = rotation_matrix(-ang / 3000);

    Vector3 sun_vrtc[3] = {
        { 0.0f, -5.0f, 1.0f },
        { -4.0f, 2.5f, 1.0f },
        { 4.0f, 2.5f, 1.0f }
    };

    for (int i = 0; i < 3; i++) {
        sun_vrtc[i] = multiply_matrix_vector(sun_rotat, sun_vrtc[i]);
        sun_vrtc[i].x += sun_cntr.x;
        sun_vrtc[i].y += sun_cntr.y;
    }

    Elf2DDrawLine2(sun_vrtc[0].x, sun_vrtc[0].y, sun_vrtc[1].x, sun_vrtc[1].y, Buf, width, height);
    Elf2DDrawLine2(sun_vrtc[1].x, sun_vrtc[1].y, sun_vrtc[2].x, sun_vrtc[2].y, Buf, width, height);
    Elf2DDrawLine2(sun_vrtc[2].x, sun_vrtc[2].y, sun_vrtc[0].x, sun_vrtc[0].y, Buf, width, height);
}

// 게임 루프
int main() {
    int fps = 30;
    double frameTime = 1000.0 / fps;

    // 전역 변수로 스크린 버퍼 선언
    char screenBuffer[(WIDTH + 1) * HEIGHT];
    int screenWidth = WIDTH;
    int screenHeight = HEIGHT;

    GameObject_Line LineObj[3];

    // 게임 초기화
    Initialize(LineObj, 3);
    
    Elf2DInitScreen();
    Elf2DClearScreen(screenBuffer, screenWidth, screenHeight);
    Elf2DDrawBuffer(screenBuffer);
    
    ElfTimeInitialize();
    
    int isGameRunning = 1;
    while (isGameRunning) {
        
        int gameEvent = Input();
        
        //입력 계산
        if (gameEvent == 99)
        {
            isGameRunning = 0;
        }

        // 게임 업데이트
        Update(LineObj, 3, gameEvent);
        
        // 스크린버퍼에 게임 렌더링
        Render(LineObj, 3, screenBuffer, screenWidth, screenHeight);

        // 화면 그리기
        Elf2DDrawBuffer(screenBuffer);
        
        ElfTimeCalculateDeltaTime();
        Elf2DSleep( frameTime - ElfTimeGetDeltaTime() );
    }

    return 0;
}