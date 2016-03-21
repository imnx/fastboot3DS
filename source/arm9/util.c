#include "types.h"

u32 getleu32(const void* ptr)
{
	u8* p = (u8*)ptr;
	
	return (p[0]<<0) | (p[1]<<8) | (p[2]<<16) | (p[3]<<24);
}

u32 swap32(u32 num)
{
	return (num>>24) |((num>>8)&0xFF00) |
			((num<<8)&0xFF0000) |
			(num<<24);
}