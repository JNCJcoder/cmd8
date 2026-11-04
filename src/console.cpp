#include "console.hpp"

Console::Console()
{
    if (!initVideo())
    {
        closeVideo();
        return;
    }

    if (!initInput())
    {
        closeVideo();
        closeInput();
        return;
    }

    if (!initAudio())
    {
        closeVideo();
        closeInput();
        closeAudio();
        return;
    }

    initialized = true;
}

Console::~Console()
{
    closeVideo();
    closeInput();
    closeAudio();
}

bool Console::isInitialized() const
{
    return initialized;
}

bool Console::initVideo()
{
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hConsole == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    COORD coord = { CONSOLE_SCREEN_WIDTH, CONSOLE_SCREEN_HEIGHT };

    if (!SetConsoleScreenBufferSize(hConsole, coord))
    {
        return false;
    }

    if (!SetConsoleActiveScreenBuffer(hConsole))
    {
        return false;
    }

    CONSOLE_FONT_INFOEX cfi;

	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = CONSOLE_PIXEL_SIZE;
	cfi.dwFontSize.Y = CONSOLE_PIXEL_SIZE;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

    wcscpy(cfi.FaceName, L"Consolas");

    if (!SetCurrentConsoleFontEx(hConsole, FALSE, &cfi))
    {
        return false;
    }

    return true;
}

bool Console::initInput()
{
    hConsoleIn  = GetStdHandle(STD_INPUT_HANDLE);

    if (hConsoleIn == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    DWORD mode;

    if (!GetConsoleMode(hConsoleIn, &mode))
    {
        return false;
    }

    mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

    if (!SetConsoleMode(hConsoleIn, mode))
    {
        return false;
    }

    return true;
}

bool Console::initAudio()
{
    WAVEFORMATEX format{};

    format.wFormatTag       = WAVE_FORMAT_PCM;
    format.nChannels        = 1;
    format.nSamplesPerSec   = AUDIO_SAMPLE_RATE;
    format.wBitsPerSample   = 16;
    format.nBlockAlign      = 2;
    format.nAvgBytesPerSec  = (AUDIO_SAMPLE_RATE * 2u);

    audioEvent  = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    stopEvent   = CreateEvent(nullptr, TRUE, FALSE, nullptr);

    if (!audioEvent || !stopEvent)
    {
        return false;
    }

    MMRESULT result = waveOutOpen(
        &audioDevice,
        WAVE_MAPPER,
        &format,
        reinterpret_cast<DWORD_PTR>(audioEvent),
        0,
        CALLBACK_EVENT
    );

    if (result != MMSYSERR_NOERROR)
    {
        return false;
    }

    for (uint8_t index = 0u; index < AUDIO_BUFFER_COUNT; ++index)
    {
        buffers[index].header.lpData            = reinterpret_cast<LPSTR>(buffers[index].samples);
        buffers[index].header.dwBufferLength    = sizeof(buffers[index].samples);
        buffers[index].header.dwFlags           = 0;

        generateAudio(buffers[index]);

        result = waveOutPrepareHeader(audioDevice, &buffers[index].header, sizeof(WAVEHDR));

        if (result != MMSYSERR_NOERROR)
        {
            return false;
        }

        result = waveOutWrite(audioDevice, &buffers[index].header, sizeof(WAVEHDR));

        if (result != MMSYSERR_NOERROR)
        {
            return false;
        }
    }

    audioThread = std::thread(&Console::audioLoop, this);

    return true;
}

void Console::closeVideo()
{
    if (!hConsole && hConsole != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hConsole);
        hConsole = INVALID_HANDLE_VALUE;
    }
}

void Console::closeInput()
{
    if (hConsoleIn && hConsoleIn != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hConsoleIn);
        hConsoleIn = INVALID_HANDLE_VALUE;
    }
}

void Console::closeAudio()
{
    audioPlaying.store(false);

    if (stopEvent)
    {
        SetEvent(stopEvent);
    }

    if (audioThread.joinable())
    {
        audioThread.join();
    }

    if (audioDevice)
    {
        waveOutReset(audioDevice);

        for (uint8_t index = 0u; index < AUDIO_BUFFER_COUNT; ++index)
        {
            waveOutUnprepareHeader(audioDevice, &buffers[index].header, sizeof(WAVEHDR));
        }

        waveOutClose(audioDevice);
        audioDevice = nullptr;
    }

    if (audioEvent)
    {
        CloseHandle(audioEvent);
        audioEvent = nullptr;
    }

    if (stopEvent)
    {
        CloseHandle(stopEvent);
        stopEvent = nullptr;
    }
}


void Console::updateInput(bool* keypad1, bool* keypad2, bool* isRunning)
{
    DWORD events;

    GetNumberOfConsoleInputEvents(hConsoleIn, &events);

    if (events == 0)
    {
        return;
    }

    DWORD eventsRead;
    INPUT_RECORD record;

    ReadConsoleInputW(hConsoleIn, &record, TRUE, &eventsRead);

    if (record.EventType != KEY_EVENT)
    {
        return;
    }

    switch (record.Event.KeyEvent.wVirtualKeyCode)
    {
        case 'X':   keypad1[0x00u] = record.Event.KeyEvent.bKeyDown;     break;
        case '1':   keypad1[0x01u] = record.Event.KeyEvent.bKeyDown;     break;
        case '2':   keypad1[0x02u] = record.Event.KeyEvent.bKeyDown;     break;
        case '3':   keypad1[0x03u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'Q':   keypad1[0x04u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'W':   keypad1[0x05u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'E':   keypad1[0x06u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'A':   keypad1[0x07u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'S':   keypad1[0x08u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'D':   keypad1[0x09u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'Z':   keypad1[0x0Au] = record.Event.KeyEvent.bKeyDown;     break;
        case 'C':   keypad1[0x0Bu] = record.Event.KeyEvent.bKeyDown;     break;
        case '4':   keypad1[0x0Cu] = record.Event.KeyEvent.bKeyDown;     break;
        case 'R':   keypad1[0x0Du] = record.Event.KeyEvent.bKeyDown;     break;
        case 'F':   keypad1[0x0Eu] = record.Event.KeyEvent.bKeyDown;     break;
        case 'V':   keypad1[0x0Fu] = record.Event.KeyEvent.bKeyDown;     break;

        case 'M':   keypad2[0x00u] = record.Event.KeyEvent.bKeyDown;     break;
        case '6':   keypad2[0x01u] = record.Event.KeyEvent.bKeyDown;     break;
        case '7':   keypad2[0x02u] = record.Event.KeyEvent.bKeyDown;     break;
        case '8':   keypad2[0x03u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'Y':   keypad2[0x04u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'U':   keypad2[0x05u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'I':   keypad2[0x06u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'H':   keypad2[0x07u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'J':   keypad2[0x08u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'K':   keypad2[0x09u] = record.Event.KeyEvent.bKeyDown;     break;
        case 'N':   keypad2[0x0Au] = record.Event.KeyEvent.bKeyDown;     break;
        case ',':   keypad2[0x0Bu] = record.Event.KeyEvent.bKeyDown;     break;
        case '9':   keypad2[0x0Cu] = record.Event.KeyEvent.bKeyDown;     break;
        case 'O':   keypad2[0x0Du] = record.Event.KeyEvent.bKeyDown;     break;
        case 'L':   keypad2[0x0Eu] = record.Event.KeyEvent.bKeyDown;     break;
        case '.':   keypad2[0x0Fu] = record.Event.KeyEvent.bKeyDown;     break;

        case 'P':   *isRunning = false;     break;
    }
}

void Console::updateScreen(uint8_t* buffer, uint8_t* color, uint8_t backgroundColor)
{
    const uint8_t BACKGROUND_COLORS[4] = {
        0x11u,  // Blue
        0x00u,  // Black
        0x22u,  // Green
        0x44u   // Red
    };

    const uint8_t FOREGROUND_COLORS[8] = {
        0x00u,  // Black
        0xCCu,  // Red
        0x99u,  // Blue
        0xDDu,  // Purple
        0xAAu,  // Green
        0xEEu,  // Yellow
        0xBBu,  // Cyan
        0xFFu   // White
    };

    for (uint16_t y = 0u; y < CONSOLE_SCREEN_HEIGHT; ++y)
    {
        for (uint16_t x = 0u; x < CONSOLE_SCREEN_WIDTH; ++x)
        {
            const uint16_t index = y * CONSOLE_SCREEN_WIDTH + x;
            const uint8_t regionIndex = (y / 4u) * 8u + (x / 8u);

            const uint8_t pixelColor = color[regionIndex] & 0x07u;

            screen[index].Char.UnicodeChar = L'\u2588';

            if (buffer[index])
            {
                screen[index].Attributes = FOREGROUND_COLORS[pixelColor];
            }
            else
            {
                screen[index].Attributes = BACKGROUND_COLORS[backgroundColor];
            }
        }
    }

    COORD size = { CONSOLE_SCREEN_WIDTH, CONSOLE_SCREEN_HEIGHT };
    COORD coord = { 0,0 };
    SMALL_RECT screenRect = { 0, 0, CONSOLE_SCREEN_WIDTH - 1, CONSOLE_SCREEN_HEIGHT - 1 };

    WriteConsoleOutputW(hConsole, screen, size, coord, &screenRect);
}

void Console::updateSound(uint8_t soundTimer, uint16_t frequency)
{
    setAudioFrequency(frequency);

    bool shouldPlay = (soundTimer > 0u);

    if (shouldPlay == soundActive)
    {
        return;
    }

    soundActive = shouldPlay;

    audioPlaying.store(shouldPlay);
}

void Console::audioLoop()
{
    HANDLE events[2] = {
        stopEvent,
        audioEvent
    };

    while (true)
    {
        DWORD result = WaitForMultipleObjects(2, events, FALSE, INFINITE);

        if (result == WAIT_OBJECT_0)
        {
            break;
        }

        if (result != (WAIT_OBJECT_0 + 1))
        {
            break;
        }

        for (uint8_t index = 0u; index < AUDIO_BUFFER_COUNT; ++index)
        {
            AudioBuffer& buffer = buffers[index];

            if (!(buffer.header.dwFlags & WHDR_DONE))
            {
                continue;
            }

            generateAudio(buffer);

            MMRESULT status = waveOutWrite(audioDevice, &buffer.header, sizeof(WAVEHDR));

            if (status != MMSYSERR_NOERROR)
            {
                break;
            }
        }
    }
}

void Console::setAudioFrequency(uint16_t value)
{
    if (currentFrequency == value)
    {
        return;
    }

    currentFrequency = value;

    const double cpuClock = 1760900.0;

    const double frequency = (cpuClock / 32.0) / ((value & 0xFFu) + 1u);

    audioFrequency.store(frequency);
}

void Console::generateAudio(AudioBuffer& buffer)
{
    const bool      isPlaying = audioPlaying.load();
    const double    frequency = audioFrequency.load();

    const double phaseStep  = (frequency / AUDIO_SAMPLE_RATE);

    const double TAU        = (3.14159265358979323846 * 2.0);
    const double amplitude  = 16000;

    for (uint16_t i = 0u; i < AUDIO_BUFFER_SAMPLES; ++i)
    {
        double sample = 0.0;

        if (isPlaying)
        {
            sample = static_cast<int16_t>(sin(TAU * audioPhase) * amplitude);
        }

        buffer.samples[i] = sample;

        audioPhase += phaseStep;

        if (audioPhase >= 1.0)
        {
            audioPhase -= 1.0;
        }
    }
}
