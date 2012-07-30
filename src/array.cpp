#include <gperl.hpp>
using namespace std;

#define IS_UNBOX(v) TYPE_CHECK(v) < 2

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
	for (size_t i = 0; i < size; i++) {
		GPerlValue v = a->list[i];
        switch (TYPE_CHECK(v)) {
        case 0: case 1: break;
        case 2:
            ((GPerlString *)getStringObj(v))->h.mark_flag = 1;
            break;
        case 3: {
            GPerlObject *o = (GPerlObject *)getObject(v);
            o->free(o);
            break;
        }
        default:
            break;
        }
	}
}

void Array_free(GPerlObject *o)
{
    GPerlArray *a = (GPerlArray *)o;
    size_t size = a->size;
    GPerlValue *list = a->list;
    for (size_t i = 0; i < size; i++) {
        switch (TYPE_CHECK(list[i])) {
        case 0: case 1: break;
        case 2: {
            GPerlString *s = getStringObj(list[i]);
            s->free((GPerlObject *)s);
            break;
        }
        case 3:
            GPerlObject *o = (GPerlObject *)getObject(list[i]);
            o->free(o);
            break;
        }
    }
}

GPerlArray *new_GPerlInitArray(GPerlValue *list, size_t asize)
{
	fprintf(stderr, "new Array\n");
	GPerlArray *a = (GPerlArray *)mm->gmalloc();
	a->h.type = Array;
	a->list = list;
	a->size = asize;
	a->write = Array_write;
	a->mark = Array_mark;
	a->free = Array_free;
	return a;
}

GPerlObject *new_GPerlArray(GPerlVirtualMachineCode *, GPerlValue v)
{
	//root.stack_top_idx = cur_pc->cur_stack_top;
	//root.stack_bottom = stack;
	//root.callstack_bottom = callstack_bottom;
	//root.global_memory = global_memory;
	//root.init_variabls = init_variabls;
	GPerlArray *a = (GPerlArray *)getObject(v);
    /*
	//size_t size = a->size;
	GPerlValue *list = (GPerlValue *)safe_malloc(size * sizeof(GPerlValue));
	for (size_t i = 0; i < size; i++) {
    //needs deep copy
		GPerlValue *const_list = a->list;
		switch (TYPE_CHECK(const_list[i])) {
		case 0: case 1: // Int or Double
			list[i] = const_list[i];
			break;
		case 2: {//String
			STRING_init(list[i], (GPerlString *)new_GPerlString(cur_pc, const_list[i]));
			break;
		}
		case 3: // Other Object
			break;
		default:
			break;
		}
	}
	a->list = list;
	a->size = size;
	a->write = Array_write;
	a->trace = Array_trace;
	a->h.type = a->h.type;
   */
    return (GPerlObject *)new_GPerlInitArray(a->list, a->size);
}
