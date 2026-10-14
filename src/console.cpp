#include "console.hpp"

Console::Console()
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

Console::~Console()
{
    delete[] screen;
    CloseHandle(hConsole);
}

void Console::draw(const uint16_t x, const uint16_t y, short character)
{
    screen[y * CONSOLE_APPLICATION_16BIT + x] = character;
}

void Console::drawBuffer(uint32_t* buffer)
{
    for (uint16_t index = 0; index < CONSOLE_SCREEN_SIZE; index++)
    {
        screen[index] = buffer[index] > 0 ? 0x2588 : 0x20; // 0x2591
    }
}

void Console::checkKeys(bool* keypad)
{
    keypad[0]   = ( GetAsyncKeyState((unsigned short)'X') & 0x8000 ) ? true : false;
    keypad[1]   = ( GetAsyncKeyState((unsigned short)'1') & 0x8000 ) ? true : false;
    keypad[2]   = ( GetAsyncKeyState((unsigned short)'2') & 0x8000 ) ? true : false;
    keypad[3]   = ( GetAsyncKeyState((unsigned short)'3') & 0x8000 ) ? true : false;
    keypad[4]   = ( GetAsyncKeyState((unsigned short)'Q') & 0x8000 ) ? true : false;
    keypad[5]   = ( GetAsyncKeyState((unsigned short)'W') & 0x8000 ) ? true : false;
    keypad[6]   = ( GetAsyncKeyState((unsigned short)'E') & 0x8000 ) ? true : false;
    keypad[7]   = ( GetAsyncKeyState((unsigned short)'A') & 0x8000 ) ? true : false;
    keypad[8]   = ( GetAsyncKeyState((unsigned short)'S') & 0x8000 ) ? true : false;
    keypad[9]   = ( GetAsyncKeyState((unsigned short)'D') & 0x8000 ) ? true : false;
    keypad[10]  = ( GetAsyncKeyState((unsigned short)'Z') & 0x8000 ) ? true : false;
    keypad[11]  = ( GetAsyncKeyState((unsigned short)'C') & 0x8000 ) ? true : false;
    keypad[12]  = ( GetAsyncKeyState((unsigned short)'4') & 0x8000 ) ? true : false;
    keypad[13]  = ( GetAsyncKeyState((unsigned short)'R') & 0x8000 ) ? true : false;
    keypad[14]  = ( GetAsyncKeyState((unsigned short)'F') & 0x8000 ) ? true : false;
    keypad[15]  = ( GetAsyncKeyState((unsigned short)'V') & 0x8000 ) ? true : false;
}

void Console::updateScreen()
{
    DWORD bytesWritten;
    WriteConsoleOutputCharacterW(
        hConsole,
        screen,
        CONSOLE_SCREEN_SIZE,
        { 0, 0 },
        &bytesWritten
    );
}

void Console::beep()
{
    Beep(800, 16);
}
