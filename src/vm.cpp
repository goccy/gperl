#include <gperl.hpp>

using namespace std;

GPerlVirtualMachine::GPerlVirtualMachine(void)
{
	//Set DEFAULT Object
	for (int i = 0; i < MAX_ARGSTACK_SIZE; i++) {
		GPerlObject *o = new GPerlObject();
		argstack[i] = o;
	}
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
		&&L(OPADD), &&L(OPiADD),
		&&L(OPSUB), &&L(OPiSUB),
		&&L(OPMUL), &&L(OPiMUL),
		&&L(OPDIV), &&L(OPiDIV),
		&&L(OPJG), &&L(OPiJG),
		&&L(OPJL), &&L(OPiJL),
		&&L(OPJGE), &&L(OPiJGE),
		&&L(OPJLE), &&L(OPiJLE),
		&&L(OPJE), &&L(OPiJE),
		&&L(OPJNE), &&L(OPiJNE),
		&&L(OPRET), &&L(OPTHCODE), &&L(OPNOP),
		&&L(OPiWRITE), &&L(OPsWRITE), &&L(OPoWRITE),
		&&L(OPPRINT), &&L(OPJMP), &&L(OPLET),
		&&L(OPSET), &&L(OPFUNCSET),
		&&L(OPCALL), &&L(OPSHIFT),
		&&L(OPiPUSH), &&L(OPsPUSH),
	};
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
		reg.pdata[pc->dst] = getFromVariableMemory(pc->src)->data.pdata;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPOiMOV) {
		DBG_PL("OPOiMOV");
		DBG_PL("idata = [%d]", getFromVariableMemory(pc->src)->data.idata);
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
	CASE(OPSUB) {
		DBG_PL("OPSUB");
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

		if (reg.idata[pc->dst] == reg.idata[pc->src]) {
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
		GPerlObject *o = getFromVariableMemory(pc->dst);
		o->data.pdata = reg.pdata[0];
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
		GPerlVirtualMachineCode *code = getFromFuncMemory(pc->src);
		int res = run(code);
		reg.idata[pc->dst] = res;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPSHIFT) {
		DBG_PL("OPSHIFT");
		reg.pdata[0] = argstack[pc->src]->data.pdata;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiPUSH) {
		DBG_PL("OPiPUSH");
		DBG_PL("idata = [%d]", reg.idata[0]);
		argstack[pc->src]->data.idata = reg.idata[pc->dst];
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
		DBG_PL("ret = [%d]", reg.idata[0]);
		return reg.idata[0];
	}
}
