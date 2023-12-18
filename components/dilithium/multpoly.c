#include "multpoly.h"
#include "hwmult.h"
#include "reduce.h"
#include <stdlib.h>
#include <string.h>

#if defined(ESP_PLATFORM)
    #include "esp_task_wdt.h"
#else
    #include <gmp.h>
    // Since we won't have this function when testing on PC
    void mult_2048(uint8_t *result, const uint8_t *a, const uint8_t *b) {}
#endif

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
    Add two integers, returns carry

    result, x are caller allocated x-size uint32_t arrays
    y is a caller allocated y-size uint32_t array
*/
uint32_t add_n(uint32_t *result, const uint32_t *x, const uint32_t *y, const size_t len_x, const size_t len_y) {
    uint32_t carry = 0;
    for (int i = 0; i < len_y; i++) {
        uint64_t sum = (uint64_t) x[i] + (uint64_t) y[i] + (uint64_t) carry;
        carry = sum >> 32;
        result[i] = sum & 0xFFFFFFFF;
    }
    for (int i = len_y; i < len_x; i++) {
        uint64_t sum = (uint64_t) x[i] + (uint64_t) carry;
        carry = sum >> 32;
        result[i] = sum & 0xFFFFFFFF;
    }
    return carry;
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
    Add two 16384-bit integers, returns carry

    result, x, y are caller allocated 2048-element byte arrays
*/
uint32_t add_16384(uint32_t *result, const uint32_t *x, const uint32_t *y) {
    uint32_t carry = 0;
    for (int i = 0; i < 16384 / 32; i++) {
        uint64_t sum = (uint64_t) x[i] + (uint64_t) y[i] + (uint64_t) carry;
        carry = sum >> 32;
        result[i] = sum & 0xFFFFFFFF;
    }
    return carry;
}

/*
    Subtracts two 2048-bit integers, returns carry

    result, x, y are caller allocated 512-element byte arrays
*/
uint32_t sub_2048(uint32_t *result, const uint32_t *x, const uint32_t *y) {
    uint32_t carry = 0;
    for (int i = 0; i < 2048 / 32; i++) {
        uint64_t diff = (uint64_t) x[i] - (uint64_t) y[i] - (uint64_t) carry;
        carry = (diff >> 32) & 1;
        result[i] = diff & 0xFFFFFFFF;
    }
    return carry;
}

/*
    Subtracts two 4096-bit integers, returns carry

    result, x, y are caller allocated 1024-element byte arrays
*/
uint32_t sub_4096(uint32_t *result, const uint32_t *x, const uint32_t *y) {
    uint32_t carry = 0;
    for (int i = 0; i < 4096 / 32; i++) {
        uint64_t diff = (uint64_t) x[i] - (uint64_t) y[i] - (uint64_t) carry;
        carry = (diff >> 32) & 1;
        result[i] = diff & 0xFFFFFFFF;
    }
    return carry;
}

/*
    Subtracts two 8192-bit integers, returns carry

    result, x, y are caller allocated 1024-element byte arrays
*/
uint32_t sub_8192(uint32_t *result, const uint32_t *x, const uint32_t *y) {
    uint32_t carry = 0;
    for (int i = 0; i < 8192 / 32; i++) {
        uint64_t diff = (uint64_t) x[i] - (uint64_t) y[i] - (uint64_t) carry;
        carry = (diff >> 32) & 1;
        result[i] = diff & 0xFFFFFFFF;
    }
    return carry;
}

/*
    Subtracts two 16384-bit integers, returns carry

    result, x, y are caller allocated 2048-element byte arrays
*/
uint32_t sub_16384(uint32_t *result, const uint32_t *x, const uint32_t *y) {
    uint32_t carry = 0;
    for (int i = 0; i < 16384 / 32; i++) {
        uint64_t diff = (uint64_t) x[i] - (uint64_t) y[i] - (uint64_t) carry;
        carry = (diff >> 32) & 1;
        result[i] = diff & 0xFFFFFFFF;
    }
    return carry;
}

/*
    Multiplies together two 4096-bit numbers to get one 8192-bit number
*/
void karatsuba_4096(uint32_t *result, uint32_t *a, uint32_t *b) {
    // feed the watchdog so we don't timeout
    //esp_task_wdt_reset();

    uint32_t ha[64], hb[64], la[64], lb[64];

    memcpy(ha, a + 64, 256);
    memcpy(hb, b + 64, 256);

    memcpy(la, a + 0, 256);
    memcpy(lb, b + 0, 256);
    
    uint32_t z_0[128], z_1[128], z_2[128];

    mult_2048(z_0, la, lb);
    mult_2048(z_2, ha, hb);

    // since we don't need la,lb anymore, save memory 
    //  by reusing them for la + ha and lb + hb
    add_2048(la, la, ha);
    add_2048(lb, lb, hb);

    // z_1 = (la + ha)(lb + hb) - z_0 - z_2
    mult_2048(z_1, la, lb);
    sub_4096(z_1, z_1, z_0);
    sub_4096(z_1, z_1, z_2);

    // low bits
    memcpy(result, z_0, 512);
    // high bits
    memcpy(result + 128, z_2, 512);
    // add mid bits
    add_n(result + 64, result + 64, z_1, 192, 128);
}

/*
    Multiplies together two 8192-bit numbers to get one 16384-bit number
*/
void karatsuba_8192(uint32_t *result, uint32_t *a, uint32_t *b) {
    uint32_t ha[128], hb[128], la[128], lb[128];

    memcpy(ha, a + 128, 512);
    memcpy(hb, b + 128, 512);

    memcpy(la, a + 0, 512);
    memcpy(lb, b + 0, 512);
    
    uint32_t z_0[256], z_1[256], z_2[256];

    karatsuba_4096(z_0, la, lb);
    karatsuba_4096(z_2, ha, hb);

    // since we don't need la,lb anymore, save memory 
    //  by reusing them for la + ha and lb + hb
    add_4096(la, la, ha);
    add_4096(lb, lb, hb);

    // z_1 = (la + ha)(lb + hb) - z_0 - z_2
    karatsuba_4096(z_1, la, lb);
    sub_8192(z_1, z_1, z_0);
    sub_8192(z_1, z_1, z_2);

    // low bits
    memcpy(result, z_0, 1024);
    // high bits
    memcpy(result + 256, z_2, 1024);
    // add mid bits
    add_n(result + 128, result + 128, z_1, 384, 256);
}

/*
    Multiplies two 64-bit polynomials of degree <=256 and returns one
        polynomial of degree <=256 reducing modulo x^256 + 1 where applicable
*/
void mult_reduce_poly64(poly64 *result, poly64 *a, poly64 *b) {
#if defined(ESP_PLATFORM)
    // For ESP32, we use the hardware multiplier and Karatsuba multiplication
    uint32_t result_full[1024];
    uint32_t ha[256], hb[256], la[256], lb[256];

    memcpy(ha, a->coeffs + 128, 1024);
    memcpy(hb, b->coeffs + 128, 1024);

    memcpy(la, a->coeffs + 0, 1024);
    memcpy(lb, b->coeffs + 0, 1024);
    
    uint32_t z_0[512], z_1[512], z_2[512];

    karatsuba_8192(z_0, la, lb);
    karatsuba_8192(z_2, ha, hb);

    // since we don't need la,lb anymore, save memory 
    //  by reusing them for la + ha and lb + hb
    add_8192(la, la, ha);
    add_8192(lb, lb, hb);

    // z_1 = (la + ha)(lb + hb) - z_0 - z_2
    karatsuba_8192(z_1, la, lb);
    sub_16384(z_1, z_1, z_0);
    sub_16384(z_1, z_1, z_2);

    // low bits
    memcpy(result_full, z_0, 2048);
    // high bits
    memcpy(result_full + 512, z_2, 2048);
    // add mid bits
    add_n(result_full + 256, result_full + 256, z_1, 768, 512);

    // Subtract the high coefficients from the low coefficients to get a * b mod x^256
    for (int i = 0; i < N; i++) {
        result->coeffs[i] = ((uint64_t *) result_full)[i] - ((uint64_t *) result_full)[256 + i];
    }
    // Reduce the resulting polynomial mod Q 
    // Note: using the % operator is slower and may not be constant time on systems without hardware division
    for (int i = 0; i < N; i++) {
        result->coeffs[i] %= Q;
    }
#else
    // Otherwise we use libgmp
    mp_limb_t result_full[512 * sizeof(uint64_t) / sizeof(mp_limb_t)];
    // Calculate a * b
    mpn_mul_n(result_full, (mp_limb_t *)a->coeffs, (mp_limb_t *)b->coeffs, 256 * sizeof(uint64_t) / sizeof(mp_limb_t));
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