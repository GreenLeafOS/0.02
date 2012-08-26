/*===============================================================
 *                       desc
 *                	 段描述符相关操作函数
 ===============================================================*/
#include "include/desc.h"


void desc_init(Desc* p_desc,u32 base,u32 limit,u16 attribute)
{
	p_desc->base_low = base & 0xFFFF;
	p_desc->base_mid = (base >> 16) & 0xFF;
	p_desc->base_high = (base >> 24) & 0xFF;
	p_desc->limit_low = limit & 0xFFFF;
	p_desc->limit_high = (limit >> 16);
	*(((u8*)p_desc)+5) = (u8)attribute;
	*(((u8*)p_desc)+6) |= (u8)(attribute >> 8);
	return;
}

Desc desc_create(u32 base,u32 limit, u16 attribute)
{
	Desc new_desc;
	desc_init(&new_desc,base,limit,attribute);
	return new_desc;
}

/*===============================================================
 *                        gate
 *                	  门描述符相关处理函数
 ===============================================================*/
void gate_init(Gate* p_gate,u32 offset,u32 selector,u8 attr,u8 param_count)
{
	p_gate->selector = selector;
	p_gate->offset_low = (u16)offset;
	p_gate->offset_high = (u16)(offset >> 16);
	p_gate->param_count = param_count;
	*(((u8*)p_gate)+5) = (u8)attr;
	return;
}

Gate gate_create(u32 offset,u32 selector,u8 attr,u8 param_count)
{
	Gate new_gate;
	gate_init(&new_gate,offset,selector,attr,param_count);
	return new_gate;
}
