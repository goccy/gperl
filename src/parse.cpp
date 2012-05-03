#include <gperl.hpp>

using namespace std;

GPerlAST::GPerlAST(void) : cluster_num(0)
{
	root = NULL;
	cur = NULL;
	root_node = NULL;
	size = 0;
}

void GPerlAST::add(GPerlCell *root_)
{
	if (root == NULL) {
		root = root_;
		cur = root;
	} else {
		cur->next = root_;
		cur = cur->next;
	}
	size++;
}

#ifdef USING_GRAPH_DEBUG
GraphvizNode *GPerlAST::createNode(GraphvizGraph *graph, const char *name)
{
	GraphvizNode *node = graph->newNode(name);
	node->set("group", "domain");
	node->set("fixedsize", "true");
	node->set("width", "3.0");
	node->set("height", "2.4");
	node->set("style","filled");
	node->set("fontsize", "24");
	return node;
}

void GPerlAST::drawEdge(GraphvizGraph *graph, GraphvizNode *from, GraphvizNode *to, const char *label)
{
	GraphvizEdge *edge = graph->newEdge(from, to);
	edge->set("style","filled");
	edge->set("color","#A9A9A9");
	edge->set("fillcolor","#A9A9A9");
	edge->set("label", label);
	edge->set("fontsize", "24");
}

void GPerlAST::drawStmt(GraphvizGraph *graph, GraphvizNode *if_node,
						GPerlScope *stmt_, const char *stmt_name, const char *color)
{
	char buf[32] = {0};
	snprintf(buf, 32, "cluster%d", cluster_num);
	GraphvizGraph *stmt_graph = graph->makeSubGraph(buf);
	GPerlCell *stmt = stmt_->root;
	const char *stmt_root_name = stmt->rawstr.c_str();
	cluster_num++;
	stmt_graph->set("fillcolor", color);
	stmt_graph->set("style","filled");
	snprintf(buf, 32, "%s stmt: [%p]", stmt_name, stmt_);
	stmt_graph->set("label", buf);
	stmt_graph->set("fontsize", "24");
	snprintf(buf, 32, "%s : [%p]", stmt_root_name, stmt);
	GraphvizNode *stmt_node = createNode(graph, (const char *)buf);
	drawEdge(graph, if_node, stmt_node, stmt_name);
	GraphvizNode *prev_node = NULL;
	for (; stmt; stmt = stmt->next) {
		const char *root_name = stmt->rawstr.c_str();
		snprintf(buf, 32, "%s : [%p]", root_name, stmt);
		GraphvizNode *root_node = createNode(stmt_graph, (const char *)buf);
		this->root_node = root_node;
		if (prev_node) {
			drawEdge(graph, prev_node, root_node, "next");
		}
		draw(stmt_graph, stmt, root_node);
		prev_node = root_node;
	}
}

void GPerlAST::draw(GraphvizGraph *graph, GPerlCell *c, GraphvizNode *node)
{
	GraphvizNode *left;
	GraphvizNode *right;
	char buf[32] = {0};
	if (c->type == IfStmt) {
		GraphvizNode *if_node = root_node;
		drawStmt(graph, if_node, c->true_stmt, "true", "#e0ffff");
		if (c->false_stmt) drawStmt(graph, if_node, c->false_stmt, "false", "#fff0f5");
	} else if (c->type == Function) {
		GraphvizNode *func_node = root_node;
		drawStmt(graph, func_node, c->body, "body", "#e6e6fa");
	} else if (c->type == Call) {
		const char *func_name = c->rawstr.c_str();
		snprintf(buf, 32, "%s : [%p]", func_name, c);
		GraphvizNode *func_node = createNode(graph, (const char *)buf);
		if (c->argsize > 0) {
			for (int i = 0; i < c->argsize; i++) {
				GPerlCell *arg = c->vargs[i];
				const char *to_name = arg->rawstr.c_str();
				snprintf(buf, 32, "%s : [%p]", to_name, arg);
				left = createNode(graph, (const char *)buf);
				drawEdge(graph, func_node, left, "vargs");
				draw(graph, arg, left);
			}
		}
	}
	if (c->left) {
		const char *to_name = c->left->rawstr.c_str();
		snprintf(buf, 32, "%s : [%p]", to_name, c->left);
		left = createNode(graph, (const char *)buf);
		drawEdge(graph, node, left, "left");
		draw(graph, c->left, left);
	}
	if (c->right && c->right->type != Return/*not Scope*/) {
		//DBG_PL("TYPENAME = [%s]", TypeName(c->type));
		const char *to_name = c->right->rawstr.c_str();
		snprintf(buf, 32, "%s : [%p]", to_name, c->right);
		right = createNode(graph, (const char *)buf);
		drawEdge(graph, node, right, "right");
		draw(graph, c->right, right);
	}
}

void GPerlAST::show(void)
{
	GPerlTokenizer t;
	GraphvizContext *ctx = new GraphvizContext();
	GraphvizGraph *graph = new GraphvizGraph("g", AGDIGRAPH);
	graph->set("ranksep", "4.0");
	GPerlCell *stmt = root;
	GraphvizNode *prev_node = NULL;
	for (; stmt; stmt = stmt->next) {
		const char *root_name = stmt->rawstr.c_str();
		char buf[32] = {0};
		snprintf(buf, 32, "%s : [%p]", root_name, stmt);
		GraphvizNode *root_node = createNode(graph, (const char *)buf);
		this->root_node = root_node;
		if (prev_node) {
			drawEdge(graph, prev_node, root_node, "next");
		}
		draw(graph, stmt, root_node);
		prev_node = root_node;
		//fprintf(stderr, "root_name = [%s]\n", root_name);
		//fprintf(stderr, "root->next = [%p]\n", root->next);
		//fprintf(stderr, "root = [%p]\n", root);
	}
	ctx->parseArgs("png", "graph");
	ctx->layoutJobs(graph);
	ctx->renderJobs(graph);
	ctx->freeLayout(graph);
	delete graph;
	delete ctx;
}
#endif

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

#define PUSH_toBLOCK(b) {						\
		blocks.push_back(b);					\
		block_num++;							\
	}

#define POP_fromBLOCK() {						\
		blocks.pop_back();						\
		block_num--;							\
	}

#define SWAP_lastBLOCK(b) {						\
		blocks.pop_back();						\
		blocks.push_back(b);					\
	}

#define CLEAR_BLOCK() {							\
		blocks.clear();							\
		block_num = 0;							\
	}

#define MOVE_NEXT_TOKEN() {						\
		it++;									\
		iterate_count++;						\
	}

#define lastBlock() blocks.at(block_num-1)

GPerlAST *GPerlParser::parse(void)
{
	GPerlAST *ast = new GPerlAST();
	GPerlCell *root = new GPerlCell(Return);
	vector<GPerlCell * > blocks;
	bool isVarDeclFlag = false;
	bool leftParenthesisFlag = false;
	bool ifStmtFlag = false;
	bool elseStmtFlag = false;
	bool funcFlag = false;
	int block_num = 0;
	while (it != end) {
		Token *t = (Token *)*it;
		switch (t->type) {
		case VarDecl:
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("VarDecl");
			isVarDeclFlag = true;
			break;
		case LocalVar:
			DBG_PL("L[%d] : ", iterate_count);
			if (isVarDeclFlag) {
				DBG_PL("LOCALVAR[%s]:NEW BLOCK => BLOCKS", cstr(t->data));
				GPerlCell *block = new GPerlCell(LocalVarDecl, t->data);
				PUSH_toBLOCK(block);
				isVarDeclFlag = false;
			} else {
				fprintf(stderr, "ERROR:syntax error!!\n");
			}
			break;
		case GlobalVar: {
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("GLOBALVAR[%s]:NEW BLOCK => BLOCKS", cstr(t->data));
			GPerlCell *block = new GPerlCell(GlobalVarDecl, t->data);
			PUSH_toBLOCK(block);
			isVarDeclFlag = false;
			break;
		}
		case Var: case Int: case String: case Call: {
			DBG_PL("L[%d] : ", iterate_count);
			GPerlT type = t->type;
			if (block_num > 0 && lastBlock()->type != Assign && lastBlock()->type != Return) {
				GPerlCell *block = lastBlock();
				DBG_PL("%s", TypeName(block->type));
				if (block->type == Call) {
					DBG_PL("%s[%s]:NEW BLOCK->BLOCKS", TypeName(type), cstr(t->data));
					PUSH_toBLOCK(new GPerlCell(type, t->data));
				} else if (block->left == NULL) {
					DBG_PL("%s[%s]:LAST BLOCK->left", TypeName(type), cstr(t->data));
					GPerlCell *b = new GPerlCell(type, t->data);
					block->left = b;
					b->parent = block;
					if (type == Call) {
						PUSH_toBLOCK(b);
					}
				} else if (block->right == NULL) {
					DBG_PL("%s[%s]:LAST BLOCK->right", TypeName(type), cstr(t->data));
					GPerlCell *b = new GPerlCell(type, t->data);
					block->right = b;
					b->parent = block;
					if (type == Call) {
						PUSH_toBLOCK(b);
					}
				} else {
					fprintf(stderr, "ERROR:[parse error]!!\n");
				}
			} else {
				//first var
				DBG_PL("%s[%s]:NEW BLOCK->BLOCKS", TypeName(type), cstr(t->data));
				PUSH_toBLOCK(new GPerlCell(type, t->data));
				if (leftParenthesisFlag) leftParenthesisFlag = false;
			}
			break;
		}
		case Shift:
			DBG_PL("L[%d] : ", iterate_count);
			if (block_num > 0 && lastBlock()->type == Assign) {
				GPerlCell *assign = lastBlock();
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
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("OPERATOR[%s]:LAST BLOCK->PARENT", cstr(t->data));
			GPerlCell *block = lastBlock();
			GPerlCell *b = new GPerlCell(t->type, t->data);
			block->parent = b;
			b->left = block;
			SWAP_lastBLOCK(b);
			break;
		}
		case Assign: {
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("ASSIGN:LAST BLOCK->PARENT");
			GPerlCell *block = NULL;
			if (block_num > 0) {
				block = lastBlock();
				POP_fromBLOCK();
			} else {
				fprintf(stderr, "ERROR:syntax error!!\n");
			}
			GPerlCell *assign = new GPerlCell(Assign, t->data);
			block->parent = assign;
			assign->left = block;
			PUSH_toBLOCK(assign);
			root = assign;
			break;
		}
		case PrintDecl: {
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("PRINT:NEW BLOCK->BLOCKS");
			root = new GPerlCell(PrintDecl, t->data);
			break;
		}
		case IfStmt: {
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("IF:ROOT = IFCELL");
			root = new GPerlCell(IfStmt, t->data);
			ast->add(root);
			ifStmtFlag = true;
			break;
		}
		case Function: {
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("FUNCTION: ");
			root = new GPerlCell(Function, t->data);
			ast->add(root);
			funcFlag = true;
			break;
		}
		case Return: {
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("RETURN: ");
			PUSH_toBLOCK(new GPerlCell(Return, t->data));
			break;
		}
		case ElseStmt: {
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("ELSE: elseStmtFlag => ON");
			elseStmtFlag = true;
			break;
		}
		case LeftBrace: {
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("LEFT BRACE:");
			if (funcFlag) {
				//DBG_PL("FuncFlag: ON");
				MOVE_NEXT_TOKEN();
				GPerlScope *body = parse();
				root->body = body;
				funcFlag = false;
				DBG_PL("ROOT->BODY = BODY");
			} else if (ifStmtFlag) {
				//DBG_PL("IFStmtFlag: ON");
				GPerlCell *cond = lastBlock();
				POP_fromBLOCK();
				root->cond = cond;
				cond->parent = root;
				MOVE_NEXT_TOKEN();
				DBG_PL("-----------ifstmt------------");
				GPerlScope *scope = parse();
				root->true_stmt = scope;
				ifStmtFlag = false;
				//DBG_PL("ROOT->TRUE_STMT = SCOPE");
			} else if (elseStmtFlag) {
				//DBG_PL("ElseStmtFlag: ON");
				MOVE_NEXT_TOKEN();
				DBG_PL("-----------elsestmt------------");
				GPerlScope *scope = parse();
				root->false_stmt = scope;
				elseStmtFlag = false;
			}
			break;
		}
		case RightBrace: {
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("RIGHT BRACE:");
			return ast;
		}
		case Comma: {
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("COMMA:VARGS->BLOCKS & CLEAR BLOCKS");
			GPerlCell *stmt = blocks.at(0);
			GPerlCell *v = root;
			v->vargs[v->argsize] = stmt;
			v->argsize++;
			CLEAR_BLOCK();
			break;
		}
		case LeftParenthesis: {
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("[(]:ON LeftParenthesis Flag");
			leftParenthesisFlag = true;
			MOVE_NEXT_TOKEN();
			GPerlScope *scope = parse();
			if (block_num > 0 && lastBlock()->type == Call) {
				GPerlCell *call = lastBlock();
				for (int i = 0; i < scope->root->argsize; i++) {
					call->vargs[i] = scope->root->vargs[i];
				}
				call->argsize = scope->root->argsize;
			} else {
				PUSH_toBLOCK(scope->root->vargs[0]);
			}
			break;
		}
		case RightParenthesis: {
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("RightParenthesis");
			if (block_num > 1) {
				DBG_PL("[)]:CONNECT BLOCK <=> BLOCK");
				GPerlCell *to = lastBlock();
				POP_fromBLOCK();
				GPerlCell *from = lastBlock();
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
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("SEMICOLON:END AST");
			int size = blocks.size();
			DBG_PL("BLOCKS SIZE = [%d]", size);
			if (size == 1) {
				GPerlCell *stmt = blocks.at(0);
				if (root->type == PrintDecl || root->type == Call) {
					GPerlCell *v = root;
					v->vargs[v->argsize] = stmt;
					v->argsize++;
				} else {
					root = stmt;
				}
			} else if (size >= 2) {
				//Assign Statement
				GPerlCell *left = blocks.at(0); /* [=] or [return] */
				GPerlCell *right = blocks.at(1);
				left->right = right;
				right->parent = left;
				root = left;
			}
			ast->add(root);
			CLEAR_BLOCK();
			DBG_PL("*************************");
			if (it != end) {
				root = new GPerlCell(Return);
			}
			break;
		}
		default:
			DBG_PL("L[%d] : default", iterate_count);
			break;
		}
		MOVE_NEXT_TOKEN();
	}
	DBG_PL("ast size = [%d]", ast->size);
	DBG_PL("=====================================");
	return ast;
}
