#include <gperl.hpp>
using namespace std;

static void String_free(GPerlObject *o)
{
    GPerlString *s = (GPerlString *)o;
    safe_free(s->s, s->len);
}

static void String_mark(GPerlObject *o)
{
    o->h.mark_flag = 1;
}

GPerlString *new_GPerlInitString(char *s, size_t len)
{
	GPerlString *str = (GPerlString *)mm->gmalloc();
	str->h.type = String;
	str->s = s;
	str->len = len;
    str->write = NULL;
    str->mark = String_mark;
    str->free = String_free;
	return str;
}

GPerlObject *new_GPerlString(GPerlValue v)
{
	char *s = getRawString(v);
	size_t len = getLength(v);
	GPerlString *str = (GPerlString *)mm->gmalloc();
	str->h.type = String;
	str->s = (char *)safe_malloc(len);
	memcpy(str->s, s, len);
	str->len = len;
    str->write = NULL;
    str->mark = String_mark;
    str->free = String_free;
	return (GPerlObject *)str;
}
