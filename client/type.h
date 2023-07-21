#ifndef TYPE_H
#define TYPE_H
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "assert.h"
#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(disable:4996)

typedef struct _ECC_point {
    uint64_t x[32];
    uint64_t y[32];
}ECC_point;
//256位ECC基点
typedef struct _AES_keytype {
    uint32_t eK[44], dK[44];
    int Nr;
}AES_key;
//AES秘钥结构
typedef struct _DES_Dtype {
    char block[64];
}DES_Dtype;
// DES数据结构
typedef struct _SM3_Dtype {
    uint8_t block[64];
    uint64_t bytes;
    uint32_t h[8];
    uint64_t cur;
}SM3_Dtype;
// SM3数据结构
typedef struct _SM3_Digest {
    uint32_t digest[8];
}SM3_Digest;
// SM3摘要结构
typedef struct _sha256_Dtype{
    uint8_t block[64];
    uint64_t bytes;
    uint32_t h[8];
    uint64_t cur;
}sha256_Dtype;
// sha256数据结构
typedef struct _sha256_Digest {
    uint32_t digest[8];
} sha256_Digest;
// sha256摘要结构（256位）
#endif // !TYPE_H

