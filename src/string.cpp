#include <gperl.hpp>
using namespace std;

GPerlString *new_GPerlString(char *s, size_t len)
{
	GPerlString *str = (GPerlString *)mm->gmalloc(len);
	str->h.type = String;
	str->s = s;
	str->len = len;
	return str;
}
