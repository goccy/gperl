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

void GPerlAST::drawCondStmt(GraphvizGraph *graph, GPerlCell *stmt_, const char *stmt_name, const char *color)
{
	char buf[32] = {0};
	snprintf(buf, 32, "cluster%d", cluster_num);
	GraphvizGraph *stmt_graph = graph->makeSubGraph(buf);
	GPerlCell *stmt = stmt_;
	const char *stmt_root_name = stmt->rawstr.c_str();
	cluster_num++;
	stmt_graph->set("fillcolor", color);
	stmt_graph->set("style","filled");
	snprintf(buf, 32, "%s stmt: [%p]", stmt_name, stmt_);
	stmt_graph->set("label", buf);
	stmt_graph->set("fontsize", "24");
	snprintf(buf, 32, "%s : [%p]", stmt_root_name, stmt);
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
	if (c->type == IfStmt || c->type == WhileStmt || c->type == ElsifStmt ||
		c->type == ForStmt || c->type == ForeachStmt) {
		GraphvizNode *if_node = root_node;
		drawStmt(graph, if_node, c->true_stmt, "true", "#e0ffff");
		if (c->false_stmt && c->type != ForeachStmt) {
			if (c->false_stmt->root->type == ElsifStmt) {
				drawStmt(graph, if_node, c->false_stmt, "elsif", "#98fb98");
			} else {
				drawStmt(graph, if_node, c->false_stmt, "false", "#fff0f5");
			}
		}
		if (c->type == ForStmt) {
			const char *to_name = c->left->rawstr.c_str();
			snprintf(buf, 32, "%s : [%p]", to_name, c->left);
			left = createNode(graph, (const char *)buf);
			drawEdge(graph, node, left, "left");
			drawCondStmt(graph, c->left, "for_cond", "#fff0f5");
		}
	} else if (c->type == Function) {
		GraphvizNode *func_node = root_node;
		drawStmt(graph, func_node, c->body, "body", "#e6e6fa");
	} else if (c->type == Call || c->type == BuiltinFunc) {
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
	} else if (c->type == List || c->type == ArrayRef) {
		for (int i = 0; i < c->argsize; i++) {
			GPerlCell *val = c->vargs[i];
			const char *to_name = val->rawstr.c_str();
			snprintf(buf, 32, "%s : [%p]", to_name, val);
			left = createNode(graph, (const char *)buf);
			drawEdge(graph, node, left, "vargs");
			draw(graph, val, left);
		}
	}
	if (c->left && c->type != ForStmt) {
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
