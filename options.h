#pragma once

#ifndef options_h_
#define options_h_

#include <stdbool.h>
#include <stdio.h>

#define ARRLEN(arr) (sizeof(arr) / sizeof(*arr))
#define ARRAY_T(type) struct { type *data; size_t count; }

typedef ARRAY_T(char *) char_ptr_array_t;
typedef ARRAY_T(FILE *) FILE_ptr_array_t;

typedef enum {
  NUMBER,
  NUMBER_NONBLANK,
  SQUEEZE_BLANK,
  SHOW_TABS,
  SHOW_ENDS,
  SHOW_NONPRINTING
} Flags;

#define FLAGS_AMOUNT (SHOW_NONPRINTING + 1)

typedef bool flags_t[FLAGS_AMOUNT];

typedef struct {
  bool empty;
  bool success;

  bool flags[FLAGS_AMOUNT];

  FILE_ptr_array_t files;
} Options;

Options parse_args(char_ptr_array_t args);

Options parse_options(int argc, char **argv);

#endif // options_h_
