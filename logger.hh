#pragma once
#include <ntddk.h>
#include <ntstrsafe.h>

inline void GetTimeString(_Out_ PCHAR TimeStr, _In_ SIZE_T Size) {
    LARGE_INTEGER systemTime, localTime;
    TIME_FIELDS timeFields;

    KeQuerySystemTime(&systemTime);
    ExSystemTimeToLocalTime(&systemTime, &localTime);
    RtlTimeToTimeFields(&localTime, &timeFields);

    RtlStringCchPrintfA(TimeStr, Size, "%02d:%02d:%02d.%03d",
        timeFields.Hour, timeFields.Minute, timeFields.Second, timeFields.Milliseconds);
}

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG_PREFIX "[100bandz]"

#define LOG_TRACE(fmt, ...) \
    do { \
        CHAR timeStr[32]; \
        GetTimeString(timeStr, sizeof(timeStr)); \
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, \
        LOG_PREFIX " [%s] [%s:%d] %s: " fmt "\n", \
        timeStr, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    } while (0)

#define LOG_INFO(fmt, ...) \
    do { \
        CHAR timeStr[32]; \
        GetTimeString(timeStr, sizeof(timeStr)); \
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, \
        LOG_PREFIX " [%s] [%s:%d] %s: " fmt "\n", \
        timeStr, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    } while (0)

#define LOG_ERROR(fmt, ...) \
    do { \
        CHAR timeStr[32]; \
        GetTimeString(timeStr, sizeof(timeStr)); \
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, \
        LOG_PREFIX " [%s] [%s:%d] %s: " fmt "\n", \
        timeStr, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    } while (0)

#define LOG_NTSTATUS(status) \
    do { \
        CHAR timeStr[32]; \
        GetTimeString(timeStr, sizeof(timeStr)); \
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, \
        LOG_PREFIX " [%s] [%s:%d] %s: 0x%08X\n", \
        timeStr, __FILENAME__, __LINE__, __FUNCTION__, status); \
    } while (0)