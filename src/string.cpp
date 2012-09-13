#include <gperl.hpp>
using namespace std;

static void String_free(GPerlObject *o)
{
	//DBG_PL("String_free");
	GPerlString *s = (GPerlString *)o;
	safe_free(s->s, s->len);
}

static void String_mark(GPerlObject *o)
{
	o->h.mark_flag = 1;
	DBG_PL("MARKING");
}

GPerlString *new_GPerlInitString(char *s, size_t len)
{
	GPerlString *str = (GPerlString *)mm->gmalloc();
	str->h.type = String;
	str->s = s;
	str->len = len;
	str->hash = hash(s, len) % HASH_TABLE_SIZE;
	str->write = NULL;
	str->h.mark = String_mark;
	str->h.free = String_free;
	return str;
}

GPerlObject *new_GPerlString(GPerlValue v, GPerlValue *)
{
	GPerlString *o = getStringObj(v);
	char *s = o->s;
	size_t len = o->len;
	GPerlString *str = (GPerlString *)mm->gmalloc();
	str->h.type = String;
	str->s = (char *)safe_malloc(len);
	memcpy(str->s, s, len);
	str->len = len;
	str->hash = o->hash;
	str->write = NULL;
	str->h.mark = String_mark;
	str->h.free = String_free;
	return (GPerlObject *)str;
}
