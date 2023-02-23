#ifndef __CONSOLE_HPP__
#define __CONSOLE_HPP__

#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#include <windows.h>
#include <cstdint>

constexpr int SCREEN_WIDTH  =  64;
constexpr int SCREEN_HEIGHT =  32;
constexpr int SCREEN_SIZE   =  SCREEN_WIDTH * SCREEN_HEIGHT;

namespace Console
{
    extern wchar_t *screen;
    extern HANDLE hConsole;

    void init();
    void draw(const int x, const int y, short character);
    void drawBuffer(uint32_t* buffer);
    uint8_t checkKeys();
    void updateScreen();
    void exit();
}

#endif // __CONSOLE_HPP__