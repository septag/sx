#include "sx/cmdline.h"
#include <stdio.h>

static void print_help(sx_cmdline_context* ctx)
{
    char buffer[2048];
    puts(sx_cmdline_create_help_string(ctx, buffer, sizeof(buffer)));
}

int main(int argc, char* argv[])
{
    const sx_alloc* alloc = sx_alloc_malloc;
    int verbose = 0;
    const sx_cmdline_opt opts[] = {
        {"help", 'h', SX_CMDLINE_OPTYPE_NO_ARG, 0x0, 'h', "print this help text", 0x0},
        {"verbose", 'v', SX_CMDLINE_OPTYPE_FLAG_SET, &verbose, 1, "verbose logging enabled", 0x0},
        {"input", 'i', SX_CMDLINE_OPTYPE_REQUIRED, 0x0, 'i', "input file", "FILE"},
        SX_CMDLINE_OPT_END
    };
    sx_cmdline_context* cmdline = sx_cmdline_create_context(alloc, argc, (const char**)argv, opts);

    int opt;
    const char* arg;
    while ((opt = sx_cmdline_next(cmdline, NULL, &arg)) != -1) {
        switch (opt) {
            case '+': printf("got argument without flag: %s\n", arg); break;
            case '?': printf("unknown flag: %s\n", arg); break;
            case '!': printf("invalid use of flag: %s\n", arg); break;
            case 'i': printf("got input with value: %s\n", arg); break;
            case 0: printf("flag was set\n"); break;
            case 'h': print_help(cmdline); break;
            default: break;
        }
    }

    if (verbose > 0)
        puts("verbose is set");

    sx_cmdline_destroy_context(cmdline, alloc);

    return 0;
}
