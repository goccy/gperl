#include <gperl.hpp>

GPerlUndef *new_GPerlUndef(GPerlVirtualMachineCode *cur_pc)
{
	GPerlObject *freeList = mm->freeList;
	GPerlObject *head = freeList;
	if (freeList->h.next != NULL) {
		mm->freeList = freeList->h.next;
		GPerlUndef *o = (GPerlUndef *)head;
		o->h.type = Undefined;
        o->write = Undef_write;
		return o;
	} else {
		DBG_PL("GC START!!");
	}
	return NULL;
}

void Undef_write(GPerlValue ){}
