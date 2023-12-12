#include "api.h"
#include <stdint.h>
#include <stddef.h>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

const char*TAG = "dilithium_tests";

#include "esp_sleep.h"
#include "ccomp_timer.h"

void app_main(void)
{
    uint8_t pk[PQCLEAN_DILITHIUM5_CLEAN_CRYPTO_PUBLICKEYBYTES];
    uint8_t sk[PQCLEAN_DILITHIUM5_CLEAN_CRYPTO_SECRETKEYBYTES];

    uint8_t sig[PQCLEAN_DILITHIUM5_CLEAN_CRYPTO_BYTES];
    size_t siglen;

    const size_t mlen = 13;
    const uint8_t m[] = "TEST MESSAGE";

    ccomp_timer_start();

    // Generate keypair
    PQCLEAN_DILITHIUM5_CLEAN_crypto_sign_keypair(pk, sk);

    int64_t keygen_time = ccomp_timer_get_time();

    // Sign message
    PQCLEAN_DILITHIUM5_CLEAN_crypto_sign_signature(sig, &siglen, m, mlen, sk);

    int64_t sign_time = ccomp_timer_get_time();
    
    // Verify Signed message
    int result = PQCLEAN_DILITHIUM5_CLEAN_crypto_sign_verify(sig, siglen, m, mlen, pk);

    int64_t verify_time = ccomp_timer_stop();

    ESP_LOG_BUFFER_HEX("Signature: ", sig, siglen);

    ESP_LOGI(TAG, "Verification Result: %d", result);

    ESP_LOGI(TAG, "Keygen: %qdus, Sign: %qdus, Verify: %qdus", keygen_time, sign_time, verify_time);

    esp_deep_sleep_start();
    while(1){};
}