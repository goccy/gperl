#include <gperl.hpp>
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
		compile_(path);
		DBG_PL("============================");
		dst = 0;//reset dst number
	}
	GPerlVirtualMachineCode *ret = createRET();
	addVMCode(ret);
	GPerlVirtualMachineCode *undef = createUNDEF();//for threaded code
	addVMCode(undef);
	optimizeFuncCode(codes, "");
	return getPureCodes(codes);
}

#define GOTO_LEFT_LEAFNODE(path) for (; path->left; path = path->left) {}
#define isRIGHT_LEAF_NODE(branch) branch->left == NULL && branch->right == NULL && branch->argsize == 0 && branch->type != IfStmt
#define isNotFALSE_STMT(branch) branch->type != IfStmt

void GPerlCompiler::compile_(GPerlCell *path)
{
	GOTO_LEFT_LEAFNODE(path);
	genVMCode(path);
	while (path->parent) {
		GPerlCell *parent = path->parent;
		GPerlCell *branch = parent->right;
		if (branch) {
			if (branch == path) return;
			if (isRIGHT_LEAF_NODE(branch)) {
				genVMCode(branch);
			} else if (isNotFALSE_STMT(parent)) {
				compile_(branch);
			}
		}
		genVMCode(parent);
		path = parent;
	}
}

void GPerlCompiler::genVMCode(GPerlCell *path) {
	GPerlVirtualMachineCode *code;
	if (path->type == Call || path->type == BuiltinFunc) {
		genFunctionCallCode(path);
	} else if (path->type == IfStmt) {
		genIfStmtCode(path);
	} else if (path->type == Function) {
		genFunctionCode(path);
		return;
	}
	code = createVMCode(path);
	addVMCode(code);
	dumpVMCode(code);
}

void GPerlCompiler::genFunctionCallCode(GPerlCell *p)
{
	size_t argsize = p->argsize;
	int dststack[argsize];
	for (size_t i = 0; i < argsize; i++) {
		compile_(p->vargs[i]);
		dststack[i] = dst;
		if (p->rawstr == "print") {
			addWriteCode();
		}
	}
	for (size_t i = 0; i < argsize; i++) {
		if (p->type == Call || p->type == BuiltinFunc) {
			addPushCode(i, dststack[i]);
		}
	}
}


#define ESCAPE_CURRENT_CODE(codes, tmp) {		\
		int size = codes->size();				\
		for (int i = 0; i < size; i++) {		\
			tmp.push_back(codes->at(i));		\
		}										\
	}

#define CLEAR_CURRENT_CODE(codes) {				\
		code_num = 0;							\
		codes->clear();							\
	}

#define ADD_FUNCCODE_TO_CODES(codes) {									\
		GPerlCell *body_stmt = path->body->root;						\
		for (; body_stmt; body_stmt = body_stmt->next) {				\
			GPerlCell *path = body_stmt;								\
			compile_(path);												\
			dst = 0;													\
		}																\
		GPerlVirtualMachineCode *ret = createRET();						\
		addVMCode(ret);													\
		GPerlVirtualMachineCode *undef = createUNDEF();/*for threaded code*/ \
		addVMCode(undef);												\
	}

#define COPY_CURRENT_CODE(codes, func_code) {	\
		size_t size = codes->size();			\
		for (size_t i = 0; i < size; i++) {		\
			func_code->push_back(codes->at(i));	\
		}										\
	}

#define REVERT_ESCAPED_CODE(tmp, codes) {		\
		size_t size = tmp.size();				\
		for (size_t i = 0; i < size; i++) {		\
			codes->push_back(tmp.at(i));		\
		}										\
	}

void GPerlCompiler::genFunctionCode(GPerlCell *path)
{
	local_vmap.clear();
	GPerlVirtualMachineCode *code;
	vector<GPerlVirtualMachineCode *> tmp;
	vector<GPerlVirtualMachineCode *> *func_code = NULL;
	dst = 0;//reset dst number
	args_count = 0;
	func_code = new vector<GPerlVirtualMachineCode *>();
	code = createVMCode(path);//OPFUNCSET
	addVMCode(code);
	dumpVMCode(code);
	ESCAPE_CURRENT_CODE(codes, tmp);//codes => tmp
	CLEAR_CURRENT_CODE(codes);
	ADD_FUNCCODE_TO_CODES(codes);
	COPY_CURRENT_CODE(codes, func_code);
	optimizeFuncCode(func_code, path->fname);
	//finalCompile(func_code);
	GPerlVirtualMachineCode *f = getPureCodes(func_code);
	DBG_PL("========= DUMP FUNC CODE ==========");
	dumpPureVMCode(f);
	DBG_PL("===================================");
	CLEAR_CURRENT_CODE(codes);
	REVERT_ESCAPED_CODE(tmp, codes);//tmp => codes
	code = codes->back();//OPFUNCSET
	code->func = f;
	DBG_PL("========= FUNCTION DECL END ==========");
}

void GPerlCompiler::genIfStmtCode(GPerlCell *path)
{
	dst = 0;//reset dst number
	GPerlCell *true_stmt = path->true_stmt->root;
	//DBG_P("code_num = [%d], codes->size() = [%d]", code_num, codes->size());
	GPerlVirtualMachineCode *jmp = codes->at(code_num - 1);
	//DBG_PL("jmp = [%d]", jmp->op);
	int cond_code_num = code_num;
	//DBG_PL("-------------TRUE STMT--------------");
	for (; true_stmt; true_stmt = true_stmt->next) {
		GPerlCell *path = true_stmt;
		compile_(path);
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
			compile_(path);
			dst = 0;//reset dst number
		}
	}
	jmp->jmp = code_num - cur_code_num;
	DBG_PL("------------------------------------");
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
	switch (reg_type[dst-1]) {
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
		if (reg_type[dst] == Int) {
			code = createiWRITE();
		} else {
			code = createoWRITE();
		}
		addVMCode(code);
		dumpVMCode(code);
		break;
	default:
		code = createWRITE();
		addVMCode(code);
		dumpVMCode(code);
		break;
	}
}

void GPerlCompiler::addPushCode(int i, int dst_)
{
	GPerlVirtualMachineCode *code = createPUSH(i, dst_);
	addVMCode(code);
	dumpVMCode(code);
}

void GPerlCompiler::optimizeFuncCode(vector<GPerlVirtualMachineCode *> *f, string fname)
{
	vector<GPerlVirtualMachineCode *>::iterator it = f->begin();
	while (it != f->end()) {
		GPerlVirtualMachineCode *c = *it;
		if (c->op == CALL && fname == c->name) {
			c->op = SELFCALL;
		} else if (c->op == NOP || c->op == SETv || c->op == SHIFT) {
			it = f->erase(it);
			code_num--;
			it--;
		}
		it++;
	}
}

#define CONCAT(c0, c1, c2) c0##c1##_##c2
#define CONCAT2(c0, c1) c0##_##c1

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
	DBG_PL("********* finalCompile ***********");
	vector<GPerlVirtualMachineCode *>::iterator it = code->begin();
	while (it != code->end()) {
		GPerlVirtualMachineCode *c = *it;
		switch (c->op) {
		/*========= TYPE1 =========*/
		case ADD:
			OPCREATE_TYPE1(ADD);
			break;
		case iADD:
			OPCREATE_TYPE1(iADD);
			break;
		case SUB:
			OPCREATE_TYPE1(SUB);
			break;
		case iSUB:
			OPCREATE_TYPE1(iSUB);
			break;
		/*========= TYPE2 =========*/
/*
		case OPiADDC:
			OPCREATE_TYPE2(iADDC);
			break;
*/
		case iSUBC:
			OPCREATE_TYPE2(iSUBC);
			break;
		case JL:
			OPCREATE_TYPE2(JL);
			break;
		case iJLC:
			OPCREATE_TYPE2(iJLC);
			break;
/*
		case OPiJGC:
			OPCREATE_TYPE2(iJGC);
			break;
*/
		case PUSH:
			OPCREATE_TYPE2(PUSH);
			break;
		case MOV:
			OPCREATE_TYPE2(MOV);
			break;
		case ARGMOV:
			OPCREATE_TYPE2(ARGMOV);
			break;
		case SELFCALL:
			OPCREATE_TYPE2(SELFCALL);
			break;
		/*========= TYPE3 =========*/
		case RET:
			OPCREATE_TYPE3(RET);
			break;
		default:
			break;
		}
		it++;
	}
}

static GPerlVirtualMachineCode pure_codes_[128];
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

#define INT(O) i ## O
#define INTC(O) i ## O ## C
#define STRING(O) s ## O

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
				code->op = T;					\
				break;							\
			}									\
			break;								\
		default:								\
			code->op = T;						\
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
		INT_init(code->v, c->data.idata);
		code->op = MOV;
		reg_type[dst] = Int;
		dst++;
		break;
	case String:
		code->dst = dst;
		code->src = -1;
		STRING_init(code->v, c->data.sdata);
		code->op = MOV;
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
	case IfStmt:
		code->op = NOP;
		break;
	case LocalVarDecl: case VarDecl: {
		code->op = SETv;
		code->dst = c->vidx;//variable_index;
		code->src = 0;
		const char *name = cstr(c->vname);
		code->name = name;
		if (c->vname.find("*") != string::npos) {
			local_vmap[c->vname] = c->vidx;
		} else {
			global_vmap[c->vname] = c->vidx;
		}
		declared_vname = name;
		break;
	}
	case GlobalVarDecl: {
		code->op = SETv;
		code->dst = c->vidx;//variable_index;
		code->src = 0;
		code->name = cstr(c->vname);
		global_vmap[c->vname] = c->vidx;
		declared_vname = cstr(c->vname);
		break;
	}
	case LocalVar: case Var: {
		int idx;
		for (int i = c->indent; i >= 0; i--) {
			string prefix = "";
			for (int j = 0; j < i; j++) {
				prefix += string(NAME_RESOLUTION_PREFIX);
			}
			if (local_vmap.find(prefix + c->vname) != local_vmap.end()) {
				idx = local_vmap[prefix + c->vname];//getVariableIndex(name);
				code->op = vMOV;
				c->vname = prefix + c->vname;
				code->name = cstr(c->vname);
				goto BREAK;
			} else if (global_vmap.find(prefix + c->vname) != global_vmap.end()) {
				idx = global_vmap[prefix + c->vname];//getVariableIndex(name);
				code->op = gMOV;
				c->vname = prefix + c->vname;
				code->name = cstr(c->vname);
				goto BREAK;
			}
		}
		fprintf(stderr, "ERROR: cannot find variable name : [%s]", cstr(c->vname));
		BREAK:;
		code->dst = dst;
		code->src = idx;
		reg_type[dst] = Object;
		dst++;
		break;
	}
	case ArrayVar: {
		DBG_PL("ArrayVar");
		const char *name = cstr(c->vname);
		int idx = getVariableIndex(name);
		code->op = MOV;
		reg_type[dst] = Array;
		code->dst = dst;
		code->src = idx;
		code->name = name;
		dst++;
		break;
	}
	case BuiltinFunc: {
		if (c->rawstr == "print") {
			DBG_PL("Print");
			code->op = FLUSH;
		} else if (c->rawstr == "push") {
			DBG_PL("Push");
			if (reg_type[0] != Array) {
				fprintf(stderr, "COMPILE ERROR : must be Array 1st argument\n");
			}
			code->op = ARRAY_PUSH;//ARRAY_PUSH;
			code->push = Array_push;
		}
		break;
	}
	case Call: {
		const char *name = cstr(c->fname);
		int idx = getFuncIndex(name);
		code->op = CALL;
		code->dst = dst-1;
		code->src = idx;
		code->name = name;
		break;
	}
	case List: {
		DBG_PL("List");
		size_t size = sizeof(GPerlObject) * (c->argsize + 1);
		GPerlObject **list = (GPerlObject **)malloc(size);
		memset(list, 0, size);
		for (int i = 0; i < c->argsize; i++) {
			GPerlCell *v = c->vargs[i];
			GPerlObject *o = new GPerlObject();//TODO : fetch from MMU
			switch (v->type) {
			case Int:
				o->value.ivalue = v->data.idata;
				o->type = Int;
				break;
			case String:
				o->value.svalue = v->data.sdata;
				o->type = String;
				break;
			case Object:
				o->value.ovalue = v->data.pdata;
				o->type = Object;
				break;
			default:
				break;
			}
			list[i] = o;
		}
		code->dst = dst;
		code->src = 0;
		OBJECT_init(code->v, list);
		code->op = MOV;
		reg_type[dst] = Object;
		dst++;
		break;
	}
	case Shift:
		//code->op = SHIFT;
		code->op = ARGMOV;
		code->dst = 0;
		code->src = args_count;
		args_count++;
		reg_type[dst] = Object;
		dst++;
		break;
	case Assign: {
		int idx;
		for (int i = c->indent; i >= 0; i--) {
			string prefix = "";
			for (int j = 0; j < i; j++) {
				prefix += string(NAME_RESOLUTION_PREFIX);
			}
			if (local_vmap.find(prefix + c->vname) != local_vmap.end()) {
				idx = local_vmap[prefix + c->vname];//getVariableIndex(name);
				c->vname = prefix + c->vname;
				code->name = cstr(c->vname);
				code->op = LET;
				goto ASSIGN_BREAK;
			} else if (global_vmap.find(prefix + c->vname) != global_vmap.end()) {
				idx = global_vmap[prefix + c->vname];//getVariableIndex(name);
				c->vname = prefix + c->vname;
				code->name = cstr(c->vname);
				code->op = gLET;
				goto ASSIGN_BREAK;
			}
		}
		fprintf(stderr, "ERROR: cannot find variable name : [%s]", cstr(c->vname));
		ASSIGN_BREAK:;
		code->dst = idx;
		code->src = dst-1;
		code->name = cstr(c->vname);
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
		case Object:
			variable_types[idx] = Object;
			break;
		default:
			break;
		}
		/* ================================== */
		declared_vname = NULL;
		break;
	}
	case Function: {
		code->op = FUNCSET;
		code->dst = func_index;
		code->src = 0;
		const char *name = cstr(c->fname);
		code->name = name;
		setToFunctionNames(name);
		declared_fname = name;
		break;
	}
	case Return:
		code->op = RET;
		code->dst = 0;
		code->src = dst-1;
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
	code->op = THCODE;
	code_num++;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createRET(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = RET;
	code_num++;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createUNDEF(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = UNDEF;
	code_num++;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createWRITE(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = WRITE;
	code->dst = dst-1;
	code_num++;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createiWRITE(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = iWRITE;
	code->dst = dst-1;
	code_num++;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createsWRITE(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = sWRITE;
	code->dst = dst-1;
	code_num++;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createoWRITE(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = oWRITE;
	code_num++;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createPUSH(int i, int dst_)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = PUSH;
	code->src = i;
	code->dst = dst_-1;
	code_num++;
	args_count++;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createJMP(int jmp_num)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = JMP;
	code->jmp = jmp_num;
	code_num++;
	return code;
}

void GPerlCompiler::dumpVMCode(GPerlVirtualMachineCode *code)
{
	(void)code;
	DBG_PL("L[%d] : %s [dst:%d], [src:%d], [jmp:%d], [name:%s]",
		   code->code_num, decl_codes[code->op].name, code->dst, code->src,
		   code->jmp, code->name);
}

void GPerlCompiler::dumpPureVMCode(GPerlVirtualMachineCode *c)
{
	int code_n = (codes->size() > (size_t)code_num) ? code_num : codes->size();
	for (int i = 0; i < code_n; i++) {
		dumpVMCode(&c[i]);
	}
}
