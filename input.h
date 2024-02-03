#ifndef INPUT
#define INPUT

#include "str-array.h"

struct Input {
	struct StrArray files;
	struct StrArray opts;
};

struct Input get_input(struct StrArray);
void free_Input_members(struct Input);

#endif // !INPUT

