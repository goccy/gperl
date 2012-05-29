
#include <gperl.hpp>
#include "gen_decl_code.cpp"
#include <vmlibs.hpp>

using namespace std;

int GPerlVirtualMachine::run(GPerlVirtualMachineCode *codes)
{
	static GPerlVirtualMachineCode *top;
	GPerlVirtualMachineCode *pc = codes;
	GPerlEnv *callstack = createCallStack();
	GPerlObject **argstack = createArgStack();
	static char shared_buf[128] = {0};//TODO must be variable buffer
	static string outbuf = "";
    GPerlValue stack[MAX_STACK_MEMORY_SIZE] = {0};
    int esp = 0;
    int ebp = 0;
#include "gen_label.cpp"

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
		GPERL_dADDC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iADDC, {
		GPERL_iADDC(pc->dst, pc->src);
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
		GPERL_dSUBC(pc->dst, pc->src);
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
		GPERL_dMULC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iMULC, {
		GPERL_iMULC(pc->dst, pc->src);
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
		GPERL_dDIVC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iDIVC, {
		GPERL_iDIVC(pc->dst, pc->src);
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
	CASE(RET, {
		GPERL_RET(pc->src);
		pc++;
		BREAK();
	});
	CASE(THCODE, {
		createDirectThreadingCode(codes, jmp_table);
		(void)block_table;
		//createSelectiveInliningCode(codes, jmp_table, block_table);
		return 0;
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
	CASE(ARRAY_PUSH, {
		GPERL_ARRAY_PUSH(callstack->argstack);
		pc++;
		BREAK();
	});
#include "gen_fast_vmcode.cpp"

	DISPATCH_END();
	return I(0);
}
