#include <gperl.hpp>
using namespace std;

#define IS_UNBOX(v) TYPE_CHECK(v) < 2

/**
 * An implementation of the djb2 hash function by Dan Bernstein.
 */
unsigned long hash(char* _str, size_t len)
{
	char* str = _str;
	unsigned long hash = 5381;
	while (len--) {
		/* hash * 33 + c */
		hash = ((hash << 5) + hash) + *str++;
	}
	return hash;
}

void Hash_write(GPerlValue o)
{
	GPerlHash *h = (GPerlHash *)getObject(o);
	if (h->h.type == HashRef) {
		char buf[32] = {0};
		sprintf(buf, "HASH(%p)", h->table);
		write_cwb(buf);
	} else {
		size_t key_n = h->size / 2;
		for (size_t i = 0; i < key_n; i++) {
			GPerlString *key = h->keys[i];
			write_cwb(key->s);
			GPerlValue value = h->table[key->hash];
			char buf[32] = {0};
			switch (TYPE_CHECK(value)) {
			case 0: // Double
				sprintf(buf, "%f", value.dvalue);
				write_cwb(buf);
				break;
			case 1: {//Int
				sprintf(buf, "%d", value.ivalue);
				write_cwb(buf);
				break;
			}
			case 2: {//String
				write_cwb(getRawString(value));
				break;
			}
			default: // Object
				break;
			}
		}
	}
}

void Hash_mark(GPerlObject* o)
{
	GPerlHash *h = (GPerlHash *)o;
	h->h.mark_flag = 1;
	//DBG_PL("MARKING");
	size_t key_n = h->size / 2;
	for (size_t i = 0; i < key_n; i++) {
		GPerlString *key = h->keys[i];
		key->h.mark_flag = 1;
		GPerlValue value = h->table[key->hash];
		switch (TYPE_CHECK(value)) {
		case 2:
			//DBG_PL("MARKING");
			((GPerlString *)getStringObj(value))->h.mark_flag = 1;
			break;
		case 3: {
			GPerlObject *o = (GPerlObject *)getObject(value);
			if (!o->h.mark_flag) o->h.mark(o);
			break;
		}
		default:
			break;
		}
	}
}

void Hash_free(GPerlObject *o)
{
	//DBG_PL("Hash_free");
	GPerlHash *h = (GPerlHash *)o;
	h->size = 0;
	GPerlValue *table = h->table;
	safe_free(table, sizeof(GPerlValue) * h->size);
	GPerlString **keys = h->keys;
	safe_free(keys, PTR_SIZE);
}

GPerlHash *new_GPerlInitHash(GPerlValue *, size_t hsize)
{
	GPerlHash *h = (GPerlHash *)mm->gmalloc();
	h->h.type = Hash;
	size_t size = sizeof(GPerlValue) * HASH_TABLE_SIZE;
	GPerlValue *table = (GPerlValue *)safe_malloc(size);
	for (int i = 0; i < HASH_TABLE_SIZE; i++) {
		OBJECT_init(table[i], undef);
	}
	h->table = table;
	h->size = hsize;
	h->write = Hash_write;
	h->h.mark = Hash_mark;
	h->h.free = Hash_free;
	return h;
}

GPerlObject *new_GPerlHash(GPerlValue v, GPerlValue *args)
{
	GPerlHash *h = (GPerlHash *)getObject(v);
	size_t hsize = h->size;
	h->keys = (GPerlString **)safe_malloc(PTR_SIZE * hsize / 2);
	int key_n = 0;
	for (size_t i = 0; i < hsize; i += 2, key_n++) {
		GPerlString *key = getStringObj(args[i]);
		h->table[key->hash] = args[i + 1];
		h->keys[key_n] = key;
	}
	GPerlHash *ret = (GPerlHash *)mm->gmalloc();
	size_t size = sizeof(GPerlValue) * HASH_TABLE_SIZE;
	ret->table = (GPerlValue *)safe_malloc(size);
	memcpy(ret->table, h->table, size);
	ret->size = h->size;
	ret->keys = h->keys;
	ret->write = Hash_write;
	ret->h.mark = Hash_mark;
	ret->h.free = Hash_free;
	ret->h.type = h->h.type;
	return (GPerlObject *)ret;
}

GPerlHash *GPerlHash_copy(GPerlHash *h)
{
	GPerlHash *ret = (GPerlHash *)mm->gmalloc();
	size_t size = sizeof(GPerlValue) * HASH_TABLE_SIZE;
	ret->table = (GPerlValue *)safe_malloc(size);
	memcpy(ret->table, h->table, size);
	ret->size = h->size;
	ret->keys = h->keys;
	ret->h.type = h->h.type;
	ret->write = Hash_write;
	ret->h.mark = Hash_mark;
	ret->h.free = Hash_free;
	return ret;
}
