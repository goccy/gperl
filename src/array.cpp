#include <gperl.hpp>

void Array_push(GPerlValue *argstack)
{
	fprintf(stderr, "Array_push called\n");
	asm("int3");
}
