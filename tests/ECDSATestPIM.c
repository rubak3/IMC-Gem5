#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int64_t s = 3;
int64_t n = 19; 
int64_t Px =5;
int64_t Py = 3; 
int64_t Qx =6;
int64_t Qy = 2;
int64_t lambda;
int64_t numerator;
int64_t denominator;
int64_t Rx;
int64_t Ry;
int64_t u1Px = 10;
int64_t u1Py=11;
int64_t u2Qx =13;
int64_t u2Qy=15;
int64_t Verification_Result;
int64_t u1;
int64_t u2;
int64_t w;
int64_t v;
int64_t z = 9;
int64_t r = 7;
int64_t num =0;
uint64_t temp;

int main() {

    // w = (s^−1)modn
    asm volatile (
        "invmod %0, %1, %2\n\t"
        :
        : "r"(&s), "r"(&n), "r"(&w)
        :
    );

    // u1 = (z*w)modn
    asm volatile (
        "mulmod %0, %1, %2, %3\n\t"
        :
        : "r"(&z), "r"(&w), "r"(&n), "r"(&u1)   
        :
    );

    // u2 = (r*w)modn
    asm volatile (
        "mulmod %0, %1, %2, %3\n\t"
        :
        : "r"(&r), "r"(&w), "r"(&n), "r"(&u2)   
        :
    );



    //////////////////// Finding u1P ////////////////////

    // numerator = (u1Py-Py)modn
    asm volatile (
        "submod %0, %1, %2, %3\n\t"
        :
        : "r"(&u1Py), "r"(&Py), "r"(&n), "r"(&numerator)   
        :
    );

    // denominator = (uiPx-Px)modn
    asm volatile (
        "submod %0, %1, %2, %3\n\t"
        :
        : "r"(&u1Px), "r"(&Px), "r"(&n), "r"(&denominator)   
        :
    );

    // denominator = (denominator^-1)modn
    asm volatile (
        "invmod %0, %1, %2\n\t"
        :
        : "r"(&denominator), "r"(&n), "r"(&denominator)   
        :
    );

    // lambda = (numerator*denominator)modn 
    asm volatile (
        "mulmod %0, %1, %2, %3\n\t"
        :
        : "r"(&numerator), "r"(&denominator), "r"(&n), "r"(&lambda)  
        :
    );
            
    // lambda = (lambda*lambda)modn
    asm volatile (
        "mulmod %0, %1, %2, %3\n\t"
        :
        : "r"(&lambda), "r"(&lambda), "r"(&n), "r"(&temp)   
        :  
    );

    // u1Px = (Px+u1Px)modn
    asm volatile (
        "addmod %0, %1, %2, %3\n\t"
        :
        : "r"(&Px), "r"(&u1Px), "r"(&n), "r"(&u1Px)   
        :
    );

    // u1Px = (temp-u1Px)modn
    asm volatile (
        "submod %0, %1, %2, %3\n\t"
        :
        : "r"(&temp), "r"(&u1Px), "r"(&n), "r"(&u1Px)    
        :
    );

     // temp = (Px-uiPx)modn
    asm volatile (//future submod
        "submod %0, %1, %2, %3\n\t"
        :
        : "r"(&Px), "r"(&u1Px), "r"(&n), "r"(&temp)  
        :
    );

    // temp = (lambda*temp)modn
    asm volatile (
        "mulmod %0, %1, %2, %3\n\t"
        :
        : "r"(&lambda), "r"(&temp), "r"(&n), "r"(&temp)   
        :
    );
          
    // Py = (Py)modn
    asm volatile (
        "addmod %0, %1, %2, %3\n\t"
        :
        : "r"(&Py), "r"(&num), "r"(&n), "r"(&Py)  
        :
    );

    // uiPy = (temp-Py)modn
    asm volatile (
        "submod %0, %1, %2, %3\n\t"
        :
        : "r"(&temp), "r"(&Py), "r"(&n), "r"(&u1Py)   
        :
    );

    

    //////////////////// Finding u2Q ////////////////////

    // numerator = (u2Qy-Qy)modn
    asm volatile (
    "submod %0, %1, %2, %3\n\t"
        :                 // 
        : "r"(&u2Qy), "r"(&Qy), "r"(&n), "r"(&numerator)   
        :
    );

    // denominator = (u2Qx-Qx)modn
    asm volatile (
        "submod %0, %1, %2, %3\n\t"
        :
        : "r"(&u2Qx), "r"(&Qx), "r"(&n),  "r"(&denominator)   
        :
    );

    // denominator = (denominator^-1)modn
    asm volatile (
        "invmod %0, %1, %2\n\t"
        :
        : "r"(&denominator), "r"(&n), "r"(&denominator)   
        :
    );

    // lambda = (numerator*denominator)modn
    asm volatile (
        "mulmod %0, %1, %2, %3\n\t"
        :
        : "r"(&numerator), "r"(&denominator), "r"(&n), "r"(&lambda)   
        :
    );
          
    // temp = (lambda*lambda)modn
    asm volatile (
        "mulmod %0, %1, %2, %3\n\t"
        :
        : "r"(&lambda), "r"(&lambda), "r"(&n), "r"(&temp)   
        :
    );

    // u2Qx = (Qx+u2Qx)modn
    asm volatile (
        "addmod %0, %1, %2, %3\n\t"
        :
        : "r"(&Qx), "r"(&u2Qx), "r"(&n), "r"(&u2Qx)   
        :
    );

    // u2Qx = (temp-u2Qx)modn
    asm volatile (
        "submod %0, %1, %2, %3\n\t"
        :
        : "r"(&temp), "r"(&u2Qx), "r"(&n), "r"(&u2Qx)   
        :
    );

    // temp = (Qx-u2Qx)modn
    asm volatile (
        "submod %0, %1, %2, %3\n\t"
        :
        : "r"(&Qx), "r"(&u2Qx), "r"(&n), "r"(&temp)   
        :
    );

    // temp = (lambda-temp)modn
    asm volatile (
        "mulmod %0, %1, %2, %3\n\t"
        : 
        : "r"(&lambda), "r"(&temp), "r"(&n), "r"(&temp)   
        :
    );
      
    // Qy = (Qy)modn
    asm volatile (
        "addmod %0, %1, %2, %3\n\t"
        :
        : "r"(&Qy), "r"(&num), "r"(&n), "r"(&Qy)   
        :
    );

    // u2Qy = (temp-Qy)modn
    asm volatile (
        "submod %0, %1, %2, %3\n\t"
        :
        : "r"(&temp), "r"(&Qy), "r"(&n), "r"(&u2Qy)   
        :
    );



    //////////////////// Finding R //////////////////////

    // numerator = (uiPy-u2Qy)modn
    asm volatile (
        "submod %0, %1, %2, %3\n\t"
        :
        : "r"(&u1Py), "r"(&u2Qy), "r"(&n), "r"(&numerator)   
        :
    );

    // denominator = (uiPx-u2Qx)modn
    asm volatile (
        "submod %0, %1, %2, %3\n\t"
        :
        : "r"(&u1Px), "r"(&u2Qx), "r"(&n), "r"(&denominator)   
        :
    );

    // denominator = (denominator^-1)modn 
    asm volatile (
        "invmod %0, %1, %2\n\t"
        :
        : "r"(&denominator), "r"(&n), "r"(&denominator)   
        :
    );

    // lambda = (numerator*denominator)modn
    asm volatile (
        "mulmod %0, %1, %2, %3\n\t"
        :
        : "r"(&numerator), "r"(&denominator), "r"(&n), "r"(&lambda)   
        :
    );
           
    // temp = (lambda*lambda)modn
    asm volatile (
        "mulmod %0, %1, %2, %3\n\t"
        :
        : "r"(&lambda), "r"(&lambda), "r"(&n), "r"(&temp)   
        :
    );

    // Rx = (u2Qx+u1Px)modn
    asm volatile (
        "addmod %0, %1, %2, %3\n\t"
        :
        : "r"(&u2Qx), "r"(&u1Px), "r"(&n), "r"(&Rx)   
        :
    );

    // Rx = (temp-Rx)modn
    asm volatile (
        "submod %0, %1, %2, %3\n\t"
        :
        : "r"(&temp), "r"(&Rx), "r"(&n), "r"(&Rx)   
        :
    );

    // temp = (uiPx-Rx)modn
    asm volatile (
        "submod %0, %1, %2, %3\n\t"
        :
        : "r"(&u1Px), "r"(&Rx), "r"(&n), "r"(&temp)   
        :
    );

    // temp = (lambda*temp)modn
    asm volatile (
        "mulmod %0, %1, %2, %3\n\t"
        :
        : "r"(&lambda), "r"(&temp), "r"(&n), "r"(&temp)   
        :
    );
      
    // u1Py = (u1Py)modn
    asm volatile (
        "addmod %0, %1, %2, %3\n\t"
        :
        : "r"(&u1Py), "r"(&num), "r"(&n), "r"(&u1Py)   
        :
    );

    // Ry = (temp-u1Py)modn
    asm volatile (
        "submod %0, %1, %2, %3\n\t"
        :
        : "r"(&temp), "r"(&u1Py), "r"(&n), "r"(&Ry)   
        :
    );


   
    // v = (Rx)modn
    asm volatile (
        "addmod %0, %1, %2, %3\n\t"
        :
        : "r"(&Rx), "r"(&num), "r"(&n), "r"(&v)   
        :
    );

    //  if(v == r) {Verification_Result = 1}
    asm volatile (
        "cmpimc %0, %1, %2\n\t"
        :
        : "r"(&v), "r"(&r), "r"(&Verification_Result)
        :
    );

    return 0;
};
