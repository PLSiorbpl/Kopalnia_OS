#include <efi.h>

#include "Loader.hpp"
#include "boot.hpp"

#include "boot_shared.h"

EFI_SYSTEM_TABLE     *ST = nullptr;
EFI_BOOT_SERVICES    *BS = nullptr;
EFI_RUNTIME_SERVICES *RT = nullptr;

EFI_GUID gEfiFileInfoGuid         = EFI_FILE_INFO_ID;
EFI_GUID gEfiLoadedImageGuid      = EFI_LOADED_IMAGE_PROTOCOL_GUID;
EFI_GUID gEfiSimpleFileSystemGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
EFI_GUID gEfiGraphicsOutputGuid   = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    ST = SystemTable;
    BS = SystemTable->BootServices;
    RT = SystemTable->RuntimeServices;

    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop = nullptr;
    BS->LocateProtocol(&gEfiGraphicsOutputGuid, nullptr, reinterpret_cast<void**>(&gop));
    if (!gop)
        return EFI_UNSUPPORTED;

    EFI_FILE* kernel_file = open_kernel_file(ImageHandle, (CHAR16*)L"\\Kernel.elf");
    UINT64 entry_point = load_elf_kernel(kernel_file, gop);

    framebuffer_internal.base = reinterpret_cast<void*>(gop->Mode->FrameBufferBase);
    framebuffer_internal.size = gop->Mode->FrameBufferSize;
    framebuffer_internal.width = gop->Mode->Info->HorizontalResolution;
    framebuffer_internal.height = gop->Mode->Info->VerticalResolution;
    framebuffer_internal.pixels_per_scanline = gop->Mode->Info->PixelsPerScanLine;

    // Memory map + exit boot services
    UINTN map_size = 0, map_key, desc_size;
    UINT32 desc_version;
    EFI_MEMORY_DESCRIPTOR* map = nullptr;

    BS->GetMemoryMap(&map_size, map, &map_key, &desc_size, &desc_version);
    map_size += 2 * desc_size;
    BS->AllocatePool(EfiLoaderData, map_size, reinterpret_cast<void **>(&map));
    BS->GetMemoryMap(&map_size, map, &map_key, &desc_size, &desc_version);

    UINT8* kernel_stack;
    BS->AllocatePool(EfiLoaderData, 64 * 1024, reinterpret_cast<void **>(&kernel_stack));
    auto stack_top = reinterpret_cast<UINT64>(kernel_stack + 64 * 1024);

    EFI_STATUS status = BS->ExitBootServices(ImageHandle, map_key);
    if (EFI_ERROR(status)) {
        map_size += 2 * desc_size;
        BS->GetMemoryMap(&map_size, map, &map_key, &desc_size, &desc_version);
        BS->ExitBootServices(ImageHandle, map_key);
    }

    Framebuffer fb_copy = framebuffer_internal;

    asm volatile(
        "mov %0, %%rsp\n"   // switch stack
        "and $-16, %%rsp\n" // align to 16 B
        "xor %%rbp, %%rbp\n"// clear frame pointer
        "mov %1, %%rdi\n"   // first arg = &g_fb
        "jmp *%2\n"         // jump to kernel entry
        :
        : "r"(stack_top), "r"(reinterpret_cast<UINT64>(&fb_copy)), "r"(entry_point)
        : "rdi", "rbp", "memory"
    );

    while (true) __asm__("hlt");
    return EFI_SUCCESS;
}