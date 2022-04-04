cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

# Pull in FreeRTOS
include(FreeRTOS_Kernel_import.cmake)

project(omni C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(omni
        main.c
        blinky.c
        nothing.c
        inputPolling.c
        inputIRQ.c
        abencoders.c
        )

pico_enable_stdio_usb(omni 1)
pico_enable_stdio_uart(omni 0)

target_include_directories(omni PRIVATE
        ${CMAKE_CURRENT_LIST_DIR})

target_compile_definitions(omni PRIVATE
        PICO_STDIO_STACK_BUFFER_SIZE=64 # use a small printf on stack buffer
)
target_link_libraries(omni pico_stdlib FreeRTOS-Kernel FreeRTOS-Kernel-Heap4)
pico_add_extra_outputs(omni)

