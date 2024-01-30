#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct Sequence {
    const char **elements;
    int amount;
};

void free_sequence_elements(struct Sequence *sequence) {
    for (int i = 0; i < sequence->amount; i++) {
        free((char*)sequence->elements[i]);
    }
    free(sequence->elements);
    sequence->elements = NULL;
}

struct Input {
    struct Sequence opts;
    struct Sequence files;
    struct Sequence all;
};

int opts_amount(struct Input *input) {
    int amount = 0;
    for (int i = 0; i < input->all.amount; i++) {
        const char* popt = input->all.elements[i];
        if (popt[0] == '-') {
            if (strcmp(popt, "--") == 0)
                break;
            amount++;
        }
    }
    return amount;
}

int files_amount(struct Input *input) {
    int amount = 0;
    for (int i = 0, is100 = 0; i < input->all.amount; i++) {
        const char *pfile = input->all.elements[i];
        if (is100 || pfile[0] != '-') {
            amount++;
        }
        if (strcmp(pfile, "--") == 0)
            is100 = 1;
    }
    return amount;
}

void parse_files(struct Input *input) {
    input->files.amount = files_amount(input);
    if (input->files.amount == 0) {
        input->files.elements = NULL;
        return;
    }

    const char **elements = (const char**)malloc(input->files.amount * sizeof(char*));
    for (int i = 0, is100 = 0, id = 0; i < input->all.amount; i++) {
        const char *pfile = input->all.elements[i];
        if (is100 || pfile[0] != '-')
            elements[id++] = pfile;
        if (strcmp(pfile, "--") == 0)
            is100 = 1;
    }
    input->files.elements = elements;
}

void parse_opts(struct Input *input) {
    input->opts.amount = opts_amount(input);
    if (input->opts.amount == 0) {
        input->opts.elements = NULL;
        return;
    }
    
    const char **elements = (const char**)malloc(input->opts.amount * sizeof(char*));
    for (int i = 0, id = 0; i < input->all.amount; i++) {
        const char *popt = input->all.elements[i];
        if (strcmp(popt, "--") == 0)
            break;
        if (popt[0] == '-')
            elements[id++] = popt;
    }
    input->opts.elements = elements;
}

void tidy_print(struct Input *input) {
    for (int i = 0; i < input->files.amount; i++) {
        printf("files[%d]: %s\n", i, input->files.elements[i]);
    }
    for (int i = 0; i < input->opts.amount; i++) {
        printf("options[%d]: %s\n", i, input->opts.elements[i]);
    }
}

struct Input parse(char **elements, int amount) {
    struct Input input;
    input.all.elements = (const char**)elements;
    input.all.amount = amount;

    parse_files(&input);
    parse_opts(&input);

    // tidy_print(&input);

    // free(input.files.elements);
    // free(input.opts.elements);

    return input;
}

int check_files(struct Sequence *files) {
    int res = 0;
    for (int i = 0; i < files->amount; i++) {
        FILE *fstream = fopen(files->elements[i], "r");
        if (fstream == NULL) {
            fprintf(stderr, "%s: ", files->elements[i]);
            perror("");
            return 0;
        }
        res = fclose(fstream);
        if (res != 0) {
            perror("fclose() failed");
            return 0;
        }
    }
    return 1;
}

char *file_content(FILE *fstream) {
    int res = fseek(fstream, 0L, SEEK_END);
    if (res != 0) {
        perror("fseek() failed");
        return NULL;
    }
    long size = ftell(fstream);
    char *content = (char*)malloc(size + 1);

    fseek(fstream, 0L, SEEK_SET);
    int i = 0;
    for (char c = fgetc(fstream);
        c != EOF;
        c = fgetc(fstream)) content[i++] = c;
    content[size] = '\0';

    return content;
}

struct Sequence files_content(struct Sequence *files) {
    struct Sequence contents;

    contents.amount = files->amount;
    const char **elements = (const char**)malloc(contents.amount * sizeof(char*));

    int res = 0;
    for (int i = 0; i < files->amount; i++) {
        FILE *fstream = fopen(files->elements[i], "r");
        if (fstream == NULL) {
            perror("fopen() failed");
            exit(-1);
        }

        const char *content = file_content(fstream);
        elements[i] = content;

        res = fclose(fstream);
        if (res != 0) {
            perror("fclose() failed");
            exit(-1);
        }
    }
    contents.elements = elements;
    return contents;
}

static const struct Sequence LONG_OPTIONS = {
    .elements = (const char*[]) {
        "show-all",
        "number-nonblank",
        "show-ends",
        "number",
        "squeeze-blank",
        "show-nonprinting",
        "help",
        "version"
    },
    .amount = 8
};

static const struct Sequence SHORT_OPTIONS = {
    .elements = (const char*[]) {
        "A",
        "b",
        "e",
        "E",
        "n",
        "s",
        "t",
        "T",
        "u",
        "v"
    },
    .amount = 10
};

int is_long_option(const char *popt) {
    popt += 2;
    for (int i = 0; i < LONG_OPTIONS.amount; i++) {
        if (strcmp(popt, LONG_OPTIONS.elements[i]) == 0)
            return 1;
    }
    return 0;
}

int is_short_option(const char *popt) {
    popt++;
    char buff[2] = {0};
    buff[0] = popt[0];
    for (int i = 0; i < SHORT_OPTIONS.amount; i++) {
        if (strcmp(buff, SHORT_OPTIONS.elements[i]) == 0) {
            if (popt[1] == '\0') return 1;
            popt++;
            buff[0] = popt[0];
            i = -1;
        }
    }
    return 0;
}

int check_options(struct Sequence *opts) {
    for (int i = 0; i < opts->amount; i++) {
        const char *popt = opts->elements[i];
        if (popt[1] == '-') {
            if (!is_long_option(popt)) {
                fprintf(stderr, "Invalid option: %s\n", popt);
                return 0;
            }
        } else {
            if (!is_short_option(popt)) {
                fprintf(stderr, "Invalid option: %s\n", popt);
                return 0;
            }
        }
    }
    return 1;
}


struct StrList {
    const char* name;
    struct StrList* next;
};

const char *long_to_short_opt(const char* opt) {
    const char* optn = opt + 2;
    if (strcmp(optn, "show-all") == 0)
        return "vET";
    if (strcmp(optn, "number-nonblank") == 0)
        return "b";
    if (strcmp(optn, "show-ends") == 0)
        return "E";
    if (strcmp(optn, "number") == 0)
        return "n";
    if (strcmp(optn, "squeeze-blank") == 0)
        return "s";
    if (strcmp(optn, "show-tabs") == 0)
        return "T";
    if (strcmp(optn, "show-nonprinting") == 0)
        return "v";

    fprintf(stderr, "Invalid option: %s\n", opt);
    exit(-1);
}

struct StrList *StrList_pushf(struct StrList *ls) {
    struct StrList *topls = (struct StrList*)malloc(sizeof(struct StrList));
    topls->next = ls;
    topls->name = NULL;
    return topls;
}

void StrList_clear(struct StrList *ls) {
    struct StrList *prev;
    struct StrList *curr = ls;
    while(curr != NULL) {
        prev = curr;
        curr = curr->next;
        
        free((char*)prev->name);
        free(prev);
    }
}

int opt_in_ls(struct StrList *ls, const char *opt) {
    struct StrList *curr = ls;

    while (curr != NULL) {
        if (strcmp(curr->name, opt) == 0)
            return 1;
        curr = curr->next;
    }

    return 0;
}

const char *adjust_short_opt(const char* opt) {
    if (strcmp(opt, "e") == 0)
        return "vE";
    if (strcmp(opt, "t") == 0)
        return "vT";
    if (strcmp(opt, "A") == 0)
        return "vET";
    return opt;
}

struct StrList *del_opt(struct StrList *ls, const char *opt) {
    struct StrList *prev = NULL;
    struct StrList *curr = ls;

    while (curr != NULL) {
        if (strcmp(curr->name, opt) == 0) {
            if (prev != NULL)
                prev->next = curr->next;
            else
                ls = curr->next;
            free((char*)curr->name);
            free(curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    return ls;
}

struct StrList *StrList_last(struct StrList *ls) {
    struct StrList *curr = ls;
    struct StrList *prev = ls;
    while (curr != NULL) {
        prev = curr;
        curr = curr->next;
    }
    return prev;
}

void swap_with_last(struct StrList *ls, const char* opt) {
    struct StrList *last = StrList_last(ls);
    struct StrList *curr = ls;
    while (curr != NULL) {
        if (strcmp(curr->name, opt) == 0) {
            const char *tname = curr->name;
            curr->name = last->name;
            last->name = tname;
            return;
        }
        curr = curr->next;
    }
}

void swap_with_first(struct StrList *ls, const char* opt) {
    struct StrList *curr = ls;
    while (curr != NULL) {
        if (strcmp(curr->name, opt) == 0) {
            const char *tname = curr->name;
            curr->name = ls->name;
            ls->name = tname;
            return;
        }
        curr = curr->next;
    }
}

struct StrList *filter_opts(struct Sequence *opts) {
    struct StrList *opts_ls = NULL;
    for (int i = 0; i < opts->amount; i++) {
        const char* opt = opts->elements[i];
        if (opt[1] == '-')
            opt = long_to_short_opt(opt);
        else
            opt++;
        for (int j = 0; j < strlen(opt); j++) {
            char buff[2] = {0};
            buff[0] = opt[j];
            const char *topt = adjust_short_opt(buff);
            for (int k = 0; topt[k] != '\0'; k++) {
                buff[0] = topt[k];
                if (!opt_in_ls(opts_ls, buff)) {
                    opts_ls = StrList_pushf(opts_ls);
                    opts_ls->name = strdup(buff);
                }
            }
        }
    }
    // b overrides n
    if (opt_in_ls(opts_ls, "b")) {
        opts_ls = del_opt(opts_ls, "n");
        swap_with_last(opts_ls, "b");
    } else {
        swap_with_last(opts_ls, "n");
    }
    // jusr ignore u
    opts_ls = del_opt(opts_ls, "u");

    // swap only if present
    swap_with_last(opts_ls, "s");

    return opts_ls;
}

typedef struct Sequence (*OptSetter)(struct Sequence *);

struct OptSetterList {
    OptSetter setter;
    struct OptSetterList *next;
};

struct OptSetterList *OptSetterList_pushf(struct OptSetterList *ls) {
    struct OptSetterList *topls = (struct OptSetterList*)malloc(sizeof(struct OptSetterList));
    topls->next = ls;
    topls->setter = NULL;
    return topls;
}

void OptSetterList_clear(struct OptSetterList *ls) {
    struct OptSetterList *prev;
    struct OptSetterList *curr = ls;
    while(curr != NULL) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }
}

int lines_amount(const char* buff) {
    int amount = 0;
    for (int i = 0; buff[i] != '\0'; i++) {
        if (buff[i] == '\n')
            amount++;
    }
    return amount;
}

int number_size(int lamount) {
    int size = log10(lamount) + 1;
    int padd = 6 - (size % 6);
    size += padd;
    return size;
}

void number_line(int nsize, int ln, char* line) {
    memset(line, ' ', nsize);
    sprintf(line, "%d", ln + 1);
    int numci = 0; // num character index
    
    for (int i = nsize - 1; i >= 0; i--) {
        if (line[i] != ' ') {
            if (line[i] == '\0') {
                line[i] = ' ';
                continue;
            }
            numci = i;
            break;
        }
    }

    int numcpi = nsize - 1; // num character put index
    while (numci >= 0) {
        line[numcpi] = line[numci];
        line[numci] = ' ';
        numcpi--;
        numci--;
    }
}

struct Sequence number_all_lines(struct Sequence *output) {
    int lamount = 0;
    for (int i = 0; i < output->amount; i++) {
        lamount += lines_amount(output->elements[i]);
    }

    struct Sequence new_output = {
        .elements = (const char**)malloc(lamount * sizeof(char*)),
        .amount = lamount
    };

    int nsize = number_size(lamount);
    for (int i = 0, ln = 0; i < output->amount; i++) {
        const char *buff = output->elements[i];
        for (int j = 0, jp = 0; buff[j] != '\0'; j++) {
            if (buff[j] != '\n')
                continue;

            int plsize = j - jp;
            if (plsize <= 1) {
                char* line = (char*)malloc(4 + nsize);

                number_line(nsize, ln, line);   
                
                line[nsize] = '\n';
                line[nsize + 1] = '\0';

                new_output.elements[ln++] = line;
                jp = j;
                continue;
            }

            int line_len = jp != 0 ?
                plsize + nsize + 3 : plsize + nsize + 4;

            char* line = (char*)malloc(line_len);

            number_line(nsize, ln, line); // line number
            
            line[nsize] = ' '; // paddng after
            line[nsize + 1] = ' ';
           
            const char *start = jp != 0 ? buff + jp + 1 : buff;
            int amount = jp != 0 ? plsize : plsize + 1;
            memcpy(line + nsize + 2, start, amount); // line itself
            line[line_len - 1] = '\0';

            new_output.elements[ln++] = line;
            jp = j;
        }
    }

    return new_output;
}

struct Sequence number_all_nonempty_lines(struct Sequence *output) {
    int lamount = 0;
    for (int i = 0; i < output->amount; i++) {
        lamount += lines_amount(output->elements[i]);
    }

    struct Sequence new_output = {
        .elements = (const char**)malloc(lamount * sizeof(char*)),
        .amount = lamount
    };

    int nsize = number_size(lamount);
    for (int i = 0, ln = 0, lni = 0; i < output->amount; i++) {
        const char *buff = output->elements[i];
        for (int j = 0, jp = 0; buff[j] != '\0'; j++) {
            if (buff[j] == '\n') {
                int plsize = j - jp;
                if (plsize <= 1) {
                    char *line = (char*)malloc(plsize + 1);
                    line[0] = '\n';
                    line[1] = '\0';
                    new_output.elements[lni++] = line;
                    jp = j;
                    continue;
                }

                int line_len = jp != 0 ?
                    plsize + nsize + 3 : plsize + nsize + 4;

                char* line = (char*)malloc(line_len);

                number_line(nsize, ln++, line); // line number
                
                line[nsize] = ' '; // paddng after
                line[nsize + 1] = ' ';

                const char *start = jp != 0 ? buff + jp + 1 : buff;
                int amount = jp != 0 ? plsize : plsize + 1;
                memcpy(line + nsize + 2, start, amount); // line itself
                line[line_len - 1] = '\0';

                new_output.elements[lni++] = line;
                jp = j;
            }
        }
    }

    return new_output;
}

int tabs_amount(const char *buff) {
    int amount = 0;
    for (int i = 0; buff[i] != '\0'; i++) {
        if (buff[i] == '\t')
            amount++;
    }
    return amount;
}

struct Sequence show_tabs(struct Sequence *output) {
    struct Sequence new_output = {
        .elements = (const char**)malloc(output->amount * sizeof(char*)),
        .amount = output->amount
    };

    for (int i = 0; i < new_output.amount; i++) {
        const char* src = output->elements[i];
        int len = strlen(src);
        int tamount = tabs_amount(src);
        char *buff = (char*)malloc(len + tamount + 1);
        for (int j = 0, k = 0; j < len; j++, k++) {
            if (src[j] == '\t') {
                buff[k++] = '^';
                buff[k] = 'I';
                continue;
            }
            buff[k] = src[j];
        }
        buff[len + tamount] = '\0';
        new_output.elements[i] = buff;
    }

    return new_output;
}

struct Sequence show_ends(struct Sequence *output) {
    struct Sequence new_output = {
        .elements = (const char**)malloc(output->amount * sizeof(char*)),
        .amount = output->amount
    };

    for (int i = 0; i < new_output.amount; i++) {
        const char* src = output->elements[i];
        int len = strlen(src);
        int lamount = lines_amount(src);
        char *buff = (char*)malloc(len + lamount + 1);
        for (int j = 0, k = 0; j < len; j++, k++) {
            if (src[j] == '\n') {
                buff[k++] = '$';
                buff[k] = '\n';
                continue;
            }
            buff[k] = src[j];
        }
        buff[len + lamount] = '\0';
        new_output.elements[i] = buff;
    }

    return new_output;
}

int elements_amount_after_squeeze(struct Sequence *output) {
    int amount = 0;
    for (int i = 0, squeeze = 0; i < output->amount; i++) {
        const char *src = output->elements[i];
        for (int j = 0, jp = 0; src[j] != '\0'; j++) {
            if (src[j] != '\n')
                continue;
            if (j - jp <= 1) {
                if (!squeeze) {
                    amount++;
                    squeeze = 1;
                }
            } else {
                amount++;
                squeeze = 0;
            }
            jp = j;
        }

        int len = strlen(src);
        if (src[len - 1] != '\n')
            amount++;
    }
    return amount;
}

struct Sequence squeeze_blank(struct Sequence *output) {
    int amount = elements_amount_after_squeeze(output);
    // printf("after squeeze: %d\n", amount);
    // printf("before: %d\n", output->amount);

    struct Sequence new_output = {
        .elements = (const char**)malloc(amount * sizeof(char*)),
        .amount = amount
    };

    int id = 0;
    for (int i = 0, squeeze = 0; i < output->amount; i++) {
        const char *src = output->elements[i];
        int jp = 0;
        for (int j = 0; src[j] != '\0'; j++) {
            if (src[j] != '\n')
                continue;
            if (j - jp <= 1) {
                if (!squeeze) {
                    new_output.elements[id++] = strdup("\n");
                    squeeze = 1;
                }
            } else {
                char *buff = (char*)malloc(j - jp + 1);
                memcpy(buff, src + jp + 1, j - jp);
                buff[j - jp] = '\0';

                new_output.elements[id++] = buff;
                squeeze = 0;
            }
            jp = j;
        }

        int len = strlen(src);
        if (src[len - 1] != '\n') {
            int j = len - 1;
            int jp = len - 1;
            while (jp >= 0) {
                if (src[jp] != '\n') {
                    jp--;
                    continue;
                }
                char *buff = (char*)malloc(j - jp + 1);
                memcpy(buff, src + jp + 1, j - jp);
                buff[j - jp] = '\0';

                new_output.elements[id++] = buff;
                break;
            }

            // no lines file
            if (jp == -1) {
                new_output.elements[id++] = strdup(src);
            }
        }
    }

    return new_output;
}

OptSetter get_OptSetter(const char *opt) {
    if (strcmp(opt, "n") == 0)
        return number_all_lines;
    if (strcmp(opt, "b") == 0)
        return number_all_nonempty_lines;
    if (strcmp(opt, "T") == 0)
        return show_tabs;
    if (strcmp(opt, "E") == 0)
        return show_ends;
    if (strcmp(opt, "s") == 0)
        return squeeze_blank;
    return NULL;
}

struct OptSetterList *get_opt_setters_list(struct Sequence *opts) {
    struct StrList *opts_ls = filter_opts(opts);
    struct StrList *curr = opts_ls;

    struct OptSetterList *setter_ls = NULL;
    while (curr != NULL) {
        OptSetter setter = get_OptSetter(curr->name);
        if (setter != NULL) {
            setter_ls = OptSetterList_pushf(setter_ls);
            setter_ls->setter = setter;
        }
        curr = curr->next;
    }

    StrList_clear(opts_ls);
    return setter_ls;
}

struct Sequence apply_options(struct Sequence *contents, struct Sequence *opts) {
    struct Sequence output = {
        .elements = (const char**)malloc(contents->amount * sizeof(char*)),
        .amount = contents->amount
    };

    for (int i = 0; i < output.amount; i++) {
        int len = strlen(contents->elements[i]);
        char *buff = (char*)malloc(len);
        memcpy(buff, contents->elements[i], len);
        output.elements[i] = buff;
    }

    struct OptSetterList *slist = get_opt_setters_list(opts);
    struct OptSetterList *curr = slist;

    while (curr != NULL) {
        struct Sequence noutput = curr->setter(&output);

        free_sequence_elements(&output);
        output.elements = noutput.elements;
        output.amount = noutput.amount;
        curr = curr->next;
    }

    OptSetterList_clear(slist);
    return output;
}

void concatenate(struct Input *input) {
    int check = check_files(&input->files) & check_options(&input->opts);
    if (!check) return;

    struct Sequence contents = files_content(&input->files);
    struct Sequence output = apply_options(&contents, &input->opts);
    free_sequence_elements(&contents);

    for (int i = 0; i < output.amount; i++) {
        printf("%s", output.elements[i]);
    }
    free_sequence_elements(&output);
}

int main(int argc, char **argv) {
    if (argc == 1) {
        fprintf(stderr, "No files given\n");
        return -1;
    }

    struct Input input = parse(++argv, --argc);
    concatenate(&input);

    free(input.files.elements);
    free(input.opts.elements);
    return 0;
}
