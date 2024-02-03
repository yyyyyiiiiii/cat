#include "parsers.h"
#include "file-ops.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

int ln_size(int lamount) { // ln -> line number
	int size = log10(lamount) + 1;
	int padd = 6 - (size % 6);
	size += padd;
	return size;
}

void set_ln(char *line, size_t ln, size_t lnsize) { // ln -> line number
	memset(line, ' ', lnsize);
	sprintf(line, "%lu", ln);
	int numci = 0; // num character index

	for (int i = lnsize - 1; i >= 0; i--) {
		if (line[i] != ' ') {
		    if (line[i] == '\0') {
				line[i] = ' ';
		    continue;
		    }
		    numci = i;
		    break;
		}
	}

	int numcpi = lnsize - 1; // num character put index
	while (numci >= 0) {
		line[numcpi] = line[numci];
		line[numci] = ' ';
		numcpi--;
		numci--;
	}
}

void set_space(char *buff) {
	buff[0] = ' ';
	buff[1] = ' ';
}

struct FileArray parser_n(struct FileArray farray) {
	size_t lamount = FileArray_content_lamount(farray);
	size_t no_nl_end_amount = FileArray_content_no_nl_end_amount(farray);

	struct FileArray pfarray = {
		.amount = (lamount + no_nl_end_amount)
	};
	pfarray.files = (struct File*)malloc(pfarray.amount * sizeof(struct File));

	size_t lnsize = ln_size(lamount);
	size_t ln = 1;
	size_t space_size = 2;

	if (lamount == 0) {
		char *content = (char*)malloc(lnsize + space_size + farray.files[0].size);
		set_ln(content, ln, lnsize);
		set_space(content + space_size);

		struct File ffile = {
			.content = content + lnsize,
			.size = farray.files[0].size
		};
		File_cpy(ffile, farray.files[0]);
		ffile.content = content;
		pfarray.files[0] = ffile;

		for (size_t i = 1; i < farray.amount; i++)
			pfarray.files[i] = File_dup(farray.files[i]);
		return pfarray;
	}

	size_t pfarray_file_id = 0;
	for (size_t i = 0; i < farray.amount; i++) {
		struct File file = farray.files[i];
		size_t prev_nl_i = -1;
		for (size_t j = 0; j < file.size; j++) {
			if (file.content[j] != '\n') continue;

			size_t buffsize = j - prev_nl_i;
			size_t size = buffsize + lnsize + space_size;
			char *content = (char*)malloc(size);
			set_ln(content, ln++, lnsize);
			set_space(content + lnsize);

			char *dest = memcpy(content + lnsize + space_size, file.content + prev_nl_i + 1, buffsize);
			if (dest != content + lnsize + space_size) {
				fprintf(stderr, "memcpy failed\n");
				exit(-1);
			}

			pfarray.files[pfarray_file_id++] = (struct File) {
				.content = content,
				.size = size
			};
			prev_nl_i = j;
		}

		if (prev_nl_i != file.size - 1) {
			size_t j = file.size - 1;

			size_t buffsize = j - prev_nl_i;
			size_t size = buffsize + lnsize + space_size;
			char *content = (char*)malloc(size);
			set_ln(content, ln++, lnsize);
			set_space(content + lnsize);

			char *dest = memcpy(content + lnsize + space_size, file.content + prev_nl_i + 1, buffsize);
			if (dest != content + lnsize + space_size) {
				fprintf(stderr, "memcpy failed\n");
				exit(-1);
			}

			pfarray.files[pfarray_file_id++] = (struct File) {
				.content = content,
				.size = size
			};
		}
	}

	return pfarray;
}

struct FileArray parser_b(struct FileArray farray) {
	size_t lamount = FileArray_content_lamount(farray);
	size_t no_nl_end_amount = FileArray_content_no_nl_end_amount(farray);

	struct FileArray pfarray = {
		.amount = (lamount + no_nl_end_amount)
	};
	pfarray.files = (struct File*)malloc(pfarray.amount * sizeof(struct File));

	size_t lnsize = ln_size(lamount);
	size_t ln = 1;
	size_t space_size = 2;

	if (lamount == 0) {
		char *content = (char*)malloc(lnsize + space_size + farray.files[0].size);
		set_ln(content, ln, lnsize);
		set_space(content + space_size);

		struct File ffile = {
			.content = content + lnsize,
			.size = farray.files[0].size
		};
		File_cpy(ffile, farray.files[0]);
		ffile.content = content;
		pfarray.files[0] = ffile;

		for (size_t i = 1; i < farray.amount; i++)
			pfarray.files[i] = File_dup(farray.files[i]);
		return pfarray;
	}

	size_t pfarray_file_id = 0;
	for (size_t i = 0; i < farray.amount; i++) {
		struct File file = farray.files[i];
		size_t prev_nl_i = -1;
		for (size_t j = 0; j < file.size; j++) {
			if (file.content[j] != '\n') continue;

			size_t buffsize = j - prev_nl_i;
			if (buffsize == 1) {
				char *content = (char*)malloc(buffsize);
				*content = '\n';

				pfarray.files[pfarray_file_id++] = (struct File) {
					.content = content,
					.size = buffsize
				};
				prev_nl_i = j;
				continue;
			}

			size_t size = buffsize + lnsize + space_size;
			char *content = (char*)malloc(size);
			set_ln(content, ln++, lnsize);
			set_space(content + lnsize);

			char *dest = memcpy(content + lnsize + space_size, file.content + prev_nl_i + 1, buffsize);
			if (dest != content + lnsize + space_size) {
				fprintf(stderr, "memcpy failed\n");
				exit(-1);
			}

			pfarray.files[pfarray_file_id++] = (struct File) {
				.content = content,
				.size = size
			};
			prev_nl_i = j;
		}

		if (prev_nl_i != file.size - 1) {
			size_t j = file.size - 1;

			size_t buffsize = j - prev_nl_i;
			size_t size = buffsize + lnsize + space_size;
			char *content = (char*)malloc(size);
			set_ln(content, ln++, lnsize);
			set_space(content + lnsize);

			char *dest = memcpy(content + lnsize + space_size, file.content + prev_nl_i + 1, buffsize);
			if (dest != content + lnsize + space_size) {
				fprintf(stderr, "memcpy failed\n");
				exit(-1);
			}

			pfarray.files[pfarray_file_id++] = (struct File) {
				.content = content,
				.size = size
			};
		}
	}

	return pfarray;
}

size_t lfcut_amount(struct FileArray farray) { // lines for cut amount
	size_t amount = 0;
	int cut = 0;
	for (size_t i = 0; i < farray.amount; i++) {
		struct File file = farray.files[i];
		size_t prev_nl_i = -1;
		for (size_t j = 0; j < file.size; j++) {
			char ch = file.content[j];
			if (ch != '\n') {
				if (cut) cut = 0;
				continue;
			}

			size_t buffsize = j - prev_nl_i;
			if (cut && buffsize == 1) {
				prev_nl_i = j;
				amount++;
				continue;
			}
			if (buffsize == 1) cut = 1;

		}
	}
	return amount;
}

struct FileArray parser_s(struct FileArray farray) {
	size_t lamount = FileArray_content_lamount(farray) - lfcut_amount(farray);
	size_t no_nl_end_amount = FileArray_content_no_nl_end_amount(farray);

	struct FileArray pfarray = {
		.amount = (lamount + no_nl_end_amount)
	};
	pfarray.files = (struct File*)malloc(pfarray.amount * sizeof(struct File));

	size_t lnsize = ln_size(lamount);
	size_t ln = 1;

	int cut = 0;
	size_t pfarray_file_id = 0;
	for (size_t i = 0; i < farray.amount; i++) {
		struct File file = farray.files[i];
		size_t prev_nl_i = -1;
		for (size_t j = 0; j < file.size; j++) {
			char ch = file.content[j];
			if (ch != '\n') {
				if (cut) cut = 0;
				continue;
			}

			size_t buffsize = j - prev_nl_i;

			if (cut && buffsize == 1) {
				prev_nl_i = j;
				continue;
			}
			if (buffsize == 1) cut = 1;

			size_t size = buffsize;
			char *content = (char*)malloc(size);

			char *dest = memcpy(content, file.content + prev_nl_i + 1, buffsize);
			if (dest != content) {
				fprintf(stderr, "memcpy failed\n");
				exit(-1);
			}

			pfarray.files[pfarray_file_id++] = (struct File) {
				.content = content,
				.size = size
			};
			prev_nl_i = j;
		}

		if (prev_nl_i != file.size - 1) {
			size_t j = file.size - 1;

			size_t buffsize = j - prev_nl_i;
			size_t size = buffsize;
			char *content = (char*)malloc(size);

			char *dest = memcpy(content, file.content + prev_nl_i + 1, buffsize);
			if (dest != content) {
				fprintf(stderr, "memcpy failed\n");
				exit(-1);
			}

			pfarray.files[pfarray_file_id++] = (struct File) {
				.content = content,
				.size = size
			};
		}
	}

	return pfarray;
}

struct FileArray parser_E(struct FileArray farray) {
	struct FileArray pfarray = {
		.files = (struct File*)malloc(farray.amount * sizeof(struct File)),
		.amount = farray.amount
	};

	size_t pfarray_file_id = 0;
	for (size_t i = 0; i < farray.amount; i++) {
		struct File file = farray.files[i];
		size_t file_content_lamount = File_content_lamount(file);

		struct File pfile;
		pfile.size = file_content_lamount + file.size;
		pfile.content = (char*)malloc(pfile.size);

		for (size_t j = 0, k = 0; j < file.size; j++, k++) {
			char ch = file.content[j];
			if (ch == '\n')
				pfile.content[k++] = '$';

			pfile.content[k] = ch;
		}

		pfarray.files[pfarray_file_id++] = pfile;
	}
	return pfarray;
}

struct FileArray parser_T(struct FileArray farray) {
	struct FileArray pfarray = {
		.files = (struct File*)malloc(farray.amount * sizeof(struct File)),
		.amount = farray.amount
	};

	size_t pfarray_file_id = 0;
	for (size_t i = 0; i < farray.amount; i++) {
		struct File file = farray.files[i];
		size_t file_content_lamount = File_content_tamount(file);

		struct File pfile;
		pfile.size = file_content_lamount + file.size;
		pfile.content = (char*)malloc(pfile.size);

		for (size_t j = 0, k = 0; j < file.size; j++, k++) {
			char ch = file.content[j];
			if (ch == '\t') {
				pfile.content[k++] = '^';
				pfile.content[k] = 'I';
				continue;
			}

			pfile.content[k] = ch;
		}

		pfarray.files[pfarray_file_id++] = pfile;
	}
	return pfarray;
}

size_t npr_crep_size(struct File file) { // npr_crep_size -> non printing characters repaction size
	size_t size = 0;
	for (size_t i = 0; i < file.size; i++) {
		unsigned char ch = file.content[i];
		if (ch == '\t' || ch == '\n') // ignore tabs + new lines
			continue;
		if (ch >= 128) {
			ch -= 128;
			size += 2;
		}
		if (ch < 32 || ch == 127)
			size++;
	}
	return size;
}

struct FileArray parser_v(struct FileArray farray) {
	struct FileArray pfarray = {
		.files = (struct File*)malloc(farray.amount * sizeof(struct File)),
		.amount = farray.amount
	};

	size_t pfarray_file_id = 0;
	for (size_t i = 0; i < farray.amount; i++) {
		struct File file = farray.files[i];
		size_t crep_size = npr_crep_size(file);

		struct File pfile;
		pfile.size = crep_size + file.size;
		pfile.content = (char*)malloc(pfile.size);

		for (size_t j = 0, k = 0; j < file.size; j++, k++) {
			unsigned char ch = file.content[j];
			if (ch == '\t' || ch == '\n') { // ignore tabs + new lines
				pfile.content[k] = ch;
				continue;
			}

			if (ch >= 128) {
				ch -= 128;
				pfile.content[k++] = 'M';
				pfile.content[k++] = '-';
			}
			if (ch < 32 || ch == 127) {
				pfile.content[k++] = '^';
				ch ^= 0x40;
			}
			pfile.content[k] = ch;
		}

		pfarray.files[pfarray_file_id++] = pfile;
	}
	return pfarray;
}

OptParser get_parser(int opt) {
	switch (opt) {
		case OPT_n: return parser_n;
		case OPT_b: return parser_b;
		case OPT_s: return parser_s;
		case OPT_E: return parser_E;
		case OPT_T: return parser_T;
		case OPT_v: return parser_v;
		default: return NULL;
	}
}

struct FileArray parse(struct FileArray farray, struct OptArray opta) {
	struct FileArray res = FileArray_dup(farray);
	for (size_t i = 0; i < opta.amount; i++) {
		OptParser parser = get_parser(opta.options[i]);
		if (parser == NULL) continue;

		struct FileArray pfarray = parser(res);
		free_FileArray_files(res);
		res = pfarray;
	}
	return res;
}
