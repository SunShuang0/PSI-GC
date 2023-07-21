#include "SM3.h"
#include "stdlib.h"
#include "string.h"
// #include"type.h"
//=================================================================
//                          逻辑函数
//=================================================================
#define rol(x, t)(((x) << (t)) | ((x) >> (32 - (t))))
//===================================================================
//                           块操作
//===================================================================

SM3_Dtype* SM3_Dtype_create(void) {
    SM3_Dtype* ctx = (SM3_Dtype*)malloc(sizeof(SM3_Dtype));
    SM3_Dtype_reset(ctx);
    return ctx;
}//创建一个块
void SM3_Dtype_reset(SM3_Dtype* ctx) {
    ctx->h[0] = 0x7380166F;
    ctx->h[1] = 0x4914B2B9;
    ctx->h[2] = 0x172442D7;
    ctx->h[3] = 0xDA8A0600;
    ctx->h[4] = 0xA96F30BC;
    ctx->h[5] = 0x163138AA;
    ctx->h[6] = 0xE38DEE4D;
    ctx->h[7] = 0xB0FB0E4E;
    ctx->bytes = 0;
    ctx->cur = 0;
}
//初始化块
#define P0(x)((x) ^ rol((x), 9) ^ rol((x), 17))
#define P1(x)((x) ^ rol((x), 15) ^ rol((x), 23))
#define get32(p)\
(\
                  (uint32_t)((uint8_t)*((uint8_t *)(p)+3))  \
                  |  ((uint8_t)*((uint8_t *)(p)+2)<<  8)    \
                  |  ((uint8_t)*((uint8_t *)(p)+1)<< 16)    \
                  |  ((uint8_t)*((uint8_t *)(p)+0)<< 24)    \
)
#define FF0(x, y, z)(x ^ y ^ z)//(i <= 15)
#define FF1(x, y, z)((x & y) | (x & z) | (y & z))//(16 <= i <= 63)
#define GG0(x, y, z)(x ^ y ^ z)//(i <= 15)
#define GG1(x, y, z)((x & y) | (~x & z))//(16 <= i <= 63)
//逻辑函数
void SM3_Dtype_process(SM3_Dtype* ctx) {
    uint32_t W1[68], W2[64], ss1, ss2, tt1, tt2;
    uint32_t A = ctx->h[0];
    uint32_t B = ctx->h[1];
    uint32_t C = ctx->h[2];
    uint32_t D = ctx->h[3];
    uint32_t E = ctx->h[4];
    uint32_t F = ctx->h[5];
    uint32_t G = ctx->h[6];
    uint32_t H = ctx->h[7];
    W1[0]  = get32(&((uint32_t*)ctx->block)[0]);
    W1[1]  = get32(&((uint32_t*)ctx->block)[1]);
    W1[2]  = get32(&((uint32_t*)ctx->block)[2]);
    W1[3]  = get32(&((uint32_t*)ctx->block)[3]);
    W1[4]  = get32(&((uint32_t*)ctx->block)[4]);
    W1[5]  = get32(&((uint32_t*)ctx->block)[5]);
    W1[6]  = get32(&((uint32_t*)ctx->block)[6]);
    W1[7]  = get32(&((uint32_t*)ctx->block)[7]);
    W1[8]  = get32(&((uint32_t*)ctx->block)[8]);
    W1[9]  = get32(&((uint32_t*)ctx->block)[9]);
    W1[10] = get32(&((uint32_t*)ctx->block)[10]);
    W1[11] = get32(&((uint32_t*)ctx->block)[11]);
    W1[12] = get32(&((uint32_t*)ctx->block)[12]);
    W1[13] = get32(&((uint32_t*)ctx->block)[13]);
    W1[14] = get32(&((uint32_t*)ctx->block)[14]);
    W1[15] = get32(&((uint32_t*)ctx->block)[15]);
    int t;
    //消息扩展
    for(t = 16; t < 68; t++) {
        uint32_t temp;
        W1[t] = P1(W1[t - 16] ^ W1[t - 9] ^ rol(W1[t - 3], 15)) ^ rol(W1[t - 13], 7) ^ W1[t - 6];
    }
    for(t = 0; t < 64; t++) {
        W2[t] = W1[t] ^ W1[t + 4];
        }
    //压缩过程
    for(t = 0; t < 64; t++) {
        if (t < 16) {
            ss1 = rol((rol(A, 12) + E + rol(0x79cc4519, t)), 7);
            ss2 = ss1 ^ rol(A, 12);
            tt1 = FF0(A, B, C) + D + ss2 + W2[t];
            tt2 = GG0(E, F, G) + H + ss1 + W1[t];
        }
        else {
            ss1 = rol((rol(A, 12) + E + rol(0x7a879d8a, t)), 7);
            ss2 = ss1 ^ rol(A, 12);;
            tt1 = FF1(A, B, C) + D + ss2 + W2[t];
            tt2 = GG1(E, F, G) + H + ss1 + W1[t];
        }
        D = C;
        C = rol(B, 9);
        B = A;
        A = tt1;
        H = G;
        G = rol(F, 19);
        F = E;
        E = P0(tt2);
    }
    ctx->h[0] ^= A;
    ctx->h[1] ^= B;
    ctx->h[2] ^= C;
    ctx->h[3] ^= D;
    ctx->h[4] ^= E;
    ctx->h[5] ^= F;
    ctx->h[6] ^= G;
    ctx->h[7] ^= H;
}
//运算一个块
SM3_Digest* SM3_Dtype_getDigest(SM3_Dtype* ctx) {
    ctx->block[ctx->cur++] = 0x80;
    if (ctx->cur > 56) {
        memset(&ctx->block[ctx->cur], 0, 64 - ctx->cur);
        SM3_Dtype_process(ctx);
        ctx->cur = 0;
    }
    memset(&ctx->block[ctx->cur], 0, 56 - ctx->cur);
    uint64_t bits = ctx->bytes * 8;
    ctx->block[56] = (uint8_t)(bits >> 56 & 0xff);
    ctx->block[57] = (uint8_t)(bits >> 48 & 0xff);
    ctx->block[58] = (uint8_t)(bits >> 40 & 0xff);
    ctx->block[59] = (uint8_t)(bits >> 32 & 0xff);
    ctx->block[60] = (uint8_t)(bits >> 24 & 0xff);
    ctx->block[61] = (uint8_t)(bits >> 16 & 0xff);
    ctx->block[62] = (uint8_t)(bits >> 8 & 0xff);
    ctx->block[63] = (uint8_t)(bits >> 0 & 0xff);
    SM3_Dtype_process(ctx);
    {
        SM3_Digest *result = malloc(sizeof(SM3_Digest));
        int i;
        for (i = 0; i < 8; i++) {
            result->digest[i] = ctx->h[i];
        }
        SM3_Dtype_reset(ctx);
        return result;
    }
}
//计算SM3hash值
SM3_Digest* SM3_Dtype_get(void* msg, uint64_t bytes) {
    SM3_Dtype* ctx = SM3_Dtype_create();
    SM3_Dtype_write(ctx, msg, bytes);
    return SM3_Dtype_getDigest(ctx);
}
//将传入的数据进行处理
void SM3_Dtype_write(SM3_Dtype* ctx, const void* msg, uint64_t bytes) {
    ctx->bytes += bytes;
    const uint8_t* src = msg;
    while (bytes) {
        if (bytes >= 64) {
            memcpy(ctx->block, src, 64);
            src += 64;
            bytes -= 64;
            SM3_Dtype_process(ctx);
        }
        else {
            bytes--;
            ctx->block[ctx->cur++] = *src++;
        }
    }
}
//对块进行赋值
void SM3Digest_toStr(SM3_Digest* digest, char* src) {
    int i;
    for (i = 0; i < 32; i++) {
        int c0 = ((uint8_t*)digest->digest)[i] >> 4;
        int c1 = ((uint8_t*)digest->digest)[i] & 0xf;
        c0 = c0 <= 9 ? '0' + c0 : 'a' + c0 - 0xa;
        c1 = c1 <= 9 ? '0' + c1 : 'a' + c1 - 0xa;
        *src++ = (char)c0;
        *src++ = (char)c1;
    }
    *src = '\0';
}
//将SM3的二进制数据转化为十六进制


//======================================================================
//                   读文件操作
//======================================================================
// char* FILE_read(FILE* fp) {
// 	uint64_t* length;
// 	length = (uint64_t*)malloc(sizeof(uint64_t));
// 	fseek(fp, 0, SEEK_END);
// 	*length = ftell(fp);
// 	char* data;
// 	data = (char*)malloc((*length + 1) * sizeof(char));
// 	rewind(fp);
// 	*length = fread(data, 1, *length, fp);
// 	data[*length] = '\0';
// 	return data;
// }
