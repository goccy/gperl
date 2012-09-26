#include <gperl.hpp>

#ifdef USING_JIT

GPerlJmpInfo::GPerlJmpInfo(int _jmp_count) : jmp_count(_jmp_count)
{
	label = jit_label_undefined;
}

GPerlJmpStack::GPerlJmpStack(void)
{
	infs = (GPerlJmpInfo **)safe_malloc(sizeof(GPerlJmpInfo) * MAX_JMP_STACK_SIZE);
	jmp_stack_idx = 0;
}

void GPerlJmpStack::push(GPerlJmpInfo *inf)
{
	infs[jmp_stack_idx] = inf;
	jmp_stack_idx++;
	if (jmp_stack_idx > MAX_JMP_STACK_SIZE) {
		fprintf(stderr, "Overflow JMP_STACK_SIZE\n");
	}
}

GPerlJmpInfo *GPerlJmpStack::pop(void)
{
	GPerlJmpInfo *inf = infs[--jmp_stack_idx];
	infs[jmp_stack_idx] = NULL;
	return inf;
}

bool GPerlJmpStack::isJmp(void)
{
	bool ret = false;
	for (int i = 0; i < jmp_stack_idx; i++) {
		infs[i]->jmp_count--;
		if (infs[i]->jmp_count < 1) {
			ret = true;
			break;
		}
	}
	return ret;
}

GPerlJITCompiler::GPerlJITCompiler(void)
{
}

jit_function_t GPerlJITCompiler::compile(JITParam *param)
{
	GPerlVirtualMachineCode *pc = param->mtd;
	jit_context_t ctx = jit_context_create();
	jit_context_build_start(ctx);
	int argc = param->argc;
	jit_type_t _params[argc];
	for (int i = 0; i < argc; i++) {
		switch (param->arg_types[i]) {
		case Int:
			_params[i] = jit_type_nfloat;
			break;
		case Double:
			_params[i] = jit_type_int;
			break;
		case String:
			break;
		case Object:
			break;
		default:
			break;
		}
	}
	jit_value_t curstack[32] = {0};
	jit_value_t argstack[MAX_ARGSTACK_SIZE] = {0};
	jit_type_t signature = jit_type_create_signature(jit_abi_fastcall, jit_type_uint, _params, argc, 0);
	jit_function_t func = jit_function_create(ctx, signature);
	jit_value_t _v[MAX_REG_SIZE] = {0};
	GPerlJmpStack *jmp_stack = new GPerlJmpStack();
	argc = 0;
	for (; pc->op != UNDEF; pc++) {
		if (jmp_stack->isJmp()) {
			GPerlJmpInfo *inf = jmp_stack->pop();
			jit_insn_label(func, &inf->label);
		}
		switch (pc->op) {
		case LET:
			DBG_PL("COMPILE LET");
			curstack[pc->dst] = _v[pc->src];
			break;
		case MOV:
			DBG_PL("COMPILE MOV");
			_v[pc->dst] = compileMOV(pc, &func);
			break;
		case vMOV:
			DBG_PL("COMPILE vMOV");
			_v[pc->dst] = curstack[pc->src];
			break;
		case gMOV:
			break;
		case ARGMOV:
			DBG_PL("COMPILE ARGMOV");
			_v[pc->dst] = jit_value_get_param(func, pc->src);
			break;
		case ADD:
			DBG_PL("COMPILE ADD");
			_v[pc->dst] = jit_insn_add(func, _v[pc->dst], _v[pc->src]);
			break;
		case iADDC:
			break;
		case iJLC: {
			DBG_PL("COMPILE iJLC");
			jit_value_t c = jit_value_create_nint_constant(func, jit_type_int, pc->v.ivalue);
			jit_value_t tmp = jit_insn_lt(func, _v[pc->dst], c);
			GPerlJmpInfo *inf = new GPerlJmpInfo(pc->jmp);
			jmp_stack->push(inf);
			jit_insn_branch_if_not(func, tmp, &inf->label);
			break;
		}
		case iJLEC: {
			DBG_PL("COMPILE iJLEC");
			jit_value_t c = jit_value_create_nint_constant(func, jit_type_int, pc->v.ivalue);
			jit_value_t tmp = jit_insn_le(func, _v[pc->dst], c);
			GPerlJmpInfo *inf = new GPerlJmpInfo(pc->jmp);
			jmp_stack->push(inf);
			jit_insn_branch_if_not(func, tmp, &inf->label);
			break;
		}
		case JE: {
			DBG_PL("COMPILE JE");
			jit_value_t tmp = jit_insn_eq(func, _v[pc->dst], _v[pc->src]);
			GPerlJmpInfo *inf = new GPerlJmpInfo(pc->jmp);
			jmp_stack->push(inf);
			jit_insn_branch_if_not(func, tmp, &inf->label);
			break;
		}
		case JLE: {
			DBG_PL("COMPILE JLE");
			jit_value_t tmp = jit_insn_le(func, _v[pc->dst], _v[pc->src]);
			GPerlJmpInfo *inf = new GPerlJmpInfo(pc->jmp);
			jmp_stack->push(inf);
			jit_insn_branch_if_not(func, tmp, &inf->label);
			break;
		}
		case iSUBC: {
			DBG_PL("COMPILE iSUBC");
			jit_value_t c = jit_value_create_nint_constant(func, jit_type_int, pc->v.ivalue);
			_v[pc->dst] = jit_insn_sub(func, _v[pc->dst], c);
			break;
		}
		case PUSH:
			DBG_PL("COMPILE PUSH");
			argstack[pc->src] = _v[pc->dst];
			argc++;
			break;
		case SELFCALL: {
			DBG_PL("COMPILE SELFCALL");
			_v[pc->dst] = jit_insn_call(func, "", func, NULL, argstack, argc, 0);//JIT_CALL_TAIL);
			argc = 0;
			break;
		}
		case SELF_FASTCALL0: {
			DBG_PL("COMPILE FASTCALL0");
			argstack[0] = _v[pc->arg0];
			_v[pc->dst] = jit_insn_call(func, "", func, NULL, argstack, 1, 0);//JIT_CALL_TAIL);
			break;
		}
		case SELF_FASTCALL1: {
			DBG_PL("COMPILE FASTCALL1");
			argstack[0] = _v[pc->arg0];
			argstack[1] = _v[pc->arg1];
			_v[pc->dst] = jit_insn_call(func, "", func, NULL, argstack, 2, 0);//JIT_CALL_TAIL);
			break;
		}
		case SELF_FASTCALL2: {
			DBG_PL("COMPILE FASTCALL2");
			argstack[0] = _v[pc->arg0];
			argstack[1] = _v[pc->arg1];
			argstack[2] = _v[pc->arg2];
			_v[pc->dst] = jit_insn_call(func, "", func, NULL, argstack, 3, 0);//JIT_CALL_TAIL);
			break;
		}
		case SELF_FASTCALL3: {
			DBG_PL("COMPILE FASTCALL3");
			argstack[0] = _v[pc->arg0];
			argstack[1] = _v[pc->arg1];
			argstack[2] = _v[pc->arg2];
			argstack[3] = _v[pc->arg3];
			_v[pc->dst] = jit_insn_call(func, "", func, NULL, argstack, 4, 0);//JIT_CALL_TAIL);
			break;
		}
		case RET:// case JIT_COUNTDOWN_RET:
			DBG_PL("COMPILE RET");
			_v[0] = _v[pc->src];
			jit_insn_return(func, _v[0]);
			break;
		default:
			DBG_PL("COMPILE DEFALUT");
			break;
		}
	}
	jit_function_compile(func);
	jit_context_build_end(ctx);
	return func;
}

GPerlValue GPerlJITCompiler::run(jit_function_t func, GPerlValue *args, JITParam *param)
{
	GPerlValue ret;
	int argc = param->argc;
	void *jit_args[argc];
	asm("int3");
	for (int i = 0; i < argc; i++) {
		switch (param->arg_types[i]) {
		case Int:
			jit_args[i] = &args[i].ivalue;
			break;
		case Double:
			jit_args[i] = &args[i].dvalue;
			break;
		case String:
			break;
		case Object:
			break;
		default:
			break;
		}
	}
	switch (param->return_type) {
	case Int: {
		unsigned int rvalue;
		jit_function_apply(func, jit_args, &rvalue);
		DBG_PL("rvalue = [%d]\n", rvalue);
		INT_init(ret, rvalue);
		break;
	}
	case Double: {
		double rvalue;
		jit_function_apply(func, jit_args, &rvalue);
		DBG_PL("rvalue = [%f]\n", rvalue);
		DOUBLE_init(ret, rvalue);
		break;
	}
	case String:
		break;
	case Object:
		break;
	default:
		break;
	}
	return ret;
}

jit_value_t GPerlJITCompiler::compileMOV(GPerlVirtualMachineCode *pc, jit_function_t *func)
{
	jit_value_t ret = NULL;
	switch (TYPE_CHECK(pc->v)) {
	case 0:
		break;
	case 1:
		ret = jit_value_create_nint_constant(*func, jit_type_int, pc->v.ivalue);
		break;
	case 2:
		break;
	case 3:
		break;
	default:
		break;
	}
	return ret;
}

#endif
