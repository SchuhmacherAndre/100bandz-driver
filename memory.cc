#include <ntddk.h>

#include "memory.hh"

extern "C" {
    NTKERNELAPI
        NTSTATUS
        ZwQuerySystemInformation(
            IN ULONG SystemInformationClass,
            OUT PVOID SystemInformation,
            IN ULONG SystemInformationLength,
            OUT PULONG ReturnLength OPTIONAL
        );

    NTKERNELAPI PVOID PsGetProcessSectionBaseAddress(__in PEPROCESS Process);
}

NTSTATUS c_memory::read_physical_addr(PVOID targetAddr, PVOID outBuffer, SIZE_T size, SIZE_T* bytesRead) {
    if (!targetAddr || !outBuffer || !bytesRead) return STATUS_INVALID_PARAMETER;

    MM_COPY_ADDRESS addrToRead = { 0 };
    addrToRead.PhysicalAddress.QuadPart = (LONGLONG)targetAddr;
    return MmCopyMemory(outBuffer, addrToRead, size, MM_COPY_MEMORY_PHYSICAL, bytesRead);
}

NTSTATUS c_memory::write_physical_addr(PVOID targetAddr, PVOID inBuffer, SIZE_T size, SIZE_T* bytesWritten) {
    if (!targetAddr || !inBuffer || !bytesWritten) return STATUS_INVALID_PARAMETER;

    PHYSICAL_ADDRESS writeAddr = { 0 };
    writeAddr.QuadPart = (LONGLONG)targetAddr;

    PVOID mappedMem = MmMapIoSpaceEx(writeAddr, size, PAGE_READWRITE);
    if (!mappedMem) return STATUS_INSUFFICIENT_RESOURCES;

    RtlCopyMemory(mappedMem, inBuffer, size);
    *bytesWritten = size;
    MmUnmapIoSpace(mappedMem, size);

    return STATUS_SUCCESS;
}

NTSTATUS c_memory::read_virtual_mem(UINT64 dirBase, UINT64 addr, PUINT8 buffer, SIZE_T size, SIZE_T* bytesRead) {
    if (!buffer || !bytesRead) return STATUS_INVALID_PARAMETER;

    UINT64 physAddr = translate_linear_addr(dirBase, addr);
    if (!physAddr) {
        DbgPrint("translation failed for addr 0x%llx\n", addr);
        return STATUS_UNSUCCESSFUL;
    }

    return read_physical_addr((PVOID)physAddr, buffer, size, bytesRead);
}

NTSTATUS c_memory::write_virtual_mem(UINT64 dirBase, UINT64 addr, PUINT8 buffer, SIZE_T size, SIZE_T* bytesWritten) {
    if (!buffer || !bytesWritten) return STATUS_INVALID_PARAMETER;

    UINT64 physAddr = translate_linear_addr(dirBase, addr);
    if (!physAddr) {
        DbgPrint("translation failed for addr 0x%llx\n", addr);
        return STATUS_UNSUCCESSFUL;
    }

    return write_physical_addr((PVOID)physAddr, buffer, size, bytesWritten);
}

UINT64 c_memory::translate_linear_addr(UINT64 dirTableBase, UINT64 virtAddr) {
    if (!dirTableBase || !virtAddr) return 0;

    dirTableBase &= ~0xf;
    UINT64 pageOffset = virtAddr & ~(~0ul << PAGE_OFFSET_SIZE);
    UINT64 pte = ((virtAddr >> 12) & (0x1ffll));
    UINT64 pt = ((virtAddr >> 21) & (0x1ffll));
    UINT64 pd = ((virtAddr >> 30) & (0x1ffll));
    UINT64 pdp = ((virtAddr >> 39) & (0x1ffll));

    SIZE_T readSize = 0;
    UINT64 pdpe = 0;

    if (!NT_SUCCESS(read_physical_addr((PVOID)(dirTableBase + 8 * pdp), &pdpe, sizeof(pdpe), &readSize)))
        return 0;
    if (~pdpe & 1) return 0;

    UINT64 pde = 0;
    if (!NT_SUCCESS(read_physical_addr((PVOID)((pdpe & PHYS_ADDR_MASK) + 8 * pd), &pde, sizeof(pde), &readSize)))
        return 0;
    if (~pde & 1) return 0;

    if (pde & 0x80)
        return (pde & (~0ull << 42 >> 12)) + (virtAddr & ~(~0ull << 30));

    UINT64 pteAddr = 0;
    if (!NT_SUCCESS(read_physical_addr((PVOID)((pde & PHYS_ADDR_MASK) + 8 * pt), &pteAddr, sizeof(pteAddr), &readSize)))
        return 0;
    if (~pteAddr & 1) return 0;

    if (pteAddr & 0x80)
        return (pteAddr & PHYS_ADDR_MASK) + (virtAddr & ~(~0ull << 21));

    UINT64 finalAddr = 0;
    if (!NT_SUCCESS(read_physical_addr((PVOID)((pteAddr & PHYS_ADDR_MASK) + 8 * pte), &finalAddr, sizeof(finalAddr), &readSize)))
        return 0;

    finalAddr &= PHYS_ADDR_MASK;
    return finalAddr ? finalAddr + pageOffset : 0;
}

DWORD64 c_memory::get_user_dir_table_base_offset() {
    RTL_OSVERSIONINFOW ver = { 0 };
    RtlGetVersion(&ver);
    return ver.dwBuildNumber <= 17763 ? 0x0278 :
        ver.dwBuildNumber <= 18363 ? 0x0280 : 0x0388;
}

ULONG_PTR c_memory::get_proc_cr3(PEPROCESS targetProc) {
    if (!targetProc) return 0;

    PUCHAR procPtr = (PUCHAR)targetProc;
    ULONG_PTR dirBase = *(PULONG_PTR)(procPtr + 0x28);

    if (!dirBase) {
        DWORD64 userDirOffset = get_user_dir_table_base_offset();
        return *(PULONG_PTR)(procPtr + userDirOffset);
    }

    return dirBase;
}
c_memory* memory;