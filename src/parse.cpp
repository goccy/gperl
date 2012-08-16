#include <gperl.hpp>

using namespace std;

GPerlCell::GPerlCell(GPerlT type_) : type(type_), vidx(0)
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

GPerlCell::GPerlCell(GPerlT type_, string name) : type(type_), vidx(0)
{
	data.pdata = NULL;
	left = NULL;
	true_stmt = NULL;
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
	} else if (type == String) {
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

GPerlParser::GPerlParser(vector<GPerlToken *> *tokens, int _argc, char **_argv) : argc(_argc), argv(_argv)
{
	it = tokens->begin();
	end = tokens->end();
	iterate_count = 0;
	func_iterate_count = 0;
	vidx = 0;
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

void GPerlParser::parseValue(GPerlToken *t, GPerlNodes *blocks, GPerlScope *scope)
{
	GPerlT type = t->info.type;
	GPerlCell *block = (blocks->block_num > 0) ? blocks->lastNode() : NULL;
	if (block && block->type != Assign && block->type != Return &&
		block->type != AddEqual && block->type != SubEqual &&
		block->type != MulEqual && block->type != DivEqual) {
		DBG_PL("%s", block->info.name);
		if (block->type == Call || block->type == BuiltinFunc) {
			DBG_PL("[%s]:NEW BLOCK->BLOCKS", cstr(t->data));
			if (scope && scope->root->argsize > 0) {
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
			} else {
				GPerlCell *v = new GPerlCell(type, t->data);
				v->indent = indent;
				blocks->pushNode(v);
			}
		} else if (block->left == NULL) {
			DBG_PL("[%s]:LAST BLOCK->left", cstr(t->data));
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
			if (type == Call || type == BuiltinFunc) blocks->pushNode(b);
		} else if (block->right == NULL) {
			DBG_PL("[%s]:LAST BLOCK->right", cstr(t->data));
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
			if (type == Call || type == BuiltinFunc) blocks->pushNode(b);
		} else {
			fprintf(stderr, "ERROR:[parse error]!!\n");
		}
	} else {
		//first var
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
		} else if (scope && scope->size > 1) {
			DBG_PL("cond @forstmt");
			GPerlCell *b = scope->root;
			b->indent = indent;
			blocks->pushNode(b);
		} else if (scope && scope->root->argsize == 0) {
			GPerlCell *b = scope->root;
			b->indent = indent;
			blocks->pushNode(b);
		} else if (scope && (scope->root->type == BuiltinFunc || scope->root->type == Call)) {
			GPerlCell *b = scope->root;
			b->indent = indent;
			blocks->pushNode(b);
		} else {
			DBG_PL("[%s]:NEW BLOCK->BLOCKS", cstr(t->data));
			GPerlCell *b = (scope) ? scope->root->vargs[0] : new GPerlCell(type, t->data);
			b->indent = indent;
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
	bool unlessStmtFlag = false;
	bool elseStmtFlag = false;
	bool elsifStmtFlag = false;
	bool funcFlag = false;
	bool whileStmtFlag = false;
	bool forStmtFlag = false;
	bool condIndentFlag = false;
	GPerlT prev_type = Undefined;

	while (it != end) {
		GPerlToken *t = (GPerlToken *)*it;
		DBG_P("L[%d] : ", iterate_count);
		DBG_PL("(%s)", t->info.name);
		switch (t->info.type) {
		case VarDecl:
			isVarDeclFlag = true;
			break;
		case LocalVar: case LocalArrayVar:
			if (isVarDeclFlag) {
				DBG_PL("[%s]:NEW BLOCK => BLOCKS", cstr(t->data));
				DBG_PL("vidx = [%d]", vidx);
				string prefix = "";
				for (int i = 0; i < indent; i++) {
					prefix += string(NAME_RESOLUTION_PREFIX);
				}
				//DBG_PL("prefix = [%s]", cstr(prefix));
				GPerlCell *var = new GPerlCell(LocalVarDecl, prefix + t->data);
				var->indent = indent;
				var->setVariableIdx(vidx);
				blocks.pushNode(var);
				isVarDeclFlag = false;
				vcount++;
				vidx++;
			} else {
				fprintf(stderr, "ERROR:syntax error!!\n");
			}
			break;
		case GlobalVar: case GlobalArrayVar: {
			DBG_PL("[%s]:NEW BLOCK => BLOCKS", cstr(t->data));
			DBG_PL("vidx = [%d]", vidx);
			string prefix = "";
			for (int i = 0; i < indent; i++) {
				prefix += string(NAME_RESOLUTION_PREFIX);
			}
			//DBG_PL("prefix = [%s]", cstr(prefix));
			GPerlCell *gvar = new GPerlCell(GlobalVarDecl, prefix + t->data);
			DBG_PL("INDENT = [%d]", indent);
			gvar->indent = indent;
			gvar->setVariableIdx(vidx);
			blocks.pushNode(gvar);
			isVarDeclFlag = false;
			vcount++;
			vidx++;
			break;
		}
		case Var: case ArrayVar: case ArgumentArray:
		case Int: case Double: case String: case Call: case BuiltinFunc: {
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
		case Add: case Sub: case Mul: case Div: case Greater: case Less: case StringAdd:
		case GreaterEqual: case LessEqual: case EqualEqual: case NotEqual: case Inc:
		case LeftShift: case RightShift: {
			DBG_PL("[%s]:LAST BLOCK->PARENT", cstr(t->data));
			GPerlCell *block = blocks.lastNode();
			GPerlCell *b = new GPerlCell(t->info.type, t->data);
			block->parent = b;
			b->left = block;
			blocks.swapLastNode(b);
			break;
		}
		case IsNot: {
			DBG_PL("[%s]:LAST BLOCK->PARENT", cstr(t->data));
			GPerlCell *b = new GPerlCell(t->info.type, t->data);
			blocks.pushNode(b);
			break;
		}
		case Assign: case AddEqual: case SubEqual: case MulEqual: case DivEqual: {
			DBG_PL("ASSIGN:LAST BLOCK->PARENT");
			GPerlCell *block = NULL;
			if (blocks.block_num > 0) {
				block = blocks.lastNode();
				blocks.popNode();
			} else {
				fprintf(stderr, "ERROR:syntax error!!\n");
			}
			GPerlCell *assign = new GPerlCell(t->info.type, t->data);
			assign->indent = indent;
			assign->vname = block->vname;
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
		case ElsifStmt: {
			if (root->type != IfStmt && root->type != ElsifStmt) {
				fprintf(stderr, "Parse Error!! near by elsif\n");
				exit(EXIT_FAILURE);
			}
			GPerlScope *elsif_stmt = new GPerlScope();
			elsif_stmt->root = new GPerlCell(ElsifStmt, t->data);
			root->false_stmt = elsif_stmt;
			root = root->false_stmt->root;
			elsifStmtFlag = true;
			break;
		}
		case UnlessStmt: {
			unlessStmtFlag = true;
			break;
		}
		case WhileStmt: {
			root = new GPerlCell(WhileStmt, t->data);
			ast->add(root);
			whileStmtFlag = true;
			break;
		}
		case ForStmt: {
			root = new GPerlCell(ForStmt, t->data);
			ast->add(root);
			forStmtFlag = true;
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
			vcount = 0;
			indent++;
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
				if (cond->type != IsNot &&
					(cond->type == Var || cond->type == ArrayVar ||
					 cond->type == ArgumentArray || cond->type == Int ||
					 cond->type == Double || cond->type == String ||
					 cond->type == Call || cond->type == BuiltinFunc)) {
					//isOperation
					GPerlCell *is = new GPerlCell(Is, "defined");
					GPerlCell *tmp = cond;
					tmp->parent = is;
					cond = is;
					cond->left = tmp;
				}
				blocks.popNode();
				root->cond = cond;
				cond->parent = root;
				MOVE_NEXT_TOKEN();
				DBG_PL("-----------ifstmt------------");
				GPerlScope *scope = parse();
				root->true_stmt = scope;
				ifStmtFlag = false;
			} else if (elsifStmtFlag) {
				GPerlCell *cond = blocks.lastNode();
				if (cond->type != IsNot &&
					(cond->type == Var || cond->type == ArrayVar ||
					 cond->type == ArgumentArray || cond->type == Int ||
					 cond->type == Double || cond->type == String ||
					 cond->type == Call || cond->type == BuiltinFunc)) {
					//isOperation
					GPerlCell *is = new GPerlCell(Is, "defined");
					GPerlCell *tmp = cond;
					tmp->parent = is;
					cond = is;
					cond->left = tmp;
				}
				blocks.popNode();
				root->cond = cond;
				cond->parent = root;
				MOVE_NEXT_TOKEN();
				DBG_PL("-----------elsifstmt------------");
				GPerlScope *scope = parse();
				root->true_stmt = scope;
				elsifStmtFlag = false;
			} else if (elseStmtFlag) {
				MOVE_NEXT_TOKEN();
				DBG_PL("-----------elsestmt------------");
				GPerlScope *scope = parse();
				root->false_stmt = scope;
				elseStmtFlag = false;
			} else if (whileStmtFlag) {
				GPerlCell *cond = blocks.lastNode();
				blocks.popNode();
				root->cond = cond;
				cond->parent = root;
				MOVE_NEXT_TOKEN();
				DBG_PL("-----------whilestmt------------");
				GPerlScope *scope = parse();
				root->true_stmt = scope;
				whileStmtFlag = false;
			} else if (forStmtFlag) {
				GPerlCell *cond = blocks.lastNode();
				blocks.popNode();
				root->cond = cond;
				cond->parent = root;
				MOVE_NEXT_TOKEN();
				DBG_PL("-----------forstmt------------");
				GPerlScope *scope = parse();
				root->true_stmt = scope;
				forStmtFlag = false;
			} else {
				//Block
				MOVE_NEXT_TOKEN();
				DBG_PL("-----------BlockScope------------");
				GPerlScope *scope = parse();
				GPerlCell *node = scope->root;
				for (; node; node = node->next) {
					ast->add(node);
				}
			}
			break;
		}
		case ArrayDereference: {
			MOVE_NEXT_TOKEN();
			indent++;
			DBG_PL("-----------ArrayDereference------------");
			GPerlScope *scope = parse();
			GPerlCell *block = scope->root;
			DBG_PL("[%s]:LAST BLOCK->PARENT", cstr(t->data));
			GPerlCell *b = new GPerlCell(t->info.type, t->data);
			b->indent = indent;
			block->parent = b;
			b->left = block;
			blocks.pushNode(b);
			break;
		}
		case RightBrace: {
			DBG_PL("RIGHT BRACE:");
			vidx -= vcount;
			DBG_PL("vidx = [%d]\n", vidx);
			indent--;
			if (ast->size == 0 && blocks.block_num == 1) {
				//for {Array/Hash}Dereference
				ast->add(blocks.lastNode());
			}
			return ast;
		}
		case LeftParenthesis: {
			if (prev_type != BuiltinFunc && prev_type != Call) {
				condIndentFlag = true;
				vcount = 0;
				indent++;
			}
			MOVE_NEXT_TOKEN();
			GPerlScope *scope = parse();
			if (condIndentFlag) {
				//vidx -= vcount;
				indent--;
				condIndentFlag = false;
			}
			parseValue(t, &blocks, scope);
			break;
		}
		case RightParenthesis: case RightBracket: {
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
				if (root->argsize > 0) {
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
		case LeftBracket: {
			MOVE_NEXT_TOKEN();
			GPerlScope *scope = parse();
			if (scope && scope->size == 1 && scope->root->type == Int) {
				DBG_PL("ARRAY_AT:");
				GPerlCell *idx = scope->root;
				idx->indent = indent;
				GPerlCell *b = new GPerlCell(ArrayAt, "[]");
				b->indent = indent;
				GPerlCell *block = blocks.lastNode();
				if (block->right) {
					GPerlCell *right = block->right;
					right->parent = b;
					idx->parent = b;
					b->left = right;
					b->right = idx;
					block->right = b;
					break;
				}
				block->parent = b;
				idx->parent = b;
				b->left = block;
				b->right = idx;
				blocks.swapLastNode(b);
			} else {
				parseValue(t, &blocks, scope);
			}
			break;
		}
		case ProgramArgument: {
			DBG_PL("ProgramArgument");
			GPerlCell *args = new GPerlCell(List);
			for (int i = 0; i < argc; i++) {
				GPerlCell *v = new GPerlCell(String, argv[i]);
				v->indent = indent;
				args->vargs[args->argsize] = v;
				args->argsize++;
			}
			args->indent = indent;
			blocks.pushNode(args);
			break;
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
		prev_type = t->info.type;
		MOVE_NEXT_TOKEN();
	}
	DBG_PL("ast size = [%d]", ast->size);
	DBG_PL("=====================================");
	return ast;
}
