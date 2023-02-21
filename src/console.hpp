#ifndef __CONSOLE_HPP__
#define __CONSOLE_HPP__

#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#include <windows.h>

constexpr int SCREEN_WIDTH  =  64;
constexpr int SCREEN_HEIGHT =  32;
constexpr int SCREEN_SIZE   =  SCREEN_WIDTH * SCREEN_HEIGHT;

namespace Console
{
    wchar_t *screen = new wchar_t[SCREEN_SIZE];
    HANDLE hConsole;

    void init();
    void draw(const int x, const int y, short character);
    void updateScreen();
    void exit();
}

#endif // __CONSOLE_HPP__