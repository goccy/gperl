#include <gperl.hpp>

using namespace std;

GPerlCompiler::GPerlCompiler(void) : dst(0), src(0), code_num(0), variable_index(0)
{
	declared_vname = NULL;
	for (int i = 0; i < MAX_REG_SIZE; i++) {
		reg_type[i] = Undefined;
	}
	for (int i = 0; i < MAX_VARIABLE_NUM; i++) {
		variable_names[i] = NULL;
		variable_types[i] = Undefined;
	}
	codes = new vector<GPerlVirtualMachineCode *>();
}

GPerlVirtualMachineCode *GPerlCompiler::compile(GPerlAST *ast)
{
	GPerlCell *root = ast->root;
	GPerlVirtualMachineCode *thcode = createTHCODE();
	addVMCode(thcode);
	for (; root; root = root->next) {
		GPerlCell *path = root;
		compile_(path, true);
		DBG_P("============================");
		dst = 0;//reset dst number
	}
	GPerlVirtualMachineCode *ret = createRET();
	addVMCode(ret);
	return getPureCodes();
}

void GPerlCompiler::addVMCode(GPerlVirtualMachineCode *code)
{
	codes->push_back(code);
}

void GPerlCompiler::compile_(GPerlCell *path, bool isRecursive)
{
	GPerlVirtualMachineCode *code;
	GPerlCell *p = path;
	for (;p->vargs && isRecursive; p = p->vargs) {
		//path : PrintDecl
		compile_(p->vargs, false);
		//add WRITE CODE for Print Stmt
		switch (reg_type[0]) {
		case Int:
			code = createiWRITE();
			addVMCode(code);
			dumpVMCode(code);
			break;
		case String:
			code = createsWRITE();
			addVMCode(code);
			dumpVMCode(code);
			break;
		default:
			break;
		}
		dst = 0;
	}
	for (; path->left; path = path->left) {}
	code = createVMCode(path);
	addVMCode(code);
	dumpVMCode(code);
	while (path->parent) {
		GPerlCell *parent = path->parent;
		GPerlCell *branch = parent->right;
		if (branch == path) return;
		if (branch->left == NULL && branch->right == NULL) {
			code = createVMCode(branch);
			addVMCode(code);
			dumpVMCode(code);
		} else if (branch->type != Return) {
			compile_(branch, true);
		}
		code = createVMCode(parent);
		addVMCode(code);
		dumpVMCode(code);
		path = parent;
	}
	//GPerlTokenizer t;
	//DBG_P("%s", t.getTypeName(path->type));
	if (path->type == IfStmt) {
		dst = 0;//reset dst number
		GPerlCell *true_stmt = path->true_stmt->root;
		GPerlVirtualMachineCode *jmp = codes->at(code_num - 2);
		DBG_P("jmp = [%d]", jmp->op);
		int cond_code_num = code_num;
		DBG_P("============TRUE STMT=============");
		for (; true_stmt; true_stmt = true_stmt->next) {
			GPerlCell *path = true_stmt;
			compile_(path, true);
			dst = 0;//reset dst number
		}
		jmp->jmp = code_num - cond_code_num + 3/*OPNOP + OPJMP + 1*/;
		int cur_code_num = code_num;
		//fprintf(stderr, "cur_code_num = [%d]\n", cur_code_num);
		jmp = createJMP(1);
		addVMCode(jmp);
		dumpVMCode(jmp);
		dst = 0;//reset dst number
		DBG_P("============FALSE STMT=============");
		if (path->false_stmt) {
			GPerlCell *false_stmt = path->false_stmt->root;
			for (; false_stmt; false_stmt = false_stmt->next) {
				GPerlCell *path = false_stmt;
				compile_(path, true);
				dst = 0;//reset dst number
			}
		}
		jmp->jmp = code_num - cur_code_num;
	}
}

GPerlVirtualMachineCode *GPerlCompiler::getPureCodes(void)
{
	int code_n = codes->size();
	GPerlVirtualMachineCode pure_codes_[code_n + 1];
	memset(pure_codes_ + code_n + 1, 0, sizeof(GPerlVirtualMachineCode));
	for (int i = 0; i < code_n; i++) {
		pure_codes_[i] = *codes->at(i);
	}
	int code_size = code_n * sizeof(GPerlVirtualMachineCode);
	GPerlVirtualMachineCode *pure_codes = (GPerlVirtualMachineCode *)malloc(code_size);
	memcpy(pure_codes, pure_codes_, code_size);
	return pure_codes;
}

GPerlVirtualMachineCode *GPerlCompiler::createVMCode(GPerlCell *c)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	switch (c->type) {
	case Int:
		code->dst = dst;
		code->src = c->data.idata;
		code->op = OPiMOV;
		reg_type[dst] = Int;
		dst++;
		break;
	case String:
		code->dst = dst;
		code->src = -1;
		code->name = c->data.sdata;
		code->op = OPsMOV;
		reg_type[dst] = String;
		dst++;
		break;
	case Add:
		dst--;
		switch (reg_type[dst - 1]) {
		case Int:
			code->op = OPiADD;
			break;
		default:
			code->op = OPADD;
			break;
		}
		code->dst = dst - 1;
		code->src = dst;
		break;
	case Sub:
		dst--;
		switch (reg_type[dst - 1]) {
		case Int:
			code->op = OPiSUB;
			break;
		default:
			code->op = OPSUB;
			break;
		}
		code->dst = dst - 1;
		code->src = dst;
		break;
	case Mul:
		dst--;
		switch (reg_type[dst - 1]) {
		case Int:
			code->op = OPiMUL;
			break;
		default:
			code->op = OPMUL;
			break;
		}
		code->dst = dst - 1;
		code->src = dst;
		break;
	case Div:
		dst--;
		switch (reg_type[dst - 1]) {
		case Int:
			code->op = OPiDIV;
			break;
		default:
			code->op = OPDIV;
			break;
		}
		code->dst = dst - 1;
		code->src = dst;
		break;
	case Greater:
		dst--;
		switch (reg_type[dst - 1]) {
		case Int:
			code->op = OPiJG;
			break;
		default:
			code->op = OPJG;
			break;
		}
		code->jmp = 1;
		code->dst = dst - 1;
		code->src = dst;
		break;
	case Less:
		dst--;
		switch (reg_type[dst - 1]) {
		case Int:
			code->op = OPiJL;
			break;
		default:
			code->op = OPJL;
			break;
		}
		code->jmp = 1;
		code->dst = dst - 1;
		code->src = dst;
		break;
	case GreaterEqual:
		dst--;
		switch (reg_type[dst - 1]) {
		case Int:
			code->op = OPiJGE;
			break;
		default:
			code->op = OPJGE;
			break;
		}
		code->jmp = 1;
		code->dst = dst - 1;
		code->src = dst;
		break;
	case LessEqual:
		dst--;
		switch (reg_type[dst - 1]) {
		case Int:
			code->op = OPiJLE;
			break;
		default:
			code->op = OPJLE;
			break;
		}
		code->jmp = 1;
		code->dst = dst - 1;
		code->src = dst;
		break;
	case EqualEqual:
		dst--;
		switch (reg_type[dst - 1]) {
		case Int:
			code->op = OPiJE;
			break;
		default:
			code->op = OPJE;
			break;
		}
		code->jmp = 1;
		code->dst = dst - 1;
		code->src = dst;
		break;
	case PrintDecl:
		code->op = OPPRINT;
		code->dst = 0;
		code->src = 0;
		break;
	case IfStmt:
		code->op = OPNOP;
		code->dst = 0;
		code->src = 0;
		break;
	case LocalVarDecl: case VarDecl: case GlobalVarDecl: {
		code->op = OPSET;
		code->dst = variable_index;
		code->src = 0;
		const char *name = cstr(c->vname);
		code->name = name;
		setToVariableNames(name);
		declared_vname = name;
		break;
	}
	case LocalVar: case Var: {
		const char *name = cstr(c->vname);
		int idx = getVariableIndex(name);
		switch (variable_types[idx]) {
		case Int:
			code->op = OPOiMOV;
			reg_type[dst] = Int;
			break;
		default:
			code->op = OPOMOV;
			reg_type[dst] = Object;
			break;
		}
		code->dst = dst;
		code->src = idx;
		code->name = name;
		dst++;
		break;
	}
	case Assign: {
		code->op = OPLET;
		int idx = getVariableIndex(declared_vname);
		code->dst = idx;
		code->src = 0;
		code->name = declared_vname;
		/* ======= for Type Inference ======= */
		switch (reg_type[0]) {
		case Int:
			variable_types[idx] = Int;
			break;
		case Float:
			variable_types[idx] = Float;
			break;
		case String:
			variable_types[idx] = String;
			break;
		default:
			break;
		}
		/* ================================== */
		declared_vname = NULL;
		break;
	}
	default:
		break;
	}
	code_num++;
	return code;
}

void GPerlCompiler::setToVariableNames(const char *name)
{
	variable_names[variable_index] = name;
	variable_index++;
}

int GPerlCompiler::getVariableIndex(const char *name)
{
	int ret = -1;
	size_t name_size = strlen(name);
	for (int i = 0; i < variable_index; i++) {
		size_t v_size = strlen(variable_names[i]);
		if (v_size == name_size &&
			!strncmp(variable_names[i], name, name_size)) {
			ret = i;
			break;
		}
	}
	if (ret == -1) {
		fprintf(stderr, "COMPILE ERROR: cannot find variable name[%s]\n", name);
		exit(1);
	}
	return ret;
}

GPerlVirtualMachineCode *GPerlCompiler::createTHCODE(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = OPTHCODE;
	code_num++;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createRET(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = OPRET;
	code_num++;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createiWRITE(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = OPiWRITE;
	code_num++;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createsWRITE(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = OPsWRITE;
	code_num++;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createJMP(int jmp_num)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = OPJMP;
	code->jmp = jmp_num;
	code_num++;
	return code;
}

void GPerlCompiler::dumpVMCode(GPerlVirtualMachineCode *code)
{
	switch (code->op) {
	case OPMOV:
		DBG_P("L[%d] : OPMOV [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	case OPiMOV:
		DBG_P("L[%d] : OPiMOV [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	case OPsMOV:
		DBG_P("L[%d] : OPsMOV [%d], [%s]", code->code_num, code->dst, code->name);
		break;
	case OPOMOV:
		DBG_P("L[%d] : OPOMOV [%d], [%d], [%s]", code->code_num, code->dst, code->src, code->name);
		break;
	case OPOiMOV:
		DBG_P("L[%d] : OPOiMOV [%d], [%d], [%s]", code->code_num, code->dst, code->src, code->name);
		break;
	case OPADD:
		DBG_P("L[%d] : OPADD [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	case OPiADD:
		DBG_P("L[%d] : OPiADD [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	case OPSUB:
		DBG_P("L[%d] : OPSUB [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	case OPiSUB:
		DBG_P("L[%d] : OPiSUB [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	case OPMUL:
		DBG_P("L[%d] : OPMUL [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	case OPiMUL:
		DBG_P("L[%d] : OPiMUL [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	case OPDIV:
		DBG_P("L[%d] : OPDIV [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	case OPiDIV:
		DBG_P("L[%d] : OPiDIV [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	case OPJL:
		DBG_P("L[%d] : OPJL [%d], [%d], [%d]", code->code_num, code->dst, code->src, code->jmp);
		break;
	case OPiJL:
		DBG_P("L[%d] : OPiJL [%d], [%d], [%d]", code->code_num, code->dst, code->src, code->jmp);
		break;
	case OPJG:
		DBG_P("L[%d] : OPJG [%d], [%d], [%d]", code->code_num, code->dst, code->src, code->jmp);
		break;
	case OPiJG:
		DBG_P("L[%d] : OPiJG [%d], [%d], [%d]", code->code_num, code->dst, code->src, code->jmp);
		break;
	case OPJGE:
		DBG_P("L[%d] : OPJGE [%d], [%d], [%d]", code->code_num, code->dst, code->src, code->jmp);
		break;
	case OPiJGE:
		DBG_P("L[%d] : OPiJGE [%d], [%d], [%d]", code->code_num, code->dst, code->src, code->jmp);
		break;
	case OPJLE:
		DBG_P("L[%d] : OPJLE [%d], [%d], [%d]", code->code_num, code->dst, code->src, code->jmp);
		break;
	case OPiJLE:
		DBG_P("L[%d] : OPiJLE [%d], [%d], [%d]", code->code_num, code->dst, code->src, code->jmp);
		break;
	case OPJE:
		DBG_P("L[%d] : OPJE [%d], [%d], [%d]", code->code_num, code->dst, code->src, code->jmp);
		break;
	case OPiJE:
		DBG_P("L[%d] : OPiJE [%d], [%d], [%d]", code->code_num, code->dst, code->src, code->jmp);
		break;
	case OPRET:
		DBG_P("L[%d] : OPRET [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	case OPTHCODE:
		DBG_P("L[%d] : OPTHCODE [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	case OPNOP:
		DBG_P("L[%d] : OPNOP [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	case OPPRINT:
		DBG_P("L[%d] : OPPRINT [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	case OPsWRITE:
		DBG_P("L[%d] : OPsWRITE [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	case OPiWRITE:
		DBG_P("L[%d] : OPiWRITE [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	case OPJMP:
		DBG_P("L[%d] : OPJMP [%d], [%d], [%d]", code->code_num, code->dst, code->src, code->jmp);
		break;
	case OPLET:
		DBG_P("L[%d] : OPLET [%s]:[%d], [%d]", code->code_num, code->name, code->dst, code->src);
		break;
	case OPSET:
		DBG_P("L[%d] : OPSET [%d], [%s]", code->code_num, code->dst, code->name);
		break;
	default:
		break;
	}
}

void GPerlCompiler::dumpPureVMCode(GPerlVirtualMachineCode *c)
{
	int code_n = codes->size();
	for (int i = 0; i < code_n; i++) {
		dumpVMCode(&c[i]);
	}
}
