#include <gperl.hpp>

using namespace std;
char *cwb;
int cwb_idx = 0;
void write_cwb(char *buf)
{
	size_t buf_size = strlen(buf);
	strncpy(cwb + cwb_idx, buf, buf_size);
	cwb_idx += buf_size;
	if (cwb_idx > MAX_CWB_SIZE) {
		fprintf(stderr, "ERROR: cwb_idx = [%d] > %d\n", cwb_idx, MAX_CWB_SIZE);
		memset(cwb, 0, MAX_CWB_SIZE);
		cwb_idx = 0;
	}
}

void clear_cwb(void)
{
	memset(cwb, 0, cwb_idx);
	cwb_idx = 0;
}

GPerlVirtualMachine::GPerlVirtualMachine(void)
{
}

void GPerlVirtualMachine::setToFuncMemory(GPerlVirtualMachineCode *func, int idx)
{
	func_memory[idx] = func;
}

inline GPerlVirtualMachineCode *GPerlVirtualMachine::getFromFuncMemory(int idx)
{
	return func_memory[idx];
}

void GPerlVirtualMachine::createDirectThreadingCode(GPerlVirtualMachineCode *codes, void **jmp_tbl)
{
	if (codes->op == THCODE) codes->op = NOP;
	GPerlVirtualMachineCode *pc = codes;
	for (; pc->op != UNDEF; pc++) {
		if (pc->op == FUNCSET) {
			createDirectThreadingCode(pc->func, jmp_tbl);
		}
		pc->opnext = jmp_tbl[pc->op];
	}
	pc->opnext = jmp_tbl[pc->op];
}

/*
static void *gxsafe_malloc(void)
{
	int pagesize = sysconf(_SC_PAGE_SIZE);
	DBG_PL("pagesize = [%d]", pagesize);
	if (pagesize < 0) perror("sysconf");
	void *_codeptr = (void *)safe_malloc(pagesize * 10L);
	void *codeptr = (void *)((long)_codeptr & ~(pagesize - 1L));
	memset(codeptr, 0xc3, pagesize * 10L);
	if (mprotect(codeptr, pagesize * 10L, PROT_WRITE|PROT_READ|PROT_EXEC) < 0) perror("mprotect");
	return codeptr;
}
*/

void GPerlVirtualMachine::createSelectiveInliningCode(GPerlVirtualMachineCode *c, void **jmp_tbl, InstBlock *block_tbl)
{
	(void)c; (void)jmp_tbl; (void)block_tbl;
//	GPerlVirtualMachineCode *pc = c;
//	pc++;//skip THCODE
//	for (; pc->op != UNDEF; pc++) {
//		InstBlock block = block_tbl[pc->op];
//		int len = (intptr_t)block.end - (intptr_t)block.start;
//		DBG_PL("len = [%d]", len);
//		if (pc->op == FUNCSET) {
//			createSelectiveInliningCode(pc->func, jmp_tbl, block_tbl);
//		}
//	}
}

GPerlArgsArray *args;
static GPerlEnv env_[MAX_CALLSTACK_SIZE];
GPerlEnv *GPerlVirtualMachine::createCallStack(void)
{
	args = (GPerlArgsArray *)safe_malloc(sizeof(GPerlArgsArray));
	memset(args, 0, sizeof(GPerlArgsArray));
	args->write = Array_write;
	for (int i = 0; i < MAX_CALLSTACK_SIZE; i++) {
		GPerlValue *reg = (GPerlValue *)safe_malloc(sizeof(GPerlValue) * MAX_REG_SIZE);
		GPerlValue *argstack = (GPerlValue *)safe_malloc(sizeof(GPerlValue) * MAX_ARGSTACK_SIZE);
		env_[i].argstack = argstack;
		env_[i].reg = reg;
	}
	int size = MAX_CALLSTACK_SIZE * sizeof(GPerlEnv);
	GPerlEnv *callstack = (GPerlEnv *)safe_malloc(size);
	memcpy(callstack, env_, size);
	return callstack;
}

GPerlValue *GPerlVirtualMachine::createMachineStack(void)
{
	return (GPerlValue *)safe_malloc(MAX_MACHINE_STACK_SIZE * sizeof(GPerlValue));
}

static GPerlObject **argstack_[MAX_CALLSTACK_SIZE];
GPerlObject **GPerlVirtualMachine::createArgStack(void)
{
	for (int i = 0; i < MAX_CALLSTACK_SIZE; i++) {
		argstack_[i] = (GPerlObject **)safe_malloc(sizeof(GPerlObject) * MAX_ARGSTACK_SIZE);
		memset(argstack_[i], 0, sizeof(GPerlObject) * MAX_ARGSTACK_SIZE);
	}
	return (GPerlObject **)argstack_;
}
