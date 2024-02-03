#ifndef OPT_ARRAY
#define OPT_ARRAY

#include "opts.h"
#include "str-array.h"

#include <stdio.h>

struct OptArray {
	int *options;
	size_t amount;
};

struct OptArray get_OptArray(struct StrArray);
void free_OptArray_options(struct OptArray);

#endif // !OPT_ARRAY

