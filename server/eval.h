#include "gen.h"

#ifndef _eval_h
#define _eval_h

/**
 * @brief 
 * 
 * @param k 
 * @param b 
 * @param x 
 * @return int 
 */
int Eval(int b, num **Kb, num *x);

/**
 * @brief 
 * 
 * @param x 
 * @param y 
 */
num* combine(cw_node *y);

/**
 * @brief 
 * 
 * @param x 
 * @param y 
 * @return num* 
 */
num* T_xor(num *x, num *y);
#endif /* _EVAL_ */