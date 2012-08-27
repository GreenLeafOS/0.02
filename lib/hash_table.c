/*
 * hash_table.c
 *
 *  Created on: 2012-8-22
 *      Author: й╔ят
 */


#include "include/hash_table.h"
#include "include/string.h"


static u32 hash_default_fun(u32 max, u32 key)
{
    return key % max;
}

static char* hash_address(hash_info* info, u32 key)
{
    key = info->hash_fun(info->total, key);
    return info->ptr + key * info->pair_size;
}

static char* hash_next(hash_info* info, char* addr)
{
    u32 index = (addr - info->ptr) / info->pair_size;
    if(++index >= info->total)
        return info->ptr;
    else
        return info->ptr + index * info->pair_size;
}

static u32 hash_key(char* addr)
{
    u32 key;
    memcpy(&key, addr, sizeof(u32));
    return key;
}

static char* hash_find_addr(hash_info* info, u32 key)
{
    char* addr = hash_address(info, key);
    u32 zero = 0;
    u32 times = 0;
    while(memcmp(addr, &zero, sizeof(u32)) && memcmp(addr, &key, sizeof(u32)) && times++ < info->max_try)
    {
        addr = hash_next(info, addr);
    }
    if(times == info->max_try)
        return NULL;
    return addr;
}

static void hash_clear(hash_info* info, char* addr)
{
    memset(addr, 0, info->pair_size);
}

static int hash_sort(hash_info* info, char* addr)
{
    for(int i = 0 ; i < info->max_try; i++)
    {
        addr = hash_next(info, addr);
        u32 key = hash_key(addr);
        char* addr2 = hash_find_addr(info, key);
        if(addr == addr2)
            continue;
        memcpy(addr2, addr, info->pair_size);
        hash_clear(info,addr);
        return hash_sort(info, addr);
    }
    return 0;
}

int hash_create(hash_info* info, char* ptr, size_t nbyte, size_t size, hash_f hash_fun)
{
    if(info == NULL || ptr == NULL)
        return -1;
    if(size == 0)
        return -1;
    if(hash_fun == NULL)
        info->hash_fun = hash_default_fun;
    memset(ptr, 0, nbyte);
    info->ptr = ptr;
    info->nbyte = nbyte;
    info->value_size = size;
    info->pair_size = sizeof(u32) + size;
    info->total = nbyte / info->pair_size;

    info->max_try = (info->total > 16) ? 16 : info->total;
    return 0;
}



int hash_set(hash_info* info, u32 key, void* value)
{
    char* addr = hash_find_addr(info, key);
    if(addr == NULL)
        return -1;
    memcpy(addr, &key, sizeof(u32));
    memcpy(addr + sizeof(u32), value, info->value_size);
    return 0;
}

int hash_get(hash_info* info, u32 key, void* value)
{
    char* addr = hash_find_addr(info, key);
    if(addr == NULL)
        return -1;
    u32 val = 0;
    memcpy(&val, addr, sizeof(u32));
    if(val == 0)
        return -1;
    memcpy(value, addr + sizeof(u32), info->value_size);
    return 0;
}

int hash_del(hash_info* info, u32 key)
{
    char* addr = hash_find_addr(info, key);
    if(addr == NULL)
        return -1;
    u32 val = 0;
    memcpy(&val, addr, sizeof(u32));
    if(val == 0)
        return -1;
    hash_clear(info, addr);
    return hash_sort(info, addr);
}


void* hash_getaddr(hash_info* info, u32 key)
{
	char* addr = hash_find_addr(info, key);
	if(addr == NULL || key == 0)
		return 	NULL;
	u32 val = 0;
	memcpy(&val, addr, sizeof(u32));
	if(val == 0)
		return NULL;

	return (addr + sizeof(u32));
}
