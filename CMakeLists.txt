cmake_minimum_required(VERSION 3.13)

# Option to select the build target
option(BUILD_FOR_PICO "Build the firmware for the Pico" ON)

if(BUILD_FOR_PICO)
    # ====================================================================================
    # PICO FIRMWARE BUILD
    # ====================================================================================
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work
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

    set(PICO_BOARD pico_w CACHE STRING "Board type")
    include(pico_sdk_import.cmake)

    project(rp2040_vl53l0x CXX C ASM)

    pico_sdk_init()

    add_executable(rp2040_vl53l0x
            rp2040-vl5310x.cpp
            I2C/I2C.cpp
            vl53l0x/VL53L0X.cpp
            )

    target_link_libraries(rp2040_vl53l0x 
        pico_stdlib 
        hardware_i2c 
        hardware_gpio
    )

    target_include_directories(rp2040_vl53l0x PRIVATE
        .
        vl53l0x
    )

    pico_enable_stdio_usb(rp2040_vl53l0x 1)
    pico_enable_stdio_uart(rp2040_vl53l0x 0)
    pico_add_extra_outputs(rp2040_vl53l0x)

else()
    # ====================================================================================
    # HOST (UNIT TEST) BUILD
    # ====================================================================================
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_CXX_STANDARD 17)

    project(rp2040_vl53l0x_tests CXX C)

    enable_testing()

    add_executable(run_tests
        test/test_main.cpp
        test/unity/unity.c
        I2C/I2C.cpp
        vl53l0x/VL53L0X.cpp
    )

    target_include_directories(run_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test/unity
        ${CMAKE_CURRENT_SOURCE_DIR}/test # For pico_sdk_stubs.h
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/vl53l0x
        ${CMAKE_CURRENT_SOURCE_DIR}/I2C
    )

    add_test(NAME unit_tests COMMAND run_tests)

endif()
