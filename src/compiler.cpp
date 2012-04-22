#include <gperl.hpp>
#include <code.hpp>
using namespace std;

GPerlCompiler::GPerlCompiler(void) : dst(0), src(0), code_num(0),
									 variable_index(0), func_index(0),
									 args_count(0)
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
	func_code = NULL;
}

GPerlVirtualMachineCode *GPerlCompiler::compile(GPerlAST *ast)
{
	GPerlCell *root = ast->root;
	GPerlVirtualMachineCode *thcode = createTHCODE();
	addVMCode(thcode);
	for (; root; root = root->next) {
		GPerlCell *path = root;
		compile_(path, true);
		DBG_PL("============================");
		dst = 0;//reset dst number
	}
	GPerlVirtualMachineCode *ret = createRET();
	addVMCode(ret);
	GPerlVirtualMachineCode *undef = createUNDEF();//for threaded code
	addVMCode(undef);
	return getPureCodes(codes);
}

void GPerlCompiler::addVMCode(GPerlVirtualMachineCode *code)
{
	codes->push_back(code);
}

void GPerlCompiler::popVMCode()
{
	codes->pop_back();
}

void GPerlCompiler::compile_(GPerlCell *path, bool isRecursive)
{
	GPerlVirtualMachineCode *code;
	GPerlCell *p = path;
	for (;p->vargs && isRecursive; p = p->vargs) {
		//path : PrintDecl
		compile_(p->vargs, false);
		if (path->type == PrintDecl) {
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
			case Object:
				//type check
				if (reg_type[1] == Int) {
					code = createiWRITE();
				} else {
					code = createoWRITE();
				}
				addVMCode(code);
				dumpVMCode(code);
				break;
			default:
				break;
			}
		} else if (path->type == Call) {
			switch (reg_type[0]) {
			case Int:
				code = createiPUSH();
				addVMCode(code);
				dumpVMCode(code);
				break;
			case String:
				code = createsPUSH();
				addVMCode(code);
				dumpVMCode(code);
				break;
			default:
				break;
			}
		}
		dst = 0;
	}
	for (; path->left; path = path->left) {}
	if (path->type == Call) {
		compile_(path->vargs, false);
		switch (reg_type[0]) {
		case Int:
			code = createiPUSH();
			addVMCode(code);
			dumpVMCode(code);
			break;
		case String:
			code = createsPUSH();
			addVMCode(code);
			dumpVMCode(code);
			break;
		case Object:
			code = createiPUSH();//TODO
			addVMCode(code);
			dumpVMCode(code);
			break;
		default:
			break;
		}
	}
	code = createVMCode(path);
	addVMCode(code);
	dumpVMCode(code);
	while (path->parent) {
		GPerlCell *parent = path->parent;
		GPerlCell *branch = parent->right;
		if (branch) {
			if (branch == path) return;
			if (branch->type == Call) {
				compile_(branch->vargs, false);
				switch (reg_type[0]) {
				case Int:
					code = createiPUSH();
					addVMCode(code);
					dumpVMCode(code);
					break;
				case String:
					code = createsPUSH();
					addVMCode(code);
					dumpVMCode(code);
					break;
				case Object:
					code = createiPUSH();//TODO
					addVMCode(code);
					dumpVMCode(code);
					break;
				default:
					break;
				}
				code = createVMCode(branch);
				addVMCode(code);
				dumpVMCode(code);
			} else if (branch->left == NULL && branch->right == NULL) {
				code = createVMCode(branch);
				addVMCode(code);
				dumpVMCode(code);
			} else if (branch->type != Return) {
				compile_(branch, true);
			}
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
		//DBG_P("code_num = [%d], codes->size() = [%d]", code_num, codes->size());
		GPerlVirtualMachineCode *jmp = codes->at(code_num - 2);
		//DBG_PL("jmp = [%d]", jmp->op);
		int cond_code_num = code_num;
		DBG_PL("-------------TRUE STMT--------------");
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
		DBG_PL("-------------FALSE STMT-------------");
		if (path->false_stmt) {
			GPerlCell *false_stmt = path->false_stmt->root;
			for (; false_stmt; false_stmt = false_stmt->next) {
				GPerlCell *path = false_stmt;
				compile_(path, true);
				dst = 0;//reset dst number
			}
		}
		jmp->jmp = code_num - cur_code_num;
		DBG_PL("------------------------------------");
	} else if (path->type == Function) {
		dst = 0;//reset dst number
		args_count = 0;
		vector<GPerlVirtualMachineCode *> tmp;
		vector<GPerlVirtualMachineCode *> *func_code = new vector<GPerlVirtualMachineCode *>();
		//escape current codes => tmp
		int size = codes->size();
		for (int i = 0; i < size; i++) {
			tmp.push_back(codes->at(i));
		}
		code_num -= size;
		codes->clear();
		//add func code to codes
		GPerlCell *body_stmt = path->body->root;
		for (; body_stmt; body_stmt = body_stmt->next) {
			GPerlCell *path = body_stmt;
			compile_(path, true);
			dst = 0;//reset dst number
		}
		GPerlVirtualMachineCode *ret = createRET();
		addVMCode(ret);
		GPerlVirtualMachineCode *undef = createUNDEF();//for threaded code
		addVMCode(undef);
		//copy codes to func_code
		size = codes->size();
		for (int i = 0; i < size; i++) {
			func_code->push_back(codes->at(i));
		}
		GPerlVirtualMachineCode *f = getPureCodes(func_code);
		DBG_PL("========= DUMP FUNC CODE ==========");
		dumpPureVMCode(f);
		DBG_PL("===================================");
		codes->clear();
		//revert codes from tmp
		size = tmp.size();
		for (int i = 0; i < size; i++) {
			codes->push_back(tmp.at(i));
		}
		code = codes->back();
		code->func = f;
		//DBG_PL("========= FUNCTION DECL END ==========");
	}
}

static GPerlVirtualMachineCode pure_codes_[32];
GPerlVirtualMachineCode *GPerlCompiler::getPureCodes(vector<GPerlVirtualMachineCode *> *codes)
{
	int code_n = codes->size();
	//GPerlVirtualMachineCode pure_codes_[code_n + 1];//for O2 option
	memset(pure_codes_ + code_n + 1, 0, sizeof(GPerlVirtualMachineCode));
	for (int i = 0; i < code_n; i++) {
		pure_codes_[i] = *codes->at(i);
	}
	volatile int code_size = code_n * sizeof(GPerlVirtualMachineCode);
	GPerlVirtualMachineCode *pure_codes = (GPerlVirtualMachineCode *)malloc(code_size);
	memcpy(pure_codes, pure_codes_, code_size);
	return pure_codes;
}

#define INT(O) OPi ## O
#define INTC(O) OPi ## O ## C
#define STRING(O) OPs ## O

#define SET_OPCODE(T) {							\
		dst--;									\
		code->src = dst;						\
		switch (reg_type[dst - 1]) {			\
		case Int:								\
			code->op = INT(T);					\
			break;								\
		case Object:							\
			switch (reg_type[dst]) {			\
			case Int:							\
				code->op = INTC(T);				\
				code->src = codes->back()->src;	\
				popVMCode();					\
				code_num--;						\
				code->code_num = code_num;		\
				break;							\
			default:							\
				code->op = OP ## T;				\
				break;							\
			}									\
			break;								\
		default:								\
			code->op = OP ## T;					\
			break;								\
		}										\
		code->dst = dst - 1;					\
		code->jmp = 1;							\
	}

GPerlVirtualMachineCode *GPerlCompiler::createVMCode(GPerlCell *c)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	switch (c->type) {
	case Int:
		code->dst = dst;
		code->src = c->data.idata;
		code->op = INT(MOV);
		reg_type[dst] = Int;
		dst++;
		break;
	case String:
		code->dst = dst;
		code->src = -1;
		code->name = c->data.sdata;
		code->op = STRING(MOV);
		reg_type[dst] = String;
		dst++;
		break;
	case Add:
		SET_OPCODE(ADD);
		break;
	case Sub:
		SET_OPCODE(SUB);
		break;
	case Mul:
		SET_OPCODE(MUL);
		break;
	case Div:
		SET_OPCODE(DIV);
		break;
	case Greater:
		SET_OPCODE(JG);
		break;
	case Less:
		SET_OPCODE(JL);
		break;
	case GreaterEqual:
		SET_OPCODE(JGE);
		break;
	case LessEqual:
		SET_OPCODE(JLE);
		break;
	case EqualEqual:
		SET_OPCODE(JE);
		break;
	case NotEqual:
		SET_OPCODE(JNE);
		break;
	case PrintDecl:
		code->op = OPPRINT;
		break;
	case IfStmt:
		code->op = OPNOP;
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
	case Call: {
		const char *name = cstr(c->fname);
		int idx = getFuncIndex(name);
		code->op = OPCALL;
		code->dst = dst-1;
		code->src = idx;
		code->name = name;
		break;
	}
	case Shift:
		code->op = OPSHIFT;
		code->dst = 0;
		code->src = args_count;
		args_count++;
		break;
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
	case Function: {
		code->op = OPFUNCSET;
		code->dst = func_index;
		code->src = 0;
		const char *name = cstr(c->fname);
		code->name = name;
		setToFunctionNames(name);
		declared_fname = name;
		break;
	}
	case Return:
		code->op = OPRET;
		code->dst = 0;
		code->src = 0;
		break;
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

void GPerlCompiler::setToFunctionNames(const char *name)
{
	func_names[func_index] = name;
	func_index++;
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

int GPerlCompiler::getFuncIndex(const char *name)
{
	int ret = -1;
	size_t name_size = strlen(name);
	for (int i = 0; i < func_index; i++) {
		size_t f_size = strlen(func_names[i]);
		if (f_size == name_size &&
			!strncmp(func_names[i], name, name_size)) {
			ret = i;
			break;
		}
	}
	if (ret == -1) {
		fprintf(stderr, "COMPILE ERROR: cannot find func name[%s]\n", name);
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

GPerlVirtualMachineCode *GPerlCompiler::createUNDEF(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = OPUNDEF;
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

GPerlVirtualMachineCode *GPerlCompiler::createoWRITE(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = OPoWRITE;
	code_num++;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createiPUSH(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = OPiPUSH;
	code->src = 0;
	code->dst = dst-1;
	code_num++;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createsPUSH(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = OPsPUSH;
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
	(void)code;
	DBG_PL("L[%d] : %s [dst:%d], [src:%d], [jmp:%d], [name:%s]",
		   code->code_num, OpName(code->op), code->dst, code->src,
		   code->jmp, code->name);
}

void GPerlCompiler::dumpPureVMCode(GPerlVirtualMachineCode *c)
{
	int code_n = codes->size();
	for (int i = 0; i < code_n; i++) {
		dumpVMCode(&c[i]);
	}
}
