#ifndef EXPORTS_HH
#define EXPORTS_HH
#pragma once

#include <ntddk.h>

extern "C" {
    NTKERNELAPI
        NTSTATUS
        ZwQuerySystemInformation(
            IN ULONG SystemInformationClass,
            OUT PVOID SystemInformation,
            IN ULONG SystemInformationLength,
            OUT PULONG ReturnLength OPTIONAL
        );
}

#endif EXPORTS_HH