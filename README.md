# _ESP32 Kyber_


## How to build / test
This project is built using [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html) and is designed to run on an original ESP32 development (although it should also run on other boards and ESP32 variants such as ESP32-S3).

## Contents

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt`
files that provide set of directives and instructions describing the project's source files and targets
(executable, library, or both). 

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── components
│   └── kyber                   Kyber implementation (based on PQClean)
│       └── kyber               Main kyber implementation files    
│       └── common              Additional functions required for kyber incluing hash functions
├── main
│   └── main.c                  Main program and testing functions
└── README.md                   This is the file you are currently reading
```
