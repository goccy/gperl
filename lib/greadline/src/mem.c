#include "greadline.h"

void *gmalloc(size_t size)
{
	void *ret = malloc(size);
	if (ret == NULL) {
		fprintf(stderr, "MALLOC ERROR!!\n");
		exit(EXIT_FAILURE);
	}
	memset(ret, 0, size);
	return ret;
}
