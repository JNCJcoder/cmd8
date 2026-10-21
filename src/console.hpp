#ifndef __CONSOLE_HPP__
#define __CONSOLE_HPP__

#define WIN32_LEAN_AND_MEAN
#define NOCOMM

#include <cstdint>
#include <windows.h>

constexpr uint8_t   CONSOLE_SCREEN_WIDTH  = 64u;
constexpr uint8_t   CONSOLE_SCREEN_HEIGHT = 32u;
constexpr uint16_t  CONSOLE_SCREEN_SIZE   = (CONSOLE_SCREEN_WIDTH * CONSOLE_SCREEN_HEIGHT);

class Console
{
    private:
        wchar_t *screen     = new wchar_t[CONSOLE_SCREEN_SIZE];
        HANDLE  hConsole    = INVALID_HANDLE_VALUE;
    
    public:
        Console();
        ~Console();

        void drawBuffer(uint32_t* buffer);
        void checkKeys(bool* keypad);
        void updateScreen();
        void beep();
};

#endif // __CONSOLE_HPP__