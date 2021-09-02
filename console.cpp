#include <windows.h>

constexpr int SCREEN_WIDTH  =  64;
constexpr int SCREEN_HEIGHT =  32;
constexpr int SCREEN_SIZE   =  SCREEN_WIDTH * SCREEN_HEIGHT;

namespace Console
{
    wchar_t *screen = new wchar_t[SCREEN_SIZE];
    HANDLE hConsole;

    void init()
    {
        hConsole = CreateConsoleScreenBuffer(
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            CONSOLE_TEXTMODE_BUFFER,
            NULL
        );
        SetConsoleActiveScreenBuffer(hConsole);
    }

    void draw(const int x, const int y, short character)
    {
        screen[y * SCREEN_WIDTH + x] = character;
    }

    void updateScreen()
    {
        DWORD bytesWritten;
        WriteConsoleOutputCharacterW(
            hConsole,
            screen,
            SCREEN_SIZE,
            { 0,0 },
            &bytesWritten
        );
    }

    void exit()
    {
        delete[] screen;
        CloseHandle(hConsole);
    }

} // namespace Console

