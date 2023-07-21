#ifndef _SM3_H
#define _SM3_H
#include "stdint.h"
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
void		SM3_Dtype_reset(SM3_Dtype*);
void       	SM3_Dtype_process(SM3_Dtype*);
SM3_Dtype*  SM3_Dtype_create(void);
SM3_Digest*	SM3_Dtype_getDigest(SM3_Dtype*);
SM3_Digest*	SM3_Dtype_get(void*, uint64_t);
void		SM3Digest_toStr(SM3_Digest*, char*);
void		SM3_Dtype_write(SM3_Dtype* ctx, const void* msg, uint64_t bytes);
// char*		FILE_read(FILE*);
#endif // !_SM3_H

