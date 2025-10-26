#ifndef __CONSOLE_HPP__
#define __CONSOLE_HPP__

#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#include <windows.h>
#include <cstdint>

constexpr int CONSOLE_SCREEN_WIDTH  =  64;
constexpr int CONSOLE_SCREEN_HEIGHT =  32;
constexpr int CONSOLE_SCREEN_SIZE   =  CONSOLE_SCREEN_WIDTH * CONSOLE_SCREEN_HEIGHT;

class Console
{
    private:
        wchar_t *screen = new wchar_t[CONSOLE_SCREEN_SIZE];
        HANDLE hConsole = INVALID_HANDLE_VALUE;
    
    public:
        Console();
        ~Console();

        void draw(const uint16_t x, const uint16_t y, short character);
        void drawBuffer(uint32_t* buffer);
        uint8_t checkKeys();
        void updateScreen();
        void beep();
};

#endif // __CONSOLE_HPP__