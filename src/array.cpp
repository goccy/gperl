#include <gperl.hpp>
using namespace std;

GPerlArray *new_GPerlArray(GPerlValue *list, size_t asize)
{
	fprintf(stderr, "new Array\n");
	GPerlArray *a = (GPerlArray *)mm->gmalloc(asize);
	a->h.type = Array;
	a->list = list;
	a->size = asize;
	a->write = Array_write;
	a->trace = Array_trace;
	return a;
}

void Array_push(GPerlValue *argstack)
{
	GPerlArray *a = (GPerlArray *)getObject(argstack[0]);
	void *tmp;
	if (!(tmp = mm->grealloc(a->list, sizeof(GPerlValue) + (a->size + 1)))) {
		fprintf(stderr, "ERROR!!: cannot allocated memory\n");
	} else {
		a->list = (GPerlValue *)tmp;
		a->list[a->size] = argstack[1];
		a->size++;
	}
}

#define MM_POP(list, obj) { \
		obj = list; \
		list = list->h.next; \
	}

#define MM_PUSH(list, obj) { \
		_gfree(obj); \
		obj->h.next = list; \
		list = obj; \
	}

#define IS_UNBOX(obj) ((obj)->h.type < 2)

size_t Array_trace(GPerlObject* o)
{
	fprintf(stderr, "Array trace...\n");
	GPerlArray *a = (GPerlArray *)o;
	size_t size = a->size;
	for (int i = 0; i < size; i++) {
		GPerlValue v = a->list[i];
		//if (IS_UNBOX(&v)) continue;
		//mm->pushStack((GPerlObject*)a->list[i]);
		//mm->pushStack((GPerlObject*)v);
	}
	return size;
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
				write_cwb(getString(v));
				break;
			}
			default: /* Object */
				break;
			}
		}
	}
}
