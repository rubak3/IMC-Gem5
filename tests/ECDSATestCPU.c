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
    int m0 = n, t, q;
    int x0 = 0, x1 = 1;
    if (n == 1) {
        w = 0;
    }
    else {
        while (s > 1) {
            q = s / n;
            t = n;
            n = s % n, s = t;
            t = x0;
            x0 = x1 - q * x0;
            x1 = t;
        }
        if (x1 < 0) {
            x1 += m0;
        }
        w = x1;
    }

    u1 = (z * w) % n;

    u2 = (r * w) % n;


    ////////////////////u1P/////////////////////////////////
    numerator = (u1Py - Py) % n;

    //  int denominator = modinv(Q.x - P.x, P); //implemted as 2 instructions
    denominator = (u1Px - Px) % n;

    // denominator = (denominator^-1)modn
    m0 = n, t, q;
    x0 = 0, x1 = 1;
    if (n == 1) {
        denominator = 0;
    }
    else {
        while (denominator > 1) {
            q = denominator / n;
            t = n;
            n = denominator % n, denominator = t;
            t = x0;
            x0 = x1 - q * x0;
            x1 = t;
        }
        if (x1 < 0) {
            x1 += m0;
        }
        denominator = x1;
    }

    lambda = (numerator * denominator) % n; 
            
    lambda = (lambda * lambda) % n;

    u1Px = (Px + u1Px) % n;
            
    u1Px = (temp - u1Px) % n;

    temp = (Px - u1Px) % n;

    temp = (lambda * temp) % n;
            
    Py = (Py + 0) % n;

    u1Py = (temp - Py) % n;


    //////////////////u2Q/////////////////////////////////////////
    numerator = (u2Qy - Qy) % n;

    denominator = (u2Qx - Qx) % n;

    // denominator = (denominator^-1)modn
    m0 = n, t, q;
    x0 = 0, x1 = 1;
    if (n == 1) {
        denominator = 0;
    }
    else {
        while (denominator > 1) {
            q = denominator / n;
            t = n;
            n = denominator % n, denominator = t;
            t = x0;
            x0 = x1 - q * x0;
            x1 = t;
        }
        if (x1 < 0) {
            x1 += m0;
        }
        denominator = x1;
    }

    lambda = (numerator * denominator) % n;
          
    temp = (lambda * lambda) % n;

    u2Qx = (Qx + u2Qx) % n;

    u2Qx = (temp - u2Qx) % n;

    temp = (Qx - u2Qx) % n;

    temp = (lambda - temp) % n;
            
    Qy = (Qy + 0) % n;

    u2Qy = (temp - Qy) % n;

    //////////////////R///////////////////////////////////////
    numerator = (u1Py - u2Qy) % n;

    denominator = (u1Px - u2Qx) % n;

    // denominator = (denominator^-1)modn
    m0 = n, t, q;
    x0 = 0, x1 = 1;
    if (n == 1) {
        denominator = 0;
    }
    else {
        while (denominator > 1) {
            q = denominator / n;
            t = n;
            n = denominator % n, denominator = t;
            t = x0;
            x0 = x1 - q * x0;
            x1 = t;
        }
        if (x1 < 0) {
            x1 += m0;
        }
        denominator = x1;
    }

    lambda = (numerator * denominator) % n;
           
    temp = (lambda * lambda) % n;

    Rx = (u2Qx + u1Px) % n;

    Rx = (temp - Rx) % n;

    temp = (u1Px - Rx) % n;

    temp = (lambda * temp) % n;
            
    u1Py = (u1Py + 0) % n;

    Ry = (temp - u1Py) % n;

   
    v = (Rx + 0) % n;

    if (v == r) {
        Verification_Result = 1;
    }
    else {
        Verification_Result = 0;
    }

    return 0;
};
