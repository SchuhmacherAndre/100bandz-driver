#include <ntddk.h>
#include <windef.h>
#include <wdm.h>
#include <wdmsec.h>
#include <intrin.h>

#include "structs.hh"
#include "utils.hh"

#include "ssdt.hh"

PULONGLONG c_ssdt::GetSSDT() {
    utils->DisableWriteProtect(o_IRQL);

    ULONGLONG KiSystemCall64 = __readmsr(0xC0000082);
    ULONGLONG KiSystemServiceRepeat = 0;
    INT32 Limit = 4096;

    // defo better way to do this XD but it works for now @n0verify
    for (int i = 0; i < Limit; i++) {
        if (*(PUINT8)(KiSystemCall64 + i) == 0x4C &&
            *(PUINT8)(KiSystemCall64 + i + 1) == 0x8D &&
            *(PUINT8)(KiSystemCall64 + i + 2) == 0x15) {
            KiSystemServiceRepeat = KiSystemCall64 + i;
            PULONGLONG result = (PULONGLONG)(*(PINT32)(KiSystemServiceRepeat + 3) +
                KiSystemServiceRepeat + 7);
            utils->EnableWriteProtect(o_IRQL);
            return result;
        }
    }

    utils->EnableWriteProtect(o_IRQL);
    return 0;
}

PVOID c_ssdt::FindFunctionAddressInSSDT(uintptr_t idx) {
    SDT* ServiceDescriptorTable = (SDT*)GetSSDT();
    if (!ServiceDescriptorTable) {
        return 0;
    }
    utils->DisableWriteProtect(o_IRQL);
    UINT64 base = (UINT64)ServiceDescriptorTable->ServiceTable;
    LONG relativeOffset = (LONG)ServiceDescriptorTable->ServiceTable[idx];
    relativeOffset = relativeOffset >> 4;
    PVOID actualAddress = (PVOID)(base + relativeOffset);
    utils->EnableWriteProtect(o_IRQL);
    return actualAddress;
}
KIRQL o_IRQL;
c_ssdt* ssdt;