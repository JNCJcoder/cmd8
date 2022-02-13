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
    ((*this).*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8()
{
    ((*this).*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE()
{
    ((*this).*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF()
{
    ((*this).*(tableF[opcode & 0x00FFu]))();
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
    uint16_t address = opcode & 0x0FFFu;

	pc = address;
}

void Chip8::OP_2nnn()
{
    uint16_t address = opcode & 0x0FFFu;

    stack[sp] = pc;
	++sp;
	pc = address;
}

void Chip8::OP_3xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] == byte) pc += 2;
}

void Chip8::OP_4xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] != byte) pc += 2;
}

void Chip8::OP_5xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] == registers[Vy]) pc += 2;
}

void Chip8::OP_6xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = byte;
}

void Chip8::OP_7xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] += byte;
}

void Chip8::OP_8xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] |= registers[Vy];
}

void Chip8::OP_8xy2()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] &= registers[Vy];
}

void Chip8::OP_8xy3()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] ^= registers[Vy];
}

void Chip8::OP_8xy4()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    uint16_t sum = registers[Vx] + registers[Vy];

    if (sum > 255U) registers[0xF] = 1;
    else            registers[0xF] = 0;

    registers[Vx] = sum & 0xFFu;
}

void Chip8::OP_8xy5()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] > registers[Vy])  registers[0xF] = 1;
    else                                registers[0xF] = 0;

    registers[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[0xF] = (registers[Vx] & 0x1u);
    registers[Vx] >>= 1;
}

void Chip8::OP_8xy7()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vy] > registers[Vx])  registers[0xF] = 1;
    else                                registers[0xF] = 0;

    registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8xyE()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
    registers[Vx] <<= 1;
}

void Chip8::OP_9xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] != registers[Vy]) pc += 2;
}

void Chip8::OP_Annn()
{
    uint16_t address = opcode & 0x0FFFu;

    index = address;
}

void Chip8::OP_Bnnn()
{
    uint16_t address = opcode & 0x0FFFu;

    pc = registers[0] + address;
}

void Chip8::OP_Cxkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = randByte(randGen) & byte;
}

void Chip8::OP_Dxyn()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    uint8_t xPos = registers[Vx] % SCREEN_WIDTH;
    uint8_t yPos = registers[Vy] % SCREEN_HEIGHT;

    registers[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row)
    {
        uint8_t spriteByte = memory[index + row];

        for (unsigned int col = 0; col < 8; ++col)
        {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t* screenPixel = &video[(yPos + row) * SCREEN_WIDTH + (xPos + col)];

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
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[Vx];

    if (keypad == key) pc += 2;
}

void Chip8::OP_ExA1()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[Vx];

    if (keypad != key) pc += 2;
}

void Chip8::OP_Fx07()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[Vx] = delayTimer;
}

void Chip8::OP_Fx0A()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    switch (keypad)
    {
        case 0:     registers[Vx] = 0;      break;
        case 1:     registers[Vx] = 1;      break;
        case 2:     registers[Vx] = 2;      break;
        case 3:     registers[Vx] = 3;      break;
        case 4:     registers[Vx] = 4;      break;
        case 5:     registers[Vx] = 5;      break;
        case 6:     registers[Vx] = 6;      break;
        case 7:     registers[Vx] = 7;      break;
        case 8:     registers[Vx] = 8;      break;
        case 9:     registers[Vx] = 9;      break;
        case 10:    registers[Vx] = 10;     break;
        case 11:    registers[Vx] = 11;     break;
        case 12:    registers[Vx] = 12;     break;
        case 13:    registers[Vx] = 13;     break;
        case 14:    registers[Vx] = 14;     break;
        case 15:    registers[Vx] = 15;     break;
        default:        pc -= 2;            break;
    }
}

void Chip8::OP_Fx15()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    delayTimer = registers[Vx];
}

void Chip8::OP_Fx18()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    soundTimer = registers[Vx];
}

void Chip8::OP_Fx1E()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    index += registers[Vx];
}

void Chip8::OP_Fx29()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t digit = registers[Vx];

    index = FONTSET_START + (5 * digit);
}

void Chip8::OP_Fx33()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t value = registers[Vx];

    memory[index + 2] = value % 10;
    value /= 10;

    memory[index + 1] = value % 10;
    value /= 10;

    memory[index] = value % 10;
}

void Chip8::OP_Fx55()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; ++i)
    {
        memory[index + i] = registers[i];
    }
}

void Chip8::OP_Fx65()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; ++i)
    {
        registers[i] = memory[index + i];
    }
}