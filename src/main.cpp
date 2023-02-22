#include "console_extend.hpp"
#include "chip8.hpp"

int main(int argc, char const *argv[])
{
    Console::init();

    Chip8 chip8;
    bool isRunning = chip8.loadRom(argv[1]);

    while(isRunning)
    {
        chip8.fetchDecodeAndExecute();
        chip8.keypad = Console::checkKeys();

        Console::drawBuffer(chip8.video);
        Console::updateScreen();

        Sleep(10);
    }

    Console::exit();
    return 0;
}
