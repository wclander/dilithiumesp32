#include <soc/hwcrypto_periph.h>

void mod_mult(uint8_t *a, uint8_t *b, uint8_t *m) {
    DPORT_REG_WRITE(RSA_MEM_X_BLOCK_BASE, 100);
    DPORT_REG_WRITE(RSA_MULT_START_REG, 1);
}

void app_main(void)
{
    
}