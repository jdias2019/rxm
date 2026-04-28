#include "rxm.h"

// read signed 8-bit value and advance ip
static int8_t read_i8(uint8_t *buf, long *ip) {
    return (int8_t)buf[(*ip)++];
}

// read signed 32-bit little-endian value and advance ip
static int32_t read_i32(uint8_t *buf, long *ip) {
    int32_t val = (uint32_t)buf[*ip]
                | (uint32_t)buf[*ip + 1] << 8
                | (uint32_t)buf[*ip + 2] << 16
                | (uint32_t)buf[*ip + 3] << 24;
    *ip += 4;
    return val;
}

// build string like [EBX + ESI*4 + 0x10] from a MemOperand
static void format_mem_operand(char *out, size_t out_size, const MemOperand *mem) {
    char tmp[128];
    int  len = 0;

    len += snprintf(tmp + len, sizeof(tmp) - len, "[");

    if (mem->has_base)
        len += snprintf(tmp + len, sizeof(tmp) - len, "%s", reg_names[mem->base_reg]);

    if (mem->has_index) {
        if (mem->has_base)
            len += snprintf(tmp + len, sizeof(tmp) - len, " + ");
        len += snprintf(tmp + len, sizeof(tmp) - len, "%s", reg_names[mem->index_reg]);
        if (mem->scale != 1)
            len += snprintf(tmp + len, sizeof(tmp) - len, "*%d", mem->scale);
    }

    if (mem->has_disp) {
        if (mem->disp < 0)
            len += snprintf(tmp + len, sizeof(tmp) - len, "%s0x%X",
                (mem->has_base || mem->has_index) ? " - " : "-",
                (unsigned int)(-mem->disp));
        else if (mem->disp > 0 || (!mem->has_base && !mem->has_index))
            len += snprintf(tmp + len, sizeof(tmp) - len, "%s0x%X",
                (mem->has_base || mem->has_index) ? " + " : "",
                (unsigned int)mem->disp);
    }

    snprintf(tmp + len, sizeof(tmp) - len, "]");
    snprintf(out, out_size, "%s", tmp);
}

// decode SIB byte: fills base, index, scale in mem
// called when ModR/M r/m == 4 and mod != 3
static void decode_sib(uint8_t *buf, long *ip, uint8_t mod, MemOperand *mem) {
    uint8_t sib   = buf[(*ip)++];
    uint8_t scale = (sib >> 6) & 0x3;
    uint8_t index = (sib >> 3) & 0x7;
    uint8_t base  =  sib       & 0x7;

    mem->scale = 1 << scale;  // 0->1, 1->2, 2->4, 3->8

    if (index != 4) {         // index 4 means no index register
        mem->has_index = 1;
        mem->index_reg = index;
    }

    if (mod == 0 && base == 5) {
        // special case: no base register, just a disp32
        mem->has_disp = 1;
        mem->disp = read_i32(buf, ip);
    } else {
        mem->has_base = 1;
        mem->base_reg = base;
    }
}

// decode ModR/M byte and print operands
// mod: addressing mode  reg: register operand  r/m: register or memory operand
void decode_modrm(uint8_t *buf, long *ip, uint8_t opcode) {
    uint8_t modrm = buf[(*ip)++];
    uint8_t mod   = (modrm >> 6) & 0x3;
    uint8_t reg   = (modrm >> 3) & 0x7;
    uint8_t rm    =  modrm       & 0x7;

    char rm_str[64]  = {0};
    char reg_str[16] = {0};
    snprintf(reg_str, sizeof(reg_str), "%s", reg_names[reg]);

    if (mod == 3) {
        // both operands are registers
        snprintf(rm_str, sizeof(rm_str), "%s", reg_names[rm]);
    } else {
        MemOperand mem = {0};
        mem.scale = 1;

        if (rm == 4) {
            decode_sib(buf, ip, mod, &mem);
        } else if (mod == 0 && rm == 5) {
            // special case: [disp32] with no base
            mem.has_disp = 1;
            mem.disp = read_i32(buf, ip);
        } else {
            mem.has_base = 1;
            mem.base_reg = rm;
        }

        if (mod == 1) {
            mem.has_disp = 1;
            mem.disp = read_i8(buf, ip);
        } else if (mod == 2) {
            mem.has_disp = 1;
            mem.disp = read_i32(buf, ip);
        }

        format_mem_operand(rm_str, sizeof(rm_str), &mem);
    }

    // operand order depends on opcode direction bit
    if (opcode == 0x8B)
        printf(" %s, %s", reg_str, rm_str);  // MOV r32, r/m32
    else
        printf(" %s, %s", rm_str, reg_str);  // MOV/ADD/SUB/CMP r/m32, r32
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        printf("Could not open '%s'\n", argv[1]);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t *buffer = malloc(size);
    fread(buffer, 1, size, file);
    fclose(file);

    long i = 0;
    while (i < size) {
        uint8_t    opcode = buffer[i];
        OpcodeInfo *info  = &opcode_table[opcode];

        printf("0x%08lx   %02x   ", i, opcode);
        i++;  // consume opcode byte

        if (info->mnemonic[0] != '\0')
            printf("%s", info->mnemonic);
        else
            printf("???");

        if (info->has_modrm)
            decode_modrm(buffer, &i, opcode);

        if (info->imm_size == 4) {
            uint32_t imm = (uint32_t)read_i32(buffer, &i);
            printf(" 0x%08X", imm);
        } else if (info->imm_size == 1) {
            printf(" 0x%02X", (uint8_t)buffer[i++]);
        } else if (info->imm_size == 2) {
            uint16_t imm = (uint16_t)buffer[i] | (uint16_t)buffer[i+1] << 8;
            i += 2;
            printf(" 0x%04X", imm);
        }

        printf("\n");
    }

    free(buffer);
    return 0;
}
