#include <gperl.hpp>

using namespace std;

GPerlCell::GPerlCell(GPerlT type_) : type(type_)
{
	data.pdata = NULL;
	left = NULL;
	true_stmt = NULL;
	right = NULL;
	parent = NULL;
	next = NULL;
	for (int i = 0; i < MAX_ARGSTACK_SIZE; i++) {
		vargs[i] = NULL;
	}
	argsize = 0;
}

GPerlCell::GPerlCell(GPerlT type_, string name) : type(type_)
{
	data.pdata = NULL;
	left = NULL;
	true_stmt = NULL;
	right = NULL;
	parent = NULL;
	next = NULL;
	for (int i = 0; i < MAX_ARGSTACK_SIZE; i++) {
		vargs[i] = NULL;
	}
	argsize = 0;
	if (type_ == Int) {
		data.idata = atoi(cstr(name));
	} else if (type == String) {
		data.sdata = (char *)cstr(name);
	} else {
		vname = name;
		fname = name;
	}
	rawstr = name;
}

GPerlParser::GPerlParser(vector<Token *> *tokens)
{
	it = tokens->begin();
	end = tokens->end();
	iterate_count = 0;
	func_iterate_count = 0;
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

void GPerlParser::parseValue(Token *t, GPerlNodes *blocks, GPerlScope *scope)
{
	GPerlT type = t->type;
	GPerlCell *block = (blocks->block_num > 0) ? blocks->lastNode() : NULL;
	if (block && block->type != Assign && block->type != Return) {
		DBG_PL("%s", TypeName(block->type));
		if (block->type == Call || block->type == BuiltinFunc) {
			DBG_PL("[%s]:NEW BLOCK->BLOCKS", cstr(t->data));
			if (scope) {
				for (int i = 0; i < scope->root->argsize; i++) {
					block->vargs[i] = scope->root->vargs[i];
				}
				block->argsize = scope->root->argsize;
			} else {
				blocks->pushNode(new GPerlCell(type, t->data));
			}
		} else if (block->left == NULL) {
			DBG_PL("[%s]:LAST BLOCK->left", cstr(t->data));
			GPerlCell *b = (scope) ? scope->root->vargs[0] : new GPerlCell(type, t->data);
			block->left = b;
			b->parent = block;
			if (type == Call || type == BuiltinFunc) blocks->pushNode(b);
		} else if (block->right == NULL) {
			DBG_PL("[%s]:LAST BLOCK->right", cstr(t->data));
			GPerlCell *b = (scope) ? scope->root->vargs[0] : new GPerlCell(type, t->data);
			block->right = b;
			b->parent = block;
			if (type == Call || type == BuiltinFunc) blocks->pushNode(b);
		} else {
			fprintf(stderr, "ERROR:[parse error]!!\n");
		}
	} else {
		//first var
		if (scope && scope->root->argsize > 1) {
			DBG_PL("List");
			scope->root->type = List;
			blocks->pushNode(scope->root);
		} else {
			DBG_PL("[%s]:NEW BLOCK->BLOCKS", cstr(t->data));
			GPerlCell *b = (scope) ? scope->root->vargs[0] : new GPerlCell(type, t->data);
			blocks->pushNode(b);
		}
	}
}

GPerlAST *GPerlParser::parse(void)
{
	GPerlAST *ast = new GPerlAST();
	GPerlCell *root = new GPerlCell(Return);
	GPerlNodes blocks;
	bool isVarDeclFlag = false;
	bool ifStmtFlag = false;
	bool elseStmtFlag = false;
	bool funcFlag = false;

	while (it != end) {
		Token *t = (Token *)*it;
		DBG_PL("L[%d] : ", iterate_count);
		DBG_PL("(%s)", TypeName(t->type));
		switch (t->type) {
		case VarDecl:
			isVarDeclFlag = true;
			break;
		case LocalVar: case LocalArrayVar:
			if (isVarDeclFlag) {
				DBG_PL("[%s]:NEW BLOCK => BLOCKS", cstr(t->data));
				blocks.pushNode(new GPerlCell(LocalVarDecl, t->data));
				isVarDeclFlag = false;
			} else {
				fprintf(stderr, "ERROR:syntax error!!\n");
			}
			break;
		case GlobalVar: case GlobalArrayVar: {
			DBG_PL("[%s]:NEW BLOCK => BLOCKS", cstr(t->data));
			blocks.pushNode(new GPerlCell(GlobalVarDecl, t->data));
			isVarDeclFlag = false;
			break;
		}
		case Var: case ArrayVar: case Int: case String:
		case Call: case BuiltinFunc: {
			parseValue(t, &blocks, NULL);
			break;
		}
		case Shift:
			if (blocks.block_num > 0 && blocks.lastNode()->type == Assign) {
				GPerlCell *assign = blocks.lastNode();
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
			break;
		case Add: case Sub: case Mul: case Div: case Greater: case Less:
		case GreaterEqual: case LessEqual: case EqualEqual: case NotEqual: {
			DBG_PL("[%s]:LAST BLOCK->PARENT", cstr(t->data));
			GPerlCell *block = blocks.lastNode();
			GPerlCell *b = new GPerlCell(t->type, t->data);
			block->parent = b;
			b->left = block;
			blocks.swapLastNode(b);
			break;
		}
		case Assign: {
			DBG_PL("ASSIGN:LAST BLOCK->PARENT");
			GPerlCell *block = NULL;
			if (blocks.block_num > 0) {
				block = blocks.lastNode();
				blocks.popNode();
			} else {
				fprintf(stderr, "ERROR:syntax error!!\n");
			}
			GPerlCell *assign = new GPerlCell(Assign, t->data);
			block->parent = assign;
			assign->left = block;
			blocks.pushNode(assign);
			root = assign;
			break;
		}
		case IfStmt: {
			root = new GPerlCell(IfStmt, t->data);
			ast->add(root);
			ifStmtFlag = true;
			break;
		}
		case Function: {
			blocks.pushNode(new GPerlCell(Function, t->data));
			funcFlag = true;
			break;
		}
		case Return: {
			blocks.pushNode(new GPerlCell(Return, t->data));
			break;
		}
		case ElseStmt: {
			elseStmtFlag = true;
			break;
		}
		case LeftBrace: {
			DBG_PL("LEFT BRACE:");
			if (funcFlag) {
				MOVE_NEXT_TOKEN();
				GPerlScope *body = parse();
				GPerlCell *func = blocks.lastNode();
				func->body = body;
				ast->add(func);
				blocks.clearNodes();
				funcFlag = false;
			} else if (ifStmtFlag) {
				GPerlCell *cond = blocks.lastNode();
				blocks.popNode();
				root->cond = cond;
				cond->parent = root;
				MOVE_NEXT_TOKEN();
				DBG_PL("-----------ifstmt------------");
				GPerlScope *scope = parse();
				root->true_stmt = scope;
				ifStmtFlag = false;
			} else if (elseStmtFlag) {
				MOVE_NEXT_TOKEN();
				DBG_PL("-----------elsestmt------------");
				GPerlScope *scope = parse();
				root->false_stmt = scope;
				elseStmtFlag = false;
			}
			break;
		}
		case RightBrace: {
			DBG_PL("RIGHT BRACE:");
			return ast;
		}
		case Comma: {
			DBG_PL("VARGS[] = STMT & CLEAR BLOCKS");
			GPerlCell *stmt = blocks.at(0);
			GPerlCell *v = root;
			v->vargs[v->argsize] = stmt;
			v->argsize++;
			blocks.clearNodes();
			break;
		}
		case LeftParenthesis: {
			MOVE_NEXT_TOKEN();
			GPerlScope *scope = parse();
			parseValue(t, &blocks, scope);
			break;
		}
		case RightParenthesis: {
			if (blocks.block_num > 1) {
				DBG_PL("[)]:CONNECT BLOCK <=> BLOCK");
				GPerlCell *to = blocks.lastNode();
				blocks.popNode();
				GPerlCell *from = blocks.lastNode();
				from->right = to;
				to->parent = from;
			}
			int size = blocks.size();
			DBG_PL("BLOCKS SIZE = [%d]", size);
			if (size == 1) {
				GPerlCell *stmt = blocks.at(0);
				if (root->vargs) {
					GPerlCell *v = root;
					v->vargs[v->argsize] = stmt;
					v->argsize++;
				} else {
					root = stmt;
				}
			}
			ast->add(root);
			return ast;
		}
		case SemiColon: {
			int size = blocks.size();
			DBG_PL("BLOCKS SIZE = [%d]", size);
			if (size == 1) {
				GPerlCell *stmt = blocks.at(0);
				if (root->type == BuiltinFunc || root->type == Call) {
					GPerlCell *v = root;
					v->vargs[v->argsize] = stmt;
					v->argsize++;
				} else {
					root = stmt;
				}
			} else if (size > 1) {
				GPerlCell *left = blocks.at(0); /* [=] or [return] */
				GPerlCell *right = blocks.at(1);
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
			blocks.clearNodes();
			if (it != end) {
				root = new GPerlCell(Return);
			}
			break;
		}
		default:
			break;
		}
		MOVE_NEXT_TOKEN();
	}
	DBG_PL("ast size = [%d]", ast->size);
	DBG_PL("=====================================");
	return ast;
}
