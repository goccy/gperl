#include <gperl.hpp>
using namespace std;

#define IS_UNBOX(v) TYPE_CHECK(v) < 2

void Array_push(GPerlValue *argstack)
{
	GPerlArray *a = (GPerlArray *)getObject(argstack[0]);
	void *tmp;
	if (!(tmp = realloc(a->list, sizeof(GPerlValue) * (a->size + 1)))) {
		fprintf(stderr, "ERROR!!: cannot allocated memory\n");
	} else {
		a->list = (GPerlValue *)tmp;
		a->list[a->size] = argstack[1];
		a->size++;
	}
}

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
	a->h.mark_flag = 1;
	size_t size = a->size;
	for (size_t i = 0; i < size; i++) {
		GPerlValue v = a->list[i];
		switch (TYPE_CHECK(v)) {
		case 0: case 1: break;
		case 2:
			((GPerlString *)getStringObj(v))->h.mark_flag = 1;
			break;
		case 3: {
			GPerlObject *o = (GPerlObject *)getObject(v);
			o->mark(o);
			break;
		}
		default:
			break;
		}
	}
}

void Array_free(GPerlObject *o)
{
	DBG_PL("Array_free");
	GPerlArray *a = (GPerlArray *)o;
	GPerlValue *list = a->list;
	safe_free(list, sizeof(GPerlValue) * a->size);
}

GPerlArray *new_GPerlInitArray(GPerlValue *list, size_t asize)
{
	GPerlArray *a = (GPerlArray *)mm->gmalloc();
	a->list = list;
	a->size = asize;
	a->write = Array_write;
	a->mark = Array_mark;
	a->free = Array_free;
	return a;
}

GPerlObject *new_GPerlArray(GPerlValue v)
{
	//double s1 = gettimeofday_sec();
	GPerlArray *a = (GPerlArray *)getObject(v);
	GPerlArray *ret = (GPerlArray *)mm->gmalloc();
	size_t size = sizeof(GPerlValue) * a->size;
	ret->list = (GPerlValue *)safe_malloc(size);
	memcpy(ret->list, a->list, size);
	ret->size = a->size;
	ret->write = Array_write;
	ret->mark = Array_mark;
	ret->free = Array_free;
	ret->h.type = a->h.type;
	//double s2 = gettimeofday_sec();
	//double time = s2 - s1;
	//malloc_time += time;
	return (GPerlObject *)ret;
}
