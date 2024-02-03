#include "file-ops.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void free_File_content(struct File file) {
	free(file.content);
}

void free_FileArray_files(struct FileArray farray) {
	for (int i = 0; i < farray.amount; i++) {
		free_File_content(farray.files[i]);
	}
	free(farray.files);
}

struct File get_file(const char *fname) {
	FILE *fstream = fopen(fname, "r");
	if (fstream == NULL) { // I guess it's fatality
		fprintf(stderr, "%s: ", fname);
		perror(""); // no way for cooking
		exit(-1);
	}

	struct File file;

	int res = fseek(fstream, 0L, SEEK_END);
	if (res != 0) { // it's bad
		perror("fseek() failed");
		file.size = 0;
		file.content = NULL;
		return file;
	}

	file.size = ftell(fstream);
	file.content = (char*)malloc(file.size);

	fseek(fstream, 0L, SEEK_SET);
	int i = 0;
	for (char c = fgetc(fstream);
		c != EOF;
		c = fgetc(fstream)) file.content[i++] = c;

	res = fclose(fstream);
	if (res != 0) { // I guess it's fatality too
		perror("fclose() failed");
		exit(-1);
	}

	return file;
}

struct FileArray get_files(struct StrArray fnames) {
	struct FileArray farray = {
		.files = (struct File*)malloc(fnames.amount * sizeof(struct File)),
		.amount = fnames.amount
	};

	for (int i = 0; i < farray.amount; i++) {
		struct File file = get_file(fnames.strs[i]);
		farray.files[i] = file;
	}

	return farray;
}

size_t File_content_ch_amount(struct File file, char _ch) {
	int amount = 0;
	for (size_t j = 0; j < file.size; j++) {
		char ch = file.content[j];
		if (ch == _ch) amount++;
	}
	return amount;
}

size_t File_content_lamount(struct File file) {
	return File_content_ch_amount(file, '\n');
}

size_t File_content_tamount(struct File file) {
	return File_content_ch_amount(file, '\t');
}

size_t FileArray_content_lamount(struct FileArray farray) { // lamount -> lines amount
	size_t amount = 0;
	for (size_t i = 0; i < farray.amount; i++)
		amount += File_content_lamount(farray.files[i]);

	return amount;
}

size_t FileArray_content_no_nl_end_amount(struct FileArray farray) { // lamount -> lines amount
	size_t amount = 0;
	for (size_t i = 0; i < farray.amount; i++) {
		struct File file = farray.files[i];
		if (file.content[file.size - 1] != '\n')
			amount++;
	}
	return amount;
}

struct File File_dup(struct File file) {
	struct File dup = {
		.content = (char*)malloc(file.size),
		.size = file.size
	};
	char *dest = memcpy(dup.content, file.content, file.size);
	if (dest != dup.content) {
		fprintf(stderr, "memcpy failed\n");
		exit(-1);
	}
	return dup;
}

void File_cpy(struct File f2, struct File f1) {
	char *dest = memcpy(f2.content, f1.content, f1.size);
	if (dest != f2.content) {
		fprintf(stderr, "memcpy failed\n");
		exit(-1);
	}
}

struct FileArray FileArray_dup(struct FileArray farray) {
	struct FileArray dup = {
		.files = (struct File*)malloc(farray.amount * sizeof(struct File)),
		.amount = farray.amount
	};

	for (size_t i = 0; i < farray.amount; i++)
		dup.files[i] = File_dup(farray.files[i]);

	return dup;
}

void print_File(struct File file) {
	for (size_t i = 0; i < file.size; i++) {
		putc(file.content[i], stdout);
	}
}

void print_FileArray(struct FileArray farray) {
	for (size_t i = 0; i < farray.amount; i++) {
		print_File(farray.files[i]);
	}
}

