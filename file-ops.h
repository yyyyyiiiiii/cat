#ifndef FILE_OPS
#define FILE_OPS

#include "str-array.h"

#include <stdio.h>

struct File {
	char *content;
	size_t size;
};

struct FileArray {
	struct File *files;
	size_t amount;
};

void free_File_content(struct File);
void free_FileArray_files(struct FileArray);

struct File get_file(const char *);
struct FileArray get_files(struct StrArray);

void print_File(struct File);
void print_FileArray(struct FileArray);

size_t File_content_ch_amount(struct File, char);
size_t File_content_tamount(struct File);
size_t File_content_lamount(struct File);
size_t FileArray_content_lamount(struct FileArray); // lamount -> lines amount

size_t FileArray_content_no_nl_end_amount(struct FileArray);

struct File File_dup(struct File);
struct FileArray FileArray_dup(struct FileArray);

void File_cpy(struct File, struct File);

#endif // !FILE_OPS
