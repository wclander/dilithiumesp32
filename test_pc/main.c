#include "api.h"
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <math.h>

#include "multpoly.h"

void test_poly_mult();

int main() {
    uint8_t pk[PQCLEAN_DILITHIUM5_CLEAN_CRYPTO_PUBLICKEYBYTES];
    uint8_t sk[PQCLEAN_DILITHIUM5_CLEAN_CRYPTO_SECRETKEYBYTES];

    uint8_t sig[PQCLEAN_DILITHIUM5_CLEAN_CRYPTO_BYTES];
    size_t siglen;

    const size_t mlen = 13;
    const uint8_t m[] = "TEST MESSAGE";

    PQCLEAN_DILITHIUM5_CLEAN_crypto_sign_keypair(pk, sk);

    // Sign message
    PQCLEAN_DILITHIUM5_CLEAN_crypto_sign_signature(sig, &siglen, m, mlen, sk);
    
    // Verify Signed message
    int result = PQCLEAN_DILITHIUM5_CLEAN_crypto_sign_verify(sig, siglen, m, mlen, pk);

    printf("Sig: %x\n", sig[0]);
    printf("Verification Result: %s\n", result ? "failure" : "success");

    // Test polynomial multiplication
    test_poly_mult();
}

void test_poly_mult() {
    poly a = {0};
    poly b = {0};

    poly result;
    poly result2;

    // we use one small and one large polynomial, since all of the 
    //  replaced polynomial multiplications have at least one
    //  polynomial with small coefficients
    for (int i = 0; i < N; i++) {
        a.coeffs[i] = i % 2 ? 1 : -1; // test both positive and negative
        b.coeffs[i] = 10 * i + 2000000;
    }

    // Kronecker multiplication
    poly_mult(&result, &a, &b);
    PQCLEAN_DILITHIUM5_CLEAN_poly_reduce(&result);

    // Original NTT multiplication
    PQCLEAN_DILITHIUM5_CLEAN_poly_ntt(&a);
    PQCLEAN_DILITHIUM5_CLEAN_poly_ntt(&b);
    PQCLEAN_DILITHIUM5_CLEAN_poly_pointwise_montgomery(&result2, &a, &b);
    PQCLEAN_DILITHIUM5_CLEAN_poly_invntt_tomont(&result2);
    PQCLEAN_DILITHIUM5_CLEAN_poly_reduce(&result2);
    for (int i = 0; i < N; i++) {
        if (result.coeffs[i] != result2.coeffs[i]) {
            printf("Coefficients do not match: %d,%d\n", result.coeffs[i], result2.coeffs[i]);
        }
    }
}