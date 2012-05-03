#include <gperl.hpp>
#define FILENAME "hoge.pl"

GPerl::GPerl(int argc, char **argv)
{
	const char *filename = (argc < 2) ? FILENAME : argv[1];
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
	GPerlTokenizer t;
	std::vector<Token *> *tokens = t.tokenize(script);
	t.annotateTokens(tokens);
	DBG_PL("=============<TOKENIZE>============");
	t.dump(tokens);
	GPerlParser p(tokens);
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
	fprintf(stderr, "%d\n", vm->run(codes));//execute code
	fclose(fp);
}
