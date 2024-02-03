#include "opt-array.h"

#include <string.h>
#include <stdlib.h>

size_t get_long_opt(const char *opt) {
	const char *optn = opt + 2;
    if (strcmp(optn, "show-all") == 0) return OPT_A;
    if (strcmp(optn, "number-nonblank") == 0) return OPT_b;
    if (strcmp(optn, "show-ends") == 0) return OPT_E;
    if (strcmp(optn, "number") == 0) return OPT_n;
    if (strcmp(optn, "squeeze-blank") == 0) return OPT_s;
    if (strcmp(optn, "show-tabs") == 0) return OPT_T;
    if (strcmp(optn, "show-nonprinting") == 0) return OPT_v;
    if (strcmp(optn, "help") == 0) return OPT_help;
    if (strcmp(optn, "version") == 0) return OPT_version;

	return OPT_INVALID;
}

size_t get_short_opt(const char optn) {
	switch (optn) {
		case 'A': return OPT_A;
		case 'b': return OPT_b;
		case 'e': return OPT_e;
		case 'E': return OPT_E;
		case 'n': return OPT_n;
		case 's': return OPT_s;
		case 't': return OPT_t;
		case 'T': return OPT_T;
		case 'u': return OPT_u;
		case 'v': return OPT_v;
		default: return OPT_INVALID;
	}
}

void filter_opts(int *all) {
	all[OPT_u] = 0; // ignore
	if (all[OPT_b]) all[OPT_n] = 0;
	if (all[OPT_A]) {
		all[OPT_v] = 1;
		all[OPT_E] = 1;
		all[OPT_T] = 1;
	}
	if (all[OPT_e]) {
		all[OPT_v] = 1;
		all[OPT_E] = 1;
	}
	if (all[OPT_t]) {
		all[OPT_v] = 1;
		all[OPT_T] = 1;
	}
}

int is_opt_container(int opt) {
	switch (opt) {
		case OPT_A: return 1;
		case OPT_e: return 1;
		case OPT_t: return 1;
		default: return 0;
	}
}

int OptArray_opt_pos(struct OptArray opta, int opt) {
	for (size_t i = 0; i < opta.amount; i++)
		if (opta.options[i] == opt) return i;
	return -1;
}

void OptArray_switch_opt_with_first(struct OptArray opta, int opt) {
	int id = OptArray_opt_pos(opta, opt);
	if (id == -1) return;
// shift right and set opt to first
	for (int i = id - 1; i >= 0; i--) {
		opta.options[i + 1] = opta.options[i];
	}
	opta.options[0] = opt;
}

void OptArray_to_right_order(struct OptArray opta) {
	OptArray_switch_opt_with_first(opta, OPT_n);
	OptArray_switch_opt_with_first(opta, OPT_b);
	OptArray_switch_opt_with_first(opta, OPT_s);
}

void invalid_opt(const char* opt) {
	fprintf(stderr, "Invalid option: %s\n", opt);
	exit(-1);
}

struct OptArray get_OptArray(struct StrArray opts) {
	struct OptArray opta = {
		.amount = 0,
		.options = NULL
	};

	if (opts.amount == 0) return opta;

	int all_opts[OPT_AMOUNT] = {0};
	for (size_t i = 0; i < opts.amount; i++) {
		char *popt = opts.strs[i]; // potential opt
		int opt;
		if (popt[1] == '-') {
			opt = get_long_opt(popt);
			if (opt == OPT_INVALID) invalid_opt(popt);
			all_opts[opt] = 1;
			continue;
		}

		for (int j = 1; popt[j] != '\0'; j++) {
			opt = get_short_opt(popt[j]);
			if (opt == OPT_INVALID) invalid_opt(popt);
			all_opts[opt] = 1;
		}
	}

	if (all_opts[OPT_help]) {
		opta = (struct OptArray) {
			.amount = 1,
			.options = (int*)malloc(1 * sizeof(int))
		};
		opta.options[0] = OPT_help;
		return opta;
	}
	if (all_opts[OPT_version]) {
		opta = (struct OptArray) {
			.amount = 1,
			.options = (int*)malloc(1 * sizeof(int))
		};
		opta.options[0] = OPT_version;
		return opta;
	}

	filter_opts(all_opts);
	for (int i = 0; i < OPT_AMOUNT; i++)
		if (all_opts[i] && !is_opt_container(i)) opta.amount++;

	opta.options = (int*)malloc(opta.amount * sizeof(int));
	for (int i = 0, j = 0; i < OPT_AMOUNT; i++)
		if (all_opts[i] && !is_opt_container(i)) opta.options[j++] = i;

	OptArray_to_right_order(opta);

	return opta;
}

void free_OptArray_options(struct OptArray opta) {
	free(opta.options);
}

