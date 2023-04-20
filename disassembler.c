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

#include <inttypes.h> // used to print uint32_t value

int main(int argc, char *argv[]){
    bool binary; // false
    int fd;
    uint32_t* program;
    struct stat buf;
    int* bprogram;
    int i;

    // TODO: get value of binary by finding out if file is a binary file. Probaly will have to clear out the hex values
    binary = true;
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