#include <ntddk.h>
#include <intrin.h>

#include "utils.hh"

VOID utils::DisableWriteProtect(KIRQL& irql) {
    irql = KeRaiseIrqlToDpcLevel();
    ULONG_PTR cr0 = __readcr0();
    cr0 &= ~0x10000;
    __writecr0(cr0);
    _ReadWriteBarrier();
}

VOID utils::EnableWriteProtect(KIRQL& irql) {
    ULONG_PTR cr0 = __readcr0();
    cr0 |= 0x10000;
    __writecr0(cr0);
    _ReadWriteBarrier();
    KeLowerIrql(irql);
}