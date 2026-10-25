#include "console.hpp"

Console::Console()
{
    hConsole    = GetStdHandle(STD_OUTPUT_HANDLE);
    hConsoleIn  = GetStdHandle(STD_INPUT_HANDLE);

    COORD coord = { CONSOLE_SCREEN_WIDTH, CONSOLE_SCREEN_HEIGHT };

    if (!SetConsoleScreenBufferSize(hConsole, coord))
    {
        return;
    }

    if (!SetConsoleActiveScreenBuffer(hConsole))
    {
        return;
    }

    CONSOLE_FONT_INFOEX cfi;

	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = CONSOLE_PIXEL_SIZE;
	cfi.dwFontSize.Y = CONSOLE_PIXEL_SIZE;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

    wcscpy(cfi.FaceName, L"Consolas");

    if (!SetCurrentConsoleFontEx(hConsole, FALSE, &cfi))
    {
        return;
    }

    DWORD mode;

    if (!GetConsoleMode(hConsoleIn, &mode))
    {
        return;
    }

    mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

    if (!SetConsoleMode(hConsoleIn, mode))
    {
        return;
    }
}

Console::~Console()
{
    CloseHandle(hConsole);
    CloseHandle(hConsoleIn);
}

void Console::drawBuffer(uint32_t* buffer)
{
    for (uint16_t index = 0; index < CONSOLE_SCREEN_SIZE; ++index)
    {
        screen[index].Char.UnicodeChar = L'\u2588';
        screen[index].Attributes = buffer[index] & 0xF; 
    }
}

void Console::checkKeys(bool* keypad)
{
    DWORD events;

    GetNumberOfConsoleInputEvents(hConsoleIn, &events);

    if (events == 0)
    {
        return;
    }

    DWORD eventsRead;
    INPUT_RECORD record;

    ReadConsoleInputW(hConsoleIn, &record, TRUE, &eventsRead);

    if (record.EventType != KEY_EVENT)
    {
        return;
    }

    switch (record.Event.KeyEvent.wVirtualKeyCode)
    {
        case 'X':   keypad[0x00u] = record.Event.KeyEvent.bKeyDown;     break;
        case '1':   keypad[0x01u] = record.Event.KeyEvent.bKeyDown;     break;
        case '2':   keypad[0x02u] = record.Event.KeyEvent.bKeyDown;     break;
        case '3':   keypad[0x03u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'Q':   keypad[0x04u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'W':   keypad[0x05u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'E':   keypad[0x06u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'A':   keypad[0x07u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'S':   keypad[0x08u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'D':   keypad[0x09u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'Z':   keypad[0x0Au] = record.Event.KeyEvent.bKeyDown;     break;
        case 'C':   keypad[0x0Bu] = record.Event.KeyEvent.bKeyDown;     break;
        case '4':   keypad[0x0Cu] = record.Event.KeyEvent.bKeyDown;     break;
        case 'R':   keypad[0x0Du] = record.Event.KeyEvent.bKeyDown;     break;
        case 'F':   keypad[0x0Eu] = record.Event.KeyEvent.bKeyDown;     break;
        case 'V':   keypad[0x0Fu] = record.Event.KeyEvent.bKeyDown;     break;
    }
}

void Console::updateScreen()
{
    COORD size = { CONSOLE_SCREEN_WIDTH, CONSOLE_SCREEN_HEIGHT };
    COORD coord = { 0,0 };
    SMALL_RECT screenRect = { 0, 0, CONSOLE_SCREEN_WIDTH - 1, CONSOLE_SCREEN_HEIGHT - 1 };

    WriteConsoleOutputW(hConsole, screen, size, coord, &screenRect);
}

void Console::beep()
{
    Beep(800u, 16u);
}
