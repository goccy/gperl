#include <gperl.hpp>

#define MemoryManager_popObject(o, list) {      \
		o = list;                               \
		if (!list->h.next) {                    \
			o = NULL;                           \
		} else {                                \
            list = list->h.next;                \
        }                                       \
	}

#define MemoryManager_pushObject(o, list) {     \
        memset(o, 0, OBJECT_SIZE);              \
		o->h.next = list;                       \
		list = o;                               \
	}

GPerlValue global_vmemory[MAX_GLOBAL_MEMORY_SIZE];
GPerlValue init_values[MAX_INIT_VALUES_SIZE];
GPerlTraceRoot root;
int init_value_idx = 0;
static int memory_leaks = 0;

void *safe_malloc(size_t size)
{
	void *ret = malloc(size);
	if (!ret) {
		fprintf(stderr, "ERROR!!:cannot allocate memory\n");
		exit(EXIT_FAILURE);
	}
	memset(ret, 0, size);
	memory_leaks += size;
	return ret;
}

void safe_free(void *ptr, size_t size)
{
	if (ptr) {
		free(ptr);
		memory_leaks -= size;
		ptr = NULL;
	}
}

int leaks(void)
{
	return memory_leaks;
}

GPerlMemoryManager::GPerlMemoryManager(void)
{
	mem_pool = (GPerlObject *)safe_malloc(PAGE_SIZE);
    mem_pool_size = PAGE_SIZE;
	head = mem_pool;
    tail = (GPerlObject *)((char *)head + PAGE_SIZE - OBJECT_SIZE);
	GPerlObject* o = (GPerlObject*)head;
	while (o < tail) {
		o->h.next = o + 1;
		o = o->h.next;
	}
	o->h.next = NULL;
	freeList = head;
    arena[0] = mem_pool;
	for (int i = 0; i < MAX_GLOBAL_MEMORY_SIZE; i++) {
		global_vmemory[i].ovalue = NULL;
	}
	for (int i = 0; i < MAX_INIT_VALUES_SIZE; i++) {
		init_values[i].ovalue = NULL;
	}
}

bool GPerlMemoryManager::isMarked(GPerlObject* obj) {
	return (obj->h.mark_flag == 1) ? true: false ;
}

void GPerlMemoryManager::expandMemPool(void)
{
    fprintf(stderr, "TODO: expand MemoryPool\n");
    /*
    arena = (GPerlObject **)realloc(arena, PTR_SIZE * max_arena_size);
    if (!arena) {
        fprintf(stderr, "ERROR!!: cannot expand arena\n");
        exit(EXIT_FAILURE);
    }
    DBG_PL("EXPAND MEMORY POOL");
    tail = (GPerlObject *)((char *)head + mem_pool_size - OBJECT_SIZE);
	GPerlObject* o = (GPerlObject*)freeList;
	while (o < tail) {
		o->h.next = o + 1;
		o = o->h.next;
	}
	o->h.next = NULL;
    */
}

GPerlObject* GPerlMemoryManager::gmalloc(void) {
	GPerlObject* ret = NULL;
    DBG_PL("freeList = [%p]", freeList);
	MemoryManager_popObject(ret, freeList);
	if (!ret) {
		gc();
		MemoryManager_popObject(ret, freeList);
        if (!ret) {
            expandMemPool();
            MemoryManager_popObject(ret, freeList);
        }
	}
	return ret;
}

void GPerlMemoryManager::gc_mark(GPerlValue v) {
    switch (TYPE_CHECK(v)) {
    case 2: {
        GPerlString *s = getStringObj(v);
        s->h.mark_flag = 1;
        break;
    }
    case 3: {
        GPerlObject *o = (GPerlObject *)getObject(v);
        o->mark(o);
        break;
    }
    default:
        break;
    }
}

#define MARK_RESET() o->h.mark_flag = 0;

void GPerlMemoryManager::gc_sweep(void) {
	GPerlObject* o = (GPerlObject*)head;
	while (o < tail) {
		if (!isMarked(o) && o->free) {
            o->free(o);
			MemoryManager_pushObject(o, freeList);
            DBG_PL("FREE!!");
		} else {
            MARK_RESET();
        }
		o++;
	}
}

void GPerlMemoryManager::gc(void) {
	DBG_PL("GC_START");
	traceRoot();
	gc_sweep();
    DBG_PL("GC_END");
}

void GPerlMemoryManager::traceRoot(void)
{
	GPerlEnv *callstack_bottom = root.callstack_bottom;
	GPerlEnv *callstack_trace_ptr = callstack_bottom;
	GPerlEnv *callstack_top = root.callstack_top;

	for (;callstack_trace_ptr != callstack_top; callstack_trace_ptr++) {
		int reg_top_idx = callstack_trace_ptr->reg_top;
		for (int i = 0; i < reg_top_idx; i++) {
			gc_mark(callstack_trace_ptr->reg[i]);
		}
	}
	GPerlValue *stack = root.stack_bottom;
	int stack_top = callstack_top->ebp + root.stack_top_idx;
	for (int i = 0; i < stack_top; i++) {
		gc_mark(stack[i]);
	}
	GPerlValue *global_vmemory = root.global_vmemory;
	for (int i = 0; global_vmemory[i].ovalue != NULL; i++) {
		gc_mark(global_vmemory[i]);
	}
	GPerlValue *linit_values = init_values;
	for (int i = 0; i < init_value_idx; i++) {
		gc_mark(linit_values[i]);
	}
}
