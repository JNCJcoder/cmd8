#include <thread>
#include <chrono>
#include "console.hpp"
#include "chip8.hpp"

int main(int argc, char const *argv[])
{
    Console console;
    Chip8 chip8;
    bool isRunning = chip8.loadRom(argv[1]);

    const int TICKRATE = 20;
    const int FPS = 60;
    const int MS_PER_SECOND = 1000;
    const float FRAME_TIME_FLOAT = MS_PER_SECOND / FPS;
    const auto FRAME_TIME_MS = std::chrono::milliseconds((int)FRAME_TIME_FLOAT);
    const auto HALF_FRAME = FRAME_TIME_MS / 2;

    using clock = std::chrono::steady_clock;

    auto last = clock::now();
    auto origin = last + HALF_FRAME;

    while(isRunning)
    {
        auto now = clock::now();
        last += std::chrono::duration_cast<std::chrono::milliseconds>(now - last);

        int frameCatchUp = 0;
        while (origin < last - FRAME_TIME_MS && frameCatchUp < 2)
        {
            origin += FRAME_TIME_MS;
            frameCatchUp++;

            chip8.keypad = console.checkKeys();

            for (int index = 0; index < TICKRATE; index++)
            {
                chip8.fetchDecodeAndExecute();
            }

            chip8.updateTimers();
            if(chip8.soundTimer == 1)
            {
                console.beep();
            }

            console.drawBuffer(chip8.video);
            console.updateScreen();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}
