#include <cstdint>
#include <windows.h>
#include "console.cpp"

namespace Console
{
    void drawBuffer(uint32_t* buffer)
    {
        for (int index = 0; index < SCREEN_SIZE; index++)
        {
            screen[index] = buffer[index] > 0 ? 0x2588 : 0x20; // 0x2591
        }
    }

    uint8_t checkKeys()
    {
        if ( GetAsyncKeyState((unsigned short)'1') & 0x8000 )      return 0;
        else if ( GetAsyncKeyState((unsigned short)'2') & 0x8000 ) return 1;
        else if ( GetAsyncKeyState((unsigned short)'3') & 0x8000 ) return 2;
        else if ( GetAsyncKeyState((unsigned short)'4') & 0x8000 ) return 3;
        else if ( GetAsyncKeyState((unsigned short)'Q') & 0x8000 ) return 4;
        else if ( GetAsyncKeyState((unsigned short)'W') & 0x8000 ) return 5;
        else if ( GetAsyncKeyState((unsigned short)'E') & 0x8000 ) return 6;
        else if ( GetAsyncKeyState((unsigned short)'R') & 0x8000 ) return 7;
        else if ( GetAsyncKeyState((unsigned short)'A') & 0x8000 ) return 8;
        else if ( GetAsyncKeyState((unsigned short)'S') & 0x8000 ) return 9;
        else if ( GetAsyncKeyState((unsigned short)'D') & 0x8000 ) return 10;
        else if ( GetAsyncKeyState((unsigned short)'F') & 0x8000 ) return 11;
        else if ( GetAsyncKeyState((unsigned short)'Z') & 0x8000 ) return 12;
        else if ( GetAsyncKeyState((unsigned short)'X') & 0x8000 ) return 13;
        else if ( GetAsyncKeyState((unsigned short)'C') & 0x8000 ) return 14;
        else if ( GetAsyncKeyState((unsigned short)'V') & 0x8000 ) return 15;
        return 16;
    }

} // namespace Console
