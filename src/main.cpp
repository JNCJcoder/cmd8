#include <thread>
#include <chrono>

#include "console.hpp"
#include "chip8.hpp"

constexpr uint16_t  TICKRATE            = 20u;
constexpr uint16_t  FPS                 = 60u;
constexpr uint16_t  MS_PER_SECOND       = 1000u;
constexpr float     FRAME_TIME_FLOAT    = (MS_PER_SECOND / FPS);

constexpr auto FRAME_TIME_MS    = std::chrono::milliseconds(static_cast<int>(FRAME_TIME_FLOAT));
constexpr auto HALF_FRAME       = FRAME_TIME_MS / 2;

int main(int argc, char const *argv[])
{
    Console console;
    Chip8 chip8;

    bool isRunning = chip8.loadRom(argv[1]);

    if (!isRunning)
    {
        return 1;
    }

    auto last = std::chrono::steady_clock::now();
    auto origin = last + HALF_FRAME;

    uint32_t frameCatchUp = 0u;

    while (isRunning)
    {
        auto now = std::chrono::steady_clock::now();
        last += std::chrono::duration_cast<std::chrono::milliseconds>(now - last);

        frameCatchUp = 0u;

        while (origin < last - FRAME_TIME_MS && frameCatchUp < 2u)
        {
            origin += FRAME_TIME_MS;
            frameCatchUp++;

            console.checkKeys(chip8.keypad);

            for (uint16_t index = 0u; index < TICKRATE; index++)
            {
                chip8.fetchDecodeAndExecute();
            }

            chip8.updateTimers();

            if (chip8.soundTimer == 1u)
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
