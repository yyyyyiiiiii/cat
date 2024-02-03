#ifndef PARSERS
#define PARSERS

#include "opt-array.h"
#include "file-ops.h"

typedef struct FileArray (*OptParser)(struct FileArray);

struct FileArray parser_n(struct FileArray);
struct FileArray parser_b(struct FileArray);
struct FileArray parser_s(struct FileArray);
struct FileArray parser_E(struct FileArray);
struct FileArray parser_T(struct FileArray);
struct FileArray parser_v(struct FileArray);
struct FileArray parse(struct FileArray, struct OptArray);

#endif // !PARSERS

