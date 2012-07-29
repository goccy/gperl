#include <gperl.hpp>

#define MM_POP(list, obj) { \
		obj = list; \
		fprintf(stderr, "list %p, tail %p\n", list, tail);\
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
	fprintf(stderr, "head: %p, tail: %p\n", head, tail);
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

void GPerlMemoryManager::pushStack(GPerlObject* obj) {
	stack[stackSize++] = obj;
	stackHead++;
}

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

void GPerlMemoryManager::gc_mark_root() {
	//stack = traceStack();
	//stack = traceRegister();
	//return traceSize;
	int i = 0;
	GPerlObject* obj = (GPerlObject*)head;
	while (obj < tail) {
		fprintf(stderr, "obj: %p\n", obj);
		pushStack(obj);
	}
}

void GPerlMemoryManager::mark_setStack(GPerlObject* obj) {
	if (!isMarked(obj)) {
		obj->h.mark_flag = 1;
	}
	pushStack(obj);
}

void GPerlMemoryManager::gc_mark() {
	size_t i, j;
	gc_mark_root();
	fprintf(stderr, "max stack size: %lu\n", maxStackSize);
	for(i = 0; i < stackSize - 1; i++) {
		GPerlObject* obj = stack[i];
		fprintf(stderr, "type: %ld\n", obj->h.type);
		traceSize = obj->trace(obj);
		for (j = 0; j < traceSize; j++) {
			mark_setStack(obj);
		}
	}
}

void GPerlMemoryManager::gc_sweep() {
	size_t deadCount = 0;
	GPerlObject* obj = (GPerlObject*)head;
	fprintf(stderr, "start sweep\n");
	while (obj < tail) {
		fprintf(stderr, "obj: %p\n", obj);
		if (!isMarked(obj)) {
			fprintf(stderr, "gc sweep: %p\n", obj);
			_gfree(obj);
			MM_PUSH(freeList, obj);
			deadCount++;
		}
		obj++;
	}
	if (deadCount < maxStackSize * 3 / 4) {
		fprintf(stderr, "Expand Heap!! \n");
		return;
	}
}

void GPerlMemoryManager::gc() {
	fprintf(stderr, "start GC.\n");
	gc_init();
	gc_mark();
	gc_sweep();
	fprintf(stderr, "end GC.\n");
}

void GPerlMemoryManager::exit() {
	free(body);
	free(stack);
}
