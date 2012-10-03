#include <gperl.hpp>
#include <sys/time.h>
#include <time.h>
#include <assert.h>

double total_time = 0.0f;
double malloc_time = 0.0f;
double gettimeofday_sec(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec * 1e-6;
}

sigjmp_buf expand_mem;
#define MemoryManager_popObject(o, list) {		\
		o = list;								\
		list = list->h.next;					\
	}

#define MemoryManager_pushObject(o, list) {		\
		memset(o, 0, OBJECT_SIZE);				\
		o->h.next = list;						\
		list = o;								\
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
#ifdef DEBUG_MODE
	memory_leaks += size;
#endif
	return ret;
}

void safe_free(void *ptr, size_t size)
{
	if (ptr) {
		free(ptr);
		ptr = NULL;
#ifdef DEBUG_MODE
		memory_leaks -= size;
#else
		(void)size;
#endif
	}
}

int leaks(void)
{
	return memory_leaks;
}

GPerlMemoryManager::GPerlMemoryManager(void)
{
	pool_size = 0;
	pool_size = 0;
	max_pool_size = INIT_MEMPOOL_NUM;
	pools = (GPerlMemPool **)safe_malloc(max_pool_size * PTR_SIZE);
	GPerlMemPool* pool = (GPerlMemPool*)safe_malloc(sizeof(GPerlMemPool));
	pool->body = (GPerlObject *)safe_malloc(MEMORY_POOL_SIZE);
	pool->size = MEMORY_POOL_SIZE;
	GPerlObject* head = pool->body;
	GPerlObject* tail = (GPerlObject *)((char *)head + MEMORY_POOL_SIZE - OBJECT_SIZE);
	GPerlObject* o = (GPerlObject*)head;
	pool->head = head;
	pool->tail = tail;
	while (o < tail) {
		o->h.next = o + 1;
		o = o + 1;
	}
	guard_prev_ptr = o;
	//o->h.next = guard;
	o->h.next = NULL;
	freeList = head;
	for (int i = 0; i < MAX_GLOBAL_MEMORY_SIZE; i++) {
		global_vmemory[i].ovalue = NULL;
	}
	for (int i = 0; i < MAX_INIT_VALUES_SIZE; i++) {
		init_values[i].ovalue = NULL;
	}
	root.init_values = init_values;
	pools[pool_size] = pool;
	pool_size++;
	_gc = &GPerlMemoryManager::dummyGC;
}

void GPerlMemoryManager::expandMemPool(void)
{
	GPerlMemPool* new_pool = (GPerlMemPool*)safe_malloc(sizeof(GPerlMemPool));
	new_pool->body = (GPerlObject *)safe_malloc(MEMORY_POOL_SIZE);
	new_pool->size = MEMORY_POOL_SIZE;
	GPerlObject* new_head = new_pool->body;
	GPerlObject* new_tail = (GPerlObject *)((char *)new_head + MEMORY_POOL_SIZE - OBJECT_SIZE);
	GPerlObject* o = (GPerlObject*)new_head;
	new_pool->head = new_head;
	new_pool->tail = new_tail;
	while (o < new_tail) {
		o->h.next = o + 1;
		o = o + 1;
	}
	new_tail->h.next = freeList;
	freeList = new_head;
	if (pool_size > max_pool_size - 1) {
		max_pool_size *= 2;
		GPerlMemPool **tmp = (GPerlMemPool **)realloc(pools, max_pool_size * PTR_SIZE);
		if (!tmp) {
			fprintf(stderr, "ERROR!! cannot allocate memory\n");
			exit(EXIT_FAILURE);
		}
		pools = tmp;
	}
	pools[pool_size] = new_pool;
	pool_size++;
	DBG_PL("Expanded Heap size: %lu, max_size: %lu (%lu)kb", pool_size, max_pool_size, pool_size * MEMORY_POOL_SIZE / 1024);
}

#define MARK_SET(o) o->h.mark_flag = 1
#define MARK_RESET(o) o->h.mark_flag = 0
#define IS_Marked(o) o->h.mark_flag

GPerlObject* GPerlMemoryManager::gmalloc(void) {
	GPerlObject *ret = freeList;
	//MemoryManager_popObject(ret, freeList);
	if (ret == NULL) {
		gc();
		if (freeList == NULL) {
			expandMemPool();
		}
		MemoryManager_popObject(ret, freeList);
	} else {
		freeList = freeList->h.next;
	}
	//DBG_PL("malloc");
	MARK_RESET(ret);
	return ret;
}

void GPerlMemoryManager::mark(GPerlValue v) {
	switch (TYPE_CHECK(v)) {
	case 2: {
		//DBG_PL("MARKING");
		GPerlString *s = getStringObj(v);
		s->h.mark_flag = 1;
		break;
	}
	case 3: {
		GPerlObject *o = (GPerlObject *)getObject(v);
		if (!o->h.mark_flag) o->h.mark(o);
		break;
	}
	default:
		break;
	}
}

void GPerlMemoryManager::gc(void)
{
	//double s1 = gettimeofday_sec();
	(this->*_gc)();
	//double s2 = gettimeofday_sec();
	//double time = s2 - s1;
	//total_time += time;
	//fprintf(stderr, "gc_time = [%f]\n", time);
}

/* switch dummy_gc => msgc (before Runtime) */
void GPerlMemoryManager::switchGC(void)
{
	_gc = &GPerlMemoryManager::msgc;
}

void GPerlMemoryManager::dummyGC(void) {
	DBG_PL("GC_START");
	DBG_PL("GC_END");
}

void GPerlMemoryManager::msgc(void) {
	//DBG_PL("GC_START");
	//traceRoot();
	//sweep();
	//DBG_PL("GC_END");
}

void GPerlMemoryManager::traceRoot(void)
{
	GPerlEnv *callstack_bottom = root.callstack_bottom;
	GPerlEnv *callstack_trace_ptr = callstack_bottom;
	GPerlEnv *callstack_top = root.callstack_top;
	for (;callstack_trace_ptr != callstack_top + 1; callstack_trace_ptr++) {
		if (callstack_trace_ptr->cur_pc) {
			int reg_top_idx = callstack_trace_ptr->cur_pc->cur_reg_top;
			for (int j = 0; j < reg_top_idx; j++) {
				mark(callstack_trace_ptr->reg[j]);
			}
		}
		//if (callstack_trace_ptr->pc) {
		int argc = callstack_trace_ptr->pc->argc;
		for (int k = 0; k < argc; k++) {
			mark(callstack_trace_ptr->argstack[k]);
		}
		//}
	}
	GPerlValue *stack_bottom = root.stack_bottom;
	GPerlValue *stack_top = callstack_top->ebp + callstack_top->pc->cur_stack_top + root.stack_top_idx;
	GPerlValue *stack_trace_ptr = stack_bottom;
	for (; stack_trace_ptr != stack_top; stack_trace_ptr++) {
		mark(*stack_trace_ptr);
	}
	GPerlValue *global_vmemory = root.global_vmemory;
	for (int i = 0; i < MAX_GLOBAL_MEMORY_SIZE; i++) {
		mark(global_vmemory[i]);
	}
	GPerlValue *linit_values = init_values;
	for (int i = 0; i < init_value_idx; i++) {
		mark(linit_values[i]);
	}
}

void GPerlMemoryManager::sweep(void) {
#ifdef DEBUG_MODE
	size_t dead_count = 0;
#endif
	for (size_t i = 0; i < pool_size; i++) {
		GPerlMemPool* p = pools[i];
		GPerlObject* o = p->head;
		GPerlObject* tail = p->tail;
		while (o < tail) {
			if (!IS_Marked(o) && o->h.free) {
				o->h.free(o);
				MemoryManager_pushObject(o, freeList);
#ifdef DEBUG_MODE
				dead_count++;
#endif
			}
			o++;
		}
	}
	//DBG_PL("maxcount: %lu", MEMORY_POOL_SIZE / sizeof(GPerlObject) * pool_size);
	//DBG_PL("deadcount: %lu", dead_count);
}
