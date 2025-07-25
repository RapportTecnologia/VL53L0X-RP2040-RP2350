# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(rp2040_vl53l0x CXX C ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(rp2040_vl53l0x
        rp2040-vl5310x.cpp
        I2C/I2C.cpp
        vl53l0x/VL53L0X.cpp
        )

# pull in common dependencies and additional i2c hardware support
target_link_libraries(rp2040_vl53l0x 
    pico_stdlib 
    hardware_i2c 
    hardware_gpio
)

# Include directories for VL53L0X library
target_include_directories(rp2040_vl53l0x PRIVATE
    .
    vl53l0x
)

# Enable USB serial output for printf
pico_enable_stdio_usb(rp2040_vl53l0x 1)
pico_enable_stdio_uart(rp2040_vl53l0x 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(rp2040_vl53l0x)

# add url via pico_set_program_url
