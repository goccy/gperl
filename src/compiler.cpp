#include <gperl.hpp>
using namespace std;
GPerlCompiler::GPerlCompiler(void) : dst(0), src(0), code_num(0),
									 func_index(0),
									 args_count(0), recv_args_count(0)
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
#define isRIGHT_LEAF_NODE(branch) branch->left == NULL && branch->right == NULL && branch->argsize == 0 && branch->type != IfStmt && branch->type != ElsifStmt
#define isNotFALSE_STMT(branch) branch->type != IfStmt && branch->type != ElsifStmt

void GPerlCompiler::compile_(GPerlCell *path)
{
	if (path->type == WhileStmt) {
		loop_start_num = code_num;
	}
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
	} else if (path->type == IfStmt || path->type == ElsifStmt) {
		genIfStmtCode(path);
	} else if (path->type == WhileStmt) {
		genWhileStmtCode(path);
	} else if (path->type == ForStmt) {
		genForStmtCode(path);
	} else if (path->type == Function) {
		genFunctionCode(path);
		return;
	}
	code = createVMCode(path);
	if (code->op != SETv && code->op != NOP && code->op != SHIFT) {
		addVMCode(code);
	}
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
			code_num++;							\
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
	recv_args_count = 0;
	func_code = new vector<GPerlVirtualMachineCode *>();
	code = createVMCode(path);//OPFUNCSET
	addVMCode(code);
	dumpVMCode(code);
	ESCAPE_CURRENT_CODE(codes, tmp);//codes => tmp
	CLEAR_CURRENT_CODE(codes);
	ADD_FUNCCODE_TO_CODES(codes);
	optimizeFuncCode(codes, path->fname);
	COPY_CURRENT_CODE(codes, func_code);//codes => func_code
#ifndef ENABLE_JIT_COMPILE
	finalCompile(func_code);
#endif
	GPerlVirtualMachineCode *f = getPureCodes(func_code);
	DBG_PL("========= DUMP FUNC CODE ==========");
	dumpPureVMCode(f);
	DBG_PL("===================================");
	CLEAR_CURRENT_CODE(codes);
	REVERT_ESCAPED_CODE(tmp, codes);//tmp => codes
	code = codes->back();//OPFUNCSET
	code->func = f;
	DBG_PL("code_num = [%d]", code_num);
	DBG_PL("========= FUNCTION DECL END ==========");
	args_count = 0;
	recv_args_count = 0;
}

void GPerlCompiler::genIfStmtCode(GPerlCell *path)
{
	dst = 0;//reset dst number
	GPerlCell *true_stmt = path->true_stmt->root;
	GPerlVirtualMachineCode *jmp = codes->at(code_num - 1);
	int cond_code_num = code_num;
	DBG_PL("-------------TRUE STMT--------------");
	for (; true_stmt; true_stmt = true_stmt->next) {
		GPerlCell *path = true_stmt;
		compile_(path);
		dst = 0;//reset dst number
	}
	jmp->jmp = code_num - cond_code_num + 2/*OPNOP + OPJMP + 1*/;
	int cur_code_num = code_num;
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

void GPerlCompiler::genWhileStmtCode(GPerlCell *path)
{
	dst = 0;//reset dst number
	DBG_PL("-------------genWhileStmtCode------------------");
	int loop_start = loop_start_num;
	GPerlCell *true_stmt = path->true_stmt->root;
	GPerlVirtualMachineCode *jmp = codes->at(code_num - 1);
	DBG_P("JMP_CODE : ");
	dumpVMCode(jmp);
	int cond_code_num = code_num;
	DBG_PL("-------------TRUE STMT--------------");
	for (; true_stmt; true_stmt = true_stmt->next) {
		GPerlCell *path = true_stmt;
		compile_(path);
		dst = 0;//reset dst number
	}
	jmp->jmp = code_num - cond_code_num + 2/*OPNOP + OPJMP + 1*/;
	int cur_code_num = code_num;// - 1;
	jmp = createJMP(1);
	addVMCode(jmp);
	dumpVMCode(jmp);
	jmp->jmp = loop_start - cur_code_num;
}

void GPerlCompiler::genForStmtCode(GPerlCell *path)
{
	dst = 0;//reset dst number
	DBG_PL("-------------genForStmtCode------------------");
	GPerlCell *cond = (path->left->next) ? path->left->next : NULL;
	if (!cond) fprintf(stderr, "SYNTAX ERROR : for stmt\n");
	int loop_start = code_num;
	compile_(cond);
	dst = 0;
	GPerlCell *true_stmt = path->true_stmt->root;
	GPerlVirtualMachineCode *jmp = codes->at(code_num - 1);
	int cond_code_num = code_num;
	DBG_PL("-------------TRUE STMT--------------");
	for (; true_stmt; true_stmt = true_stmt->next) {
		GPerlCell *path = true_stmt;
		compile_(path);
		dst = 0;//reset dst number
	}
	GPerlCell *step = (cond->next) ? cond->next : NULL;
	if (!step) fprintf(stderr, "SYNTAX ERROR : for stmt\n");
	compile_(step);
	jmp->jmp = code_num - cond_code_num + 2;// + 1/*OPNOP + OPJMP + 1*/;
	int cur_code_num = code_num;
	jmp = createJMP(1);
	addVMCode(jmp);
	dumpVMCode(jmp);
	jmp->jmp = loop_start - cur_code_num;
}

void GPerlCompiler::addVMCode(GPerlVirtualMachineCode *code)
{
	codes->push_back(code);
	code_num++;
}

void GPerlCompiler::popVMCode()
{
	codes->pop_back();
	code_num--;
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
		} else if (c->op == FASTCALL0 && fname == c->name) {
			c->op = SELF_FASTCALL0;
		} else if (c->op == FASTCALL1 && fname == c->name) {
			c->op = SELF_FASTCALL1;
		} else if (c->op == FASTCALL2 && fname == c->name) {
			c->op = SELF_FASTCALL2;
		} else if (c->op == FASTCALL3 && fname == c->name) {
			c->op = SELF_FASTCALL3;
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
		case iADDC:
			OPCREATE_TYPE2(iADDC);
			break;
		case iSUBC:
			OPCREATE_TYPE2(iSUBC);
			break;
		case JL:
			OPCREATE_TYPE2(JL);
			break;
		case JG:
			OPCREATE_TYPE2(JG);
			break;
		case iJLC:
			OPCREATE_TYPE2(iJLC);
			break;
		case iJGC:
			OPCREATE_TYPE2(iJGC);
			break;
		case PUSH:
			OPCREATE_TYPE2(PUSH);
			break;
		case LET:
			OPCREATE_TYPE2(LET);
			break;
		case MOV:
			OPCREATE_TYPE2(MOV);
			break;
		case vMOV:
			OPCREATE_TYPE2(vMOV);
			break;
		case ARGMOV:
			OPCREATE_TYPE2(ARGMOV);
			break;
		case SELFCALL:
			OPCREATE_TYPE2(SELFCALL);
			break;
		case SELF_FASTCALL0:
			OPCREATE_TYPE2(SELF_FASTCALL0);
			break;
		case SELF_FASTCALL1:
			OPCREATE_TYPE2(SELF_FASTCALL1);
			break;
		case SELF_FASTCALL2:
			OPCREATE_TYPE2(SELF_FASTCALL2);
			break;
		case SELF_FASTCALL3:
			OPCREATE_TYPE2(SELF_FASTCALL3);
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
	GPerlVirtualMachineCode *pure_codes = (GPerlVirtualMachineCode *)safe_malloc(code_size);
	memcpy(pure_codes, pure_codes_, code_size);
	return pure_codes;
}

#define INT(O) i ## O
#define INTC(O) i ## O ## C
#define STRING(O) s ## O
#define DOUBLE(O) d ## O
#define DOUBLEC(O) d ## O ## C

#define SET_OPCODE(T) {							\
		dst--;									\
		code->src = dst;						\
		switch (reg_type[dst - 1]) {			\
		case Int:								\
			code->op = INT(T);					\
			break;								\
		case Double:							\
			code->op = DOUBLE(T);				\
			break;								\
		case Object:							\
			switch (reg_type[dst]) {			\
			case Int:							\
				code->op = INTC(T);				\
				code->src = codes->back()->src;	\
				code->v = codes->back()->v;		\
				popVMCode();					\
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
	case Int: case Double:
	case String: case List:
	case ArrayRef:
		setMOV(code, c);
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
	case IsNot:
		setISNOT(code, c);
		break;
	case Is:
		setIS(code, c);
		break;
	case StringAdd:
		SET_OPCODE(StringADD);
		break;
	case Inc:
		setINC(code, c);
		break;
	case Dec:
		setDEC(code, c);
		break;
	case ArrayDereference:
		setArrayDereference(code, c);
		break;
	case IfStmt: case WhileStmt: case ForStmt: case ElsifStmt:
		code->op = NOP;
		break;
	case GlobalVarDecl: case LocalVarDecl: case VarDecl:
		setSETv(code, c);
		break;
	case LocalVar: case Var: case ArrayVar:
		setVMOV(code, c);
		break;
	case ArgumentArray:
		setArrayARGMOV(code, c);
		break;
	case BuiltinFunc:
		setBFUNC(code, c);
		break;
	case Call:
		setCALL(code, c);
		break;
	case Shift:
		setARGMOV(code, c);
		break;
	case Assign:
		setLET(code, c);
		break;
	case ArrayAt:
		setArrayAt(code, c);
		break;
	case Function:
		setFUNC(code, c);
		break;
	case Return:
		code->op = RET;
		code->dst = 0;
		code->src = dst-1;
		break;
	default:
		break;
	}
	setEscapeStackNum(code, c);
	return code;
}

void GPerlCompiler::setToFunctionNames(const char *name)
{
	func_names[func_index] = name;
	func_index++;
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

void GPerlCompiler::setMOV(GPerlVirtualMachineCode *code, GPerlCell *c)
{
	GPerlT type = c->type;
	switch (type) {
	case Int:
		INT_init(code->v, c->data.idata);
		code->op = MOV;
		break;
	case Double:
		DOUBLE_init(code->v, c->data.ddata);
		code->op = MOV;
		break;
	case String:
		code->src = -1;
		code->op = NEW_STRING;
		code->_new = new_GPerlString;
		STRING_init(code->v, new_GPerlInitString(c->data.sdata, strlen(c->data.sdata) + 1));
		init_values[init_value_idx] = code->v;
		init_value_idx++;
		break;
	case List: case ArrayRef: {
		code->op = NEW;
		code->_new = new_GPerlArray;
		size_t size = sizeof(GPerlValue) * (c->argsize + 1);
		GPerlValue *list = (GPerlValue *)safe_malloc(size);
		memset(list, 0, size);
		for (int i = 0; i < c->argsize; i++) {
			GPerlCell *v = c->vargs[i];
			switch (v->type) {
			case Int:
				INT_init(list[i], v->data.idata);
				break;
			case Double:
				DOUBLE_init(list[i], v->data.ddata);
				break;
			case String:
				STRING_init(list[i], new_GPerlInitString(v->data.sdata, strlen(v->data.sdata) + 1));
				break;
			case Object:
				OBJECT_init(list[i], v->data.pdata);
				break;
			default:
				break;
			}
		}
		OBJECT_init(code->v, new_GPerlInitArray(list, c->argsize));
		if (type == ArrayRef) {
			GPerlArray *a = (GPerlArray *)getObject(code->v);
			a->h.type = ArrayRef;
		} else {
			GPerlArray *a = (GPerlArray *)getObject(code->v);
			a->h.type = Array;
		}
		init_values[init_value_idx] = code->v;
		init_value_idx++;
		code->src = 0;
		type = Object;
		break;
	}
	default:
		break;
	}
	code->dst = dst;
	reg_type[dst] = type;
	dst++;
}

void GPerlCompiler::setArrayARGMOV(GPerlVirtualMachineCode *code, GPerlCell *)
{
	DBG_PL("ArrayARGMOV");
	code->op = ArrayARGMOV;
	code->dst = 0;
	reg_type[dst] = Array;
	dst++;
}

void GPerlCompiler::setARGMOV(GPerlVirtualMachineCode *code, GPerlCell *c)
{
	(void)c;
	code->op = ARGMOV;
	code->dst = dst;
	code->src = recv_args_count;
	recv_args_count++;
	reg_type[dst] = Object;
	dst++;
}

void GPerlCompiler::setInstByVMap(GPerlVirtualMachineCode *code, GPerlCell *c,
								  GPerlOpCode lop, GPerlOpCode gop, int *idx)
{
	for (int i = c->indent; i >= 0; i--) {
		string prefix = "";
		for (int j = 0; j < i; j++) {
			prefix += string(NAME_RESOLUTION_PREFIX);
		}
		if (local_vmap.find(prefix + c->vname) != local_vmap.end()) {
			*idx = local_vmap[prefix + c->vname];
			code->op = lop;
			c->vname = prefix + c->vname;
			code->name = cstr(c->vname);
			goto BREAK;
		} else if (global_vmap.find(prefix + c->vname) != global_vmap.end()) {
			*idx = global_vmap[prefix + c->vname];
			code->op = gop;
			c->vname = prefix + c->vname;
			code->name = cstr(c->vname);
			goto BREAK;
		}
	}
	fprintf(stderr, "ERROR: cannot find variable name : [%s]", cstr(c->vname));
	code->op = MOV;
BREAK:;
}

void GPerlCompiler::setArrayDereference(GPerlVirtualMachineCode *code, GPerlCell *c_)
{
	(void)c_;
	code->op = ARRAY_DREF;
	code->src = dst-1;
	code->dst = dst;
	dst++;
}

void GPerlCompiler::setArrayAt(GPerlVirtualMachineCode *code, GPerlCell *c_)
{
	GPerlCell *c = c_->left;
	c->vname.replace(c->indent, 1, "@");
	c->fname.replace(c->indent, 1, "@");
	c->rawstr.replace(c->indent, 1, "@");
	int idx = 0;
	if (reg_type[dst - 1] == Int) {
		idx = codes->back()->v.ivalue;
		dst--;
		popVMCode();//remove MOV
		if (c->vname.find("$_") != string::npos || c->vname.find("@_") != string::npos) {
			//ARGMOV
			code->op = ARGMOV;
			code->dst = dst;
			code->src = idx;
			reg_type[dst] = Object;
			dst++;
			return;
		}
		code->idx = idx;
		setInstByVMap(code, c, ARRAY_ATC, ARRAY_gATC, &idx);
	} else {
		setInstByVMap(code, c, ARRAY_AT, ARRAY_gAT, &idx);
		code->idx = dst-1;
	}
	c->vname.replace(c->indent, 1, "$");
	c->fname.replace(c->indent, 1, "$");
	c->rawstr.replace(c->indent, 1, "$");
	code->src = idx;
	code->dst = dst;
	reg_type[dst] = Object;
	dst++;
}

void GPerlCompiler::setIS(GPerlVirtualMachineCode *code, GPerlCell *)
{
	code->op = IS;
	code->dst = dst - 1;
	code->jmp = 1;
}

void GPerlCompiler::setISNOT(GPerlVirtualMachineCode *code, GPerlCell *)
{
	code->op = ISNOT;
	code->dst = dst - 1;
	code->jmp = 1;
}

void GPerlCompiler::setINC(GPerlVirtualMachineCode *code, GPerlCell *c_)
{
	GPerlCell *c = c_->left;
	int idx = 0;
	setInstByVMap(code, c, INC, gINC, &idx);
	code->dst = idx;
}

void GPerlCompiler::setDEC(GPerlVirtualMachineCode *code, GPerlCell *c_)
{
	GPerlCell *c = c_->left;
	int idx = 0;
	setInstByVMap(code, c, DEC, gDEC, &idx);
	code->dst = idx;
}

void GPerlCompiler::setVMOV(GPerlVirtualMachineCode *code, GPerlCell *c)
{
	GPerlCell *parent = c->parent;
	if (parent && parent->left == c &&
		(parent->type == Assign || parent->type == Inc ||
		 parent->type == ArrayAt)) {
		code->op = NOP;
		//code_num--;
		return;
	}
	int idx = 0;
	setInstByVMap(code, c, vMOV, gMOV, &idx);
	code->src = idx;
	code->dst = dst;
	if (c->type == ArrayVar) {
		reg_type[dst] = Array;
	} else {
		reg_type[dst] = Object;
	}
	dst++;
}

void GPerlCompiler::setLET(GPerlVirtualMachineCode *code, GPerlCell *c)
{
	int idx = 0;
	setInstByVMap(code, c, LET, gLET, &idx);
	code->dst = idx;
	code->src = dst-1;
	variable_types[idx] = reg_type[0];
	declared_vname = NULL;
}

void GPerlCompiler::setSETv(GPerlVirtualMachineCode *code, GPerlCell *c)
{
	const char *name = cstr(c->vname);
	switch (c->type) {
	case GlobalVarDecl:
		global_vmap[c->vname] = c->vidx;
		break;
	case LocalVarDecl: case VarDecl:
		if (c->vname.find("*") != string::npos) {
			local_vmap[c->vname] = c->vidx;
		} else {
			global_vmap[c->vname] = c->vidx;
		}
		break;
	default:
		break;
	}
	code->op = SETv;
	code->dst = c->vidx;//variable_index;
	code->src = 0;
	code->name = name;
	declared_vname = name;
	cur_stack_top = c->vidx + 1;
	if (!c->parent) {
		//not initialization
		GPerlVirtualMachineCode *mov = new GPerlVirtualMachineCode();
		mov->op = MOV;
		mov->code_num = code_num;
		mov->dst = dst;
		dst++;
		GPerlUndef *undef = new_GPerlUndef();
		OBJECT_init(mov->v, undef);
		addVMCode(mov);
		dumpVMCode(mov);
		GPerlVirtualMachineCode *let = new GPerlVirtualMachineCode();
		let->code_num = code_num;
		let->dst = dst;
		int idx = 0;
		setInstByVMap(let, c, LET, gLET, &idx);
		let->dst = idx;
		let->src = dst-1;
		variable_types[idx] = reg_type[0];
		declared_vname = NULL;
		addVMCode(let);
		dumpVMCode(let);
	}
}

void GPerlCompiler::setEscapeStackNum(GPerlVirtualMachineCode *code, GPerlCell *)
{
	/*
	int ebp_num = 0;
	map<string, int>::iterator it = local_vmap.begin();
	while (it != local_vmap.end()) {
		string key = it->first;
		string prefix = "";
		DBG_PL("indent = [%d]", c->indent);
		for (int i = 1; i < c->indent; i++) {
			prefix += string(NAME_RESOLUTION_PREFIX);
			if (!strncmp(cstr(prefix), cstr(key), i)) {
				ebp_num++;
			}
		}
		it++;
	}
	*/
	//code->ebp = cur_stack_top;//ebp_num;
	code->cur_stack_top = cur_stack_top;//ebp_num;
	code->cur_reg_top = dst;
}

void GPerlCompiler::setCALL(GPerlVirtualMachineCode *code, GPerlCell *c)
{
	if (args_count < 5) {
		if (args_count == 0) {
			code->op = CALL;
		} else if (args_count == 1) {
			GPerlVirtualMachineCode *arg = codes->back();
			code->arg0 = arg->dst;
			popVMCode();
			code->op = FASTCALL0;
		} else if (args_count == 2) {
			GPerlVirtualMachineCode *arg = codes->back();
			code->arg1 = arg->dst;
			popVMCode();
			arg = codes->back();
			code->arg0 = arg->dst;
			popVMCode();
			code->op = FASTCALL1;
		} else if (args_count == 3) {
			GPerlVirtualMachineCode *arg = codes->back();
			code->arg2 = arg->dst;
			popVMCode();
			arg = codes->back();
			code->arg1 = arg->dst;
			popVMCode();
			arg = codes->back();
			code->arg0 = arg->dst;
			popVMCode();
			code->op = FASTCALL2;
		} else if (args_count == 4) {
			GPerlVirtualMachineCode *arg = codes->back();
			code->arg3 = arg->dst;
			popVMCode();
			arg = codes->back();
			code->arg2 = arg->dst;
			popVMCode();
			arg = codes->back();
			code->arg1 = arg->dst;
			popVMCode();
			arg = codes->back();
			code->arg0 = arg->dst;
			popVMCode();
			code->op = FASTCALL3;
		}
	} else {
		code->op = CALL;
	}
	const char *name = cstr(c->fname);
	int idx = getFuncIndex(name);
	setEscapeStackNum(code, c);
	code->dst = dst-1;
	code->src = idx;
	code->name = name;
	code->argc = args_count;
	code->cur_reg_top = dst-1;
	reg_type[dst-1] = Object;
	args_count = 0;
}

void GPerlCompiler::setBFUNC(GPerlVirtualMachineCode *code, GPerlCell *c)
{
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
	} else if (c->rawstr == "ref") {
		DBG_PL("Ref");
		code->op = REF;
	}
}

void GPerlCompiler::setFUNC(GPerlVirtualMachineCode *code, GPerlCell *c)
{
	code->op = FUNCSET;
	code->dst = func_index;
	code->src = 0;
	const char *name = cstr(c->fname);
	code->name = name;
	setToFunctionNames(name);
	declared_fname = name;
}

GPerlVirtualMachineCode *GPerlCompiler::createTHCODE(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = THCODE;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createRET(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = RET;
	code->cur_stack_top = cur_stack_top;//ebp_num;
	code->cur_reg_top = dst;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createUNDEF(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = UNDEF;
	code->cur_stack_top = cur_stack_top;//ebp_num;
	code->cur_reg_top = dst;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createWRITE(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = WRITE;
	code->dst = dst-1;
	code->cur_stack_top = cur_stack_top;//ebp_num;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createiWRITE(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = iWRITE;
	code->dst = dst-1;
	code->cur_stack_top = cur_stack_top;//ebp_num;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createsWRITE(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = sWRITE;
	code->dst = dst-1;
	code->cur_stack_top = cur_stack_top;//ebp_num;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createoWRITE(void)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = oWRITE;
	code->cur_stack_top = cur_stack_top;//ebp_num;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createPUSH(int i, int dst_)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = PUSH;
	code->src = i;
	code->dst = dst_-1;
	code->cur_stack_top = cur_stack_top;//ebp_num;
	code->cur_reg_top = dst_;
	args_count++;
	return code;
}

GPerlVirtualMachineCode *GPerlCompiler::createJMP(int jmp_num)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = JMP;
	code->jmp = jmp_num;
	code->cur_stack_top = cur_stack_top;//ebp_num;
	code->cur_reg_top = dst;
	return code;
}

void GPerlCompiler::dumpVMCode(GPerlVirtualMachineCode *code)
{
	(void)code;
	DBG_PL("L[%d] : %s [dst:%d], [src:%d], [jmp:%d], [name:%s], [ivalue: %d], [cur_stack_top: %d], [cur_reg_top: %d], [argc: %d], [arg0 : %d], [arg1: %d], [arg2: %d]",
		   code->code_num, decl_codes[code->op].name, code->dst, code->src,
		   code->jmp, code->name, code->v.ivalue, code->cur_stack_top, code->cur_reg_top, code->argc, code->arg0, code->arg1, code->arg2);
}

void GPerlCompiler::dumpPureVMCode(GPerlVirtualMachineCode *c)
{
	int code_n = (codes->size() > (size_t)code_num) ? code_num : codes->size();
	for (int i = 0; i < code_n; i++) {
		dumpVMCode(&c[i]);
	}
}
