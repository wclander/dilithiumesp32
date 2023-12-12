#include "api.h"
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

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

    printf("Sig: %x", sig[0]);
    printf("Verification Result: %d", result);
}