#pragma once

#include <stdint.h>

/*
    Performs first-time setup for RSA co-processor
*/
void setup_hwmult();

/*
    Calculates a * b mod m using the RSA accelerator

    result is a 4096-bit number (512 element byte array)
    a,b,m,r are each 4096-bit numbers (512 element byte array)

    r should be 2^8192 mod m
    m_prime should be an integer such that:
        m''\cross m + 1 = R \cross R^{-1} mod m
        m_prime = m'' mod 2^{32}
*/
void mod_mult_4096(uint8_t *result, const uint8_t *a, const uint8_t *b, const uint8_t *m, const uint8_t *r, uint32_t m_prime);

/*
    Calculates a * b using the RSA accelerator

    result is a 4096-bit number (512 element byte array)
    a,b are each 2048-bit numbers (256 element byte array)

    All numbers are little endian
*/
void mult_2048(uint8_t *result, const uint8_t *a, const uint8_t *b);