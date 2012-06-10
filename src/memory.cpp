#include <gperl.hpp>

GPerlMemoryManager::GPerlMemoryManager(void)
{
	//freeList = (GPerlObject **)malloc(MEMORY_POOL_SIZE);
	freeList = (GPerlObject *)malloc(OBJECT_SIZE);
	GPerlObject *head = freeList;
	for (int i = 0; i < PAGE_SIZE; i++) {
		head->h.next = (GPerlObject *)malloc(OBJECT_SIZE);
		head = head->h.next;
	}
}
