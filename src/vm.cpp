#include <gperl.hpp>

using namespace std;

GPerlVirtualMachine::GPerlVirtualMachine(void)
{
	//Set DEFAULT Object
	GPerlEnv env_[MAX_CALLSTACK_SIZE];
	for (int i = 0; i < MAX_CALLSTACK_SIZE; i++) {
		GPerlObject **argstack = (GPerlObject **)malloc(sizeof(GPerlObject) * MAX_ARGSTACK_SIZE);
		env_[i].argstack = argstack;
		for (int j = 0; j < MAX_ARGSTACK_SIZE; j++) {
			GPerlObject *o = new GPerlObject();
			env_[i].argstack[j] = o;
		}
	}
	int size = MAX_CALLSTACK_SIZE * sizeof(GPerlEnv);
	callstack = (GPerlEnv *)malloc(size);
	memcpy(callstack, env_, size);
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
	int pos = 0;
	GPerlVirtualMachineCode *pc = c;
	pc++;//skip THCODE
	for (; pc->op != OPUNDEF; pc++) {
		//pc->code = gxmalloc();
		//pos = 0;
		InstBlock block = block_tbl[pc->op];
		int len = (intptr_t)block.end - (intptr_t)block.start;
		DBG_PL("len = [%d]", len);
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

#define DISPATCH_START() goto *jmp_table[pc->op]
#define GOTO_NEXTOP() goto *(pc->opnext)
#define L(op) L_##op

#define B(op) {&&L(OP##op##_START), &&L(OP##op##_END)}
#define START(op) L_##op##_START:
#define END(op) L_##op##_END:

#define _CASE(op) L(op) : { DBG_PL(#op); }
#define BREAK() GOTO_NEXTOP()
#define CASE(op, block) _CASE(op) { START(op); block; END(op); }

static GPerlVirtualMachineCode *top;
int GPerlVirtualMachine::run(GPerlVirtualMachineCode *codes)
{
	GPerlVirtualMachineCode *pc = codes;
	union {
		int idata[MAX_REG_SIZE];
		float fdata[MAX_REG_SIZE];
		char *sdata[MAX_REG_SIZE];
		void *pdata[MAX_REG_SIZE];
	};// reg;
	static char shared_buf[128] = {0};//TODO must be variable buffer
	static string outbuf = "";
	static void *jmp_table[] = {
		&&L(OPUNDEF),
		&&L(OPMOV), &&L(OPiMOV), &&L(OPsMOV),
		&&L(OPOMOV), &&L(OPOiMOV),
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
		&&L(OPRET), &&L(OPTHCODE), &&L(OPNOP),
		&&L(OPiWRITE), &&L(OPsWRITE), &&L(OPoWRITE),
		&&L(OPPRINT), &&L(OPJMP), &&L(OPLET),
		&&L(OPSET), &&L(OPFUNCSET),
		&&L(OPCALL), &&L(OPSELFCALL), &&L(OPSHIFT),
		&&L(OPiPUSH), &&L(OPsPUSH),
		/*-------------final inst-----------------*/
		&&L(OPABADD), &&L(OPACADD), &&L(OPADADD), &&L(OPBCADD), &&L(OPBDADD), &&L(OPCDADD),
		&&L(OPAiSUBC), &&L(OPBiSUBC), &&L(OPCiSUBC), &&L(OPDiSUBC),
		&&L(OPAiJLC), &&L(OPBiJLC), &&L(OPCiJLC), &&L(OPDiJLC),
		&&L(OPAiPUSH), &&L(OPBiPUSH), &&L(OPCiPUSH), &&L(OPDiPUSH),
		&&L(OPAiMOV), &&L(OPBiMOV), &&L(OPCiMOV), &&L(OPDiMOV),
		&&L(OPAOMOV), &&L(OPBOMOV), &&L(OPCOMOV), &&L(OPDOMOV),
		&&L(OPASELFCALL), &&L(OPBSELFCALL), &&L(OPCSELFCALL), &&L(OPDSELFCALL),
		&&L(OPARET), &&L(OPBRET), &&L(OPCRET), &&L(OPDRET),
		&&L(OPSUPERCALL),
	};
	static InstBlock block_table[] = {
		B(UNDEF),
		B(MOV), B(iMOV), B(sMOV),
		B(OMOV), B(OiMOV),
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
		B(RET), B(THCODE), B(NOP),
		B(iWRITE), B(sWRITE), B(oWRITE),
		B(PRINT), B(JMP), B(LET),
		B(SET), B(FUNCSET),
		B(CALL), B(SELFCALL), B(SHIFT),
		B(iPUSH), B(sPUSH),
		/*-------------final inst-----------------*/
		B(ABADD), B(ACADD), B(ADADD), B(BCADD), B(BDADD), B(CDADD),
		B(AiSUBC), B(BiSUBC), B(CiSUBC), B(DiSUBC),
		B(AiJLC), B(BiJLC), B(CiJLC), B(DiJLC),
		B(AiPUSH), B(BiPUSH), B(CiPUSH), B(DiPUSH),
		B(AiMOV), B(BiMOV), B(CiMOV), B(DiMOV),
		B(AOMOV), B(BOMOV), B(COMOV), B(DOMOV),
		B(ASELFCALL), B(BSELFCALL), B(CSELFCALL), B(DSELFCALL),
		B(ARET), B(BRET), B(CRET), B(DRET),
		B(SUPERCALL),
	};

	//asm("int3");
	DISPATCH_START();

	CASE(OPUNDEF, {
		//no reach this case
	});
	CASE(OPMOV, {
		//reg.idata[pc->dst] = pc->src;
		idata[pc->dst] = pc->src;
		pc++;
		BREAK();
	});
	CASE(OPiMOV, {
		idata[pc->dst] = pc->src;
		pc++;
		BREAK();
	});
	CASE(OPsMOV, {
		sdata[pc->dst] = (char *)pc->name;
		pc++;
		BREAK();
	});
	CASE(OPOMOV, {
		//DBG_PL("reg[%d] = [%d]", pc->dst, getFromVariableMemory(pc->src)->data.idata);
		//reg.idata[pc->dst] = getFromVariableMemory(pc->src)->data.idata;
		//reg.pdata[pc->dst] = getFromVariableMemory(pc->src)->data.pdata;
		idata[pc->dst] = callstack->argstack[pc->src]->idata;
		pc++;
		BREAK();
	});
	CASE(OPOiMOV, {
		//DBG_PL("reg[%d] = [%d]", pc->dst, getFromVariableMemory(pc->src)->data.idata);
		idata[pc->dst] = getFromVariableMemory(pc->src)->idata;
		pc++;
		BREAK();
	});
	CASE(OPADD, {
		idata[pc->dst] += idata[pc->src];
		pc++;
		BREAK();
	});
	CASE(OPiADD, {
		idata[pc->dst] += idata[pc->src];
		pc++;
		BREAK();
	});
	CASE(OPiADDC, {
		idata[pc->dst] += pc->src;
		pc++;
		BREAK();
	});
	CASE(OPSUB, {
		//DBG_PL("reg[%d]:%d -= reg[%d]:%d", pc->dst, reg.idata[pc->dst],
		//   pc->src, reg.idata[pc->src]);
		idata[pc->dst] -= idata[pc->src];
		pc++;
		BREAK();
	});
	CASE(OPiSUB, {
		idata[pc->dst] -= idata[pc->src];
		pc++;
		BREAK();
	});
	CASE(OPiSUBC, {
		idata[pc->dst] -= pc->src;
		pc++;
		BREAK();
	});
	CASE(OPMUL, {
		idata[pc->dst] *= idata[pc->src];
		pc++;
		BREAK();
	});
	CASE(OPiMUL, {
		idata[pc->dst] *= idata[pc->src];
		pc++;
		BREAK();
	});
	CASE(OPiMULC, {
		idata[pc->dst] *= pc->src;
		pc++;
		BREAK();
	});
	CASE(OPDIV, {
		idata[pc->dst] /= idata[pc->src];
		pc++;
		BREAK();
	});
	CASE(OPiDIV, {
		idata[pc->dst] /= idata[pc->src];
		pc++;
		BREAK();
	});
	CASE(OPiDIVC, {
		idata[pc->dst] /= pc->src;
		pc++;
		BREAK();
	});
	CASE(OPJG, {
		if (idata[pc->dst] > idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPiJG, {
		if (idata[pc->dst] > idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPiJGC, {
		if (idata[pc->dst] > pc->src) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPJL, {
		if (idata[pc->dst] < idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPiJL, {
		if (idata[pc->dst] < idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPiJLC, {
		if (idata[pc->dst] < pc->src) {
			//reg.idata[pc->dst] = 1;
			pc++;
		} else {
			//reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPJGE, {
		if (idata[pc->dst] >= idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPiJGE, {
		if (idata[pc->dst] >= idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPiJGEC, {
		if (idata[pc->dst] >= pc->src) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPJLE, {
		if (idata[pc->dst] <= idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPiJLE, {
		if (idata[pc->dst] <= idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPiJLEC, {
		if (idata[pc->dst] <= pc->src) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPJE, {
		if (idata[pc->dst] == idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPiJE, {
		if (idata[pc->dst] == idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPiJEC, {
		if (idata[pc->dst] == pc->src) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPJNE, {
		if (idata[pc->dst] != idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPiJNE, {
		if (idata[pc->dst] != idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPiJNEC, {
		if (idata[pc->dst] != pc->src) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPiWRITE, {
		sprintf(shared_buf, "%d", idata[0]);
		outbuf += string(shared_buf);
		pc++;
		BREAK();
	});
	CASE(OPsWRITE, {
		outbuf += sdata[0];
		pc++;
		BREAK();
	});
	CASE(OPoWRITE, {
		//TODO: need typecheck
		sprintf(shared_buf, "%p", pdata[0]);
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
		GPerlObject *o = getFromVariableMemory(pc->dst);
		o->idata = idata[0];
		//o->data.pdata = reg.pdata[0];
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
		int res = run(code);
        callstack--;
		idata[pc->dst] = res;
		pc++;
		BREAK();
	});
	CASE(OPSELFCALL, {
		idata[pc->dst] = run(top);
        callstack--;
		pc++;
		BREAK();
	});
	CASE(OPSHIFT, {
		idata[0] = callstack->argstack[pc->src]->idata;
		//DBG_PL("reg[%d]%d = argstack[%d]", 0, reg.idata[0], pc->src);
		//reg.pdata[0] = callstack->argstack[pc->src]->data.pdata;
		pc++;
		BREAK();
	});
	CASE(OPiPUSH, {
		callstack++;//TODO : multiple arg
		callstack->argstack[pc->src]->idata = idata[pc->dst];
		//DBG_PL("argstack[%d] = reg[%d]:%d", pc->src, pc->dst, reg.idata[pc->dst]);
		pc++;
		BREAK();
	});
	CASE(OPsPUSH, {
		pc++;
		BREAK();
	});
	CASE(OPTHCODE, {
		createDirectThreadingCode(codes, jmp_table);
		createSelectiveInliningCode(codes, jmp_table, block_table);
		return 0;
	});
	CASE(OPNOP, {
		pc++;
		BREAK();
	});
	CASE(OPRET, {
		return idata[pc->src];
	});
	CASE(OPABADD, {
		idata[0] += idata[1];
		pc++;
		BREAK();
	});
	CASE(OPACADD, {
		idata[0] += idata[2];
		pc++;
		BREAK();
	});
	CASE(OPADADD, {
		idata[0] += idata[3];
		pc++;
		BREAK();
	});
	CASE(OPBCADD, {
		idata[1] += idata[2];
		pc++;
		BREAK();
	});
	CASE(OPBDADD, {
		idata[1] += idata[3];
		pc++;
		BREAK();
	});
	CASE(OPCDADD, {
		idata[2] += idata[3];
		pc++;
		BREAK();
	});
	CASE(OPAiSUBC, {
		//eax -= pc->src;
		idata[0] -= pc->src;
		pc++;
		BREAK();
	});
	CASE(OPBiSUBC, {
		//ebx -= pc->src;
		idata[1] -= pc->src;
		pc++;
		BREAK();
	});
	CASE(OPCiSUBC, {
		idata[2] -= pc->src;
		pc++;
		BREAK();
	});
	CASE(OPDiSUBC, {
		idata[3] -= pc->src;
		pc++;
		BREAK();
	});
	CASE(OPAiJLC, {
		if (idata[0] < pc->src) {
//		if (eax < pc->src) {
			pc++;
		} else {
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPBiJLC, {
		if (idata[1] < pc->src) {
			pc++;
		} else {
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPCiJLC, {
		if (idata[2] < pc->src) {
			pc++;
		} else {
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPDiJLC, {
		if (idata[3] < pc->src) {
			pc++;
		} else {
			pc += pc->jmp;
		}
		BREAK();
	});
	CASE(OPAiPUSH, {
		callstack++;
		//callstack->argstack[pc->src]->data.idata = eax;
		//callstack->argstack[0]->idata = idata[0];
		callstack->argstack[pc->src]->idata = idata[0];
		pc++;
		BREAK();
	});
	CASE(OPBiPUSH, {
		callstack++;
		//callstack->argstack[pc->src]->data.idata = ebx;
		//callstack->argstack[0]->idata = idata[1];
		callstack->argstack[pc->src]->idata = idata[1];
		pc++;
		BREAK();
	});
	CASE(OPCiPUSH, {
		callstack++;
		callstack->argstack[pc->src]->idata = idata[2];
		pc++;
		BREAK();
	});
	CASE(OPDiPUSH, {
		callstack++;
		callstack->argstack[pc->src]->idata = idata[3];
		pc++;
		BREAK();
	});
	CASE(OPAiMOV, {
		//eax = pc->src;
		idata[0] = pc->src;
		pc++;
		BREAK();
	});
	CASE(OPBiMOV, {
		idata[1] = pc->src;
		pc++;
		BREAK();
	});
	CASE(OPCiMOV, {
		idata[2] = pc->src;
		pc++;
		BREAK();
	});
	CASE(OPDiMOV, {
		idata[3] = pc->src;
		pc++;
		BREAK();
	});
	CASE(OPAOMOV, {
		//eax = callstack->argstack[pc->src]->data.idata;
		//idata[0] = callstack->argstack[0]->idata;
		idata[0] = callstack->argstack[pc->src]->idata;
		pc++;
		BREAK();
	});
	CASE(OPBOMOV, {
		//ebx = callstack->argstack[pc->src]->data.idata;
		//idata[1] = callstack->argstack[0]->idata;
		idata[1] = callstack->argstack[pc->src]->idata;
		pc++;
		BREAK();
	});
	CASE(OPCOMOV, {
		idata[2] = callstack->argstack[pc->src]->idata;
		pc++;
		BREAK();
	});
	CASE(OPDOMOV, {
		idata[3] = callstack->argstack[pc->src]->idata;
		pc++;
		BREAK();
	});
	CASE(OPASELFCALL, {
		//eax = run(top);
		idata[0] = run(top);
        callstack--;
		pc++;
		BREAK();
	});
	CASE(OPBSELFCALL, {
		//ebx = run(top);
		idata[1] = run(top);
        callstack--;
		pc++;
		BREAK();
	});
	CASE(OPCSELFCALL, {
		idata[2] = run(top);
        callstack--;
		pc++;
		BREAK();
	});
	CASE(OPDSELFCALL, {
		idata[3] = run(top);
        callstack--;
		pc++;
		BREAK();
	});
	CASE(OPARET, {
		///return eax;
		return idata[0];
	});
	CASE(OPBRET, {
		return idata[1];
	});
	CASE(OPCRET, {
		return idata[2];
	});
	CASE(OPDRET, {
		return idata[3];
	});
	CASE(OPSUPERCALL, {
/* AOMOV */
		idata[0] = callstack->argstack[0]->idata;
		pc++;
/* AiJLC */
		if (idata[0] < pc->src) {
			pc++;
/* AiMOV */
			idata[0] = pc->src;
			pc++;
/* ARET */
			return idata[0];
/* JMP */
			pc += pc->jmp;
		} else {
			pc += pc->jmp;
/* AiSUBC */
			idata[0] -= pc->src;
			pc++;
/* AiPUSH */
			callstack++;
			callstack->argstack[0]->idata = idata[0];
			pc++;
/* ASELFCALL */
			idata[0] = run(top);
			callstack--;
			pc++;
/* OPBOMOV */
			idata[1] = callstack->argstack[0]->idata;
			pc++;
/* OPBiSUBC */
			idata[1] -= pc->src;
			pc++;
/* OPBiPUSH */
			callstack++;
			callstack->argstack[0]->idata = idata[1];
			pc++;
/* OPBSELFCALL */
			idata[1] = run(top);
			callstack--;
			pc++;
/* ABADD */
			idata[0] += idata[1];
			pc++;
/* ARET */
			return idata[0];
		}
	});
	return 0;
}
