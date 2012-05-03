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

void GPerlCompiler::addWriteCode(void)
{
	GPerlVirtualMachineCode *code;
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
}

void GPerlCompiler::addPushCode(void)
{
	GPerlVirtualMachineCode *code;
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

void GPerlCompiler::compile_(GPerlCell *path, bool isRecursive)
{
	GPerlVirtualMachineCode *code;
	GPerlCell *p = path;
	if (p->argsize > 0 && isRecursive) {
		for (int i = 0; i < p->argsize; i++) {
			compile_(p->vargs[i], false);
			if (path->type == PrintDecl) {
				//add WRITE CODE for Print Stmt
				addWriteCode();
			} else if (path->type == Call) {
				addPushCode();
			}
			dst = 0;
		}
	}
	for (; path->left; path = path->left) {}
	if (path->type == Call) {
		for (int i = 0; i < path->argsize; i++) {
			compile_(path->vargs[i], false);
			addPushCode();
			//dst = 0;
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
				for (int i = 0; i < branch->argsize; i++) {
					compile_(branch->vargs[i], false);
					addPushCode();
					//dst = 0;
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
		jmp->jmp = code_num - cond_code_num + 2/*OPNOP + OPJMP + 1*/;
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
		//GPerlVirtualMachineCode *super = createRET();
		//super->op = OPSUPERCALL;
		//addVMCode(super);
		GPerlVirtualMachineCode *ret = createRET();
		addVMCode(ret);
		GPerlVirtualMachineCode *undef = createUNDEF();//for threaded code
		addVMCode(undef);
		//copy codes to func_code
		size = codes->size();
		DBG_PL("SIZE = [%d]", size);
		for (int i = 0; i < size; i++) {
			func_code->push_back(codes->at(i));
		}
		optimizeFuncCode(func_code, path->fname);
		size = codes->size();
		DBG_PL("SIZE = [%d]", code_num);
		finalCompile(func_code);
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

void GPerlCompiler::optimizeFuncCode(vector<GPerlVirtualMachineCode *> *f, string fname)
{
	vector<GPerlVirtualMachineCode *>::iterator it = f->begin();
	int reg_n = 0;
	bool isOMOVCall = false;
	while (it != f->end()) {
		GPerlVirtualMachineCode *c = *it;
		if (c->op == OPJCALL && fname == c->name) {
			c->op = OPJSELFCALL;
			//c->op = OPSELFCALL;
		} else if (c->op == OPNOP) {
			it = f->erase(it);
			code_num--;
			it--;
		} else if (c->op == OPSET) {
			it = f->erase(it);
			code_num--;
			it--;
		} else if (c->op == OPSHIFT) {
			it = f->erase(it);
			code_num--;
			it--;
		} else if (c->op == OPLET) {
			it = f->erase(it);
			code_num--;
			it--;
		} else if (c->op == OPOMOV) {
			reg_n = c->dst;
			if (isOMOVCall) {
				//TODO
				it = f->erase(it);
				code_num--;
				it--;
				isOMOVCall = false;
			} else {
				//c->op = OPSUPERCALL;
				isOMOVCall = true;
			}
		}
		it++;
	}
}

#define CONCAT(c0, c1, c2) OP##c0##c1##c2
#define CONCAT2(c0, c1) OP##c0##c1

#define DST(n) c->dst == n
#define SRC(n) c->src == n

#define OPCREATE_TYPE1(O) {						\
		if (DST(0) && SRC(1)) {					\
			c->op = CONCAT(A, B, O);			\
		} else if (DST(0) && SRC(2)) {			\
			c->op = CONCAT(A, C, O);			\
		} else if (DST(0) && SRC(3)) {			\
			c->op = CONCAT(A, D, O);			\
		} else if (DST(1) && SRC(2)) {			\
			c->op = CONCAT(B, C, O);			\
		} else if (DST(1) && SRC(3)) {			\
			c->op = CONCAT(B, D, O);			\
		} else if (DST(2) && SRC(3)) {			\
			c->op = CONCAT(C, D, O);			\
		}										\
	}

#define OPCREATE_TYPE2(O) {						\
		if (DST(0)) {							\
			c->op = CONCAT2(A, O);				\
		} else if (DST(1)) {					\
			c->op = CONCAT2(B, O);				\
		} else if (DST(2)) {					\
			c->op = CONCAT2(C, O);				\
		} else if (DST(3)) {					\
			c->op = CONCAT2(D, O);				\
		}										\
	}

#define OPCREATE_TYPE3(O) {						\
		if (SRC(0)) {							\
			c->op = CONCAT2(A, O);				\
		} else if (SRC(1)) {					\
			c->op = CONCAT2(B, O);				\
		} else if (SRC(2)) {					\
			c->op = CONCAT2(C, O);				\
		} else if (SRC(3)) {					\
			c->op = CONCAT2(D, O);				\
		}										\
	}

void GPerlCompiler::finalCompile(vector<GPerlVirtualMachineCode *> *code)
{
	DBG_P("********* finalCompile ***********");
	vector<GPerlVirtualMachineCode *>::iterator it = code->begin();
	while (it != code->end()) {
		GPerlVirtualMachineCode *c = *it;
		switch (c->op) {
		/*========= TYPE1 =========*/
		case OPADD:
			OPCREATE_TYPE1(ADD);
			break;
			/*
		case OPiADD:
			OPCREATE_TYPE1(iADD);
			break;
		case OPSUB:
			OPCREATE_TYPE1(SUB);
			break;
		case OPiSUB:
			OPCREATE_TYPE1(iSUB);
			break;
		*/
		/*========= TYPE2 =========*/
/*
		case OPiADDC:
			OPCREATE_TYPE2(iADDC);
			break;
*/
		case OPiSUBC:
			OPCREATE_TYPE2(iSUBC);
			break;
		case OPiJLC:
			OPCREATE_TYPE2(iJLC);
			break;
/*
		case OPiJGC:
			OPCREATE_TYPE2(iJGC);
			break;
		case OPPUSH:
			OPCREATE_TYPE2(PUSH);
			break;
*/
		case OPiPUSH:
			OPCREATE_TYPE2(iPUSH);
			break;
/*
		case OPMOV:
			OPCREATE_TYPE2(MOV);
			break;
*/
		case OPiMOV:
			OPCREATE_TYPE2(iMOV);
			break;
		case OPOMOV:
			OPCREATE_TYPE2(OMOV);
			break;
		case OPSELFCALL:
			OPCREATE_TYPE2(SELFCALL);
			break;
		case OPJSELFCALL:
			OPCREATE_TYPE2(JSELFCALL);
			break;
		/*========= TYPE3 =========*/
		case OPRET:
			OPCREATE_TYPE3(RET);
			break;
		case OPJRET:
			OPCREATE_TYPE3(JRET);
			break;
		default:
			break;
		}
		it++;
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
		code->op = OPJCALL;
		//code->op = OPCALL;
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
		code->op = OPJRET;
		//code->op = OPRET;
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
	code->op = OPJRET;
	//code->op = OPRET;
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
	int code_n = (codes->size() > (size_t)code_num) ? code_num : codes->size();
	for (int i = 0; i < code_n; i++) {
		dumpVMCode(&c[i]);
	}
}
