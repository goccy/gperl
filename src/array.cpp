#include <gperl.hpp>
using namespace std;

GPerlArray *new_GPerlArray(GPerlValue *list, size_t asize)
{
	GPerlObject *freeList = mm->freeList;
	GPerlObject *head = freeList;
	if (freeList->h.next != NULL) {
		freeList = freeList->h.next;
		GPerlArray *a = (GPerlArray *)head;
		a->list = list;
		a->size = asize;
		a->write = Array_write;
		return a;
	} else {
		DBG_PL("GROW FREELIST!!");
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
	size_t size = a->size;
	GPerlValue *list = a->list;
	for (size_t i = 0; i < size; i++) {
		GPerlValue v = list[i];
		switch (TYPE_CHECK(v)) {
		case 0: /* Double */
			sprintf(shared_buf, "%f", v.dvalue);
			outbuf += string(shared_buf);
			break;
		case 1: /* Int */
			sprintf(shared_buf, "%d", v.ivalue);
			outbuf += string(shared_buf);
			break;
		case 2: /* String */
			break;
		default: /* Object */
			break;
		}
	}
}
