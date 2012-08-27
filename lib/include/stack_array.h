#ifndef ARRAY_H_
#define ARRAY_H_
/*===============================================================
 *                     StackArray.h
 *                    ���ݽṹ-ջ����
 *													��Ҷ-��������
 ===============================================================*/
#include <lib/include/type.h>
#include <lib/include/string.h>



// ջ�������ݽṹ
typedef struct stack_array
{
	u16 max;
	u16 size;
	u16 count;
	u8 *array;
}StackArray;

#define StackArrayDefine(name,datatype,max,type)	\
		type datatype name[max];					\
		type StackArray name##_data


#define StackArrayStruct(name,datatype,max,type)	\
		type datatype name[max] = {0};				\
		type StackArray name##_data = {max,sizeof(datatype),0,(u8*)&name[0]}


/* ��ʼ�� */
static inline StackArray stack_array_init(point array,u16 max,u16 size)
{
	StackArray data;
	data.max = max;
	data.size = size;
	data.count = 0;
	data.array = (u8*)array;
	return data;
}

/* ����� */
static inline int stack_array_add(StackArray *data,...)
{
	int id = data->count;
	u8* p = ((u8*)&data)+sizeof(data);		// �ɱ������ջ�еĵ�ַ
	if (++data->count < data->max)
	{
		memcpy(data->array + id*data->size,p,data->size);
		return id;
	}
	return E_MAX;
}

/* ɾ���� */
static inline point stack_array_delete(StackArray *data)
{
	if (data->count > 0)
	{
		return (point)(data->array + --data->count*data->size);
	}
	else
	{
		return NULL;
	}
}

#endif /* ARRAY_H_ */
