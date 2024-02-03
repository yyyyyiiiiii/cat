#include "info-opts.h"
#include "opts.h"
#include "str-array.h"

#define SIZE(array) (sizeof((array))/sizeof((array)[0]))

void print_OPT_help() {
	const char *strs[] = {
		"Usage: cat [OPTION]... [FILE]...",
		"Concatenate FILE(s) to standard output.",
		"With no FILE, or when FILE is -, read standard input.",
		"  -A, --show-all           equivalent to -vET",
		"  -b, --number-nonblank    number nonempty output lines, overrides -n",
		"  -e                       equivalent to -vE",
		"  -E, --show-ends          display $ at end of each line",
		"  -n, --number             number all output lines",
		"  -s, --squeeze-blank      suppress repeated empty output lines",
		"  -t                       equivalent to -vT",
		"  -T, --show-tabs          display TAB characters as ^I",
		"  -u                       (ignored)",
		"  -v, --show-nonprinting   use ^ and M- notation, except for LFD and TAB",
		"      --help        display this help and exit",
		"      --version     output version information and exit",
		"",
		"Source of this help page:",
		"https://github.com/coreutils/coreutils/blob/master/src/cat.c"
	};
	struct StrArray sarray = {
		.strs = (char**)strs,
		.amount = SIZE(strs)
	};
	print_StrArray(sarray);
}

void print_OPT_version() {
	const char *strs[] = {
		"cat (yyyyyiiiiii) 1.0",
		" /\\_/\\",
		"( o.o )",
		" > ^ < ",
		"",
		"Source of thsi ascii cat:",
		"https://www.asciiart.eu/animals/cats"
	};
	struct StrArray sarray = {
		.strs = (char**)strs,
		.amount = SIZE(strs)
	};
	print_StrArray(sarray);
}

void print_info_opt(int opt) {
	switch (opt) {
		case OPT_version: {
			print_OPT_version();
		} break;
		case OPT_help: {
			print_OPT_help();
		} break;
		default: {
			fprintf(stderr, "no info OPT with value %d\n", opt);
		}
	};
}

int is_info_opt(int opt) {
	return opt == OPT_version || opt == OPT_help;
}

