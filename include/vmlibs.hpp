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
#define _CASE(op) L(op) : { /* DBG_PL(#op); */ }
#define CASE(op, block) _CASE(op) { START(op); block; END(op); }
#define RETURN() { goto *callstack->ret_addr; }

#define I(idx) reg[idx].ivalue
#define D(idx) reg[idx].dvalue
#define S(idx) reg[idx].svalue
#define O(idx) reg[idx].ovalue

#define GPERL_UNDEF()
#define GPERL_LET(dst, src) stack[dst] = reg[src];
#define GPERL_gLET(dst, src) global_vmemory[dst] = reg[src]
#define GPERL_MLET(dst, src, idx) do {						\
		GPerlArray *a = (GPerlArray *)getObject(reg[src]);	\
		if (a->size > idx) {								\
			stack[dst] = a->list[idx];						\
		}													\
	} while (0)
#define GPERL_gMLET(dst, src, idx) do {						\
		GPerlArray *a = (GPerlArray *)getObject(reg[src]);	\
		if (a->size > idx) {								\
			global_vmemory[dst] = a->list[idx];				\
		}													\
	} while (0)

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

#define GPERL_iLSHIFT(dst, src) I(dst) <<= I(src)
#define GPERL_dLSHIFT(dst, src)
#define GPERL_sLSHIFT(dst, src)
#define GPERL_oLSHIFT(dst, src)
#define GPERL_LSHIFTC(dst, src)
#define GPERL_iLSHIFTC(dst, v) I(dst) <<= v.ivalue
#define GPERL_dLSHIFTC(dst, v)
#define GPERL_sLSHIFTC(dst, src)
#define GPERL_oLSHIFTC(dst, src)

#define GPERL_iRSHIFT(dst, src) I(dst) >>= I(src)
#define GPERL_dRSHIFT(dst, src)
#define GPERL_sRSHIFT(dst, src)
#define GPERL_oRSHIFT(dst, src)
#define GPERL_RSHIFTC(dst, src)
#define GPERL_iRSHIFTC(dst, v) I(dst) >>= v.ivalue
#define GPERL_dRSHIFTC(dst, v)
#define GPERL_sRSHIFTC(dst, src)
#define GPERL_oRSHIFTC(dst, src)

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

#define JIT_SET_TYPE(type, reg) do {			\
		switch (TYPE_CHECK(reg)) {				\
		case 0: type = Double;					\
			break;								\
		case 1: type = Int;						\
			break;								\
		case 2: type = String;					\
			break;								\
		case 3: type = Object;					\
			break;								\
		}										\
	} while (0);

#define GPERL_JIT_COUNTDOWN_RET(src) do {					\
		reg[0] = reg[src];									\
		pc->op = decl_codes[pc->op - 1].code;				\
		if (!top->is_defined_return_type) {					\
			JITParam *prm = getParam(top);					\
			JIT_SET_TYPE(prm->return_type, reg[0]);			\
			top->is_defined_return_type = true;				\
		}													\
		pc->opnext = jmp_table[RET];						\
		RETURN();											\
	} while (0);

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
		fprintf(stdout, "%s", cwb->buf);				\
		clear_cwb();                                    \
	}

#define GPERL_REF(v) {										\
		int ret = 0;										\
		if (TYPE_CHECK(v) > 1) {							\
			GPerlObject *o = (GPerlObject *)getObject(v);	\
			if (o->h.type == ArrayRef) {					\
				ret = 1;									\
			}												\
		}													\
		INT_init(reg[0], ret);								\
	}

static inline GPerlArray *GPERL_KEYS(GPerlValue arg)
{
	GPerlHash *h = (GPerlHash *)getObject(arg);
	if (h->h.type == Hash) {
		GPerlString **keys = h->keys;
		size_t key_n = h->size / 2;
		size_t size = sizeof(GPerlValue) * (key_n + 1);
		GPerlValue *list = (GPerlValue *)safe_malloc(size);
		size_t i = 0;
		for (i = 0; i < key_n; i++) {
			STRING_init(list[i], keys[i]);
		}
		OBJECT_init(list[i], undef);
		GPerlArray *ret = new_GPerlInitArray(list, key_n);
		return ret;
	} else {
		fprintf(stdout, "type error: 'keys' needs hash variable\n");
		return (GPerlArray *)undef;
	}
}

static inline GPerlArray *GPERL_VALUES(GPerlValue arg)
{
	GPerlHash *h = (GPerlHash *)getObject(arg);
	if (h->h.type == Hash) {
		GPerlString **keys = h->keys;
		size_t key_n = h->size / 2;
		size_t size = sizeof(GPerlValue) * (key_n + 1);
		GPerlValue *list = (GPerlValue *)safe_malloc(size);
		size_t i = 0;
		for (i = 0; i < key_n; i++) {
			GPerlString *key = keys[i];
			GPerlValue value = h->table[key->hash];
			list[i] = value;
		}
		OBJECT_init(list[i], undef);
		GPerlArray *ret = new_GPerlInitArray(list, key_n);
		return ret;
	} else {
		fprintf(stdout, "type error: 'values' needs hash variable\n");
		return (GPerlArray *)undef;
	}
}

#define GPERL_BLESS(args) do {								\
		GPerlHash *self = (GPerlHash *)getObject(args[0]);	\
		GPerlString *class_name = getStringObj(args[1]);	\
		GPerlClass *gclass = pkg_table[class_name->hash];	\
		gclass->fields = self;								\
		OBJECT_init(reg[pc->dst], gclass);					\
	} while (0);

#define GPERL_JMP() pc += pc->jmp
#define GPERL_FUNCSET(func, dst) setToFuncMemory(func, dst)
#define GPERL_SETv(name, dst)

#define PROLOGUE(NAME) do {							\
		callstack++;								\
		callstack->ebp = stack;						\
		stack += pc->cur_stack_top;					\
		argstack = callstack->argstack;				\
		callstack->ret_addr = &&L_##NAME##AFTER;	\
		callstack->pc = pc;							\
	} while (0);

#define EPILOGUE(dst, NAME) do {					\
	L_##NAME##AFTER:								\
		pc = callstack->pc;							\
		(callstack-1)->reg[dst] = reg[0];			\
		stack = callstack->ebp;						\
		callstack--;								\
		reg = callstack->reg;						\
		argstack = callstack->argstack;				\
	} while (0);

#define INVOKE_FUNC() do {						\
		reg = callstack->reg;					\
		pc = top;								\
		GOTO_NEXTOP();							\
	} while (0);

#define GPERL_CLOSURE(dst, src, NAME) do {					\
		GPerlFunc *f = (GPerlFunc *)getObject(reg[src]);	\
		top   = f->code;									\
		PROLOGUE(NAME);										\
		INVOKE_FUNC();										\
		EPILOGUE(dst, NAME);								\
	} while (0);

#define GPERL_STATIC_CALL(dst, src, NAME) do {				\
		GPerlFunc *mtd = (GPerlFunc *)getObject(pc->v);		\
		top   = mtd->code;									\
		PROLOGUE(NAME);										\
		INVOKE_FUNC();										\
		EPILOGUE(dst, NAME);								\
	} while (0);

#define GPERL_DYNAMIC_CALL(dst, src, NAME) do {							\
		GPerlClass *self = (GPerlClass *)getObject((callstack+1)->argstack[src]); \
		GPerlFunc *mtd = self->mtds[pc->hash];							\
		top   = mtd->code;												\
		PROLOGUE(NAME);													\
		INVOKE_FUNC();													\
		EPILOGUE(dst, NAME);											\
	} while (0);

#define GPERL_CALL(dst, src, NAME) {				\
		code_ = func_memory[src];					\
		top   = code_;								\
		PROLOGUE(NAME);								\
		INVOKE_FUNC();								\
		EPILOGUE(dst, NAME);						\
	}

#define GPERL_JIT_COUNTDOWN_CALL(dst, src, NAME) {	\
		code_ = func_memory[src];					\
		top   = code_;								\
		pc->opnext = jmp_table[pc->op - 2];			\
		pc->op = decl_codes[pc->op - 2].code;		\
		top->jit_count_down--;						\
		PROLOGUE(NAME);								\
		INVOKE_FUNC();								\
		EPILOGUE(dst, NAME);						\
	}

#define GPERL_FASTCALL0(arg0, dst, src, NAME) {		\
		code_ = func_memory[src];					\
		top   = code_;								\
		PROLOGUE(NAME);								\
		argstack[0] = reg[arg0];					\
		INVOKE_FUNC();								\
		EPILOGUE(dst, NAME);						\
	}

#define GPERL_JIT_COUNTDOWN_FASTCALL0(arg0, dst, src, NAME) {	\
		code_ = func_memory[src];								\
		top   = code_;											\
		pc->opnext = jmp_table[pc->op - 1];						\
		pc->op = decl_codes[pc->op - 1].code;					\
		JITParam *prm = getParam(top);							\
		if (prm->argc == 0) {									\
			prm->argc = 1;										\
			JIT_SET_TYPE(prm->arg_types[0], reg[arg0]);			\
		}														\
		top->jit_count_down--;									\
		PROLOGUE(NAME);											\
		argstack[0] = reg[arg0];								\
		INVOKE_FUNC();											\
		EPILOGUE(dst, NAME);									\
	}

#define GPERL_FASTCALL1(arg0, arg1, dst, src, NAME) {	\
		code_ = func_memory[src];						\
		top   = code_;									\
		PROLOGUE(NAME);									\
		argstack[0] = reg[arg0];						\
		argstack[1] = reg[arg1];						\
		INVOKE_FUNC();									\
		EPILOGUE(dst, NAME);							\
	}

#define GPERL_JIT_COUNTDOWN_FASTCALL1(arg0, arg1, dst, src, NAME) {	\
		code_ = func_memory[src];									\
		top   = code_;												\
		pc->opnext = jmp_table[pc->op - 1];							\
		pc->op = decl_codes[pc->op - 1].code;						\
		JITParam *prm = getParam(top);								\
		if (prm->argc == 0) {										\
			prm->argc = 2;											\
			JIT_SET_TYPE(prm->arg_types[0], reg[arg0]);				\
			JIT_SET_TYPE(prm->arg_types[1], reg[arg1]);				\
		}															\
		top->jit_count_down--;										\
		PROLOGUE(NAME);												\
		argstack[0] = reg[arg0];									\
		argstack[1] = reg[arg1];									\
		INVOKE_FUNC();												\
		EPILOGUE(dst, NAME);										\
	}

#define GPERL_FASTCALL2(arg0, arg1, arg2, dst, src, NAME) {	\
		code_ = func_memory[src];							\
		top   = code_;										\
		PROLOGUE(NAME);										\
		argstack[0] = reg[arg0];							\
		argstack[1] = reg[arg1];							\
		argstack[2] = reg[arg2];							\
		INVOKE_FUNC();										\
		EPILOGUE(dst, NAME);								\
	}

#define GPERL_JIT_COUNTDOWN_FASTCALL2(arg0, arg1, arg2, dst, src, NAME) { \
		code_ = func_memory[src];										\
		top   = code_;													\
		pc->opnext = jmp_table[pc->op - 1];								\
		pc->op = decl_codes[pc->op - 1].code;							\
		JITParam *prm = getParam(top);									\
		if (prm->argc == 0) {											\
			prm->argc = 3;												\
			JIT_SET_TYPE(prm->arg_types[0], reg[arg0]);					\
			JIT_SET_TYPE(prm->arg_types[1], reg[arg1]);					\
			JIT_SET_TYPE(prm->arg_types[2], reg[arg2]);					\
		}																\
		top->jit_count_down--;											\
		PROLOGUE(NAME);													\
		argstack[0] = reg[arg0];										\
		argstack[1] = reg[arg1];										\
		argstack[2] = reg[arg2];										\
		INVOKE_FUNC();													\
		EPILOGUE(dst, NAME);											\
	}

#define GPERL_FASTCALL3(arg0, arg1, arg2, arg3, dst, src, NAME) {	\
		code_ = func_memory[src];									\
		top   = code_;												\
		PROLOGUE(NAME);												\
		argstack[0] = reg[arg0];									\
		argstack[1] = reg[arg1];									\
		argstack[2] = reg[arg2];									\
		argstack[3] = reg[arg3];									\
		INVOKE_FUNC();												\
		EPILOGUE(dst, NAME);										\
	}

#define GPERL_JIT_COUNTDOWN_FASTCALL3(arg0, arg1, arg2, arg3, dst, src, NAME) {	\
		code_ = func_memory[src];										\
		top   = code_;													\
		pc->opnext = jmp_table[pc->op - 1];								\
		pc->op = decl_codes[pc->op - 1].code;							\
		JITParam *prm = getParam(top);									\
		if (prm->argc == 0) {											\
			prm->argc = 4;												\
			JIT_SET_TYPE(prm->arg_types[0], reg[arg0]);					\
			JIT_SET_TYPE(prm->arg_types[1], reg[arg1]);					\
			JIT_SET_TYPE(prm->arg_types[2], reg[arg2]);					\
			JIT_SET_TYPE(prm->arg_types[3], reg[arg3]);					\
		}																\
		top->jit_count_down--;											\
		PROLOGUE(NAME);													\
		argstack[0] = reg[arg0];										\
		argstack[1] = reg[arg1];										\
		argstack[2] = reg[arg2];										\
		argstack[3] = reg[arg3];										\
		INVOKE_FUNC();													\
		EPILOGUE(dst, NAME);											\
	}

#define GPERL_SELFCALL(dst, NAME) {					\
		PROLOGUE(NAME);								\
		INVOKE_FUNC();								\
		EPILOGUE(dst, NAME);						\
	}

#define GPERL_JIT_COUNTDOWN_SELFCALL(dst, NAME) {	\
		pc->op = decl_codes[pc->op - 2].code;		\
		pc->opnext = jmp_table[pc->op - 2];			\
		top->jit_count_down--;						\
		PROLOGUE(NAME);								\
		INVOKE_FUNC();								\
		EPILOGUE(dst, NAME);						\
	}

#define GPERL_SELF_FASTCALL0(arg0, dst, NAME) {		\
		PROLOGUE(NAME);								\
		argstack[0] = reg[arg0];					\
		INVOKE_FUNC();								\
		EPILOGUE(dst, NAME);						\
	}

#define GPERL_JIT_COUNTDOWN_SELF_FASTCALL0(arg0, dst, NAME) {	\
		pc->opnext = jmp_table[pc->op - 2];						\
		pc->op = decl_codes[pc->op - 2].code;					\
		JITParam *prm = getParam(top);							\
		if (prm->argc == 0) {									\
			prm->argc = 1;										\
			JIT_SET_TYPE(prm->arg_types[0], reg[arg0]);			\
		}														\
		top->jit_count_down--;									\
		PROLOGUE(NAME);											\
		argstack[0] = reg[arg0];								\
		INVOKE_FUNC();											\
		EPILOGUE(dst, NAME);									\
	}

#define GPERL_SELF_FASTCALL1(arg0, arg1, dst, NAME) {	\
		PROLOGUE(NAME);									\
		argstack[0] = reg[arg0];						\
		argstack[1] = reg[arg1];						\
		INVOKE_FUNC();									\
		EPILOGUE(dst, NAME);							\
	}

#define GPERL_JIT_COUNTDOWN_SELF_FASTCALL1(arg0, arg1, dst, NAME) {	\
		pc->opnext = jmp_table[pc->op - 2];							\
		pc->op = decl_codes[pc->op - 2].code;						\
		JITParam *prm = getParam(top);								\
		if (prm->argc == 0) {										\
			prm->argc = 2;											\
			JIT_SET_TYPE(prm->arg_types[0], reg[arg0]);				\
			JIT_SET_TYPE(prm->arg_types[1], reg[arg1]);				\
		}															\
		top->jit_count_down--;										\
		PROLOGUE(NAME);												\
		argstack[0] = reg[arg0];									\
		argstack[1] = reg[arg1];									\
		INVOKE_FUNC();												\
		EPILOGUE(dst, NAME);										\
	}

#define GPERL_SELF_FASTCALL2(arg0, arg1, arg2, dst, NAME) {	\
		PROLOGUE(NAME);										\
		argstack[0] = reg[arg0];							\
		argstack[1] = reg[arg1];							\
		argstack[2] = reg[arg2];							\
		INVOKE_FUNC();										\
		EPILOGUE(dst, NAME);								\
	}

#define GPERL_JIT_COUNTDOWN_SELF_FASTCALL2(arg0, arg1, arg2, dst, NAME) { \
		pc->opnext = jmp_table[pc->op - 2];								\
		pc->op = decl_codes[pc->op - 2].code;							\
		JITParam *prm = getParam(top);									\
		if (prm->argc == 0) {											\
			prm->argc = 3;												\
			JIT_SET_TYPE(prm->arg_types[0], reg[arg0]);					\
			JIT_SET_TYPE(prm->arg_types[1], reg[arg1]);					\
			JIT_SET_TYPE(prm->arg_types[2], reg[arg2]);					\
		}																\
		top->jit_count_down--;											\
		PROLOGUE(NAME);													\
		argstack[0] = reg[arg0];										\
		argstack[1] = reg[arg1];										\
		argstack[2] = reg[arg2];										\
		INVOKE_FUNC();													\
		EPILOGUE(dst, NAME);											\
	}

#define GPERL_SELF_FASTCALL3(arg0, arg1, arg2, arg3, dst, NAME) {	\
		PROLOGUE(NAME);												\
		argstack[0] = reg[arg0];									\
		argstack[1] = reg[arg1];									\
		argstack[2] = reg[arg2];									\
		argstack[3] = reg[arg3];									\
		INVOKE_FUNC();												\
		EPILOGUE(dst, NAME);										\
	}

#define GPERL_JIT_COUNTDOWN_SELF_FASTCALL3(arg0, arg1, arg2, arg3, dst, NAME) {	\
		pc->opnext = jmp_table[pc->op - 2];								\
		pc->op = decl_codes[pc->op - 2].code;							\
		JITParam *prm = getParam(top);									\
		if (prm->argc == 0) {											\
			prm->argc = 4;												\
			JIT_SET_TYPE(prm->arg_types[0], reg[arg0]);					\
			JIT_SET_TYPE(prm->arg_types[1], reg[arg1]);					\
			JIT_SET_TYPE(prm->arg_types[2], reg[arg2]);					\
			JIT_SET_TYPE(prm->arg_types[3], reg[arg3]);					\
		}																\
		top->jit_count_down--;											\
		PROLOGUE(NAME);													\
		argstack[0] = reg[arg0];										\
		argstack[1] = reg[arg1];										\
		argstack[2] = reg[arg2];										\
		argstack[3] = reg[arg3];										\
		INVOKE_FUNC();													\
		EPILOGUE(dst, NAME);											\
	}

#define GPERL_JIT_CALL(dst, src, NAME) {								\
		code_ = func_memory[src];										\
		top   = code_;													\
		JITParam *param = getParam(top);								\
		reg[dst] = jit_compiler.run(param->func, (callstack+1)->argstack, param); \
	}

#define GPERL_JIT_SELFCALL(dst, NAME) {									\
		JITParam *param = getParam(top);								\
		reg[dst] = jit_compiler.run(param->func, (callstack+1)->argstack, param); \
	}

#define GPERL_JIT_SELF_FASTCALL0(arg0, dst, NAME) {						\
		JITParam *param = getParam(top);								\
		(callstack+1)->argstack[0] = reg[arg0];							\
		reg[dst] = jit_compiler.run(param->func, (callstack+1)->argstack, param); \
	}

#define GPERL_JIT_SELF_FASTCALL1(arg0, arg1, dst, NAME) {				\
		JITParam *param = getParam(top);								\
		(callstack+1)->argstack[0] = reg[arg0];							\
		(callstack+1)->argstack[1] = reg[arg1];							\
		reg[dst] = jit_compiler.run(param->func, (callstack+1)->argstack, param); \
	}

#define GPERL_JIT_SELF_FASTCALL2(arg0, arg1, arg2, dst, NAME) {			\
		JITParam *param = getParam(top);								\
		(callstack+1)->argstack[0] = reg[arg0];							\
		(callstack+1)->argstack[1] = reg[arg1];							\
		(callstack+1)->argstack[2] = reg[arg2];							\
		reg[dst] = jit_compiler.run(param->func, (callstack+1)->argstack, param); \
	}

#define GPERL_JIT_SELF_FASTCALL3(arg0, arg1, arg2, arg3, dst, NAME) {	\
		JITParam *param = getParam(top);								\
		(callstack+1)->argstack[0] = reg[arg0];							\
		(callstack+1)->argstack[1] = reg[arg1];							\
		(callstack+1)->argstack[2] = reg[arg2];							\
		(callstack+1)->argstack[3] = reg[arg3];							\
		reg[dst] = jit_compiler.run(param->func, (callstack+1)->argstack, param); \
	}

#define GPERL_SHIFT(src)
#define GPERL_iSHIFT(src)
#define GPERL_dSHIFT(src)
#define GPERL_sSHIFT(src)
#define GPERL_oSHIFT(src)
/*
#define GPERL_aPUSH(dst, src) {											\
		GPerlArray *a = (GPerlArray*)getObject(reg[dst]);				\
		size_t size = a->size;											\
		for (size_t i = 0; size; i++) {									\
			(callstack+1)->argstack_set_ptr[i] = a->list[i];			\
		}																\
		(callstack+1)->argstack_set_ptr += a->size;						\
	}
*/
#define GPERL_aPUSH(dst, src) (callstack+1)->argstack_set_ptr[src] = reg[dst];
#define GPERL_PUSH(dst, src) (callstack+1)->argstack_set_ptr[src] = reg[dst];
#define GPERL_NEW() do {												\
		root.callstack_top = callstack;									\
		root.stack_top_idx = pc->cur_stack_top;							\
		callstack->cur_pc = pc;											\
		OBJECT_init(reg[pc->dst], pc->_new(pc->v, (callstack+1)->argstack)); \
	} while (0)
#define GPERL_NEW_STRING() do {											\
		root.callstack_top = callstack;									\
		root.stack_top_idx = pc->cur_stack_top;							\
		callstack->cur_pc = pc;											\
		STRING_init(reg[pc->dst], (GPerlString *)pc->_new(pc->v, NULL)); \
	} while (0)
#define GPERL_ARRAY_PUSH(argstack) pc->push(argstack);

#define GPERL_ARRAY_ARGAT(dst, src, idx) do {					\
		GPerlArray *a = (GPerlArray *)getObject(argstack[src]);	\
		if (a->size > idx) {									\
			reg[dst] = a->list[idx];							\
		} else {												\
			OBJECT_init(reg[dst], undef);						\
		}														\
	} while (0);

#define GPERL_ARRAY_AT(dst, src, idx) do {						\
		GPerlArray *a = (GPerlArray *)getObject(stack[src]);	\
		if (a->size > I(idx)) {									\
			reg[dst] = a->list[I(idx)];							\
		} else {												\
			root.callstack_top = callstack;						\
			root.stack_top_idx = pc->cur_stack_top;				\
			callstack->cur_pc = pc;								\
			OBJECT_init(reg[dst], undef);						\
		}														\
	} while (0);

#define GPERL_ARRAY_ATC(dst, src, idx) do {						\
		GPerlArray *a = (GPerlArray *)getObject(stack[src]);	\
		if (a->size > idx) {									\
			reg[dst] = a->list[idx];							\
		} else {												\
			root.callstack_top = callstack;						\
			root.stack_top_idx = pc->cur_stack_top;				\
			callstack->cur_pc = pc;								\
			OBJECT_init(reg[dst], undef);						\
		}														\
	} while (0);

#define GPERL_ARRAY_gAT(dst, src, idx) do {								\
		GPerlArray *a = (GPerlArray *)getObject(global_vmemory[src]);	\
		if (a->size > I(idx)) {									\
			reg[dst] = a->list[I(idx)];									\
		} else {														\
			root.callstack_top = callstack;								\
			root.stack_top_idx = pc->cur_stack_top;						\
			callstack->cur_pc = pc;										\
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
			callstack->cur_pc = pc;										\
			OBJECT_init(reg[dst], undef);								\
		}																\
	} while (0);

#define GPERL_ARRAY_SET(dst, src, idx) do {								\
		GPerlArray *a =(GPerlArray *)getObject(stack[dst]);				\
		if (a->size > I(idx)) {											\
			a->list[I(idx)] = reg[src];									\
		} else {														\
			size_t new_size = I(idx);									\
			void *tmp;													\
			if (!(tmp = realloc(a->list, sizeof(GPerlValue) * (new_size)))) { \
				fprintf(stderr, "ERROR!!: cannot allocated memory\n");	\
			} else {													\
				a->list = (GPerlValue *)tmp;							\
				for (size_t i = a->size; i < new_size - 1; i++) {		\
					root.callstack_top = callstack;						\
					root.stack_top_idx = pc->cur_stack_top;				\
					callstack->cur_pc = pc;								\
					OBJECT_init(a->list[i], undef);						\
				}														\
				a->list[new_size - 1] = reg[dst];						\
				a->size = new_size;										\
			}															\
		}																\
	} while (0)

#define GPERL_ARRAY_gSET(dst, src, idx) do {							\
		GPerlArray *a = (GPerlArray *)getObject(global_vmemory[dst]);	\
		if (a->size > I(idx)) {									\
			a->list[I(idx)] = reg[src];									\
		} else {														\
			size_t new_size = I(idx);									\
			void *tmp;													\
			if (!(tmp = realloc(a->list, sizeof(GPerlValue) * (new_size)))) { \
				fprintf(stderr, "ERROR!!: cannot allocated memory\n");	\
			} else {													\
				a->list = (GPerlValue *)tmp;							\
				for (size_t i = a->size; i < new_size - 1; i++) {		\
					root.callstack_top = callstack;						\
					root.stack_top_idx = pc->cur_stack_top;				\
					callstack->cur_pc = pc;								\
					OBJECT_init(a->list[i], undef);						\
				}														\
				a->list[new_size - 1] = reg[dst];						\
				a->size = new_size;										\
			}															\
		}																\
	} while (0)

#define GPERL_ARRAY_DREF(dst, src) do {									\
		root.callstack_top = callstack;									\
		root.stack_top_idx = pc->cur_stack_top;							\
		callstack->cur_pc = pc;											\
		GPerlArray *ref_a = (GPerlArray *)getObject(reg[src]);			\
		GPerlArray *a = (GPerlArray *)new_GPerlArray(reg[src], ref_a->list); \
		a->h.type = Array;												\
		OBJECT_init(reg[dst], a);										\
	} while (0);

#define GPERL_HASH_AT(dst, src, idx) do {								\
		GPerlObject *o = (GPerlObject *)getObject(stack[src]);			\
		GPerlHash *h = NULL;											\
		if (o->h.type == Hash || o->h.type == HashRef) {				\
			h = (GPerlHash *)o;											\
		} else {														\
			h = ((GPerlClass *)o)->fields;								\
		}																\
		GPerlString *key = getStringObj(reg[idx]);						\
		reg[dst] = h->table[key->hash];									\
	} while (0);

#define GPERL_HASH_gAT(dst, src, idx) do {								\
		GPerlObject *o = (GPerlObject *)getObject(global_vmemory[src]);	\
		GPerlHash *h = NULL;											\
		if (o->h.type == Hash || o->h.type == HashRef) {				\
			h = (GPerlHash *)o;											\
		} else {														\
			h = ((GPerlClass *)o)->fields;								\
		}																\
		GPerlString *key = getStringObj(reg[idx]);						\
		reg[dst] = h->table[key->hash];									\
	} while (0);

#define GPERL_HASH_ATC(dst, src, hash) do {								\
		GPerlObject *o = (GPerlObject *)getObject(stack[src]);			\
		GPerlHash *h = NULL;											\
		if (o->h.type == Hash || o->h.type == HashRef) {				\
			h = (GPerlHash *)o;											\
		} else {														\
			h = ((GPerlClass *)o)->fields;								\
		}																\
		reg[dst] = h->table[hash];										\
	} while (0);

#define GPERL_HASH_gATC(dst, src, hash) do {							\
		GPerlObject *o = (GPerlObject *)getObject(global_vmemory[src]);	\
		GPerlHash *h = NULL;											\
		if (o->h.type == Hash || o->h.type == HashRef) {				\
			h = (GPerlHash *)o;											\
		} else {														\
			h = ((GPerlClass *)o)->fields;								\
		}																\
		reg[dst] = h->table[hash];										\
	} while (0);

#define GPERL_HASH_SET(dst, src, idx)
#define GPERL_HASH_gSET(dst, src, idx)
#define GPERL_HASH_DREF(dst, src) do {									\
		root.callstack_top = callstack;									\
		root.stack_top_idx = pc->cur_stack_top;							\
		callstack->cur_pc = pc;											\
		GPerlHash *ref_h = (GPerlHash *)getObject(reg[src]);			\
		GPerlHash *h = GPerlHash_copy(ref_h);							\
		h->h.type = Hash;												\
		OBJECT_init(reg[dst], h);										\
	} while (0);

#define GPERL_CODE_REF(dst, src) do {									\
		OBJECT_init(reg[dst], new_GPerlFunc(pc->name, func_memory[src])); \
	} while (0);

#define GPERL_EACH_INIT(dst, src) do {									\
		GPerlArray *a = (GPerlArray *)getObject(stack[src]);			\
		OBJECT_init(stack[dst], new_GPerlInitArray(a->list, a->size));	\
	} while (0);

#define GPERL_EACH_gINIT(dst, src) do {									\
		GPerlArray *a = (GPerlArray *)getObject(global_vmemory[src]);	\
		OBJECT_init(stack[dst], new_GPerlInitArray(a->list, a->size));	\
	} while (0);

#define GPERL_EACH_rINIT(dst, src) do {									\
		GPerlArray *a = (GPerlArray *)getObject(reg[src]);				\
		OBJECT_init(stack[dst], new_GPerlInitArray(a->list, a->size));	\
	} while (0);

#define GPERL_EACH_LET(dst, src) do {									\
		GPerlArray *a = (GPerlArray *)getObject(stack[src]);			\
		if (a->size > 0) {												\
			stack[dst] = a->list[0];									\
			pc++;														\
		} else {														\
			pc += pc->jmp;												\
		}																\
	} while (0);

#define GPERL_EACH_STEP(dst, src) do {							\
		GPerlArray *a = (GPerlArray *)getObject(stack[src]);	\
		a->list++;												\
		a->size--;												\
		pc += pc->jmp;											\
	} while (0);
