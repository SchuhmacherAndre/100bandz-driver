#include <ntddk.h>
#include <windef.h>
#include <wdm.h>
#include <wdmsec.h>
#include <intrin.h>
#include "structs.hh"
#include "utils.hh"
#include "ssdt.hh"
#include "logger.hh"

PULONGLONG c_ssdt::GetSSDT() {
    LOG_INFO("starting ssdt search");

    utils->DisableWriteProtect(o_IRQL);
    ULONGLONG KiSystemCall64 = __readmsr(0xC0000082);
    LOG_INFO("kisystemcall64 address: 0x%llx", KiSystemCall64);

    ULONGLONG KiSystemServiceRepeat = 0;
    INT32 Limit = 4096;

    LOG_INFO("searching for service repeat pattern");
    for (int i = 0; i < Limit; i++) {
        if (*(PUINT8)(KiSystemCall64 + i) == 0x4C &&
            *(PUINT8)(KiSystemCall64 + i + 1) == 0x8D &&
            *(PUINT8)(KiSystemCall64 + i + 2) == 0x15) {

            KiSystemServiceRepeat = KiSystemCall64 + i;
            LOG_INFO("found pattern at offset 0x%x", i);

            PULONGLONG result = (PULONGLONG)(*(PINT32)(KiSystemServiceRepeat + 3) +
                KiSystemServiceRepeat + 7);

            LOG_INFO("ssdt found at 0x%llx", (ULONGLONG)result);
            utils->EnableWriteProtect(o_IRQL);
            return result;
        }
    }

    LOG_ERROR("failed to find ssdt pattern");
    utils->EnableWriteProtect(o_IRQL);
    return 0;
}

PVOID c_ssdt::FindFunctionAddressInSSDT(uintptr_t idx) {
    LOG_INFO("looking for function at index %llu", idx);

    SDT* ServiceDescriptorTable = (SDT*)GetSSDT();
    if (!ServiceDescriptorTable) {
        LOG_ERROR("failed to get ssdt");
        return 0;
    }

    utils->DisableWriteProtect(o_IRQL);

    UINT64 base = (UINT64)ServiceDescriptorTable->ServiceTable;
    LOG_INFO("service table base: 0x%llx", base);

    LONG relativeOffset = (LONG)ServiceDescriptorTable->ServiceTable[idx];
    relativeOffset = relativeOffset >> 4;
    LOG_INFO("relative offset: 0x%x", relativeOffset);

    PVOID actualAddress = (PVOID)(base + relativeOffset);
    LOG_INFO("found function at 0x%llx", (ULONGLONG)actualAddress);

    utils->EnableWriteProtect(o_IRQL);
    return actualAddress;
}

KIRQL o_IRQL;
c_ssdt* ssdt;