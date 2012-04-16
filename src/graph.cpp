#ifdef USING_GRAPH_DEBUG
#include <gperl.hpp>

GraphvizContext::GraphvizContext(void)
{
	gvc = gvContext();
}

void GraphvizContext::parseArgs(const char *format, const char *filename)
{
	char *buf = (char *)malloc(16);
	char *buf2 = (char *)malloc(32);
	memset(buf, 0, 16);
	memset(buf2, 0, 32);
	int format_size = strlen(format);
	int filename_size = strlen(filename);
	snprintf(buf, format_size + 3, "-T%s", format);
	snprintf(buf2, filename_size + format_size + 4, "-o%s.%s",
			 filename, format);
	//fprintf(stderr, "buf = [%s], buf2 = [%s]\n", buf, buf2);
	char *argv[] = {(char *)"dot"/*redering engine name*/,
					buf, buf2,
					(char *)"-q"/*hide warning*/};
	gvParseArgs(gvc, 4, argv);
}

void GraphvizContext::layoutJobs(GraphvizGraph *g)
{
	gvLayoutJobs(gvc, g->g);
}

void GraphvizContext::renderJobs(GraphvizGraph *g)
{
	gvRenderJobs(gvc, g->g);
}

void GraphvizContext::freeLayout(GraphvizGraph *g)
{
	gvFreeLayout(gvc, g->g);
}

GraphvizContext::~GraphvizContext(void)
{
	gvFreeContext(gvc);
}

GraphvizGraph::GraphvizGraph(const char *name, int kind)
{
	g = agopen((char *)name, kind);
}

GraphvizGraph::GraphvizGraph(void)
{
	g = NULL;
}

GraphvizGraph::~GraphvizGraph(void)
{
	agclose(g);
}

GraphvizGraph *GraphvizGraph::makeSubGraph(const char *name)
{
	Agraph_t *subg = agsubg(g, (char *)name);
	GraphvizGraph *graph = new GraphvizGraph();
	graph->g = subg;
	return graph;
}

void GraphvizGraph::set(const char *name, const char *value)
{
	agsafeset(g, (char *)name, (char *)value, NULL);
}

GraphvizNode *GraphvizGraph::newNode(const char *name)
{
	Agnode_t *n = agnode(g, (char *)name);
	GraphvizNode *node = new GraphvizNode();
	node->node = n;
	return node;
}

GraphvizEdge *GraphvizGraph::newEdge(GraphvizNode *start, GraphvizNode *end)
{
	Agedge_t *e = agedge(g, start->node, end->node);
	GraphvizEdge *edge = new GraphvizEdge();
	edge->edge = e;
	return edge;
}

GraphvizNode::GraphvizNode(void)
{

}

void GraphvizNode::free(GraphvizGraph *g)
{
	agdelete(g->g, node);
}

void GraphvizNode::set(const char *name, const char *value)
{
	agsafeset(node, (char *)name, (char *)value, NULL);
}

void GraphvizNode::copyAttributeTo(void *oldobj, void *newobj)
{
	agcopyattr(oldobj, newobj);
}

GraphvizEdge::GraphvizEdge(void)
{

}

void GraphvizEdge::free(GraphvizGraph *g)
{
	agdelete(g->g, (void *)edge);
}

void GraphvizEdge::set(const char *name, const char *value)
{
	agsafeset(edge, (char *)name, (char *)value, NULL);
}

void GraphvizEdge::copyAttributeTo(void *oldobj, void *newobj)
{
	agcopyattr(oldobj, newobj);
}

#endif
