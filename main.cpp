#include <thread>
#include <chrono>
#include "console_extend.cpp"
#include "chip8.cpp"

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

        std::this_thread::sleep_for(std::chrono::microseconds(1200));
    }

    Console::exit();
    return 0;
}
