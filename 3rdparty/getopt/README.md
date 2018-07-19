# getopt - command line options parser in c

[![Build Status](https://travis-ci.org/wc-duck/getopt.svg?branch=master)](https://travis-ci.org/wc-duck/getopt)
[![Build status](https://ci.appveyor.com/api/projects/status/4twbkbyhj7h7i689)](https://ci.appveyor.com/project/wc-duck/getopt)

## About:
This module aims to be a simple drop-in argc/argv-parser for c/cpp-code. getopt support
short ( -s ) and long ( --long ) options, flags and help-generation.

## Example:
This is a simple example showing of most of the features:

```c
#include <stdio.h>
#include <getopt/getopt.h>

int verbose = 0; // verbose flag

static const getopt_option_t option_list[] =
{
	{ "help",    'h', GETOPT_OPTION_TYPE_NO_ARG,   0x0,      'h', "print this help text",       0x0 },
	{ "verbose", 'v', GETOPT_OPTION_TYPE_FLAG_SET, &verbose,  1,  "verbose logging enabled",    0x0 },
	{ "input",   'i', GETOPT_OPTION_TYPE_REQUIRED, 0x0,      'i', "an input file",           "FILE" },
	GETOPT_OPTIONS_END
};

static void print_help_string( getopt_context_t ctx )
{
	char buffer[2048];
	printf( "%s\n", getopt_create_help_string( &ctx, buffer, sizeof( buffer ) ) );
}

int main( int argc, const char** argv )
{
	getopt_context_t ctx;
	if( getopt_create_context( &ctx, argc, argv, option_list ) < 0 )
	{
		printf( "error while creating getopt ctx, bad options-list?" );
		return 1;
	}

	int opt;

	while( ( opt = getopt_next( &ctx ) ) != -1 )
	{
		switch( opt )
		{
			case '+': printf( "got argument without flag: %s\n",   ctx.current_opt_arg ); break;
			case '?': printf( "unknown flag %s\n",                 ctx.current_opt_arg ); break;
			case '!': printf( "invalid use of flag %s\n",          ctx.current_opt_arg ); break;
			case 'i': printf( "got -i or --input with value %s\n", ctx.current_opt_arg ); break;
			case   0: printf( "flag was set!\n"); break;
			case 'h': print_help_string( ctx ); break;
			default: break;
		}
	}

	if( verbose > 0 )
		printf( "verbose flag was set!" );
}


```

