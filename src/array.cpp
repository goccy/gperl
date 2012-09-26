#include <gperl.hpp>
using namespace std;

#define IS_UNBOX(v) TYPE_CHECK(v) < 2

void Array_write(GPerlValue o)
{
	GPerlArray *a = (GPerlArray *)getObject(o);
	if (a->h.type == ArrayRef) {
		char buf[32] = {0};
		sprintf(buf, "ARRAY(%p)", a->list);
		write_cwb(buf);
	} else {
		size_t size = a->size;
		GPerlValue *list = a->list;
		for (size_t i = 0; i < size; i++) {
			GPerlValue v = list[i];
			char buf[32] = {0};
			switch (TYPE_CHECK(v)) {
			case 0: /* Double */
				sprintf(buf, "%f", v.dvalue);
				write_cwb(buf);
				break;
			case 1: /* Int */ {
				sprintf(buf, "%d", v.ivalue);
				write_cwb(buf);
				break;
			}
			case 2: /* String */ {
				write_cwb(getRawString(v));
				break;
			}
			default: /* Object */
				break;
			}
		}
	}
}

void Array_mark(GPerlObject* o)
{
	GPerlArray *a = (GPerlArray *)o;
	size_t size = a->size;
	a->h.mark_flag = 1;
	//DBG_PL("MARKING");
	for (size_t i = 0; i < size; i++) {
		GPerlValue v = a->list[i];
		switch (TYPE_CHECK(v)) {
		case 2:
			//DBG_PL("MARKING");
			((GPerlString *)getStringObj(v))->h.mark_flag = 1;
			break;
		case 3: {
			GPerlObject *o = (GPerlObject *)getObject(v);
			if (!o->h.mark_flag) o->h.mark(o);
			break;
		}
		default:
			break;
		}
	}
}

#define MemoryManager_pushObject(o, list) {		\
		memset(o, 0, OBJECT_SIZE);				\
		o->h.next = list;						\
		list = o;								\
	}

void Array_free(GPerlObject *o)
{
	//DBG_PL("Array_free");
	GPerlArray *a = (GPerlArray *)o;
	size_t size = a->size;
	for (size_t i = 0; i < size; i++) {
		GPerlValue v = a->list[i];
		switch (TYPE_CHECK(v)) {
		case 2: {
			GPerlString* str = ((GPerlString *)getStringObj(v));
			if (str->h.mark_flag == 0) {
				str->h.free((GPerlObject*)str);
				MemoryManager_pushObject(o, mm->freeList);
			}
			break;
		}
		case 3: {
			GPerlObject *o = (GPerlObject *)getObject(v);
			if (!o->h.mark_flag) {
				MemoryManager_pushObject(o, mm->freeList);
			}
			break;
		}
		default:
			break;
		}
	}
	if (a->size > 0) {
		safe_free(a->list, sizeof(GPerlValue) * (a->size + 1));
	}
	a->size = 0;
}

void Array_push(GPerlValue *argstack)
{
	GPerlArray *a = (GPerlArray *)getObject(argstack[0]);
	if (a->h.type == Undefined) {
		a->h.type = Array;
		a->write = Array_write;
		a->h.mark = Array_mark;
		a->h.free = Array_free;
	}
	void *tmp;
	if (!(tmp = realloc(a->list, sizeof(GPerlValue) * (a->size + 1)))) {
		fprintf(stderr, "ERROR!!: cannot allocated memory\n");
	} else {
		a->list = (GPerlValue *)tmp;
		a->list[a->size] = argstack[1];
		a->size++;
	}
}

GPerlArray *new_GPerlInitArray(GPerlValue *list, size_t asize)
{
	GPerlArray *a = (GPerlArray *)mm->gmalloc();
	a->h.type = Array;
	a->list = list;
	a->size = asize;
	a->write = Array_write;
	a->h.mark = Array_mark;
	a->h.free = Array_free;
	return a;
}

GPerlObject *new_GPerlArray(GPerlValue v, GPerlValue *args)
{
	GPerlArray *a = (GPerlArray *)getObject(v);
	GPerlArray *ret = (GPerlArray *)mm->gmalloc();
	size_t size = sizeof(GPerlValue) * (a->size + 1);
	ret->list = (GPerlValue *)safe_malloc(size);
	memcpy(ret->list, args, size);
	ret->size = a->size;
	ret->write = Array_write;
	ret->h.mark = Array_mark;
	ret->h.free = Array_free;
	ret->h.type = a->h.type;
	return (GPerlObject *)ret;
}
