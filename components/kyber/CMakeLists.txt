idf_component_register(SRCS "kyber/cbd.c" "kyber/indcpa.c" "kyber/kem.c" "kyber/ntt.c" "kyber/poly.c" "kyber/polyvec.c" "kyber/reduce.c" "kyber/symmetric-shake.c" "kyber/verify.c"
                  REQUIRES "common"
                  INCLUDE_DIRS "kyber")