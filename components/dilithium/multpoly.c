#include "multpoly.h"
#include "hwmult.h"
#include "reduce.h"
#include <stdlib.h>
#include <string.h>

#if !defined(ESP_PLATFORM)
    #include <gmp.h>
#endif

/*
    Implementations below use algorithms introduced in:

    https://doi.org/10.13154/tches.v2019.i1.169-208 
*/

typedef struct {
    // equivalent to a 16384-bit integer if little endian
    int64_t coeffs[N];
} poly64;

/*
    Convert a 32-bit polynomial to a 64-bit polynomial
*/
void poly_to_poly64(poly64 *result, const poly *x) {
    for (int i = 0; i < N; i++) {
        result->coeffs[i] = x->coeffs[i];
    }
}

/*
    Add two 2048-bit integers, returns carry

    result, x, y are caller allocated 512-element byte arrays
*/
uint32_t add_2048(uint32_t *result, const uint32_t *x, const uint32_t *y) {
    uint32_t carry = 0;
    for (int i = 0; i < 2048 / 32; i++) {
        uint64_t sum = (uint64_t) x[i] + (uint64_t) y[i] + (uint64_t) carry;
        carry = sum >> 32;
        result[i] = sum & 0xFFFFFFFF;
    }
    return carry;
}

/*
    Add two 4096-bit integers, returns carry

    result, x, y are caller allocated 1024-element byte arrays
*/
uint32_t add_4096(uint32_t *result, const uint32_t *x, const uint32_t *y) {
    uint32_t carry = 0;
    for (int i = 0; i < 4096 / 32; i++) {
        uint64_t sum = (uint64_t) x[i] + (uint64_t) y[i] + (uint64_t) carry;
        carry = sum >> 32;
        result[i] = sum & 0xFFFFFFFF;
    }
    return carry;
}

/*
    Add two 8192-bit integers, returns carry

    result, x, y are caller allocated 2048-element byte arrays
*/
uint32_t add_8192(uint32_t *result, const uint32_t *x, const uint32_t *y) {
    uint32_t carry = 0;
    for (int i = 0; i < 8192 / 32; i++) {
        uint64_t sum = (uint64_t) x[i] + (uint64_t) y[i] + (uint64_t) carry;
        carry = sum >> 32;
        result[i] = sum & 0xFFFFFFFF;
    }
    return carry;
}

/*
    Multiplies together two 4096-bit numbers to get one 8192 bit number
*/
void karatsuba_4096(uint8_t *result, uint8_t *x, uint8_t *y) {
    
}

/*
    Multiplies two 64-bit polynomials of degree <=256 and returns one
        polynomial of degree <=256 reducing modulo x^256 + 1 where applicable
*/
void mult_reduce_poly64(poly64 *result, poly64 *a, poly64 *b) {
#if defined(ESP_PLATFORM)
    // For ESP32, we use the hardware multiplier and Karatsuba multiplication
    uint8_t ha[1024], hb[1024], la[1024], lb[1024];

    memcpy(ha, a->coeffs + 128, 1024);
    memcpy(hb, b->coeffs + 128, 1024);

    memcpy(la, a->coeffs + 0, 1024);
    memcpy(lb, b->coeffs + 0, 1024);
    
    uint8_t z_0[1024], z_1[1024], z_2[1024];

    karatsuba_inner(z_0, la, lb);
    // since we don't need la,lb anymore, save memory 
    //  by reusing them for la + ha and lb + hb
    add_4096(la, la, ha);
    add_4096(lb, lb, hb);

    karatsuba_inner(z_1, la, lb);
    karatsuba_inner(z_2, ha, hb);

    memcpy(result->coeffs, z_0, 2048);
    add_4096(result->coeffs + 1024, result->coeffs + 1024, z_1);
    sub_4096(result->coeffs, result->coeffs, z_1 + 1024);

    // Since we are reducing mod 2^8192 + 1 we subtract z_2 from result
    //  since it is the bits higher than 2^8192 in the result
    sub_8192(result, result, z_2);
#else
    // Otherwise we use libgmp
    mp_limb_t result_full[512 * sizeof(uint64_t) / sizeof(mp_limb_t)];
    // Calculate a * b
    mpn_mul_n(result_full, (mp_limb_t *)a, (mp_limb_t *)b, 256 * sizeof(uint64_t) / sizeof(mp_limb_t));
    // Subtract the high coefficients from the low coefficients to get a * b mod x^256
    for (int i = 0; i < N; i++) {
        result->coeffs[i] = ((uint64_t *) result_full)[i] - ((uint64_t *) result_full)[256 + i];
    }
    // Reduce the resulting polynomial mod Q 
    // Note: using the % operator is slower and may not be constant time on systems without hardware division
    for (int i = 0; i < N; i++) {
        result->coeffs[i] %= Q;
    }
#endif
}

void poly_mult(poly *c, const poly *a, const poly *b) {
    // We use coefficients in [0,q] rather than [-q/2, q/2] to reduce carrying when
    //  calculating the Kronecker substitution
    poly ap;
    poly bp;
    memcpy(ap.coeffs, a->coeffs, sizeof(ap.coeffs));
    memcpy(bp.coeffs, b->coeffs, sizeof(ap.coeffs));
    PQCLEAN_DILITHIUM5_CLEAN_poly_caddq(&ap);
    PQCLEAN_DILITHIUM5_CLEAN_poly_caddq(&bp);

    // Expanding the integers to 64-bit means they will be stored in Kronecker form
    //  in memory as this is equivalent to evaluating the polynomial at 2^64
    poly64 a64, b64;
    poly_to_poly64(&a64, &ap);
    poly_to_poly64(&b64, &bp);

    poly64 result = {0};

    mult_reduce_poly64(&result, &a64, &b64);

    //Coefficient recovery
    for (int i = 0; i < N; i++) {
        c->coeffs[i] = result.coeffs[i];
    }
}

void mult_polyvecl(polyvecl *c, const poly *a, const polyvecl *b) {
    for (int i = 0; i < L; i++) {
        poly_mult(&c->vec[i], a, &b->vec[i]);
    }
}

void mult_polyveck(polyveck *c, const poly *a, const polyveck *b) {
    for (int i = 0; i < K; i++) {
        poly_mult(&c->vec[i], a, &b->vec[i]);
    }
}