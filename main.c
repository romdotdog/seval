#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "parser.h"
#include "display.h"

int main()
{
	printf("seval v1.0\n");
	while (1)
	{
	reset:
		printf(">> ");

		char *src = NULL;
		size_t buf_size;
		ssize_t size = getline(&src, &buf_size, stdin);
		if (size <= 0)
		{
			printf("no source to parse.\n");
			goto reset;
		}

		lexer_t lex;
		seval_lex_init(&lex, src, size);
		symbol_t ast;
		seval_parse_expr(&lex, &ast);
		seval_simplify(&ast);
		seval_print(&ast);
	}
}