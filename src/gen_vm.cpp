#include <gperl.hpp>
#include "gen_decl_code.cpp"

using namespace std;

#define L(op) L_##op
#define DISPATCH_START() {						\
		callstack->ret_addr = &&L_RETURN;		\
		(callstack+1)->ret_addr = &&L_RETURN;	\
		goto *jmp_table[pc->op];				\
	}

#define DISPATCH_END() L_RETURN:
#define GOTO_NEXTOP() goto *(pc->opnext)

#define B(op) {&&L(op##_START), &&L(op##_END)}
#define START(op) L_##op##_START:
#define END(op) L_##op##_END:

#define BREAK() GOTO_NEXTOP()
#define _CASE(op) L(op) : { DBG_PL(#op); }
#define CASE(op, block) _CASE(op) { START(op); block; END(op); }
#define RETURN() { goto *callstack->ret_addr; }

#define NaN       (uint64_t)(0xFFF0000000000000)
#define Mask      (uint64_t)(0x00000000FFFFFFFF)
#define TYPE      (uint64_t)(0x000F000000000000)

#define IntTag    (uint64_t)(0x0001000000000000)
#define StringTag (uint64_t)(0x0002000000000000)
#define ObjectTag (uint64_t)(0x0003000000000000)

#define I(data) reg->ivalue
#define D(data) reg->dvalue
#define S(data) reg->svalue
#define O(data) reg->ovalue

#define INT_init(o) (GPerlValue)((o & Mask) | NaN | IntTag)
#define DOUBLE_init(o) (GPerlValue)(o)
#define STRING_init(o, ptr) o.o = ptr; o.bytes |= NaN | StringTag
#define OBJECT_init(o, ptr) o.o = ptr; o.bytes |= NaN | ObjectTag
#define getString(o) (o.bytes ^ (NaN | StringTag))
#define getObject(o) (o.bytes ^ (NaN | ObjectTag))
#define TYPE_CHECK(T) ((T.bytes & NaN) == NaN) * ((T.bytes & TYPE) >> 48 )

#define GPERL_UNDEF()

#define GPERL_MOV(dst, src)
#define GPERL_iMOV(dst, src) I(data)[dst] = I(data)[src]
#define GPERL_dMOV(dst, src) D(data)[dst] = D(data)[src]
#define GPERL_sMOV(dst, src) S(data)[dst] = S(data)[src]
#define GPERL_oMOV(dst, src) O(data)[dst] = O(data)[src]

#define GPERL_ADD(dst, src)
#define GPERL_iADD(dst, src) I(data)[dst] += I(data)[src]
#define GPERL_dADD(dst, src) D(data)[dst] += D(data)[src]
#define GPERL_sADD(dst, src)
#define GPERL_oADD(dst, src)
#define GPERL_iADDC(dst, src) I(data)[dst] += src
#define GPERL_dADDC(dst, src) D(data)[dst] += src
#define GPERL_sADDC(dst, src)

#define GPERL_SUB(dst, src)
#define GPERL_iSUB(dst, src) I(data)[dst] -= I(data)[src]
#define GPERL_dSUB(dst, src) D(data)[dst] -= D(data)[src]
#define GPERL_oSUB(dst, src)
#define GPERL_iSUBC(dst, src) I(data)[dst] -= src
#define GPERL_dSUBC(dst, src) D(data)[dst] -= src

#define GPERL_MUL(dst, src)
#define GPERL_iMUL(dst, src) I(data)[dst] *= I(data)[src]
#define GPERL_dMUL(dst, src) D(data)[dst] *= D(data)[src]
#define GPERL_sMUL(dst, src)
#define GPERL_oMUL(dst, src)
#define GPERL_iMULC(dst, src) I(data)[dst] *= src
#define GPERL_dMULC(dst, src) D(data)[dst] *= src
#define GPERL_sMULC(dst, src)

#define GPERL_DIV(dst, src)
#define GPERL_iDIV(dst, src) I(data)[dst] /= I(data)[src]
#define GPERL_dDIV(dst, src) D(data)[dst] /= D(data)[src]
#define GPERL_oDIV(dst, src)
#define GPERL_iDIVC(dst, src) I(data)[dst] /= src
#define GPERL_dDIVC(dst, src) D(data)[dst] /= src

#define GPERL_CMP_JMP(op, dst, src)
#define GPERL_iCMP_JMP(op, dst, src) {				\
		if (I(data)[dst] op I(data)[src]) {			\
			pc++;									\
		} else {									\
			pc += pc->jmp;							\
		}											\
	}
#define GPERL_dCMP_JMP(op, dst, src) {				\
		if (D(data)[dst] op D(data)[src]) {			\
			pc++;									\
		} else {									\
			pc += pc->jmp;							\
		}											\
	}
#define GPERL_sCMP_JMP(op, dst, src) {}
#define GPERL_oCMP_JMP(op, dst, src) {}
#define GPERL_CMP_JMPC(op, dst, src)
#define GPERL_iCMP_JMPC(op, dst, src) {			\
		if (I(data)[dst] op src) {				\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}
#define GPERL_dCMP_JMPC(op, dst, src) {			\
		if (D(data)[dst] op src) {				\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}
#define GPERL_sCMP_JMPC(op, dst, src) {}
#define GPERL_oCMP_JMPC(op, dst, src) {}

#define GPERL_RET(src)
#define GPERL_iRET(src) return I(data)[src]
#define GPERL_dRET(src) return (int)D(data)[src]
#define GPERL_sRET(src) return (int)S(data)[src]
#define GPERL_oRET(src) return (int)O(data)[src]

#define GPERL_NOP()

#define GPERL_WRITE(dst)
#define GPERL_iWRITE(dst) {								\
		sprintf(shared_buf, "%d", I(data)[dst]);		\
		outbuf += string(shared_buf);					\
	}
#define GPERL_dWRITE(dst) {								\
		sprintf(shared_buf, "%f", D(data)[dst]);		\
		outbuf += string(shared_buf);					\
	}
#define GPERL_sWRITE(dst) outbuf += S(data)[dst]
#define GPERL_oWRITE(dst)

#define GPERL_FRUSH() {												\
		fprintf(stderr, "%s", cstr(outbuf));						\
		outbuf = "";												\
	}
#define GPERL_JMP() pc += pc->jmp
#define GPERL_LET(dst, src) variable_memory[dst]->value.ovalue = O(data)[0]
#define GPERL_FUNCSET(func, dst) setToFuncMemory(func, dst)
#define GPERL_SET(name, dst) setToVariableMemory(name, dst)

#define GPERL_CALL(dst, src)
#define GPERL_iCALL(dst, src) {									\
		GPerlVirtualMachineCode *code = func_memory[src];		\
		top = code;												\
		callstack++;											\
		callstack->ret_addr = &&L_iCALLAFTER;					\
		callstack->pc = pc;										\
		reg++;													\
		pc = top;												\
		GOTO_NEXTOP();											\
	L_iCALLAFTER:												\
		pc = callstack->pc;										\
		int res = I(data)[0];									\
		callstack--;											\
		reg--;													\
		I(data)[dst] = res;										\
	}

#define GPERL_dCALL(dst, src) {									\
		GPerlVirtualMachineCode *code = func_memory[src];		\
		top = code;												\
		callstack++;											\
		callstack->ret_addr = &&L_dCALLAFTER;					\
		callstack->pc = pc;										\
		reg++;													\
		pc = top;												\
		GOTO_NEXTOP();											\
	L_dCALLAFTER:												\
		pc = callstack->pc;										\
		double res = D(data)[0];								\
		callstack--;											\
		reg--;													\
		D(data)[dst] = res;										\
	}

#define GPERL_sCALL(dst, src) {									\
		GPerlVirtualMachineCode *code = func_memory[src];		\
		top = code;												\
		callstack++;											\
		callstack->ret_addr = &&L_sCALLAFTER;					\
		callstack->pc = pc;										\
		reg++;													\
		pc = top;												\
		GOTO_NEXTOP();											\
	L_sCALLAFTER:												\
		pc = callstack->pc;										\
		char *res = S(data)[0];									\
		callstack--;											\
		reg--;													\
		S(data)[dst] = res;										\
	}

#define GPERL_oCALL(dst, src) {									\
		GPerlVirtualMachineCode *code = func_memory[src];		\
		top = code;												\
		callstack++;											\
		callstack->ret_addr = &&L_oCALLAFTER;					\
		callstack->pc = pc;										\
		reg++;													\
		pc = top;												\
		GOTO_NEXTOP();											\
	L_oCALLAFTER:												\
		pc = callstack->pc;										\
		GPerlObject *res = (GPerlObject *)O(data)[0];			\
		callstack--;											\
		reg--;													\
		O(data)[dst] = res;										\
	}

#define GPERL_SELFCALL(dst)
#define GPERL_iSELFCALL(dst) {						\
		callstack++;								\
		callstack->ret_addr = &&L_iSELFCALLAFTER;	\
		callstack->pc = pc;							\
		reg++;										\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_iSELFCALLAFTER:								\
		pc = callstack->pc;							\
		int res = I(data)[0];						\
		callstack--;								\
		reg--;										\
		I(data)[dst] = res;							\
	}

#define GPERL_dSELFCALL(dst) {						\
		callstack++;								\
		callstack->ret_addr = &&L_dSELFCALLAFTER;	\
		callstack->pc = pc;							\
		reg++;										\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_dSELFCALLAFTER:								\
		pc = callstack->pc;							\
		double res = D(data)[0];					\
		callstack--;								\
		reg--;										\
		D(data)[dst] = res;							\
	}

#define GPERL_sSELFCALL(dst) {						\
		callstack++;								\
		callstack->ret_addr = &&L_sSELFCALLAFTER;	\
		callstack->pc = pc;							\
		reg++;										\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_sSELFCALLAFTER:								\
		pc = callstack->pc;							\
		char *res = S(data)[0];						\
		callstack--;								\
		reg--;										\
		S(data)[dst] = res;							\
	}

#define GPERL_oSELFCALL(dst) {						\
		callstack++;								\
		callstack->ret_addr = &&L_oSELFCALLAFTER;	\
		callstack->pc = pc;							\
		reg++;										\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_oSELFCALLAFTER:								\
		pc = callstack->pc;							\
		GPerlObject *res = (GPerlObject *)O(data)[0];	\
		callstack--;								\
		reg--;										\
		O(data)[dst] = res;							\
	}

#define GPERL_SHIFT(src)
#define GPERL_iSHIFT(src) I(data)[0] = argstack[src]->value.ivalue
#define GPERL_dSHIFT(src) D(data)[0] = argstack[src]->value.dvalue
#define GPERL_sSHIFT(src) S(data)[0] = argstack[src]->value.svalue
#define GPERL_oSHIFT(src) O(data)[0] = (GPerlObject *)argstack[src]->value.ovalue
#define GPERL_PUSH(dst, src)
#define GPERL_iPUSH(dst, src) {											\
		(callstack+1)->argstack[src]->value.ivalue = I(data)[dst];		\
	}
#define GPERL_dPUSH(dst, src) {											\
		(callstack+1)->argstack[src]->value.dvalue = D(data)[dst];		\
	}
#define GPERL_sPUSH(dst, src) {											\
		(callstack+1)->argstack[src]->value.svalue = S(data)[dst];		\
	}
#define GPERL_oPUSH(dst, src) {											\
		(callstack+1)->argstack[src]->value.ovalue = O(data)[dst];		\
	}
#define GPERL_NEW()

int GPerlVirtualMachine::run(GPerlVirtualMachineCode *codes)
{
	static GPerlVirtualMachineCode *top;
	GPerlVirtualMachineCode *pc = codes;
	Reg reg_[MAX_CALLSTACK_SIZE];
	Reg *reg = reg_;
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
		GPERL_sMOV(pc->dst, pc->src);
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
#ifdef USING_SELECTIVE_INLINING
		createSelectiveInliningCode(codes, jmp_table, block_table);
#else
		(void)block_table;
#endif
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
	CASE(FRUSH, {
		GPERL_FRUSH();
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
		GPERL_SET(pc->name, pc->dst);
		pc++;
		BREAK();
	});
	CASE(FUNCSET, {
		GPERL_FUNCSET(pc->func, pc->dst);
		pc++;
		BREAK();
	});
	CASE(CALL, {
		GPERL_CALL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iCALL, {
		GPERL_iCALL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(dCALL, {
		GPERL_dCALL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sCALL, {
		GPERL_sCALL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oCALL, {
		GPERL_oCALL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(SELFCALL, {
		GPERL_SELFCALL(pc->dst);
		pc++;
		BREAK();
	});
	CASE(iSELFCALL, {
		GPERL_iSELFCALL(pc->dst);
		pc++;
		BREAK();
	});
	CASE(dSELFCALL, {
		GPERL_dSELFCALL(pc->dst);
		pc++;
		BREAK();
	});
	CASE(sSELFCALL, {
		GPERL_sSELFCALL(pc->dst);
		pc++;
		BREAK();
	});
	CASE(oSELFCALL, {
		GPERL_oSELFCALL(pc->dst);
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
	return I(data)[0];
}
