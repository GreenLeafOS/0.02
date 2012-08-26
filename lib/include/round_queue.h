/*
 * round_queue.h
 *
 *  Created on: 2012-8-10
 *      Author: greenleaf
 */

#ifndef CIRCULAR_QUEUE_H_
#define CIRCULAR_QUEUE_H_

#include <lib/include/type.h>
#include <lib/include/string.h>

#define Max data->max
#define Size data->size
#define Array data->array
#define Head data->head
#define Tail data->tail


#define RoundQueueDefine(name,datatype,max,type)	\
		type datatype name[max];					\
		type RoundQueue name##_data


#define RoundQueueStruct(name,datatype,max,type)	\
		type datatype name[max] = {0};				\
		type RoundQueue name##_data = {max,sizeof(datatype),(u8*)&name[0],0,0}


typedef struct round_queue
{
	u16 max;
	u16 size;
	u8 *array;
	u16 head;
	u16 tail;
}RoundQueue;

static inline RoundQueue round_queue_init(u16 max,u16 size,point array)
{
	RoundQueue data;
	data.max = max;
	data.size = size;
	data.array = (u8*)array;
	data.head = 0;
	data.tail = 0;
	return data;
}

static inline Result round_queue_add(RoundQueue *data,...)
{
	u8* p = ((u8*)&data)+sizeof(data);
	if ((Tail+1)%Max == Head) return E_MAX;
	Tail = (Tail + 1) % Max;
	memcpy(Array + Tail*Size,p,Size);
}

static inline point round_queue_delete(RoundQueue *data)
{
	if (Head == Tail) return NULL;
	Head = (Head + 1) % Max;
	return (Array + Head*Size);

}
#endif /* CIRCULAR_QUEUE_H_ */
