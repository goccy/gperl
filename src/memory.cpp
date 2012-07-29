#include <gperl.hpp>

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
	//freeList = (GPerlObject **)malloc(MEMORY_POOL_SIZE);
	freeList = (GPerlObject *)safe_malloc(OBJECT_SIZE);
	GPerlObject *head = freeList;
	for (int i = 0; i < PAGE_SIZE; i++) {
		head->h.next = (GPerlObject *)safe_malloc(OBJECT_SIZE);
		head = head->h.next;
	}
	for (int i = 0; i < MAX_GLOBAL_MEMORY_SIZE; i++) {
		global_vmemory[i].ovalue = NULL;
	}
	for (int i = 0; i < MAX_INIT_VALUES_SIZE; i++) {
		init_values[i].ovalue = NULL;
	}
}

void GPerlMemoryManager::traceRoot(void)
{
	GPerlTraceRoot root;
	GPerlEnv *callstack_bottom = root.callstack_bottom;
	GPerlEnv *callstack_trace_ptr = callstack_bottom;
	GPerlEnv *callstack_top = root.callstack_top;
	for (;callstack_trace_ptr != callstack_top; callstack_trace_ptr++) {
		int reg_top_idx = callstack_trace_ptr->reg_top;
		for (int i = 0; i < reg_top_idx; i++) {
			//mark(callstack_trace_ptr->reg[i]);
		}
	}
	GPerlValue *stack = root.stack_bottom;
	int stack_top = callstack_top->ebp + root.stack_top_idx;
	for (int i = 0; i < stack_top; i++) {
		//mark(stack[i]);
	}
	GPerlValue *global_vmemory = root.global_vmemory;
	for (int i = 0; global_vmemory[i].ovalue != NULL; i++) {
		//mark(global_vmemory[i]);
	}
	GPerlValue *init_values = root.init_values;
	for (int i = 0; global_vmemory[i].ovalue != NULL; i++) {
		//mark(init_values[i]);
	}
}
