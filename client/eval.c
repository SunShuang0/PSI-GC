#include "eval.h"
#include "stdlib.h"
#include "stdint.h"
#include "string.h"

/**
 * @brief 258位 x ^ y
 * 
 * @param x 
 * @param y 
 * @return num* 
 */
num* T_xor(num *x, num *y)
{
    int i;
    num* result = malloc(sizeof(num));
    result->digit = malloc(sizeof(uint64_t) * 5);
    for(i = 0; i < 5; i++){
        result->digit[i] = x->digit[i] ^ y->digit[i]; /// result = x ^ y
    }
    result->length = bit_length(result->digit[4]) + 64 * 4; ///计算result长度
    return result;
}

/**
 * @brief 将scw, tcw_l, tcw_r组成一个258位的数据
 * 
 * @param y 
 * @return num* 
 */
num* combine(cw_node *y)
{
    num *result = init();
    result->digit = malloc(sizeof(uint64_t) * 5);
    memcpy(result->digit, y->digit, sizeof(uint64_t) * 2); /// 将scw作为result第一字段  //scw??
    result->digit[2] = (y->digit[0] << 1)  | (y->tcw_l); /// tcw_l 作为第二个字段
    result->digit[3] = (y->digit[0] >> 63) | (y->digit[1] << 1); ///再次赋值scw作为第一个字段
    result->digit[4] = (y->digit[1] >> 63) | (y->tcw_r << 1); /// tcw_r 作为最后一个字段
    return result;
}
int Eval(int b, num **Kb, num *x)
{
    num *ki = init();
    ki = Kb[b]; ///根据b选择对应的key
    int i = 1, count = 1, t = b, flag;
    num *T = init();
    num *s = malloc(sizeof(num*));
    s->digit = malloc(sizeof(uint64_t) * 2); 
    memcpy(s->digit, ki->digit, sizeof(uint64_t) * 2); ///赋值s0
    s->length = bit_length(s->digit[1]) + 64; ///计算s的长度

    cw_node *current = ki->next;
    num *pseudo_s, *cw_combine;
    for(; i <= x->length; i++)//x?s?
    {
        if(i % 64 == 0)
            flag = (((x->digit[(i / 64) - 1]) & 0x8000000000000000) != 0) ? 1 : 0; ///计算flag的值
        else
            flag = (x->digit[i / 64] >> ((i - 1) % 64)) & 1; ///当i % 64 = 0,单独计算

        pseudo_s = pseudo_random_gen(s, 258);/// 生成伪随机数
        if(t == 1)
        {   
            cw_combine = combine(current); ///拓展current成为258位数据
            T = T_xor(pseudo_s, cw_combine); /// T = pseudo_s ^ cw_combine
        }
        else
        {
            T = pseudo_s;
        }

        if(flag == 0)///flag = 0,选取左部分
        {
            memcpy(s->digit, T->digit, sizeof(uint64_t) * 2); 
            s->length = bit_length(s->digit[1]) + 64;
            t = (T->digit[2]) & 0x1;
        }
        else /// flag = 1,选取右部分
        {
            s->digit[0] = ((T->digit[2]) >> 1) | ((T->digit[3] & 0x1) << 63);
            s->digit[1] = ((T->digit[3]) >> 1) | ((T->digit[4] & 0x1) << 63);
            s->length = bit_length(s->digit[1]) + 64;
            t = (T->digit[4] >> 1) & 0x1;
        }

         current = current->next;
        
    }
    if(b == 0)
    {
        return (((s->digit[0] & 1) + t * current->tcw_l) % 2);
    }
    else
        return (2 + (-1 * (((s->digit[0] & 1) + t * current->tcw_l) % 2))) % 2;

}