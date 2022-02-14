#include <cstdint>
#include <cstring>
#include <fstream>
#include <chrono>
#include <random>
#include "chip8.hpp"

Chip8::Chip8(): randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    pc      = START_ADDRESS;
    opcode  = 0x0000;
    index   = 0x0000;
    sp      = 0x00;

    memset(video, 0, sizeof(video));
    memset(memory, 0x0, MAX_MEMORY);
    memcpy(memory, fontset, FONTSET_SIZE);

    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

    table[0x0]      = &Chip8::Table0;
    table[0x1]      = &Chip8::OP_1nnn;
    table[0x2]      = &Chip8::OP_2nnn;
    table[0x3]      = &Chip8::OP_3xkk;
    table[0x4]      = &Chip8::OP_4xkk;
    table[0x5]      = &Chip8::OP_5xy0;
    table[0x6]      = &Chip8::OP_6xkk;
    table[0x7]      = &Chip8::OP_7xkk;
    table[0x8]      = &Chip8::Table8;
    table[0x9]      = &Chip8::OP_9xy0;
    table[0xA]      = &Chip8::OP_Annn;
    table[0xB]      = &Chip8::OP_Bnnn;
    table[0xC]      = &Chip8::OP_Cxkk;
    table[0xD]      = &Chip8::OP_Dxyn;
    table[0xE]      = &Chip8::TableE;
    table[0xF]      = &Chip8::TableF;

    table0[0x0]     = &Chip8::OP_00E0;
    table0[0xE]     = &Chip8::OP_00EE;

    table8[0x0]     = &Chip8::OP_8xy0;
    table8[0x1]     = &Chip8::OP_8xy1;
    table8[0x2]     = &Chip8::OP_8xy2;
    table8[0x3]     = &Chip8::OP_8xy3;
    table8[0x4]     = &Chip8::OP_8xy4;
    table8[0x5]     = &Chip8::OP_8xy5;
    table8[0x6]     = &Chip8::OP_8xy6;
    table8[0x7]     = &Chip8::OP_8xy7;
    table8[0xE]     = &Chip8::OP_8xyE;

    tableE[0x1]     = &Chip8::OP_ExA1;
    tableE[0xE]     = &Chip8::OP_Ex9E;

    tableF[0x07]    = &Chip8::OP_Fx07;
    tableF[0x0A]    = &Chip8::OP_Fx0A;
    tableF[0x15]    = &Chip8::OP_Fx15;
    tableF[0x18]    = &Chip8::OP_Fx18;
    tableF[0x1E]    = &Chip8::OP_Fx1E;
    tableF[0x29]    = &Chip8::OP_Fx29;
    tableF[0x33]    = &Chip8::OP_Fx33;
    tableF[0x55]    = &Chip8::OP_Fx55;
    tableF[0x65]    = &Chip8::OP_Fx65;
}

bool Chip8::LoadROM(const char* fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);

    const bool RomOpened = file.is_open();
    if (RomOpened)
    {
        std::streampos RomSize = file.tellg();
        char* buffer = new char[RomSize];

        file.seekg(0, std::ios::beg);
        file.read(buffer, RomSize);

        for (long index = 0; index < RomSize; index++)
            memory[START_ADDRESS + index] = buffer[index];
        
        delete[] buffer;
    }

    file.close();
    return RomOpened;
}

void Chip8::FetchDecodeAndExecute()
{
    opcode = (memory[pc] << 8u) | memory[pc + 1];

    pc += 2;

    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();

    if (delayTimer > 0) --delayTimer;
    if (soundTimer)
    {
        if (soundTimer == 1) Beep(750, 300);
        --soundTimer;
    }
}

void Chip8::Table0()
{
    ((*this).*(table0[OP_NIBBLE]))();
}

void Chip8::Table8()
{
    ((*this).*(table8[OP_NIBBLE]))();
}

void Chip8::TableE()
{
    ((*this).*(tableE[OP_NIBBLE]))();
}

void Chip8::TableF()
{
    ((*this).*(tableF[OP_BYTE]))();
}

void Chip8::OP_NULL()
{

}

void Chip8::OP_00E0()
{
    memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE()
{
    --sp;
    pc = stack[sp];
}

void Chip8::OP_1nnn()
{
	pc = OP_ADDRESS;
}

void Chip8::OP_2nnn()
{
    stack[sp] = pc;
	++sp;
	pc = OP_ADDRESS;
}

void Chip8::OP_3xkk()
{
    if (registers[OP_HIGH_BYTE] == OP_BYTE) pc += 2;
}

void Chip8::OP_4xkk()
{
    if (registers[OP_HIGH_BYTE] != OP_BYTE) pc += 2;
}

void Chip8::OP_5xy0()
{
    if (registers[OP_HIGH_BYTE] == registers[OP_LOW_BYTE]) pc += 2;
}

void Chip8::OP_6xkk()
{
    registers[OP_HIGH_BYTE] = OP_BYTE;
}

void Chip8::OP_7xkk()
{
    registers[OP_HIGH_BYTE] += OP_BYTE;
}

void Chip8::OP_8xy0()
{
    registers[OP_HIGH_BYTE] = registers[OP_LOW_BYTE];
}

void Chip8::OP_8xy1()
{
    registers[OP_HIGH_BYTE] |= registers[OP_LOW_BYTE];
}

void Chip8::OP_8xy2()
{
    registers[OP_HIGH_BYTE] &= registers[OP_LOW_BYTE];
}

void Chip8::OP_8xy3()
{
    registers[OP_HIGH_BYTE] ^= registers[OP_LOW_BYTE];
}

void Chip8::OP_8xy4()
{
    uint16_t sum = registers[OP_HIGH_BYTE] + registers[OP_LOW_BYTE];

    if (sum > 255U) registers[0xF] = 1;
    else            registers[0xF] = 0;

    registers[OP_HIGH_BYTE] = sum & 0xFFu;
}

void Chip8::OP_8xy5()
{
    if (registers[OP_HIGH_BYTE] > registers[OP_LOW_BYTE])   registers[0xF] = 1;
    else                                                    registers[0xF] = 0;

    registers[OP_HIGH_BYTE] -= registers[OP_LOW_BYTE];
}

void Chip8::OP_8xy6()
{
    registers[0xF] = (registers[OP_HIGH_BYTE] & 0x1u);
    registers[OP_HIGH_BYTE] >>= 1;
}

void Chip8::OP_8xy7()
{
    if (registers[OP_LOW_BYTE] > registers[OP_HIGH_BYTE])   registers[0xF] = 1;
    else                                                    registers[0xF] = 0;

    registers[OP_HIGH_BYTE] = registers[OP_LOW_BYTE] - registers[OP_HIGH_BYTE];
}

void Chip8::OP_8xyE()
{
    registers[0xF] = (registers[OP_HIGH_BYTE] & 0x80u) >> 7u;
    registers[OP_HIGH_BYTE] <<= 1;
}

void Chip8::OP_9xy0()
{
    if (registers[OP_HIGH_BYTE] != registers[OP_LOW_BYTE]) pc += 2;
}

void Chip8::OP_Annn()
{
    index = OP_ADDRESS;
}

void Chip8::OP_Bnnn()
{
    pc = registers[0] + OP_ADDRESS;
}

void Chip8::OP_Cxkk()
{
    registers[OP_HIGH_BYTE] = randByte(randGen) & OP_BYTE;
}

void Chip8::OP_Dxyn()
{
    uint8_t height = OP_NIBBLE;
    uint8_t xPosition = registers[OP_HIGH_BYTE] % SCREEN_WIDTH;
    uint8_t yPosition = registers[OP_LOW_BYTE] % SCREEN_HEIGHT;

    registers[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row)
    {
        uint8_t spriteByte = memory[index + row];

        for (unsigned int column = 0; column < 8; ++column)
        {
            uint8_t spritePixel = spriteByte & (0x80u >> column);
            uint32_t* screenPixel = &video[(yPosition + row) * SCREEN_WIDTH + (xPosition + column)];

            if (spritePixel)
            {
                if (*screenPixel == 0xFFFFFFFF)
                {
                    registers[0xF] = 1;
                }

                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

void Chip8::OP_Ex9E()
{
    uint8_t key = registers[OP_HIGH_BYTE];

    if (keypad == key) pc += 2;
}

void Chip8::OP_ExA1()
{
    uint8_t key = registers[OP_HIGH_BYTE];

    if (keypad != key) pc += 2;
}

void Chip8::OP_Fx07()
{
    registers[OP_HIGH_BYTE] = delayTimer;
}

void Chip8::OP_Fx0A()
{
    if (keypad <= 15)   registers[OP_HIGH_BYTE] = keypad;
    else                pc -= 2;
}

void Chip8::OP_Fx15()
{
    delayTimer = registers[OP_HIGH_BYTE];
}

void Chip8::OP_Fx18()
{
    soundTimer = registers[OP_HIGH_BYTE];
}

void Chip8::OP_Fx1E()
{
    index += registers[OP_HIGH_BYTE];
}

void Chip8::OP_Fx29()
{
    uint8_t digit = registers[OP_HIGH_BYTE];

    index = FONTSET_START + (5 * digit);
}

void Chip8::OP_Fx33()
{
    uint8_t value = registers[OP_HIGH_BYTE];

    memory[index + 2] = value % 10;
    value /= 10;

    memory[index + 1] = value % 10;
    value /= 10;

    memory[index] = value % 10;
}

void Chip8::OP_Fx55()
{
    uint8_t count = OP_HIGH_BYTE;

    for (uint8_t i = 0; i <= count; ++i)
    {
        memory[index + i] = registers[i];
    }
}

void Chip8::OP_Fx65()
{
    uint8_t count = OP_HIGH_BYTE;

    for (uint8_t i = 0; i <= count; ++i)
    {
        registers[i] = memory[index + i];
    }
}