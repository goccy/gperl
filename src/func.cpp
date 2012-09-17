#include <gperl.hpp>

void Func_write(GPerlValue o)
{
	GPerlFunc *f = (GPerlFunc *)getObject(o);
	if (f->h.type == CodeRef) {
		char buf[32] = {0};
		sprintf(buf, "CODE(%p)", f->code);
		write_cwb(buf);
	}
}

void Func_mark(GPerlObject* o)
{
	GPerlFunc *f = (GPerlFunc *)o;
	f->h.mark_flag = 1;
	//DBG_PL("MARKING");
}

void Func_free(GPerlObject *o)
{
	//DBG_PL("Func_free");
	GPerlFunc *f = (GPerlFunc *)o;
	safe_free((char *)f->fname, strlen(f->fname));
}

GPerlObject *new_GPerlFunc(const char *fname, GPerlVirtualMachineCode *code)
{
	GPerlFunc *ret = (GPerlFunc *)mm->gmalloc();
	ret->fname = fname;
	ret->code = code;
	ret->write = Func_write;
	ret->h.mark = Func_mark;
	ret->h.free = Func_free;
	ret->h.type = CodeRef;
	return (GPerlObject *)ret;
}
