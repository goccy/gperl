#include <gperl.hpp>

using namespace std;

GPerlAST::GPerlAST(void)
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

GraphvizNode *GPerlAST::createNode(GraphvizGraph *graph, const char *name)
{
	GraphvizNode *node = graph->newNode(name);
	node->set("group", "domain");
	node->set("fixedsize", "true");
	node->set("width", "3.0");
	node->set("height", "2.4");
	node->set("style","filled");
	return node;
}

void GPerlAST::drawEdge(GraphvizGraph *graph, GraphvizNode *from, GraphvizNode *to)
{
	GraphvizEdge *edge = graph->newEdge(from, to);
	edge->set("style","filled");
	edge->set("color","#A9A9A9");
	edge->set("fillcolor","#A9A9A9");
}

void GPerlAST::draw(GraphvizGraph *graph, GPerlCell *c, GraphvizNode *node)
{
	GraphvizNode *left;
	GraphvizNode *right;
	char buf[32] = {0};
	if (c->vargs) {//TODO multi vargs => for stmt
		const char *to_name = c->vargs->rawstr.c_str();
		snprintf(buf, 32, "%s : [%p]", to_name, to_name);
		left = createNode(graph, (const char *)buf);
		drawEdge(graph, root_node, left);
		draw(graph, c->vargs, left);
	}
	if (c->left != NULL) {
		const char *to_name = c->left->rawstr.c_str();
		snprintf(buf, 32, "%s : [%p]", to_name, to_name);
		left = createNode(graph, (const char *)buf);
		drawEdge(graph, node, left);
	}
	if (c->right != NULL) {
		const char *to_name = c->right->rawstr.c_str();
		snprintf(buf, 32, "%s : [%p]", to_name, to_name);
		right = createNode(graph, (const char *)buf);
		drawEdge(graph, node, right);
	}
	if (c->left && c->left->left != NULL) {
		draw(graph, c->left, left);
	}
	if (c->right && c->right->left != NULL) {
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
	for (; stmt; stmt = stmt->next) {
		const char *root_name = stmt->rawstr.c_str();
		char buf[32] = {0};
		snprintf(buf, 32, "%s : [%p]", root_name, root_name);
		GraphvizNode *root_node = createNode(graph, (const char *)buf);//root_name);
		this->root_node = root_node;
		draw(graph, stmt, root_node);
		fprintf(stderr, "hogehgoe\n");
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

GPerlCell::GPerlCell(GPerlTypes type_) : type(type_)
{
	data.pdata = NULL;
	left = NULL;
	center = NULL;
	right = NULL;
	next = NULL;
	vargs = NULL;
}

GPerlParser::GPerlParser(void)
{

}


GPerlAST *GPerlParser::parse(vector<Token *> *tokens)
{
	GPerlAST *ast = new GPerlAST();
	GPerlCell *root = new GPerlCell(Return);
	vector<GPerlCell * > blocks;
	vector<Token *>::iterator it = tokens->begin();
	bool isVarDeclFlag = false;
	bool leftParenthesisFlag = false;
	int i = 0;
	int block_num = 0;
	while (it != tokens->end()) {
		Token *t = (Token *)*it;
		switch (t->type) {
		case VarDecl:
			isVarDeclFlag = true;
			break;
		case LocalVar:
			if (isVarDeclFlag) {
				fprintf(stderr, "LOCALVAR[%s]:NEW BLOCK => BLOCKS\n", t->data.c_str());
				GPerlCell *block = new GPerlCell(LocalVar);
				block->vname = t->data;
				block->rawstr = t->data;
				blocks.push_back(block);
				block_num++;
				isVarDeclFlag = false;
			} else {
				fprintf(stderr, "ERROR:syntax error!!\n");
			}
			break;
		case Var:
			if (block_num > 0 && blocks.at(block_num-1)->type != Assign) {
				GPerlCell *block = blocks.at(block_num-1);
				if (block->left == NULL) {
					fprintf(stderr, "VAR[%s]:LAST BLOCK->left\n", t->data.c_str());
					GPerlCell *var = new GPerlCell(Var);
					var->vname = t->data;
					var->rawstr = t->data;
					block->left = var;
				} else if (block->right == NULL) {
					fprintf(stderr, "VAR[%s]:LAST BLOCK->right\n", t->data.c_str());
					GPerlCell *var = new GPerlCell(Var);
					var->vname = t->data;
					var->rawstr = t->data;
					block->right = var;
				} else {
					fprintf(stderr, "ERROR:[parse error]!!\n");
				}
			} else {
				//first var
				fprintf(stderr, "VAR[%s]:NEW BLOCK->BLOCKS\n", t->data.c_str());
				GPerlCell *block = new GPerlCell(Var);
				block->vname = t->data;
				block->rawstr = t->data;
				blocks.push_back(block);
				block_num++;
			}
			break;
		case Int:
			if (block_num > 0 && blocks.at(block_num-1)->type != Assign &&
				!leftParenthesisFlag) {
				GPerlCell *block = blocks.at(block_num-1);
				//fprintf(stderr, "BLOCK:[%d]\n", block->type);
				if (block->left == NULL) {
					fprintf(stderr, "INT[%s]:LAST BLOCK->left\n", t->data.c_str());
					GPerlCell *num = new GPerlCell(Int);
					num->data.idata = atoi(t->data.c_str());
					num->rawstr = t->data;
					block->left = num;
				} else if (block->right == NULL) {
					fprintf(stderr, "INT[%s]:LAST BLOCK->right\n", t->data.c_str());
					GPerlCell *num = new GPerlCell(Int);
					num->data.idata = atoi(t->data.c_str());
					num->rawstr = t->data;
					block->right = num;
				} else {
					fprintf(stderr, "ERROR:[parse error]!!\n");
				}
			} else {
				//first var
				fprintf(stderr, "INT[%s]:NEW BLOCK->BLOCKS\n", t->data.c_str());
				GPerlCell *num = new GPerlCell(Int);
				num->data.idata = atoi(t->data.c_str());
				num->rawstr = t->data;
				blocks.push_back(num);
				block_num++;
				if (leftParenthesisFlag) {
					leftParenthesisFlag = false;
				}
			}
			break;
		case String:
			if (block_num > 0 && blocks.at(block_num-1)->type != Assign) {
				GPerlCell *block = blocks.at(block_num-1);
				//fprintf(stderr, "BLOCK:[%d]\n", block->type);
				if (block->left == NULL) {
					fprintf(stderr, "String[%s]:LAST BLOCK->left\n", t->data.c_str());
					GPerlCell *str = new GPerlCell(String);
					str->data.sdata = (char *)t->data.c_str();
					str->rawstr = t->data;
					block->left = str;
				} else if (block->right == NULL) {
					fprintf(stderr, "String[%s]:LAST BLOCK->right\n", t->data.c_str());
					GPerlCell *str = new GPerlCell(String);
					str->data.sdata = (char *)t->data.c_str();
					str->rawstr = t->data;
					block->right = str;
				} else {
					fprintf(stderr, "ERROR:[parse error]!!\n");
				}
			} else {
				//first var
				fprintf(stderr, "String[%s]:NEW BLOCK->BLOCKS\n", t->data.c_str());
				GPerlCell *str = new GPerlCell(String);
				str->data.sdata = (char *)t->data.c_str();
				str->rawstr = t->data;
				blocks.push_back(str);
				block_num++;
			}
			break;
		case Operator: {
			fprintf(stderr, "OPERATOR[%s]:LAST BLOCK->PARENT\n", t->data.c_str());
			GPerlCell *block = blocks.at(block_num-1);
			GPerlCell *op;
			if (t->data == "+") {
				op = new GPerlCell(Add);
				op->rawstr = t->data;
			}
			//fprintf(stderr, "op = [%p]\n", op);
			block->parent = op;
			op->left = block;
			blocks.pop_back();
			blocks.push_back(op);
			break;
		}
		case Assign: {
			fprintf(stderr, "ASSIGN:LAST BLOCK->PARENT\n");
			GPerlCell *block;
			if (block_num > 0) {
				block = blocks.at(block_num-1);
				blocks.pop_back();
			} else {
				fprintf(stderr, "ERROR:syntax error!!\n");
			}
			GPerlCell *assign = new GPerlCell(Assign);
			assign->rawstr = t->data;
			block->parent = assign;
			assign->left = block;
			blocks.push_back(assign);
			break;
		}
		case PrintDecl: {
			fprintf(stderr, "PRINT:NEW BLOCK->BLOCKS\n");
			GPerlCell *p = new GPerlCell(PrintDecl);
			p->rawstr = t->data;
			root = p;
			//blocks.push_back(p);
			//block_num++;
			break;
		}
		case Comma: {
			fprintf(stderr, "COMMA:VARGS->BLOCKS & CLEAR BLOCKS\n");
			GPerlCell *stmt = blocks.at(0);
			GPerlCell *v = root;
			for (; v->vargs; v = v->vargs) {}
			v->vargs = stmt;
			blocks.clear();
			block_num = 0;
			break;
		}
		case LeftParenthesis:
			fprintf(stderr, "[(]:ON LeftParenthesis Flag\n");
			leftParenthesisFlag = true;
			break;
		case RightParenthesis:
			fprintf(stderr, "[)]:CONNECT BLOCK <=> BLOCK\n");
			if (block_num > 0) {
				GPerlCell *to = blocks.at(block_num-1);
				//fprintf(stderr, "to = [%p]\n", to);
				//fprintf(stderr, "to = [%s]\n", to->rawstr.c_str());
				blocks.pop_back();
				block_num--;
				GPerlCell *from = blocks.at(block_num-1);
				//fprintf(stderr, "from = [%p]\n", from);
				//fprintf(stderr, "from = [%s]\n", from->rawstr.c_str());
				from->right = to;
				to->parent = from;
			} else {
				fprintf(stderr, "ERROR:syntax error!!\n");
			}
			break;
		case SemiColon: {
			fprintf(stderr, "SEMICOLON:END AST\n");
			int size = blocks.size();
			fprintf(stderr, "BLOCKS SIZE = [%d]\n", size);
			if (size == 1) {
				GPerlCell *stmt = blocks.at(0);
				if (root->type == PrintDecl) {
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
			blocks.clear();
			block_num = 0;
			fprintf(stderr, "*************************\n");
			if (it != tokens->end()) {
				root = new GPerlCell(Return);
			}
			break;
		}
		default:
			break;
		}
		it++;
		i++;
	}
	fprintf(stderr, "ast size = [%d]\n", ast->size);
	fprintf(stderr, "=====================================\n");
	return ast;
}
