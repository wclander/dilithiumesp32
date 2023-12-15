#include "hwmult.h"
#include <string.h>
#include "soc/hwcrypto_periph.h"
#include "esp_private/periph_ctrl.h"

/*
    Relevant sources:

    Section 24 of the ESP32 Technical Reference Manual
    https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf#subsubsection.24.3.4

    For interrupts:
    https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/intr_alloc.html
*/

#define IS_RSA_ENABLED DPORT_REG_READ(RSA_CLEAN_REG)

void setup_hwmult() {
    // Enable RSA peripheral
    periph_module_enable(PERIPH_RSA_MODULE);
    DPORT_REG_CLR_BIT(DPORT_RSA_PD_CTRL_REG, DPORT_RSA_PD);

    // Wait until enabled
    while(!IS_RSA_ENABLED);
}

void mod_mult_2048(uint8_t *result, const uint8_t *a, const uint8_t *b, const uint8_t *m) {
    return;
}
//void mod_mult_4096_async();

// copy count uint32_t from src to reg
void copy_reg_uint_32t(volatile uint32_t *reg, const uint32_t *src, size_t count) {
    for (size_t i = 0; i < count; i++) {
        reg[i] = src[i];
    }
}

// copy count uint32_t from reg to src
void copy_from_reg_uint_32t(uint32_t *src, volatile uint32_t *reg, size_t count) {
    for (size_t i = 0; i < count; i++) {
        src[i] = reg[i];
    }
}

// Set first count ints in reg to val
void set_reg_uint32_t(volatile uint32_t *reg, const uint32_t val, size_t count) {
    for (size_t i = 0; i < count; i++) {
        reg[i] = val;
    }
}

void mult_2048(uint8_t *result, const uint8_t *a, const uint8_t *b) {
    assert(IS_RSA_ENABLED);

    DPORT_REG_WRITE(RSA_MULT_MODE_REG, 4096 / 512 - 1 + 8); // Set the output length parameter to 4096 bits
    copy_reg_uint_32t((volatile uint32_t *) RSA_MEM_X_BLOCK_BASE, (uint32_t *) a, 2048 / 32); // Store  0 * 2048 || a
    set_reg_uint32_t((volatile uint32_t *) (RSA_MEM_X_BLOCK_BASE + (2048 / 8)), 0, 2048 / 32);

    set_reg_uint32_t((volatile uint32_t *) RSA_MEM_Z_BLOCK_BASE, 0, 2048 / 32); // Store 0*2048 || b
    copy_reg_uint_32t((volatile uint32_t *) (RSA_MEM_Z_BLOCK_BASE + (2048 / 8)), (uint32_t *) b, 2048 / 32);

    DPORT_REG_WRITE(RSA_MULT_START_REG, 1); // Start multiplication

    // We just poll the interrupt register instead of using the interrupt
    //  Should also use a task delay or similar instead of busy waiting
    while(DPORT_REG_READ(RSA_INTERRUPT_REG) != 1);

    copy_from_reg_uint_32t((uint32_t *) result, (volatile uint32_t *) RSA_MEM_Z_BLOCK_BASE, 4096 / 32); // Get result
    
    DPORT_REG_WRITE(RSA_INTERRUPT_REG, 1);

    return;
}
//void mult_4096_async();