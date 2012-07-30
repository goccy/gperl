#include <gperl.hpp>
using namespace std;

static void String_free(GPerlObject *o)
{
    GPerlString *s = (GPerlString *)o;
    safe_free(s->s, s->len);
}

GPerlString *new_GPerlInitString(char *s, size_t len)
{
    DBG_PL("new_GPerlInitString");
	GPerlString *str = (GPerlString *)mm->gmalloc();
	str->h.type = String;
	str->s = s;
	str->len = len;
    str->write = NULL;
    str->mark = NULL;
    str->free = String_free;
	return str;
}

GPerlObject *new_GPerlString(GPerlVirtualMachineCode *, GPerlValue v)
{
	DBG_PL("new_GPerlString");
	char *s = getRawString(v);
	size_t len = getLength(v);
	GPerlString *str = (GPerlString *)mm->gmalloc();
	str->h.type = String;
	str->s = (char *)safe_malloc(len);
	memcpy(str->s, s, len);
	str->len = len;
    str->write = NULL;
    str->mark = NULL;
    str->free = String_free;
	return (GPerlObject *)str;
}
