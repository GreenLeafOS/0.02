/*
 * hash_table.h
 *
 *  Created on: 2012-8-22
 *      Author: й╔ят
 */

#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_

#include "type.h"

#define VALUE_MAXSIZE 160

typedef u32 (*hash_f)(u32, u32);       //hash function pointer


typedef struct {
    char* ptr;
    hash_f hash_fun;
    size_t nbyte;
    size_t value_size;
    size_t total;
    size_t pair_size;
    size_t max_try;
} hash_info;



extern int hash_create(hash_info* info, char* ptr, size_t nbyte, size_t size, hash_f hash_fun);
extern int hash_set(hash_info* info, u32 key, void* value);
extern int hash_get(hash_info* info, u32 key, void* value);
extern void* hash_getaddr(hash_info* info, u32 key);
extern int hash_del(hash_info* info, u32 key);


#endif /* HASH_TABLE_H_ */
