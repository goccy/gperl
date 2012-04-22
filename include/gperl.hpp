#include <stdio.h>
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

typedef enum {
	Return,
	Add,
	Sub,
	Mul,
	Div,
	Greater,
	Less,
	GreaterEqual,
	LessEqual,
	EqualEqual,
	NotEqual,
	VarDecl,
	FunctionDecl,
	Assign,
	PrintDecl,
	IfStmt,
	ElseStmt,
	Comma,
	SemiColon,
	LeftParenthesis,
	RightParenthesis,
	LeftBrace,
	RightBrace,
	LeftBracket,
	RightBracket,
	Shift,
	FieldDecl,
	TypeRef,
	LabelRef,
	LocalVarDecl,
	GlobalVarDecl,
	Var,
	Int,
	Float,
	String,
	Object,
	Operator,
	LocalVar,
	GlobalVar,
	Function,
	Call,
	Argument,
	Undefined,
} GPerlT;

typedef struct _GPerlTokenType {
	GPerlT type;
	const char *name;
	const char *str;
} GPerlTokenType;

typedef enum {
	OPUNDEF,
	OPMOV,
	OPiMOV,
	OPsMOV,
	OPOMOV,
	OPOiMOV,
	OPADD,
	OPiADD,
	OPSUB,
	OPiSUB,
	OPMUL,
	OPiMUL,
	OPDIV,
	OPiDIV,
	OPJG,
	OPiJG,
	OPJL,
	OPiJL,
	OPJGE,
	OPiJGE,
	OPJLE,
	OPiJLE,
	OPJE,
	OPiJE,
	OPJNE,
	OPiJNE,
	OPRET,
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
	OPSHIFT,
	OPiPUSH,
	OPsPUSH,
} GPerlOpCodes;

typedef struct _GPerlOpDef {
	GPerlOpCodes code;
	const char *name;
} GPerlOpDef;

#define DECL(T, S) {T, #T, S}
extern GPerlTokenType decl_token_types[];
extern GPerlOpDef decl_opdef[];
#define TypeName(type) decl_token_types[type].name
#define RawName(type) decl_token_types[type].str
#define OpName(op) decl_opdef[op].name

class GPerl {
public:
	GPerl(int argc, char **argv);
};

class Token {
public:
	std::string data;
	GPerlT type;
	int idx;
	Token(std::string data_, int idx_ = 0);
};

class GPerlTokenizer {
public:
	GPerlTokenizer(void);
	std::vector<Token *> *tokenize(char *script);
	void annotateTokens(std::vector<Token *> *tokens);
	void dump(std::vector<Token *> *tokens);
	void dumpType(Token *token);
	GPerlT getType(const char *name);
	const char *getTypeName(GPerlT type);
	bool search(std::vector<std::string> list, std::string str);
};

class GPerlAST;
typedef GPerlAST GPerlScope;

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
	GPerlCell *vargs; /* for print */
	GPerlT type;
	std::string vname;/* variable name */
	std::string fname;/* function name */
	std::string rawstr;
	union {
		int idata;
		float fdata;
		bool bdata;
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
	GraphvizNode *createNode(GraphvizGraph *graph, const char *name);
	void drawEdge(GraphvizGraph *graph, GraphvizNode *from, GraphvizNode *to, const char *label);
#endif
};

class GPerlParser {
public:
	int iterate_count;
	int func_iterate_count;

	GPerlParser(void);
	GPerlAST *parse(std::vector<Token *> *tokens, std::vector<Token *>::iterator it);
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

typedef struct _GPerlVirtualMachineCode {
	GPerlOpCodes op; /* operation code */
	int dst;   /* register number */
	int src;   /* src value or register number */
	int code_num;
	union {
		void (*code)(void);/* selective inlining code */
		int jmp;   /* jmp register number */
	};
	const char *name; /* variable or function name */
	struct _GPerlVirtualMachineCode *func;
	void *opnext; /* for direct threading */
} GPerlVirtualMachineCode;

#define MAX_REG_SIZE 32
#define MAX_VARIABLE_NUM 128
#define MAX_FUNC_NUM 128

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
	GPerlVirtualMachineCode *getPureCodes(std::vector<GPerlVirtualMachineCode *> *c);
	void compile_(GPerlCell *path, bool isRecursive);
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
	GPerlVirtualMachineCode *createiPUSH(void);
	GPerlVirtualMachineCode *createsPUSH(void);
	GPerlVirtualMachineCode *createJMP(int jmp_num);
	void addVMCode(GPerlVirtualMachineCode *code);
	void dumpVMCode(GPerlVirtualMachineCode *code);
	void dumpPureVMCode(GPerlVirtualMachineCode *codes);
};

typedef struct _GPerlObject {
	union {
		int idata;
		float fdata;
		bool bdata;
		char *sdata;
		void *pdata; /* other Object */
	} data;
	const char *name;
} GPerlObject;

#define MAX_ARGSTACK_SIZE 8
class GPerlVirtualMachine {
public:
	GPerlObject *variable_memory[MAX_VARIABLE_NUM];
	GPerlVirtualMachineCode *func_memory[MAX_FUNC_NUM];
	GPerlObject *argstack[MAX_ARGSTACK_SIZE];
	/*
	union {
		int idata[MAX_ARGSTACK_SIZE];
		float fdata[MAX_ARGSTACK_SIZE];
		char *sdata[MAX_ARGSTACK_SIZE];
		void *pdata[MAX_ARGSTACK_SIZE];
	} argstack;
	*/

	GPerlVirtualMachine();
	void setToVariableMemory(const char *name, int idx);
	void setToFuncMemory(GPerlVirtualMachineCode *func, int idx);
	GPerlObject *getFromVariableMemory(int idx);
	GPerlVirtualMachineCode *getFromFuncMemory(int idx);
	void createDirectThreadingCode(GPerlVirtualMachineCode *codes, void **jmp_tbl);
	int run(GPerlVirtualMachineCode *codes);
};
