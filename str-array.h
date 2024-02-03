#ifndef STR_SEQUENCE
#define STR_SEQUENCE

#include <stdio.h>

struct StrArray {
	char **strs;
	size_t amount;
};

void free_StrArray_strs(struct StrArray);
void print_StrArray(struct StrArray); 

#endif // !STR_SEQUENCE
