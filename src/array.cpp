#include <gperl.hpp>
using namespace std;

GPerlArray *new_GPerlArray(GPerlValue *list, size_t asize)
{
	GPerlObject *freeList = mm->freeList;
	GPerlObject *head = freeList;
	if (freeList->h.next != NULL) {
		mm->freeList = freeList->h.next;
		GPerlArray *a = (GPerlArray *)head;
		a->h.type = Array;
		a->list = list;
		a->size = asize;
		a->write = Array_write;
		return a;
	} else {
		DBG_PL("GC START!!");
	}
	return NULL;
}

void Array_push(GPerlValue *argstack)
{
	GPerlArray *a = (GPerlArray *)getObject(argstack[0]);
	void *tmp;
	if (!(tmp = realloc(a->list, sizeof(GPerlValue) + (a->size + 1)))) {
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
		outbuf += string(shared_buf);
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
