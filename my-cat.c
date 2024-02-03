#include "input.h"
#include "file-ops.h"
#include "opt-array.h"
#include "parsers.h"
#include "info-opts.h"

int main(int argc, char **argv) {
    if (argc == 1) {
        fprintf(stderr, "No files given\n");
        return -1;
    }

	struct StrArray unparsed = {
		.strs = argv + 1,
		.amount = argc - 1
	};
    struct Input input = get_input(unparsed);

	struct FileArray farray = get_files(input.files);
	struct OptArray opta = get_OptArray(input.opts);
	free_Input_members(input);

	if (opta.amount == 1 && is_info_opt(opta.options[0])) {
		print_info_opt(opta.options[0]);
	} else {
		struct FileArray fparsed = parse(farray, opta);
		print_FileArray(fparsed);
		free_FileArray_files(fparsed);
	}

	free_OptArray_options(opta);
	free_FileArray_files(farray);
    return 0;
}

