nasm -f elf32 src/boot.asm -o build/boot.o
g++ -m32 -ffreestanding -fno-exceptions -fno-rtti -c src/Kernel.cpp -o build/Kernel.o
ld -m elf_i386 -nostdlib -T linker.ld build/boot.o build/Kernel.o -o isodir/boot/kobylnik_os
grub-mkrescue -o mykernel.iso isodir
qemu-system-i386 -cdrom mykernel.iso