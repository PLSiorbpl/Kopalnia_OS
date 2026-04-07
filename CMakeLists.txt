cmake_minimum_required(VERSION 3.28)

# Enable NASM compiler
set(CMAKE_ASM_NASM_COMPILER nasm)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

project(kopalnia_os)

enable_language(ASM_NASM)

#set(CMAKE_VERBOSE_MAKEFILE ON) # Debug only

# All 64bit and 32bit .cpp and .asm files
file(GLOB_RECURSE CPP_SOURCES_64 CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE ASM_SOURCES_64 CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.asm")
file(GLOB_RECURSE ASM_SOURCES_32 CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src_boot/*.asm")
# ISO
set(ISODIR ${CMAKE_SOURCE_DIR}/isodir)

add_executable(kopalnia_os ${ASM_SOURCES_32} ${CPP_SOURCES_64} ${ASM_SOURCES_64})

target_include_directories(kopalnia_os PUBLIC
        # ig this will be enough
        "${CMAKE_CURRENT_SOURCE_DIR}/src_boot"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/PLlib"
)

target_compile_options(kopalnia_os PRIVATE
        # NASM cant read this flags so enable them only for C,C++
        $<$<COMPILE_LANGUAGE:C,CXX>:-m64>
        $<$<COMPILE_LANGUAGE:C,CXX>:-ffreestanding>
        $<$<COMPILE_LANGUAGE:C,CXX>:-fno-exceptions>
        $<$<COMPILE_LANGUAGE:C,CXX>:-fno-stack-protector>
        $<$<COMPILE_LANGUAGE:C,CXX>:-fno-omit-frame-pointer>
        $<$<COMPILE_LANGUAGE:C,CXX>:-fno-builtin>
        $<$<COMPILE_LANGUAGE:C,CXX>:-fno-rtti>
        $<$<COMPILE_LANGUAGE:C,CXX>:-fno-pie>
        $<$<COMPILE_LANGUAGE:C,CXX>:-fno-pic>
        $<$<COMPILE_LANGUAGE:C,CXX>:-std=gnu++17>
        $<$<COMPILE_LANGUAGE:C,CXX>:-mcmodel=kernel>
        -Wall
        -O2
)

set_target_properties(kopalnia_os PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/isodir/boot
)

target_link_options(kopalnia_os PRIVATE
        -m64
        -nostdlib
        -no-pie
        -T ${CMAKE_SOURCE_DIR}/linker.ld
)

# Make ISO and run it
add_custom_target(run
        COMMAND grub-mkrescue -o ${CMAKE_BINARY_DIR}/mykernel.iso ${ISODIR}
        COMMAND qemu-system-x86_64 -cpu max -m 512M -M pc -vga std -cdrom ${CMAKE_BINARY_DIR}/mykernel.iso -serial stdio #-d int # Debug only
        COMMENT "Building ISO and running QEMU"
        DEPENDS ${CMAKE_PROJECT_NAME}
)