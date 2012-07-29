#include <gperl.hpp>

#define MM_POP(list, obj) { \
		obj = list; \
		list = list->h.next; \
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
	GPerlObject* obj = (GPerlObject*)head;
	while (obj < tail) {
		obj->h.next = obj + 1;
		obj = obj->h.next;
	}
	obj->h.next = NULL;
	freeList = (GPerlObject*)head;
	stack = (GPerlObject **)safe_malloc(PAGE_SIZE * sizeof(void*) / sizeof(GPerlObject));
	head = stack;
	stackTail = (void*)((char*)stack + PAGE_SIZE * sizeof(void*) / sizeof(GPerlObject));
	stackSize = 0;
	maxStackSize = PAGE_SIZE / sizeof(GPerlObject);
}

bool GPerlMemoryManager::isMarked(GPerlObject* obj) {
	return (obj->h.mark_flag == 1) ? true: false ;
}

void GPerlMemoryManager::popStack(GPerlObject* obj) {
	stack[stackSize++] = obj;
	stackHead++;
}

void GPerlMemoryManager::pushStack() {
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
}

void GPerlMemoryManager::gc_mark() {
	size_t i, j;
	for(i = 0; i < maxStackSize; i++) {
		GPerlObject* obj = stack[i];
		obj->trace();
		for (j = 0; j < traceSize; j++) {
			if (!isMarked(obj)) {
				obj->h.mark_flag = 1;
			}
		}
	}
}

void GPerlMemoryManager::gc_sweep() {
	GPerlObject* obj = (GPerlObject*)head;
	while(obj < tail) {
		if (!isMarked(obj)) {
			_gfree(obj);
			MM_PUSH(freeList, obj);
		}
		obj++;
	}
}

void GPerlMemoryManager::gc() {
	fprintf(stderr, "start GC.\n");
	gc_init();
	gc_mark();
	gc_sweep();
}

void GPerlMemoryManager::exit() {
	free(body);
	free(stack);
}
