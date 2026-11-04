#ifndef __CONSOLE_HPP__
#define __CONSOLE_HPP__

#define WIN32_LEAN_AND_MEAN
#define NOCOMM

#include <cstdint>
#include <cmath>
#include <atomic>
#include <thread>

#include <windows.h>
#include <mmsystem.h>

constexpr uint8_t   CONSOLE_PIXEL_SIZE    = 10u;
constexpr uint8_t   CONSOLE_SCREEN_WIDTH  = 64u;
constexpr uint8_t   CONSOLE_SCREEN_HEIGHT = 32u;
constexpr uint16_t  CONSOLE_SCREEN_SIZE   = (CONSOLE_SCREEN_WIDTH * CONSOLE_SCREEN_HEIGHT);

constexpr DWORD     AUDIO_SAMPLE_RATE       = 48000;
constexpr double    AUDIO_SAMPLE_HALF_RATE  = (AUDIO_SAMPLE_RATE / 2.0);
constexpr uint8_t   AUDIO_BUFFER_COUNT      = 2u;
constexpr uint16_t  AUDIO_BUFFER_SAMPLES    = 2048u;

class Console
{
    private:
        bool initialized = false;

        // Video
        CHAR_INFO screen[CONSOLE_SCREEN_SIZE];

        HANDLE hConsole = INVALID_HANDLE_VALUE;

        // Input
        HANDLE hConsoleIn = INVALID_HANDLE_VALUE;
        
        // Audio
        std::atomic<bool>   audioPlaying{false};
        std::atomic<double> audioFrequency{0};

        struct AudioBuffer
        {
            WAVEHDR header{};
            int16_t samples[AUDIO_BUFFER_SAMPLES]{};
        };

        AudioBuffer buffers[AUDIO_BUFFER_COUNT];

        HWAVEOUT audioDevice = nullptr;

        HANDLE audioEvent   = INVALID_HANDLE_VALUE;
        HANDLE stopEvent    = INVALID_HANDLE_VALUE;

        std::thread audioThread;

        bool        soundActive         = false;
        uint16_t    currentFrequency    = 0xFFFFu;
        double      audioPhase          = 0.0;

        void setAudioFrequency(uint16_t frequency);
        void generateAudio(AudioBuffer& buffer);
        void audioLoop();

        bool initVideo();
        bool initInput();
        bool initAudio();

        void closeVideo();
        void closeInput();
        void closeAudio();
    
    public:
        Console();
        ~Console();

        bool isInitialized() const;

        // Video
        void updateScreen(uint8_t* buffer, uint8_t* color, uint8_t backgroundColor);

        // Input
        void updateInput(bool* keypad1, bool* keypad2, bool* isRunning);

        // Audio
        void updateSound(uint8_t soundTimer, uint16_t frequency);
};

#endif // __CONSOLE_HPP__