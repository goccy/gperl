#include <gperl.hpp>
using namespace std;

GPerlString *new_GPerlInitString(char *s, size_t len)
{
	GPerlString *str = (GPerlString *)mm->gmalloc(len);
	str->h.type = String;
	str->s = s;
	str->len = len;
	str->trace = String_trace;
	return str;
}

GPerlObject *new_GPerlString(GPerlVirtualMachineCode *cur_pc, GPerlValue v)
{
	DBG_PL("new_GPerlString\n");
	char *s = getString(v);
	fprintf(stderr, "new_GPerlString: %s\n", s);
	size_t len = getLength(v);
	GPerlObject *freeList = mm->freeList;
	GPerlObject *head = freeList;

	GPerlString *str = (GPerlString *)mm->gmalloc(len);
	str->h.type = String;
	str->s = (char *)safe_malloc(len);
	memcpy(str->s, s, len);
	str->len = len;
	return (GPerlObject *)str;
}

size_t String_trace(GPerlObject* o)
{
	//fprintf(stderr, "String trace...\n");
	//GPerlString *a = (GPerlObject *)o;
	return 1;
}

