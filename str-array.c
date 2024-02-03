#include "str-array.h"

#include <stdlib.h>

void print_StrArray(struct StrArray sarray) {
	for (size_t i = 0; i < sarray.amount; i++) {
		int res = puts(sarray.strs[i]);
		if (res < 0) {
			fprintf(stderr, "puts failed\n");
			exit(-1);
		}
	}
}

void free_StrArray_strs(struct StrArray sarray) {
	if (sarray.amount == 0) return;

	for (size_t i = 0; i < sarray.amount; i++) {
		free(sarray.strs[i]);
	}
	free(sarray.strs);
	sarray.strs = NULL;
}

