#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "gen.h"
#include "SM3.h"
#include "tree.h"


/**
 * @brief 开辟一个num结构体大小的动态空间
          定义一个指针变量x，令其指向该空间
          通过x访问结构体内部成员变量，并令各成员变量值为空
 * @return num型指针变量x
 */
num * init()
{
    num *x = malloc(sizeof(num));
    x->digit = NULL;//digit是指针，令其指向空地址
    x->length = 0;
    x->next = NULL;
    return x;
}

/**
 * @brief 返回一个数的bit数目
 *
 * @param seed
 * @return int
 */
int bit_length(uint64_t seed)
{
    int length = 0;
    while(seed > 0)
    {
        length++; /// 每次向右移位，获取seed长度
        seed >>= 1;
    }
    return length;
}

/**
 * @brief 生成length长度的伪随机数
 *
 * @param seed
 * @param length
 * @return num*
 */
num* pseudo_random_gen(num *seed, int length)//传入一个num结构体的地址，以及一个int值
{

    int i, j, n, m;
    num *pse_num = init();	//开辟一个新的num结构的空间，定义一个指针变量pse_num拿到该空间地址
    pse_num->length = length;//当初始化后pse_num的digit、next都指空，length为0，现给length赋值为传入的参数值

    SM3_Digest *SM3_prime_digest = malloc(sizeof(SM3_Digest));//开辟SM3_Digest结构(数组digest[8])的空间，定义指针变量SM3_prime_digest拿到该空间地址
    n = (length >> 6) + 1 - (length % 64 == 0); //n与传入的参数length相关
    m = (seed->length) >> 6;					//m与传入的参数seed所指向的 num结构体的成员变量length 相关

                                                  //根据n的值开辟新空间，存入pse_num中的指针变量digit中，那么当下pse_num指向一个num结构体
    pse_num->digit = malloc(sizeof(uint64_t) * n);//pse_num的成员变量digit指向一个uint64_t数据/结构体
    memset(pse_num->digit, 0, sizeof(uint64_t) * n);//初始化

    SM3_prime_digest = SM3_Dtype_get(seed->digit, seed->length >> 3); //将种子传入SM3中计算其hash值，返回一个uint32_t型数组，共8个元素

    for(i = 0, j = 0; i < n; i++, j += 2)//i代表n
    {
        if(i != n - 1)//如果不是最后一位
            pse_num->digit[i] = (uint64_t)(SM3_prime_digest->digest[j + 1]) << 32 | (uint64_t)(SM3_prime_digest->digest[j]); /// 将两个32位拼接为64位
        else
        {
            pse_num->digit[i] = (uint64_t)(SM3_prime_digest->digest[2]) << 32 | (uint64_t)(SM3_prime_digest->digest[3]); ///最后一个数据根据长度移位
            pse_num->digit[i] >>= bit_length(pse_num->digit[i]) - (length % 64); /// 计算伪随机数树长度
        }
        //pse_num->digit指向一个数组，元素个数为n，每个元素都是一个19个字节的数，如4345238205797146445
        //返回的pse_num指向的num结构体：digit如上，length=length(258)，cw_node * next = NULL(仍是初始化状态)
    }
    return pse_num;
}

/**
 * @brief x ^ y
 *
 * @param x
 * @param y
 * @return num*
 */
num* num_xor(num *x, num *y)
{
    int i;
    num *result = init();
    result->digit = malloc(sizeof(uint64_t) * 2);
    memset(result->digit, 0, sizeof(uint64_t) * 2);

    for(i = 0; i < 2; i++){
        result->digit[i] = (x->digit[i]) ^ (y->digit[i]); /// result = x ^ y
    }
    result->length = bit_length(result->digit[1]) + 64; /// 计算result的长度
    return result;

}

/**
 * @brief 利用seed^(i - 1)生成sL,tL,sR,t0R
 *
 * @param s0
 * @param t0
 * @param s1
 * @param t1
 * @param seed
 */
void gen_tree_node(num *s0, int *t0, num *s1, int *t1, num *seed)
{
    num *tmp = init();
    tmp = pseudo_random_gen(seed, 258); //获取伪随机数,tmp长度为8
//pseudo_random_gen()返回一个num型指针值，将其赋予tmp
//tmp指向的num结构体：tmp->digit指向一个数组，元素个数为n，每个元素都是一个19个字节的数，tmp->length=258，tmp->next = NULL(仍是初始化状态)
//下面的s0、s1也是跟tmp一样的指针，指向同样的num结构体

    s0->digit = malloc(sizeof(uint64_t) * 2);//给s0、s1所指的结构体的digit开辟空间
    s1->digit = malloc(sizeof(uint64_t) * 2); //tmp的digit最初开辟的空间大小： pse_num->digit = malloc(sizeof(uint64_t) * n)

    memset(s0->digit, 0, sizeof(uint64_t) * 2);//s0、s1初始化
    memset(s1->digit, 0, sizeof(uint64_t) * 2);

    memcpy(s0->digit, tmp->digit, sizeof(uint64_t) * 2);//把tmp->digit[0]的值的前128bit(16位)给s0->digit
    s1->digit[0] = ((tmp->digit[2]) >> 1) | ((tmp->digit[3] & 0x1) << 63); //s1为tmp->digit[2]、digit[3]处理后拼接  tmp的130-257bit
    s1->digit[1] = ((tmp->digit[3]) >> 1) | ((tmp->digit[4] & 0x1) << 63);

    s0->length = bit_length(s0->digit[1]) + 64; /// 获取s0长度
    s1->length = bit_length(s1->digit[1]) + 64; /// 获取s1长度

    *t0 = tmp->digit[2] & 0x1; /// t0为tmp第129bit
    *t1 = tmp->digit[4] >> 1;  /// t1为tmp第258bit

    free(tmp);
}

/**
 * @brief 利用修正词对GGM树中的结点进行修改
 *
 * @param scw
 * @param tcw_l
 * @param tcw_r
 * @return cw_node*
 */
cw_node* node_correction(num *scw, int tcw_l, int tcw_r)
{
    cw_node *result = malloc(sizeof(cw_node));
    result->digit = malloc(sizeof(uint64_t) * 2);
    memcpy(result->digit, scw->digit, sizeof(uint64_t) * 2); /// scw进行修改
    result->tcw_l = tcw_l; /// tcw_l进行修改
    result->tcw_r = tcw_r; /// tcw_r进行修改
    return result;
}



//
num** Gen(int security_param, num* alpha, int beta)
{
    num *s0_L = init(),
        *s1_L = init(),
        *s0_R = init(),
        *s1_R = init();
    num *s0_keep = init(),
        *s1_keep = init();
    num *scw = init();
    num *seed0 = init(),
        *seed1 = init(),
        *seed0_next = init(),
        *seed1_next = init();
    node *Tree0,
         *Tree1,
         *Tree0_cur = malloc(sizeof(node)),
         *Tree1_cur = malloc(sizeof(node));
    cw_node *cw_current = malloc(sizeof(cw_node)),
               *cw_head = malloc(sizeof(cw_node));

    int t0 = 0,
        t1 = 1;
    int t0_L = 0,
        t1_L = 0,
        t0_R = 0,
        t1_R = 0,
        i,
        flag;
    int t0_keep,
        t1_keep;
    int tcw_l,
        tcw_r;


    seed0->digit = malloc(sizeof(uint64_t) * 2);
    seed1->digit = malloc(sizeof(uint64_t) * 2);
    seed0->digit[0] = 0xff1a14c586786886;/// 对s0进行初始化
    seed0->digit[1] = 0xa541689312978234;
    seed1->digit[0] = 0x86876c78ae767868;/// 对s1进行初始化
    seed1->digit[1] = 0xb768761213567323;
    seed0->length = bit_length(seed0->digit[1]) + 64; /// 获取s0长度
    seed1->length = bit_length(seed1->digit[1]) + 64; /// 获取s1长度

    Tree0 = create_tree_node(seed0, t0, NULL); /// 初始化tree0
    Tree1 = create_tree_node(seed1, t1, NULL); /// 初始化tree1
    Tree0_cur = Tree0;
    Tree1_cur = Tree1;

     for(i = 1; i <= alpha->length; i++)
    {
        node *Tree0_node = malloc(sizeof(node));
        node *Tree1_node = malloc(sizeof(node));
        if(i % 64 == 0)
            flag = (((alpha->digit[(i / 64) - 1]) & 0x8000000000000000) != 0) ? 1 : 0; /// 计算flag值
        else
            flag = (alpha->digit[i / 64] >> ((i - 1) % 64)) & 1;

        gen_tree_node(s0_L, &t0_L, s0_R, &t0_R, Tree0_cur->seed); /// 将计算好的s，t放入树结构中
        gen_tree_node(s1_L, &t1_L, s1_R, &t1_R, Tree1_cur->seed);
        if(flag == 0)
        {
            s0_keep = s0_L;
            s1_keep = s1_L;
            t0_keep = t0_L;
            t1_keep = t1_L;
            scw = num_xor(s0_R, s1_R); /// scw = s0_R ^ s1_R
        }
        else
        {
            s0_keep = s0_R;
            s1_keep = s1_R;
            t0_keep = t0_R;
            t1_keep = t1_R;
            scw = num_xor(s0_L, s1_L); /// scw = s0-L ^ s1_L
        }

        Tree0_node = create_tree_node(s0_keep, t0_keep, Tree0_cur); /// 将节点与上一个节点连接
        Tree1_node = create_tree_node(s1_keep, t1_keep, Tree1_cur);

        Tree0_cur = Tree0_node;
        Tree1_cur = Tree1_node;
        tcw_l = t0_L ^ t1_L ^ flag ^ 1;
        tcw_r = t0_R ^ t1_R ^ flag;

        cw_node *cw_new = node_correction(scw, tcw_l, tcw_r);
        if(i == 1){
            cw_current = cw_new; /// 形成双向连边结构
            cw_head = cw_current;
        }
        else
        {
            cw_current->next = cw_new;
            cw_current = cw_new;
        }

        if(Tree0_cur->prev->control_bit == 1)
        {
            Tree0_cur->seed = num_xor(Tree0_cur->seed, scw); /// 对节点进行修改
            if(flag == 0)
            {
               Tree0_cur->control_bit = Tree0_cur->control_bit ^ tcw_l;
            }
            else
            {
               Tree0_cur->control_bit = Tree0_cur->control_bit ^ tcw_r;
            }

        }

        if(Tree1_cur->prev->control_bit == 1)
        {
            Tree1_cur->seed = num_xor(Tree1_cur->seed, scw);
            if(flag == 0)
            {
                Tree1_cur->control_bit = Tree1_cur->control_bit ^ tcw_l;
            }
            else
            {
                Tree1_cur->control_bit = Tree1_cur->control_bit ^ tcw_r;
            }
        }
    }
    cw_node *cw_last = malloc(sizeof(cw_node));
    if(Tree1_cur->control_bit == 0)
    {
        cw_last->tcw_l =  (2 + (beta - (Tree0_cur->seed->digit[0] & 1) + (Tree1_cur->seed->digit[0] & 1))) % 2;
    }
    else
    {
        cw_last->tcw_l =  (2 + (-1 * ((beta - (Tree0_cur->seed->digit[0] & 1) + (Tree1_cur->seed->digit[0] & 1))))) % 2;
    }
    cw_last->digit = NULL;
    cw_current->next = cw_last;
    cw_current = cw_last; /// 链表结束

    Tree0->seed->next = cw_head; /// 将cw_head 作为链表头
    Tree1->seed->next = cw_head;

    num *k0 = init(), *k1 = init();

    k0 = Tree0->seed;//%d:500611728; %p:00000249E2294570
    k1 = Tree1->seed;//%d:500612976; %p:00000249E2294090


    num **result = (num**)malloc(sizeof(num*) * 2);
    result[0] = k0;
    result[1] = k1; ///将两个key放入数组中作为返回值

    return result;

}
