#include "greadline.h"
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
} /* extern "C" */
#endif
