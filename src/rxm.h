#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// mnemonic, whether it uses ModR/M, and immediate size in bytes
typedef struct {
    char mnemonic[16];
    int  has_modrm;
    int  imm_size;
} OpcodeInfo;

// decoded memory operand: base + index*scale + disp
typedef struct {
    int     has_base;
    int     has_index;
    int     has_disp;
    int     base_reg;
    int     index_reg;
    int     scale;
    int32_t disp;
} MemOperand;

// register names indexed 0-7 (x86 encoding order)
static const char *reg_names[] = {
    "EAX", "ECX", "EDX", "EBX",
    "ESP", "EBP", "ESI", "EDI"
};

OpcodeInfo opcode_table[256] = {

    // no operands
    [0x90] = {"NOP",   0, 0},
    [0xC3] = {"RET",   0, 0},
    [0xF4] = {"HLT",   0, 0},
    [0xCC] = {"INT3",  0, 0},

    // PUSH/POP reg (register encoded in low 3 bits of opcode)
    [0x50] = {"PUSH",  0, 0},
    [0x51] = {"PUSH",  0, 0},
    [0x52] = {"PUSH",  0, 0},
    [0x53] = {"PUSH",  0, 0},
    [0x54] = {"PUSH",  0, 0},
    [0x55] = {"PUSH",  0, 0},
    [0x56] = {"PUSH",  0, 0},
    [0x57] = {"PUSH",  0, 0},

    [0x58] = {"POP",   0, 0},
    [0x59] = {"POP",   0, 0},
    [0x5A] = {"POP",   0, 0},
    [0x5B] = {"POP",   0, 0},
    [0x5C] = {"POP",   0, 0},
    [0x5D] = {"POP",   0, 0},
    [0x5E] = {"POP",   0, 0},
    [0x5F] = {"POP",   0, 0},

    // MOV reg, imm32 (register encoded in low 3 bits of opcode)
    [0xB8] = {"MOV",   0, 4},
    [0xB9] = {"MOV",   0, 4},
    [0xBA] = {"MOV",   0, 4},
    [0xBB] = {"MOV",   0, 4},
    [0xBC] = {"MOV",   0, 4},
    [0xBD] = {"MOV",   0, 4},
    [0xBE] = {"MOV",   0, 4},
    [0xBF] = {"MOV",   0, 4},

    // jumps
    [0xEB] = {"JMP",   0, 1},  // rel8
    [0xE9] = {"JMP",   0, 4},  // rel32
    [0xE8] = {"CALL",  0, 4},  // rel32

    // short conditional jumps rel8
    [0x70] = {"JO",    0, 1},
    [0x71] = {"JNO",   0, 1},
    [0x72] = {"JB",    0, 1},
    [0x73] = {"JNB",   0, 1},
    [0x74] = {"JE",    0, 1},
    [0x75] = {"JNE",   0, 1},
    [0x76] = {"JBE",   0, 1},
    [0x77] = {"JA",    0, 1},
    [0x78] = {"JS",    0, 1},
    [0x79] = {"JNS",   0, 1},
    [0x7A] = {"JP",    0, 1},
    [0x7B] = {"JNP",   0, 1},
    [0x7C] = {"JL",    0, 1},
    [0x7D] = {"JGE",   0, 1},
    [0x7E] = {"JLE",   0, 1},
    [0x7F] = {"JG",    0, 1},

    [0xCD] = {"INT",   0, 1},  // INT imm8

    // misc
    [0x9C] = {"PUSHF", 0, 0},
    [0x9D] = {"POPF",  0, 0},
    [0xCB] = {"RETF",  0, 0},
    [0xC2] = {"RET",   0, 2},  // RET imm16
    [0xC9] = {"LEAVE", 0, 0},

    // instructions that use ModR/M
    [0x89] = {"MOV",   1, 0},  // MOV r/m32, r32
    [0x8B] = {"MOV",   1, 0},  // MOV r32, r/m32
    [0x01] = {"ADD",   1, 0},  // ADD r/m32, r32
    [0x29] = {"SUB",   1, 0},  // SUB r/m32, r32
    [0x39] = {"CMP",   1, 0},  // CMP r/m32, r32
};
