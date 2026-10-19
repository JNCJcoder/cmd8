#ifndef __CHIP8_HPP__
#define __CHIP8_HPP__

#include <cstdint>
#include <random>
#include <fstream>
#include <chrono>
#include <cstring>

#define MAX_MEMORY      0x1000u
#define MAX_ROM_SIZE    0x0DFFu
#define FONTSET_SIZE    0x50u
#define FONTSET_START   0x50u
#define START_ADDRESS   0x200u
#define REGISTERS       0x10u
#define STACK_LEVELS    0x10u
#define KEYPAD_KEYS     0x10u

#define OP_ADDRESS      (opcode & 0x0FFFu)
#define OP_BYTE         (opcode & 0x00FFu)
#define OP_NIBBLE       (opcode & 0x000Fu)
#define OP_HIGH_BYTE    ((opcode & 0x0F00u) >> 8u)
#define OP_LOW_BYTE     ((opcode & 0x00F0u) >> 4u)

constexpr uint8_t   SCREEN_WIDTH  = 64u;
constexpr uint8_t   SCREEN_HEIGHT = 32u;
constexpr uint16_t  SCREEN_SIZE   = (SCREEN_WIDTH * SCREEN_HEIGHT);

constexpr uint8_t fontset[FONTSET_SIZE] =
{
    0xF0u, 0x90u, 0x90u, 0x90u, 0xF0u, // 0
    0x20u, 0x60u, 0x20u, 0x20u, 0x70u, // 1
    0xF0u, 0x10u, 0xF0u, 0x80u, 0xF0u, // 2
    0xF0u, 0x10u, 0xF0u, 0x10u, 0xF0u, // 3
    0x90u, 0x90u, 0xF0u, 0x10u, 0x10u, // 4
    0xF0u, 0x80u, 0xF0u, 0x10u, 0xF0u, // 5
    0xF0u, 0x80u, 0xF0u, 0x90u, 0xF0u, // 6
    0xF0u, 0x10u, 0x20u, 0x40u, 0x40u, // 7
    0xF0u, 0x90u, 0xF0u, 0x90u, 0xF0u, // 8
    0xF0u, 0x90u, 0xF0u, 0x10u, 0xF0u, // 9
    0xF0u, 0x90u, 0xF0u, 0x90u, 0x90u, // A
    0xE0u, 0x90u, 0xE0u, 0x90u, 0xE0u, // B
    0xF0u, 0x80u, 0x80u, 0x80u, 0xF0u, // C
    0xE0u, 0x90u, 0x90u, 0x90u, 0xE0u, // D
    0xF0u, 0x80u, 0xF0u, 0x80u, 0xF0u, // E
    0xF0u, 0x80u, 0xF0u, 0x80u, 0x80u  // F
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

        Chip8Func table[0x0Fu + 1u]     { &Chip8::OP_NULL };
        Chip8Func table0[0x0Fu + 1u]    { &Chip8::OP_NULL };
        Chip8Func table8[0x0Fu + 1u]    { &Chip8::OP_NULL };
        Chip8Func tableE[0x0Fu + 1u]    { &Chip8::OP_NULL };
        Chip8Func tableF[0xFFu + 1u]    { &Chip8::OP_NULL };

    public:
        Chip8();

        bool loadRom(const char* fileName);
        void fetchDecodeAndExecute();
        void updateTimers();

        uint32_t    video[SCREEN_SIZE];
        bool        keypad[KEYPAD_KEYS];
        uint8_t     keyPressed;
        uint8_t     soundTimer;
};

#endif // __CHIP8_HPP__