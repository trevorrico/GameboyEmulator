#ifndef BITWISE_UTILS_H_
#define BITWISE_UTILS_H_

#define GET_BIT(x, bit) ((x & (1 << bit)) >> bit)
#define SET_BIT(x, bit) x |= (1 << bit) 
#define CLEAR_BIT(x, bit) x &= (~(1 << bit))

#endif