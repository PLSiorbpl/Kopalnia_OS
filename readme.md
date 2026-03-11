# Kopalnia-OS
- Very basic *kernel* made by PLSiorbpl in **C++**.
- _**Should**_ work on all modern and older CPUs.

## Structure
```bash
Kopalnia-OS/
├── isodir/   # used to create bootable .iso
├── src/      # Source code
│   ├── Drivers/    # Drivers code
│   ├── PLib/       # Standard library
│   └── Kernel.cpp  # Main Kernel file
├── CMakeLists.txt    # Build Kernel.elf, iso and runs it in qemu
├── build.bash  # Alternative to CMake
└── Linker.ld   # Linker file
```

### Commands
- help
- clear
- echo

## Building
Using CMake:
```bash
> mkdir build/
> cd build/
> cmake ..
> make run    # Automatically runs qemu (VM) and build ISO
# Alternatively just make (wont build ISO and run qemu)
> make
```