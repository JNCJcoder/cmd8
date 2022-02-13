#include <cstdint>
#include <cstring>
#include <fstream>
#include <chrono>
#include <thread>
#include <random>

#define MAX_MEMORY      4096
#define FONTSET_SIZE    80
#define FONTSET_START   0x50
#define START_ADDRESS   0x200
#define REGISTERS       16
#define STACK_LEVELS    16

constexpr uint8_t fontset[FONTSET_SIZE] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

class Chip8
{
    private:
        uint16_t    pc;
        uint16_t    opcode;
        uint16_t    index;
        uint16_t    stack[STACK_LEVELS];
        uint8_t     registers[REGISTERS];
        uint8_t     memory[MAX_MEMORY];
        uint8_t     sp;
        uint8_t     delayTimer;
        uint8_t     soundTimer;

        std::default_random_engine randGen;
        std::uniform_int_distribution<uint8_t> randByte;

        void Table0();
        void Table8();
        void TableE();
        void TableF();

        void OP_NULL();     // Do nothing
        void OP_00E0();     // CLS
        void OP_00EE();     // RET
        void OP_1nnn();     // JP address
        void OP_2nnn();     // CALL address
        void OP_3xkk();     // SE Vx, byte
        void OP_4xkk();     // SNE Vx, byte
        void OP_5xy0();     // SE Vx, Vy
        void OP_6xkk();     // LD Vx, byte
        void OP_7xkk();     // ADD Vx, byte
        void OP_8xy0();     // LD Vx, Vy
        void OP_8xy1();     // OR Vx, Vy
        void OP_8xy2();     // AND Vx, Vy
        void OP_8xy3();     // XOR Vx, Vy
        void OP_8xy4();     // ADD Vx, Vy
        void OP_8xy5();     // SUB Vx, Vy
        void OP_8xy6();     // SHR Vx
        void OP_8xy7();     // SUBN Vx, Vy
        void OP_8xyE();     // SHL Vx
        void OP_9xy0();     // SNE Vx, Vy
        void OP_Annn();     // LD I, address
        void OP_Bnnn();     // JP V0, address
        void OP_Cxkk();     // RND Vx, byte
        void OP_Dxyn();     // DRW Vx, Vy, height
        void OP_Ex9E();     // SKP Vx
        void OP_ExA1();     // SKNP Vx
        void OP_Fx07();     // LD Vx, DT
        void OP_Fx0A();     // LD Vx, K
        void OP_Fx15();     // LD DT, Vx
        void OP_Fx18();     // LD ST, Vx
        void OP_Fx1E();     // ADD I, Vx
        void OP_Fx29();     // LD F, Vx
        void OP_Fx33();     // LD B, Vx
        void OP_Fx55();     // LD [I], Vx
        void OP_Fx65();     // LD Vx, [I]

        typedef void (Chip8::*Chip8Func)();
        Chip8Func table[0xF + 1]{&Chip8::OP_NULL};
        Chip8Func table0[0xE + 1]{&Chip8::OP_NULL};
        Chip8Func table8[0xE + 1]{&Chip8::OP_NULL};
        Chip8Func tableE[0xE + 1]{&Chip8::OP_NULL};
        Chip8Func tableF[0x65 + 1]{&Chip8::OP_NULL};

    public:
        Chip8();
        bool LoadROM(const char* fileName);
        void FetchDecodeAndExecute();

        uint32_t    video[SCREEN_SIZE];
        uint8_t     keypad;
};
