#define _CRT_SECURE_NO_WARNINGS
// #include "step1.h"
#include "eval.h"
#include "gen.h"
#include "tree.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "time.h"
#include "string.h"
#include "SM3.h"

/*
生产侧，生成密钥并将两个密钥写到k0.dat、k1.dat中
产生k0和k1，记录到k0.dat和k1.dat
分开运行时，请将k0.dat传递給server1[server1可能是自身],将k1.dat传给server2
*/

/**
 * @brief 将密钥写到文件中
 *
 * @param num* k1密钥
 * @param FILE* fp文件名
 */
void writefile(num* k1, FILE* fp)
{

    uint64_t* d = k1->digit;
    int nodecount = 0;
    fwrite((unsigned char*)&nodecount, sizeof(int), 1, fp);
    fwrite((unsigned char*)&(k1->length), sizeof(int), 1, fp);
    fwrite((unsigned char*)&d[0], 8, 1, fp);
    fwrite((unsigned char*)&d[1], 8, 1, fp);
    cw_node* n = k1->next;
    int cc = 0;
    uint64_t empty = 0;
    while (n != NULL & d != NULL) {
        d = n->digit;
        if (d != NULL) {
            fwrite((unsigned char*)&d[0], 8, 1, fp);
            fwrite((unsigned char*)&d[1], 8, 1, fp);
        }
        else
        {
            fwrite((unsigned char*)&empty, 8, 1, fp);
            fwrite((unsigned char*)&empty, 8, 1, fp);
        }
        int l = n->tcw_l;
        fwrite((unsigned char*)&l, sizeof(int), 1, fp);
        int r = n->tcw_r;
        fwrite((unsigned char*)&r, sizeof(int), 1, fp);
        cc++;
        n = n->next;
    }
    rewind(fp);
    fwrite((unsigned char*)&cc, sizeof(int), 1, fp);
}

/**
 * @brief 生产侧，生成密钥并将两个密钥写到k0.dat、k1.dat中
 *
 * @param alpha 客户端的数据
 */
extern int server1(int getalpha, char * filenumber)
{
    num *alpha = malloc(sizeof(num*));
    num *x_group[10];
    num *alpha_group[100];

    uint32_t *prime = malloc(sizeof(uint32_t)*4);
    prime[0] = 0x00213412;
    prime[1] = 0xff134124;

    SM3_Digest *random = malloc(sizeof(SM3_Digest));
    random = SM3_Dtype_get(prime, 8);
    for(int i = 0; i < 10; i++) // 生成x，并循环处理
    {
        num *x = malloc(sizeof(num*));
        x->digit = malloc(sizeof(uint64_t) * 2);
        x->digit[0] = (uint64_t)random->digest[0];
        x->digit[0] >>= 24;
        x->length = 8;
        x->digit[0] &= 255;
        x_group[i] = x;
        prime[0] = random->digest[0];
        prime[1] = random->digest[1];
        prime[2] = random->digest[2];
        prime[3] = random->digest[3];
        random = SM3_Dtype_get(prime, 16);
        x->digit[0] = i + 100;
    }

    // 接收用户输入alpha
    alpha->digit = malloc(sizeof(uint64_t) * 2);
    memset(alpha->digit, 0, sizeof(uint64_t));
    alpha->digit[0] = getalpha;
    alpha->length = 8;
    alpha_group[0] = alpha;

    int count = 0;
    num ** tmp_k = (num**)malloc(sizeof (num*) * 2);
    tmp_k = Gen(128, alpha_group[0], 1);    // tmp_k需要传出去
    for(int i = 0; i < 10; i++)
    {
        // 原代码循环10轮，每轮产生一个Eval1、一个Eval2，二者异或后得到count
        // count累加10轮，存入result数组。有几个alpha，result数组就有几个元素
        int Eval1 = Eval(0, tmp_k, x_group[i]);
        count += Eval1; // count 要返回给QT，传给监管侧
    }

    // tmp_k[1]传出去

    FILE* fk1 = fopen(filenumber, "wb");
    writefile(tmp_k[1], fk1);
    fclose(fk1);

    return count;

}
