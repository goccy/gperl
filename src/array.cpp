#include <gperl.hpp>
using namespace std;

GPerlArray *new_GPerlInitArray(GPerlValue *list, size_t asize)
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

GPerlObject *new_GPerlArray(GPerlVirtualMachineCode *cur_pc, GPerlValue v)
{
	//root.stack_top_idx = cur_pc->cur_stack_top;

	//root.stack_bottom = stack;
	//root.callstack_bottom = callstack_bottom;
	//root.global_memory = global_memory;
	//root.init_variabls = init_variabls;
	GPerlArray *a = (GPerlArray *)getObject(v);
	size_t size = a->size;
	GPerlValue *list = (GPerlValue *)safe_malloc(size * sizeof(GPerlValue));
	for (size_t i = 0; i < size; i++) {
		/* needs deep copy */
		GPerlValue *const_list = a->list;
		switch (TYPE_CHECK(const_list[i])) {
		case 0: case 1: /* Int or Double */
			list[i] = const_list[i];
			break;
		case 2: /* String */ {
			STRING_init(list[i], (GPerlString *)new_GPerlString(cur_pc, const_list[i]));
			break;
		}
		case 3: /* Other Object */
			break;
		default:
			break;
		}
	}
	GPerlObject *freeList = mm->freeList;
	GPerlObject *head = freeList;
	if (freeList->h.next != NULL) {
		mm->freeList = freeList->h.next;
		GPerlArray *ret = (GPerlArray *)head;
		ret->list = list;
		ret->size = size;
		ret->write = Array_write;
		ret->h.type = a->h.type;
		return (GPerlObject *)ret;
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
