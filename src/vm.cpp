#include <gperl.hpp>

using namespace std;

GPerlVirtualMachine::GPerlVirtualMachine(void)
{
}

void GPerlVirtualMachine::setToVariableMemory(const char *name, int idx)
{
	GPerlObject *o = new GPerlObject();//TODO fetch from memory manage unit
	o->name = name;
	variable_memory[idx] = o;
}

void GPerlVirtualMachine::setToFuncMemory(GPerlVirtualMachineCode *func, int idx)
{
	func_memory[idx] = func;
}

inline GPerlObject *GPerlVirtualMachine::getFromVariableMemory(int idx)
{
	return variable_memory[idx];
}

inline GPerlVirtualMachineCode *GPerlVirtualMachine::getFromFuncMemory(int idx)
{
	return func_memory[idx];
}

void GPerlVirtualMachine::createDirectThreadingCode(GPerlVirtualMachineCode *codes, void **jmp_tbl)
{
	if (codes->op == OPTHCODE) codes->op = OPNOP;
	GPerlVirtualMachineCode *pc = codes;
	for (; pc->op != OPUNDEF; pc++) {
		if (pc->op == OPFUNCSET) {
			createDirectThreadingCode(pc->func, jmp_tbl);
		}
		pc->opnext = jmp_tbl[pc->op];
	}
	pc->opnext = jmp_tbl[pc->op];
}

static void *gxmalloc(void)
{
	int pagesize = sysconf(_SC_PAGE_SIZE);
	DBG_PL("pagesize = [%d]", pagesize);
	if (pagesize < 0) perror("sysconf");
	void *_codeptr = (void *)malloc(pagesize * 10L);
	void *codeptr = (void *)((long)_codeptr & ~(pagesize - 1L));
	memset(codeptr, 0xc3, pagesize * 10L);
	if (mprotect(codeptr, pagesize * 10L, PROT_WRITE|PROT_READ|PROT_EXEC) < 0) perror("mprotect");
	return codeptr;
}

void GPerlVirtualMachine::createSelectiveInliningCode(GPerlVirtualMachineCode *c, void **jmp_tbl, InstBlock *block_tbl)
{
	GPerlVirtualMachineCode *pc = c;
	pc++;//skip THCODE
	for (; pc->op != OPUNDEF; pc++) {
		//pc->code = gxmalloc();
		//pos = 0;
		InstBlock block = block_tbl[pc->op];
		int len = (intptr_t)block.end - (intptr_t)block.start;
		DBG_PL("len = [%d]", len);
		if (pc->op == OPSUPERCALL) {
			DBG_PL("OPSUPERCALL");
			pc->code = gxmalloc();
			memcpy(pc->code, block.start, len);
		}
		if (pc->op == OPFUNCSET) {
			createSelectiveInliningCode(pc->func, jmp_tbl, block_tbl);
		}
		//memcpy(pc->code + pos, block.start, len);
		//pos += len;
		//DBG_PL("=====================================");
		//if (pc->op == OPSELFCALL) {
		//} else if (pc->op == OPRET || pc->op == OPARET || pc->op == OPBRET ||
		//		   pc->op == OPCRET || pc->op == OPDRET) {
			//pc->op = RET_BLOCK;
			//pc->opnext = jmp_table[pc->op];
		//} else {
			//pc->op = GOTO_BLOCK;
		//	pc->opnext = jmp_tbl[pc->op];
		//}
	}
}

#define DISPATCH_START() {						\
		callstack->ret_addr = &&L_RETURN;		\
		(callstack+1)->ret_addr = &&L_RETURN;	\
		goto *jmp_table[pc->op];				\
	}
#define DISPATCH_END() L_RETURN:
#define GOTO_NEXTOP() goto *(pc->opnext)
#define L(op) L_##op

#define B(op) {&&L(OP##op##_START), &&L(OP##op##_END)}
#define START(op) L_##op##_START:
#define END(op) L_##op##_END:

#define _CASE(op) L(op) : { DBG_PL(#op); }
#define BREAK() GOTO_NEXTOP()
#define CASE(op, block) _CASE(op) { START(op); block; END(op); }
#define RETURN() { goto *callstack->ret_addr; }

#define I(data) reg->i ## data
#define F(data) reg->f ## data
#define S(data) reg->s ## data
#define O(data) reg->p ## data

//I(data)[dst] = 1;
//I(data)[dst] = 0;
#define CMP_JMP(op, dst, src) {						\
		if (I(data)[dst] op I(data)[src]) {			\
			pc++;									\
		} else {									\
			pc += pc->jmp;							\
		}											\
	}

//I(data)[dst] = 1;
//I(data)[dst] = 0;
#define CMP_JMPC(op, dst) {						\
		if (I(data)[dst] op pc->src) {			\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}

#define ADD(dst, src)	I(data)[dst] += I(data)[src]
#define ADDC(dst)	I(data)[dst] += pc->src
#define SUB(dst, src) I(data)[dst] -= I(data)[src]
#define SUBC(dst)	I(data)[dst] -=  pc->src

static GPerlEnv env_[MAX_CALLSTACK_SIZE];
GPerlEnv *GPerlVirtualMachine::createCallStack(void)
{
	for (int i = 0; i < MAX_CALLSTACK_SIZE; i++) {
		GPerlObject **argstack = (GPerlObject **)malloc(sizeof(GPerlObject) * MAX_ARGSTACK_SIZE);
		env_[i].argstack = argstack;
		//effect to FAST PROGRAM?? (fib(36) 0.38 => 0.35)
		for (int j = 0; j < MAX_ARGSTACK_SIZE; j++) {
			GPerlObject *o = new GPerlObject();
			env_[i].argstack[j] = o;
		}
	}
	int size = MAX_CALLSTACK_SIZE * sizeof(GPerlEnv);
	GPerlEnv *callstack = (GPerlEnv *)malloc(size);
	memcpy(callstack, env_, size);
	return callstack;
}

static GPerlObject **argstack_[MAX_CALLSTACK_SIZE];
GPerlObject **GPerlVirtualMachine::createArgStack(void)
{
	for (int i = 0; i < MAX_CALLSTACK_SIZE; i++) {
		argstack_[i] = (GPerlObject **)malloc(sizeof(GPerlObject) * MAX_ARGSTACK_SIZE);
		memset(argstack_[i], 0, sizeof(GPerlObject) * MAX_ARGSTACK_SIZE);
	}
	return (GPerlObject **)argstack_;
}

int GPerlVirtualMachine::run(GPerlVirtualMachineCode *codes)
{
	static GPerlVirtualMachineCode *top;
	GPerlVirtualMachineCode *pc = codes;
	Reg reg_[MAX_CALLSTACK_SIZE];
	Reg *reg = reg_;
	GPerlEnv *callstack = createCallStack();
	GPerlObject **argstack = createArgStack();
	static char shared_buf[128] = {0};//TODO must be variable buffer
	static string outbuf = "";
	static void *jmp_table[] = {
		&&L(OPUNDEF),
		&&L(OPMOV), &&L(OPiMOV), &&L(OPsMOV),
		&&L(OPOMOV), &&L(OPLMOV), &&L(OPOiMOV),
		&&L(OPADD), &&L(OPiADD), &&L(OPiADDC),
		&&L(OPSUB), &&L(OPiSUB), &&L(OPiSUBC),
		&&L(OPMUL), &&L(OPiMUL), &&L(OPiMULC),
		&&L(OPDIV), &&L(OPiDIV), &&L(OPiDIVC),
		&&L(OPJG), &&L(OPiJG), &&L(OPiJGC),
		&&L(OPJL), &&L(OPiJL), &&L(OPiJLC),
		&&L(OPJGE), &&L(OPiJGE), &&L(OPiJGEC),
		&&L(OPJLE), &&L(OPiJLE), &&L(OPiJLEC),
		&&L(OPJE), &&L(OPiJE), &&L(OPiJEC),
		&&L(OPJNE), &&L(OPiJNE), &&L(OPiJNEC),
		&&L(OPRET), &&L(OPJRET), &&L(OPTHCODE), &&L(OPNOP),
		&&L(OPiWRITE), &&L(OPsWRITE), &&L(OPoWRITE),
		&&L(OPPRINT), &&L(OPJMP), &&L(OPLET),
		&&L(OPSET), &&L(OPFUNCSET),
		&&L(OPCALL), &&L(OPJCALL), &&L(OPSELFCALL), &&L(OPJSELFCALL), &&L(OPSHIFT),
		&&L(OPiPUSH), &&L(OPsPUSH), &&L(OPoPUSH), &&L(OPARRAY_PUSH), &&L(OPLIST_NEW),
		/*-------------final inst-----------------*/
		&&L(OPABADD), &&L(OPACADD), &&L(OPADADD), &&L(OPBCADD), &&L(OPBDADD), &&L(OPCDADD),
		&&L(OPAiSUBC), &&L(OPBiSUBC), &&L(OPCiSUBC), &&L(OPDiSUBC),
		&&L(OPAiJLC), &&L(OPBiJLC), &&L(OPCiJLC), &&L(OPDiJLC),
		&&L(OPAiPUSH), &&L(OPBiPUSH), &&L(OPCiPUSH), &&L(OPDiPUSH),
		&&L(OPAiMOV), &&L(OPBiMOV), &&L(OPCiMOV), &&L(OPDiMOV),
		&&L(OPAOMOV), &&L(OPBOMOV), &&L(OPCOMOV), &&L(OPDOMOV),
		&&L(OPASELFCALL), &&L(OPBSELFCALL), &&L(OPCSELFCALL), &&L(OPDSELFCALL),
		&&L(OPAJSELFCALL), &&L(OPBJSELFCALL), &&L(OPCJSELFCALL), &&L(OPDJSELFCALL),
		&&L(OPARET), &&L(OPBRET), &&L(OPCRET), &&L(OPDRET),
		&&L(OPAJRET), &&L(OPBJRET), &&L(OPCJRET), &&L(OPDJRET),
		&&L(OPSUPERCALL),
	};
	static InstBlock block_table[] = {
		B(UNDEF),
		B(MOV), B(iMOV), B(sMOV),
		B(OMOV), B(LMOV), B(OiMOV),
		B(ADD), B(iADD), B(iADDC),
		B(SUB), B(iSUB), B(iSUBC),
		B(MUL), B(iMUL), B(iMULC),
		B(DIV), B(iDIV), B(iDIVC),
		B(JG), B(iJG), B(iJGC),
		B(JL), B(iJL), B(iJLC),
		B(JGE), B(iJGE), B(iJGEC),
		B(JLE), B(iJLE), B(iJLEC),
		B(JE), B(iJE), B(iJEC),
		B(JNE), B(iJNE), B(iJNEC),
		B(RET), B(JRET), B(THCODE), B(NOP),
		B(iWRITE), B(sWRITE), B(oWRITE),
		B(PRINT), B(JMP), B(LET),
		B(SET), B(FUNCSET),
		B(CALL), B(JCALL), B(SELFCALL), B(JSELFCALL), B(SHIFT),
		B(iPUSH), B(sPUSH), B(oPUSH), B(ARRAY_PUSH), B(LIST_NEW),
		/*-------------final inst-----------------*/
		B(ABADD), B(ACADD), B(ADADD), B(BCADD), B(BDADD), B(CDADD),
		B(AiSUBC), B(BiSUBC), B(CiSUBC), B(DiSUBC),
		B(AiJLC), B(BiJLC), B(CiJLC), B(DiJLC),
		B(AiPUSH), B(BiPUSH), B(CiPUSH), B(DiPUSH),
		B(AiMOV), B(BiMOV), B(CiMOV), B(DiMOV),
		B(AOMOV), B(BOMOV), B(COMOV), B(DOMOV),
		B(ASELFCALL), B(BSELFCALL), B(CSELFCALL), B(DSELFCALL),
		B(AJSELFCALL), B(BJSELFCALL), B(CJSELFCALL), B(DJSELFCALL),
		B(ARET), B(BRET), B(CRET), B(DRET),
		B(AJRET), B(BJRET), B(CJRET), B(DJRET),
		B(SUPERCALL),
	};

	DISPATCH_START();

	CASE(OPUNDEF, {
		//no reach this case
	});
	CASE(OPMOV, {
		I(data)[pc->dst] = pc->src;
		pc++;
		BREAK();
	});
	CASE(OPiMOV, {
		I(data)[pc->dst] = pc->src;
		pc++;
		BREAK();
	});
	CASE(OPsMOV, {
		S(data)[pc->dst] = (char *)pc->name;
		pc++;
		BREAK();
	});
	CASE(OPOMOV, {
		I(data)[pc->dst] = callstack->argstack[pc->src]->value.idata;
		pc++;
		BREAK();
	});
	CASE(OPLMOV, {
		O(data)[pc->dst] = variable_memory[pc->src]->value.pdata;
		asm("int3");
		pc++;
		BREAK();
	});
	CASE(OPLIST_NEW, {
		O(data)[pc->dst] = (GPerlObject *)pc->list;
		asm("int3");
		pc++;
		BREAK();
	});
	CASE(OPOiMOV, {
		I(data)[pc->dst] = callstack->argstack[pc->src]->value.idata;
		pc++;
		BREAK();
	});
	CASE(OPADD, {
		ADD(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(OPiADD, {
		ADD(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(OPiADDC, {
		ADDC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(OPSUB, {
		SUB(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(OPiSUB, {
		SUB(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(OPiSUBC, {
		SUBC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(OPMUL, {
		I(data)[pc->dst] *= I(data)[pc->src];
		pc++;
		BREAK();
	});
	CASE(OPiMUL, {
		I(data)[pc->dst] *= I(data)[pc->src];
		pc++;
		BREAK();
	});
	CASE(OPiMULC, {
		I(data)[pc->dst] *= pc->src;
		pc++;
		BREAK();
	});
	CASE(OPDIV, {
		I(data)[pc->dst] /= I(data)[pc->src];
		pc++;
		BREAK();
	});
	CASE(OPiDIV, {
		I(data)[pc->dst] /= I(data)[pc->src];
		pc++;
		BREAK();
	});
	CASE(OPiDIVC, {
		I(data)[pc->dst] /= pc->src;
		pc++;
		BREAK();
	});
	CASE(OPJG, {
		CMP_JMP(>, pc->dst, pc->src);
		BREAK();
	});
	CASE(OPiJG, {
		CMP_JMP(>, pc->dst, pc->src);
		BREAK();
	});
	CASE(OPiJGC, {
		CMP_JMPC(>, pc->dst);
		BREAK();
	});
	CASE(OPJL, {
		CMP_JMP(<, pc->dst, pc->src);
		BREAK();
	});
	CASE(OPiJL, {
		CMP_JMP(<, pc->dst, pc->src);
		BREAK();
	});
	CASE(OPiJLC, {
		CMP_JMPC(<, pc->dst);
		BREAK();
	});
	CASE(OPJGE, {
		CMP_JMP(>=, pc->dst, pc->src);
		BREAK();
	});
	CASE(OPiJGE, {
		CMP_JMP(>=, pc->dst, pc->src);
		BREAK();
	});
	CASE(OPiJGEC, {
		CMP_JMPC(>=, pc->dst);
		BREAK();
	});
	CASE(OPJLE, {
		CMP_JMP(<=, pc->dst, pc->src);
		BREAK();
	});
	CASE(OPiJLE, {
		CMP_JMP(<=, pc->dst, pc->src);
		BREAK();
	});
	CASE(OPiJLEC, {
		CMP_JMPC(<=, pc->dst);
		BREAK();
	});
	CASE(OPJE, {
		CMP_JMP(==, pc->dst, pc->src);
		BREAK();
	});
	CASE(OPiJE, {
		CMP_JMP(==, pc->dst, pc->src);
		BREAK();
	});
	CASE(OPiJEC, {
		CMP_JMPC(==, pc->dst);
		BREAK();
	});
	CASE(OPJNE, {
		CMP_JMP(!=, pc->dst, pc->src);
		BREAK();
	});
	CASE(OPiJNE, {
		CMP_JMP(!=, pc->dst, pc->src);
		BREAK();
	});
	CASE(OPiJNEC, {
		CMP_JMPC(!=, pc->dst);
		BREAK();
	});
	CASE(OPiWRITE, {
		sprintf(shared_buf, "%d", I(data)[pc->dst]);
		outbuf += string(shared_buf);
		pc++;
		BREAK();
	});
	CASE(OPsWRITE, {
		outbuf += S(data)[pc->dst];
		pc++;
		BREAK();
	});
	CASE(OPoWRITE, {
		//TODO: need typecheck
		sprintf(shared_buf, "%p", O(data)[0]);
		outbuf += string(shared_buf);
		pc++;
		BREAK();
	});

	CASE(OPPRINT, {
		fprintf(stderr, "%s", cstr(outbuf));
		outbuf = "";
		pc++;
		BREAK();
	});
	CASE(OPJMP, {
		pc += pc->jmp;
		BREAK();
	});
	CASE(OPLET, {
//		variable_memory[pc->dst]->value.idata = I(data)[0];
		variable_memory[pc->dst]->value.pdata = O(data)[0];
		pc++;
		BREAK();
	});
	CASE(OPSET, {
		setToVariableMemory(pc->name, pc->dst);
		pc++;
		BREAK();
	});
	CASE(OPFUNCSET, {
		setToFuncMemory(pc->func, pc->dst);
		pc++;
		BREAK();
	});
	CASE(OPCALL, {
		GPerlVirtualMachineCode *code = func_memory[pc->src];
		top = code;
		I(data)[pc->dst] = run(code);
		pc++;
		BREAK();
	});
	CASE(OPJCALL, {
		GPerlVirtualMachineCode *code = func_memory[pc->src];
		top = code;
		callstack++;
		callstack->ret_addr = &&L_JCALLAFTER;
		callstack->pc = pc;
		reg++;
		pc = top;
		GOTO_NEXTOP();
	L_JCALLAFTER:
		pc = callstack->pc;
		int res = I(data)[0];
		callstack--;
		reg--;
		I(data)[pc->dst] = res;
		pc++;
		BREAK();
	});
	CASE(OPJSELFCALL, {
		callstack++;
		callstack->ret_addr = &&L_JSELFCALLAFTER;
		callstack->pc = pc;
		reg++;
		pc = top;
		GOTO_NEXTOP();
	L_JSELFCALLAFTER:
		pc = callstack->pc;
		int res = I(data)[0];
		callstack--;
		reg--;
		I(data)[pc->dst] = res;
		pc++;
		BREAK();
	});
	CASE(OPJRET, {
		I(data)[0] = I(data)[pc->src];
		RETURN();
	});
	CASE(OPSELFCALL, {
		I(data)[pc->dst] = run(top);
        argstack--;
		pc++;
		BREAK();
	});
	CASE(OPSHIFT, {
		I(data)[0] = argstack[pc->src]->value.idata;
		pc++;
		BREAK();
	});
	CASE(OPiPUSH, {
		(callstack+1)->argstack[pc->src]->value.idata = I(data)[pc->dst];
		pc++;
		BREAK();
	});
	CASE(OPsPUSH, {
		pc++;
		BREAK();
	});
	CASE(OPoPUSH, {
		(callstack+1)->argstack[pc->src]->value.pdata = O(data)[pc->dst];
		pc++;
		BREAK();
	});
	CASE(OPARRAY_PUSH, {
		GPerlObject **array = (GPerlObject **)((callstack+1)->argstack[0]->value.pdata);
		int data = (callstack+1)->argstack[1]->value.idata;//TODO
		asm("int3");
		pc++;
		BREAK();
	});
	CASE(OPTHCODE, {
		createDirectThreadingCode(codes, jmp_table);
		(void)block_table;
		//createSelectiveInliningCode(codes, jmp_table, block_table);
		return 0;
	});
	CASE(OPNOP, {
		pc++;
		BREAK();
	});
	CASE(OPRET, {
		return I(data)[pc->src];
	});
	CASE(OPABADD, {
		ADD(0, 1);
		pc++;
		BREAK();
	});
	CASE(OPACADD, {
		ADD(0, 2);
		pc++;
		BREAK();
	});
	CASE(OPADADD, {
		ADD(0, 3);
		pc++;
		BREAK();
	});
	CASE(OPBCADD, {
		ADD(1, 2);
		pc++;
		BREAK();
	});
	CASE(OPBDADD, {
		ADD(1, 3);
		pc++;
		BREAK();
	});
	CASE(OPCDADD, {
		ADD(2, 3);
		pc++;
		BREAK();
	});
	CASE(OPAiSUBC, {
		SUBC(0);
		pc++;
		BREAK();
	});
	CASE(OPBiSUBC, {
		SUBC(1);
		pc++;
		BREAK();
	});
	CASE(OPCiSUBC, {
		SUBC(2);
		pc++;
		BREAK();
	});
	CASE(OPDiSUBC, {
		SUBC(3);
		pc++;
		BREAK();
	});
	CASE(OPAiJLC, {
		CMP_JMPC(<, 0);
		BREAK();
	});
	CASE(OPBiJLC, {
		CMP_JMPC(<, 1);
		BREAK();
	});
	CASE(OPCiJLC, {
		CMP_JMPC(<, 2);
		BREAK();
	});
	CASE(OPDiJLC, {
		CMP_JMPC(<, 3);
		BREAK();
	});
	CASE(OPAiPUSH, {
		(callstack+1)->argstack[pc->src]->value.idata = I(data)[0];
		pc++;
		BREAK();
	});
	CASE(OPBiPUSH, {
		(callstack+1)->argstack[pc->src]->value.idata = I(data)[1];
		pc++;
		BREAK();
	});
	CASE(OPCiPUSH, {
		(callstack+1)->argstack[pc->src]->value.idata = I(data)[2];
		pc++;
		BREAK();
	});
	CASE(OPDiPUSH, {
		(callstack+1)->argstack[pc->src]->value.idata = I(data)[3];
		pc++;
		BREAK();
	});
	CASE(OPAiMOV, {
		I(data)[0] = pc->src;
		pc++;
		BREAK();
	});
	CASE(OPBiMOV, {
		I(data)[1] = pc->src;
		pc++;
		BREAK();
	});
	CASE(OPCiMOV, {
		I(data)[2] = pc->src;
		pc++;
		BREAK();
	});
	CASE(OPDiMOV, {
		I(data)[3] = pc->src;
		pc++;
		BREAK();
	});
	CASE(OPAOMOV, {
//		O(data)[0] = argstack[pc->src]->pdata;
		I(data)[0] = callstack->argstack[pc->src]->value.idata;
		pc++;
		BREAK();
	});
	CASE(OPBOMOV, {
		I(data)[1] = callstack->argstack[pc->src]->value.idata;
//		O(data)[1] = argstack[pc->src]->pdata;
		pc++;
		BREAK();
	});
	CASE(OPCOMOV, {
		I(data)[2] = argstack[pc->src]->value.idata;
		pc++;
		BREAK();
	});
	CASE(OPDOMOV, {
		I(data)[3] = argstack[pc->src]->value.idata;
		pc++;
		BREAK();
	});
	CASE(OPASELFCALL, {
		I(data)[0] = run(top);
        callstack--;
		pc++;
		BREAK();
	});
	CASE(OPBSELFCALL, {
		I(data)[1] = run(top);
        callstack--;
		pc++;
		BREAK();
	});
	CASE(OPCSELFCALL, {
		I(data)[2] = run(top);
        callstack--;
		pc++;
		BREAK();
	});
	CASE(OPDSELFCALL, {
		I(data)[3] = run(top);
        callstack--;
		pc++;
		BREAK();
	});
	CASE(OPAJSELFCALL, {
		callstack++;
		callstack->ret_addr = &&L_AJSELFCALLAFTER;
		callstack->pc = pc;
		reg++;
		pc = top;
		GOTO_NEXTOP();
	L_AJSELFCALLAFTER:
		DBG_PL("AJSELFCALLAFTER");
		pc = callstack->pc;
		int res = I(data)[0];
		callstack--;
		reg--;
		I(data)[0] = res;
		pc++;
		BREAK();
	});
	CASE(OPBJSELFCALL, {
		callstack++;
		callstack->ret_addr = &&L_BJSELFCALLAFTER;
		callstack->pc = pc;
		reg++;
		pc = top;
		GOTO_NEXTOP();
	L_BJSELFCALLAFTER:
		DBG_PL("BJSELFCALLAFTER");
		pc = callstack->pc;
		int res = I(data)[0];
		callstack--;
		reg--;
		I(data)[1] = res;
		pc++;
		BREAK();
	});
	CASE(OPCJSELFCALL, {
		callstack++;
		callstack->ret_addr = &&L_CJSELFCALLAFTER;
		callstack->pc = pc;
		reg++;
		pc = top;
		GOTO_NEXTOP();
	L_CJSELFCALLAFTER:
		pc = callstack->pc;
		int res = I(data)[0];
		callstack--;
		reg--;
		I(data)[2] = res;
		pc++;
		BREAK();
	});
	CASE(OPDJSELFCALL, {
		callstack++;
		callstack->ret_addr = &&L_DJSELFCALLAFTER;
		callstack->pc = pc;
		reg++;
		pc = top;
		GOTO_NEXTOP();
	L_DJSELFCALLAFTER:
		pc = callstack->pc;
		int res = I(data)[0];
		callstack--;
		reg--;
		I(data)[3] = res;
		pc++;
		BREAK();
	});
	CASE(OPARET, {
		return I(data)[0];
	});
	CASE(OPBRET, {
		return I(data)[1];
	});
	CASE(OPCRET, {
		return I(data)[2];
	});
	CASE(OPDRET, {
		return I(data)[3];
	});
	CASE(OPAJRET, {
		RETURN();
	});
	CASE(OPBJRET, {
		I(data)[0] = I(data)[1];
		RETURN();
	});
	CASE(OPCJRET, {
		I(data)[0] = I(data)[2];
		RETURN();
	});
	CASE(OPDJRET, {
		I(data)[0] = I(data)[3];
		RETURN();
	});
	CASE(OPSUPERCALL, {
	/*
		I(data)[0] = callstack->argstack[0]->idata;
		pc++;
		if (I(data)[0] < pc->src) {
			pc++;
			I(data)[0] = pc->src;
			pc++;
			return I(data)[0];
			pc += pc->jmp;
		} else {
			pc += pc->jmp;
			I(data)[0] -= pc->src;
			pc++;
			callstack++;
			callstack->argstack[0]->idata = I(data)[0];
			pc++;
			I(data)[0] = run(top);
			callstack--;
			pc++;
			I(data)[1] = callstack->argstack[0]->idata;
			pc++;
			I(data)[1] -= pc->src;
			pc++;
			callstack++;
			callstack->argstack[0]->idata = I(data)[1];
			pc++;
			I(data)[1] = run(top);
			callstack--;
			pc++;
			I(data)[0] += I(data)[1];
			pc++;
			return I(data)[0];
		}
	*/
	});
	DISPATCH_END();
	return I(data)[0];
}
