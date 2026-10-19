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
    screen[y * CONSOLE_SCREEN_WIDTH + x] = character;
}

void Console::drawBuffer(uint32_t* buffer)
{
    for (uint16_t index = 0; index < CONSOLE_SCREEN_SIZE; ++index)
    {
        screen[index] = buffer[index] > 0 ? 0x2588 : 0x20; // 0x2591
    }
}

void Console::checkKeys(bool* keypad)
{
    keypad[0x00u] = ( GetAsyncKeyState((unsigned short)'X') & 0x8000 ) ? true : false;
    keypad[0x01u] = ( GetAsyncKeyState((unsigned short)'1') & 0x8000 ) ? true : false;
    keypad[0x02u] = ( GetAsyncKeyState((unsigned short)'2') & 0x8000 ) ? true : false;
    keypad[0x03u] = ( GetAsyncKeyState((unsigned short)'3') & 0x8000 ) ? true : false;
    keypad[0x04u] = ( GetAsyncKeyState((unsigned short)'Q') & 0x8000 ) ? true : false;
    keypad[0x05u] = ( GetAsyncKeyState((unsigned short)'W') & 0x8000 ) ? true : false;
    keypad[0x06u] = ( GetAsyncKeyState((unsigned short)'E') & 0x8000 ) ? true : false;
    keypad[0x07u] = ( GetAsyncKeyState((unsigned short)'A') & 0x8000 ) ? true : false;
    keypad[0x08u] = ( GetAsyncKeyState((unsigned short)'S') & 0x8000 ) ? true : false;
    keypad[0x09u] = ( GetAsyncKeyState((unsigned short)'D') & 0x8000 ) ? true : false;
    keypad[0x0Au] = ( GetAsyncKeyState((unsigned short)'Z') & 0x8000 ) ? true : false;
    keypad[0x0Bu] = ( GetAsyncKeyState((unsigned short)'C') & 0x8000 ) ? true : false;
    keypad[0x0Cu] = ( GetAsyncKeyState((unsigned short)'4') & 0x8000 ) ? true : false;
    keypad[0x0Du] = ( GetAsyncKeyState((unsigned short)'R') & 0x8000 ) ? true : false;
    keypad[0x0Eu] = ( GetAsyncKeyState((unsigned short)'F') & 0x8000 ) ? true : false;
    keypad[0x0Fu] = ( GetAsyncKeyState((unsigned short)'V') & 0x8000 ) ? true : false;
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
    Beep(800u, 16u);
}
