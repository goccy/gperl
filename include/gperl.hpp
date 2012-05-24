#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#define EOL '\0'
#define MAX_LINE_SIZE 128
#define MAX_TOKEN_SIZE 4096
#define MAX_SCRIPT_SIZE 4096 * 10

#define cstr(s) s.c_str()

#ifdef DEBUG_MODE
#define DBG_P(fmt, ...) {\
	fprintf(stderr, fmt, ## __VA_ARGS__);	\
	}
#define DBG_PL(fmt, ...) {\
	fprintf(stderr, fmt, ## __VA_ARGS__);	\
	fprintf(stderr, "\n");						\
	}
#else
#define DBG_P(fmt, ...) {}
#define DBG_PL(fmt, ...) {}
#endif

#include <gen_token.hpp>

typedef struct _GPerlTokenType {
	GPerlT type;
	const char *name;
	const char *str;
} GPerlTokenType;

#include <gen_vmcode.hpp>

typedef enum {
	OPUNDEF,
	OPMOV,
	OPiMOV,
	OPsMOV,
	OPOMOV,
	OPLMOV,
	OPOiMOV,
	OPADD,
	OPiADD,
	OPiADDC,
	OPSUB,
	OPiSUB,
	OPiSUBC,
	OPMUL,
	OPiMUL,
	OPiMULC,
	OPDIV,
	OPiDIV,
	OPiDIVC,
	OPJG,
	OPiJG,
	OPiJGC,
	OPJL,
	OPiJL,
	OPiJLC,
	OPJGE,
	OPiJGE,
	OPiJGEC,
	OPJLE,
	OPiJLE,
	OPiJLEC,
	OPJE,
	OPiJE,
	OPiJEC,
	OPJNE,
	OPiJNE,
	OPiJNEC,
	OPRET,
	OPJRET,
	OPTHCODE,
	OPNOP,
	OPiWRITE,
	OPsWRITE,
	OPoWRITE,
	OPPRINT,
	OPJMP,
	OPLET,
	OPSET,
	OPFUNCSET,
	OPCALL,
	OPJCALL,
	OPSELFCALL,
	OPJSELFCALL,
	OPSHIFT,
	OPiPUSH,
	OPsPUSH,
	OPoPUSH,
	OPARRAY_PUSH,
	OPLIST_NEW,
	/*------------final inst----------------*/
	OPABADD,
	OPACADD,
	OPADADD,
	OPBCADD,
	OPBDADD,
	OPCDADD,

	OPAiSUBC,
	OPBiSUBC,
	OPCiSUBC,
	OPDiSUBC,

	OPAiJLC,
	OPBiJLC,
	OPCiJLC,
	OPDiJLC,

	OPAiPUSH,
	OPBiPUSH,
	OPCiPUSH,
	OPDiPUSH,

	OPAiMOV,
	OPBiMOV,
	OPCiMOV,
	OPDiMOV,

	OPAOMOV,
	OPBOMOV,
	OPCOMOV,
	OPDOMOV,

	OPASELFCALL,
	OPBSELFCALL,
	OPCSELFCALL,
	OPDSELFCALL,

	OPAJSELFCALL,
	OPBJSELFCALL,
	OPCJSELFCALL,
	OPDJSELFCALL,

	OPARET,
	OPBRET,
	OPCRET,
	OPDRET,
	OPAJRET,
	OPBJRET,
	OPCJRET,
	OPDJRET,
	OPSUPERCALL,
} GPerlOpCodes;

#define MAX_ARGSTACK_SIZE 8
#define MAX_CALLSTACK_SIZE 128

typedef struct _GPerlOpDef {
	GPerlOpCodes code;
	const char *name;
} GPerlOpDef;

#define DECL(T, S) {T, #T, S}
extern GPerlTokenInfo decl_tokens[];
extern GPerlCodeInfo decl_codes[];
extern GPerlOpDef decl_opdef[];
#define OpName(op) decl_opdef[op].name

class GPerl {
public:
	GPerl(int argc, char **argv);
};

class GPerlToken {
public:
	std::string data;
	GPerlT type;
	GPerlTokenInfo info;
	int idx;
	GPerlToken(std::string data_, int idx_ = 0);
};

class GPerlTokenizer {
public:
	GPerlTokenizer(void);
	std::vector<GPerlToken *> *tokenize(char *script);
	void annotateTokens(std::vector<GPerlToken *> *tokens);
	void dump(std::vector<GPerlToken *> *tokens);
	GPerlTokenInfo getTokenInfo(const char *name, const char *data);
	bool search(std::vector<std::string> list, std::string str);
};

class GPerlAST;
typedef GPerlAST GPerlScope;

class GPerlNodes;

class GPerlCell {
public:
	GPerlCell *parent;
	union {
		GPerlCell *left;
		GPerlCell *cond;
	};
	union {
		GPerlScope *true_stmt;/* for if stmt */
		GPerlScope *body;/* for function block */
	};
	union {
		GPerlCell *right;
		GPerlScope *false_stmt;
	};
	GPerlCell *next;/* for next stmt */
	GPerlCell *vargs[MAX_ARGSTACK_SIZE]; /* for print */
	int argsize;
	GPerlT type;
	GPerlTokenInfo info;
	std::string vname;/* variable name */
	std::string fname;/* function name */
	std::string rawstr;
	union {
		int idata;
		float fdata;
		char *sdata;
		void *pdata; /* other Object */
	} data;
	GPerlCell(GPerlT type_);
	GPerlCell(GPerlT type_, std::string name);
};

#ifdef USING_GRAPH_DEBUG
class GraphvizContext;
class GraphvizGraph;
class GraphvizNode;
class GraphvizEdge;
#endif

class GPerlAST {
public:
	int size;
	int cluster_num;
	GPerlCell *root;
	GPerlCell *cur;
#ifdef USING_GRAPH_DEBUG
	GraphvizNode *root_node;
#else
	void *root_node;
#endif

	GPerlAST(void);
	void add(GPerlCell *root);
#ifdef USING_GRAPH_DEBUG
	void show(void);
	void draw(GraphvizGraph *graph, GPerlCell *c, GraphvizNode *node);
	void drawStmt(GraphvizGraph *graph, GraphvizNode *node, GPerlScope *stmt, const char *stmt_name, const char *color);
	GraphvizNode *createNode(GraphvizGraph *graph, const char *name);
	void drawEdge(GraphvizGraph *graph, GraphvizNode *from, GraphvizNode *to, const char *label);
#endif
};

class GPerlParser {
public:
	int iterate_count;
	int func_iterate_count;
	std::vector<GPerlToken *>::iterator it;
	std::vector<GPerlToken *>::iterator end;

	GPerlParser(std::vector<GPerlToken *> *tokens);
	GPerlAST *parse(void);
	void parseValue(GPerlToken *t, GPerlNodes *nodes, GPerlScope *scope);
};

#ifdef USING_GRAPH_DEBUG
#include <graphviz/gvc.h>
#include <graphviz/graph.h>

class GraphvizContext {
public:
	GVC_t *gvc;
	GraphvizContext(void);
	void parseArgs(const char *format, const char *filename);
	void layoutJobs(GraphvizGraph *g);
	void renderJobs(GraphvizGraph *g);
	void freeLayout(GraphvizGraph *g);
	~GraphvizContext(void);
};

class GraphvizGraph {
public:
	Agraph_t *g;
	GraphvizGraph(const char *name, int kind);
	GraphvizGraph(void);
	GraphvizGraph *makeSubGraph(const char *name);
	void set(const char *name, const char *value);
	GraphvizNode *newNode(const char *name);
	GraphvizEdge *newEdge(GraphvizNode *start, GraphvizNode *end);
	~GraphvizGraph(void);
};

class GraphvizNode {
public:
	Agnode_t *node;
	GraphvizNode(void);
	void set(const char *name, const char *value);
	void copyAttributeTo(void *oldobj, void *newobj);
	void free(GraphvizGraph *g);
};

class GraphvizEdge {
public:
	Agedge_t *edge;
	GraphvizEdge(void);
	void set(const char *name, const char *value);
	void copyAttributeTo(void *oldobj, void *newobj);
	void free(GraphvizGraph *g);
};
#endif

#define MAX_REG_SIZE 32
#define MAX_VARIABLE_NUM 128
#define MAX_FUNC_NUM 128

typedef union {
	uint64_t bytes; /* for typecheck */
	int ivalue;
	double dvalue;
	bool bvalue;
	char *svalue;
	void *ovalue; /* other Object */
} GPerlValue;

typedef struct _GPerlObject {
	GPerlT type;
	GPerlValue value;
	const char *name;
} GPerlObject;

typedef struct _GPerlVirtualMachineCode {
	GPerlOpCode op; /* operation code */
	int dst;   /* register number */
	int src;   /* src value or register number */
	int code_num;
	union {
		void *code;/* selective inlining code */
		int jmp;   /* jmp register number */
		GPerlObject **list;
	};
	const char *name; /* variable or function name */
	struct _GPerlVirtualMachineCode *func;
	void *opnext; /* for direct threading */
} GPerlVirtualMachineCode;

class GPerlCompiler {
public:
	int dst;
	int src;
	int code_num;
	GPerlT reg_type[MAX_REG_SIZE];
	std::vector<GPerlVirtualMachineCode *> *codes;
	std::vector<GPerlVirtualMachineCode *> *func_code;
	const char *variable_names[MAX_VARIABLE_NUM];
	const char *func_names[MAX_FUNC_NUM];
	int variable_types[MAX_VARIABLE_NUM];
	const char *declared_vname;
	const char *declared_fname;
	int variable_index;
	int func_index;
	int args_count;/* for shift */

	GPerlCompiler(void);
	GPerlVirtualMachineCode *compile(GPerlAST *ast);
	void finalCompile(std::vector<GPerlVirtualMachineCode *> *f);
	void optimizeFuncCode(std::vector<GPerlVirtualMachineCode *> *f, std::string fname);
	GPerlVirtualMachineCode *getPureCodes(std::vector<GPerlVirtualMachineCode *> *c);
	void compile_(GPerlCell *path);
	void setToVariableNames(const char *name);
	void setToFunctionNames(const char *name);
	int getVariableIndex(const char *name);
	int getFuncIndex(const char *name);
	GPerlVirtualMachineCode *createVMCode(GPerlCell *c);
	GPerlVirtualMachineCode *createTHCODE(void);
	GPerlVirtualMachineCode *createRET(void);
	GPerlVirtualMachineCode *createUNDEF(void);
	GPerlVirtualMachineCode *createiWRITE(void);
	GPerlVirtualMachineCode *createsWRITE(void);
	GPerlVirtualMachineCode *createoWRITE(void);
	GPerlVirtualMachineCode *createiPUSH(int i, int dst_);
	GPerlVirtualMachineCode *createsPUSH(int i, int dst_);
	GPerlVirtualMachineCode *createoPUSH(int i, int dst_);
	GPerlVirtualMachineCode *createJMP(int jmp_num);
	void addWriteCode(void);
	void addPushCode(int i, int dst_);
	void genFunctionCallCode(GPerlCell *p);
	void genFunctionCode(GPerlCell *path);
	void genIfStmtCode(GPerlCell *path);
	void genVMCode(GPerlCell *path);
	void addVMCode(GPerlVirtualMachineCode *code);
	void popVMCode(void);
	void dumpVMCode(GPerlVirtualMachineCode *code);
	void dumpPureVMCode(GPerlVirtualMachineCode *codes);
};

typedef GPerlCell GPerlNode;
class GPerlNodes : public std::vector<GPerlCell *> {
public:
	int block_num;
	GPerlNodes(void);
	void pushNode(GPerlNode *node);
	void popNode(void);
	void swapLastNode(GPerlNode *node);
	void clearNodes(void);
	GPerlNode *lastNode(void);
};

typedef union {
	uint64_t bytes[MAX_REG_SIZE];
	int ivalue[MAX_REG_SIZE];
	double dvalue[MAX_REG_SIZE];
	char *svalue[MAX_REG_SIZE];
	GPerlObject *ovalue[MAX_REG_SIZE];
} Reg;

typedef struct _GPerlEnv {
	GPerlValue *reg;
	GPerlObject **argstack;
	GPerlVirtualMachineCode *pc;
	void *ret_addr;
} GPerlEnv;

#include <sys/mman.h>
#include <unistd.h>
typedef struct _InstBlock {
	void *start;
	void *end;
} InstBlock;

class GPerlVirtualMachine {
public:
	GPerlObject *variable_memory[MAX_VARIABLE_NUM];
	GPerlVirtualMachineCode *func_memory[MAX_FUNC_NUM];

	GPerlVirtualMachine();
	void setToVariableMemory(const char *name, int idx);
	void setToFuncMemory(GPerlVirtualMachineCode *func, int idx);
	GPerlObject *getFromVariableMemory(int idx);
	GPerlVirtualMachineCode *getFromFuncMemory(int idx);
	GPerlEnv *createCallStack(void);
	GPerlObject **createArgStack(void);
	void createDirectThreadingCode(GPerlVirtualMachineCode *codes, void **jmp_tbl);
	void createSelectiveInliningCode(GPerlVirtualMachineCode *codes, void **jmp_tbl, InstBlock *block_tbl);
	int run(GPerlVirtualMachineCode *codes);
};
