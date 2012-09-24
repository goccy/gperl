#include <gperl.hpp>

static void Undef_mark(GPerlObject *o)
{
	o->h.mark_flag = 1;
	//DBG_PL("MARKING");
}

static void Undef_free(GPerlObject *){}

GPerlUndef *new_GPerlUndef(void)
{
	//DBG_PL("UNDEF");
	GPerlUndef *o = (GPerlUndef *)mm->gmalloc();
	o->h.type = Undefined;
	o->write = Undef_write;
	o->h.mark = Undef_mark;
	o->h.free = Undef_free;
	return o;
}

void Undef_write(GPerlValue ){}
