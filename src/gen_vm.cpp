#include <gperl.hpp>
#include "gen_decl_code.cpp"
#include <vmlibs.hpp>

using namespace std;
bool isRunFinished = false;
static void *jitTimingCheck(void *args)
{
	DBG_PL("jitTimingCheck");
	JITParams *params = (JITParams *)args;
	void **jmp_tbl = params->jmp_table;
	size_t params_num = params->params_num;
	JITParam **prms = params->params;
	while (!isRunFinished) {
		for (size_t i = 0; i < params_num; i++) {
			JITParam *param = prms[i];
			if (!param->func && param->mtd->jit_count_down == 0 && param->return_type != Return) {
				//DBG_PL("Start JIT Compile");
				GPerlJITCompiler jit_compiler;
				param->func = jit_compiler.compile(param);
				size_t calls_num = param->calls_num;
				GPerlVirtualMachineCode *mtd = param->mtd;
				for (size_t j = 0; j < calls_num; j++) {
					int offset = param->offsets[j];
					switch ((mtd + offset)->op) {
					case CALL: case FASTCALL0: case FASTCALL1: case FASTCALL2: case FASTCALL3:
						(mtd + offset)->opnext = jmp_tbl[JIT_CALL];
						break;
					case SELFCALL:
						(mtd + offset)->opnext = jmp_tbl[JIT_SELFCALL];
						break;
					case SELF_FASTCALL0:
						(mtd + offset)->opnext = jmp_tbl[JIT_SELF_FASTCALL0];
						break;
					case SELF_FASTCALL1:
						(mtd + offset)->opnext = jmp_tbl[JIT_SELF_FASTCALL1];
						break;
					case SELF_FASTCALL2:
						(mtd + offset)->opnext = jmp_tbl[JIT_SELF_FASTCALL2];
						break;
					case SELF_FASTCALL3:
						(mtd + offset)->opnext = jmp_tbl[JIT_SELF_FASTCALL3];
						break;
					default:
						break;
					}
				}
			}
		}
	}
    return NULL;
}

GPerlValue GPerlVirtualMachine::run(GPerlVirtualMachineCode *codes, JITParams *params)
{
	GPerlVirtualMachineCode *pc = codes, *code_ = NULL, *top = NULL;
	GPerlValue *stack = createMachineStack();
	GPerlEnv *callstack = createCallStack();
	callstack->pc = pc;
	callstack->cur_pc = pc;
	(callstack+1)->cur_pc = pc;
	GPerlValue *argstack = callstack->argstack;
	GPerlValue *reg = callstack->reg;
	callstack->ebp = stack;
	GPerlEnv *callstack_bottom = callstack;
	char shared_buf[128] = {0};//TODO must be variable buffer
	string outbuf = "";
	root.stack_bottom = stack;
	root.callstack_bottom = callstack_bottom;
	root.global_vmemory = global_vmemory;
#include "gen_label.cpp"
#ifdef ENABLE_JIT_COMPILE
	GPerlJITCompiler jit_compiler;
	if (params && params->params_num > 0) {
		this->params = params;
		pthread_t th;
		params->jmp_table = jmp_table;
		pthread_create(&th, NULL, jitTimingCheck, (void *)params);
	}
#endif
    DISPATCH_START();

	CASE(UNDEF, {
		GPERL_UNDEF();
		pc++;
		BREAK();
	});
	CASE(MOV, {
		GPERL_MOV(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(vMOV, {
		GPERL_vMOV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(gMOV, {
		GPERL_gMOV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(ARGMOV, {
		GPERL_ARGMOV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(ArrayARGMOV, {
		GPERL_ArrayARGMOV(pc->dst);
		pc++;
		BREAK();
	});
	CASE(ADD, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		goto *jmp_table[pc->op + 2 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(STATIC_ADD, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(dADD, {
		GPERL_dADD(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iADD, {
		GPERL_iADD(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sADD, {
		GPERL_sADD(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oADD, {
		GPERL_oADD(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dADDC, {
		GPERL_dADDC(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(iADDC, {
		GPERL_iADDC(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(SUB, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		goto *jmp_table[pc->op + 2 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(STATIC_SUB, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(dSUB, {
		GPERL_dSUB(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iSUB, {
		GPERL_iSUB(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oSUB, {
		GPERL_oSUB(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dSUBC, {
		GPERL_dSUBC(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(iSUBC, {
		GPERL_iSUBC(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(MUL, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		goto *jmp_table[pc->op + 2 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(STATIC_MUL, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(dMUL, {
		GPERL_dMUL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iMUL, {
		GPERL_iMUL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sMUL, {
		GPERL_sMUL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oMUL, {
		GPERL_oMUL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dMULC, {
		GPERL_dMULC(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(iMULC, {
		GPERL_iMULC(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(DIV, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		goto *jmp_table[pc->op + 2 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(STATIC_DIV, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(dDIV, {
		GPERL_dDIV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iDIV, {
		GPERL_iDIV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oDIV, {
		GPERL_oDIV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dDIVC, {
		GPERL_dDIVC(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(iDIVC, {
		GPERL_iDIVC(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(LSHIFT, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		goto *jmp_table[pc->op + 2 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(STATIC_LSHIFT, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(dLSHIFT, {
		GPERL_dLSHIFT(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iLSHIFT, {
		GPERL_iLSHIFT(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sLSHIFT, {
		GPERL_sLSHIFT(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oLSHIFT, {
		GPERL_oLSHIFT(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dLSHIFTC, {
		GPERL_dLSHIFTC(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(iLSHIFTC, {
		GPERL_iLSHIFTC(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(RSHIFT, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		goto *jmp_table[pc->op + 2 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(STATIC_RSHIFT, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(dRSHIFT, {
		GPERL_dRSHIFT(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iRSHIFT, {
		GPERL_iRSHIFT(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sRSHIFT, {
		GPERL_sRSHIFT(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oRSHIFT, {
		GPERL_oRSHIFT(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dRSHIFTC, {
		GPERL_dRSHIFTC(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(iRSHIFTC, {
		GPERL_iRSHIFTC(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(JG, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		goto *jmp_table[pc->op + 2 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(STATIC_JG, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(dJG, {
		GPERL_dCMP_JMP(>, pc->dst, pc->src);
		BREAK();
	});
	CASE(iJG, {
		GPERL_iCMP_JMP(>, pc->dst, pc->src);
		BREAK();
	});
	CASE(sJG, {
		GPERL_sCMP_JMP(>, pc->dst, pc->src);
		BREAK();
	});
	CASE(oJG, {
		GPERL_oCMP_JMP(>, pc->dst, pc->src);
		BREAK();
	});
	CASE(dJGC, {
		GPERL_dCMP_JMPC(>, pc->dst, pc->v);
		BREAK();
	});
	CASE(iJGC, {
		GPERL_iCMP_JMPC(>, pc->dst, pc->v);
		BREAK();
	});
	CASE(JL, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		goto *jmp_table[pc->op + 2 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(STATIC_JL, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(dJL, {
		GPERL_dCMP_JMP(<, pc->dst, pc->src);
		BREAK();
	});
	CASE(iJL, {
		GPERL_iCMP_JMP(<, pc->dst, pc->src);
		BREAK();
	});
	CASE(sJL, {
		GPERL_sCMP_JMP(<, pc->dst, pc->src);
		BREAK();
	});
	CASE(oJL, {
		GPERL_oCMP_JMP(<, pc->dst, pc->src);
		BREAK();
	});
	CASE(dJLC, {
		GPERL_dCMP_JMPC(<, pc->dst, pc->v);
		BREAK();
	});
	CASE(iJLC, {
		GPERL_iCMP_JMPC(<, pc->dst, pc->v);
		BREAK();
	});
	CASE(JGE, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		goto *jmp_table[pc->op + 2 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(STATIC_JGE, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(dJGE, {
		GPERL_dCMP_JMP(>=, pc->dst, pc->src);
		BREAK();
	});
	CASE(iJGE, {
		GPERL_iCMP_JMP(>=, pc->dst, pc->src);
		BREAK();
	});
	CASE(sJGE, {
		GPERL_sCMP_JMP(>=, pc->dst, pc->src);
		BREAK();
	});
	CASE(oJGE, {
		GPERL_oCMP_JMP(>=, pc->dst, pc->src);
		BREAK();
	});
	CASE(dJGEC, {
		GPERL_dCMP_JMPC(>=, pc->dst, pc->v);
		BREAK();
	});
	CASE(iJGEC, {
		GPERL_iCMP_JMPC(>=, pc->dst, pc->v);
		BREAK();
	});
	CASE(JLE, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		goto *jmp_table[pc->op + 2 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(STATIC_JLE, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(dJLE, {
		GPERL_dCMP_JMP(<=, pc->dst, pc->src);
		BREAK();
	});
	CASE(iJLE, {
		GPERL_iCMP_JMP(<=, pc->dst, pc->src);
		BREAK();
	});
	CASE(sJLE, {
		GPERL_sCMP_JMP(<=, pc->dst, pc->src);
		BREAK();
	});
	CASE(oJLE, {
		GPERL_oCMP_JMP(<=, pc->dst, pc->src);
		BREAK();
	});
	CASE(dJLEC, {
		GPERL_dCMP_JMPC(<=, pc->dst, pc->v);
		BREAK();
	});
	CASE(iJLEC, {
		GPERL_iCMP_JMPC(<=, pc->dst, pc->v);
		BREAK();
	});
	CASE(JE, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		goto *jmp_table[pc->op + 2 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(STATIC_JE, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(dJE, {
		GPERL_dCMP_JMP(==, pc->dst, pc->src);
		BREAK();
	});
	CASE(iJE, {
		GPERL_iCMP_JMP(==, pc->dst, pc->src);
		BREAK();
	});
	CASE(sJE, {
		GPERL_sCMP_JMP(==, pc->dst, pc->src);
		BREAK();
	});
	CASE(oJE, {
		GPERL_oCMP_JMP(==, pc->dst, pc->src);
		BREAK();
	});
	CASE(dJEC, {
		GPERL_dCMP_JMPC(==, pc->dst, pc->v);
		BREAK();
	});
	CASE(iJEC, {
		GPERL_iCMP_JMPC(==, pc->dst, pc->v);
		BREAK();
	});
	CASE(JNE, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		goto *jmp_table[pc->op + 2 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(STATIC_JNE, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(dJNE, {
		GPERL_dCMP_JMP(!=, pc->dst, pc->src);
		BREAK();
	});
	CASE(iJNE, {
		GPERL_iCMP_JMP(!=, pc->dst, pc->src);
		BREAK();
	});
	CASE(sJNE, {
		GPERL_sCMP_JMP(!=, pc->dst, pc->src);
		BREAK();
	});
	CASE(oJNE, {
		GPERL_oCMP_JMP(!=, pc->dst, pc->src);
		BREAK();
	});
	CASE(dJNEC, {
		GPERL_dCMP_JMPC(!=, pc->dst, pc->v);
		BREAK();
	});
	CASE(iJNEC, {
		GPERL_iCMP_JMPC(!=, pc->dst, pc->v);
		BREAK();
	});
	CASE(IS, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(STATIC_IS, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(dIS, {
		GPERL_dIS(pc->dst, pc->v);
		BREAK();
	});
	CASE(iIS, {
		GPERL_iIS(pc->dst, pc->v);
		BREAK();
	});
	CASE(sIS, {
		GPERL_sIS(pc->dst, pc->v);
		BREAK();
	});
	CASE(oIS, {
		GPERL_oIS(pc->dst, pc->v);
		BREAK();
	});
	CASE(dISC, {
		GPERL_dISC(pc->dst, pc->v);
		BREAK();
	});
	CASE(iISC, {
		GPERL_iISC(pc->dst, pc->v);
		BREAK();
	});
	CASE(ISNOT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(STATIC_ISNOT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(dISNOT, {
		GPERL_dISNOT(pc->dst, pc->v);
		BREAK();
	});
	CASE(iISNOT, {
		GPERL_iISNOT(pc->dst, pc->v);
		BREAK();
	});
	CASE(sISNOT, {
		GPERL_sISNOT(pc->dst, pc->v);
		BREAK();
	});
	CASE(oISNOT, {
		GPERL_oISNOT(pc->dst, pc->v);
		BREAK();
	});
	CASE(dISNOTC, {
		GPERL_dISNOTC(pc->dst, pc->v);
		BREAK();
	});
	CASE(iISNOTC, {
		GPERL_iISNOTC(pc->dst, pc->v);
		BREAK();
	});
	CASE(StringADD, {
		GPERL_StringADD(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(STATIC_StringADD, {
		int dst_type = TYPE_CHECK(reg[pc->dst]);
		int src_type = TYPE_CHECK(reg[pc->src]);
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
		BREAK();
	});
	CASE(dStringADD, {
		GPERL_dStringADD(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(iStringADD, {
		GPERL_iStringADD(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(sStringADD, {
		GPERL_sStringADD(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(oStringADD, {
		GPERL_oStringADD(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(dStringADDC, {
		GPERL_dStringADDC(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(iStringADDC, {
		GPERL_iStringADDC(pc->dst, pc->v);
		pc++;
		BREAK();
	});
	CASE(INC, {
		int type = TYPE_CHECK(stack[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(STATIC_INC, {
		int type = TYPE_CHECK(stack[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(dINC, {
		GPERL_dINC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(iINC, {
		GPERL_iINC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(sINC, {
		GPERL_sINC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(oINC, {
		GPERL_oINC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(gINC, {
		int type = TYPE_CHECK(global_vmemory[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(STATIC_gINC, {
		int type = TYPE_CHECK(global_vmemory[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(dgINC, {
		GPERL_dgINC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(igINC, {
		GPERL_igINC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(sgINC, {
		GPERL_sgINC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(ogINC, {
		GPERL_ogINC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(DEC, {
		int type = TYPE_CHECK(stack[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(STATIC_DEC, {
		int type = TYPE_CHECK(stack[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(dDEC, {
		GPERL_dDEC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(iDEC, {
		GPERL_iDEC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(sDEC, {
		GPERL_sDEC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(oDEC, {
		GPERL_oDEC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(gDEC, {
		int type = TYPE_CHECK(global_vmemory[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(STATIC_gDEC, {
		int type = TYPE_CHECK(global_vmemory[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(dgDEC, {
		GPERL_dgDEC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(igDEC, {
		GPERL_igDEC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(sgDEC, {
		GPERL_sgDEC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(ogDEC, {
		GPERL_ogDEC(pc->dst);
		pc++;
		BREAK();
	});
	CASE(RET, {
		GPERL_RET(pc->src);
		pc++;
		BREAK();
	});
	CASE(JIT_COUNTDOWN_RET, {
		GPERL_JIT_COUNTDOWN_RET(pc->src);
		pc++;
		BREAK();
	});
	CASE(NOP, {
		GPERL_NOP();
		pc++;
		BREAK();
	});
	CASE(WRITE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(STATIC_WRITE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(dWRITE, {
		GPERL_dWRITE(pc->dst);
		pc++;
		BREAK();
	});
	CASE(iWRITE, {
		GPERL_iWRITE(pc->dst);
		pc++;
		BREAK();
	});
	CASE(sWRITE, {
		GPERL_sWRITE(pc->dst);
		pc++;
		BREAK();
	});
	CASE(oWRITE, {
		GPERL_oWRITE(pc->dst);
		pc++;
		BREAK();
	});
	CASE(FLUSH, {
		GPERL_FLUSH();
		pc++;
		BREAK();
	});
	CASE(REF, {
		GPERL_REF((callstack+1)->argstack[0]);
		pc++;
		BREAK();
	});
	CASE(BLESS, {
		GPERL_BLESS((callstack+1)->argstack);
		pc++;
		BREAK();
	});
	CASE(JMP, {
		GPERL_JMP();
		pc++;
		BREAK();
	});
	CASE(LET, {
		GPERL_LET(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(gLET, {
		GPERL_gLET(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(MLET, {
		GPERL_MLET(pc->dst, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(gMLET, {
		GPERL_gMLET(pc->dst, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(SETv, {
		GPERL_SETv(pc->name, pc->dst);
		pc++;
		BREAK();
	});
	CASE(FUNCSET, {
		GPERL_FUNCSET(pc->func, pc->dst);
		pc++;
		BREAK();
	});
	CASE(CLOSURE, {
		GPERL_CLOSURE(pc->dst, pc->src, CLOSURE);
		pc++;
		BREAK();
	});
	CASE(STATIC_CALL, {
		GPERL_STATIC_CALL(pc->dst, pc->src, STATIC_CALL);
		pc++;
		BREAK();
	});
	CASE(DYNAMIC_CALL, {
		GPERL_DYNAMIC_CALL(pc->dst, pc->src, DYNAMIC_CALL);
		pc++;
		BREAK();
	});
	CASE(CALL, {
		GPERL_CALL(pc->dst, pc->src, CALL);
		pc++;
		BREAK();
	});
	CASE(JIT_CALL, {
		GPERL_JIT_CALL(pc->dst, pc->src, JIT_CALL);
		pc++;
		BREAK();
	});
	CASE(JIT_COUNTDOWN_CALL, {
		GPERL_JIT_COUNTDOWN_CALL(pc->dst, pc->src, JIT_COUNTDOWN_CALL);
		pc++;
		BREAK();
	});
	CASE(FASTCALL0, {
		GPERL_FASTCALL0(pc->arg0, pc->dst, pc->src, FASTCALL0);
		pc++;
		BREAK();
	});
	CASE(JIT_COUNTDOWN_FASTCALL0, {
		GPERL_JIT_COUNTDOWN_FASTCALL0(pc->arg0, pc->dst, pc->src, JIT_COUNTDOWN_FASTCALL0);
		pc++;
		BREAK();
	});
	CASE(FASTCALL1, {
		GPERL_FASTCALL1(pc->arg0, pc->arg1, pc->dst, pc->src, FASTCALL1);
		pc++;
		BREAK();
	});
	CASE(JIT_COUNTDOWN_FASTCALL1, {
		GPERL_JIT_COUNTDOWN_FASTCALL1(pc->arg0, pc->arg1, pc->dst, pc->src, JIT_COUNTDOWN_FASTCALL1);
		pc++;
		BREAK();
	});
	CASE(FASTCALL2, {
		GPERL_FASTCALL2(pc->arg0, pc->arg1, pc->arg2, pc->dst, pc->src, FASTCALL2);
		pc++;
		BREAK();
	});
	CASE(JIT_COUNTDOWN_FASTCALL2, {
		GPERL_JIT_COUNTDOWN_FASTCALL2(pc->arg0, pc->arg1, pc->arg2, pc->dst, pc->src, JIT_COUNTDOWN_FASTCALL2);
		pc++;
		BREAK();
	});
	CASE(FASTCALL3, {
		GPERL_FASTCALL3(pc->arg0, pc->arg1, pc->arg2, pc->arg3, pc->dst, pc->src, FASTCALL3);
		pc++;
		BREAK();
	});
	CASE(JIT_COUNTDOWN_FASTCALL3, {
		GPERL_JIT_COUNTDOWN_FASTCALL3(pc->arg0, pc->arg1, pc->arg2, pc->arg3, pc->dst, pc->src, JIT_COUNTDOWN_FASTCALL3);
		pc++;
		BREAK();
	});
	CASE(SELFCALL, {
		GPERL_SELFCALL(pc->dst, SELFCALL);
		pc++;
		BREAK();
	});
	CASE(JIT_SELFCALL, {
		GPERL_JIT_SELFCALL(pc->dst, JIT_SELFCALL);
		pc++;
		BREAK();
	});
	CASE(JIT_COUNTDOWN_SELFCALL, {
		GPERL_JIT_COUNTDOWN_SELFCALL(pc->dst, JIT_COUNTDOWN_SELFCALL);
		pc++;
		BREAK();
	});
	CASE(SELF_FASTCALL0, {
		GPERL_SELF_FASTCALL0(pc->arg0, pc->dst, SELF_FASTCALL0);
		pc++;
		BREAK();
	});
	CASE(JIT_SELF_FASTCALL0, {
		GPERL_JIT_SELF_FASTCALL0(pc->arg0, pc->dst, JIT_SELF_FASTCALL0);
		pc++;
		BREAK();
	});
	CASE(JIT_COUNTDOWN_SELF_FASTCALL0, {
		GPERL_JIT_COUNTDOWN_SELF_FASTCALL0(pc->arg0, pc->dst, JIT_COUNTDOWN_SELF_FASTCALL0);
		pc++;
		BREAK();
	});
	CASE(SELF_FASTCALL1, {
		GPERL_SELF_FASTCALL1(pc->arg0, pc->arg1, pc->dst, SELF_FASTCALL1);
		pc++;
		BREAK();
	});
	CASE(JIT_SELF_FASTCALL1, {
		GPERL_JIT_SELF_FASTCALL1(pc->arg0, pc->arg1, pc->dst, JIT_SELF_FASTCALL1);
		pc++;
		BREAK();
	});
	CASE(JIT_COUNTDOWN_SELF_FASTCALL1, {
		GPERL_JIT_COUNTDOWN_SELF_FASTCALL1(pc->arg0, pc->arg1, pc->dst, JIT_COUNTDOWN_SELF_FASTCALL1);
		pc++;
		BREAK();
	});
	CASE(SELF_FASTCALL2, {
		GPERL_SELF_FASTCALL2(pc->arg0, pc->arg1, pc->arg2, pc->dst, SELF_FASTCALL2);
		pc++;
		BREAK();
	});
	CASE(JIT_SELF_FASTCALL2, {
		GPERL_JIT_SELF_FASTCALL2(pc->arg0, pc->arg1, pc->arg2, pc->dst, JIT_SELF_FASTCALL2);
		pc++;
		BREAK();
	});
	CASE(JIT_COUNTDOWN_SELF_FASTCALL2, {
		GPERL_JIT_COUNTDOWN_SELF_FASTCALL2(pc->arg0, pc->arg1, pc->arg2, pc->dst, JIT_COUNTDOWN_SELF_FASTCALL2);
		pc++;
		BREAK();
	});
	CASE(SELF_FASTCALL3, {
		GPERL_SELF_FASTCALL3(pc->arg0, pc->arg1, pc->arg2, pc->arg3, pc->dst, SELF_FASTCALL3);
		pc++;
		BREAK();
	});
	CASE(JIT_SELF_FASTCALL3, {
		GPERL_JIT_SELF_FASTCALL3(pc->arg0, pc->arg1, pc->arg2, pc->arg3, pc->dst, JIT_SELF_FASTCALL3);
		pc++;
		BREAK();
	});
	CASE(JIT_COUNTDOWN_SELF_FASTCALL3, {
		GPERL_JIT_COUNTDOWN_SELF_FASTCALL3(pc->arg0, pc->arg1, pc->arg2, pc->arg3, pc->dst, JIT_COUNTDOWN_SELF_FASTCALL3);
		pc++;
		BREAK();
	});
	CASE(SHIFT, {
		GPERL_SHIFT(pc->src);
		pc++;
		BREAK();
	});
	CASE(STATIC_SHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(dSHIFT, {
		GPERL_dSHIFT(pc->src);
		pc++;
		BREAK();
	});
	CASE(iSHIFT, {
		GPERL_iSHIFT(pc->src);
		pc++;
		BREAK();
	});
	CASE(sSHIFT, {
		GPERL_sSHIFT(pc->src);
		pc++;
		BREAK();
	});
	CASE(oSHIFT, {
		GPERL_oSHIFT(pc->src);
		pc++;
		BREAK();
	});
	CASE(PUSH, {
		GPERL_PUSH(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(aPUSH, {
		GPERL_aPUSH(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(NEW, {
		GPERL_NEW();
		pc++;
		BREAK();
	});
	CASE(NEW_STRING, {
		GPERL_NEW_STRING();
		pc++;
		BREAK();
	});
	CASE(ARRAY_PUSH, {
		GPERL_ARRAY_PUSH((callstack+1)->argstack);
		pc++;
		BREAK();
	});
	CASE(ARRAY_AT, {
		GPERL_ARRAY_AT(pc->dst, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(ARRAY_ARGAT, {
		GPERL_ARRAY_ARGAT(pc->dst, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(ARRAY_ATC, {
		GPERL_ARRAY_ATC(pc->dst, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(ARRAY_gAT, {
		GPERL_ARRAY_gAT(pc->dst, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(ARRAY_gATC, {
		GPERL_ARRAY_gATC(pc->dst, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(ARRAY_SET, {
		GPERL_ARRAY_SET(pc->dst, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(ARRAY_gSET, {
		GPERL_ARRAY_gSET(pc->dst, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(ARRAY_DREF, {
		GPERL_ARRAY_DREF(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(HASH_AT, {
		GPERL_HASH_AT(pc->dst, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(HASH_ATC, {
		GPERL_HASH_ATC(pc->dst, pc->src, pc->hash);
		pc++;
		BREAK();
	});
	CASE(HASH_gAT, {
		GPERL_HASH_gAT(pc->dst, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(HASH_gATC, {
		GPERL_HASH_gATC(pc->dst, pc->src, pc->hash);
		pc++;
		BREAK();
	});
	CASE(HASH_SET, {
		GPERL_HASH_SET(pc->dst, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(HASH_gSET, {
		GPERL_HASH_gSET(pc->dst, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(HASH_DREF, {
		GPERL_HASH_DREF(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(CODE_REF, {
		GPERL_CODE_REF(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(EACH_INIT, {
		GPERL_EACH_INIT(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(EACH_gINIT, {
		GPERL_EACH_gINIT(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(THCODE, {
		createDirectThreadingCode(codes, jmp_table);
		size_t pkgs_n = pkgs->size();
		for (size_t i = 0; i < pkgs_n; i++) {
			GPerlClass *gclass = (GPerlClass *)pkgs->at(i);
			GPerlString **mtd_names = gclass->ext->mtd_names;
			for (size_t j = 0; mtd_names[j] != NULL; j++) {
				GPerlFunc *mtd = gclass->mtds[mtd_names[j]->hash];
				createDirectThreadingCode(mtd->code, jmp_table);
			}
		}
		(void)block_table;
		//createSelectiveInliningCode(codes, jmp_table, block_table);
		return reg[0];
	});
	CASE(KEYS, {
		GPerlArray *a = GPERL_KEYS((callstack+1)->argstack[0]);
		OBJECT_init(reg[pc->dst], a);
		pc++;
		BREAK();
	});
	CASE(VALUES, {
		GPerlArray *a = GPERL_VALUES((callstack+1)->argstack[0]);
		OBJECT_init(reg[pc->dst], a);
		pc++;
		BREAK();
	});
	CASE(EACH_LET, {
		GPERL_EACH_LET(pc->dst, pc->src);
		BREAK();
	});
	CASE(EACH_STEP, {
		GPERL_EACH_STEP(pc->dst, pc->src);
		BREAK();
	});

#include "gen_fast_vmcode.cpp"

	DISPATCH_END();
	return reg[0];
}
