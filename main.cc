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

#pragma warning(disable : 4311 4302 4083 4005 4100 4003 4242 4244 4189 4022 4047 4152 4055 4142)

PVOID getNtReadVirtualMemoryTest() {
    __try {
        return ssdt->FindFunctionAddressInSSDT(0x003f);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return (PVOID)0x1;
    }
}


NTSTATUS hunnidbandzEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);
    DbgPrint("0x%p", getNtReadVirtualMemoryTest());
    return 0;
}

