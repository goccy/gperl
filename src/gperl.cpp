#include <gperl.hpp>
using namespace std;

GPerlMemoryManager *mm;

GPerl::GPerl(int argc, char **argv)
{
	if (argc < 2) exit(1);
	const char *filename = argv[1];
	char line[MAX_LINE_SIZE] = {0};
	char script_[MAX_SCRIPT_SIZE] = {0};
	char *tmp = script_;
	char *script = script_;
	FILE *fp = fopen(filename, "r");
	while (fgets(line, MAX_LINE_SIZE, fp) != NULL) {
		//DBG_PL("line = [%s]", line);
		int line_size = strlen(line);
		snprintf(tmp, line_size + 1, "%s\n", line);
		tmp += line_size;
	}
	cwb = (char *)malloc(MAX_CWB_SIZE);
	memset(cwb, 0, MAX_CWB_SIZE);
	mm = new GPerlMemoryManager();
	GPerlTokenizer t;
	std::vector<GPerlToken *> *tokens = t.tokenize(script);
	t.annotateTokens(tokens);
	DBG_PL("=============<TOKENIZE>============");
	t.dump(tokens);
	GPerlParser p(tokens, argc - 2, argv + 2);
	DBG_PL("==============<PARSE>==============");
	GPerlAST *ast = p.parse();
#ifdef USING_GRAPH_DEBUG
	ast->show();//graph debug with graphviz
#endif
	GPerlCompiler compiler;
	DBG_PL("=============<COMPILE>=============");
	GPerlVirtualMachineCode *codes = compiler.compile(ast);
	DBG_PL("-----------<DUMP VMCODE>-----------");
	compiler.dumpPureVMCode(codes);
	DBG_PL("-----------------------------------");
	GPerlVirtualMachine *vm = new GPerlVirtualMachine();
	DBG_PL("=============<RUNTIME>=============");
	vm->run(codes);//create threading code
	vm->run(codes);//execute code
	//fprintf(stderr, "%d\n", vm->run(codes));//execute code
	fclose(fp);
}
