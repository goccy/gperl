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

unsigned int GPerlJITCompiler::compile(GPerlVirtualMachineCode *codes, GPerlJITEnv *env)
{
    GPerlVirtualMachineCode *pc = codes;
    return _compile(pc, env);
}

unsigned int GPerlJITCompiler::_compile(GPerlVirtualMachineCode *codes, GPerlJITEnv *env)
{
    GPerlVirtualMachineCode *pc = codes;
    jit_context_t ctx = jit_context_create();
    jit_context_build_start(ctx);
    GPerlArgsArray *args = env->args;
    int argc = args->size;
    int arg_size = argc;
    jit_type_t _params[argc];
    int i = 0;
    for (i = 0; i < argc; i++) {
        switch (TYPE_CHECK(args->list[i])) {
        case 0: /* Double */
            _params[i] = jit_type_nfloat;
            break;
        case 1: /* Int */
            _params[i] = jit_type_int;
            break;
        case 2: /* String */
            break;
        case 3: /* Object */
            break;
        default:
            break;
        }
    }
    jit_value_t curstack[32] = {0};
    jit_value_t argstack[MAX_ARGSTACK_SIZE] = {0};
    jit_type_t _signature = jit_type_create_signature(jit_abi_fastcall, jit_type_uint, _params, argc, 0);
    jit_function_t func = jit_function_create(ctx, _signature);
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
            curstack[pc->dst] = _v[pc->src];
            break;
        case MOV:
            _v[pc->dst] = compileMOV(pc, &func);
            break;
        case vMOV:
            _v[pc->dst] = curstack[pc->src];
            break;
        case gMOV:
            break;
        case ARGMOV:
            _v[pc->dst] = jit_value_get_param(func, pc->src);
            break;
        case ADD:
            _v[pc->dst] = jit_insn_add(func, _v[pc->dst], _v[pc->src]);
            break;
        case iADDC:
            break;
        case iJLC: {
            jit_value_t c = jit_value_create_nint_constant(func, jit_type_int, pc->v.ivalue);
            jit_value_t tmp = jit_insn_lt(func, _v[pc->dst], c);
            GPerlJmpInfo *inf = new GPerlJmpInfo(pc->jmp);
            jmp_stack->push(inf);
            jit_insn_branch_if_not(func, tmp, &inf->label);
            break;
        }
        case iJLEC: {
            jit_value_t c = jit_value_create_nint_constant(func, jit_type_int, pc->v.ivalue);
            jit_value_t tmp = jit_insn_le(func, _v[pc->dst], c);
            GPerlJmpInfo *inf = new GPerlJmpInfo(pc->jmp);
            jmp_stack->push(inf);
            jit_insn_branch_if_not(func, tmp, &inf->label);
            break;
        }
        case JE: {
            jit_value_t tmp = jit_insn_eq(func, _v[pc->dst], _v[pc->src]);
            GPerlJmpInfo *inf = new GPerlJmpInfo(pc->jmp);
            jmp_stack->push(inf);
            jit_insn_branch_if_not(func, tmp, &inf->label);
            break;
        }
        case JLE: {
            jit_value_t tmp = jit_insn_le(func, _v[pc->dst], _v[pc->src]);
            GPerlJmpInfo *inf = new GPerlJmpInfo(pc->jmp);
            jmp_stack->push(inf);
            jit_insn_branch_if_not(func, tmp, &inf->label);
            break;
        }
        case iSUBC: {
            jit_value_t c = jit_value_create_nint_constant(func, jit_type_int, pc->v.ivalue);
            _v[pc->dst] = jit_insn_sub(func, _v[pc->dst], c);
            break;
        }
        case PUSH:
            argstack[pc->src] = _v[pc->dst];
            argc++;
            break;
        case SELFCALL: {
            _v[pc->dst] = jit_insn_call(func, "", func, NULL, argstack, argc, 0);//JIT_CALL_TAIL);
            argc = 0;
            break;
        }
        case RET:
            _v[0] = _v[pc->src];
            jit_insn_return(func, _v[0]);
            break;
        default:
            break;
        }
    }
    jit_function_compile(func);
    jit_context_build_end(ctx);
    void *jit_args[argc];
    jit_uint result;
    for (int i = 0; i < arg_size; i++) {
        switch (TYPE_CHECK(args->list[i])) {
        case 0: /* Double */
            jit_args[i] = &args->list[i].dvalue;
            break;
        case 1: /* Int */
            jit_args[i] = &args->list[i].ivalue;
            break;
        case 2: /* String */
            break;
        case 3: /* Object */
            break;
        default:
            break;
        }
    }
    jit_function_apply(func, jit_args, &result);
    //fprintf(stderr, "result = [%u]\n", (unsigned int)result);
    return (unsigned int)result;
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
