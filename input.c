#include "input.h"
#include "str-array.h"

#include <string.h>
#include <stdlib.h>

static size_t contain_double_hyphen = 0;

int files_amount(struct StrArray unparsed) {
	size_t amount = 0;
	for (int i = 0; i < unparsed.amount; i++) {
		const char *pfile = unparsed.strs[i]; // pfile -> potential file

		if (pfile[0] != '-') {
			amount++;
			continue;
		}

		if (strcmp(pfile, "--") == 0) {
			contain_double_hyphen = 1;
			amount += unparsed.amount - i - 1;
			break;
		}
	}
	return amount;
}

void get_fnames(struct StrArray unparsed, struct StrArray fnames) {
	for (int i = 0, is100 = 0, id = 0; i < unparsed.amount; i++) {
		char *pfile = unparsed.strs[i];
		if (is100 || pfile[0] != '-') {
			fnames.strs[id++] = strdup(pfile);
			continue;
		}

		if (strcmp(pfile, "--") == 0)
			is100 = 1;
    }
}

void get_Input_opts(struct StrArray unparsed, struct StrArray opts) {
	for (int i = 0, id = 0; i < unparsed.amount; i++) {
		char *popt = unparsed.strs[i];
		if (strcmp(popt, "--") == 0)
			break;
		if (popt[0] == '-')
			opts.strs[id++] = strdup(popt);
	}
}

struct Input get_input(struct StrArray unparsed) {
    struct Input input;

	size_t famount = files_amount(unparsed);
	if (famount == 0) {
		input.files.amount = 0;
		input.files.strs = NULL;
		input.opts.amount = unparsed.amount - contain_double_hyphen;
	} else {
		input = (struct Input) {
			.files.amount = famount,
			.files.strs = (char**)malloc(famount * sizeof(char*)),
			.opts.amount = unparsed.amount - famount - contain_double_hyphen
		};
		get_fnames(unparsed, input.files);
	}

	if (input.opts.amount) {
		input.opts.strs = (char**)malloc(input.opts.amount * sizeof(char*));
		get_Input_opts(unparsed, input.opts);
	} else {
		input.opts.strs = NULL;
	}

	return input;
}

void free_Input_members(struct Input input) {
	free_StrArray_strs(input.files);
	free_StrArray_strs(input.opts);
}
