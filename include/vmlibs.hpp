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

#define I(idx) callstack->reg[idx].ivalue
#define D(idx) callstack->reg[idx].dvalue
#define S(idx) callstack->reg[idx].svalue
#define O(idx) callstack->reg[idx].ovalue

#define INT_init(o) (GPerlValue)((o & Mask) | NaN | IntTag)
#define DOUBLE_init(o) (GPerlValue)(o)
#define STRING_init(o, ptr) o.o = ptr; o.bytes |= NaN | StringTag
#define OBJECT_init(o, ptr) o.o = ptr; o.bytes |= NaN | ObjectTag
#define getString(o) (o.bytes ^ (NaN | StringTag))
#define getObject(o) (o.bytes ^ (NaN | ObjectTag))
#define TYPE_CHECK(T) ((T.bytes & NaN) == NaN) * ((T.bytes & TYPE) >> 48 )

#define GPERL_UNDEF()

#define GPERL_MOV(dst, src)
#define GPERL_iMOV(dst, src) I(dst) = I(src)
#define GPERL_dMOV(dst, src) D(dst) = D(src)
#define GPERL_sMOV(dst, name) S(dst) = (char *)name
#define GPERL_oMOV(dst, src) O(dst) = O(src)

#define GPERL_ADD(dst, src)
#define GPERL_iADD(dst, src) I(dst) += I(src)
#define GPERL_dADD(dst, src) D(dst) += D(src)
#define GPERL_sADD(dst, src)
#define GPERL_oADD(dst, src)
#define GPERL_iADDC(dst, src) I(dst) += src
#define GPERL_dADDC(dst, src) D(dst) += src
#define GPERL_sADDC(dst, src)

#define GPERL_SUB(dst, src)
#define GPERL_iSUB(dst, src) I(dst) -= I(src)
#define GPERL_dSUB(dst, src) D(dst) -= D(src)
#define GPERL_oSUB(dst, src)
#define GPERL_iSUBC(dst, src) I(dst) -= src
#define GPERL_dSUBC(dst, src) D(dst) -= src

#define GPERL_MUL(dst, src)
#define GPERL_iMUL(dst, src) I(dst) *= I(src)
#define GPERL_dMUL(dst, src) D(dst) *= D(src)
#define GPERL_sMUL(dst, src)
#define GPERL_oMUL(dst, src)
#define GPERL_iMULC(dst, src) I(dst) *= src
#define GPERL_dMULC(dst, src) D(dst) *= src
#define GPERL_sMULC(dst, src)

#define GPERL_DIV(dst, src)
#define GPERL_iDIV(dst, src) I(dst) /= I(src)
#define GPERL_dDIV(dst, src) D(dst) /= D(src)
#define GPERL_oDIV(dst, src)
#define GPERL_iDIVC(dst, src) I(dst) /= src
#define GPERL_dDIVC(dst, src) D(dst) /= src

#define GPERL_CMP_JMP(op, dst, src)
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

#define GPERL_RET(src)
#define GPERL_iRET(src) I(0) = I(src); RETURN();
#define GPERL_dRET(src) D(0) = D(src); RETURN();
#define GPERL_sRET(src) S(0) = S(src); RETURN();
#define GPERL_oRET(src) O(0) = O(src); RETURN();

#define GPERL_NOP()

#define GPERL_WRITE(dst)
#define GPERL_iWRITE(dst) {								\
		sprintf(shared_buf, "%d", I(dst));				\
		printf("%d\n", I(dst));							\
		outbuf += string(shared_buf);					\
	}
#define GPERL_dWRITE(dst) {								\
		sprintf(shared_buf, "%f", D(dst));				\
		outbuf += string(shared_buf);					\
	}
#define GPERL_sWRITE(dst) outbuf += string(S(dst))
#define GPERL_oWRITE(dst)

#define GPERL_FLUSH() {												\
		fprintf(stderr, "%s", cstr(outbuf));						\
		outbuf = "";												\
	}
#define GPERL_JMP() pc += pc->jmp
#define GPERL_LET(dst, src) variable_memory[dst]->value.ovalue = O(0)
#define GPERL_FUNCSET(func, dst) setToFuncMemory(func, dst)
#define GPERL_SETv(name, dst) setToVariableMemory(name, dst)

#define GPERL_CALL(dst, src, NAME)
#define GPERL_iCALL(dst, src, NAME) {							\
		GPerlVirtualMachineCode *code = func_memory[src];		\
		top = code;												\
		callstack++;											\
		callstack->ret_addr = &&L_##NAME##AFTER;				\
		callstack->pc = pc;										\
		pc = top;												\
		GOTO_NEXTOP();											\
	L_##NAME##AFTER:											\
		pc = callstack->pc;										\
		int res = I(0);											\
		callstack--;											\
		I(dst) = res;											\
	}

#define GPERL_dCALL(dst, src, NAME) {							\
		GPerlVirtualMachineCode *code = func_memory[src];		\
		top = code;												\
		callstack++;											\
		callstack->ret_addr = &&L_##NAME##AFTER;				\
		callstack->pc = pc;										\
		pc = top;												\
		GOTO_NEXTOP();											\
	L_##NAME##AFTER:											\
		pc = callstack->pc;										\
		double res = D(0);										\
		callstack--;											\
		D(dst) = res;											\
	}

#define GPERL_sCALL(dst, src, NAME) {							\
		GPerlVirtualMachineCode *code = func_memory[src];		\
		top = code;												\
		callstack++;											\
		callstack->ret_addr = &&L_##NAME##AFTER;				\
		callstack->pc = pc;										\
		pc = top;												\
		GOTO_NEXTOP();											\
	L_##NAME##AFTER:											\
		pc = callstack->pc;										\
		char *res = S(0);										\
		callstack--;											\
		S(dst) = res;											\
	}

#define GPERL_oCALL(dst, src, NAME) {							\
		GPerlVirtualMachineCode *code = func_memory[src];		\
		top = code;												\
		callstack++;											\
		callstack->ret_addr = &&L_##NAME##AFTER;				\
		callstack->pc = pc;										\
		pc = top;												\
		GOTO_NEXTOP();											\
	L_##NAME##AFTER:											\
		pc = callstack->pc;										\
		GPerlObject *res = (GPerlObject *)O(0);					\
		callstack--;											\
		O(dst) = res;											\
	}

#define GPERL_SELFCALL(dst, NAME)
#define GPERL_iSELFCALL(dst, NAME) {				\
		callstack++;								\
		callstack->ret_addr = &&L_##NAME##AFTER;	\
		callstack->pc = pc;							\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_##NAME##AFTER:								\
		pc = callstack->pc;							\
		int res = I(0);								\
		callstack--;								\
		I(dst) = res;								\
	}

#define GPERL_dSELFCALL(dst, NAME) {				\
		callstack++;								\
		callstack->ret_addr = &&L_##NAME##AFTER;	\
		callstack->pc = pc;							\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_##NAME##AFTER:								\
		pc = callstack->pc;							\
		double res = D(0);							\
		callstack--;								\
		D(dst) = res;								\
	}

#define GPERL_sSELFCALL(dst, NAME) {				\
		callstack++;								\
		callstack->ret_addr = &&L_##NAME##AFTER;	\
		callstack->pc = pc;							\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_##NAME##AFTER:								\
		pc = callstack->pc;							\
		char *res = S(0);							\
		callstack--;								\
		S(dst) = res;								\
	}

#define GPERL_oSELFCALL(dst, NAME) {				\
		callstack++;								\
		callstack->ret_addr = &&L_##NAME##AFTER;	\
		callstack->pc = pc;							\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_##NAME##AFTER:								\
		pc = callstack->pc;							\
		GPerlObject *res = (GPerlObject *)O(0);		\
		callstack--;								\
		O(dst) = res;								\
	}

#define GPERL_SHIFT(src)
#define GPERL_iSHIFT(src) I(0) = argstack[src]->value.ivalue
#define GPERL_dSHIFT(src) D(0) = argstack[src]->value.dvalue
#define GPERL_sSHIFT(src) S(0) = argstack[src]->value.svalue
#define GPERL_oSHIFT(src) O(0) = (GPerlObject *)argstack[src]->value.ovalue
#define GPERL_PUSH(dst, src)
#define GPERL_iPUSH(dst, src) {											\
		(callstack+1)->argstack[src]->value.ivalue = I(dst);			\
	}
#define GPERL_dPUSH(dst, src) {											\
		(callstack+1)->argstack[src]->value.dvalue = D(dst);			\
	}
#define GPERL_sPUSH(dst, src) {											\
		(callstack+1)->argstack[src]->value.svalue = S(dst);			\
	}
#define GPERL_oPUSH(dst, src) {											\
		(callstack+1)->argstack[src]->value.ovalue = O(dst);			\
	}
#define GPERL_NEW()
