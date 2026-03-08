cmake_minimum_required(VERSION 3.28)
project(kopalnia_os)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
set(BOOT_OBJ ${CMAKE_SOURCE_DIR}/build/boot.o)
set(ISODIR ${CMAKE_SOURCE_DIR}/isodir)

add_executable(${CMAKE_PROJECT_NAME} ${MY_SOURCES} ${BOOT_OBJ})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Plib"
)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/isodir/boot
)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
        -m32
        -ffreestanding
        -fno-exceptions
        -fno-rtti
        -Wall
        -O2
)

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
        -m32
        -nostdlib
        -T ${CMAKE_SOURCE_DIR}/linker.ld
)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND grub-mkrescue -o ${CMAKE_BINARY_DIR}/mykernel.iso ${ISODIR}
        COMMAND qemu-system-i386 -m 512M -cdrom ${CMAKE_BINARY_DIR}/mykernel.iso -serial stdio
        COMMENT "Building ISO and running QEMU"
)