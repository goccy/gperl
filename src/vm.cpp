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

int GPerlVirtualMachine::run(GPerlVirtualMachineCode *codes)
{
	GPerlVirtualMachineCode *pc = codes;
	union {
		int idata[MAX_REG_SIZE];
		float fdata[MAX_REG_SIZE];
		char *sdata[MAX_REG_SIZE];
		void *pdata[MAX_REG_SIZE];
	} reg;
	char shared_buf[128] = {0};//TODO must be variable buffer
	string outbuf = "";
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
		&&L(OPCALL), &&L(OPSHIFT),
		&&L(OPiPUSH), &&L(OPsPUSH),
	};
	//asm("int3");
	DISPATCH_START();

	CASE(OPUNDEF) {
		DBG_PL("OPUNDEF");
		//no reach this case
	}
	CASE(OPMOV) {
		DBG_PL("OPMOV");
		reg.idata[pc->dst] = pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiMOV) {
		DBG_PL("OPiMOV");
		reg.idata[pc->dst] = pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPsMOV) {
		DBG_PL("OPsMOV");
		reg.sdata[pc->dst] = (char *)pc->name;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPOMOV) {
		DBG_PL("OPOMOV");
		//DBG_PL("reg[%d] = [%d]", pc->dst, getFromVariableMemory(pc->src)->data.idata);
		//reg.idata[pc->dst] = getFromVariableMemory(pc->src)->data.idata;
		//reg.pdata[pc->dst] = getFromVariableMemory(pc->src)->data.pdata;
		reg.idata[pc->dst] = callstack->argstack[pc->src]->data.idata;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPOiMOV) {
		DBG_PL("OPOiMOV");
		//DBG_PL("reg[%d] = [%d]", pc->dst, getFromVariableMemory(pc->src)->data.idata);
		reg.idata[pc->dst] = getFromVariableMemory(pc->src)->data.idata;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPADD) {
		DBG_PL("OPADD");
		reg.idata[pc->dst] += reg.idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiADD) {
		DBG_PL("OPiADD");
		reg.idata[pc->dst] += reg.idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiADDC) {
		DBG_PL("OPiADDC");
		reg.idata[pc->dst] += pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPSUB) {
		DBG_PL("OPSUB");
		//DBG_PL("reg[%d]:%d -= reg[%d]:%d", pc->dst, reg.idata[pc->dst],
		//   pc->src, reg.idata[pc->src]);
		reg.idata[pc->dst] -= reg.idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiSUB) {
		DBG_PL("OPiSUB");
		reg.idata[pc->dst] -= reg.idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiSUBC) {
		DBG_PL("OPiSUBC");
		reg.idata[pc->dst] -= pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPMUL) {
		DBG_PL("OPMUL");
		reg.idata[pc->dst] *= reg.idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiMUL) {
		DBG_PL("OPiMUL");
		reg.idata[pc->dst] *= reg.idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiMULC) {
		DBG_PL("OPiMULC");
		reg.idata[pc->dst] *= pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPDIV) {
		DBG_PL("OPDIV");
		reg.idata[pc->dst] /= reg.idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiDIV) {
		DBG_PL("OPiDIV");
		reg.idata[pc->dst] /= reg.idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiDIVC) {
		DBG_PL("OPiDIVC");
		reg.idata[pc->dst] /= pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPJG) {
		DBG_PL("OPJG");
		if (reg.idata[pc->dst] > reg.idata[pc->src]) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJG) {
		DBG_PL("OPiJG");
		if (reg.idata[pc->dst] > reg.idata[pc->src]) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJGC) {
		DBG_PL("OPiJGC");
		if (reg.idata[pc->dst] > pc->src) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPJL) {
		DBG_PL("OPJL");
		if (reg.idata[pc->dst] < reg.idata[pc->src]) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJL) {
		DBG_PL("OPiJL");
		if (reg.idata[pc->dst] < reg.idata[pc->src]) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJLC) {
		DBG_PL("OPiJLC");
		if (reg.idata[pc->dst] < pc->src) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPJGE) {
		DBG_PL("OPJGE");
		if (reg.idata[pc->dst] >= reg.idata[pc->src]) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJGE) {
		DBG_PL("OPiJGE");
		if (reg.idata[pc->dst] >= reg.idata[pc->src]) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJGEC) {
		DBG_PL("OPiJGEC");
		if (reg.idata[pc->dst] >= pc->src) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPJLE) {
		DBG_PL("OPJLE");
		if (reg.idata[pc->dst] <= reg.idata[pc->src]) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJLE) {
		DBG_PL("OPiJLE");
		if (reg.idata[pc->dst] <= reg.idata[pc->src]) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJLEC) {
		DBG_PL("OPiJLEC");
		if (reg.idata[pc->dst] <= pc->src) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPJE) {
		DBG_PL("OPJE");
		if (reg.idata[pc->dst] == reg.idata[pc->src]) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJE) {
		DBG_PL("OPiJE");
		if (reg.idata[pc->dst] == reg.idata[pc->src]) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJEC) {
		DBG_PL("OPiJEC");
		if (reg.idata[pc->dst] == pc->src) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPJNE) {
		DBG_PL("OPJNE");
		if (reg.idata[pc->dst] != reg.idata[pc->src]) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJNE) {
		DBG_PL("OPiJNE");
		if (reg.idata[pc->dst] != reg.idata[pc->src]) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiJNEC) {
		DBG_PL("OPiJNEC");
		if (reg.idata[pc->dst] != pc->src) {
			reg.idata[pc->dst] = 1;
			pc++;
		} else {
			reg.idata[pc->dst] = 0;
			pc += pc->jmp;
		}
		GOTO_NEXTOP();
	}
	CASE(OPiWRITE) {
		DBG_PL("OPiWRITE");
		sprintf(shared_buf, "%d", reg.idata[0]);
		outbuf += string(shared_buf);
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPsWRITE) {
		DBG_PL("OPsWRITE");
		outbuf += reg.sdata[0];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPoWRITE) {
		DBG_PL("OPoWRITE");
		//TODO: need typecheck
		sprintf(shared_buf, "%p", reg.pdata[0]);
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
		//GPerlObject *o = getFromVariableMemory(pc->dst);
		//o->data.idata = reg.idata[0];
		//o->data.pdata = reg.pdata[0];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPSET) {
		DBG_PL("OPSET");
		//setToVariableMemory(pc->name, pc->dst);
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
		GPerlVirtualMachineCode *code = getFromFuncMemory(pc->src);
		int res = run(code);
        callstack--;
		reg.idata[pc->dst] = res;
		//DBG_PL("res = [%d]", res);
		//DBG_PL("reg[%d] = [%d]", pc->dst, reg.idata[pc->dst]);
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPSHIFT) {
		DBG_PL("OPSHIFT");
		//reg.idata[0] = callstack->argstack[pc->src]->data.idata;
		//DBG_PL("reg[%d]%d = argstack[%d]", 0, reg.idata[0], pc->src);
		//reg.pdata[0] = callstack->argstack[pc->src]->data.pdata;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiPUSH) {
		DBG_PL("OPiPUSH");
		callstack++;//TODO : multiple arg
		callstack->argstack[pc->src]->data.idata = reg.idata[pc->dst];
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
		reg.idata[0] = reg.idata[pc->src];
		//DBG_PL("ret = [%d]", reg.idata[0]);
		return reg.idata[0];
	}
}
