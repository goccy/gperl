#include <gperl.hpp>
using namespace std;

GPerlPackage::GPerlPackage(void) : GPerlCompiler()
{
}

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
	StaticTypingFlag = false;
	JITSafeFlag = false;
	codes = new vector<GPerlVirtualMachineCode *>();
	clses = new vector<GPerlClass *>();
	func_code = NULL;
	jit_params = new JITParams();
}

GPerlVirtualMachineCode *GPerlCompiler::compile(GPerlAST *ast, vector<GPerlClass *> *_clses)
{
	if (_clses) {
		clses->insert(clses->end(), _clses->begin(), _clses->end());
	}
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
			if (parent->type != AddEqual && parent->type != SubEqual &&
				parent->type != MulEqual && parent->type != DivEqual &&
				parent->type != Pointer) {
				if (isRIGHT_LEAF_NODE(branch)) {
					genVMCode(branch);
				} else if (isNotFALSE_STMT(parent)) {
					compile_(branch);
				}
			}
		}
		genVMCode(parent);
		path = parent;
	}
}

void GPerlCompiler::genVMCode(GPerlCell *path) {
	GPerlVirtualMachineCode *code;
	if (path->type == Call || path->type == BuiltinFunc || path->type == CodeVar) {
		genFunctionCallCode(path, 0);
	} else if (path->type == IfStmt || path->type == ElsifStmt) {
		genIfStmtCode(path);
	} else if (path->type == WhileStmt) {
		genWhileStmtCode(path);
	} else if (path->type == ForStmt) {
		genForStmtCode(path);
	} else if (path->type == ForeachStmt) {
		genForeachStmtCode(path);
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

void GPerlCompiler::genFunctionCallCode(GPerlCell *p, int offset)
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
		if (p->type == Call || p->type == BuiltinFunc || p->type == CodeVar) {
			addPushCode(i + offset, dststack[i], p->vargs[i]->type);
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
	if (StaticTypingFlag) {
		staticTypingCompile(func_code);
	} else if (JITSafeFlag) {
		jitSafeCompile(func_code);
	}
	if (!JITSafeFlag) {
		finalCompile(func_code);
	}
	GPerlVirtualMachineCode *f = getPureCodes(func_code);
	if (JITSafeFlag) {
		jit_params->params[jit_params->params_num - 1]->mtd = f;
	}
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
	mtd_map[code->name] = code->func;
	StaticTypingFlag = false;
	JITSafeFlag = false;
}

void GPerlCompiler::staticTypingCompile(vector<GPerlVirtualMachineCode *> *f)
{
	DBG_PL("StaticTypingCompile");
	vector<GPerlVirtualMachineCode *>::iterator it = f->begin();
	while (it != f->end()) {
		GPerlVirtualMachineCode *c = *it;
		switch (c->op) {
		case ADD: case SUB: case MUL: case DIV:
		case LSHIFT: case RSHIFT:
		case JG: case JL: case JGE: case JLE: case JE: case JNE:
		case ISNOT: case StringADD:
		case INC: case gINC: case DEC: case gDEC:
		case WRITE: case SHIFT: case IS:
			c->op = decl_codes[c->op + 1].code;
			break;
		default:
			break;
		}
		it++;
	}
}

JITParams::JITParams(void)
{
	params = NULL;
	params_num = 0;
}

void JITParams::addParam(JITParam *param)
{
	if (!params) {
		params = (JITParam **)safe_malloc(PTR_SIZE);
		params[0] = param;
		params_num++;
	} else {
		JITParam **tmp = (JITParam **)realloc(params, PTR_SIZE * (params_num + 1));
		if (tmp) {
			params = tmp;
			params[params_num] = param;
			params_num++;
		} else {
			fprintf(stderr, "cannot reallocate memory!!\n");
			exit(EXIT_FAILURE);
		}
	}
}

#define SET_JIT_PARAM() do {					\
		param->offsets[idx] = offset;			\
		idx++;									\
		jit_count_down++;						\
	} while (0);

void GPerlCompiler::jitSafeCompile(vector<GPerlVirtualMachineCode *> *f)
{
	DBG_PL("JITSafeCompile");
	int jit_count_down = 0;
	size_t offset = 0;
	JITParam *param = new JITParam();
	size_t idx = 0;
	vector<GPerlVirtualMachineCode *>::iterator it = f->begin();
	while (it != f->end()) {
		GPerlVirtualMachineCode *c = *it;
		switch (c->op) {
		case CALL:
			c->op = JIT_COUNTDOWN_CALL;
			SET_JIT_PARAM();
			break;
		case SELFCALL:
			c->op = JIT_COUNTDOWN_SELFCALL;
			SET_JIT_PARAM();
			break;
		case FASTCALL0:
			c->op = JIT_COUNTDOWN_FASTCALL0;
			SET_JIT_PARAM();
			break;
		case FASTCALL1:
			c->op = JIT_COUNTDOWN_FASTCALL1;
			SET_JIT_PARAM();
			break;
		case FASTCALL2:
			c->op = JIT_COUNTDOWN_FASTCALL2;
			SET_JIT_PARAM();
			break;
		case FASTCALL3:
			c->op = JIT_COUNTDOWN_FASTCALL3;
			SET_JIT_PARAM();
			break;
		case SELF_FASTCALL0:
			c->op = JIT_COUNTDOWN_SELF_FASTCALL0;
			SET_JIT_PARAM();
			break;
		case SELF_FASTCALL1:
			c->op = JIT_COUNTDOWN_SELF_FASTCALL1;
			SET_JIT_PARAM();
			break;
		case SELF_FASTCALL2:
			c->op = JIT_COUNTDOWN_SELF_FASTCALL2;
			SET_JIT_PARAM();
			break;
		case SELF_FASTCALL3:
			c->op = JIT_COUNTDOWN_SELF_FASTCALL3;
			SET_JIT_PARAM();
			break;
		case RET:
			c->op = JIT_COUNTDOWN_RET;
			break;
		default:
			break;
		}
		offset++;
		it++;
	}
	param->calls_num = idx;
	DBG_PL("jit_count_down = [%d]", jit_count_down);
	(*f->begin())->jit_count_down = jit_count_down;
	jit_params->addParam(param);
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

void GPerlCompiler::genForeachStmtCode(GPerlCell *path)
{
	dst = 0;//reset dst number
	DBG_PL("-------------genForeachStmtCode------------------");
	int vidx = 0;
	GPerlCell *array = NULL;
	if (path->right && path->right->left) {
		int idx;
		//foreach my $var (@array)
		//GPERL_EACH_INIT
		GPerlCell *var = path->right;
		string prefix = "";
		for (int i = 0; i < var->indent; i++) {
			prefix += string(NAME_RESOLUTION_PREFIX);
		}
		local_vmap[prefix + var->vname] = var->vidx;//TODO
		GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
		setInstByVMap(code, var, LET, gLET, &idx);
		vidx = idx;
		code->dst = idx;
		code->src = dst-1;
		variable_types[idx] = reg_type[0];
		declared_vname = NULL;
		addVMCode(code);
		dumpVMCode(code);

		array = path->right->left;
		code = new GPerlVirtualMachineCode();
		setInstByVMap(code, array, LET, gLET, &idx);
		if (code->op == gLET) {
			code->op = EACH_gINIT;
		} else {
			code->op = EACH_INIT;
		}
		code->dst  = array->vidx;//TODO
		code->src = idx;
		variable_types[idx] = reg_type[0];
		declared_vname = NULL;
		addVMCode(code);
		dumpVMCode(code);
	} else if (path->right) {
		//foreach (@array)
		//GPERL_EACH_INIT
		array = path->right;
		GPerlVirtualMachineCode *code = createVMCode(array);
		code->op = EACH_INIT;
		addVMCode(code);
		dumpVMCode(code);
	} else {
		fprintf(stderr, "SYNTAX ERROR : foreach stmt\n");
	}
	//GPERL_EACH_LET
	GPerlVirtualMachineCode *each_let = new GPerlVirtualMachineCode();
	each_let->op = EACH_LET;
	each_let->dst = vidx;
	each_let->src = array->vidx;
	addVMCode(each_let);
	dumpVMCode(each_let);
	int loop_start = code_num;
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
	//GPERL_EACH_STEP
	GPerlVirtualMachineCode *each_step = new GPerlVirtualMachineCode();
	each_step->op = EACH_STEP;
	each_step->src = array->vidx;
	addVMCode(each_step);
	dumpVMCode(each_step);
	jmp->jmp = code_num - cond_code_num + 2;// + 1/*OPNOP + OPJMP + 1*/;
	int cur_code_num = code_num;
	jmp = createJMP(1);
	addVMCode(jmp);
	dumpVMCode(jmp);
	each_step->jmp = loop_start - cur_code_num;
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

void GPerlCompiler::addPushCode(int i, int dst_, GPerlT type)
{
	GPerlVirtualMachineCode *code = NULL;
	if (type == ArrayVar) {
		code = createPUSH(i, dst_);
		//code = createArrayPUSH(i, dst_);
	} else {
		code = createPUSH(i, dst_);
	}
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
		case STATIC_ADD:
			OPCREATE_TYPE1(STATIC_ADD);
			break;
		case iADD:
			OPCREATE_TYPE1(iADD);
			break;
		case SUB:
			OPCREATE_TYPE1(SUB);
			break;
		case STATIC_SUB:
			OPCREATE_TYPE1(STATIC_SUB);
			break;
		case iSUB:
			OPCREATE_TYPE1(iSUB);
			break;
		case MUL:
			OPCREATE_TYPE1(MUL);
			break;
		case STATIC_MUL:
			OPCREATE_TYPE1(STATIC_MUL);
			break;
		case iMUL:
			OPCREATE_TYPE1(iMUL);
			break;
		/*========= TYPE2 =========*/
		case iADDC:
			OPCREATE_TYPE2(iADDC);
			break;
		case iSUBC:
			OPCREATE_TYPE2(iSUBC);
			break;
		case iMULC:
			OPCREATE_TYPE2(iMULC);
			break;
		case JL:
			OPCREATE_TYPE2(JL);
			break;
		case STATIC_JL:
			OPCREATE_TYPE2(STATIC_JL);
			break;
		case JG:
			OPCREATE_TYPE2(JG);
			break;
		case STATIC_JG:
			OPCREATE_TYPE2(STATIC_JG);
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
		case aPUSH:
			OPCREATE_TYPE2(aPUSH);
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

#define SET_OPCODE(T) {								\
		dst--;										\
		code->src = dst;							\
		switch (reg_type[dst - 1]) {				\
		case Int:									\
			code->op = INT(T);						\
			break;									\
		case Double:								\
			code->op = DOUBLE(T);					\
			break;									\
		case Object:								\
			switch (reg_type[dst]) {				\
			case Int:								\
				if (codes->back()->op == MOV) {		\
					code->op = INTC(T);				\
					code->src = codes->back()->src;	\
					code->v = codes->back()->v;		\
					popVMCode();					\
				} else {							\
					code->op = T;					\
				}									\
				code->code_num = code_num;			\
				break;								\
			default:								\
				code->op = T;						\
				break;								\
			}										\
			break;									\
		default:									\
			code->op = T;							\
			break;									\
		}											\
		code->dst = dst - 1;						\
		code->jmp = 1;								\
	}

GPerlVirtualMachineCode *GPerlCompiler::createVMCode(GPerlCell *c)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	switch (c->type) {
	case Int: case Double:
	case String: case List: case Key:
	case ArrayRef: case HashRef: case CodeRef:
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
	case LeftShift:
		SET_OPCODE(LSHIFT);
		break;
	case RightShift:
		SET_OPCODE(RSHIFT);
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
	case HashDereference:
		setHashDereference(code, c);
		break;
	case IfStmt: case WhileStmt: case ForStmt: case ElsifStmt: case ForeachStmt:
		code->op = NOP;
		break;
	case GlobalVarDecl: case LocalVarDecl: case VarDecl:
		setSETv(code, c);
		break;
	case MultiGlobalVarDecl: case MultiLocalVarDecl:
		setMultiSETv(code, c);
		break;
	case LocalVar: case Var: case ArrayVar: case CodeVar:
	case HashVar: case SpecificValue:
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
	case AddEqual: case SubEqual: case MulEqual: case DivEqual:
		setOpAssign(code, c);
		break;
	case ArrayAt:
		setArrayAt(code, c);
		break;
	case Arrow:
		setArrow(code, c);
		break;
	case Pointer:
		setPointer(code, c);
		break;
	case HashAt:
		setHashAt(code, c);
		break;
	case Annotation:
		setAnnotation(code, c);
		break;
	case Function:
		setFUNC(code, c);
		break;
	case Return:
		code->op = RET;
		code->dst = 0;
		code->src = dst-1;
		break;
	case Package: {
		GPerlPackage pkg;
		GPerlVirtualMachineCode *pkg_code = pkg.compile(c->pkg_stmt, clses);
		pkg.dumpPureVMCode(pkg_code);
		map<string, GPerlVirtualMachineCode *>::iterator it = pkg.mtd_map.begin();
		size_t mtd_n = pkg.mtd_map.size();
		GPerlFunc **mtd_table = (GPerlFunc **)safe_malloc(PTR_SIZE * HASH_TABLE_SIZE);
		GPerlString **mtd_names = (GPerlString **)safe_malloc(PTR_SIZE * (mtd_n + 1));
		mtd_names[mtd_n] = NULL;
		for (size_t i = 0; i < mtd_n; i++) {
			string fname = (string)(*it).first;
			GPerlVirtualMachineCode *f = (GPerlVirtualMachineCode *)(*it).second;
			char *name = (char *)cstr(fname);
			size_t len = strlen(name) + 1;
			char *new_name = (char *)safe_malloc(len);
			memcpy(new_name, name, len);
			GPerlString *s = new_GPerlInitString(new_name, len);
			mtd_names[i] = s;
			DBG_PL("name = [%s], hash = [%lu]", new_name, s->hash);
			mtd_table[s->hash] = (GPerlFunc *)new_GPerlFunc(new_name, f);
			it++;
		}
		GPerlClass *gclass = new_GPerlClass(cstr(c->rawstr), mtd_table);
		gclass->ext->mtd_names = mtd_names;
		clses->push_back(gclass);
		if (c->rawstr == "main") {
			vector<GPerlVirtualMachineCode *> *main_codes = pkg.codes;
			main_codes->erase(main_codes->begin());//remove THCODE
			codes->insert(codes->end(), main_codes->begin(), main_codes->end());
		}
		code->op = NOP;
		break;
	}
	default:
		code->op = NOP;
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
	case String: case Key:
		code->src = -1;
		code->op = NEW_STRING;
		code->_new = new_GPerlString;
		STRING_init(code->v, new_GPerlInitString(c->data.sdata, strlen(c->data.sdata) + 1));
		init_values[init_value_idx] = code->v;
		init_value_idx++;
		break;
	case List: case ArrayRef: case HashRef: {
		size_t argsize = c->argsize;
		int dststack[argsize * 2];
		GPerlT typestack[argsize * 2];
		size_t fixed_argsize = argsize;
		size_t j = 0;
		for (size_t i = 0; i < argsize; i++, j++) {
			if (c->vargs[i]->type == Arrow) {
				GPerlCell *_key = c->vargs[i]->left;
				GPerlCell *_value = c->vargs[i]->right;
				GPerlVirtualMachineCode *key = createVMCode(_key);
				addVMCode(key);
				dumpVMCode(key);
				dststack[j] = dst;
				typestack[j] = _key->type;
				j++;
				GPerlVirtualMachineCode *value = createVMCode(_value);
				addVMCode(value);
				dumpVMCode(value);
				dststack[j] = dst;
				typestack[j] = _value->type;
				fixed_argsize++;
			} else {
				compile_(c->vargs[i]);
				dststack[i] = dst;
				typestack[j] = c->vargs[i]->type;
			}
		}
		c->argsize = fixed_argsize;
		for (size_t i = 0; i < fixed_argsize; i++) {
			addPushCode(i, dststack[i], typestack[i]);
		}
		code->op = NEW;
		size_t size = sizeof(GPerlValue) * (c->argsize + 1);
		GPerlValue *list = (GPerlValue *)safe_malloc(size);
		if (cstr(c->parent->left->vname)[0] == '%' || c->type == HashRef) {
			OBJECT_init(code->v, new_GPerlInitHash(list, c->argsize));
			code->_new = new_GPerlHash;
			if (type == HashRef) {
				GPerlHash *h = (GPerlHash *)getObject(code->v);
				h->h.type = HashRef;
			}
		} else {
			OBJECT_init(code->v, new_GPerlInitArray(list, c->argsize));
			code->_new = new_GPerlArray;
			if (type == ArrayRef) {
				GPerlArray *a = (GPerlArray *)getObject(code->v);
				a->h.type = ArrayRef;
			}
		}
		args_count = 0;
		init_values[init_value_idx] = code->v;
		init_value_idx++;
		code->src = 0;
		type = Object;
		break;
	}
	case CodeRef: {
		DBG_PL("CodeReference");
		code->op = CODE_REF;
		const char *fname = cstr(c->left->fname);
		code->src = getFuncIndex(fname);
		code->name = fname;
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
	code->dst = dst;
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
		}
	}
	if (global_vmap.find(c->vname) != global_vmap.end()) {
		*idx = global_vmap[c->vname];
		code->op = gop;
		c->vname = c->vname;
		code->name = cstr(c->vname);
		goto BREAK;
	}
	fprintf(stderr, "ERROR: cannot find variable name : [%s]\n", cstr(c->vname));
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

void GPerlCompiler::setHashDereference(GPerlVirtualMachineCode *code, GPerlCell *c_)
{
	(void)c_;
	code->op = HASH_DREF;
	code->src = dst-1;
	code->dst = dst;
	dst++;
}

void GPerlCompiler::setArraySet(GPerlVirtualMachineCode *, GPerlCell *)
{
	DBG_PL("ArraySet");
}

void GPerlCompiler::setArrayAt(GPerlVirtualMachineCode *code, GPerlCell *c_)
{
	GPerlCell *parent = c_->parent;
	if (parent && parent->left == c_ &&
		(parent->type == Assign || parent->type == Inc || parent->type == Dec ||
		 parent->type == ArrayAt)) {
		code->op = NOP;
		c_->type = ArraySet;
		return;
	}
	int idx = 0;
	if (c_->left->type == Pointer) {
		//ArrayRefAt
		GPerlCell *c = c_->left->left;
		if (reg_type[dst - 1] == Int) {
			idx = codes->back()->v.ivalue;
			dst--;
			popVMCode();//remove MOV
			code->idx = c_->right->data.idata;
			if (c->type == ArrayAt) {
				code->op = ARRAY_AT;
				code->dst = dst;
				code->src = dst-1;
				code->idx = idx;
				reg_type[dst] = Object;
				dst++;
				return;
			} else {
				setInstByVMap(code, c, ARRAY_ATC, ARRAY_gATC, &idx);
			}
		} else {
			setInstByVMap(code, c, ARRAY_AT, ARRAY_gAT, &idx);
			code->idx = dst-1;
		}
	} else {
		GPerlCell *c = c_->left;
		c->vname.replace(0, 1, "@");
		c->fname.replace(0, 1, "@");
		c->rawstr.replace(0, 1, "@");
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
		c->vname.replace(0, 1, "$");
		c->fname.replace(0, 1, "$");
		c->rawstr.replace(0, 1, "$");
	}
	code->src = idx;
	code->dst = dst;
	reg_type[dst] = Object;
	dst++;
}

void GPerlCompiler::setHashAt(GPerlVirtualMachineCode *code, GPerlCell *c_)
{
	GPerlCell *parent = c_->parent;
	if (parent && parent->left == c_ &&
		(parent->type == Assign || parent->type == Inc || parent->type == Dec ||
		 parent->type == HashAt)) {
		code->op = NOP;
		c_->type = HashSet;
		return;
	}
	int idx = 0;
	if (c_->left->type == Pointer) {
		//HashRefAt
		GPerlCell *c = c_->left->left;
		if (reg_type[dst - 1] == String) {
			idx = codes->back()->v.ivalue;
			dst--;
			popVMCode();//remove MOV
			char *key = c_->right->data.sdata;
			code->hash = hash(key, strlen(key) + 1) % HASH_TABLE_SIZE;
			setInstByVMap(code, c, HASH_ATC, HASH_gATC, &idx);
		} else {
			setInstByVMap(code, c, HASH_AT, HASH_gAT, &idx);
			code->idx = dst-1;
		}
	} else {
		GPerlCell *c = c_->left;
		c->vname.replace(0, 1, "%");
		c->fname.replace(0, 1, "%");
		c->rawstr.replace(0, 1, "%");
		if (reg_type[dst - 1] == String) {
			idx = codes->back()->v.ivalue;
			dst--;
			popVMCode();//remove MOV
			char *key = c_->right->data.sdata;
			code->hash = hash(key, strlen(key) + 1) % HASH_TABLE_SIZE;
			setInstByVMap(code, c, HASH_ATC, HASH_gATC, &idx);
		} else {
			setInstByVMap(code, c, HASH_AT, HASH_gAT, &idx);
			code->idx = dst-1;
		}
		c->vname.replace(0, 1, "$");
		c->fname.replace(0, 1, "$");
		c->rawstr.replace(0, 1, "$");
	}
	code->src = idx;
	code->dst = dst;
	reg_type[dst] = Object;
	dst++;
}

void GPerlCompiler::setArrow(GPerlVirtualMachineCode *code, GPerlCell *)
{
	DBG_PL("Arrow");
	code->op = NOP;
}

GPerlClass *GPerlCompiler::getClassByName(string name)
{
	size_t size = clses->size();
	for (size_t i = 0; i < size; i++) {
		GPerlClass *gclass = clses->at(i);
		if (gclass->ext->class_name == name) {
			return gclass;
		}
	}
	return NULL;
}

void GPerlCompiler::setPointer(GPerlVirtualMachineCode *code, GPerlCell *c)
{
	DBG_PL("Pointer");
	if (c->parent && c->parent->type == HashAt ||
		c->parent && c->parent->type == ArrayAt) {
		code->op = NOP;
		return;
	}
	if (c->left->type == Class) {
		//Static Method Call
		string class_name = c->left->rawstr;
		GPerlClass *gclass = getClassByName(class_name);
		char *mtd_name = (char *)cstr(c->right->rawstr);
		unsigned long h = hash(mtd_name, strlen(mtd_name) + 1) % HASH_TABLE_SIZE;
		DBG_PL("mtd_name = [%s], hash = [%lu]", mtd_name, h);
		GPerlFunc *mtd = gclass->mtds[h];
		if (!mtd) {
			fprintf(stdout, "error!!: cannot find method name[%s]\n", mtd_name);
			exit(EXIT_FAILURE);
		}
		GPerlVirtualMachineCode *smov = new GPerlVirtualMachineCode();
		setMOV(smov, new GPerlCell(String, class_name));
		addVMCode(smov);
		dumpVMCode(smov);
		addPushCode(0, dst, String);
		genFunctionCallCode(c->right, 1);
		GPerlVirtualMachineCode *mtd_call = new GPerlVirtualMachineCode();
		mtd_call->op = STATIC_CALL;
		mtd_call->dst = dst - 1;
		mtd_call->src = dst;
		mtd_call->name = mtd_name;
		mtd_call->argc = args_count;
		mtd_call->cur_reg_top = dst;
		OBJECT_init(mtd_call->v, mtd);
		setEscapeStackNum(mtd_call, c->right);
		addVMCode(mtd_call);
		dumpVMCode(mtd_call);
		args_count = 0;
		code->op = NOP;
	} else {
		//Dynamic Method Call
		char *mtd_name = (char *)cstr(c->right->rawstr);
		GPerlVirtualMachineCode *vmov = new GPerlVirtualMachineCode();
		int self_reg_idx = dst - 1;
		setVMOV(vmov, c->left);
		addVMCode(vmov);
		dumpVMCode(vmov);
		addPushCode(0, dst, Object);
		genFunctionCallCode(c->right, 1);
		GPerlVirtualMachineCode *mtd_call = new GPerlVirtualMachineCode();
		mtd_call->op = DYNAMIC_CALL;
		mtd_call->dst = dst - 1;
		mtd_call->src = self_reg_idx;
		mtd_call->name = mtd_name;
		mtd_call->hash = hash(mtd_name, strlen(mtd_name) + 1) % HASH_TABLE_SIZE;
		mtd_call->argc = args_count;
		mtd_call->cur_reg_top = dst;
		setEscapeStackNum(mtd_call, c->right);
		addVMCode(mtd_call);
		dumpVMCode(mtd_call);
		args_count = 0;
		code->op = NOP;
	}
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
	int idx = 0;
	GPerlCell *parent = c->parent;
	if (parent && parent->left == c &&
        (parent->type == Assign || parent->type == Inc || parent->type == Dec ||
		 parent->type == ArrayAt || parent->type == HashAt ||
		 parent->type == AddEqual || parent->type == SubEqual ||
		 parent->type == MulEqual || parent->type == DivEqual)) {
		code->op = NOP;
		return;
	}
	setInstByVMap(code, c, vMOV, gMOV, &idx);
	code->src = idx;
	code->dst = dst;
	if (c->type == ArrayVar) {
		reg_type[dst] = Array;
	} else if (c->type == CodeVar) {
		GPerlVirtualMachineCode *vmov = new GPerlVirtualMachineCode();
		vmov->op = code->op;
		vmov->src = idx;
		vmov->dst = dst;
		setEscapeStackNum(vmov, c);
		addVMCode(vmov);
		dumpVMCode(vmov);
		GPerlVirtualMachineCode *closure = new GPerlVirtualMachineCode();
		closure->op = CLOSURE;
		closure->dst = dst;
		closure->src = dst;
		closure->name = cstr(c->vname);
		closure->argc = args_count;
		closure->cur_reg_top = dst;
		setEscapeStackNum(closure, c);
		addVMCode(closure);
		dumpVMCode(closure);
		code->op = NOP;
		args_count = 0;
	} else {
		reg_type[dst] = Object;
	}
	dst++;
}

void GPerlCompiler::setOpAssign(GPerlVirtualMachineCode *_code, GPerlCell *c)
{
	DBG_PL("OpAssign");
	GPerlT type = c->type;
	c->type = Undefined;//Dummy Operation for escape first branch of VMOV
	GPerlVirtualMachineCode *vmov = new GPerlVirtualMachineCode();
	setVMOV(vmov, c->left);
	addVMCode(vmov);
	dumpVMCode(vmov);
	int op_dst = dst - 1;
	//DBG_PL("op_dst = [%d]", op_dst);
	DBG_PL("COMPIEL RIGHT CELL");
	compile_(c->right);
	DBG_PL("COMPILE OP");
	c->type = type;
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	switch (c->type) {
	case AddEqual:
		SET_OPCODE(ADD);
		break;
	case SubEqual:
		SET_OPCODE(SUB);
		break;
	case MulEqual:
		SET_OPCODE(MUL);
		break;
	case DivEqual:
		SET_OPCODE(DIV);
		break;
	default:
		break;
	}
	code->dst = op_dst;
	addVMCode(code);
	dumpVMCode(code);
	setLET(_code, c);
	_code->src = op_dst;
}

void GPerlCompiler::setLET(GPerlVirtualMachineCode *code, GPerlCell *c)
{
	DBG_PL("setLET");
	int idx = 0;
	if (c->left->type == MultiLocalVarDecl || c->left->type == MultiGlobalVarDecl) {
		GPerlCell *multi_decl = c->left;
		for (int i = 0; i < multi_decl->argsize; i++) {
			GPerlCell *v = multi_decl->vargs[i];
			GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
			setInstByVMap(code, v, MLET, gMLET, &idx);// reg[idx] = reg[pc->src]->list[i]
			code->dst = idx;
			code->src = dst-1;
			code->idx = i;
			addVMCode(code);
			dumpVMCode(code);
			//variable_types[idx] = reg_type[0];
		}
		code->op = NOP;
	} else if (c->left->type == ArraySet) {
		GPerlCell *a = c->left;
		GPerlCell *var = a->left;
		var->vname.replace(0, 1, "@");
		var->fname.replace(0, 1, "@");
		var->rawstr.replace(0, 1, "@");
		setInstByVMap(code, var, ARRAY_SET, ARRAY_gSET, &idx);
		code->dst = idx;
		code->src = dst-1;
		code->idx = dst-2;
		variable_types[idx] = reg_type[0];
		declared_vname = NULL;
		var->vname.replace(0, 1, "$");
		var->fname.replace(0, 1, "$");
		var->rawstr.replace(0, 1, "$");
	} else if (c->left->type == HashSet) {

	} else {
		c->indent = c->left->indent;
		setInstByVMap(code, c, LET, gLET, &idx);
		code->dst = idx;
		code->src = dst-1;
		variable_types[idx] = reg_type[0];
		declared_vname = NULL;
	}
}

void GPerlCompiler::setSETv(GPerlVirtualMachineCode *code, GPerlCell *c)
{
	const char *name = cstr(c->vname);
	switch (c->type) {
	case GlobalVarDecl:
		global_vmap[c->vname] = c->vidx;
		break;
	case LocalVarDecl: case VarDecl:
		if (c->indent > 0) {
			string prefix = "";
			for (int i = 0; i < c->indent; i++) {
				prefix += string(NAME_RESOLUTION_PREFIX);
			}
			local_vmap[prefix + c->vname] = c->vidx;
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
		OBJECT_init(mov->v, undef);
		init_values[init_value_idx] = mov->v;
		init_value_idx++;
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

void GPerlCompiler::setMultiSETv(GPerlVirtualMachineCode *code, GPerlCell *c)
{
	DBG_PL("MULTI SETv");
	code->op = NOP;
	for (int i = 0; i < c->argsize; i++) {
		GPerlCell *v = c->vargs[i];
		setSETv(new GPerlVirtualMachineCode(), v);
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
	if (c->parent && c->parent->type == CodeRef) {
		code->op = NOP;
		return;
	}
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
	} else if (c->rawstr == "map") {
		DBG_PL("Map");
	} else if (c->rawstr == "keys") {
		DBG_PL("Keys");
		code->op = KEYS;
	} else if (c->rawstr == "values") {
		DBG_PL("Values");
		code->op = VALUES;
	} else if (c->rawstr == "bless") {
		DBG_PL("Bless");
		code->op = BLESS;
		code->dst = dst - 1;
	}
	args_count = 0;
}

void GPerlCompiler::setAnnotation(GPerlVirtualMachineCode *code, GPerlCell *c)
{
	DBG_PL("Annotation");
	if (c->rawstr == "static_typing") {
		StaticTypingFlag = true;
	} else if (c->rawstr == "jit_safe") {
#ifdef ENABLE_JIT_COMPILE
		JITSafeFlag = true;
#endif
	}
	code->op = NOP;
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

GPerlVirtualMachineCode *GPerlCompiler::createArrayPUSH(int i, int dst_)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	code->op = aPUSH;
	code->src = i;
	code->dst = dst_-1;
	code->cur_stack_top = cur_stack_top;//ebp_num;
	code->cur_reg_top = dst_;
	args_count++;
	return code;
}

void GPerlCompiler::genMapFunctionCode(GPerlCell *path)
{
	GPerlVirtualMachineCode *argmov = new GPerlVirtualMachineCode();
	//argmov->setArrayARGMOV(argmov, NULL);
	argmov->dst = 0; argmov->src = 0;
	GPerlVirtualMachineCode *let_recv = new GPerlVirtualMachineCode();
	let_recv->op = LET; let_recv->dst = 0; let_recv->src = 0;
	GPerlVirtualMachineCode *mov = new GPerlVirtualMachineCode();
	mov->op = MOV; mov->dst = 0; mov->src = 0;
	GPerlVirtualMachineCode *let_ret = new GPerlVirtualMachineCode();
	let_ret->op = LET; let_ret->dst = 1; let_recv->src = 0;
	GPerlVirtualMachineCode *let_$_ = new GPerlVirtualMachineCode();
	let_$_->op = LET; let_ret->dst = 3; let_recv->src = 0;
	GPerlVirtualMachineCode *each_init = new GPerlVirtualMachineCode();
	each_init->op = EACH_INIT; each_init->dst = 2; each_init->src = 0;
	GPerlVirtualMachineCode *each_let = new GPerlVirtualMachineCode();
	each_let->op = EACH_LET; each_init->dst = 3; each_init->src = 2;
	size_t block_code_num = 0;
	(void)path;
	/*
	  GPerlVirtualMachineCode *vmov = new GPerlVirtualMachineCode();
	  vmov->op = vMOV; vmov->dst = 0; vmov->src = 1;
	  B_vMOV [dst:1], [src:3], [jmp:0], [name:*$_]
	  iMULC [dst:1], [src:0], [jmp:1], [name:(null)], [ivalue: 2]
	 */

	each_let->jmp = 5 + block_code_num;
	GPerlVirtualMachineCode *apush = new GPerlVirtualMachineCode();
	apush->op = aPUSH; apush->dst = 0; apush->src = 0;
	GPerlVirtualMachineCode *push = new GPerlVirtualMachineCode();
	push->op = PUSH; push->dst = 1; push->src = 1;
	GPerlVirtualMachineCode *array_push = new GPerlVirtualMachineCode();
	array_push->op = ARRAY_PUSH; push->dst = 0; push->src = 0;
	GPerlVirtualMachineCode *each_step = new GPerlVirtualMachineCode();
	each_step->op = EACH_STEP; each_step->dst = 0; each_step->src = 2;
	each_step->jmp = -4 - block_code_num;

	GPerlVirtualMachineCode *vmov = new GPerlVirtualMachineCode();
	vmov->op = vMOV; vmov->dst = 0; vmov->src = 1;
	GPerlVirtualMachineCode *ret = new GPerlVirtualMachineCode();
	ret->op = RET; ret->dst = 0; ret->src = 0;
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
	int code_n = codes->size();//(codes->size() > (size_t)code_num) ? code_num : codes->size();
	for (int i = 0; i < code_n; i++) {
		dumpVMCode(&c[i]);
	}
}
