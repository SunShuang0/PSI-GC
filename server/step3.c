#define _CRT_SECURE_NO_WARNINGS
#include "gen.h"
#include "eval.h"
#include "tree.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "time.h"
#include "string.h"
#include "SM3.h"

/**
 * @brief 从文件k0.dat或k1.dat中读取密钥
 *
 * @param b:b=0,读取k0.dat；b=1,读取k1.dat
 */
num* getKfromFile(char * filename)
{

    int head_len = 0;
    int nodecount = 0;

    FILE* fk1 = fopen(filename, "rb");

    fread((unsigned char*)&nodecount, 4, 1, fk1);
    fread((unsigned char*)&head_len, 4, 1, fk1);


    num* head = init();
    head->length = head_len;
    head->digit = malloc(sizeof(uint64_t) * 2);
    fread(head->digit, head_len / 8, 1, fk1);

    cw_node* pre = NULL;

    for (int i = 0; i < nodecount; i++) {
        cw_node *node = malloc(sizeof(cw_node));
        if (i == 0)
            head->next = node;
        node->digit = malloc(sizeof(uint64_t) * 2);
        fread((unsigned char *)node->digit, sizeof(uint64_t) * 2, 1, fk1);
        fread((unsigned char*)&(node->tcw_l), 4, 1, fk1);
        fread((unsigned char*)&(node->tcw_r), 4, 1, fk1);
        if (pre != NULL) {
            pre->next = node;
        }
        pre = node;

    }
    fclose(fk1);

    return head;
}


/**
 * @brief 监管侧服务器2，读取k1.dat中的密钥，计算Eval2之和
 *从文件中读取eval_k0.dat的数据，进行异或得出最终结果
 *
 */
extern int test11(int count1, char * filename)
{
    // 1 生成x
    num *x_group[10];

    uint32_t *prime = malloc(sizeof(uint32_t)*4);
    prime[0] = 0x00213412;
    prime[1] = 0xff134124;

    SM3_Digest *random = malloc(sizeof(SM3_Digest));
    random = SM3_Dtype_get(prime, 8);

    for (int i = 0; i < 10; i++)
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

    // 2 读出k1，构建tmp_k
    num *k;
    k = getKfromFile(filename);

    num **tmp_k = (num**)malloc(sizeof(num*) * 2);
    tmp_k[1] = k;
    tmp_k[0] = NULL;

    // 3 计算出count2
    int count2 = 0;
    for (int i = 0; i < 10; i++)
    {
        int rv = Eval(1, tmp_k, x_group[i]);
        count2 += rv;
    }

    int result = count1 ^ count2;

    return result;
}
