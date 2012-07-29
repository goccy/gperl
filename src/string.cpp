#include <gperl.hpp>
using namespace std;

GPerlString *new_GPerlInitString(char *s, size_t len)
{
	GPerlObject *freeList = mm->freeList;
	GPerlObject *head = freeList;
	if (freeList->h.next != NULL) {
		mm->freeList = freeList->h.next;
		GPerlString *str = (GPerlString *)head;
		str->h.type = String;
		str->s = s;
		str->len = len;
		return str;
	} else {
		DBG_PL("GC START!!");
	}
	return NULL;
}

GPerlObject *new_GPerlString(GPerlVirtualMachineCode *cur_pc, GPerlValue v)
{
	DBG_PL("new_GPerlString");
	char *s = getString(v);
	size_t len = getLength(v);
	GPerlObject *freeList = mm->freeList;
	GPerlObject *head = freeList;
	if (freeList->h.next != NULL) {
		mm->freeList = freeList->h.next;
		GPerlString *str = (GPerlString *)head;
		str->h.type = String;
		str->s = (char *)safe_malloc(len);
		memcpy(str->s, s, len);
		str->len = len;
		return (GPerlObject *)str;
	} else {
		DBG_PL("GC START!!");
	}
	return NULL;
}
