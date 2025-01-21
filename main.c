#include "options.h"

typedef struct {
  size_t index;
  FILE_ptr_array_t array;
} FILE_ptr_array_ctx_t;

bool FILE_ptr_array_feof(FILE_ptr_array_ctx_t *ctx) {
  const size_t count = ctx->array.count;
  while (ctx->index + 1 < count && feof(ctx->array.data[ctx->index]))
    ctx->index++;
  return !(ctx->index < count) || feof(ctx->array.data[ctx->index]);
}

char FILE_ptr_array_get(FILE_ptr_array_ctx_t *ctx) {
  const size_t count = ctx->array.count;
  if (ctx->index >= count)
    return EOF;
  FILE* file = ctx->array.data[ctx->index];
  char ch = fgetc(file);
  while (feof(file) && ctx->index + 1 < count) {
    ctx->index++;
    file = ctx->array.data[ctx->index];
    ch = fgetc(file);
  }
  return ch;
}

void FILE_ptr_array_unget(FILE_ptr_array_ctx_t *ctx, char ch) {
  if (ctx->index >= ctx->array.count)
    return;
  FILE* file = ctx->array.data[ctx->index];
  ungetc(ch, file);
}

char FILE_ptr_array_next(FILE_ptr_array_ctx_t *ctx) {
  char ch = FILE_ptr_array_get(ctx);
  FILE_ptr_array_unget(ctx, ch);
  return ch;
}

typedef struct {
  size_t index;
  FILE_ptr_array_ctx_t *file_ptr_array_ctx;
} number_ctx_t;

void number(number_ctx_t* ctx, char current) {
  if (current == '\n' && FILE_ptr_array_next(ctx->file_ptr_array_ctx) != EOF)
    fprintf(stdout, "%6d\t", ++ctx->index);
}

typedef struct {
  size_t index;
  FILE_ptr_array_ctx_t *file_ptr_array_ctx;
} number_nonblank_ctx_t;

void number_nonblank(number_nonblank_ctx_t* ctx, char current) {
  char next = FILE_ptr_array_next(ctx->file_ptr_array_ctx);
  if (current == '\n' && next != '\n' && next != EOF)
    fprintf(stdout, "%6d\t", ++ctx->index);
}

typedef void* show_tabs_ctx_t;

void show_tabs(show_tabs_ctx_t* ctx, char current) {
  if (current == '\t')
    fputs("^I", stdout);
}

typedef void* show_ends_ctx_t;

void show_ends(show_ends_ctx_t* ctx, char current) {
  if (current == '\n')
    fputs("$\n", stdout);
}

typedef void* show_nonprinting_ctx_t;

void show_nonprinting(show_nonprinting_ctx_t* ctx, char current) {
  const unsigned char value = current;

  if (value == '\n'
      || value == '\t'
      || (value >= 32 && value <= 126))
    return;

  fputs((value < 32
         ? (char []){'^', current + '@', '\0'}
         : value == 127
         ? "^?"
         : value < 128 + 32
         ? (char []){'M', '-', '^', value - 128 + '@', '\0'}
         : value < 128 + 127
         ? (char []){'M', '-', value - 128, '\0'}
         : "M-^?"), stdout);
}

typedef struct {
  bool show_nonprinting;
  bool show_tabs;
  bool show_ends;
} print_ctx_t;

void print(print_ctx_t *ctx, char current) {
  const unsigned char value = current;
  const bool show_nonprinting = ctx->show_nonprinting;
  const bool show_tabs = ctx->show_tabs;
  const bool show_ends = ctx->show_ends;

  if (((value <= 31 && value >= 0)
       && ((current == '\t' && !show_tabs)
           || (current == '\n' && !show_ends)
           || (current != '\t' && current != '\n' && !show_nonprinting)))
      || (value >= 127 && !show_nonprinting)
      || (value >= 32 && value < 127))
      fputc(current, stdout);
}

typedef struct {
  FILE_ptr_array_ctx_t *file_ptr_array_ctx;
} squeeze_blank_ctx_t;

void squeeze_blank(squeeze_blank_ctx_t *ctx, char current) {
  if (current != '\n')
    return;

  const bool was = FILE_ptr_array_next(ctx->file_ptr_array_ctx) == '\n';
  while (FILE_ptr_array_next(ctx->file_ptr_array_ctx) == '\n')
    FILE_ptr_array_get(ctx->file_ptr_array_ctx);
  if (was)
    FILE_ptr_array_unget(ctx->file_ptr_array_ctx, '\n');
}

typedef void(*process_fn_t)(void *, char);

#define PROCESS_FN(fn) fn ## process_fn
#define DEF_PROCESS_FN(fn) void PROCESS_FN(fn) (void* ctx, char current) { fn(ctx, current); }

DEF_PROCESS_FN(number);
DEF_PROCESS_FN(number_nonblank);
DEF_PROCESS_FN(squeeze_blank);
DEF_PROCESS_FN(show_nonprinting);
DEF_PROCESS_FN(show_tabs);
DEF_PROCESS_FN(show_ends);
DEF_PROCESS_FN(print);

typedef struct {
  void *ctx;
  process_fn_t fn;
} process_ctx_t;

int main(int argc, char **argv) {
  Options opts = parse_options(argc, argv);

  if (!opts.success)
    return -1;

  if (opts.empty)
    return 0;

  FILE_ptr_array_ctx_t ctx = {.array = opts.files, .index = 0};
  print_ctx_t print_ctx = {.show_nonprinting = opts.flags[SHOW_NONPRINTING],
                           .show_tabs = opts.flags[SHOW_TABS],
                           .show_ends = opts.flags[SHOW_ENDS]};
  number_ctx_t number_ctx = {.index = 0, .file_ptr_array_ctx = &ctx};
  number_nonblank_ctx_t number_nonblank_ctx = {.index = 0, .file_ptr_array_ctx = &ctx};
  squeeze_blank_ctx_t squeeze_blank_ctx = {.file_ptr_array_ctx = &ctx};
  show_nonprinting_ctx_t show_nonprinting_ctx;
  show_tabs_ctx_t show_tabs_ctx;
  show_ends_ctx_t show_ends_ctx;

  process_ctx_t processes[FLAGS_AMOUNT];
  size_t count = 0;

  processes[count++] = (process_ctx_t){.ctx = &print_ctx, .fn = PROCESS_FN(print)};

  if (opts.flags[SQUEEZE_BLANK]) {
    processes[count++] = (process_ctx_t){.ctx = &squeeze_blank_ctx, .fn = PROCESS_FN(squeeze_blank)};
    PROCESS_FN(squeeze_blank)(&squeeze_blank_ctx, '\n');
  }

  if (opts.flags[SHOW_TABS])
    processes[count++] = (process_ctx_t){.ctx = &show_tabs_ctx, .fn = PROCESS_FN(show_tabs)};

  if (opts.flags[SHOW_ENDS])
    processes[count++] = (process_ctx_t){.ctx = &show_ends_ctx, .fn = PROCESS_FN(show_ends)};

  if (opts.flags[SHOW_NONPRINTING])
    processes[count++] = (process_ctx_t){.ctx = &show_nonprinting_ctx, .fn = PROCESS_FN(show_nonprinting)};

  if (opts.flags[NUMBER] && !opts.flags[NUMBER_NONBLANK]) {
    processes[count++] = (process_ctx_t){.ctx = &number_ctx, .fn = PROCESS_FN(number)};
    PROCESS_FN(number)(&number_ctx, '\n');
  }

  if (opts.flags[NUMBER_NONBLANK]) {
    processes[count++] = (process_ctx_t){.ctx = &number_nonblank_ctx, .fn = PROCESS_FN(number_nonblank)};
    PROCESS_FN(number_nonblank)(&number_nonblank_ctx, '\n');
  }

  char ch;
  while (ch = FILE_ptr_array_get(&ctx), !FILE_ptr_array_feof(&ctx)) {
    for (size_t i = 0; i < count; i++) {
      void *ctx = processes[i].ctx;
      process_fn_t fn = processes[i].fn;
      fn(ctx, ch);
    }
  }

  return 0;
}
