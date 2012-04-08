#include <gperl.hpp>

using namespace std;

GPerlVirtualMachine::GPerlVirtualMachine(void)
{

}

#define L(op) L_##op
#define CASE(op) L(op) :
#define DISPATCH_START() goto *jmp_table[pc->op]
#define GOTO_NEXTOP() goto *(pc->opnext)

void GPerlVirtualMachine::createDirectThreadingCode(GPerlVirtualMachineCode *codes, void **jmp_tbl)
{
	codes->op = OPNOP;
	GPerlVirtualMachineCode *pc = codes;
	for (; pc->op != OPRET; pc++) {
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
		&&L(OPMOV), &&L(OPiMOV), &&L(OPsMOV), &&L(OPADD), &&L(OPiADD),
		&&L(OPSUB), &&L(OPiSUB), &&L(OPMUL), &&L(OPiMUL),
		&&L(OPRET), &&L(OPTHCODE), &&L(OPNOP),
		&&L(OPiWRITE), &&L(OPsWRITE), &&L(OPPRINT), /*&&L(OPSUB), &&L(OPMUL), &&L(OPDIV),
		&&L(OPCALL), &&L(OPJMP), &&L(OPCMP), &&L(OPPOP), &&L(OPPUSH),
		&&L(OPRET), &&L(OPJL), &&L(OPJG), &&L(OPSTORE), &&L(OPLOAD),
		&&L(OPiADDC), &&L(OPiSUBC), &&L(OPiJLC), &&L(OPiJGC), &&L(OPFASTCALL),
		&&L(OPiPUSHC), &&L(OPCOPY), &&L(OPTHCODE), &&L(OPNOP),*/
	};
	DISPATCH_START();

	CASE(OPMOV) {
		DBG_P("OPMOV");
		reg.idata[pc->dst] = pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiMOV) {
		DBG_P("OPiMOV");
		reg.idata[pc->dst] = pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPsMOV) {
		DBG_P("OPsMOV");
		reg.sdata[pc->dst] = (char *)pc->name;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPADD) {
		DBG_P("OPADD");
		reg.idata[pc->dst] += reg.idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiADD) {
		DBG_P("OPiADD");
		reg.idata[pc->dst] += reg.idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPSUB) {
		DBG_P("OPSUB");
		reg.idata[pc->dst] -= reg.idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiSUB) {
		DBG_P("OPiSUB");
		reg.idata[pc->dst] -= reg.idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPMUL) {
		DBG_P("OPMUL");
		reg.idata[pc->dst] *= reg.idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiMUL) {
		DBG_P("OPiMUL");
		reg.idata[pc->dst] *= reg.idata[pc->src];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPiWRITE) {
		DBG_P("OPiWRITE");
		sprintf(shared_buf, "%d", reg.idata[0]);
		outbuf += string(shared_buf);
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPsWRITE) {
		DBG_P("OPsWRITE");
		outbuf += reg.sdata[0];
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPPRINT) {
		DBG_P("OPPRINT");
		fprintf(stderr, "%s", cstr(outbuf));
		outbuf = "";
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPTHCODE) {
		DBG_P("OPTHCODE");
		createDirectThreadingCode(codes, jmp_table);
		return 0;
	}
	CASE(OPNOP) {
		DBG_P("OPNOP");
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPRET) {
		DBG_P("OPRET");
		reg.idata[0] = reg.idata[pc->src];
		DBG_P("ret = [%d]", reg.idata[0]);
		return reg.idata[0];
	}
}
