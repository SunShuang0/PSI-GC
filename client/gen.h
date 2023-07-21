#include "stdint.h"
#ifndef _GEN_H
#define _GEN_H
#define sec_params 128

/**
 * @brief CW = scw//tcw_l//tcw_r
 *
 */
typedef struct _cw_node{
    uint64_t *digit; // scw
    int tcw_l; //tcw_l
    int tcw_r; //tcw_l
    struct _cw_node * next;//下一个修正词
}cw_node;

/**
 * @brief 基本数据类型
 *
 */
typedef struct _num{
    uint64_t *digit; //数据存储
    int length; //数据长度
    struct _cw_node * next;
}num;


/**
 * @brief 生成一个length长度的伪随机数
 *
 * @param seed 初始化种子
 * @param length 伪随机数的长度
 * @return uint64_t
 */
num* pseudo_random_gen(num *seed, int length);

/**
 * @brief 将s转化为群G中的元素
 *
 * @param s
 * @param G
 * @return uint64_t
 */
num Convert(num s, num *G);

/**
 * @brief 返回seed的bit数
 *
 * @param seed
 * @return int
 */
int bit_length(uint64_t seed);

/**
 * @brief
 *
 * @param security_param
 * @param alpha
 * @param beta
 * @param G
 * @return cw_node**
 */

num** Gen(int security_param, num *alpha, int beta);

/**
 * @brief λbit seed 生成 2(λ + 1)bit
 *
 * @param s0
 * @param t0
 * @param s1
 * @param t1
 * @param seed
 */
void gen_tree_node(num *s0, int *t0, num *s1, int *t1, num* seed);


/**
 * @brief  x ^ y
 *
 * @param z
 * @param y
 */
num* num_xor(num *x, num *y);

/**
 * @brief
 *
 * @param seed
 * @param t_l
 * @param t_r
 * @return cw = scw || tcw_l || tcw_r
 */
cw_node* node_correction(num* scw, int t_l, int t_r);

/**
 * @brief
 *
 * @return num*
 */
num* init();


#endif
