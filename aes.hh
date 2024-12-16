#ifndef _AES_H_
#define _AES_H_

#include <ntddk.h>

#ifndef CBC
#define CBC 1
#endif

#ifndef ECB
#define ECB 1
#endif

#ifndef CTR
#define CTR 1
#endif

#define AES128 1

#define AES_BLOCKLEN 16 

#if defined(AES256) && (AES256 == 1)
#define AES_KEYLEN 32
#define AES_keyExpSize 240
#elif defined(AES192) && (AES192 == 1)
#define AES_KEYLEN 24
#define AES_keyExpSize 208
#else
#define AES_KEYLEN 16   
#define AES_keyExpSize 176
#endif

struct AES_ctx
{
    UINT8 RoundKey[AES_keyExpSize];
#if (defined(CBC) && (CBC == 1)) || (defined(CTR) && (CTR == 1))
    UINT8 Iv[AES_BLOCKLEN];
#endif
};

void AES_init_ctx(struct AES_ctx* ctx, const UINT8* key);
#if (defined(CBC) && (CBC == 1)) || (defined(CTR) && (CTR == 1))
void AES_init_ctx_iv(struct AES_ctx* ctx, const UINT8* key, const UINT8* iv);
void AES_ctx_set_iv(struct AES_ctx* ctx, const UINT8* iv);
#endif

#if defined(ECB) && (ECB == 1)

void AES_ECB_encrypt(const struct AES_ctx* ctx, UINT8* buf);
void AES_ECB_decrypt(const struct AES_ctx* ctx, UINT8* buf);

#endif 

#if defined(CBC) && (CBC == 1)

void AES_CBC_encrypt_buffer(struct AES_ctx* ctx, UINT8* buf, size_t length);
void AES_CBC_decrypt_buffer(struct AES_ctx* ctx, UINT8* buf, size_t length);

#endif 

#if defined(CTR) && (CTR == 1)

void AES_CTR_xcrypt_buffer(struct AES_ctx* ctx, UINT8* buf, size_t length);

#endif 

#endif 