#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>

#define MAX_REG_SIZE 32
typedef union {
	uint64_t bytes[MAX_REG_SIZE];
	int ivalue[MAX_REG_SIZE];
	double dvalue[MAX_REG_SIZE];
	char *svalue[MAX_REG_SIZE];
	void *ovalue[MAX_REG_SIZE];
} Reg;

typedef union _Value {
	uint64_t bytes;
	double d;
	int i;
	void *o;
} Value;

#define NaN       (uint64_t)(0xFFF0000000000000)
#define Mask      (uint64_t)(0x00000000FFFFFFFF)
#define TYPE      (uint64_t)(0x000F000000000000) // for typecheck

#define IntTag    (uint64_t)(0x0001000000000000) //Type int
#define ObjectTag (uint64_t)(0x0002000000000000) //Type Object

#define Int_init(b) ((b & Mask) | NaN | IntTag)
#define Double_init(b) (b)
#define Object_init(o, ptr) o.o = ptr; o.bytes |= NaN | ObjectTag
#define get_Object(b) (b.bytes ^ (NaN | ObjectTag))
#define type_check(T) ((T & NaN) == NaN) * ((T & TYPE) >> 48 )

void main()
{
	int i = 0;
	void *ptr = malloc(100);
	//fprintf(stderr, "%p\n", ptr);
	//Value v = Int_init(54);
	Reg reg;
	reg.ivalue[0] = Int_init(54);
	//Value d = (Value)Double_init(54.234);
	//Value o;
	//Object_init(o, ptr);
	fprintf(stderr, "%d\n", type_check(reg.bytes[0]));
	//fprintf(stderr, "%d\n", type_check(d));
	//fprintf(stderr, "%d\n", type_check(o));
	//fprintf(stderr, "%d\n", v.bytes);
	//fprintf(stderr, "%p\n", get_Object(o));
	//Value v2;
	//v2.bytes = v.bytes + 12;
	//v2.bytes = Int_init(v.i + 12);
	//fprintf(stderr, "%d\n", v2.bytes);
	//fprintf(stderr, "%d\n", type_check(v2));
	double dvalue = 1.234;
	int ivalue = 2;
	//void **adr = jmp_table[OPADD + type_check(v1) + type_check(v2)];
	//pc->next = adr;//override next_ptr => adr //STATIC_TYPE_MODE
	//goto *adr;//non override next_ptr //DYNAMIC_TYPE_MODE
	for (i = 0; i < 1000000000; i++) {
		//dvalue += 1.235;
		//ivalue += 3;
		//type_check(v);
		//Value v = Int_init(54);
		//int v = 54;
		//v++;
		//v.i++;
	}
}
