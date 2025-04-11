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

    /*asm volatile (
        "addmod %0, %1, %2, %3\n\t"
        : "=r"(result)                // Output operand (we'll ignore it if IMC writes to memory directly)
        : "r"(a), "r"(b), "r"(c)   // Input operands: addresses of a, b, c
        :                             // No clobbers
    );*/
    
    result1 = (a + b) % c;
    printf("Result: %d\n", result1);
    result2 = (a * b) % c;
    printf("Result: %d\n", result2);
    result3 = a * b;
    printf("Result: %d\n", result3);

    int m0 = b, t, q;
    int x0 = 0, x1 = 1;
    if (b == 1) {
        result4 = 0;
    }
    else {
        while (a > 1) {
            q = a / b;
            t = b;
            b = a % b, a = t;
            t = x0;
            x0 = x1 - q * x0;
            x1 = t;
        }
        if (x1 < 0) {
            x1 += m0;
        }
        result4 = x1;
    }


    //result = (a + b) % result;

    printf("Result: %d\n", result4);
    return 0;
};