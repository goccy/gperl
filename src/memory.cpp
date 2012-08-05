#include <gperl.hpp>

sigjmp_buf expand_mem;
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

static void segv_handler(int , siginfo_t *, void *)
{
	exit(EXIT_FAILURE);
	//siglongjmp(expand_mem, 1);
}

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
	pool_size = 0;
	max_pool_size = INIT_MEMPOOL_NUM;
	pools = (GPerlMemPool **)safe_malloc(max_pool_size * PTR_SIZE);
	GPerlMemPool* pool = (GPerlMemPool*)safe_malloc(sizeof(GPerlMemPool));
	pool->body = (GPerlObject *)safe_malloc(PAGE_SIZE);
	pool->size = PAGE_SIZE;
	GPerlObject* head = pool->body;
	GPerlObject* tail = (GPerlObject *)((char *)head + PAGE_SIZE - OBJECT_SIZE);
	GPerlObject* o = (GPerlObject*)head;
	pool->head = head;
	pool->tail = tail;
	while (o < tail) {
		o->h.next = o + 1;
		o = o + 1;
	}
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
	gc = &GPerlMemoryManager::dummy_gc;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = segv_handler;
    if (sigaction(SIGBUS, &sa, NULL) == -1) {
		fprintf(stderr, "ERROR!!: sigaction\n");
		exit(EXIT_FAILURE);
	}
}

bool GPerlMemoryManager::isMarked(GPerlObject* obj) {
	return (obj->h.mark_flag == 1) ? true: false ;
}

void GPerlMemoryManager::expandMemPool(void)
{
	DBG_PL("Expand Heap");
	GPerlMemPool* new_pool = (GPerlMemPool*)safe_malloc(sizeof(GPerlMemPool));
	new_pool->body = (GPerlObject *)safe_malloc(PAGE_SIZE);
	new_pool->size = PAGE_SIZE;
	GPerlObject* new_head = new_pool->body;
	GPerlObject* new_tail = (GPerlObject *)((char *)new_head + PAGE_SIZE - OBJECT_SIZE);
	GPerlObject* o = (GPerlObject*)new_head;
	new_pool->head = new_head;
	new_pool->tail = new_tail;
	while (o < new_tail) {
		o->h.next = o + 1;
		o = o + 1;
	}
	o->h.next = NULL;
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
    //sa.sa_flags = SA_SIGINFO;
    //sigemptyset(&sa.sa_mask);
    //sa.sa_sigaction = segv_handler;
    //if (sigaction(SIGBUS, &sa, NULL) == -1) {
	//fprintf(stderr, "ERROR!!: sigaction\n");
	//exit(EXIT_FAILURE);
	//}
}

GPerlObject* GPerlMemoryManager::gmalloc(void) {
	GPerlObject* ret = NULL;
	DBG_PL("freeList = [%p]", freeList);
	MemoryManager_popObject(ret, freeList);
	if (!ret) {
		(mm->*gc)();
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
	size_t i, dead = 0;
	for (i = 0; i < pool_size; i++) {
		GPerlMemPool* p = pools[i];
		GPerlObject* o = p->head;
		GPerlObject* tail = p->tail;
		while (o < tail) {
			if (!isMarked(o) && o->free) {
				o->free(o);
				MemoryManager_pushObject(o, freeList);
				DBG_PL("FREE!!");
				dead++;
			} else {
				MARK_RESET();
			}
			o++;
		}
	}
	DBG_PL("maxcount: %lu", PAGE_SIZE / sizeof(GPerlObject) * pool_size);
	DBG_PL("deadcount: %lu", dead);
}

void GPerlMemoryManager::dummy_gc(void) {
	DBG_PL("GC_START");
	DBG_PL("GC_END");
}

void GPerlMemoryManager::msgc(void) {
	DBG_PL("GC_START");
	traceRoot();
	gc_sweep();
	DBG_PL("GC_END");
}

void GPerlMemoryManager::traceRoot(void)
{
	DBG_PL("traceRoot");
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
