#include <gperl.hpp>

using namespace std;
GPerlCWB* cwb;

GPerlCWB* init_cwb(size_t bufsize)
{
	GPerlCWB* cwb = (GPerlCWB*)safe_malloc(sizeof(GPerlCWB));
	cwb->buf = (char*)safe_malloc(bufsize);
	cwb->bufsize = bufsize;
	return cwb;
}

void free_cwb(GPerlCWB* cwb)
{
	safe_free((void*)cwb->buf, cwb->bufsize);
	safe_free((void*)cwb, sizeof(GPerlCWB));
}

void expand_cwb(size_t size)
{
	cwb->bufsize = size;
	cwb->buf = (char*)realloc(cwb->buf, size);
}

void write_cwb(char *buf)
{
	size_t idx = cwb->idx;
	size_t buf_size = strlen(buf);
	strncpy(cwb->buf + idx, buf, buf_size);
	cwb->idx += buf_size;
	if (idx > cwb->bufsize) {
		DBG_PL("Expand CWB %lu", cwb->bufsize);
		expand_cwb(cwb->bufsize * 2);
	}
}

void clear_cwb(void)
{
	memset(cwb->buf, 0, cwb->idx);
	cwb->idx = 0;
}

GPerlVirtualMachine::GPerlVirtualMachine(vector<GPerlClass *> *pkgs)
{
	vector<GPerlClass *>::iterator it = pkgs->begin();
	this->pkgs = pkgs;
	size_t pkg_n = pkgs->size();
	pkg_table = (GPerlClass **)safe_malloc(PTR_SIZE * HASH_TABLE_SIZE);
	for (size_t i = 0; i < pkg_n; i++) {
		GPerlClass *gclass = (GPerlClass *)(*it);
		char *class_name = (char *)gclass->ext->class_name;
		unsigned long h = hash(class_name, strlen(class_name) + 1) % HASH_TABLE_SIZE;
		DBG_PL("name = [%s], hash = [%lu]", class_name, h);
		pkg_table[h] = gclass;
		it++;
	}
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
		env_[i].argstack_set_ptr = argstack;
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
