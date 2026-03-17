cmake_minimum_required(VERSION 3.28)

# Enable NASM compiler
set(CMAKE_ASM_NASM_COMPILER nasm)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

project(kopalnia_os)
#set(CMAKE_VERBOSE_MAKEFILE ON) # Debug only

enable_language(ASM_NASM)

# All .cpp and .asm files
file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE ASM_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.asm")
# ISO
set(ISODIR ${CMAKE_SOURCE_DIR}/isodir)

add_executable(kopalnia_os ${CPP_SOURCES} ${ASM_SOURCES})

target_include_directories(kopalnia_os PUBLIC
        # ig this will be enough
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

set_target_properties(kopalnia_os PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/isodir/boot
)

target_compile_options(kopalnia_os PRIVATE
        # NASM cant read this flags so enable them only for C,C++
        $<$<COMPILE_LANGUAGE:C,CXX>:-m32>
        $<$<COMPILE_LANGUAGE:C,CXX>:-ffreestanding>
        $<$<COMPILE_LANGUAGE:C,CXX>:-fno-exceptions>
        $<$<COMPILE_LANGUAGE:C,CXX>:-fno-stack-protector>
        $<$<COMPILE_LANGUAGE:C,CXX>:-fno-omit-frame-pointer>
        $<$<COMPILE_LANGUAGE:C,CXX>:-fno-builtin>
        $<$<COMPILE_LANGUAGE:C,CXX>:-fno-rtti>
        $<$<COMPILE_LANGUAGE:C,CXX>:-fno-pie>
        $<$<COMPILE_LANGUAGE:C,CXX>:-fno-pic>
        $<$<COMPILE_LANGUAGE:C,CXX>:-march=i386>
        $<$<COMPILE_LANGUAGE:C,CXX>:-mtune=i386>
        $<$<COMPILE_LANGUAGE:C,CXX>:-std=gnu++17>
        -Wall
        -O2
)

target_link_options(kopalnia_os PRIVATE
        -m32
        -nostdlib
        -no-pie
        -T ${CMAKE_SOURCE_DIR}/linker.ld
)

# Make ISO and run it
add_custom_target(run
        COMMAND grub-mkrescue -o ${CMAKE_BINARY_DIR}/mykernel.iso ${ISODIR}
        COMMAND qemu-system-i386 -m 512M -M pc -cdrom ${CMAKE_BINARY_DIR}/mykernel.iso -no-reboot -no-shutdown -serial stdio -vga std #-d int # Debug only
        COMMENT "Building ISO and running QEMU"
        DEPENDS ${CMAKE_PROJECT_NAME}
)