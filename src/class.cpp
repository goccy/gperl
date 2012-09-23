#include <gperl.hpp>

void Class_write(GPerlValue o)
{
	GPerlClass *c = (GPerlClass *)getObject(o);
	char buf[32] = {0};
	sprintf(buf, "%s=HASH(%p)", c->ext->class_name, c->fields);
	write_cwb(buf);
}

void Class_mark(GPerlObject* o)
{
	GPerlClass *c = (GPerlClass *)o;
	c->h.mark_flag = 1;
	//DBG_PL("MARKING");
}

void Class_free(GPerlObject *o)
{
	//DBG_PL("Func_free");
	GPerlClass *c = (GPerlClass *)o;
	(void)c;
	//safe_free((char *)f->fname, strlen(f->fname));
}

void ClassExtend_mark(GPerlObject* o)
{
	GPerlClassExtend *ce = (GPerlClassExtend *)o;
	ce->h.mark_flag = 1;
	//DBG_PL("MARKING");
}

void ClassExtend_free(GPerlObject *o)
{
	//DBG_PL("Func_free");
	GPerlClassExtend *ce = (GPerlClassExtend *)o;
	(void)ce;
	//safe_free((char *)f->fname, strlen(f->fname));
}

GPerlClassExtend *new_GPerlClassExtend(const char *class_name)
{
	GPerlClassExtend *ext = (GPerlClassExtend *)mm->gmalloc();
	ext->class_name = class_name;
	ext->vars = NULL;
	ext->super = NULL;
	ext->h.mark = ClassExtend_mark;
	ext->h.free = ClassExtend_free;
	return ext;
}

GPerlClass *new_GPerlClass(const char *class_name, GPerlFunc **mtds)
{
	GPerlClass *ret = (GPerlClass *)mm->gmalloc();
	ret->fields = NULL;
	ret->mtds = mtds;
	ret->ext = new_GPerlClassExtend(class_name);
	ret->write = Class_write;
	ret->h.mark = Class_mark;
	ret->h.free = Class_free;
	//ret->h.type
	return ret;
}
