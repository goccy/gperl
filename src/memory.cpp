#include <gperl.hpp>

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
	//fprintf(stderr, "head=%p, tail=%p, (%d)\n", head, tail, PAGE_SIZE);
	GPerlObject* obj = (GPerlObject*)head;
	while (obj < tail) {
		obj->h.next = obj + 1;
		obj = obj->h.next;
	}
	obj->h.next = NULL;
}

GPerlObject* GPerlMemoryManager::gmalloc(size_t size) {
	(void)size;
	GPerlObject* ret = _pop();
	if (ret == NULL) {
		gc();
		ret = _pop();
	}
	return ret;
}

void GPerlMemoryManager::_gfree(GPerlObject* obj) {
	memset(obj, '\0', OBJECT_SIZE);
}

/* freeList stacks all object */
GPerlObject* GPerlMemoryManager::_pop() {
	GPerlObject* obj = freeList;
	return obj;
}

void GPerlMemoryManager::_push(GPerlObject* obj) {
	_gfree(obj);
	obj->h.next = freeList;
	freeList = obj;
}

void GPerlMemoryManager::_gc_init() {
}

void GPerlMemoryManager::_gc_mark_root() {
}

void GPerlMemoryManager::_gc_mark() {
}

void GPerlMemoryManager::_gc_sweep() {
}

void GPerlMemoryManager::gc() {
	_gc_init();
	_gc_mark();
	_gc_sweep();
}

void GPerlMemoryManager::exit() {
	free(body);
}
