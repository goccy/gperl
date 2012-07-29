#include <gperl.hpp>

#define MM_POP(list, obj) { \
		obj = list; \
		list = list->h.next;\
		if (list == NULL) {\
			obj = NULL;\
		}\
	}

#define MM_PUSH(list, obj) { \
		_gfree(obj); \
		obj->h.next = list; \
		list = obj; \
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
	body = (GPerlObject *)safe_malloc(PAGE_SIZE);
	head = (void*)body;
	tail = (void*)((char*)body + PAGE_SIZE);
	GPerlObject* obj = (GPerlObject*)head;
	while (obj < tail) {
		obj->h.next = obj + 1;
		obj = obj->h.next;
	}
	obj->h.next = NULL;
	freeList = (GPerlObject*)head;
	stack = (GPerlObject **)safe_malloc(PAGE_SIZE * sizeof(void*) / sizeof(GPerlObject));
	stackHead = stack[0];
	stackTail = (void*)((char*)stack + PAGE_SIZE * sizeof(void*) / sizeof(GPerlObject));
	stackSize = 0;
	maxStackSize = PAGE_SIZE / sizeof(GPerlObject);
}

bool GPerlMemoryManager::isMarked(GPerlObject* obj) {
	return (obj->h.mark_flag == 1) ? true: false ;
}

void GPerlMemoryManager::popStack() {
}

//void GPerlMemoryManager::pushStack(GPerlValue obj) {
//	stack[stackSize++] = obj;
//	stackHead++;
//}

void GPerlMemoryManager::expandStack() {
	stack = (GPerlObject**)realloc((void*)stack, stackSize * 2);
	maxStackSize *= 2;
}

GPerlObject* GPerlMemoryManager::gmalloc(size_t size) {
	(void)size;
	GPerlObject* ret;
	MM_POP(freeList, ret);
	if (ret == NULL) {
		gc();
		MM_POP(freeList, ret);
	}
	return ret;
}

GPerlObject* GPerlMemoryManager::grealloc(void* obj, size_t size) {
	//(void)size;
	//GPerlObject* ret;
	//MM_POP(freeList, ret);
	//if (ret == NULL) {
	//	gc();
	//	MM_POP(freeList, ret);
	//}
	//return ret;
	return (GPerlObject*)realloc(obj, size);
}

// TODO set Private
void GPerlMemoryManager::_gfree(GPerlObject* obj) {
	memset(obj, '\0', OBJECT_SIZE);
}

void GPerlMemoryManager::gc_init() {
}

//void GPerlMemoryManager::mark_setStack(GPerlObject* obj) {
//	if (!isMarked(obj)) {
//		obj->h.mark_flag = 1;
//	}
//	//pushStack(obj);
//}

void GPerlMemoryManager::gc_mark(GPerlValue v) {
	size_t i;
	GPerlObject* obj = (GPerlObject*)getStringObj(v);
	traceSize = obj->trace(obj);
	for (i = 0; i < traceSize; i++) {
		if (!isMarked(obj)) {
			obj->h.mark_flag = 1;
		}
		//gc_mark(v);
	}
}

void GPerlMemoryManager::gc_sweep() {
	size_t deadCount = 0;
	GPerlObject* obj = (GPerlObject*)head;
	//fprintf(stderr, "start sweep\n");
	while (obj < tail) {
		//fprintf(stderr, "obj: %p\n", obj);
		if (!isMarked(obj)) {
			//fprintf(stderr, "gc sweep: %p\n", obj);
			MM_PUSH(freeList, obj);
			deadCount++;
		}
		obj++;
	}
	//fprintf(stderr, "sweeped: %lu\n", deadCount);
	if (deadCount < maxStackSize * 3 / 4) {
		//fprintf(stderr, "Expand Heap!! \n");
		return;
	}
}

void GPerlMemoryManager::gc() {
	//fprintf(stderr, "start GC.\n");
	gc_init();
	traceRoot();
	gc_sweep();
	//fprintf(stderr, "end GC.\n");
}

void GPerlMemoryManager::exit() {
	free(body);
	free(stack);
	for (int i = 0; i < MAX_GLOBAL_MEMORY_SIZE; i++) {
		global_vmemory[i].ovalue = NULL;
	}
	for (int i = 0; i < MAX_INIT_VALUES_SIZE; i++) {
		init_values[i].ovalue = NULL;
	}
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
