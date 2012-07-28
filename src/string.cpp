#include <gperl.hpp>
using namespace std;

GPerlString *new_GPerlString(char *s, size_t len)
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
