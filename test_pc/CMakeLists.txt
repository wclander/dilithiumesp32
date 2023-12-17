project(PCTests)
cmake_minimum_required(VERSION 3.0)

add_library(common STATIC ../components/common/aes.c ../components/common/fips202.c ../components/common/nistseedexpander.c ../components/common/randombytes.c ../components/common/sha2.c ../components/common/sp800-185.c)
target_include_directories(common PRIVATE ../components/common)
add_library(dilithium STATIC ../components/dilithium/multpoly.c ../components/dilithium/ntt.c ../components/dilithium/packing.c ../components/dilithium/poly.c ../components/dilithium/polyvec.c ../components/dilithium/reduce.c ../components/dilithium/rounding.c ../components/dilithium/sign.c ../components/dilithium/symmetric-shake.c)
find_library(gmp gmp)
target_link_libraries(dilithium PRIVATE common gmp)
target_include_directories(dilithium PRIVATE ../components/common)
target_include_directories(dilithium PRIVATE ../components/dilithium)

add_executable(test main.c)
target_link_libraries(test PRIVATE dilithium)
target_include_directories(test PRIVATE ../components/dilithium)