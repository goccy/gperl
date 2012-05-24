
#include <gperl.hpp>
#include "gen_decl_code.cpp"
#include <vmlibs.hpp>

using namespace std;

int GPerlVirtualMachine::run(GPerlVirtualMachineCode *codes)
{
	static GPerlVirtualMachineCode *top;
	GPerlVirtualMachineCode *pc = codes;
	//Reg reg_[MAX_CALLSTACK_SIZE];
	//Reg *reg = reg_;
	GPerlEnv *callstack = createCallStack();
	GPerlObject **argstack = createArgStack();
	static char shared_buf[128] = {0};//TODO must be variable buffer
	static string outbuf = "";

#include "gen_label.cpp"

    DISPATCH_START();

	CASE(UNDEF, {
		GPERL_UNDEF();
		pc++;
		BREAK();
	});
	CASE(MOV, {
		GPERL_MOV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iMOV, {
		GPERL_iMOV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dMOV, {
		GPERL_dMOV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sMOV, {
		GPERL_sMOV(pc->dst, pc->name);
		pc++;
		BREAK();
	});
	CASE(oMOV, {
		GPERL_oMOV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(ADD, {
		GPERL_ADD(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iADD, {
		GPERL_iADD(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dADD, {
		GPERL_dADD(pc->dst, pc->src);
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
	CASE(iADDC, {
		GPERL_iADDC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dADDC, {
		GPERL_dADDC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(SUB, {
		GPERL_SUB(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iSUB, {
		GPERL_iSUB(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dSUB, {
		GPERL_dSUB(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oSUB, {
		GPERL_oSUB(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iSUBC, {
		GPERL_iSUBC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dSUBC, {
		GPERL_dSUBC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(MUL, {
		GPERL_MUL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iMUL, {
		GPERL_iMUL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dMUL, {
		GPERL_dMUL(pc->dst, pc->src);
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
	CASE(iMULC, {
		GPERL_iMULC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dMULC, {
		GPERL_dMULC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(DIV, {
		GPERL_DIV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iDIV, {
		GPERL_iDIV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dDIV, {
		GPERL_dDIV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oDIV, {
		GPERL_oDIV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iDIVC, {
		GPERL_iDIVC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dDIVC, {
		GPERL_dDIVC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(JG, {
		GPERL_CMP_JMP(>, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJG, {
		GPERL_iCMP_JMP(>, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dJG, {
		GPERL_dCMP_JMP(>, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sJG, {
		GPERL_sCMP_JMP(>, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oJG, {
		GPERL_oCMP_JMP(>, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJGC, {
		GPERL_iCMP_JMPC(>, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dJGC, {
		GPERL_dCMP_JMPC(>, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(JL, {
		GPERL_CMP_JMP(<, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJL, {
		GPERL_iCMP_JMP(<, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dJL, {
		GPERL_dCMP_JMP(<, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sJL, {
		GPERL_sCMP_JMP(<, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oJL, {
		GPERL_oCMP_JMP(<, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJLC, {
		GPERL_iCMP_JMPC(<, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dJLC, {
		GPERL_dCMP_JMPC(<, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(JGE, {
		GPERL_CMP_JMP(>=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJGE, {
		GPERL_iCMP_JMP(>=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dJGE, {
		GPERL_dCMP_JMP(>=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sJGE, {
		GPERL_sCMP_JMP(>=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oJGE, {
		GPERL_oCMP_JMP(>=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJGEC, {
		GPERL_iCMP_JMPC(>=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dJGEC, {
		GPERL_dCMP_JMPC(>=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(JLE, {
		GPERL_CMP_JMP(<=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJLE, {
		GPERL_iCMP_JMP(<=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dJLE, {
		GPERL_dCMP_JMP(<=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sJLE, {
		GPERL_sCMP_JMP(<=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oJLE, {
		GPERL_oCMP_JMP(<=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJLEC, {
		GPERL_iCMP_JMPC(<=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dJLEC, {
		GPERL_dCMP_JMPC(<=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(JE, {
		GPERL_CMP_JMP(==, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJE, {
		GPERL_iCMP_JMP(==, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dJE, {
		GPERL_dCMP_JMP(==, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sJE, {
		GPERL_sCMP_JMP(==, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oJE, {
		GPERL_oCMP_JMP(==, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJEC, {
		GPERL_iCMP_JMPC(==, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dJEC, {
		GPERL_dCMP_JMPC(==, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(JNE, {
		GPERL_CMP_JMP(!=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJNE, {
		GPERL_iCMP_JMP(!=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dJNE, {
		GPERL_dCMP_JMP(!=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sJNE, {
		GPERL_sCMP_JMP(!=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oJNE, {
		GPERL_oCMP_JMP(!=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJNEC, {
		GPERL_iCMP_JMPC(!=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dJNEC, {
		GPERL_dCMP_JMPC(!=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(RET, {
		GPERL_RET(pc->src);
		pc++;
		BREAK();
	});
	CASE(iRET, {
		GPERL_iRET(pc->src);
		pc++;
		BREAK();
	});
	CASE(dRET, {
		GPERL_dRET(pc->src);
		pc++;
		BREAK();
	});
	CASE(sRET, {
		GPERL_sRET(pc->src);
		pc++;
		BREAK();
	});
	CASE(oRET, {
		GPERL_oRET(pc->src);
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
		GPERL_WRITE(pc->dst);
		pc++;
		BREAK();
	});
	CASE(iWRITE, {
		GPERL_iWRITE(pc->dst);
		pc++;
		BREAK();
	});
	CASE(dWRITE, {
		GPERL_dWRITE(pc->dst);
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
	CASE(iCALL, {
		GPERL_iCALL(pc->dst, pc->src, iCALL);
		pc++;
		BREAK();
	});
	CASE(dCALL, {
		GPERL_dCALL(pc->dst, pc->src, dCALL);
		pc++;
		BREAK();
	});
	CASE(sCALL, {
		GPERL_sCALL(pc->dst, pc->src, sCALL);
		pc++;
		BREAK();
	});
	CASE(oCALL, {
		GPERL_oCALL(pc->dst, pc->src, oCALL);
		pc++;
		BREAK();
	});
	CASE(SELFCALL, {
		GPERL_SELFCALL(pc->dst, SELFCALL);
		pc++;
		BREAK();
	});
	CASE(iSELFCALL, {
		GPERL_iSELFCALL(pc->dst, iSELFCALL);
		pc++;
		BREAK();
	});
	CASE(dSELFCALL, {
		GPERL_dSELFCALL(pc->dst, dSELFCALL);
		pc++;
		BREAK();
	});
	CASE(sSELFCALL, {
		GPERL_sSELFCALL(pc->dst, sSELFCALL);
		pc++;
		BREAK();
	});
	CASE(oSELFCALL, {
		GPERL_oSELFCALL(pc->dst, oSELFCALL);
		pc++;
		BREAK();
	});
	CASE(SHIFT, {
		GPERL_SHIFT(pc->src);
		pc++;
		BREAK();
	});
	CASE(iSHIFT, {
		GPERL_iSHIFT(pc->src);
		pc++;
		BREAK();
	});
	CASE(dSHIFT, {
		GPERL_dSHIFT(pc->src);
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
	CASE(iPUSH, {
		GPERL_iPUSH(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dPUSH, {
		GPERL_dPUSH(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sPUSH, {
		GPERL_sPUSH(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oPUSH, {
		GPERL_oPUSH(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(NEW, {
		GPERL_NEW();
		pc++;
		BREAK();
	});
#include "gen_fast_vmcode.cpp"

	DISPATCH_END();
	return I(0);
}
