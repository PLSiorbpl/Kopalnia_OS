cmake_minimum_required(VERSION 3.28)

# Enable NASM compiler
set(CMAKE_ASM_NASM_COMPILER nasm)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

project(kopalnia_os)

enable_language(ASM_NASM)

#file(GLOB_RECURSE CPP_SOURCES_32 CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src_boot/*.cpp")
file(GLOB_RECURSE ASM_SOURCES_32 CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src_boot/*.asm")

add_library(boot OBJECT ${ASM_SOURCES_32}) # ${CPP_SOURCES_32}

target_include_directories(boot PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src_boot"
)

target_compile_options(boot PRIVATE
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
        -Wall
        -O2
)

#set(CMAKE_VERBOSE_MAKEFILE ON) # Debug only



# All 64bit .cpp and .asm files
file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE ASM_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.asm")
# ISO
set(ISODIR ${CMAKE_SOURCE_DIR}/isodir)

add_library(kernel OBJECT ${CPP_SOURCES} ${ASM_SOURCES})

target_include_directories(kernel PUBLIC
        # ig this will be enough
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/PLlib"
)

target_compile_options(kernel PRIVATE
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
        -Wall
        -O2
)

add_executable(kopalnia_os $<TARGET_OBJECTS:boot> $<TARGET_OBJECTS:kernel>)

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