#include <gperl.hpp>

GPerlUndef *new_GPerlUndef(GPerlVirtualMachineCode *)
{
    DBG_PL("UNDEF");
    GPerlUndef *o = (GPerlUndef *)mm->gmalloc();
    o->h.type = Undefined;
    o->write = Undef_write;
    return o;
}

void Undef_write(GPerlValue ){}
