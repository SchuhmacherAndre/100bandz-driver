#include <ntddk.h>
#include <windef.h>
#include <wdm.h>
#include <wdmsec.h>
#include <intrin.h>
#include <ntstrsafe.h>
#include "structs.hh"
#include "utils.hh"
#include "ssdt.hh"
#include "aes.hh"
#include "logger.hh"

#pragma warning(disable : 4311 4302 4083 4005 4319 4100 4003 4242 4244 4189 4022 4047 4152 4055 4142)

PVOID getNtReadVirtualMemoryTest() {
    LOG_INFO("attempting to find ntreadvirtualmemory");

    __try {
        PVOID addr = ssdt->FindFunctionAddressInSSDT(0x003f);
        if (addr) {
            LOG_INFO("found function at 0x%llx", (ULONGLONG)addr);
        }
        else {
            LOG_ERROR("failed to find function");
        }
        return addr;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        LOG_ERROR("exception while finding function");
        return (PVOID)0x1;
    }
}

NTSTATUS hunnidbandzEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath) {
    LOG_INFO("driver initializing");

    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);

    PVOID addr = getNtReadVirtualMemoryTest();
    LOG_INFO("ntreadvirtualmemory located at 0x%llx", (ULONGLONG)addr);

    return STATUS_SUCCESS;
} 