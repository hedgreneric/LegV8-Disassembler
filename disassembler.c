// Eric Hedgren - ehedgren
// Dominik Chally - drchally TODO check that I put the right netid

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
#include <endian.h>
/*
 * The red underlined headers are not on Windows by default.
 * To work around this you can run this in the terminal in pyrite (a Linux OS)
 */

typedef struct instruction {
    const char* instr;
    void (*function)();
    int opcode;
} instruction_t;

void decode(int32_t, int*); // binary, line
void decode_R_type (instruction_t, int32_t, int*, int); // instruction, binary of insturction, line number
void decode_I_type (instruction_t, int32_t, int*, int);
void decode_D_type (instruction_t, int32_t, int*, int);
void decode_B_type (instruction_t, int32_t, int*, int);
void decode_CB_type (instruction_t, int32_t, int*, int);
char* get_condition (instruction_t, int32_t);

instruction_t instructions[] = {
        { "ADD",     decode_R_type,    0b10001011000 },
        { "ADDI",    decode_I_type,   0b1001000100  },
        { "AND",     decode_R_type,    0b10001010000 },
        { "ANDI",    decode_I_type,   0b1001001000  },
        { "B",       decode_B_type,      0b000101      },
        { "BL",      decode_B_type,     0b100101      },
        { "BR",      decode_R_type,     0b11010110000 },
        { "CBNZ",    decode_CB_type,   0b10110101    },
        { "CBZ",     decode_CB_type,    0b10110100    },
        { "DUMP",    decode_R_type,   0b11111111110 },
        { "EOR",     decode_R_type,    0b11001010000 },
        { "EORI",    decode_I_type,   0b1101001000  },
        { "HALT",    decode_R_type,   0b11111111111 },
        { "LDUR",    decode_D_type,   0b11111000010 },
        { "LSL",     decode_R_type,    0b11010011011 },
        { "LSR",     decode_R_type,    0b11010011010 },
        { "ORR",     decode_R_type,    0b10101010000 },
        { "ORRI",    decode_I_type,   0b1011001000  },
        { "PRNL",    decode_R_type,   0b11111111100 },
        { "PRNT",    decode_R_type,   0b11111111101 },
        { "STUR",    decode_D_type,   0b11111000000 },
        { "SUB",     decode_R_type,    0b11001011000 },
        { "SUBI",    decode_I_type,   0b1101000100  },
        { "SUBIS",   decode_I_type,  0b1111000100  },
        { "SUBS",    decode_R_type,   0b11101011000 },
        { "B.",      decode_CB_type, 0b01010100    }
};

int main(int argc, char *argv[]){
    int fd;
    int32_t* program;
    struct stat buf;
    int* bprogram;
    int i;

    fd = open(argv[1], O_RDONLY); // opens the specified file given from the cmd
    fstat(fd, &buf); // gets the size of the file fills struct stat with the pointer buf
    program = mmap(NULL, buf.st_size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE, fd, 0); // maps the file into memory
    bprogram = calloc(buf.st_size / 4, sizeof (*bprogram)); //allocates space with the number values and size of int
    for (i = 0; i < (buf.st_size / 4); i++){ // iterates through each value
        program[i] = be32toh(program[i]); // reads 32-bit value and converts it from big-endian to byte order
        decode(program[i], bprogram + i); // TODO: this line is creating a Segementaion fault (core dump) no matter what
    }

    //emulate(bprogram, buf->st_size / 4, &m); // I guess this runs the code that we decode
    return 0;
}

void decode (int32_t binary, int* line) {
    int opcode;

    int32_t opcode11bit = (binary & 0xFFE00000) >> 21; // first 11 bits
    int32_t opcode10bit = (binary & 0xFFC00000) >> 22; // first 10 bits
    int32_t opcode8bit = (binary & 0xFF000000) >> 24; // first 8 bits
    int32_t opcode6bit = (binary & 0xFc000000) >> 26; // first 6 bits

    for (int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); i++) {
        opcode = instructions[i].opcode;
        if (opcode == opcode11bit ||
            opcode == opcode10bit ||
            opcode == opcode8bit ||
            opcode == opcode6bit) {
            instructions[i].function(instructions[i], binary, line, i);
        }
    }
}

// opcode 11 bits, Rm 5 bits, shamt 6 bits, Rn 5 bits, Rd 5 bits
void decode_R_type (instruction_t instruction, int32_t binary, int* line, int instr_idx){
    int32_t rm = (binary & 0x001F0000) >> 16;
    int32_t shamt = (binary & 0x0000FC00) >> 10;
    int32_t rn = (binary & 0x000003E0) >> 5;
    int32_t rd = (binary & 0x0000001F);

    // instruction is DUMP, HALT, PRNL
    if (instr_idx == 9 || instr_idx == 12 || instr_idx == 18) {
        printf("%s\n", instruction.instr);
    }
    // instruction is PRNT
    else if (instr_idx == 19) {
        printf("%s X%d\n", instruction.instr, rd);
    }
    // instruction is BR
    else if (instr_idx == 6) {
        printf("%s X%d\n", instruction.instr, rn);
    }
    // instruction is LSL LSR
    else if (instr_idx == 14 || instr_idx == 15) {
        printf("%s X%d, X%d, #%d\n", instruction.instr, rd, rn, shamt);
    }
    else {
        printf("%s X%d, X%d, X%d\n", instruction.instr, rd, rn, rm);
    }
}

// opcode 10 bits, ALU 12 bits, Rn 5 bits, Rd 5 bits
void decode_I_type (instruction_t instruction, int32_t binary, int* line, int instr_idx){
    int32_t alu = (binary & 0x003FFC00) >> 10;
    int32_t aluNeg = (binary & 0x00200000) >> 21;
    int32_t rn = (binary & 0x000003E0) >> 5;
    int32_t rd = (binary & 0x0000001F);

    if (aluNeg == 1){
        alu = ~alu;
    }
    printf("%s X%d X%d #%d\n", instruction.instr, rd, rn, alu);

}

// opcode 11 bits, DT address 9 bits, op 2 bits, Rn 5 bits, Rt 5 bits
void decode_D_type (instruction_t instruction, int32_t binary, int* line, int instr_idx){
    int32_t dt_addr = (binary & 0x001FF000) >> 12;
    int32_t rn = (binary & 0x000003E0) >> 5;
    int32_t rt = (binary & 0x0000001F);

    printf("%s X%d, [X%d, #%d]\n", instruction.instr, rt, rn, dt_addr);
}

void decode_B_type (instruction_t instruction, int32_t binary, int* line, int instr_idx){
    int32_t brAddr = (binary & 0x03FFFFFF);
    int32_t brAddrNeg = (binary & 0x02000000) >> 25;

    if (brAddrNeg == 1){
        brAddr = ~brAddr;
    }
    printf("%s %d\n", instruction.instr, brAddr);
}

void decode_CB_type (instruction_t instruction, int32_t binary, int* line, int instr_idx){
    int32_t cond_br_addr = (binary & 0x00FFFFE0) >> 5;
    int32_t rt = (binary & 0x0000001F);

    if (instr_idx == 25) {
        char* cond = get_condition(instruction, rt);
        printf("%s%s\n", instruction.instr, cond);
    }
    else {
        printf("%s X%d, DO THE LABEL SHIT HERE\n", instruction.instr, rt);
    }
}

char* get_condition(instruction_t instruction, int32_t rt) {
    switch (rt) {
        case 0:
            return "EQ";
            break;
        case 1:
            return "NE";
            break;
        case 2:
            return "HS";
            break;
        case 3:
            return "LO";
            break;
        case 4:
            return "MI";
            break;
        case 5:
            return "PL";
            break;
        case 6:
            return "VS";
            break;
        case 7:
            return "VC";
            break;
        case 8:
            return "HI";
            break;
        case 9:
            return "LS";
            break;
        case 10:
            return "GE";
            break;
        case 11:
            return "LT";
            break;
        case 12:
            return "GT";
            break;
        case 13:
            return "LE";
            break;
    }
}

