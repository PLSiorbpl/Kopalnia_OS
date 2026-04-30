#include <efi.h>
#include <efilib.h>

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    SystemTable->ConOut->OutputString(
        SystemTable->ConOut,
        (CHAR16*)L"Hello\r\n"
    );

    return EFI_SUCCESS;
}