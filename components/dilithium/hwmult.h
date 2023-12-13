#pragma once

#include <stdint.h>

/*
    Performs first-time setup for RSA co-processor
*/
void setup_hwmult();

/*
    Calculates a * b mod m using the RSA accelerator

    result is a 4096-bit number (512 element byte array)
    a,b,m are each 2048-bit numbers (256 element byte array)
*/
void mod_mult_2048(uint8_t *result, const uint8_t *a, const uint8_t *b, const uint8_t *m);
//void mod_mult_4096_async();

/*
    Calculates a * b using the RSA accelerator

    result is a 4096-bit number (512 element byte array)
    a,b,m are each 2048-bit numbers (256 element byte array)
*/
void mult_2048(uint8_t *result, const uint8_t *a, const uint8_t *b);
//void mult_4096_async();