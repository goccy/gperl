#include <gperl.hpp>

using namespace std;

GPerlCell::GPerlCell(GPerlT type_) : type(type_), vidx(0)
{
	data.pdata = NULL;
	left = NULL;
	true_stmt = NULL;
	pkg_stmt = NULL;
	right = NULL;
	parent = NULL;
	next = NULL;
	for (int i = 0; i < MAX_ARGSTACK_SIZE; i++) {
		vargs[i] = NULL;
	}
	argsize = 0;
}

GPerlCell::GPerlCell(GPerlT type_, string name) : type(type_), vidx(0)
{
	data.pdata = NULL;
	left = NULL;
	true_stmt = NULL;
	pkg_stmt = NULL;
	right = NULL;
	parent = NULL;
	next = NULL;
	int i = 0;
	for (i = 0; i < MAX_ARGSTACK_SIZE; i++) {
		vargs[i] = NULL;
	}
	for (i = 0; i < Undefined; i++) {
		if (type_ == decl_tokens[i].type) {
			if (name == "BuiltinFunc") {
				for (int j = 0; j < Undefined; j++) {
					if (name == decl_tokens[j].data) {
						info = decl_tokens[j];
						break;
					}
				}
			} else {
				info = decl_tokens[i];
			}
			break;
		}
	}
	if (i == Undefined) info = decl_tokens[i];
	argsize = 0;
	if (type_ == Int) {
		data.idata = atoi(cstr(name));
	} else if (type_ == Double) {
		data.ddata = atof(cstr(name));
	} else if (type == String || type == Key) {
		data.sdata = (char *)cstr(name);
	} else {
		vname = name;
		fname = name;
	}
	rawstr = name;
}

void GPerlCell::setVariableIdx(int idx)
{
	vidx = idx;
}

GPerlParser::GPerlParser(vector<GPerlToken *> *tokens, int _argc, char **_argv)
	: argc(_argc), argv(_argv)
{
	it = tokens->begin();
	end = tokens->end();
	pkgs = new vector<GPerlCell *>();
	iterate_count = 0;
	func_iterate_count = 0;
	vidx = 0;
	gidx = 0;
	vcount = 0;
	indent = 0;
}

GPerlNodes::GPerlNodes() : vector<GPerlNode *>()
{
	block_num = 0;
}

void GPerlNodes::pushNode(GPerlNode *node)
{
	push_back(node);
	block_num++;
}

void GPerlNodes::popNode(void)
{
	pop_back();
	block_num--;
}

void GPerlNodes::swapLastNode(GPerlNode *node)
{
	pop_back();
	push_back(node);
}

void GPerlNodes::clearNodes(void)
{
	clear();
	block_num = 0;

}

GPerlNode *GPerlNodes::lastNode(void)
{
	return at(block_num - 1);
}

#define MOVE_NEXT_TOKEN() {						\
		it++;									\
		iterate_count++;						\
	}

GPerlFlags::GPerlFlags(void): isVarDeclFlag(false), isCallDeclFlag(false),
							  ifStmtFlag(false), unlessStmtFlag(false),
							  elseStmtFlag(false), elsifStmtFlag(false),
							  funcFlag(false), whileStmtFlag(false),
							  forStmtFlag(false), foreachStmtFlag(false),
							  condIndentFlag(false), packageFlag(false),
							  annotateFlag(false)
{
}

void GPerlParser::parseValue(GPerlToken *t, GPerlNodes *blocks, GPerlScope *scope)
{
	GPerlCell *block = (blocks->block_num > 0) ? blocks->lastNode() : NULL;
	if (block && block->type != Assign && block->type != Return &&
		block->type != AddEqual && block->type != SubEqual &&
		block->type != MulEqual && block->type != DivEqual) {
		DBG_PL("%s", block->info.name);
		if (block->type == Call || block->type == BuiltinFunc ||
			block->type == CodeVar) {
			parseFunctionCall(t, blocks, scope);
		} else if (block->left == NULL) {
			parseLeftTerm(t, blocks, scope);
		} else if (block->right == NULL) {
			parseRightTerm(t, blocks, scope);
		} else {
			fprintf(stderr, "ERROR:[parse error]!!\n");
		}
	} else {
		parseSingleTerm(t, blocks, scope);
	}
}

void GPerlParser::parseFunctionCall(GPerlToken *t, GPerlNodes *blocks, GPerlScope *scope)
{
	DBG_PL("[%s]:NEW BLOCK->BLOCKS[%d]", cstr(t->data), __LINE__);
	GPerlT type = t->info.type;
	GPerlCell *block = blocks->lastNode();
	if (scope &&
		scope->root->type == Call || scope->root->type == BuiltinFunc ||
		scope->root->type == CodeVar) {
		block->vargs[0] = scope->root;
		block->argsize = 1;
	} else if (scope && scope->root->argsize > 0) {
		for (int i = 0; i < scope->root->argsize; i++) {
			block->vargs[i] = scope->root->vargs[i];
		}
		block->argsize = scope->root->argsize;
	} else if (scope && scope->size == 1) {
		if (scope->root->type == Return) {
			//void argument
		} else {
			block->vargs[0] = scope->root;
			block->argsize = 1;
		}
	} else if (block->rawstr == "map" && scope) {
		GPerlCell *arg_block = scope->root;
		if (arg_block->next->type == ArrayVar) {
			GPerlCell *v = new GPerlCell(LocalVarDecl, "$_");
			v->indent++;
			v->setVariableIdx(vidx);
			vcount++;
			vidx++;
			v->next = arg_block;
			block->vargs[0] = v;
			block->vargs[1] = arg_block->next;
			arg_block->next = NULL;
			block->argsize = 2;
		} else {
			fprintf(stderr, "Syntax Error: near by map\n");
		}
	} else {
		GPerlCell *v = new GPerlCell(type, t->data);
		v->indent = indent;
		blocks->pushNode(v);
	}
}

void GPerlParser::parseSingleTerm(GPerlToken *t, GPerlNodes *blocks, GPerlScope *scope)
{
	GPerlT type = t->info.type;
	if (scope && scope->root->argsize > 1 && type == LeftParenthesis) {
		DBG_PL("List");
		scope->root->type = List;
		scope->root->indent = indent;
		blocks->pushNode(scope->root);
	} else if (scope && scope->root->argsize > 1 && type == LeftBracket) {
		DBG_PL("ArrayRef");
		scope->root->type = ArrayRef;
		scope->root->indent = indent;
		blocks->pushNode(scope->root);
	} else if (scope && scope->root->argsize > 1 && type == LeftBrace) {
		DBG_PL("HashRef");
		scope->root->type = HashRef;
		scope->root->indent = indent;
		blocks->pushNode(scope->root);
	} else if (scope && scope->size > 1) {
		DBG_PL("cond @forstmt");
		GPerlCell *b = scope->root;
		b->indent = indent;
		blocks->pushNode(b);
	} else if (scope && scope->root->argsize == 0) {
		GPerlCell *b = scope->root;
		b->indent = indent;
		blocks->pushNode(b);
	} else if (scope && (scope->root->type == BuiltinFunc || scope->root->type == Call || scope->root->type == CodeVar)) {
		GPerlCell *b = scope->root;
		b->indent = indent;
		blocks->pushNode(b);
	} else {
		DBG_PL("[%s]:NEW BLOCK->BLOCKS[%d]", cstr(t->data), __LINE__);
		GPerlCell *b = (scope) ? scope->root->vargs[0] : new GPerlCell(type, t->data);
		b->indent = indent;
		blocks->pushNode(b);
	}
}

void GPerlParser::parseLeftTerm(GPerlToken *t, GPerlNodes *blocks, GPerlScope *scope)
{
	DBG_PL("[%s]:LAST BLOCK->left", cstr(t->data));
	GPerlT type = t->info.type;
	GPerlCell *block = blocks->lastNode();
	GPerlCell *b = NULL;
	if (scope && scope->size > 0 && !scope->root->vargs[0]) {
		b = scope->root;
	} else if (scope) {
		b = scope->root->vargs[0];
	} else {
		b = new GPerlCell(type, t->data);
	}
	b->indent = indent;
	block->left = b;
	b->parent = block;
	if (type == Call || type == BuiltinFunc || type == CodeVar) blocks->pushNode(b);
}

void GPerlParser::parseRightTerm(GPerlToken *t, GPerlNodes *blocks, GPerlScope *scope)
{
	DBG_PL("[%s]:LAST BLOCK->right", cstr(t->data));
	GPerlT type = t->info.type;
	GPerlCell *block = blocks->lastNode();
	GPerlCell *b = NULL;
	if (scope && scope->size > 0 && !scope->root->vargs[0]) {
		b = scope->root;
	} else if (scope) {
		b = scope->root->vargs[0];
	} else {
		b = new GPerlCell(type, t->data);
	}
	b->indent = indent;
	block->right = b;
	b->parent = block;
	if (type == Call || type == BuiltinFunc || type == CodeVar) blocks->pushNode(b);
}

void GPerlParser::parseLocalVar(GPerlToken *t, GPerlNodes *blocks, GPerlFlags *flags)
{
	if (flags->isVarDeclFlag) {
		DBG_PL("[%s]:NEW BLOCK => BLOCKS", cstr(t->data));
		DBG_PL("vidx = [%d]", vidx);
		DBG_PL("indent = [%d]", indent);
		if (indent > 0) {
			GPerlCell *var = new GPerlCell(LocalVarDecl, t->data);
			var->indent = indent;
			var->setVariableIdx(vidx);
			blocks->pushNode(var);
			flags->isVarDeclFlag = false;
			vcount++;
			vidx++;
		} else {
			DBG_PL("gidx = [%d]", gidx);
			GPerlCell *gvar = new GPerlCell(GlobalVarDecl, t->data);
			gvar->indent = indent;
			gvar->setVariableIdx(gidx);
			blocks->pushNode(gvar);
			flags->isVarDeclFlag = false;
			gidx++;
		}
	}
}

void GPerlParser::parseGlobalVar(GPerlToken *t, GPerlNodes *blocks, GPerlFlags *flags)
{
	DBG_PL("[%s]:NEW BLOCK => BLOCKS", cstr(t->data));
	DBG_PL("gidx = [%d]", gidx);
	DBG_PL("INDENT = [%d]", indent);
	GPerlCell *gvar = new GPerlCell(GlobalVarDecl, t->data);
	gvar->indent = indent;
	gvar->setVariableIdx(gidx);
	blocks->pushNode(gvar);
	flags->isVarDeclFlag = false;
	gidx++;
}

void GPerlParser::parseSingleTermOperator(GPerlToken *t, GPerlNodes *blocks)
{
	DBG_PL("[%s]:LAST BLOCK->PARENT", cstr(t->data));
	GPerlCell *b = new GPerlCell(t->info.type, t->data);
	blocks->pushNode(b);
}

void GPerlParser::parseDoubleTermOperator(GPerlToken *t, GPerlNodes *blocks)
{
	DBG_PL("[%s]:LAST BLOCK->PARENT", cstr(t->data));
	if (t->info.type == Pointer) {
		asm("int3");
	}
	GPerlCell *block = blocks->lastNode();
	GPerlCell *b = new GPerlCell(t->info.type, t->data);
	block->parent = b;
	b->left = block;
	blocks->swapLastNode(b);
}

GPerlCell *GPerlParser::parseAssign(GPerlToken *t, GPerlNodes *blocks)
{
	DBG_PL("ASSIGN:LAST BLOCK->PARENT");
	GPerlCell *block = NULL;
	if (blocks->block_num > 0) {
		block = blocks->lastNode();
		blocks->popNode();
	} else {
		fprintf(stderr, "ERROR:syntax error!!\n");
	}
	GPerlCell *assign = new GPerlCell(t->info.type, t->data);
	assign->indent = block->indent;
	assign->vname = block->vname;
	block->parent = assign;
	assign->left = block;
	blocks->pushNode(assign);
	return assign;
}

void GPerlParser::parseFunc(GPerlToken *, GPerlNodes *blocks, GPerlFlags *flags, GPerlCell *, GPerlAST *ast)
{
	MOVE_NEXT_TOKEN();
	GPerlScope *body = parse();
	GPerlCell *func = blocks->lastNode();
	func->body = body;
	ast->add(func);
	blocks->clearNodes();
	flags->funcFlag = false;
}

void GPerlParser::parseIfStmt(GPerlToken *, GPerlNodes *blocks, GPerlFlags *flags, GPerlCell *root)
{
	GPerlCell *cond = blocks->lastNode();
	if (cond->type != IsNot &&
		(cond->type == Var || cond->type == ArrayVar ||
		 cond->type == HashVar ||
		 cond->type == ArgumentArray || cond->type == Int ||
		 cond->type == Double || cond->type == String ||
		 cond->type == Call || cond->type == BuiltinFunc || cond->type == CodeVar)) {
		//isOperation
		GPerlCell *is = new GPerlCell(Is, "defined");
		GPerlCell *tmp = cond;
		tmp->parent = is;
		cond = is;
		cond->left = tmp;
	}
	blocks->popNode();
	root->cond = cond;
	cond->parent = root;
	MOVE_NEXT_TOKEN();
	DBG_PL("-----------ifstmt------------");
	GPerlScope *scope = parse();
	root->true_stmt = scope;
	flags->ifStmtFlag = false;
}

void GPerlParser::parseElseIfStmt(GPerlToken *, GPerlNodes *blocks, GPerlFlags *flags, GPerlCell *root)
{
	GPerlCell *cond = blocks->lastNode();
	if (cond->type != IsNot &&
		(cond->type == Var || cond->type == ArrayVar ||
		 cond->type == HashVar ||
		 cond->type == ArgumentArray || cond->type == Int ||
		 cond->type == Double || cond->type == String ||
		 cond->type == Call || cond->type == BuiltinFunc || cond->type == CodeVar)) {
		//isOperation
		GPerlCell *is = new GPerlCell(Is, "defined");
		GPerlCell *tmp = cond;
		tmp->parent = is;
		cond = is;
		cond->left = tmp;
	}
	blocks->popNode();
	root->cond = cond;
	cond->parent = root;
	MOVE_NEXT_TOKEN();
	DBG_PL("-----------elsifstmt------------");
	GPerlScope *scope = parse();
	root->true_stmt = scope;
	flags->elsifStmtFlag = false;
}

void GPerlParser::parseElseStmt(GPerlToken *, GPerlNodes *, GPerlFlags *flags, GPerlCell *root)
{
	MOVE_NEXT_TOKEN();
	DBG_PL("-----------elsestmt------------");
	GPerlScope *scope = parse();
	root->false_stmt = scope;
	flags->elseStmtFlag = false;
}

void GPerlParser::parseWhileStmt(GPerlToken *, GPerlNodes *blocks, GPerlFlags *flags, GPerlCell *root)
{
	GPerlCell *cond = blocks->lastNode();
	blocks->popNode();
	root->cond = cond;
	cond->parent = root;
	MOVE_NEXT_TOKEN();
	DBG_PL("-----------whilestmt------------");
	GPerlScope *scope = parse();
	root->true_stmt = scope;
	flags->whileStmtFlag = false;
}

void GPerlParser::parseForStmt(GPerlToken *t, GPerlNodes *blocks, GPerlFlags *flags, GPerlCell *root)
{
	GPerlCell *cond = blocks->lastNode();
	if (!cond->next) {
		flags->forStmtFlag = false;
		root->type = ForeachStmt;
		parseForeachStmt(t, blocks, flags, root);
		return;
	}
	blocks->popNode();
	root->cond = cond;
	cond->parent = root;
	MOVE_NEXT_TOKEN();
	DBG_PL("-----------forstmt------------");
	GPerlScope *scope = parse();
	root->true_stmt = scope;
	flags->forStmtFlag = false;
}

void GPerlParser::parseForeachStmt(GPerlToken *, GPerlNodes *blocks, GPerlFlags *flags, GPerlCell *root)
{
	GPerlCell *cond = blocks->lastNode();
	cond->indent++;
	cond->setVariableIdx(vidx);
	vcount++;
	vidx++;
	if (cond->left) {
		cond->left->setVariableIdx(vidx);
		cond->left->indent++;
		vcount++;
		vidx++;
	} else {
		GPerlCell *v = new GPerlCell(LocalVarDecl, "$_");
		v->indent++;
		v->setVariableIdx(vidx);
		vcount++;
		vidx++;
		GPerlCell *tmp = cond;
		cond = v;
		cond->left = tmp;
	}
	blocks->popNode();
	root->right = cond;
	cond->parent = root;
	MOVE_NEXT_TOKEN();
	DBG_PL("-----------foreachstmt------------");
	GPerlScope *scope = parse();
	root->true_stmt = scope;
	flags->foreachStmtFlag = false;
}

void GPerlParser::parseBlock(GPerlToken *t, GPerlNodes *blocks, GPerlFlags *, GPerlCell *, GPerlAST *ast)
{
	MOVE_NEXT_TOKEN();
	DBG_PL("-----------BlockScope------------");
	GPerlScope *scope = parse();
	GPerlCell *node = scope->root;
	if (scope && !node->next &&
		(node->type == String || node->type == Key)) {
		DBG_PL("HASH_AT:");
		GPerlCell *block = blocks->lastNode();
		GPerlCell *key = scope->root;
		key->indent = indent;
		GPerlCell *b = new GPerlCell(HashAt, "{}");
		b->indent = indent;
		if (block->right) {
			GPerlCell *right = block->right;
			right->parent = b;
			key->parent = b;
			b->left = right;
			b->right = key;
			block->right = b;
			return;
		}
		block->parent = b;
		key->parent = b;
		b->left = block;
		b->right = key;
		blocks->popNode();
		//blocks.swapLastNode(b);
		GPerlScope *scope = new GPerlScope();
		scope->add(b);
		parseValue(t, blocks, scope);
	} else if (node->argsize > 0) {
		//HashReference
		parseValue(t, blocks, scope);
	} else {
		for (; node; node = node->next) {
			ast->add(node);
		}
	}
}

void GPerlParser::parseLeftBrace(GPerlToken *t, GPerlNodes *blocks, GPerlFlags *flags, GPerlCell *root, GPerlAST *ast)
{
	DBG_PL("LEFT BRACE:");
	vcount = 0;
	indent++;
	if (flags->funcFlag) {
		parseFunc(t, blocks, flags, root, ast);
	} else if (flags->ifStmtFlag) {
		parseIfStmt(t, blocks, flags, root);
	} else if (flags->elsifStmtFlag) {
		parseElseIfStmt(t, blocks, flags, root);
	} else if (flags->elseStmtFlag) {
		parseElseStmt(t, blocks, flags, root);
	} else if (flags->whileStmtFlag) {
		parseWhileStmt(t, blocks, flags, root);
	} else if (flags->forStmtFlag) {
		parseForStmt(t, blocks, flags, root);
	} else if (flags->foreachStmtFlag) {
		parseForeachStmt(t, blocks, flags, root);
	} else {
		parseBlock(t, blocks, flags, root, ast);
	}
}

void GPerlParser::parseRightBrace(GPerlToken *, GPerlNodes *blocks, GPerlFlags *, GPerlCell *root, GPerlAST *ast)
{
	DBG_PL("RIGHT BRACE:");
	if (root->argsize > 0) {
		//HashReference
		GPerlCell *stmt = blocks->at(0);
		GPerlCell *v = root;
		v->vargs[v->argsize] = stmt;
		v->argsize++;
		ast->add(root);
	} else if (ast->size == 0 && blocks->block_num == 1) {
		//for {Array/Hash}Dereference
		ast->add(blocks->lastNode());
	} else {
		vidx -= vcount;
		DBG_PL("vidx = [%d]\n", vidx);
		indent--;
	}
}

void GPerlParser::parseLeftParenthesis(GPerlToken *t, GPerlNodes *blocks, GPerlFlags *flags, GPerlT prev_type)
{
	if (prev_type != BuiltinFunc && prev_type != Call &&
		prev_type != CodeVar) {
		flags->condIndentFlag = true;
		vcount = 0;
		indent++;
	}
	MOVE_NEXT_TOKEN();
	GPerlScope *scope = parse();
	if (flags->condIndentFlag) {
		//vidx -= vcount;
		indent--;
		flags->condIndentFlag = false;
	}
	parseValue(t, blocks, scope);
	if (flags->isVarDeclFlag) {
		DBG_PL("MultiVarDecl");
		DBG_PL("vidx = [%d]", vidx);
		DBG_PL("indent = [%d]", indent);
		if (indent > 0) {
			scope->root->type = MultiLocalVarDecl;
			for (int i = 0; i < scope->root->argsize; i++) {
				GPerlCell *var = scope->root->vargs[i];
				var->type = LocalVarDecl;
				var->indent = indent;
				var->setVariableIdx(vidx);
				vcount++;
				vidx++;
			}
			flags->isVarDeclFlag = false;
		} else {
			DBG_PL("gidx = [%d]", gidx);
			scope->root->type = MultiGlobalVarDecl;
			for (int i = 0; i < scope->root->argsize; i++) {
				GPerlCell *gvar = scope->root->vargs[i];
				gvar->type = GlobalVarDecl;
				gvar->indent = indent;
				gvar->setVariableIdx(gidx);
				gidx++;
			}
			flags->isVarDeclFlag = false;
		}
	}
}

void GPerlParser::parseRightParenthesis(GPerlToken *, GPerlNodes *blocks, GPerlFlags *, GPerlCell *root, GPerlAST *ast)
{
	if (blocks->block_num > 1) {
		DBG_PL("[)]:CONNECT BLOCK <=> BLOCK");
		GPerlCell *to = blocks->lastNode();
		blocks->popNode();
		GPerlCell *from = blocks->lastNode();
		from->right = to;
		to->parent = from;
	}
	int size = blocks->size();
	DBG_PL("BLOCKS SIZE = [%d]", size);
	if (size == 1) {
		GPerlCell *stmt = blocks->at(0);
		if (root->argsize > 0) {
			GPerlCell *v = root;
			v->vargs[v->argsize] = stmt;
			v->argsize++;
		} else {
			root = stmt;
		}
	}
	ast->add(root);
}

void GPerlParser::parseLeftBracket(GPerlToken *t, GPerlNodes *blocks, GPerlFlags *)
{
	MOVE_NEXT_TOKEN();
	GPerlScope *scope = parse();
	GPerlCell *block = blocks->lastNode();
	if (/*(block->type == Var || block->type == GlobalVar ||
		  block->type == GlobalVarDecl) &&*/
		scope && scope->size == 1 && scope->root->argsize == 0) {
		DBG_PL("ARRAY_AT:");
		GPerlCell *idx = scope->root;
		idx->indent = indent;
		GPerlCell *b = new GPerlCell(ArrayAt, "[]");
		b->indent = indent;
		if (block->right) {
			GPerlCell *right = block->right;
			right->parent = b;
			idx->parent = b;
			b->left = right;
			b->right = idx;
			block->right = b;
			return;
		}
		block->parent = b;
		idx->parent = b;
		b->left = block;
		b->right = idx;
		blocks->popNode();
		//blocks.swapLastNode(b);
		GPerlScope *scope = new GPerlScope();
		scope->add(b);
		parseValue(t, blocks, scope);
	} else {
		//ArrayRef
		parseValue(t, blocks, scope);
	}
}

void GPerlParser::parseDereference(GPerlToken *t, GPerlNodes *blocks, GPerlFlags *)
{
	MOVE_NEXT_TOKEN();
	indent++;
	DBG_PL("-----------(Array/Hash)Dereference------------");
	GPerlScope *scope = parse();
	GPerlCell *block = scope->root;
	DBG_PL("[%s]:LAST BLOCK->PARENT", cstr(t->data));
	GPerlCell *b = new GPerlCell(t->info.type, t->data);
	b->indent = indent;
	block->parent = b;
	b->left = block;
	blocks->pushNode(b);
}

void GPerlParser::parseTerm(GPerlToken *t, GPerlNodes *blocks, GPerlFlags *flags, GPerlCell *root, GPerlAST *ast)
{
	GPerlScope *pkg = NULL;
	if (flags->isCallDeclFlag && t->info.type != Call && t->info.type != BuiltinFunc) {
		t->info.type = CodeVar;
		flags->isCallDeclFlag = false;
	} else if (flags->packageFlag) {
		root = new GPerlCell(Package, t->data);
		MOVE_NEXT_TOKEN();
		pkg = parse();
		pkg->root = pkg->root->next;
		root->pkg_stmt = pkg;
		pkgs->insert(pkgs->begin(), 1, root);
		flags->packageFlag = false;
		return;
	} else if (flags->annotateFlag) {
		root = new GPerlCell(Annotation, t->data);
		ast->add(root);
		flags->annotateFlag = false;
		return;
	}
	parseValue(t, blocks, NULL);
}

void GPerlParser::parseSemiColon(GPerlToken *, GPerlNodes *blocks, GPerlFlags *, GPerlCell *root, GPerlAST *ast)
{
	int size = blocks->size();
	DBG_PL("BLOCKS SIZE = [%d]", size);
	if (size == 1) {
		GPerlCell *stmt = blocks->at(0);
		if (root->type == BuiltinFunc || root->type == Call || root->type == CodeVar) {
			GPerlCell *v = root;
			v->vargs[v->argsize] = stmt;
			v->argsize++;
		} else {
			root = stmt;
		}
	} else if (size > 1) {
		GPerlCell *left = blocks->at(0); /* [=] or [return] */
		GPerlCell *right = blocks->at(1);
		if (left->left) {
			left->right = right;
			right->parent = left;
		} else {
			left->left = right;
			right->parent = left;
		}
		root = left;
	}
	ast->add(root);
	blocks->clearNodes();
	if (it != end) {
		root = new GPerlCell(Return);
	}
}

void GPerlParser::parseShift(GPerlToken *t, GPerlNodes *blocks, GPerlFlags *)
{
	if (blocks->block_num > 0 && blocks->lastNode()->type == Assign) {
		GPerlCell *assign = blocks->lastNode();
		if (assign->right == NULL) {
			DBG_PL("Shift:LAST BLOCK->right");
			GPerlCell *shift = new GPerlCell(Shift, t->data);
			assign->right = shift;
		} else {
			fprintf(stderr, "ERROR:[parse error]!!\n");
		}
	} else {
		fprintf(stderr, "Warning: unused keyword shift\n");
	}
}

void GPerlParser::parseArgument(GPerlToken *, GPerlNodes *blocks, GPerlFlags *)
{
	DBG_PL("ProgramArgument");
	GPerlCell *args = new GPerlCell(List);
	for (int i = 0; i < argc; i++) {
		GPerlCell *v = new GPerlCell(String, argv[i]);
		v->indent = indent;
		args->vargs[args->argsize] = v;
		args->argsize++;
	}
	args->indent = indent;
	blocks->pushNode(args);
}

void GPerlParser::parseComma(GPerlToken *, GPerlNodes *blocks, GPerlFlags *, GPerlCell *root)
{
	DBG_PL("VARGS[] = STMT & CLEAR BLOCKS");
	GPerlCell *stmt = blocks->at(0);
	GPerlCell *v = root;
	v->vargs[v->argsize] = stmt;
	v->argsize++;
	blocks->clearNodes();
}

GPerlAST *GPerlParser::parse(void)
{
	GPerlAST *ast = new GPerlAST();
	GPerlCell *root = new GPerlCell(Return);
	GPerlNodes blocks;
	GPerlFlags flags;
	GPerlT prev_type = Undefined;
	while (it != end) {
		GPerlToken *t = (GPerlToken *)*it;
		if (!t) break;
		DBG_P("L[%d] : ", iterate_count);
		DBG_PL("(%s)", t->info.name);
		switch (t->info.type) {
		case LocalVar: case LocalArrayVar: case LocalHashVar:
			parseLocalVar(t, &blocks, &flags);
			break;
		case GlobalVar: case GlobalArrayVar: case GlobalHashVar:
			parseGlobalVar(t, &blocks, &flags);
			break;
		case Var: case ArrayVar: case HashVar: case ArgumentArray:
		case SpecificValue: case Int: case Double: case String:
		case Call: case BuiltinFunc:
		case Key: case CodeVar: case Class:
			parseTerm(t, &blocks, &flags, root, ast);
			break;
		case Shift:
			parseShift(t, &blocks, &flags);
			break;
		case Add: case Sub: case Mul: case Div:
		case Greater: case Less: case GreaterEqual: case LessEqual:
		case EqualEqual: case NotEqual: case Inc: case Dec:
		case LeftShift: case RightShift:
		case StringAdd: case Arrow: case Pointer:
			parseDoubleTermOperator(t, &blocks);
			break;
		case IsNot: case CodeRef:
			parseSingleTermOperator(t, &blocks);
			break;
		case Assign: case AddEqual: case SubEqual: case MulEqual: case DivEqual:
			root = parseAssign(t, &blocks);
			break;
		case IfStmt:
			root = new GPerlCell(IfStmt, t->data);
			ast->add(root);
			flags.ifStmtFlag = true;
			break;
		case ElsifStmt: {
			if (root->type != IfStmt && root->type != ElsifStmt) {
				fprintf(stderr, "Parse Error!! near by elsif\n");
				exit(EXIT_FAILURE);
			}
			GPerlScope *elsif_stmt = new GPerlScope();
			elsif_stmt->root = new GPerlCell(ElsifStmt, t->data);
			root->false_stmt = elsif_stmt;
			root = root->false_stmt->root;
			flags.elsifStmtFlag = true;
			break;
		}
		case VarDecl:
			flags.isVarDeclFlag = true;
			break;
		case CallDecl:
			flags.isCallDeclFlag = true;
			break;
		case UnlessStmt:
			flags.unlessStmtFlag = true;
			break;
		case ElseStmt:
			flags.elseStmtFlag = true;
			break;
		case Package:
			flags.packageFlag = true;
			break;
		case Annotation:
			flags.annotateFlag = true;
			break;
		case WhileStmt:
			root = new GPerlCell(WhileStmt, t->data);
			ast->add(root);
			flags.whileStmtFlag = true;
			break;
		case ForStmt:
			root = new GPerlCell(ForStmt, t->data);
			ast->add(root);
			flags.forStmtFlag = true;
			break;
		case ForeachStmt:
			root = new GPerlCell(ForeachStmt, t->data);
			ast->add(root);
			flags.foreachStmtFlag = true;
			break;
		case Function:
			vidx = 0;
			blocks.pushNode(new GPerlCell(Function, t->data));
			flags.funcFlag = true;
			break;
		case Return:
			blocks.pushNode(new GPerlCell(Return, t->data));
			break;
		case LeftBrace:
			parseLeftBrace(t, &blocks, &flags, root, ast);
			break;
		case ArrayDereference: case HashDereference:
			parseDereference(t, &blocks, &flags);
			break;
		case RightBrace:
			parseRightBrace(t, &blocks, &flags, root, ast);
			return ast;
		case LeftParenthesis:
			parseLeftParenthesis(t, &blocks, &flags, prev_type);
			break;
		case RightParenthesis: case RightBracket:
			parseRightParenthesis(t, &blocks, &flags, root, ast);
			return ast;
		case LeftBracket:
			parseLeftBracket(t, &blocks, &flags);
			break;
		case ProgramArgument:
			parseArgument(t, &blocks, &flags);
			break;
		case Comma:
			parseComma(t, &blocks, &flags, root);
			break;
		case SemiColon:
			parseSemiColon(t, &blocks, &flags, root, ast);
			break;
		default:
			break;
		}
		prev_type = t->info.type;
		MOVE_NEXT_TOKEN();
	}
	DBG_PL("ast size = [%d]", ast->size);
	DBG_PL("=====================================");
	return ast;
}
