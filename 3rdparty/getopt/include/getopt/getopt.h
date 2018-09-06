/* a getopt.
   version 0.1, march, 2012

   Copyright (C) 2012- Fredrik Kihlander

   https://github.com/wc-duck/getopt

   This software is provided 'as-is', without any express or implied
   warranty.  In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
      claim that you wrote the original software. If you use this software
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original software.
   3. This notice may not be removed or altered from any source distribution.

   Fredrik Kihlander
*/

#ifndef GETOPT_GETOPT_H_INCLUDED
#define GETOPT_GETOPT_H_INCLUDED

#include <stddef.h>

#if defined (__cplusplus)
extern "C" {
#endif

// Define GETOPT_PRIVATE_API before including getopt.h to inline all functions and self-include the source unit
// #define GETOPT_PRIVATE_API
// #include "getopt/getopt.h"
#ifdef GETOPT_PRIVATE_API
#  if defined(__GNUC__) || defined(__clang__)
#    define GETOPT_API static __attribute__((unused))
#  else
#	 define GETOPT_API static
#  endif
#else
#	define GETOPT_API 
#endif

/*
	File: getopt.h
		Sumary:
			Provides functionality to parse standard argc/argv in an easy manner.
*/

/*
	Enum: getopt_option_type_t
		Types of supported options by system.

		GETOPT_OPTION_TYPE_NO_ARG   - The option can have no argument
		GETOPT_OPTION_TYPE_REQUIRED - The option requires an argument (--option=arg, -o arg)
		GETOPT_OPTION_TYPE_OPTIONAL - The option-argument is optional

		GETOPT_OPTION_TYPE_FLAG_SET - The option is a flag and value will be set to flag
		GETOPT_OPTION_TYPE_FLAG_AND - The option is a flag and value will be and:ed with flag
		GETOPT_OPTION_TYPE_FLAG_OR  - The option is a flag and value will be or:ed with flag
*/
typedef enum getopt_option_type
{
	GETOPT_OPTION_TYPE_NO_ARG,
	GETOPT_OPTION_TYPE_REQUIRED,
	GETOPT_OPTION_TYPE_OPTIONAL,
	GETOPT_OPTION_TYPE_FLAG_SET,
	GETOPT_OPTION_TYPE_FLAG_AND,
	GETOPT_OPTION_TYPE_FLAG_OR
} getopt_option_type_t;

/**
 * Helper-macro to define end-element in options-array.
 * Mostly helpful on higher warning-level where compiler would complain for { 0 }
 */
#define GETOPT_OPTIONS_END { 0, 0, GETOPT_OPTION_TYPE_NO_ARG, 0, 0, 0, 0 }

/*
	Struct: getopt_option
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
typedef struct getopt_option
{
	const char*          name;
	int                  name_short;
	getopt_option_type_t type;
	int*                 flag;
	int                  value;
	const char*          desc;
	const char*          value_desc;
} getopt_option_t;

/*
	Struct: getopt_context_t
		Context used while parsing options.
		Need to be initialized by <getopt_create_context> before usage. If reused a re-initialization by <getopt_create_context> is needed.
		Do not modify data in this struct manually!

	Members:
		argc            - Internal variable
		argv            - Internal variable
		opts            - Internal variable
		num_opts        - Internal variable
		current_index   - Internal variable
		current_opt_arg - Used to return values. See <getopt_next>
*/
typedef struct getopt_context
{
	int                    argc;
	const char**           argv;
	const getopt_option_t* opts;
	int                    num_opts;
	int                    current_index;
	const char*            current_opt_arg;
} getopt_context_t;

/*
	Function: getopt_create_context
		Initializes an getopt_context_t-struct to be used by <getopt_next>

	Arguments:
		ctx  - Pointer to context to initialize.
		argc - argc from "int main(int argc, char** argv)" or equal.
		argv - argv from "int main(int argc, char** argv)" or equal. Data need to be valid during option-parsing and usage of data.
		opts - Pointer to array with options that should be looked for. Should end with an option that is all zeroed!

	Returns:
		0 on success, otherwise error-code.
*/
GETOPT_API int getopt_create_context( getopt_context_t* ctx, int argc, const char** argv, const getopt_option_t* opts );

/*
	Function: getopt_next
		Used to parse argc/argv with the help of a getopt_context_t.
		Tries to parse the next token in ctx and return id depending on status.

	Arguments:
		ctx - Pointer to a initialized <getopt_context_t>

	Returns:
		- '!' on error. ctx->current_opt_arg will be set to flag-name! Errors that can occur, 
		      Argument missing if argument is required or Argument found when there should be none.
		- '?' if item was an unrecognized option, ctx->current_opt_arg will be set to item!
		- '+' if item was no option, ctx->current_opt_arg will be set to item!
		- '0' if the opt was a flag and it was set. ctx->current_opt_arg will be set to flag-name!
		      the value stored is value in the found option.
		- -1 no more options to parse!
*/
GETOPT_API int getopt_next( getopt_context_t* ctx );

/*
	Function: GetOptCreateHelpString
		Builds a string that describes all options for use with the --help-flag etc.

	Arguments:
		ctx         - Pointer to a initialized <getopt_context_t>
		buffer      - Pointer to buffer to build string in.
		buffer_size - Size of buffer.

	Returns:
		buffer filled with a help-string.
*/
GETOPT_API const char* getopt_create_help_string( getopt_context_t* ctx, char* buffer, size_t buffer_size );

#ifdef GETOPT_PRIVATE_API
#	include "../../src/getopt.c"
#endif

#if defined (__cplusplus)
}
#endif

#endif
