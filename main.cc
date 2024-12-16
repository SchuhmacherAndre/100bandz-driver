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

#pragma warning(disable : 4083 4005 4100 4003 4242 4244 4189 4022 4047 4152 4055 4142)

NTSTATUS EncryptionTest()
{
    UINT8 key[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };

    UINT8 iv[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                   0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

    UINT8 data[] = "ffuck you eac lalalalal";
    UINT8 encrypted[32];
    UINT8 decrypted[32];

    RtlCopyMemory(encrypted, data, sizeof(data));

    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);

    AES_CBC_encrypt_buffer(&ctx, encrypted, sizeof(encrypted));

    CHAR hexString[100] = { 0 };
    for (int i = 0; i < sizeof(encrypted); i++) {
        RtlStringCchPrintfA(&hexString[i * 3], 4, "%02X ", encrypted[i]);
    }
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Encrypted data: %s\n", hexString);

    RtlCopyMemory(decrypted, encrypted, sizeof(encrypted));

    AES_init_ctx_iv(&ctx, key, iv);

    AES_CBC_decrypt_buffer(&ctx, decrypted, sizeof(decrypted));

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Decrypted data: %s\n", decrypted);

    return STATUS_SUCCESS;
}

//NTSTATUS getNtReadVirtualMemoryTest() {
//    __try {
//        SDT* ServiceDescriptorTable = (SDT*)ssdt::GetSSDT();
//        if (!ServiceDescriptorTable) {
//            return STATUS_UNSUCCESSFUL;
//        }
//        return ssdt::FindFunctionAddressInSSDT(0x003f);
//    }
//    __except (EXCEPTION_EXECUTE_HANDLER) {
//        return STATUS_UNSUCCESSFUL;
//    }
//}


NTSTATUS hunnidbandzEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);
    return EncryptionTest();
}

