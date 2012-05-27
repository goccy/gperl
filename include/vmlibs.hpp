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

#define I(idx) callstack->reg[idx].ivalue
#define D(idx) callstack->reg[idx].dvalue
#define S(idx) callstack->reg[idx].svalue
#define O(idx) callstack->reg[idx].ovalue

#define GPERL_UNDEF()

#define GPERL_MOV(dst, v) callstack->reg[dst] = v
#define GPERL_ARGMOV(dst, src) callstack->reg[dst] = callstack->argstack[src]

#define GPERL_iADD(dst, src) I(dst) += I(src)
#define GPERL_dADD(dst, src) D(dst) += D(src)
#define GPERL_sADD(dst, src)
#define GPERL_oADD(dst, src)
#define GPERL_ADDC(dst, src)
#define GPERL_iADDC(dst, src) I(dst) += src
#define GPERL_dADDC(dst, src) D(dst) += src
#define GPERL_sADDC(dst, src)

#define GPERL_iSUB(dst, src) I(dst) -= I(src)
#define GPERL_dSUB(dst, src) D(dst) -= D(src)
#define GPERL_oSUB(dst, src)
#define GPERL_SUBC(dst, src)
#define GPERL_iSUBC(dst, src) I(dst) -= src
#define GPERL_dSUBC(dst, src) D(dst) -= src

#define GPERL_iMUL(dst, src) I(dst) *= I(src)
#define GPERL_dMUL(dst, src) D(dst) *= D(src)
#define GPERL_sMUL(dst, src)
#define GPERL_oMUL(dst, src)
#define GPERL_MULC(dst, src)
#define GPERL_iMULC(dst, src) I(dst) *= src
#define GPERL_dMULC(dst, src) D(dst) *= src
#define GPERL_sMULC(dst, src)

#define GPERL_iDIV(dst, src) I(dst) /= I(src)
#define GPERL_dDIV(dst, src) D(dst) /= D(src)
#define GPERL_oDIV(dst, src)
#define GPERL_DIVC(dst, src)
#define GPERL_iDIVC(dst, src) I(dst) /= src
#define GPERL_dDIVC(dst, src) D(dst) /= src

#define GPERL_iCMP_JMP(op, dst, src) {				\
		if (I(dst) op I(src)) {						\
			pc++;									\
		} else {									\
			pc += pc->jmp;							\
		}											\
	}
#define GPERL_dCMP_JMP(op, dst, src) {				\
		if (D(dst) op D(src)) {						\
			pc++;									\
		} else {									\
			pc += pc->jmp;							\
		}											\
	}
#define GPERL_sCMP_JMP(op, dst, src) {}
#define GPERL_oCMP_JMP(op, dst, src) {}
#define GPERL_CMP_JMPC(op, dst, src)
#define GPERL_iCMP_JMPC(op, dst, src) {			\
		if (I(dst) op src) {					\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}
#define GPERL_dCMP_JMPC(op, dst, src) {			\
		if (D(dst) op src) {					\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}
#define GPERL_sCMP_JMPC(op, dst, src) {}
#define GPERL_oCMP_JMPC(op, dst, src) {}

#define GPERL_RET(src) callstack->reg[0] = callstack->reg[src]; RETURN();
#define GPERL_NOP()

#define GPERL_WRITE(dst)
#define GPERL_iWRITE(dst) {								\
		sprintf(shared_buf, "%d", I(dst));				\
		outbuf += string(shared_buf);					\
	}
#define GPERL_dWRITE(dst) {								\
		sprintf(shared_buf, "%f", D(dst));				\
		outbuf += string(shared_buf);					\
	}
#define GPERL_sWRITE(dst) outbuf += string(getString(callstack->reg[dst]))
#define GPERL_oWRITE(dst)

#define GPERL_FLUSH() {												\
		fprintf(stderr, "%s", cstr(outbuf));						\
		outbuf = "";												\
	}
#define GPERL_JMP() pc += pc->jmp
#define GPERL_LET(dst, src) variable_memory[dst]->value.ovalue = O(0)
#define GPERL_FUNCSET(func, dst) setToFuncMemory(func, dst)
#define GPERL_SETv(name, dst) setToVariableMemory(name, dst)

#define GPERL_CALL(dst, src, NAME) {							\
		GPerlVirtualMachineCode *code = func_memory[src];		\
		top = code;												\
		callstack++;											\
		callstack->ret_addr = &&L_##NAME##AFTER;				\
		callstack->pc = pc;										\
		pc = top;												\
		GOTO_NEXTOP();											\
	L_##NAME##AFTER:											\
		pc = callstack->pc;										\
		(callstack-1)->reg[dst] = callstack->reg[0];			\
		callstack--;											\
	}

#define GPERL_SELFCALL(dst, NAME) {					\
		callstack++;								\
		callstack->ret_addr = &&L_##NAME##AFTER;	\
		callstack->pc = pc;							\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_##NAME##AFTER:								\
		pc = callstack->pc;							\
		(callstack-1)->reg[dst] = callstack->reg[0];			\
		callstack--;											\
	}

#define GPERL_SHIFT(src)
#define GPERL_iSHIFT(src) I(0) = argstack[src]->value.ivalue
#define GPERL_dSHIFT(src) D(0) = argstack[src]->value.dvalue
#define GPERL_sSHIFT(src) S(0) = argstack[src]->value.svalue
#define GPERL_oSHIFT(src) O(0) = (GPerlObject *)argstack[src]->value.ovalue
#define GPERL_PUSH(dst, src) (callstack+1)->argstack[src] = callstack->reg[dst]
#define GPERL_NEW()
