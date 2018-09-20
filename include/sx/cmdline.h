//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// cmdline.h - Command line parser
//             This is actually a very thin wrapper around getopt
//             Source: https://github.com/wc-duck/getopt
//             Example: https://github.com/wc-duck/getopt/blob/master/example/example.cpp
//
#pragma once

#ifndef SX_CMDLINE_H_
#define SX_CMDLINE_H_

#include "allocator.h"

/*
	Enum: sx_cmdline_optype
		Types of supported options by system.

		SX_CMDLINE_OPTYPE_NO_ARG   - The option can have no argument
		SX_CMDLINE_OPTYPE_REQUIRED - The option requires an argument (--option=arg, -o arg)
		SX_CMDLINE_OPTYPE_OPTIONAL - The option-argument is optional

		SX_CMDLINE_OPTYPE_FLAG_SET - The option is a flag and value will be set to flag
		SX_CMDLINE_OPTYPE_FLAG_AND - The option is a flag and value will be and:ed with flag
		SX_CMDLINE_OPTYPE_FLAG_OR  - The option is a flag and value will be or:ed with flag
*/
typedef enum sx_cmdline_optype
{
	SX_CMDLINE_OPTYPE_NO_ARG,
	SX_CMDLINE_OPTYPE_REQUIRED,
	SX_CMDLINE_OPTYPE_OPTIONAL,
	SX_CMDLINE_OPTYPE_FLAG_SET,
	SX_CMDLINE_OPTYPE_FLAG_AND,
	SX_CMDLINE_OPTYPE_FLAG_OR
} sx_cmdline_optype;

#define SX_CMDLINE_OPT_END { 0, 0, SX_CMDLINE_OPTYPE_NO_ARG, 0, 0, 0, 0 }

/*
    Struct: sx_cmdline_opt
        Option in system.

    Members:
        name       - Long name of argument, set to NULL if only short name is valid.
        name_short - Short name of argument, set to 0 if only long name is valid.
        type       - Type of option, see <getopt_option_type>.
        flag       - Pointer to flag to set if option is of flag-type, set to null NULL if option is not of flag-type.
        value      - If option is of flag-type, this value will be set/and:ed/or:ed to the flag, else it will be returned from GetOpt when option is found.
        desc       - Description of option.
        value_desc - Short description of valid values to the option, will only be used when generating help-text. example: "--my_option=<value_desc_goes_here>"
*/
typedef struct sx_cmdline_opt_s
{
	const char*          name;
	int                  name_short;
	sx_cmdline_optype    type;
	int*                 flag;
	int                  value;
	const char*          desc;
	const char*          value_desc;
} sx_cmdline_opt;

typedef struct getopt_context sx_cmdline_context;

#ifdef __cplusplus
extern "C" {
#endif

sx_cmdline_context* sx_cmdline_create_context(const sx_alloc* alloc, int argc, const char** argv, 
											  const sx_cmdline_opt* opts);
void sx_cmdline_destroy_context(sx_cmdline_context* ctx, const sx_alloc* alloc);
int sx_cmdline_next(sx_cmdline_context* ctx, SX_OUT int* index, SX_OUT const char** arg);
const char* sx_cmdline_create_help_string(sx_cmdline_context* ctx, char* buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif // SX_CMDLINE_H_