#include <gperl.hpp>


GPerlVirtualMachine::GPerlVirtualMachine(void)
{

}

#define L(op) L_##op
#define CASE(op) L(op) :
#define DISPATCH_START() goto *jmp_table[pc->op]
#define GOTO_NEXTOP() goto *(pc->opnext)
#define MAX_REG_SIZE 32

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
	int reg[MAX_REG_SIZE] = {0};
	static void *jmp_table[] = {
		&&L(OPMOV), &&L(OPADD), &&L(OPRET), &&L(OPTHCODE), &&L(OPNOP),/*&&L(OPSUB), &&L(OPMUL), &&L(OPDIV),
		&&L(OPCALL), &&L(OPJMP), &&L(OPCMP), &&L(OPPOP), &&L(OPPUSH),
		&&L(OPRET), &&L(OPJL), &&L(OPJG), &&L(OPSTORE), &&L(OPLOAD),
		&&L(OPiADDC), &&L(OPiSUBC), &&L(OPiJLC), &&L(OPiJGC), &&L(OPFASTCALL),
		&&L(OPiPUSHC), &&L(OPCOPY), &&L(OPTHCODE), &&L(OPNOP),*/
	};
	DISPATCH_START();

	CASE(OPMOV) {
		DBG_P("OPMOV");
		reg[pc->dst] = pc->src;
		pc++;
		GOTO_NEXTOP();
	}
	CASE(OPADD) {
		DBG_P("OPADD");
		reg[pc->dst] += reg[pc->src];
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
		reg[0] = reg[pc->src];
		DBG_P("ret = [%d]", reg[0]);
		return reg[0];
	}
}
