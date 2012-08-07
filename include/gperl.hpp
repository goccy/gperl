#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <new>
#include <greadline.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <setjmp.h>

#define EOL '\0'
#define MAX_LINE_SIZE 128
#define MAX_TOKEN_SIZE 4096
#define MAX_SCRIPT_SIZE 4096 * 10
#define NaN       (uint64_t)(0xFFF0000000000000)
#define Mask      (uint64_t)(0x00000000FFFFFFFF)
#define TYPE      (uint64_t)(0x000F000000000000)

#define IntTag    (uint64_t)(0x0001000000000000)
#define StringTag (uint64_t)(0x0002000000000000)
#define ObjectTag (uint64_t)(0x0003000000000000)

#define INT_init(o, idata) o.ivalue = idata; o.bytes = ((idata & Mask) | NaN | IntTag)
#define DOUBLE_init(o, ddata) o.dvalue = ddata;
#define STRING_init(o, ptr) o.svalue = ptr; o.bytes |= NaN | StringTag
#define OBJECT_init(o, ptr) o.ovalue = ptr; o.bytes |= NaN | ObjectTag
#define getStringObj(o) (GPerlString *)(o.bytes ^ (NaN | StringTag))
#define getRawString(o) ((GPerlString *)(o.bytes ^ (NaN | StringTag)))->s
#define getLength(o) ((GPerlString *)(o.bytes ^ (NaN | StringTag)))->len
#define getObject(o) (o.bytes ^ (NaN | ObjectTag))
#define TYPE_CHECK(T) ((T.bytes & NaN) == NaN) * ((T.bytes & TYPE) >> 48 )

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
#include <gen_vmcode.hpp>

#define MAX_ARGSTACK_SIZE 8
#define MAX_CALLSTACK_SIZE 128

#define DECL(T, S) {T, #T, S}
extern GPerlTokenInfo decl_tokens[];
extern GPerlCodeInfo decl_codes[];
struct _GPerlString;

typedef union {
	uint64_t bytes; /* for typecheck */
	int ivalue;
	double dvalue;
	bool bvalue;
	_GPerlString *svalue;
	void *ovalue; /* other Object */
} GPerlValue;

class GPerl {
public:
	int brace_count;

	GPerl(int argc, char **argv);
	void init(void);
	int checkBrace(char *line);
	void startInteractiveMode(void);
	void startEvalScriptMode(int argc, char **argv);
	GPerlValue eval(char *script, int argc = 0, char **argv = NULL);
};

class GPerlToken {
public:
	std::string data;
	GPerlT type;
	GPerlTokenInfo info;
	int idx;
	int indent;
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
	int vidx; /* variable idx */
	int indent;
	GPerlCell(GPerlT type_);
	GPerlCell(GPerlT type_, std::string name);
	void setVariableIdx(int idx);
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
	void drawCondStmt(GraphvizGraph *graph, GPerlCell *stmt, const char *stmt_name, const char *color);
	GraphvizNode *createNode(GraphvizGraph *graph, const char *name);
	void drawEdge(GraphvizGraph *graph, GraphvizNode *from, GraphvizNode *to, const char *label);
#endif
};

class GPerlParser {
public:
	int iterate_count;
	int func_iterate_count;
	int vidx; /* variable idx */
	int vcount; /* variable count in scope block */
	int indent;
	int argc;
	char **argv;

	std::vector<GPerlToken *>::iterator it;
	std::vector<GPerlToken *>::iterator end;

	GPerlParser(std::vector<GPerlToken *> *tokens, int argc = 0, char **argv = NULL);
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

struct _GPerlObject;
struct _GPerlMemPool;

class GPerlMemoryManager {
public:
	_GPerlMemPool **pools;
	_GPerlObject *freeList;
	_GPerlObject *guard;
	_GPerlObject *guard_prev_ptr;
	size_t max_pool_size;
	size_t pool_size;
    struct sigaction sa;
	void (GPerlMemoryManager::*gc)();

	GPerlMemoryManager(void);
	_GPerlObject* gmalloc(void);
	bool isMarked(_GPerlObject* obj);
	void expandMemPool(void);
	void dummy_gc(void);
	void msgc(void);
	void gcWrapper(void);
	void gc_mark(GPerlValue v);
	void gc_sweep(void);
	void traceRoot(void);
};

typedef struct _GPerlMemPool {
	_GPerlObject *body;
	_GPerlObject *head;
	_GPerlObject *tail;
	size_t size;
} GPerlMemPool;

typedef struct _GPerlObjectHeader {
	intptr_t type;
	intptr_t mark_flag;
	_GPerlObject *next;
} GPerlObjectHeader;

typedef struct _GPerlObject {
	GPerlObjectHeader h;
	void *slot1;
	void *slot2;
	void (*write)(GPerlValue v);
	void (*mark)(_GPerlObject *o);
	void (*free)(_GPerlObject *o);
} GPerlObject;

typedef struct _GPerlArray {
	GPerlObjectHeader h;
	intptr_t size;
	GPerlValue *list;
	void (*write)(GPerlValue v);
	void (*mark)(_GPerlObject *o);
	void (*free)(GPerlObject *o);
} GPerlArray;

typedef struct _GPerlString {
	GPerlObjectHeader h;
	char *s;
	size_t len;
	void (*write)(GPerlValue v);
	void (*mark)(_GPerlObject *o);
	void (*free)(GPerlObject *o);
} GPerlString;

/* Protected ArrayObject */
typedef struct _GPerlArgsArray {
	GPerlObjectHeader h;
	intptr_t size;
	GPerlValue *list;
	void (*write)(GPerlValue v);
	void (*mark)(_GPerlObject *o);
	void (*free)(GPerlObject *o);
} GPerlArgsArray;

typedef struct _GPerlUndef {
	GPerlObjectHeader h;
	void *slot1;
	void *slot2;
	void (*write)(GPerlValue v);
	void (*mark)(_GPerlObject *o);
	void (*free)(GPerlObject *o);
} GPerlUndef;

typedef struct _GPerlVirtualMachineCode {
	GPerlOpCode op; /* operation code */
	int dst;   /* register number */
	int src;   /* src value or register number */
	int code_num;
	GPerlValue v;
	union {
		void *code;/* selective inlining code */
		int jmp;   /* jmp register number */
		//int ebp;   /* stack base pointer */
		int idx;   /* array[idx] */
		GPerlObject *(*_new)(GPerlValue);
		void (*push)(GPerlValue *);
		void (*write)(GPerlValue );
	};
	int cur_reg_top;
	int cur_stack_top;
	int argc; /* for JIT_CALL */
	const char *name;
	struct _GPerlVirtualMachineCode *func;
	void *opnext; /* for direct threading */
} GPerlVirtualMachineCode;

class GPerlCompiler {
public:
	int dst;
	int src;
	int code_num;
	int loop_start_num;
	GPerlT reg_type[MAX_REG_SIZE];
	std::vector<GPerlVirtualMachineCode *> *codes;
	std::vector<GPerlVirtualMachineCode *> *func_code;
	const char *variable_names[MAX_VARIABLE_NUM];
	const char *func_names[MAX_FUNC_NUM];
	int variable_types[MAX_VARIABLE_NUM];
	const char *declared_vname;
	const char *declared_fname;
	int cur_stack_top;
	int func_index;
	int args_count;/* for CALL */
	int recv_args_count; /* for ARGMOV */
	std::map<std::string, int> local_vmap;
	std::map<std::string, int> global_vmap;

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
	void setInstByVMap(GPerlVirtualMachineCode *code, GPerlCell *c, GPerlOpCode lop, GPerlOpCode gop, int *idx);
	void setEscapeStackNum(GPerlVirtualMachineCode *code, GPerlCell *c);
	void setMOV(GPerlVirtualMachineCode *code, GPerlCell *c);
	void setVMOV(GPerlVirtualMachineCode *code, GPerlCell *c);
	void setINC(GPerlVirtualMachineCode *code, GPerlCell *c);
	void setDEC(GPerlVirtualMachineCode *code, GPerlCell *c);
	void setLET(GPerlVirtualMachineCode *code, GPerlCell *c);
	void setSETv(GPerlVirtualMachineCode *code, GPerlCell *c);
	void setCALL(GPerlVirtualMachineCode *code, GPerlCell *c);
	void setBFUNC(GPerlVirtualMachineCode *code, GPerlCell *c);
	void setFUNC(GPerlVirtualMachineCode *code, GPerlCell *c);
	void setArrayARGMOV(GPerlVirtualMachineCode *code, GPerlCell *c);
	void setARGMOV(GPerlVirtualMachineCode *code, GPerlCell *c);
	void setArrayAt(GPerlVirtualMachineCode *code, GPerlCell *c);
	void setArrayDereference(GPerlVirtualMachineCode *code, GPerlCell *c);
	GPerlVirtualMachineCode *createTHCODE(void);
	GPerlVirtualMachineCode *createRET(void);
	GPerlVirtualMachineCode *createUNDEF(void);
	GPerlVirtualMachineCode *createWRITE(void);
	GPerlVirtualMachineCode *createiWRITE(void);
	GPerlVirtualMachineCode *createsWRITE(void);
	GPerlVirtualMachineCode *createoWRITE(void);
	GPerlVirtualMachineCode *createPUSH(int i, int dst_);
	GPerlVirtualMachineCode *createJMP(int jmp_num);
	void addWriteCode(void);
	void addPushCode(int i, int dst_);
	void genFunctionCallCode(GPerlCell *p);
	void genFunctionCode(GPerlCell *path);
	void genIfStmtCode(GPerlCell *path);
	void genWhileStmtCode(GPerlCell *path);
	void genForStmtCode(GPerlCell *path);
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

typedef struct _GPerlEnv {
	GPerlValue *reg;
	GPerlValue *argstack;
	size_t args_size;
	GPerlVirtualMachineCode *pc;
	GPerlVirtualMachineCode **cur_pc;
	void *ret_addr;
	int reg_top;
	int esp;
	int ebp;
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
	void setToFuncMemory(GPerlVirtualMachineCode *func, int idx);
	GPerlVirtualMachineCode *getFromFuncMemory(int idx);
	GPerlEnv *createCallStack(void);
	GPerlValue *createMachineStack(void);
	GPerlObject **createArgStack(void);
	void createDirectThreadingCode(GPerlVirtualMachineCode *codes, void **jmp_tbl);
	void createSelectiveInliningCode(GPerlVirtualMachineCode *codes, void **jmp_tbl, InstBlock *block_tbl);
	GPerlValue run(GPerlVirtualMachineCode *codes);
};

#ifdef USING_JIT

#include <jit/jit.h>
#define ENABLE_JIT_COMPILE
typedef struct _GPerlJITEnv {
	GPerlEnv *callstack;
	GPerlValue *stack;
	GPerlArgsArray *args;
} GPerlJITEnv;

class GPerlJmpInfo {
public:
	jit_label_t label;
	int jmp_count;
	GPerlJmpInfo(int jmp_count);
};

class GPerlJmpStack {
public:
	int jmp_stack_idx;
	GPerlJmpInfo **infs;

	GPerlJmpStack(void);
	void push(GPerlJmpInfo *info);
	GPerlJmpInfo *pop(void);
	bool isJmp(void);
};

#define MAX_JMP_STACK_SIZE 128

typedef jit_function_t GPerlJITCode;
class GPerlJITCompiler {
public:
	GPerlJITCompiler(void);
	unsigned int compile(GPerlVirtualMachineCode *codes, GPerlJITEnv *env);
	unsigned int _compile(GPerlVirtualMachineCode *codes, GPerlJITEnv *env);
	jit_value_t compileMOV(GPerlVirtualMachineCode *pc, jit_function_t *func);
	jit_value_t compileVMOV(GPerlVirtualMachineCode *pc, jit_function_t *func);
};

#endif

typedef struct _GPerlTraceRoot {
	/* callstack trace for register */
	GPerlEnv *callstack_top;
	GPerlEnv *callstack_bottom;
	/* virtual stack to stack[stack_top_idx] */
	GPerlValue *stack_bottom;
	int stack_top_idx;
	/* global variables */
	GPerlValue *global_vmemory;
	/* initial variables */
	GPerlValue *init_values;
} GPerlTraceRoot;

#define PAGE_SIZE (OBJECT_SIZE * 64)
//#define PAGE_SIZE 4096
#define PTR_SIZE sizeof(void*)
#define OBJECT_SIZE (PTR_SIZE * 8)
#define MEMORY_POOL_SIZE (OBJECT_SIZE * PAGE_SIZE)
#define NAME_RESOLUTION_PREFIX "*"
#define MAX_GLOBAL_MEMORY_SIZE 128
#define MAX_MACHINE_STACK_SIZE 1024 * 8 /* 8M */
#define MAX_INIT_VALUES_SIZE 64
#define KB 1024
#define MB KB * KB
#define MAX_CWB_SIZE 1 * MB
#define INIT_MEMPOOL_NUM 4

extern GPerlValue global_vmemory[MAX_GLOBAL_MEMORY_SIZE];
extern GPerlValue init_values[MAX_INIT_VALUES_SIZE];
extern GPerlMemoryManager *mm;
extern char shared_buf[128];
extern std::string outbuf;
extern char *cwb;
extern GPerlArgsArray *args;
extern GPerlUndef *new_GPerlUndef(void);
extern GPerlArray *new_GPerlInitArray(GPerlValue *list, size_t asize);
extern GPerlObject *new_GPerlArray(GPerlValue v);
extern GPerlString *new_GPerlInitString(char *s, size_t len);
extern GPerlObject *new_GPerlString(GPerlValue v);
extern void Undef_write(GPerlValue );
extern void Array_push(GPerlValue *);
extern void Array_write(GPerlValue );
extern size_t Array_trace(GPerlObject* );
extern size_t String_trace(GPerlObject* );
extern void write_cwb(char *buf);
extern void clear_cwb(void);
extern void *safe_malloc(size_t size);
extern void safe_free(void *ptr, size_t size);
extern GPerlVirtualMachineCode **cur_pc;
extern GPerlTraceRoot root;
extern int init_value_idx;
extern sigjmp_buf expand_mem;
