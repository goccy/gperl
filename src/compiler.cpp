#include <gperl.hpp>

using namespace std;

GPerlCompiler::GPerlCompiler(void)
{
	dst = 0;
	src = 0;
	code_num = 0;
	for (int i = 0; i < MAX_REG_SIZE; i++) {
		reg_type[i] = Undefined;
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
		} else {
			compile_(branch, true);
		}
		code = createVMCode(parent);
		addVMCode(code);
		dumpVMCode(code);
		path = parent;
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
	case PrintDecl:
		code->op = OPPRINT;
		code->dst = 0;
		code->src = 0;
		break;
	default:
		break;
	}
	code_num++;
	return code;
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
