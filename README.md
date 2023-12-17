# Dilithium on ESP32


## How to build / test
This project is built using [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html) and is designed to run on an original ESP32 devkit (although it should also run on other boards and ESP32 variants such as ESP32-S3).

To test on linux, there is a test executable in the test_pc folder. This executable requires `libgmp-dev` to build and can be compiled using,
```
cmake .
make
./test
```

## Contents

```
├── components
│   └── dilithium               Dilithium implementation based on PQClean
│       └── multpoly.h          Functions to multiply polynomials using Kronecker substitution
│       └── hwmult.h            Provides functions that use the RSA coprocessor on the ESP32
│   └── common                  Common files from PQClean
├── main
│   └── main.c                  Main executable for ESP32
├── test_pc
│   └── main.c                  Main executable for linux / windows
└── README.md                   This is the file you are currently reading
```
