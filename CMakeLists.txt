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
        "${CMAKE_CURRENT_SOURCE_DIR}/src/libs"
)

target_compile_options(kopalnia_os PRIVATE
        # C++/C
        $<$<COMPILE_LANGUAGE:C,CXX>:
        -m64
        -ffreestanding
        -fno-exceptions
        -fno-stack-protector
        -fno-omit-frame-pointer
        -fno-builtin
        -fno-rtti
        -fno-pie
        -fno-pic
        -mcmodel=kernel
        -mno-red-zone
        >
        # NASM (asm)
        $<$<COMPILE_LANGUAGE:NASM>:
        -g -F dwarf
        >
        # general
        -g
        -Wall
        -O0
)

target_compile_options(kopalnia_os PRIVATE
        $<$<OR:$<COMPILE_LANGUAGE:C>,$<COMPILE_LANGUAGE:CXX>>:
        -msse2
        -mfpmath=sse
        -mno-avx
        >
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

set(QEMU_FLAGS
        # PC
        -cpu max
        -m 512M
        -M q35
        -vga std
        # drive
        -cdrom ${CMAKE_BINARY_DIR}/mykernel.iso
        -drive file=../disk.img,format=raw
        # USB
        -device qemu-xhci,id=xhci
        -device usb-storage,drive=usbstick
        -drive if=none,id=usbstick,file=../usb.img,format=raw
        -device virtio-keyboard-pci
        # Debug
        -serial stdio
        #-d trace:usb_xhci_*
        #-d usb,guest_errors,int
        #-d int # Debug
        #-trace usb_xhci_*
        -s
)

# Make ISO and run it
add_custom_target(run
        COMMAND grub-mkrescue -o ${CMAKE_BINARY_DIR}/mykernel.iso ${ISODIR}
        COMMAND qemu-system-x86_64 ${QEMU_FLAGS}
        COMMENT "Building ISO and running QEMU"
        DEPENDS ${CMAKE_PROJECT_NAME}
)