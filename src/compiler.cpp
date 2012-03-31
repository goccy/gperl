#include <gperl.hpp>

GPerlCompiler::GPerlCompiler(void)
{
	dst = 0;
	src = 0;
	code_num = 0;
}

GPerlVirtualMachineCode *GPerlCompiler::compile(GPerlAST *ast)
{
	GPerlCell *root = ast->root;
	for (; root; root = root->next) {
		GPerlCell *path = root;
		compile_(path);
		DBG_P("============================");
	}
	return NULL;
}

void GPerlCompiler::compile_(GPerlCell *path)
{
	for (; path->left; path = path->left) {}
	dumpVMCode(createVMCode(path));
	GPerlCell *branch = path->parent->right;
	if (branch->left == NULL && branch->right == NULL) {
		dumpVMCode(createVMCode(branch));
	} else {
		compile_(branch);
	}
	dumpVMCode(createVMCode(path->parent));
	if (path->parent->parent) {
		GPerlCell *parentBranch = path->parent->parent->right;
		if (parentBranch->left == NULL && parentBranch->right == NULL) {
			dumpVMCode(createVMCode(parentBranch));
		} else {
			compile_(parentBranch);
		}
		dumpVMCode(createVMCode(path->parent));
	}
}

GPerlVirtualMachineCode *GPerlCompiler::createVMCode(GPerlCell *c)
{
	GPerlVirtualMachineCode *code = new GPerlVirtualMachineCode();
	code->code_num = code_num;
	switch (c->type) {
	case Int:
		code->dst = dst;
		code->src = c->data.idata;
		code->op = OPMOV;
		dst++;
		break;
	case Add:
		dst--;
		code->op = OPADD;
		code->dst = dst - 1;
		code->src = dst;
		break;
	default:
		break;
	}
	code_num++;
	return code;
}

void GPerlCompiler::dumpVMCode(GPerlVirtualMachineCode *code)
{
	switch (code->op) {
	case OPMOV:
		DBG_P("L[%d] : OPMOV [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	case OPADD:
		DBG_P("L[%d] : OPADD [%d], [%d]", code->code_num, code->dst, code->src);
		break;
	default:
		break;
	}
}
