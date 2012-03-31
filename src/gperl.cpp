#include <gperl.hpp>
#define FILENAME "hoge.pl"

GPerl::GPerl(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	const char *filename = FILENAME;
	char line[MAX_LINE_SIZE] = {0};
	char script_[MAX_SCRIPT_SIZE] = {0};
	char *tmp = script_;
	char *script = script_;
	FILE *fp = fopen(filename, "r");
	while (fgets(line, MAX_LINE_SIZE, fp) != NULL) {
		fprintf(stderr, "line = [%s]\n", line);
		int line_size = strlen(line);
		snprintf(tmp, line_size + 1, "%s\n", line);
		tmp += line_size;
	}
	GPerlTokenizer t;
	std::vector<Token *> *tokens = t.tokenize(script);
	t.annotateTokens(tokens);
	fprintf(stderr, "============<TOKENIZE>===========\n");
	t.dump(tokens);
	GPerlParser p;
	fprintf(stderr, "=============<PARSE>=============\n");
	GPerlAST *ast = p.parse(tokens);
	ast->show();//graph debug with graphviz
	GPerlCompiler compiler;
	GPerlVirtualMachineCode *codes = compiler.compile(ast);
	fclose(fp);
}
