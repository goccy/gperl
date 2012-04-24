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

#define L(op) L_##op
#define CASE(op) L(op) :
#define DISPATCH_START() goto *jmp_table[pc->op]
#define GOTO_NEXTOP() goto *(pc->opnext)

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
	};
	//asm("int3");
	DISPATCH_START();

	CASE(OPUNDEF) {
		DBG_PL("OPUNDEF");
		//no reach this case
	}
	CASE(OPMOV) {
		DBG_PL("OPMOV");
		//reg.idata[pc->dst] = pc->src;
		idata[pc->dst] = pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiMOV) {
		DBG_PL("OPiMOV");
		idata[pc->dst] = pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPsMOV) {
		DBG_PL("OPsMOV");
		sdata[pc->dst] = (char *)pc->name;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPOMOV) {
		DBG_PL("OPOMOV");
		//DBG_PL("reg[%d] = [%d]", pc->dst, getFromVariableMemory(pc->src)->data.idata);
		//reg.idata[pc->dst] = getFromVariableMemory(pc->src)->data.idata;
		//reg.pdata[pc->dst] = getFromVariableMemory(pc->src)->data.pdata;
		idata[pc->dst] = callstack->argstack[pc->src]->idata;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPOiMOV) {
		DBG_PL("OPOiMOV");
		//DBG_PL("reg[%d] = [%d]", pc->dst, getFromVariableMemory(pc->src)->data.idata);
		idata[pc->dst] = getFromVariableMemory(pc->src)->idata;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPADD) {
		DBG_PL("OPADD");
		idata[pc->dst] += idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiADD) {
		DBG_PL("OPiADD");
		idata[pc->dst] += idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiADDC) {
		DBG_PL("OPiADDC");
		idata[pc->dst] += pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPSUB) {
		DBG_PL("OPSUB");
		//DBG_PL("reg[%d]:%d -= reg[%d]:%d", pc->dst, reg.idata[pc->dst],
		//   pc->src, reg.idata[pc->src]);
		idata[pc->dst] -= idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiSUB) {
		DBG_PL("OPiSUB");
		idata[pc->dst] -= idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiSUBC) {
		DBG_PL("OPiSUBC");
		idata[pc->dst] -= pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPMUL) {
		DBG_PL("OPMUL");
		idata[pc->dst] *= idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiMUL) {
		DBG_PL("OPiMUL");
		idata[pc->dst] *= idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiMULC) {
		DBG_PL("OPiMULC");
		idata[pc->dst] *= pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPDIV) {
		DBG_PL("OPDIV");
		idata[pc->dst] /= idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiDIV) {
		DBG_PL("OPiDIV");
		idata[pc->dst] /= idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiDIVC) {
		DBG_PL("OPiDIVC");
		idata[pc->dst] /= pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPJG) {
		DBG_PL("OPJG");
		if (idata[pc->dst] > idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJG) {
		DBG_PL("OPiJG");
		if (idata[pc->dst] > idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJGC) {
		DBG_PL("OPiJGC");
		if (idata[pc->dst] > pc->src) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPJL) {
		DBG_PL("OPJL");
		if (idata[pc->dst] < idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJL) {
		DBG_PL("OPiJL");
		if (idata[pc->dst] < idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJLC) {
		DBG_PL("OPiJLC");
		if (idata[pc->dst] < pc->src) {
			//reg.idata[pc->dst] = 1;
			pc++;
		} else {
			//reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPJGE) {
		DBG_PL("OPJGE");
		if (idata[pc->dst] >= idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJGE) {
		DBG_PL("OPiJGE");
		if (idata[pc->dst] >= idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJGEC) {
		DBG_PL("OPiJGEC");
		if (idata[pc->dst] >= pc->src) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPJLE) {
		DBG_PL("OPJLE");
		if (idata[pc->dst] <= idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJLE) {
		DBG_PL("OPiJLE");
		if (idata[pc->dst] <= idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJLEC) {
		DBG_PL("OPiJLEC");
		if (idata[pc->dst] <= pc->src) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPJE) {
		DBG_PL("OPJE");
		if (idata[pc->dst] == idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJE) {
		DBG_PL("OPiJE");
		if (idata[pc->dst] == idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJEC) {
		DBG_PL("OPiJEC");
		if (idata[pc->dst] == pc->src) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPJNE) {
		DBG_PL("OPJNE");
		if (idata[pc->dst] != idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJNE) {
		DBG_PL("OPiJNE");
		if (idata[pc->dst] != idata[pc->src]) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJNEC) {
		DBG_PL("OPiJNEC");
		if (idata[pc->dst] != pc->src) {
			idata[pc->dst] = 1;
			pc++;
		} else {
			idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiWRITE) {
		DBG_PL("OPiWRITE");
		sprintf(shared_buf, "%d", idata[0]);
		outbuf += string(shared_buf);
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPsWRITE) {
		DBG_PL("OPsWRITE");
		outbuf += sdata[0];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPoWRITE) {
		DBG_PL("OPoWRITE");
		//TODO: need typecheck
		sprintf(shared_buf, "%p", pdata[0]);
		outbuf += string(shared_buf);
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPPRINT) {
		DBG_PL("OPPRINT");
		fprintf(stderr, "%s", cstr(outbuf));
		outbuf = "";
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPJMP) {
		DBG_PL("OPJMP");
		pc += pc->jmp;
		GOTO_NEXTOP();
	}
	CASE(OPLET) {
		DBG_PL("OPLET");
		GPerlObject *o = getFromVariableMemory(pc->dst);
		o->idata = idata[0];
		//o->data.pdata = reg.pdata[0];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPSET) {
		DBG_PL("OPSET");
		setToVariableMemory(pc->name, pc->dst);
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPFUNCSET) {
		DBG_PL("OPFUNCSET");
		setToFuncMemory(pc->func, pc->dst);
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPCALL) {
		DBG_PL("OPCALL");
		//GPerlVirtualMachineCode *code = getFromFuncMemory(pc->src);
		GPerlVirtualMachineCode *code = func_memory[pc->src];
		top = code;
		int res = run(code);
        callstack--;
		idata[pc->dst] = res;
		//DBG_PL("res = [%d]", res);
		//DBG_PL("reg[%d] = [%d]", pc->dst, reg.idata[pc->dst]);
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPSELFCALL) {
		DBG_PL("OPSELFCALL");
		idata[pc->dst] = run(top);
        callstack--;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPSHIFT) {
		DBG_PL("OPSHIFT");
		idata[0] = callstack->argstack[pc->src]->idata;
		//DBG_PL("reg[%d]%d = argstack[%d]", 0, reg.idata[0], pc->src);
		//reg.pdata[0] = callstack->argstack[pc->src]->data.pdata;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiPUSH) {
		DBG_PL("OPiPUSH");
		callstack++;//TODO : multiple arg
		callstack->argstack[pc->src]->idata = idata[pc->dst];
		//DBG_PL("argstack[%d] = reg[%d]:%d", pc->src, pc->dst, reg.idata[pc->dst]);
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPsPUSH) {
		DBG_PL("OPsPUSH");
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPTHCODE) {
		DBG_PL("OPTHCODE");
		createDirectThreadingCode(codes, jmp_table);
		return 0;
	}
	CASE(OPNOP) {
		DBG_PL("OPNOP");
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPRET) {
		DBG_PL("OPRET");
		return idata[pc->src];
		//DBG_PL("ret = [%d]", reg.idata[0]);
		//return idata[0];
	}

	CASE(OPABADD) {
		DBG_PL("OPABADD");
		idata[0] += idata[1];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPACADD) {
		DBG_PL("OPACADD");
		idata[0] += idata[2];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPADADD) {
		DBG_PL("OPADADD");
		idata[0] += idata[3];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPBCADD) {
		DBG_PL("OPBCADD");
		idata[1] += idata[2];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPBDADD) {
		DBG_PL("OPBDADD");
		idata[1] += idata[3];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPCDADD) {
		DBG_PL("OPCDADD");
		idata[2] += idata[3];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPAiSUBC) {
		DBG_PL("OPAiSUBC");
		//eax -= pc->src;
		idata[0] -= pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPBiSUBC) {
		DBG_PL("OPBiSUBC");
		//ebx -= pc->src;
		idata[1] -= pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPCiSUBC) {
		DBG_PL("OPCiSUBC");
		idata[2] -= pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPDiSUBC) {
		DBG_PL("OPDiSUBC");
		idata[3] -= pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPAiJLC) {
		DBG_PL("OPAiJLC");
		if (idata[0] < pc->src) {
//		if (eax < pc->src) {
			pc++;
		} else {
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPBiJLC) {
		DBG_PL("OPBiJLC");
		if (idata[1] < pc->src) {
			pc++;
		} else {
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPCiJLC) {
		DBG_PL("OPCiJLC");
		if (idata[2] < pc->src) {
			pc++;
		} else {
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPDiJLC) {
		DBG_PL("OPDiJLC");
		if (idata[3] < pc->src) {
			pc++;
		} else {
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPAiPUSH) {
		DBG_PL("OPAiPUSH");
		callstack++;
		//callstack->argstack[pc->src]->data.idata = eax;
		callstack->argstack[pc->src]->idata = idata[0];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPBiPUSH) {
		DBG_PL("OPBiPUSH");
		callstack++;
		//callstack->argstack[pc->src]->data.idata = ebx;
		callstack->argstack[pc->src]->idata = idata[1];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPCiPUSH) {
		DBG_PL("OPCiPUSH");
		callstack++;
		callstack->argstack[pc->src]->idata = idata[2];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPDiPUSH) {
		DBG_PL("OPDiPUSH");
		callstack++;
		callstack->argstack[pc->src]->idata = idata[3];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPAiMOV) {
		DBG_PL("OPAiMOV");
		//eax = pc->src;
		idata[0] = pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPBiMOV) {
		DBG_PL("OPBiMOV");
		idata[1] = pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPCiMOV) {
		DBG_PL("OPCiMOV");
		idata[2] = pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPDiMOV) {
		DBG_PL("OPDiMOV");
		idata[3] = pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPAOMOV) {
		DBG_PL("OPAOMOV");
		//eax = callstack->argstack[pc->src]->data.idata;
		idata[0] = callstack->argstack[pc->src]->idata;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPBOMOV) {
		DBG_PL("OPBOMOV");
		//ebx = callstack->argstack[pc->src]->data.idata;
		idata[1] = callstack->argstack[pc->src]->idata;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPCOMOV) {
		DBG_PL("OPCOMOV");
		idata[2] = callstack->argstack[pc->src]->idata;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPDOMOV) {
		DBG_PL("OPDOMOV");
		idata[3] = callstack->argstack[pc->src]->idata;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPASELFCALL) {
		DBG_PL("OPASELFCALL");
		//eax = run(top);
		idata[0] = run(top);
        callstack--;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPBSELFCALL) {
		DBG_PL("OPBSELFCALL");
		//ebx = run(top);
		idata[1] = run(top);
        callstack--;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPCSELFCALL) {
		DBG_PL("OPCSELFCALL");
		idata[2] = run(top);
        callstack--;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPDSELFCALL) {
		DBG_PL("OPDSELFCALL");
		idata[3] = run(top);
        callstack--;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPARET) {
		DBG_PL("OPARET");
		///return eax;
		return idata[0];
	}
	CASE(OPBRET) {
		DBG_PL("OPBRET");
		return idata[1];
	}
	CASE(OPCRET) {
		DBG_PL("OPCRET");
		return idata[2];
	}
	CASE(OPDRET) {
		DBG_PL("OPDRET");
		return idata[3];
	}
}
