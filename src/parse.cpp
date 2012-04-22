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

void GPerlAST::draw(GraphvizGraph *graph, GPerlCell *c, GraphvizNode *node)
{
	GraphvizNode *left;
	GraphvizNode *right;
	char buf[32] = {0};
	if (c->type == IfStmt) {
		snprintf(buf, 32, "cluster%d", cluster_num);
		GraphvizGraph *true_stmt_graph = graph->makeSubGraph(buf);
		cluster_num++;
		true_stmt_graph->set("fillcolor","#e0ffff");
		true_stmt_graph->set("style","filled");
		snprintf(buf, 32, "true stmt: [%p]", c->true_stmt);
		true_stmt_graph->set("label", buf);
		true_stmt_graph->set("fontsize", "24");
		GPerlCell *true_stmt = c->true_stmt->root;
		const char *true_stmt_name = true_stmt->rawstr.c_str();
		snprintf(buf, 32, "%s : [%p]", true_stmt_name, true_stmt_name);
		GraphvizNode *true_stmt_node = createNode(graph, (const char *)buf);
		GraphvizNode *if_node = root_node;
		drawEdge(graph, if_node, true_stmt_node, "true_stmt");
		GraphvizNode *prev_node = NULL;
		for (; true_stmt; true_stmt = true_stmt->next) {
			const char *root_name = true_stmt->rawstr.c_str();
			snprintf(buf, 32, "%s : [%p]", root_name, root_name);
			GraphvizNode *root_node = createNode(true_stmt_graph, (const char *)buf);//root_name);
			this->root_node = root_node;
			if (prev_node) {
				drawEdge(graph, prev_node, root_node, "next");
			}
			draw(true_stmt_graph, true_stmt, root_node);
			prev_node = root_node;
		}
		if (c->false_stmt) {
			prev_node = NULL;
			snprintf(buf, 32, "cluster%d", cluster_num);
			GraphvizGraph *false_stmt_graph = graph->makeSubGraph(buf);
			cluster_num++;
			false_stmt_graph->set("fillcolor","#fff0f5");
			false_stmt_graph->set("style","filled");
			snprintf(buf, 32, "false stmt: [%p]", c->false_stmt);
			false_stmt_graph->set("label", buf);
			false_stmt_graph->set("fontsize", "24");
			GPerlCell *false_stmt = c->false_stmt->root;
			const char *false_stmt_name = false_stmt->rawstr.c_str();
			snprintf(buf, 32, "%s : [%p]", false_stmt_name, false_stmt_name);
			GraphvizNode *false_stmt_node = createNode(graph, (const char *)buf);
			drawEdge(graph, if_node, false_stmt_node, "false_stmt");
			for (; false_stmt; false_stmt = false_stmt->next) {
				const char *root_name = false_stmt->rawstr.c_str();
				snprintf(buf, 32, "%s : [%p]", root_name, root_name);
				GraphvizNode *root_node = createNode(false_stmt_graph, (const char *)buf);//root_name);
				this->root_node = root_node;
				if (prev_node) {
					drawEdge(graph, prev_node, root_node, "next");
				}
				draw(false_stmt_graph, false_stmt, root_node);
				prev_node = root_node;
			}
		}
	}
	if (c->type == Function) {
		DBG_P("Function");
		snprintf(buf, 32, "cluster%d", cluster_num);
		GraphvizGraph *body_graph = graph->makeSubGraph(buf);
		cluster_num++;
		body_graph->set("fillcolor","#e6e6fa");
		body_graph->set("style","filled");
		snprintf(buf, 32, "body: [%p]", c->body);
		body_graph->set("label", buf);
		body_graph->set("fontsize", "24");

		GPerlCell *body_stmt = c->body->root;
		const char *body_stmt_name = body_stmt->rawstr.c_str();
		snprintf(buf, 32, "%s : [%p]", body_stmt_name, body_stmt_name);
		GraphvizNode *body_stmt_node = createNode(graph, (const char *)buf);
		GraphvizNode *func_node = root_node;
		drawEdge(graph, func_node, body_stmt_node, "body");
		GraphvizNode *prev_node = NULL;
		for (; body_stmt; body_stmt = body_stmt->next) {
			const char *root_name = body_stmt->rawstr.c_str();
			snprintf(buf, 32, "%s : [%p]", root_name, root_name);
			GraphvizNode *root_node = createNode(body_graph, (const char *)buf);
			this->root_node = root_node;
			if (prev_node) {
				drawEdge(graph, prev_node, root_node, "next");
			}
			draw(body_graph, body_stmt, root_node);
			prev_node = root_node;
		}
	}
	if (c->vargs) {
		const char *to_name = c->vargs->rawstr.c_str();
		snprintf(buf, 32, "%s : [%p]", to_name, to_name);
		left = createNode(graph, (const char *)buf);
		drawEdge(graph, root_node, left, "vargs");
		draw(graph, c->vargs, left);
	}
	if (c->left != NULL) {
		const char *to_name = c->left->rawstr.c_str();
		snprintf(buf, 32, "%s : [%p]", to_name, to_name);
		left = createNode(graph, (const char *)buf);
		drawEdge(graph, node, left, "left");
	}
	if (c->right != NULL && c->right->type != Return/*not Scope*/) {
		const char *to_name = c->right->rawstr.c_str();
		snprintf(buf, 32, "%s : [%p]", to_name, to_name);
		right = createNode(graph, (const char *)buf);
		drawEdge(graph, node, right, "right");
	}
	if (c->left && c->left->left != NULL) {
		draw(graph, c->left, left);
	}
	if (c->right && c->right->type != Return && c->right->left != NULL) {
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
		snprintf(buf, 32, "%s : [%p]", root_name, root_name);
		GraphvizNode *root_node = createNode(graph, (const char *)buf);//root_name);
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
	vargs = NULL;
}

GPerlCell::GPerlCell(GPerlT type_, string name) : type(type_)
{
	data.pdata = NULL;
	left = NULL;
	true_stmt = NULL;
	right = NULL;
	parent = NULL;
	next = NULL;
	vargs = NULL;
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

GPerlParser::GPerlParser(void)
{
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

GPerlAST *GPerlParser::parse(vector<Token *> *tokens, vector<Token *>::iterator it)
{
	GPerlAST *ast = new GPerlAST();
	GPerlCell *root = new GPerlCell(Return);
	vector<GPerlCell * > blocks;
	bool isVarDeclFlag = false;
	bool leftParenthesisFlag = false;
	bool ifStmtFlag = false;
	bool elseStmtFlag = false;
	bool funcFlag = false;
	bool callFlag = false;
	int block_num = 0;
	while (it != tokens->end()) {
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
		case Var: case Int: case String: {
			DBG_PL("L[%d] : ", iterate_count);
			GPerlT type = t->type;
			if (block_num > 0 && lastBlock()->type != Assign) {
				GPerlCell *block = lastBlock();
				if (block->type == Call) {
					DBG_PL("%s[%s]:NEW BLOCK->BLOCKS", TypeName(type), cstr(t->data));
					PUSH_toBLOCK(new GPerlCell(type, t->data));
				} else if (block->left == NULL) {
					DBG_PL("%s[%s]:LAST BLOCK->left", TypeName(type), cstr(t->data));
					GPerlCell *b = new GPerlCell(type, t->data);
					block->left = b;
					b->parent = block;
				} else if (block->right == NULL) {
					DBG_PL("%s[%s]:LAST BLOCK->right", TypeName(type), cstr(t->data));
					GPerlCell *b = new GPerlCell(type, t->data);
					block->right = b;
					b->parent = block;
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
			GPerlCell *block;
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
			break;
		}
		case PrintDecl: {
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("PRINT:NEW BLOCK->BLOCKS");
			root = new GPerlCell(PrintDecl, t->data);
			break;
		}
		case Call: {
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("CALL:NEW BLOCK->BLOCKS");
			PUSH_toBLOCK(new GPerlCell(Call, t->data));
			callFlag = true;
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
				DBG_PL("----------------function-------------------");
				MOVE_NEXT_TOKEN();
				iterate_count--;
				func_iterate_count = iterate_count+1;
				GPerlScope *body = parse(tokens, it);
				DBG_PL("-----------return from function------------");
				//DBG_PL("func_iterate_count = [%d]", func_iterate_count);
				it += func_iterate_count;
				root->body = body;
				funcFlag = false;
				DBG_PL("ROOT->BODY = BODY");
				func_iterate_count--;
			} else if (ifStmtFlag) {
				//DBG_PL("IFStmtFlag: ON");
				GPerlCell *cond = lastBlock();
				POP_fromBLOCK();
				root->cond = cond;
				cond->parent = root;
				MOVE_NEXT_TOKEN();
				//DBG_PL("iterate_count = [%d]", iterate_count);
				func_iterate_count += iterate_count;
				iterate_count = 0;
				DBG_PL("-----------ifstmt------------");
				GPerlScope *scope = parse(tokens, it);
				//DBG_PL("iterate_count = [%d]", iterate_count);
				it += iterate_count;
				func_iterate_count += iterate_count;
				root->true_stmt = scope;
				ifStmtFlag = false;
				//DBG_PL("func_iterate_count = [%d]", func_iterate_count);
				DBG_PL("ROOT->TRUE_STMT = SCOPE");
				func_iterate_count--;
			} else if (elseStmtFlag) {
				//DBG_PL("ElseStmtFlag: ON");
				MOVE_NEXT_TOKEN();
				iterate_count = 0;
				DBG_PL("-----------elsestmt------------");
				GPerlScope *scope = parse(tokens, it);
				//DBG_PL("iterate_count = [%d]", iterate_count);
				it += iterate_count;
				func_iterate_count += iterate_count;
				//DBG_PL("func_iterate_count = [%d]", func_iterate_count);
				root->false_stmt = scope;
				elseStmtFlag = false;
				//DBG_PL("ROOT->FALSE_STMT = SCOPE");
				func_iterate_count--;
			}
			continue;
		}
		case RightBrace: {
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("RIGHT BRACE:");
			MOVE_NEXT_TOKEN();
			return ast;
		}
		case Comma: {
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("COMMA:VARGS->BLOCKS & CLEAR BLOCKS");
			GPerlCell *stmt = blocks.at(0);
			GPerlCell *v = root;
			for (; v->vargs; v = v->vargs) {}
			v->vargs = stmt;
			CLEAR_BLOCK();
			break;
		}
		case LeftParenthesis:
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("[(]:ON LeftParenthesis Flag");
			leftParenthesisFlag = true;
			break;
		case RightParenthesis:
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("RightParenthesis");
			if (callFlag) {
				GPerlCell *stmt = lastBlock();
				GPerlCell *func = blocks.at(block_num-2);
				func->vargs = stmt;//TODO:multiple argument
				POP_fromBLOCK();
				callFlag = false;
			} else if (block_num > 1) {
				DBG_PL("[)]:CONNECT BLOCK <=> BLOCK");
				GPerlCell *to = lastBlock();
				POP_fromBLOCK();
				GPerlCell *from = lastBlock();
				from->right = to;
				to->parent = from;
			}
			break;
		case SemiColon: {
			DBG_PL("L[%d] : ", iterate_count);
			DBG_PL("SEMICOLON:END AST");
			int size = blocks.size();
			DBG_PL("BLOCKS SIZE = [%d]", size);
			if (size == 1) {
				GPerlCell *stmt = blocks.at(0);
				if (root->type == PrintDecl || root->type == Call) {
					GPerlCell *v = root;
					for (; v->vargs; v = v->vargs) {}
					v->vargs = stmt;
				} else {
					root = stmt;
				}
			} else if (size == 2) {
				//Assign Statement
				GPerlCell *left = blocks.at(0); /* [=] */
				GPerlCell *right = blocks.at(1);
				left->right = right;
				right->parent = left;
				root = left;
			}
			ast->add(root);
			CLEAR_BLOCK();
			DBG_PL("*************************");
			if (it != tokens->end()) {
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
