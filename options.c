#include "options.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static const char *kHelp =
  "Source from where i stole this help: https://www.gnu.org/software/coreutils/cat\n"
  "Usage: cat [OPTION]... [FILE]...\n"
  "Concatenate FILE(s) to standard output.\n"
  "With no FILE, or when FILE is -, read standard input.\n"
  "\n"
  "  -A, --show-all           equivalent to -vET\n"
  "  -b, --number-nonblank    number nonempty output lines, overrides -n\n"
  "  -e                       equivalent to -vE\n"
  "  -E, --show-ends          display $ at end of each line\n"
  "  -n, --number             number all output lines\n"
  "  -s, --squeeze-blank      suppress repeated empty output lines\n"
  "  -t                       equivalent to -vT\n"
  "  -T, --show-tabs          display TAB characters as ^I\n"
  "  -u                       (ignored)\n"
  "  -v, --show-nonprinting   use ^ and M- notation, except for LFD and TAB\n"
  "      --help        display this help and exit\n"
  "      --version     output version information and exit\n"
  "Examples:\n"
  "  cat f - g  Output f's contents, then standard input, then g's contents.\n"
  "  cat        Copy standard input to standard output.\n";

static const char *kVersion =
  "cat v2\n"
  "License MIT.\n"
  "This is free software: you are free to change and redistribute it.\n"
  "There is NO WARRANTY, to the extent permitted by law.\n";

#define END_OF_FLAGS(arg) (strcmp(arg, "--") == 0)
#define IS_FLAG(flag) (flag[0] == '-' && flag[1] != '\0')
#define IS_STDIN(arg) (arg[0] == '-' && arg[1] == '\0')
#define IS_AT_LEAST_ONE_SHORT_FLAG(flag) (IS_FLAG(flag) && flag[1] != '-')
#define IS_SHORT_FLAG(flag) (IS_AT_LEAST_ONE_SHORT_FLAG(flag) && flag[2] == '\0')
#define IS_LONG_FLAG(flag) (strstr(flag, "--") == flag && flag[2] != '\0')

void log_invalid_flag(const char *arg) {
  fprintf(stderr,
          "cat: invalid option -- %s\n"
          "Try 'cat --help' for more information.\n",
          arg);
}

FILE_ptr_array_t get_files(char_ptr_array_t args) {
  FILE **files = NULL;
  int count = 0;
  bool only_files = false;
  for (size_t i = 0; i < args.count; i++) {
    char * const arg = args.data[i];
    if (!only_files && IS_FLAG(arg)) {
      only_files = END_OF_FLAGS(args.data[i]);
      continue;
    }

    files = realloc(files, sizeof(FILE*) * (++count));
    files[count - 1] = IS_STDIN(arg) ? stdin : fopen(arg, "rb");
  }

  return (FILE_ptr_array_t){.data = files, .count = count};
}


bool have_short_flag(char_ptr_array_t args, char *flag) {
  assert(IS_SHORT_FLAG(flag));
  for (size_t i = 0; i < args.count && !END_OF_FLAGS(args.data[i]); i++) {
    char * const arg = args.data[i];
    if (!IS_AT_LEAST_ONE_SHORT_FLAG(arg))
      continue;

    const char flag_ch = flag[1];

    for (size_t j = 1; arg[j] != '\0'; j++)
      if (arg[j] == flag_ch)
        return true;
  }

  return false;
}

bool have_one_of_short_flags(char_ptr_array_t args, char_ptr_array_t flags) {
  for (size_t i = 0; i < flags.count; i++)
    if (have_short_flag(args, flags.data[i]))
      return true;
  return false;
}

bool have_long_flag(char_ptr_array_t args, char *flag) {
  assert(IS_LONG_FLAG(flag));
  for (size_t i = 0; i < args.count && !END_OF_FLAGS(args.data[i]); i++) {
    char * const arg = args.data[i];
    if (IS_LONG_FLAG(arg)
        && strcmp(arg, flag) == 0)
      return true;
  }

  return false;
}

bool have_one_of_long_flags(char_ptr_array_t args, char_ptr_array_t flags) {
  for (size_t i = 0; i < flags.count; i++)
    if (have_long_flag(args, flags.data[i]))
      return true;
  return false;
}

bool have_at_least_part_of_long_flag(char_ptr_array_t args, char *flag) {
  assert(IS_LONG_FLAG(flag));
  for (size_t i = 0; i < args.count && !END_OF_FLAGS(args.data[i]); i++) {
    char * const arg = args.data[i];
    if (IS_LONG_FLAG(arg)
        && strstr(flag, arg) == flag)
      return true;
  }

  return false;
}

bool have_message_flags(char_ptr_array_t args) {
  struct { char *flag; const char *message; } flags[] = {
    {"--help", kHelp},
    {"--version", kVersion}
  };

  for (int i = 0; i < ARRLEN(flags); i++) {
    if (!have_at_least_part_of_long_flag(args, flags[i].flag))
      continue;
    printf(flags[i].message);
    return true;
  }

  return false;
}

bool bad_file(char *path) {
  FILE *file = fopen(path, "rb");
  if (file)
    fclose(file);
  else
    fprintf(stderr, "cat: %s: No such file or directory\n", path);
  return !file;
}

typedef bool(*have_flag_fn_t)(char_ptr_array_t, char*);

bool have_flag(char_ptr_array_t args, char *flag) {
  have_flag_fn_t have_flag = IS_SHORT_FLAG(flag) ? have_short_flag : have_long_flag;
  return have_flag(args, flag);
}

bool have_at_least_one_flag(char_ptr_array_t args, char_ptr_array_t flags) {
  for (size_t i = 0; i < flags.count; i++)
    if (have_flag(args, flags.data[i]))
      return true;
  return false;
}

bool have_at_least_one_flagv(char_ptr_array_t args, char **flags, size_t count) {
  return have_at_least_one_flag(args, (char_ptr_array_t){.data = flags, .count = count});
}

#define HAVE_AT_LEAST_ONE_FLAG(args, flags) have_at_least_one_flagv(args, flags, ARRLEN(flags))

bool bad_short_flag(char *arg) {
  char flags[] = {
    'A',
    'b',
    'e',
    'E',
    'n',
    's',
    't',
    'T',
    'u',
    'v'
  };

  for (int i = 1; arg[i] != '\0'; i++) {
    bool found = false;
    for (int j = 0; j < ARRLEN(flags); j++) {
      if (flags[j] != arg[i])
        continue;
      found = true;
      break;
    }

    if (!found) {
      log_invalid_flag(arg);
      return true;
    }
  }

  return false;
}

bool bad_long_flag(char *arg) {
  char *similar[] = {"--help", "--version"};
  char *precise[] = {
    "--show-all",
    "--number-nonblank",
    "--show-ends",
    "--number",
    "--squeeze-blank",
    "--show-tabs",
    "--show-nonprinting"
  };

  bool bad = !(have_at_least_part_of_long_flag((char_ptr_array_t){
        .data = similar,
        .count = ARRLEN(similar)
      }, arg)
    || have_flag((char_ptr_array_t){
        .data = precise,
        .count = ARRLEN(precise)
      }, arg));

  if (bad)
    log_invalid_flag(arg);

  return bad;
}

typedef bool(*check_if_bad_fn_t)(char *);

bool bad_args(char_ptr_array_t args) {
  bool only_files = false;

  for (size_t i = 0; i < args.count; i++) {
    char * const arg = args.data[i];

    if (END_OF_FLAGS(arg)) {
      only_files = true;
      continue;
    }

    if (IS_STDIN(arg))
      continue;

    check_if_bad_fn_t bad_arg = (only_files || arg[0] != '-'
                                 ? bad_file
                                 : IS_LONG_FLAG(arg)
                                 ? bad_long_flag
                                 : bad_short_flag);

    if (bad_arg(arg))
      return true;
  }

  return false;
}

Options parse_args(char_ptr_array_t args) {
  Options options = {.empty = true, .success = true};
  if (have_message_flags(args))
    return options;

  if (bad_args(args)) {
    options.success = false;
    return options;
  }

  options.files = get_files(args);
  options.flags[NUMBER] = HAVE_AT_LEAST_ONE_FLAG(args, ((char* []){"-n", "--number"}));
  options.flags[NUMBER_NONBLANK] = HAVE_AT_LEAST_ONE_FLAG(args, ((char* []){"-b", "--number-nonblank"}));
  options.flags[SQUEEZE_BLANK] = HAVE_AT_LEAST_ONE_FLAG(args, ((char* []){"-s", "--squeeze-blank"}));
  options.flags[SHOW_TABS] = HAVE_AT_LEAST_ONE_FLAG(args, ((char* []){"-A", "-t", "-T", "--show-all", "--show-tabs"}));
  options.flags[SHOW_ENDS] = HAVE_AT_LEAST_ONE_FLAG(args, ((char* []){"-A", "-e", "-E", "--show-all", "--show-ends"}));
  options.flags[SHOW_NONPRINTING] = HAVE_AT_LEAST_ONE_FLAG(args, ((char* []){"-A", "-e", "-t", "-v", "--show-all", "--show-nonprinting"}));

  if (options.files.count == 0) {
    options.files.data = realloc(options.files.data, sizeof(FILE*));
    options.files.data[options.files.count++] = stdin;
  }

  options.empty = false;

  return options;
}

Options parse_options(int argc, char **argv) {
  return parse_args((char_ptr_array_t){.data = argv + 1, .count = argc - 1});
}
