#include <ntddk.h>

#include "memory.hh"
#include "logger.hh"
#include "exports.hh"

#pragma warning(disable : 4311 4302 4083 4005 4319 4100 4003 4242 4244 4189 4022 4047 4152 4055 4142)

NTSTATUS c_memory::read_physical_addr(PVOID targetAddr, PVOID outBuffer, SIZE_T size, SIZE_T* bytesRead) {
    if (!targetAddr || !outBuffer || !bytesRead || !size) {
        LOG_ERROR("invalid parameters for physical read");
        return STATUS_INVALID_PARAMETER;
    }

    *bytesRead = 0;
    MM_COPY_ADDRESS addr;
    addr.PhysicalAddress.QuadPart = (LONGLONG)targetAddr;

    NTSTATUS status = MmCopyMemory(outBuffer, addr, size, MM_COPY_MEMORY_PHYSICAL, bytesRead);
    if (!NT_SUCCESS(status)) {
        LOG_ERROR("physical read failed at address 0x%llx", (UINT64)targetAddr);
        LOG_NTSTATUS(status);
    }
    return status;
}

NTSTATUS c_memory::write_physical_addr(PVOID targetAddr, PVOID inBuffer, SIZE_T size, SIZE_T* bytesWritten) {
    if (!targetAddr || !inBuffer || !bytesWritten) {
        LOG_ERROR("invalid parameters for physical write");
        return STATUS_INVALID_PARAMETER;
    }

    PHYSICAL_ADDRESS writeAddr = { 0 };
    writeAddr.QuadPart = (LONGLONG)targetAddr;

    LOG_INFO("mapping physical memory at 0x%llx", (UINT64)targetAddr);
    PVOID mappedMem = MmMapIoSpaceEx(writeAddr, size, PAGE_READWRITE);
    if (!mappedMem) {
        LOG_ERROR("failed to map physical memory");
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlCopyMemory(mappedMem, inBuffer, size);
    *bytesWritten = size;
    MmUnmapIoSpace(mappedMem, size);

    LOG_INFO("wrote %zu bytes to physical address", *bytesWritten);
    return STATUS_SUCCESS;
}

NTSTATUS c_memory::read_virtual_mem(UINT64 dirBase, UINT64 addr, PUINT8 buffer, SIZE_T size, SIZE_T* bytesRead) {
    if (!buffer || !bytesRead) {
        LOG_ERROR("invalid parameters");
        return STATUS_INVALID_PARAMETER;
    }

    LOG_INFO("translating virtual address 0x%llx", addr);
    UINT64 physAddr = translate_linear_addr(dirBase, addr);

    if (!physAddr) {
        LOG_ERROR("address translation failed for 0x%llx", addr);
        return STATUS_UNSUCCESSFUL;
    }

    NTSTATUS status = read_physical_addr((PVOID)physAddr, buffer, size, bytesRead);

    if (!NT_SUCCESS(status)) {
        LOG_ERROR("failed to read physical address 0x%llx", physAddr);
        LOG_NTSTATUS(status);
    }
    else {
        LOG_INFO("read %zu bytes from physical address 0x%llx", *bytesRead, physAddr);
    }

    return status;
}

NTSTATUS c_memory::write_virtual_mem(UINT64 dirBase, UINT64 addr, PUINT8 buffer, SIZE_T size, SIZE_T* bytesWritten) {
    if (!buffer || !bytesWritten) {
        LOG_ERROR("invalid parameters");
        return STATUS_INVALID_PARAMETER;
    }

    LOG_INFO("translating virtual address 0x%llx", addr);
    UINT64 physAddr = translate_linear_addr(dirBase, addr);

    if (!physAddr) {
        LOG_ERROR("address translation failed for 0x%llx", addr);
        return STATUS_UNSUCCESSFUL;
    }

    NTSTATUS status = write_physical_addr((PVOID)physAddr, buffer, size, bytesWritten);

    if (!NT_SUCCESS(status)) {
        LOG_ERROR("failed to write to physical address 0x%llx", physAddr);
        LOG_NTSTATUS(status);
    }
    else {
        LOG_INFO("wrote %zu bytes to physical address 0x%llx", *bytesWritten, physAddr);
    }

    return status;
}

UINT64 c_memory::translate_linear_addr(UINT64 dirTableBase, UINT64 virtAddr) {
    if (!dirTableBase || !virtAddr) {
        LOG_ERROR("invalid parameters: dirbase=0x%llx virtaddr=0x%llx", dirTableBase, virtAddr);
        return 0;
    }

    LOG_INFO("starting translation: dirbase=0x%llx virtaddr=0x%llx", dirTableBase, virtAddr);

    const UINT64 level_shifts[] = { 39, 30, 21, 12 };
    const UINT64 level_masks[] = { ~(~0ull << 30), ~(~0ull << 21), ~(~0ull << 12) };

    UINT64 addr = dirTableBase & PHYS_ADDR_MASK;
    UINT64 pageOffset = virtAddr & ((1ull << PAGE_OFFSET_SIZE) - 1);

    for (int i = 0; i < 4; i++) {
        UINT64 entry = 0;
        SIZE_T readSize = 0;
        UINT64 index = (virtAddr >> level_shifts[i]) & 0x1FF;

        LOG_INFO("level %d: index=0x%llx addr=0x%llx", i, index, addr + 8 * index);

        if (!NT_SUCCESS(read_physical_addr((PVOID)(addr + 8 * index), &entry, sizeof(entry), &readSize))) {
            LOG_ERROR("failed to read page table at level %d", i);
            return 0;
        }

        if (!(entry & 1)) {
            LOG_ERROR("page not present at level %d: entry=0x%llx", i, entry);
            return 0;
        }

        if (i < 3 && entry & 0x80) {
            UINT64 physAddr = (entry & PHYS_ADDR_MASK) + (virtAddr & level_masks[i]);
            LOG_INFO("large page at level %d: physaddr=0x%llx", i, physAddr);
            return physAddr;
        }

        addr = entry & PHYS_ADDR_MASK;
    }

    UINT64 finalAddr = addr + pageOffset;
    LOG_INFO("translation complete: physaddr=0x%llx", finalAddr);
    return finalAddr;
}

ULONG_PTR c_memory::get_proc_cr3(PEPROCESS targetProc) {
    if (!targetProc) {
        LOG_ERROR("invalid process object");
        return 0;
    }

    PUCHAR procPtr = (PUCHAR)targetProc;
    ULONG_PTR dirBase = *(PULONG_PTR)(procPtr + 0x28);

    if (!dirBase) {
        RTL_OSVERSIONINFOW ver = { 0 };
        RtlGetVersion(&ver);
        LOG_INFO("checking build number %lu for dir offset", ver.dwBuildNumber);

        DWORD64 userDirOffset = ver.dwBuildNumber <= 17763 ? 0x0278 :
            ver.dwBuildNumber <= 18363 ? 0x0280 :
            0x0388;

        dirBase = *(PULONG_PTR)(procPtr + userDirOffset);
        LOG_INFO("using user dir offset 0x%llx, got base 0x%llx", userDirOffset, dirBase);
    }
    else {
        LOG_INFO("found directory base 0x%llx at default offset", dirBase);
    }

    return dirBase;
}
c_memory* memory;