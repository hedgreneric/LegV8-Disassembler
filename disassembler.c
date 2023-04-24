// Eric Hedgren - ehedgren
// Dominik Chally - drchally TODO check that I put the right netid

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
    void* function;
    int opcode;
} instruction_t;

void decode(int32_t, int); // binary, line
void decode_R_type (instruction_t, int32_t, int); // instruction, binary of insturction, line number
void decode_I_type (instruction_t, int32_t, int);
void decode_D_type (instruction_t, int32_t, int);
void decode_B_type (instruction_t, int32_t, int);
void decode_CB_type (instruction_t, int32_t, int);

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
        { "SUBS",    decode_R_type,   0b11101011000 }
};


int main(int argc, char *argv[]){
    bool binary;
    int fd;
    int32_t* program;
    struct stat buf;
    int* bprogram;
    int i;

    // TODO: get value of binary by finding out if file is a binary file
    binary = true; // we may not have to check if it is a binary file
    if (binary){ // checks if the file is a binary file
        fd = open(argv[1], O_RDONLY); // opens the specified file given from the cmd
        fstat(fd, &buf); // gets the size of the file fills struct stat with the pointer buf
        program = mmap(NULL, buf.st_size, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE, fd, 0); // maps the file into memory
        bprogram = calloc(buf.st_size / 4, sizeof (*bprogram)); //allocates space with the number values and size of int
        for (i = 0; i < (buf.st_size / 4); i++){ // iterates through each value
            program[i] = be32toh(program[i]); // reads 32-bit value and converts it from big-endian to byte order
            //decode(program[i], bprogram + i); // we have to do this method, but it decodes it
            printf("%x\n", program[i]);
        }

        //emulate(bprogram, buf->st_size / 4, &m); // I guess this runs the code that we decode
    }
    return 0;
}

// TODO: ask alex anpit param instruc. is it a new binary value each time?
void decode (int32_t instruc, int line) {
    int i;
    unsigned int opcode11bit = (instruc & 0xFFE00000) >> 21; // first 11 bits
    unsigned int opcode10bit = (instruc & 0xFFC00000) >> 20; // first 10 bits
    unsigned int opcode8bit = (instruc & 0xFF000000) >> 24; // first 8 bits
    unsigned int opcode6bit = (instruc & 0xFc000000) >> 26; // first 6 bits

    for (i = 0; i < sizeof(instructions); i++) {
        if (opcode11bit == instructions[i].opcode) {
            instructions[i].function(instructions[i], instruc, line);
        }
        else if (opcode10bit == instructions[i].opcode) {
            instructions[i].function(instructions[i], instruc, line);
        }
        else if (opcode8bit == instructions[i].opcode) {
            instructions[i].function(instructions[i], instruc, line);
        }
        else if (opcode6bit == instructions[i].opcode) {
            instructions[i].function(instructions[i], instruc, line);
        }
    }
}

void decode_R_type (instruction_t instruction, int32_t opcode, int line){

}

void decode_I_type (instruction_t instruction, int32_t opcode, int line){

}

void decode_D_type (instruction_t instruction, int32_t opcode, int line){

}

void decode_B_type (instruction_t instruction, int32_t opcode, int line){

}

void decode_CB_type (instruction_t instruction, int32_t opcode, int line){

}