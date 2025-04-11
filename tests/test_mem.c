#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t a = 13;
uint64_t b = 8;
uint64_t c = 6;
uint64_t result1;
uint64_t result2;
uint64_t result3;
uint64_t result4;

int main() {

    /*uint64_t values[4];  // 0: a, 1: b, 2: c

    values[0] = 13;  // a
    values[1] = 8;   // b
    values[2] = 6;
    values[3];*/

    //printf("a: %p, b: %p, c: %p, result: %p\n", &values[0], &values[1], &values[2], &values[3]);
    
    /*asm volatile (
        "addmod %0, %1, %2, %3\n\t"
        : "=r"(result)                // Output operand (we'll ignore it if IMC writes to memory directly)
        : "r"(&a), "r"(&b), "r"(&c)   // Input operands: addresses of a, b, c
        :                             // No clobbers
    );*/

    asm volatile (
        "addmod %0, %1, %2, %3\n\t"
        :                 // Output operand (we'll ignore it if IMC writes to memory directly)
        : "r"(&a), "r"(&b), "r"(&c), "r"(&result1)   // Input operands: addresses of a, b, c, result
        :                             // No clobbers
    );

    printf("Result: %d\n", result1);

    asm volatile (
        "mulmod %0, %1, %2, %3\n\t"
        :                 // Output operand (we'll ignore it if IMC writes to memory directly)
        : "r"(&a), "r"(&b), "r"(&c), "r"(&result2)   // Input operands: addresses of a, b, c, result
        :                             // No clobbers
    );

    printf("Result: %d\n", result2);

    asm volatile (
        "mulimc %0, %1, %2\n\t"
        :                 // Output operand (we'll ignore it if IMC writes to memory directly)
        : "r"(&a), "r"(&b), "r"(&result3)   // Input operands: addresses of a, b, c, result
        :                             // No clobbers
    );

    printf("Result: %d\n", result3);

    asm volatile (
        "invmod %0, %1, %2\n\t"
        :                 // Output operand (we'll ignore it if IMC writes to memory directly)
        : "r"(&a), "r"(&b), "r"(&result4)   // Input operands: addresses of a, b, c, result
        :                             // No clobbers
    );

    /*asm volatile (
        "addmod2 %0\n\t"
        :                // Output operand (we'll ignore it if IMC writes to memory directly)
        : "r"(&values[0])   // Input operands: addresses of a, b, c
        :                             // No clobbers
    );*/

    //asm volatile ("addmod %0\n" :: "r"(&values[0]) :);

    printf("Result: %d\n", result4);
    return 0;
};