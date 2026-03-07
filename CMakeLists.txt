cmake_minimum_required(VERSION 3.28)
project(kobylnik_os)

set(CMAKE_C_STANDARD 14)
set(CMAKE_CXX_STANDARD 14)

set(KERNEL_SRC ${CMAKE_SOURCE_DIR}/src/Kernel.cpp)
set(BOOT_OBJ ${CMAKE_SOURCE_DIR}/build/boot.o)
set(ISODIR ${CMAKE_SOURCE_DIR}/isodir)

add_executable(kobylnik_os ${KERNEL_SRC} ${BOOT_OBJ})

set_target_properties(kobylnik_os PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/isodir/boot
)

target_compile_options(kobylnik_os PRIVATE
        -m32
        -ffreestanding
        -fno-exceptions
        -fno-rtti
)

target_link_options(kobylnik_os PRIVATE
        -m32
        -nostdlib
        -T ${CMAKE_SOURCE_DIR}/linker.ld
)

add_custom_command(TARGET kobylnik_os POST_BUILD
        COMMAND grub-mkrescue -o ${CMAKE_BINARY_DIR}/mykernel.iso ${ISODIR}
        COMMAND qemu-system-i386 -m 512M -cdrom ${CMAKE_BINARY_DIR}/mykernel.iso -serial stdio
        COMMENT "Building ISO and running QEMU"
)