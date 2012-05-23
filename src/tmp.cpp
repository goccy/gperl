/* UNDEF OPERATION */
#define GPERL_UNDEF()

/* MOV OPERATION */
#define GPERL_MOV(dst, src) O(data)[dst] = O(data)[src] //NaN-boxing
#define GPERL_iMOV(dst, src) I(data)[dst] = I(data)[src]
#define GPERL_fMOV(dst, src) F(data)[dst] = F(data)[src]
#define GPERL_sMOV(dst, src) S(data)[dst] = S(data)[src]
#define GPERL_oMOV(dst, src) O(data)[dst] = O(data)[src]

/* ADD OPERATION */
#define GPERL_ADD(dst, src)
#define GPERL_iADD(dst, src) I(data)[dst] += I(data)[src]
#define GPERL_fADD(dst, src) F(data)[dst] += F(data)[src]
#define GPERL_sADD(dst, src)
#define GPERL_tADD(dst, src)

#define GPERL_iADDC(dst, src) I(data)[dst] += src
#define GPERL_fADDC(dst, src) F(data)[dst] += src
#define GPERL_sADDC(dst, src)

/* SUB OPERATION */
#define GPERL_SUB(dst, src)
#define GPERL_iSUB(dst, src) I(data)[dst] -= I(data)[src]
#define GPERL_fSUB(dst, src) F(data)[dst] -= F(data)[src]
#define GPERL_tSUB(dst, src)

#define GPERL_iSUBC(dst, src) I(data)[dst] -= src
#define GPERL_fSUBC(dst, src) F(data)[dst] -= src

/* MUL OPERATION */
#define GPERL_MUL(dst, src)
#define GPERL_iMUL(dst, src) I(data)[dst] *= I(data)[src]
#define GPERL_fMUL(dst, src) F(data)[dst] *= F(data)[src]
#define GPERL_sMUL(dst, src)
#define GPERL_tMUL(dst, src)

#define GPERL_iMULC(dst, src) I(data)[dst] *= src
#define GPERL_fMULC(dst, src) F(data)[dst] *= src
#define GPERL_sMULC(dst, src)

/* DIV OPERATION */
#define GPERL_DIV(dst, src)
#define GPERL_iDIV(dst, src) I(data)[dst] /= I(data)[src]
#define GPERL_fDIV(dst, src) F(data)[dst] /= F(data)[src]
#define GPERL_tDIV(dst, src)

#define GPERL_iDIVC(dst, src) I(data)[dst] /= src
#define GPERL_fDIVC(dst, src) F(data)[dst] /= src

/* CMP OPERATION */
#define GPERL_iCMP_JMP(op, dst, src) {				\
		if (I(data)[dst] op I(data)[src]) {			\
			pc++;									\
		} else {									\
			pc += pc->jmp;							\
		}											\
	}
#define GPERL_fCMP_JMP(op, dst, src) {				\
		if (F(data)[dst] op F(data)[src]) {			\
			pc++;									\
		} else {									\
			pc += pc->jmp;							\
		}											\
	}

#define GPERL_sCMP_JMP(op, dst, src) {				\
	}

#define GPERL_tCMP_JMP(op, dst, src) {			\
	}

#define GPERL_iCMP_JMPC(op, dst, src) {			\
		if (I(data)[dst] op src) {				\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}

#define GPERL_fCMP_JMPC(op, dst, src) {			\
		if (F(data)[dst] op src) {				\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}

#define GPERL_sCMP_JMPC(op, dst, src) {			\
	}

/* RET OPERATION */
#define GPERL_RET(src)
#define GPERL_iRET(src) return I(data)[src]
#define GPERL_fRET(src) return F(data)[src]
#define GPERL_sRET(src) return S(data)[src]
#define GPERL_tRET(src)

/* THREADED CODE OPERATION */
#define GPERL_THCODE() {												\
		createDirectThreadingCode(codes, jmp_table);					\
		#ifdef USING_SELECTIVE_INLINING									\
			createSelectiveInliningCode(codes, jmp_table, block_table);	\
		#else															\
			(void)block_table;											\
		#endif															\
		return 0;														\
	}

/* NOP OPERATION */
#define GPERL_NOP()

/* WRITE OPERATION */
#define GPERL_WRITE(dst)
#define GPERL_iWRITE(dst) {								\
		sprintf(shared_buf, "%d", I(data)[dst]);		\
		outbuf += string(shared_buf);					\
	}
#define GPERL_fWRITE(dst) {								\
		sprintf(shared_buf, "%f", F(data)[dst]);		\
		outbuf += string(shared_buf);					\
	}
#define GPERL_sWRITE(dst) outbuf += S(data)[dst];
#define GPERL_tWRITE(dst) {						\
	}

/* FRUSH OPERATION */
#define GPERL_FRUSH() {												\
		fprintf(stderr, "%s", cstr(outbuf));						\
		outbuf = "";												\
	}

/* JMP OPERATION */
#define GPERL_JMP() pc += pc->jmp

/* LET OPERATION */
#define GPERL_LET(dst, src) variable_memory[pc->dst]->value.pdata = O(data)[0]

/* SET OPERATION */
#define GPERL_SET(name, dst) setToVariableMemory(name, dst)
#define GPERL_FUNCSET(func, dst) setToFuncMemory(func, dst)

/* CALL OPERATION */
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

#define GPERL_fCALL(dst, src) {									\
		GPerlVirtualMachineCode *code = func_memory[src];		\
		top = code;												\
		callstack++;											\
		callstack->ret_addr = &&L_fCALLAFTER;					\
		callstack->pc = pc;										\
		reg++;													\
		pc = top;												\
		GOTO_NEXTOP();											\
	L_fCALLAFTER:												\
		pc = callstack->pc;										\
		float res = F(data)[0];									\
		callstack--;											\
		reg--;													\
		F(data)[dst] = res;										\
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

#define GPERL_tCALL(dst, src) {									\
		GPerlVirtualMachineCode *code = func_memory[src];		\
		top = code;												\
		callstack++;											\
		callstack->ret_addr = &&L_tCALLAFTER;					\
		callstack->pc = pc;										\
		reg++;													\
		pc = top;												\
		GOTO_NEXTOP();											\
	L_tCALLAFTER:												\
		pc = callstack->pc;										\
		void *res = O(data)[0];									\
		callstack--;											\
		reg--;													\
		O(data)[dst] = res;										\
	}

#define GPERL_SELFCALL(dst)
#define GPERL_iSELFCALL(dst) {						\
		callstack++;								\
		callstack->ret_addr = &&L_iJSELFCALLAFTER;	\
		callstack->pc = pc;							\
		reg++;										\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_iJSELFCALLAFTER:								\
		pc = callstack->pc;							\
		int res = I(data)[0];						\
		callstack--;								\
		reg--;										\
		I(data)[dst] = res;							\
	}

#define GPERL_fSELFCALL(dst) {						\
		callstack++;								\
		callstack->ret_addr = &&L_fJSELFCALLAFTER;	\
		callstack->pc = pc;							\
		reg++;										\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_fJSELFCALLAFTER:								\
		pc = callstack->pc;							\
		float res = F(data)[0];						\
		callstack--;								\
		reg--;										\
		F(data)[dst] = res;							\
	}

#define GPERL_sSELFCALL(dst) {						\
		callstack++;								\
		callstack->ret_addr = &&L_sJSELFCALLAFTER;	\
		callstack->pc = pc;							\
		reg++;										\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_sJSELFCALLAFTER:								\
		pc = callstack->pc;							\
		char *res = S(data)[0];						\
		callstack--;								\
		reg--;										\
		S(data)[dst] = res;							\
	}

#define GPERL_tSELFCALL(dst) {						\
		callstack++;								\
		callstack->ret_addr = &&L_tJSELFCALLAFTER;	\
		callstack->pc = pc;							\
		reg++;										\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_tJSELFCALLAFTER:								\
		pc = callstack->pc;							\
		void *res = O(data)[0];						\
		callstack--;								\
		reg--;										\
		O(data)[dst] = res;							\
	}

/* SHIFT OPERATION */
#define GPERL_SHIFT(src)
#define GPERL_iSHIFT(src) I(data)[0] = argstack[src]->value.idata;
#define GPERL_fSHIFT(src) F(data)[0] = argstack[src]->value.fdata;
#define GPERL_sSHIFT(src) S(data)[0] = argstack[src]->value.sdata;
#define GPERL_tSHIFT(src) O(data)[0] = argstack[src]->value.pdata;

/* PUSH OPERATION */
#define GPERL_PUSH(dst, src)
#define GPERL_iPUSH(dst, src) {											\
		(callstack+1)->argstack[src]->value.idata = I(data)[dst];		\
	}
#define GPERL_fPUSH(dst, src) {											\
		(callstack+1)->argstack[src]->value.fdata = F(data)[dst];		\
	}
#define GPERL_sPUSH(dst, src) {											\
		(callstack+1)->argstack[src]->value.sdata = S(data)[dst];		\
	}
#define GPERL_oPUSH(dst, src) {											\
		(callstack+1)->argstack[src]->value.pdata = O(data)[dst];		\
	}

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

	CASE(UNDEF), {
		GPERL_UNDEF();
		pc++;
		BREAK();
	});
	CASE(MOV), {
		GPERL_MOV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iMOV), {
		GPERL_iMOV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fMOV), {
		GPERL_fMOV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sMOV), {
		GPERL_sMOV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oMOV), {
		GPERL_oMOV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(ADD), {
		GPERL_ADD(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iADD), {
		GPERL_iADD(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fADD), {
		GPERL_fADD(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sADD), {
		GPERL_sADD(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(tADD), {
		GPERL_tADD(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iADDC), {
		GPERL_iADDC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fADDC), {
		GPERL_fADDC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sADDC), {
		GPERL_sADDC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(SUB), {
		GPERL_SUB(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iSUB), {
		GPERL_iSUB(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fSUB), {
		GPERL_fSUB(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sSUB), {
		GPERL_sSUB(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(tSUB), {
		GPERL_tSUB(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iSUBC), {
		GPERL_iSUBC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fSUBC), {
		GPERL_fSUBC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(MUL), {
		GPERL_MUL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iMUL), {
		GPERL_iMUL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fMUL), {
		GPERL_fMUL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sMUL), {
		GPERL_sMUL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(tMUL), {
		GPERL_tMUL(pc->dst, pc->dst);
		pc++;
		BREAK();
	});
	CASE(iMULC), {
		GPERL_iMULC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fMULC), {
		GPERL_fMULC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sMULC), {
		GPERL_sMULC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(DIV), {
		GPERL_DIV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iDIV), {
		GPERL_iDIV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fDIV), {
		GPERL_fDIV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sDIV), {
		GPERL_sDIV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(tDIV), {
		GPERL_tDIV(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iDIVC), {
		GPERL_iDIVC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fDIVC), {
		GPERL_fDIVC(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(JG), {
		GPERL_tCMP_JMP(>, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJG), {
		GPERL_iCMP_JMP(>, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fJG), {
		GPERL_fCMP_JMP(>, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sJG), {
		GPERL_sCMP_JMP(>, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oJG), {
		GPERL_tCMP_JMP(>, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJGC), {
		GPERL_iCMP_JMPC(>, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fJGC), {
		GPERL_fCMP_JMPC(>, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sJGC), {
		GPERL_sCMP_JMPC(>, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(JL), {
		GPERL_tCMP_JMP(<, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJL), {
		GPERL_iCMP_JMP(<, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fJL), {
		GPERL_fCMP_JMP(<, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sJL), {
		GPERL_sCMP_JMP(<, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oJL), {
		GPERL_tCMP_JMP(<, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJLC), {
		GPERL_iCMP_JMPC(<, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fJLC), {
		GPERL_fCMP_JMPC(<, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sJLC), {
		GPERL_sCMP_JMPC(<, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(JE), {
		GPERL_tCMP_JMP(==, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJE), {
		GPERL_iCMP_JMP(==, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fJE), {
		GPERL_fCMP_JMP(==, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sJE), {
		GPERL_sCMP_JMP(==, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oJE), {
		GPERL_tCMP_JMP(==, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJEC), {
		GPERL_iCMP_JMPC(==, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fJEC), {
		GPERL_fCMP_JMPC(==, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sJEC), {
		GPERL_sCMP_JMPC(==, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(JNE), {
		GPERL_tCMP_JMP(!=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJNE), {
		GPERL_iCMP_JMP(!=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fJNE), {
		GPERL_fCMP_JMP(!=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sJNE), {
		GPERL_sCMP_JMP(!=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oJNE), {
		GPERL_tCMP_JMP(!=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iJNEC), {
		GPERL_iCMP_JMPC(!=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fJNEC), {
		GPERL_fCMP_JMPC(!=, pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(RET), {
		GPERL_RET(pc->src);
		pc++;
		BREAK();
	});
	CASE(iRET), {
		GPERL_iRET(pc->src);
		pc++;
		BREAK();
	});
	CASE(fRET), {
		GPERL_fRET(pc->src);
		pc++;
		BREAK();
	});
	CASE(sRET), {
		GPERL_sRET(pc->src);
		pc++;
		BREAK();
	});
	CASE(tRET), {
		GPERL_tRET(pc->src);
		pc++;
		BREAK();
	});
	CASE(THCODE), {
		GPERL_THCODE();
		pc++;
		BREAK();
	});
	CASE(NOP), {
		GPERL_NOP();
		pc++;
		BREAK();
	});
	CASE(WRITE), {
		GPERL_WRITE(pc->dst);
		pc++;
		BREAK();
	});
	CASE(iWRITE), {
		GPERL_iWRITE(pc->dst);
		pc++;
		BREAK();
	});
	CASE(fWRITE), {
		GPERL_fWRITE(pc->dst);
		pc++;
		BREAK();
	});
	CASE(sWRITE), {
		GPERL_sWRITE(pc->dst);
		pc++;
		BREAK();
	});
	CASE(tWRITE), {
		GPERL_tWRITE(pc->dst);
		pc++;
		BREAK();
	});
	CASE(FRUSH), {
		GPERL_FRUSH();
		pc++;
		BREAK();
	});
	CASE(JMP), {
		GPERL_JMP();
		pc++;
		BREAK();
	});
	CASE(LET), {
		GPERL_LET(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(SET), {
		GPERL_SET(pc->name, pc->dst);
		pc++;
		BREAK();
	});
	CASE(FUNCSET), {
		GPERL_FUNCSET(pc->func, pc->dst);
		pc++;
		BREAK();
	});
	CASE(CALL), {
		GPERL_CALL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iCALL), {
		GPERL_iCALL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fCALL), {
		GPERL_fCALL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sCALL), {
		GPERL_sCALL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(tCALL), {
		GPERL_tCALL(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(SELFCALL), {
		GPERL_SELFCALL(pc->dst);
		pc++;
		BREAK();
	});
	CASE(iSELFCALL), {
		GPERL_iSELFCALL(pc->dst);
		pc++;
		BREAK();
	});
	CASE(fSELFCALL), {
		GPERL_fSELFCALL(pc->dst);
		pc++;
		BREAK();
	});
	CASE(sSELFCALL), {
		GPERL_sSELFCALL(pc->dst);
		pc++;
		BREAK();
	});
	CASE(tSELFCALL), {
		GPERL_tSELFCALL(pc->dst);
		pc++;
		BREAK();
	});
	CASE(SHIFT), {
		GPERL_SHIFT(pc->src);
		pc++;
		BREAK();
	});
	CASE(iSHIFT), {
		GPERL_iSHIFT(pc->src);
		pc++;
		BREAK();
	});
	CASE(fSHIFT), {
		GPERL_fSHIFT(pc->src);
		pc++;
		BREAK();
	});
	CASE(sSHIFT), {
		GPERL_SHIFT(pc->src);
		pc++;
		BREAK();
	});
	CASE(tSHIFT), {
		GPERL_tSHIFT(pc->src);
		pc++;
		BREAK();
	});
	CASE(PUSH), {
		GPERL_PUSH(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(iPUSH), {
		GPERL_iPUSH(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(fPUSH), {
		GPERL_fPUSH(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(sPUSH), {
		GPERL_sPUSH(pc->dst, pc->src);
		pc++;
		BREAK();
	});
	CASE(oPUSH), {
		GPERL_oPUSH(pc->dst, pc->src);
		pc++;
		BREAK();
	});
#include "gen_fast_vmcode.cpp"

	DISPATCH_END();
	return I(data)[0];
}
