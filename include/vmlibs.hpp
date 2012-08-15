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

#define I(idx) reg[idx].ivalue
#define D(idx) reg[idx].dvalue
#define S(idx) reg[idx].svalue
#define O(idx) reg[idx].ovalue

#define GPERL_UNDEF()
#define GPERL_LET(dst, src) stack[dst] = reg[src];
#define GPERL_gLET(dst, src) global_vmemory[dst] = reg[src]
#define GPERL_MOV(dst, v) reg[dst] = v
#define GPERL_vMOV(dst, src) reg[dst] = stack[src]
#define GPERL_gMOV(dst, src) reg[dst] = global_vmemory[src];
#define GPERL_ARGMOV(dst, src) reg[dst] = argstack[src]
#define GPERL_ArrayARGMOV(_dst) {                   \
		args->list = argstack;						\
		args->size = callstack->pc->argc;			\
		OBJECT_init(reg[_dst], args);				\
	}

#define GPERL_iADD(dst, src) I(dst) += I(src)
#define GPERL_dADD(dst, src) D(dst) += D(src)
#define GPERL_sADD(dst, src)
#define GPERL_oADD(dst, src)
#define GPERL_ADDC(dst, src)
#define GPERL_iADDC(dst, v) I(dst) += v.ivalue
#define GPERL_dADDC(dst, v) D(dst) += v.dvalue
#define GPERL_sADDC(dst, src)

#define GPERL_StringADD(dst, v)
#define GPERL_iStringADD(dst, v)
#define GPERL_dStringADD(dst, v)
#define GPERL_sStringADD(dst, v)
#define GPERL_oStringADD(dst, v)

#define GPERL_StringADDC(dst, v)
#define GPERL_iStringADDC(dst, v)
#define GPERL_dStringADDC(dst, v)
#define GPERL_sStringADDC(dst, v)

#define GPERL_iSUB(dst, src) I(dst) -= I(src)
#define GPERL_dSUB(dst, src) D(dst) -= D(src)
#define GPERL_oSUB(dst, src)
#define GPERL_SUBC(dst, src)
#define GPERL_iSUBC(dst, v) I(dst) -= v.ivalue
#define GPERL_dSUBC(dst, v) D(dst) -= v.dvalue

#define GPERL_iMUL(dst, src) I(dst) *= I(src)
#define GPERL_dMUL(dst, src) D(dst) *= D(src)
#define GPERL_sMUL(dst, src)
#define GPERL_oMUL(dst, src)
#define GPERL_MULC(dst, src)
#define GPERL_iMULC(dst, v) I(dst) *= v.ivalue
#define GPERL_dMULC(dst, v) D(dst) *= v.dvalue
#define GPERL_sMULC(dst, src)

#define GPERL_iDIV(dst, src) I(dst) /= I(src)
#define GPERL_dDIV(dst, src) D(dst) /= D(src)
#define GPERL_oDIV(dst, src)
#define GPERL_DIVC(dst, src)
#define GPERL_iDIVC(dst, v) I(dst) /= v.ivalue
#define GPERL_dDIVC(dst, v) D(dst) /= v.dvalue

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
#define GPERL_iCMP_JMPC(op, dst, v) {			\
		if (I(dst) op v.ivalue) {				\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}
#define GPERL_dCMP_JMPC(op, dst, v) {			\
		if (D(dst) op v.dvalue) {				\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}
#define GPERL_sCMP_JMPC(op, dst, src) {}
#define GPERL_oCMP_JMPC(op, dst, src) {}

#define GPERL_iISNOT(dst, v) {					\
		if (!I(dst)) {							\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}

#define GPERL_iISNOTC(dst, v) {					\
		if (!v.ivalue) {						\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}
#define GPERL_dISNOT(dst, v) {					\
		if (!D(dst)) {							\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}

#define GPERL_dISNOTC(dst, v) {					\
		if (!v.dvalue) {						\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}

#define GPERL_sISNOT(dst, v) {					\
		if (getLength(reg[dst]) == 1) {			\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}

#define GPERL_sISNOTC(dst, v) {					\
		if (getLength(reg[dst]) == 1) {			\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}

#define GPERL_oISNOT(dst, v) {									\
		GPerlObject *o = (GPerlObject *)getObject(reg[dst]);	\
		if (o->h.type == Undefined) {							\
			pc++;												\
		} else {												\
			pc += pc->jmp;										\
		}														\
	}

#define GPERL_oISNOTC(dst, v) {							\
		GPerlObject *o = (GPerlObject *)getObject(v);	\
		if (o->h.type == Undefined) {					\
			pc++;										\
		} else {										\
			pc += pc->jmp;								\
		}												\
	}
#define GPERL_iIS(dst, v) {						\
		if (I(dst)) {							\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}

#define GPERL_iISC(dst, v) {					\
		if (v.ivalue) {							\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}
#define GPERL_dIS(dst, v) {						\
		if (D(dst)) {							\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}

#define GPERL_dISC(dst, v) {					\
		if (v.dvalue) {							\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}

#define GPERL_sIS(dst, v) {						\
		if (getLength(reg[dst]) != 1) {			\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}

#define GPERL_sISC(dst, v) {					\
		if (getLength(reg[dst]) != 1) {			\
			pc++;								\
		} else {								\
			pc += pc->jmp;						\
		}										\
	}

#define GPERL_oIS(dst, v) {										\
		GPerlObject *o = (GPerlObject *)getObject(reg[dst]);	\
		if (o->h.type != Undefined) {							\
			pc++;												\
		} else {												\
			pc += pc->jmp;										\
		}														\
	}

#define GPERL_oISC(dst, v) {							\
		GPerlObject *o = (GPerlObject *)getObject(v);	\
		if (o->h.type != Undefined) {					\
			pc++;										\
		} else {										\
			pc += pc->jmp;								\
		}												\
	}

#define GPERL_iINC(dst) stack[dst].ivalue++
#define GPERL_dINC(dst) stack[dst].dvalue++
#define GPERL_sINC(dst)
#define GPERL_oINC(dst)
#define GPERL_igINC(dst) global_vmemory[dst].ivalue++
#define GPERL_dgINC(dst) global_vmemory[dst].dvalue++
#define GPERL_sgINC(dst)
#define GPERL_ogINC(dst)

#define GPERL_iDEC(dst) stack[dst].ivalue--
#define GPERL_dDEC(dst) stack[dst].dvalue--
#define GPERL_sDEC(dst)
#define GPERL_oDEC(dst)
#define GPERL_igDEC(dst) global_vmemory[dst].ivalue--
#define GPERL_dgDEC(dst) global_vmemory[dst].dvalue--
#define GPERL_sgDEC(dst)
#define GPERL_ogDEC(dst)

#define GPERL_RET(src) reg[0] = reg[src]; RETURN();
#define GPERL_NOP()

#define GPERL_WRITE(dst)
#define GPERL_iWRITE(dst) {								\
		sprintf(shared_buf, "%d", I(dst));				\
		write_cwb(shared_buf);                          \
	}
#define GPERL_dWRITE(dst) {								\
		sprintf(shared_buf, "%f", D(dst));				\
		write_cwb(shared_buf);                          \
	}
#define GPERL_sWRITE(dst) write_cwb(getRawString(reg[dst]))
#define GPERL_oWRITE(dst) {									\
		GPerlObject *o = (GPerlObject *)getObject(reg[dst]);			\
		o->write(reg[dst]);												\
	}

#define GPERL_FLUSH() {                                 \
		fprintf(stdout, "%s", cwb);						\
		clear_cwb();                                    \
	}
#define GPERL_JMP() pc += pc->jmp
#define GPERL_FUNCSET(func, dst) setToFuncMemory(func, dst)
#define GPERL_SETv(name, dst)

#define GPERL_CALL(dst, src, NAME) {				\
		code_ = func_memory[src];					\
		top   = code_;								\
		callstack->ebp = stack;						\
		stack += pc->cur_stack_top;					\
		callstack++;								\
		reg = callstack->reg;						\
		argstack = callstack->argstack;				\
		callstack->ret_addr = &&L_##NAME##AFTER;	\
		callstack->pc = pc;							\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_##NAME##AFTER:								\
		pc = callstack->pc;							\
		(callstack-1)->reg[dst] = reg[0];			\
		callstack--;								\
		reg = callstack->reg;						\
		argstack = callstack->argstack;				\
		stack = callstack->ebp;						\
	}

#define GPERL_FASTCALL0(arg0, dst, src, NAME) {		\
		code_ = func_memory[src];					\
		top   = code_;								\
		callstack->ebp = stack;						\
		stack += pc->cur_stack_top;					\
		callstack++;								\
		argstack = callstack->argstack;				\
		argstack[0] = reg[arg0];					\
		reg = callstack->reg;						\
		callstack->ret_addr = &&L_##NAME##AFTER;	\
		callstack->pc = pc;							\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_##NAME##AFTER:								\
		pc = callstack->pc;							\
		(callstack-1)->reg[dst] = reg[0];			\
		callstack--;								\
		reg = callstack->reg;						\
		argstack = callstack->argstack;				\
		stack = callstack->ebp;						\
	}

#define GPERL_FASTCALL1(arg0, arg1, dst, src, NAME) {	\
		code_ = func_memory[src];					\
		top   = code_;								\
		callstack->ebp = stack;						\
		stack += pc->cur_stack_top;					\
		callstack++;								\
		argstack = callstack->argstack;				\
		argstack[0] = reg[arg0];					\
		argstack[1] = reg[arg1];					\
		reg = callstack->reg;						\
		callstack->ret_addr = &&L_##NAME##AFTER;	\
		callstack->pc = pc;							\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_##NAME##AFTER:								\
		pc = callstack->pc;							\
		(callstack-1)->reg[dst] = reg[0];			\
		callstack--;								\
		reg = callstack->reg;						\
		argstack = callstack->argstack;				\
		stack = callstack->ebp;						\
	}

#define GPERL_FASTCALL2(arg0, arg1, arg2, dst, src, NAME) {	\
		code_ = func_memory[src];					\
		top   = code_;								\
		callstack->ebp = stack;						\
		stack += pc->cur_stack_top;					\
		callstack++;								\
		argstack = callstack->argstack;				\
		argstack[0] = reg[arg0];					\
		argstack[1] = reg[arg1];					\
		argstack[2] = reg[arg2];					\
		reg = callstack->reg;						\
		callstack->ret_addr = &&L_##NAME##AFTER;	\
		callstack->pc = pc;							\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_##NAME##AFTER:								\
		pc = callstack->pc;							\
		(callstack-1)->reg[dst] = reg[0];			\
		callstack--;								\
		reg = callstack->reg;						\
		argstack = callstack->argstack;				\
		stack = callstack->ebp;						\
	}

#define GPERL_FASTCALL3(arg0, arg1, arg2, arg3, dst, src, NAME) {	\
		code_ = func_memory[src];					\
		top   = code_;								\
		callstack->ebp = stack;						\
		stack += pc->cur_stack_top;					\
		callstack++;								\
		argstack = callstack->argstack;				\
		argstack[0] = reg[arg0];					\
		argstack[1] = reg[arg1];					\
		argstack[2] = reg[arg2];					\
		argstack[3] = reg[arg3];					\
		reg = callstack->reg;						\
		callstack->ret_addr = &&L_##NAME##AFTER;	\
		callstack->pc = pc;							\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_##NAME##AFTER:								\
		pc = callstack->pc;							\
		(callstack-1)->reg[dst] = reg[0];			\
		callstack--;								\
		reg = callstack->reg;						\
		argstack = callstack->argstack;				\
		stack = callstack->ebp;						\
	}

#define GPERL_SELFCALL(dst, NAME) {					\
		callstack->ebp = stack;						\
		stack += pc->cur_stack_top;					\
		callstack++;								\
		reg = callstack->reg;						\
		argstack = callstack->argstack;				\
		callstack->ret_addr = &&L_##NAME##AFTER;	\
		callstack->pc = pc;							\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_##NAME##AFTER:								\
		pc = callstack->pc;							\
		(callstack-1)->reg[dst] = reg[0];			\
		callstack--;								\
		reg = callstack->reg;						\
		argstack = callstack->argstack;				\
		stack = callstack->ebp;						\
	}

#define GPERL_SELF_FASTCALL0(arg0, dst, NAME) {		\
		callstack->ebp = stack;						\
		stack += pc->cur_stack_top;					\
		callstack++;								\
		argstack = callstack->argstack;				\
		argstack[0] = reg[arg0];					\
		reg = callstack->reg;						\
		callstack->ret_addr = &&L_##NAME##AFTER;	\
		callstack->pc = pc;							\
		pc = top;									\
		GOTO_NEXTOP();								\
	L_##NAME##AFTER:								\
		pc = callstack->pc;							\
		(callstack-1)->reg[dst] = reg[0];			\
		callstack--;								\
		reg = callstack->reg;						\
		argstack = callstack->argstack;				\
		stack = callstack->ebp;						\
	}

#define GPERL_SELF_FASTCALL1(arg0, arg1, dst, NAME) {	\
		callstack->ebp = stack;							\
		stack += pc->cur_stack_top;						\
		callstack++;									\
		argstack = callstack->argstack;					\
		argstack[0] = reg[arg0];						\
		argstack[1] = reg[arg1];						\
		reg = callstack->reg;							\
		callstack->ret_addr = &&L_##NAME##AFTER;		\
		callstack->pc = pc;								\
		pc = top;										\
		GOTO_NEXTOP();									\
	L_##NAME##AFTER:									\
		pc = callstack->pc;								\
		(callstack-1)->reg[dst] = reg[0];				\
		callstack--;									\
		reg = callstack->reg;							\
		argstack = callstack->argstack;					\
		stack = callstack->ebp;							\
	}

#define GPERL_SELF_FASTCALL2(arg0, arg1, arg2, dst, NAME) {	\
		callstack->ebp = stack;							\
		stack += pc->cur_stack_top;						\
		callstack++;									\
		argstack = callstack->argstack;					\
		argstack[0] = reg[arg0];						\
		argstack[1] = reg[arg1];						\
		argstack[2] = reg[arg2];						\
		reg = callstack->reg;							\
		callstack->ret_addr = &&L_##NAME##AFTER;		\
		callstack->pc = pc;								\
		pc = top;										\
		GOTO_NEXTOP();									\
	L_##NAME##AFTER:									\
		pc = callstack->pc;								\
		(callstack-1)->reg[dst] = reg[0];				\
		callstack--;									\
		reg = callstack->reg;							\
		argstack = callstack->argstack;					\
		stack = callstack->ebp;							\
	}

#define GPERL_SELF_FASTCALL3(arg0, arg1, arg2, arg3, dst, NAME) {	\
		callstack->ebp = stack;							\
		stack += pc->cur_stack_top;						\
		callstack++;									\
		argstack = callstack->argstack;					\
		argstack[0] = reg[arg0];						\
		argstack[1] = reg[arg1];						\
		argstack[2] = reg[arg2];						\
		argstack[3] = reg[arg3];						\
		reg = callstack->reg;							\
		callstack->ret_addr = &&L_##NAME##AFTER;		\
		callstack->pc = pc;								\
		pc = top;										\
		GOTO_NEXTOP();									\
	L_##NAME##AFTER:									\
		pc = callstack->pc;								\
		(callstack-1)->reg[dst] = reg[0];				\
		callstack--;									\
		reg = callstack->reg;							\
		argstack = callstack->argstack;					\
		stack = callstack->ebp;							\
	}

#define GPERL_JIT_CALL(src, env) {                              \
		code_ = func_memory[src];								\
		top   = code_;											\
		args->list = (callstack+1)->argstack;					\
		args->size = pc->argc;									\
		unsigned int result = jit_compiler.compile(top, &env);	\
		INT_init(reg[pc->dst], result);							\
	}

#define GPERL_JIT_SELFCALL(top, env) {                          \
		args->list = (callstack+1)->argstack;					\
		args->size = pc->argc;									\
		unsigned int result = jit_compiler.compile(top, &env);	\
		INT_init(reg[pc->dst], result);							\
	}

#define GPERL_SHIFT(src)
#define GPERL_iSHIFT(src)
#define GPERL_dSHIFT(src)
#define GPERL_sSHIFT(src)
#define GPERL_oSHIFT(src)
#define GPERL_PUSH(dst, src) (callstack+1)->argstack[src] = reg[dst];
#define GPERL_NEW() do {										\
		root.callstack_top = callstack;							\
		root.stack_top_idx = pc->cur_stack_top;					\
		OBJECT_init(reg[pc->dst], pc->_new(pc->v));				\
	} while (0)
#define GPERL_NEW_STRING() do {\
		root.callstack_top = callstack;							\
		root.stack_top_idx = pc->cur_stack_top;					\
		STRING_init(reg[pc->dst], (GPerlString *)pc->_new(pc->v)); \
	} while (0)
#define GPERL_ARRAY_PUSH(argstack) pc->push(argstack);
#define GPERL_ARRAY_AT(dst, src, idx) do {					\
	  GPerlArray *a = (GPerlArray *)getObject(stack[src]);	\
	  if (a->size > I(idx)) {								\
		  reg[dst] = a->list[I(idx)];						\
	  } else {												\
		  root.callstack_top = callstack;					\
		  root.stack_top_idx = pc->cur_stack_top;			\
		  GPerlUndef *undef = new_GPerlUndef();				\
		  OBJECT_init(reg[dst], undef);						\
	  }														\
  } while (0);

#define GPERL_ARRAY_ATC(dst, src, idx) do {					\
	  GPerlArray *a = (GPerlArray *)getObject(stack[src]);	\
	  if (a->size > idx) {									\
		  reg[dst] = a->list[idx];							\
	  } else {												\
		  root.callstack_top = callstack;					\
		  root.stack_top_idx = pc->cur_stack_top;			\
		  GPerlUndef *undef = new_GPerlUndef();				\
		  OBJECT_init(reg[dst], undef);						\
	  }														\
  } while (0);

#define GPERL_ARRAY_gAT(dst, src, idx) do {								\
		GPerlArray *a = (GPerlArray *)getObject(global_vmemory[src]);	\
		if (a->size > I(idx)) {											\
			reg[dst] = a->list[I(idx)];									\
		} else {														\
			root.callstack_top = callstack;								\
			root.stack_top_idx = pc->cur_stack_top;						\
			GPerlUndef *undef = new_GPerlUndef();						\
			OBJECT_init(reg[dst], undef);								\
		}																\
	} while (0);

#define GPERL_ARRAY_gATC(dst, src, idx) do {							\
		GPerlArray *a = (GPerlArray *)getObject(global_vmemory[src]);	\
		if (a->size > idx) {											\
			reg[dst] = a->list[idx];									\
		} else {														\
			root.callstack_top = callstack;								\
			root.stack_top_idx = pc->cur_stack_top;						\
			GPerlUndef *undef = new_GPerlUndef();						\
			OBJECT_init(reg[dst], undef);								\
		}																\
	} while (0);


#define GPERL_ARRAY_DREF(dst, src) do {									\
		root.callstack_top = callstack;									\
		root.stack_top_idx = pc->cur_stack_top;							\
		GPerlArray *a = (GPerlArray *)new_GPerlArray(reg[src]);			\
		a->h.type = Array;												\
		OBJECT_init(reg[dst], a);										\
	} while (0);
