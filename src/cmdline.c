//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
#include "sx/cmdline.h"

#define GETOPT_PRIVATE_API
#include "../3rdparty/getopt/include/getopt/getopt.h"

sx_cmdline_context* sx_cmdline_create_context(const sx_alloc* alloc, int argc, const char** argv, 
											  const sx_cmdline_opt* opts)
{
    sx_cmdline_context* ctx = (sx_cmdline_context*)sx_malloc(alloc, sizeof(sx_cmdline_context));
    if (!ctx)
        sx_out_of_memory();

    int r = getopt_create_context(ctx, argc, argv, (const getopt_option_t*)opts);
    if (r < 0) {
        sx_free(alloc, ctx);
        return NULL;
    }

    return ctx;
}

void sx_cmdline_destroy_context(sx_cmdline_context* ctx, const sx_alloc* alloc)
{
    sx_assert(ctx);
    sx_free(alloc, ctx);
}

int sx_cmdline_next(sx_cmdline_context* ctx, SX_OUT int* index, SX_OUT const char** arg)
{
    int r = getopt_next(ctx);
    if (r != -1) {
        if (index)
            *index = ((getopt_context_t*)ctx)->current_index;
        if (arg)
            *arg = ((getopt_context_t*)ctx)->current_opt_arg;
    }
    return r;
}

const char* sx_cmdline_create_help_string(sx_cmdline_context* ctx, char* buffer, size_t buffer_size)
{
    return getopt_create_help_string(ctx, buffer, buffer_size);
}


