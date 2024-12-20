#include <ntddk.h>
#include <intrin.h>
#include "utils.hh"
#include "logger.hh"

VOID c_utils::DisableWriteProtect(KIRQL& irql) {
    LOG_INFO("raising irql to dpc level");
    irql = KeRaiseIrqlToDpcLevel();

    ULONG_PTR cr0 = __readcr0();
    LOG_INFO("current cr0: 0x%llx", cr0);

    cr0 &= ~0x10000;
    __writecr0(cr0);
    _ReadWriteBarrier();

    LOG_INFO("write protection disabled, new cr0: 0x%llx", __readcr0());
}

VOID c_utils::EnableWriteProtect(KIRQL& irql) {
    ULONG_PTR cr0 = __readcr0();
    LOG_INFO("current cr0: 0x%llx", cr0);

    cr0 |= 0x10000;
    __writecr0(cr0);
    _ReadWriteBarrier();

    LOG_INFO("write protection enabled, new cr0: 0x%llx", __readcr0());

    KeLowerIrql(irql);
    LOG_INFO("irql restored");
}

c_utils* utils;