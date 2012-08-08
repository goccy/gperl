#include <gperl.hpp>
#include "gen_decl_code.cpp"
#include <vmlibs.hpp>

using namespace std;

char shared_buf[128] = {0};//TODO must be variable buffer
string outbuf = "";

GPerlValue GPerlVirtualMachine::run(GPerlVirtualMachineCode *codes)
{
	static GPerlVirtualMachineCode *top;
	GPerlVirtualMachineCode *pc = codes, *code_ = NULL;
	GPerlEnv *callstack = createCallStack();
	GPerlValue *stack = createMachineStack();
	int esp = 0;
	int ebp = 0;
	root.stack_bottom = stack;
	root.callstack_bottom = callstack;
	root.global_vmemory = global_vmemory;
#ifdef USING_JIT
	GPerlJITCompiler jit_compiler;
	GPerlJITEnv jit_env;
	jit_env.callstack = callstack;
	jit_env.stack = stack;
	jit_env.args = args;
#endif

#include "gen_label.cpp"
	if (sigsetjmp(expand_mem, 1)) {
		DBG_PL("GC");
		pc = callstack->cur_pc;
		callstack->reg_top = pc->cur_reg_top;
		root.callstack_top = callstack;
		root.stack_top_idx = pc->cur_stack_top;
		mm->gc();
		if (mm->freeList == mm->guard) {
			mm->expandMemPool();
		}
	}
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
		int dst_type = TYPE_CHECK(callstack->reg[pc->dst]);
		int src_type = TYPE_CHECK(callstack->reg[pc->src]);
#ifdef STATIC_TYPING_MODE
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#else /* DYNAMIC_TYPING_MODE */
		goto *jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#endif
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
		int dst_type = TYPE_CHECK(callstack->reg[pc->dst]);
		int src_type = TYPE_CHECK(callstack->reg[pc->src]);
#ifdef STATIC_TYPING_MODE
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#else /* DYNAMIC_TYPING_MODE */
		goto *jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#endif
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
		int dst_type = TYPE_CHECK(callstack->reg[pc->dst]);
		int src_type = TYPE_CHECK(callstack->reg[pc->src]);
#ifdef STATIC_TYPING_MODE
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#else /* DYNAMIC_TYPING_MODE */
		goto *jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#endif
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
		int dst_type = TYPE_CHECK(callstack->reg[pc->dst]);
		int src_type = TYPE_CHECK(callstack->reg[pc->src]);
#ifdef STATIC_TYPING_MODE
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#else /* DYNAMIC_TYPING_MODE */
		goto *jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#endif
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
	CASE(JG, {
		int dst_type = TYPE_CHECK(callstack->reg[pc->dst]);
		int src_type = TYPE_CHECK(callstack->reg[pc->src]);
#ifdef STATIC_TYPING_MODE
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#else /* DYNAMIC_TYPING_MODE */
		goto *jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#endif
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
		int dst_type = TYPE_CHECK(callstack->reg[pc->dst]);
		int src_type = TYPE_CHECK(callstack->reg[pc->src]);
#ifdef STATIC_TYPING_MODE
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#else /* DYNAMIC_TYPING_MODE */
		goto *jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#endif
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
		int dst_type = TYPE_CHECK(callstack->reg[pc->dst]);
		int src_type = TYPE_CHECK(callstack->reg[pc->src]);
#ifdef STATIC_TYPING_MODE
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#else /* DYNAMIC_TYPING_MODE */
		goto *jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#endif
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
		int dst_type = TYPE_CHECK(callstack->reg[pc->dst]);
		int src_type = TYPE_CHECK(callstack->reg[pc->src]);
#ifdef STATIC_TYPING_MODE
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#else /* DYNAMIC_TYPING_MODE */
		goto *jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#endif
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
		int dst_type = TYPE_CHECK(callstack->reg[pc->dst]);
		int src_type = TYPE_CHECK(callstack->reg[pc->src]);
#ifdef STATIC_TYPING_MODE
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#else /* DYNAMIC_TYPING_MODE */
		goto *jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#endif
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
		int dst_type = TYPE_CHECK(callstack->reg[pc->dst]);
		int src_type = TYPE_CHECK(callstack->reg[pc->src]);
#ifdef STATIC_TYPING_MODE
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#else /* DYNAMIC_TYPING_MODE */
		goto *jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#endif
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
	CASE(StringADD, {
		GPERL_StringADD(pc->dst, pc->v);
		pc++;
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
		int type = TYPE_CHECK(stack[ebp + pc->dst]);
#ifdef STATIC_TYPING_MODE
		pc->opnext = jmp_table[pc->op + 1 + type];
#else /* DYNAMIC_TYPING_MODE */
		goto *jmp_table[pc->op + 1 + type];
#endif
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
#ifdef STATIC_TYPING_MODE
		pc->opnext = jmp_table[pc->op + 1 + type];
#else /* DYNAMIC_TYPING_MODE */
		goto *jmp_table[pc->op + 1 + type];
#endif
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
		int type = TYPE_CHECK(stack[ebp + pc->dst]);
#ifdef STATIC_TYPING_MODE
		pc->opnext = jmp_table[pc->op + 1 + type];
#else /* DYNAMIC_TYPING_MODE */
		goto *jmp_table[pc->op + 1 + type];
#endif
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
#ifdef STATIC_TYPING_MODE
		pc->opnext = jmp_table[pc->op + 1 + type];
#else /* DYNAMIC_TYPING_MODE */
		goto *jmp_table[pc->op + 1 + type];
#endif
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
	CASE(THCODE, {
		createDirectThreadingCode(codes, jmp_table);
		(void)block_table;
		//createSelectiveInliningCode(codes, jmp_table, block_table);
		return callstack->reg[0];
	});
	CASE(NOP, {
		GPERL_NOP();
		pc++;
		BREAK();
	});
	CASE(WRITE, {
		int type = TYPE_CHECK(callstack->reg[pc->dst]);
#ifdef STATIC_TYPING_MODE
		pc->opnext = jmp_table[pc->op + 1 + type];
#else /* DYNAMIC_TYPING_MODE */
		goto *jmp_table[pc->op + 1 + type];
#endif
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
	CASE(JMP, {
		GPERL_JMP();
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
	CASE(CALL, {
		GPERL_CALL(pc->dst, pc->src, CALL);
		pc++;
		BREAK();
	});
	CASE(SELFCALL, {
		GPERL_SELFCALL(pc->dst, SELFCALL);
		pc++;
		BREAK();
	});
	CASE(SHIFT, {
		GPERL_SHIFT(pc->src);
		pc++;
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
	CASE(ARRAY_gAT, {
		GPERL_ARRAY_gAT(pc->dst, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(ARRAY_DREF, {
		GPERL_ARRAY_DREF(pc->dst, pc->src);
		pc++;
		BREAK();
	});
#include "gen_fast_vmcode.cpp"

	DISPATCH_END();
	return callstack->reg[0];
}
