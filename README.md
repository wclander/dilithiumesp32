# _ESP32 Kyber_


## How to build / test
This project is built using [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html) and is designed to run on an original ESP32 devkit (although it should also run on other boards and ESP32 variants such as ESP32-S3).

## Contents

```
├── components
│   └── kyber                   Kyber implementation (based on PQClean)
│       └── kyber               Main kyber implementation files    
│       └── common              Additional functions required for kyber incluing hash functions
├── main
│   └── main.c                  Testing functions
└── README.md                   This is the file you are currently reading
```
