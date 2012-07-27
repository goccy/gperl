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
	//freeList = (GPerlObject **)malloc(MEMORY_POOL_SIZE);
	freeList = (GPerlObject *)safe_malloc(OBJECT_SIZE);
	GPerlObject *head = freeList;
	for (int i = 0; i < PAGE_SIZE; i++) {
		head->h.next = (GPerlObject *)safe_malloc(OBJECT_SIZE);
		head = head->h.next;
	}
}
