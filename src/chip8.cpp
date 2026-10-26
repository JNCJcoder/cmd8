#include "chip8.hpp"

Chip8::Chip8(): randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    pc      = START_ADDRESS;
    opcode  = 0x0000u;
    index   = 0x0000u;
    sp      = 0x00u;
    
    delayTimer = 0u;
    soundTimer = 0u;

    keyPressed = 0xFFu;

    memset(video, 0x00000000u, sizeof(video));
    memset(memory, 0x00u, MAX_MEMORY);
    memset(keypad, false, sizeof(keypad));
    memcpy(memory + FONTSET_START, fontset, FONTSET_SIZE);

    randByte = std::uniform_int_distribution<uint8_t>(0x00u, 0xFFu);

    for (auto& opFunc : table)
    {
        opFunc = &Chip8::OP_NULL;
    }

    for (auto& opFunc : table0)
    {
        opFunc = &Chip8::OP_NULL;
    }

    for (auto& opFunc : table8)
    {
        opFunc = &Chip8::OP_NULL;
    }

    for (auto& opFunc : tableE)
    {
        opFunc = &Chip8::OP_NULL;
    }

    for (auto& opFunc : tableF)
    {
        opFunc = &Chip8::OP_NULL;
    }

    table[0x00u]    = &Chip8::Table0;
    table[0x01u]    = &Chip8::OP_1nnn;
    table[0x02u]    = &Chip8::OP_2nnn;
    table[0x03u]    = &Chip8::OP_3xkk;
    table[0x04u]    = &Chip8::OP_4xkk;
    table[0x05u]    = &Chip8::OP_5xy0;
    table[0x06u]    = &Chip8::OP_6xkk;
    table[0x07u]    = &Chip8::OP_7xkk;
    table[0x08u]    = &Chip8::Table8;
    table[0x09u]    = &Chip8::OP_9xy0;
    table[0x0Au]    = &Chip8::OP_Annn;
    table[0x0Bu]    = &Chip8::OP_Bnnn;
    table[0x0Cu]    = &Chip8::OP_Cxkk;
    table[0x0Du]    = &Chip8::OP_Dxyn;
    table[0x0Eu]    = &Chip8::TableE;
    table[0x0Fu]    = &Chip8::TableF;

    table0[0x00u]   = &Chip8::OP_00E0;
    table0[0x0Eu]   = &Chip8::OP_00EE;

    table8[0x00u]   = &Chip8::OP_8xy0;
    table8[0x01u]   = &Chip8::OP_8xy1;
    table8[0x02u]   = &Chip8::OP_8xy2;
    table8[0x03u]   = &Chip8::OP_8xy3;
    table8[0x04u]   = &Chip8::OP_8xy4;
    table8[0x05u]   = &Chip8::OP_8xy5;
    table8[0x06u]   = &Chip8::OP_8xy6;
    table8[0x07u]   = &Chip8::OP_8xy7;
    table8[0x0Eu]   = &Chip8::OP_8xyE;

    tableE[0x01u]   = &Chip8::OP_ExA1;
    tableE[0x0Eu]   = &Chip8::OP_Ex9E;

    tableF[0x07u]   = &Chip8::OP_Fx07;
    tableF[0x0Au]   = &Chip8::OP_Fx0A;
    tableF[0x15u]   = &Chip8::OP_Fx15;
    tableF[0x18u]   = &Chip8::OP_Fx18;
    tableF[0x1Eu]   = &Chip8::OP_Fx1E;
    tableF[0x29u]   = &Chip8::OP_Fx29;
    tableF[0x33u]   = &Chip8::OP_Fx33;
    tableF[0x55u]   = &Chip8::OP_Fx55;
    tableF[0x65u]   = &Chip8::OP_Fx65;
}

bool Chip8::loadRom(const char* fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);

    if (!file)
    {
        return false;
    }

    std::streampos romSize = file.tellg();

    if (romSize <= 0 || romSize > MAX_ROM_SIZE)
    {
        return false;
    }

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(memory + START_ADDRESS), romSize);

    if (!file)
    {
        return false;
    }

    return true;
}

void Chip8::fetchDecodeAndExecute()
{
    opcode = (memory[pc] << 8u) | memory[pc + 1u];

    pc += 2u;

    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();
}

void Chip8::updateTimers()
{
    if (delayTimer > 0u)
    {
        --delayTimer;
    }

    if (soundTimer > 0u)
    {
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
    memset(video, 0x00000000u, sizeof(video));
}

void Chip8::OP_00EE()
{
    pc = stack[--sp];
}

void Chip8::OP_1nnn()
{
	pc = OP_ADDRESS;
}

void Chip8::OP_2nnn()
{
    stack[sp++] = pc;

	pc = OP_ADDRESS;
}

void Chip8::OP_3xkk()
{
    if (registers[OP_HIGH_BYTE] == OP_BYTE)
    {
        pc += 2u;
    }
}

void Chip8::OP_4xkk()
{
    if (registers[OP_HIGH_BYTE] != OP_BYTE)
    {
        pc += 2u;
    }
}

void Chip8::OP_5xy0()
{
    if (registers[OP_HIGH_BYTE] == registers[OP_LOW_BYTE])
    {
        pc += 2u;
    }
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

    registers[0x0Fu] = 0u;
}

void Chip8::OP_8xy2()
{
    registers[OP_HIGH_BYTE] &= registers[OP_LOW_BYTE];

    registers[0x0Fu] = 0u;
}

void Chip8::OP_8xy3()
{
    registers[OP_HIGH_BYTE] ^= registers[OP_LOW_BYTE];

    registers[0x0Fu] = 0u;
}

void Chip8::OP_8xy4()
{
    const uint16_t sum = (registers[OP_HIGH_BYTE] + registers[OP_LOW_BYTE]);

    registers[OP_HIGH_BYTE] = (sum & 0xFFu);

    registers[0x0Fu] = (sum > 0xFFu) ? 1u : 0u;
}

void Chip8::OP_8xy5()
{
    const uint8_t carry = (registers[OP_HIGH_BYTE] >= registers[OP_LOW_BYTE]);

    registers[OP_HIGH_BYTE] = (registers[OP_HIGH_BYTE] - registers[OP_LOW_BYTE]);

    registers[0x0Fu] = carry;
}

void Chip8::OP_8xy6()
{
    const uint8_t carry = (registers[OP_HIGH_BYTE] & 0x01u);

    registers[OP_HIGH_BYTE] = registers[OP_LOW_BYTE] >> 1u;

    registers[0x0Fu] = carry;
}

void Chip8::OP_8xy7()
{
    const uint8_t carry = (registers[OP_LOW_BYTE] >= registers[OP_HIGH_BYTE]);

    registers[OP_HIGH_BYTE] = (registers[OP_LOW_BYTE] - registers[OP_HIGH_BYTE]);

    registers[0x0Fu] = carry;
}

void Chip8::OP_8xyE()
{
    const uint8_t carry = (registers[OP_HIGH_BYTE] & 0x80u) >> 7u;

    registers[OP_HIGH_BYTE] = (registers[OP_LOW_BYTE] << 1u);

    registers[0x0Fu] = carry;
}

void Chip8::OP_9xy0()
{
    if (registers[OP_HIGH_BYTE] != registers[OP_LOW_BYTE])
    {
        pc += 2u;
    }
}

void Chip8::OP_Annn()
{
    index = OP_ADDRESS;
}

void Chip8::OP_Bnnn()
{
    pc = (registers[0x00u] + OP_ADDRESS);
}

void Chip8::OP_Cxkk()
{
    registers[OP_HIGH_BYTE] = (randByte(randGen) & OP_BYTE);
}

void Chip8::OP_Dxyn()
{
    const uint8_t height = OP_NIBBLE;
    const uint8_t xPosition = (registers[OP_HIGH_BYTE] % SCREEN_WIDTH);
    const uint8_t yPosition = (registers[OP_LOW_BYTE] % SCREEN_HEIGHT);

    registers[0x0Fu] = 0u;

    for (uint8_t row = 0u; row < height; ++row)
    {
        uint8_t y = yPosition + row;

        if (y >= SCREEN_HEIGHT)
        {
            break;
        }

        uint8_t spriteByte = memory[index + row];

        for (uint8_t column = 0u; column < 8u; ++column)
        {
            const uint8_t x = xPosition + column;

            if (x >= SCREEN_WIDTH)
            {
                break;
            }

            const uint8_t spritePixel = spriteByte & (0x80u >> column);
            uint32_t* screenPixel = &video[y * SCREEN_WIDTH + x];

            if (spritePixel)
            {
                if (*screenPixel == 0xFFFFFFFFu)
                {
                    registers[0x0Fu] = 1u;
                }

                *screenPixel ^= 0xFFFFFFFFu;
            }
        }
    }
}

void Chip8::OP_Ex9E()
{
    const uint8_t key = registers[OP_HIGH_BYTE];

    if (keypad[key & 0x0Fu])
    {
        pc += 2u;
    }
}

void Chip8::OP_ExA1()
{
    const uint8_t key = registers[OP_HIGH_BYTE];

    if (!keypad[key & 0x0Fu])
    {
        pc += 2u;
    }
}

void Chip8::OP_Fx07()
{
    registers[OP_HIGH_BYTE] = delayTimer;
}

void Chip8::OP_Fx0A()
{
    if (keyPressed != 0xFFu)
    {
        if (!keypad[keyPressed])
        {
            registers[OP_HIGH_BYTE] = keyPressed;
            keyPressed = 0xFFu;
            return;
        }
    }
    else
    {
        for (uint8_t i = 0u; i < KEYPAD_KEYS; i++)
        {
            if (keypad[i])
            {
                keyPressed = i;
                break;
            }
        }
    }

    pc -= 2u;
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
    const uint8_t digit = registers[OP_HIGH_BYTE];

    index = FONTSET_START + (5u * digit);
}

void Chip8::OP_Fx33()
{
    uint8_t value = registers[OP_HIGH_BYTE];

    memory[index + 2u] = value % 10u;
    value /= 10u;

    memory[index + 1u] = value % 10u;
    value /= 10u;

    memory[index] = value % 10u;
}

void Chip8::OP_Fx55()
{
    const uint8_t count = OP_HIGH_BYTE;

    for (uint8_t i = 0u; i <= count; ++i)
    {
        memory[index + i] = registers[i];
    }

    index += count + 1u;
}

void Chip8::OP_Fx65()
{
    const uint8_t count = OP_HIGH_BYTE;

    for (uint8_t i = 0u; i <= count; ++i)
    {
        registers[i] = memory[index + i];
    }

    index += count + 1u;
}