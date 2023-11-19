#include "api.h"
#include <stdint.h>
#include <stddef.h>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

const char*TAG = "kyber_tests";

#include "esp_sleep.h"
#include "ccomp_timer.h"

void app_main(void)
{
    uint8_t pk[PQCLEAN_KYBER512_CLEAN_CRYPTO_PUBLICKEYBYTES];
    uint8_t sk[PQCLEAN_KYBER512_CLEAN_CRYPTO_SECRETKEYBYTES];

    uint8_t sse[PQCLEAN_KYBER512_CLEAN_CRYPTO_BYTES];
    uint8_t ct[PQCLEAN_KYBER512_CLEAN_CRYPTO_CIPHERTEXTBYTES];

    uint8_t ss[PQCLEAN_KYBER512_CLEAN_CRYPTO_BYTES];

    ccomp_timer_start();

    // Generate keypair
    PQCLEAN_KYBER512_CLEAN_crypto_kem_keypair(pk, sk);

    int64_t keygen_time = ccomp_timer_get_time();

    // Create ciphertext and shared secret
    PQCLEAN_KYBER512_CLEAN_crypto_kem_enc(ct, sse, pk);

    int64_t encap_time = ccomp_timer_get_time();
    
    // Decrypt ciphertext to recover secret
    PQCLEAN_KYBER512_CLEAN_crypto_kem_dec(ss, ct, sk);

    int64_t decap_time = ccomp_timer_stop();

    ESP_LOG_BUFFER_HEX("Encap ss: ", sse, sizeof(sse));
    ESP_LOG_BUFFER_HEX("Decap ss: ", ss, sizeof(ss));

    ESP_LOGI(TAG, "Keygen: %qdus, Encap: %qdus, Decap: %qdus", keygen_time, encap_time, decap_time);

    esp_deep_sleep_start();
    while(1){};
}