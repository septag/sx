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

#ifndef GETOPT_PRIVATE_API
#	include <getopt/getopt.h>
#endif

#include <stdio.h>  /* for vsnprintf */
#include <stdarg.h> /* for va_list */
#include <string.h>
#if !defined(_MSC_VER)
#   include <strings.h> /* for strncasecmp */
#else
#   include <ctype.h> /* tolower */
#endif

static int getopt__str_case_cmp_len(const char* s1, const char* s2, unsigned int len)
{
#if defined (_MSC_VER)
	for(unsigned int i = 0; i < len; i++)
	{
		int c1 = tolower(s1[i]);
		int c2 = tolower(s2[i]);
		if(c1 < c2) return -1;
		if(c1 > c2) return  1;
		if(c1 == '\0' && c1 == c2) return 0;
	}
	return 0;
#else /* defined (_MSC_VER) */
	return strncasecmp(s1, s2, len);
#endif /* defined (_MSC_VER) */
}

static int getopt__str_format(char* buf, size_t buf_size, const char* fmt, ...)
{
	va_list args;
	va_start( args, fmt );
	int ret = vsnprintf( buf, buf_size, fmt, args );
#if defined(_MSC_VER)
	buf[buf_size - 1] = '\0';
#endif
	va_end( args );
	return ret;
}

GETOPT_API int getopt_create_context( getopt_context_t* ctx, int argc, const char** argv, const getopt_option_t* opts )
{
	ctx->argc            = (argc > 1) ? (argc - 1) : 0; /* stripping away file-name! */
	ctx->argv            = (argc > 1) ? (argv + 1) : argv; /* stripping away file-name! */
	ctx->opts            = opts;
	ctx->current_index   = 0;
	ctx->current_opt_arg = 0x0;

	/* count opts */
	ctx->num_opts = 0;
	const getopt_option_t* opt = opts;
	while( !(opt->name == 0x0 && opt->name_short == 0) )
	{
		if( opt->value == '!' || 
			opt->value == '?' || 
			opt->value == '+' || 
			opt->value == -1)
			return -1;

		ctx->num_opts++; opt++;
	}

	return 0;
}

GETOPT_API int getopt_next( getopt_context_t* ctx )
{
	/* are all options processed? */
	if(ctx->current_index == ctx->argc )
		return -1;

	/* reset opt-arg */
	ctx->current_opt_arg = 0x0;

	const char* curr_token = ctx->argv[ ctx->current_index ];
	
	/* this token has been processed! */
	ctx->current_index++;

	/* check if item is no option */
	if( curr_token[0] && curr_token[0] != '-' )
	{
		ctx->current_opt_arg = curr_token;
		return '+'; /* return '+' as identifier for no option! */
	}
	
	const getopt_option_t* found_opt = 0x0;
	const char* found_arg = 0x0;

	/* short opt */
	if( curr_token[1] != '\0' && curr_token[1] != '-' && curr_token[2] == '\0' )
	{
		int i = 0;
		for( ; i < ctx->num_opts; i++ )
		{
			const getopt_option_t* opt = ctx->opts + i;

			if( opt->name_short == curr_token[1] )
			{
				found_opt = opt;

				/* if there is an value when: - current_index < argc and value in argv[current_index] do not start with '-' */
				if( ( ( ctx->current_index != ctx->argc) && ( ctx->argv[ctx->current_index][0] != '-' ) ) && 
					  ( opt->type == GETOPT_OPTION_TYPE_OPTIONAL || opt->type == GETOPT_OPTION_TYPE_REQUIRED ) )
				{
					found_arg = ctx->argv[ctx->current_index];
					ctx->current_index++; /* next token has been processed aswell! */
				}
				break;
			}
		}
	}
	/* long opt */
	else if(curr_token[1] == '-' && curr_token[2] != '\0')
	{
		const char* check_option = curr_token + 2;

		int i = 0;
		for( ; i < ctx->num_opts; i++ )
		{
			const getopt_option_t* opt = ctx->opts + i;

			unsigned int name_len = (unsigned int)strlen( opt->name );

			if( getopt__str_case_cmp_len( opt->name, check_option, name_len ) == 0 )
			{
				check_option += name_len;

				/* find arg if there is any */
				switch( *check_option )
				{
					case '\0':
					{
						if( ctx->current_index < ctx->argc ) /* are there more tokens that can contain the '='? */
						{
							const char* next_token = ctx->argv[ ctx->current_index ];
							if( next_token[0] == '=' )
							{
								ctx->current_index++; /* next token has been processed aswell! */

								if( next_token[1] != '\0' ) /* does this token contain the arg-value? */
									found_arg = next_token + 1;
								else if( ctx->current_index < ctx->argc )
									found_arg = ctx->argv[ ctx->current_index++ ]; /* next token has been processed aswell! */
							}
							else if( next_token[0] != '-' )
							{
								ctx->current_index++; /* next token has been processed aswell! */
								found_arg = next_token;
							}
						}
					}
					break;
					case '=':
						if( check_option[1] != '\0' )
							found_arg = check_option + 1;
						else if( ctx->current_index < ctx->argc )
							found_arg = ctx->argv[ ctx->current_index++ ]; /* next token has been processed aswell! */
					break;
					default:
						/* not found, matched for example --test but it was --testing*/
						continue;
				}

				found_opt = opt;
				break;
			}
		}
	}
	/* malformed opt "-", "-xyz" or "--" */
	else
	{
		ctx->current_opt_arg = curr_token;
		return '!';
	}

	/* found no matching option! */
	if(found_opt == 0x0)
	{
		ctx->current_opt_arg = curr_token;
		return '?';
	}

	if(found_arg != 0x0)
	{
		switch(found_opt->type)
		{
			case GETOPT_OPTION_TYPE_FLAG_SET:
			case GETOPT_OPTION_TYPE_FLAG_AND:
			case GETOPT_OPTION_TYPE_FLAG_OR:
			case GETOPT_OPTION_TYPE_NO_ARG:
				/* these types should have no argument, usage error! */
				ctx->current_opt_arg = found_opt->name;
				return '!';

			case GETOPT_OPTION_TYPE_OPTIONAL:
			case GETOPT_OPTION_TYPE_REQUIRED:
				ctx->current_opt_arg = found_arg;
				return found_opt->value;
		}
	}
	/* no argument found */
	else
	{
		switch(found_opt->type)
		{
			case GETOPT_OPTION_TYPE_FLAG_SET: *found_opt->flag  = found_opt->value; return 0;
			case GETOPT_OPTION_TYPE_FLAG_AND: *found_opt->flag &= found_opt->value; return 0;
			case GETOPT_OPTION_TYPE_FLAG_OR:  *found_opt->flag |= found_opt->value; return 0; /* zero is "a flag was set!" */
			
			case GETOPT_OPTION_TYPE_NO_ARG:
			case GETOPT_OPTION_TYPE_OPTIONAL:
				return found_opt->value;

			/* the option requires an argument! (--option=arg, -o arg) */
			case GETOPT_OPTION_TYPE_REQUIRED:
					ctx->current_opt_arg = found_opt->name;
					return '!';
		}
	}

 	return -1;
}

GETOPT_API const char* getopt_create_help_string( getopt_context_t* ctx, char* buffer, size_t buffer_size )
{
	size_t buffer_pos = 0;
	int    opt_index  = 0;
	for( ; opt_index < ctx->num_opts; ++opt_index )
	{
		const getopt_option_t* opt = ctx->opts + opt_index;

		size_t outpos;
		char long_name[64];
		int chars_written = getopt__str_format( long_name, 64, "--%s", opt->name );
		if( chars_written < 0 )
			return buffer;

		outpos = (size_t)chars_written;

		switch( opt->type )
		{
			case GETOPT_OPTION_TYPE_REQUIRED:
				getopt__str_format(long_name + outpos, 64 - outpos, "=<%s>", opt->value_desc);
				break;
			case GETOPT_OPTION_TYPE_OPTIONAL:
				getopt__str_format(long_name + outpos, 64 - outpos, "(=%s)", opt->value_desc);
				break;
			default:
				break;
		}

		if(opt->name_short == 0x0)
			chars_written = getopt__str_format( buffer + buffer_pos, buffer_size - buffer_pos, "   %-32s - %s\n", long_name, opt->desc );
		else
			chars_written = getopt__str_format( buffer + buffer_pos, buffer_size - buffer_pos, "-%c %-32s - %s\n", opt->name_short, long_name, opt->desc);

		if( chars_written < 0 )
			return buffer;
		
		buffer_pos += (size_t)chars_written;
	}

	return buffer;
}
