idf_component_register(SRCS "common/aes.c" "common/fips202.c" "common/nistseedexpander.c" "common/randombytes.c" "common/sha2.c" "common/sp800-185.c"
                            "kyber/cbd.c" "kyber/indcpa.c" "kyber/kem.c" "kyber/ntt.c" "kyber/poly.c" "kyber/polyvec.c" "kyber/reduce.c" "kyber/symmetric-shake.c" "kyber/verify.c"
                  INCLUDE_DIRS "kyber" "common")