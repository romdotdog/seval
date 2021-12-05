#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define next(ctx) (ctx->current = ctx->src[ctx->offset++])
#define peek(ctx) (ctx->src[ctx->offset + 1])

void seval_lex_init(lexer_t *ctx, char *src, size_t size)
{
	ctx->src = src;
	ctx->size = size;
	ctx->offset = 0;
	ctx->lookahead.type = TK_NONE;
	ctx->tk.type = TK_NONE;
	ctx->current = next(ctx);
}

static int read_numeral(lexer_t *ctx)
{
#define num ctx->tk.v.operation.lhs
#define denom ctx->tk.v.operation.rhs
	enum
	{
		INTEGER,
		FRACTIONAL
	} state = INTEGER;

	ctx->tk.type = TK_NUMBER;
	ctx->tk.v.type = AT_DIVIDE;

	num = malloc(sizeof(symbol_t));
	num->type = AT_INTEGER;
	num->integer = 0;

	denom = malloc(sizeof(symbol_t));
	denom->type = AT_INTEGER;
	denom->integer = 1;

	char c = ctx->current;
	do
	{
		switch (c)
		{
		case '0' ... '9':;
			int val = c - '0';
			switch (state)
			{
			case FRACTIONAL:
				denom->integer *= 10;
			case INTEGER:
				num->integer = num->integer * 10 + val;
				break;
			}
			c = next(ctx);
			break;
		case '.':
			if (state == FRACTIONAL)
			{
				printf("error: two decimal points found in number");
				return 1;
			}

			state = FRACTIONAL;
			c = next(ctx);
			break;
		default:
			return 0;
		}
	} while (ctx->offset < ctx->size);

	return 0;
#undef num
#undef denom
}

static void free_tk(lexer_t *ctx)
{
	if (ctx->tk.type == TK_NUMBER)
	{
		free(ctx->tk.v.operation.lhs);
		free(ctx->tk.v.operation.rhs);
	}
}

static tokentype_t lex(lexer_t *ctx, symbol_t *symbol)
{
	if (ctx->offset >= ctx->size)
		return TK_NONE;

	switch (ctx->current)
	{
	case '+':
		next(ctx);
		return TK_PLUS;
	case '-':
		next(ctx);
		return TK_MINUS;
	case '*':
		next(ctx);
		return TK_ASTERISK;
	case '/':
		next(ctx);
		return TK_SLASH;
	case '(':
		next(ctx);
		return TK_LEFTPAREN;
	case ')':
		next(ctx);
		return TK_RIGHTPAREN;
	case '0' ... '9':
	case '.':
		read_numeral(ctx);
		return TK_NUMBER;
	case ' ':
	case '\t':
	case '\n':
		next(ctx);
		return lex(ctx, symbol);
	}

	printf("error: unexpected token '%c'", ctx->current);
	return TK_NONE;
}

void seval_lex_next(lexer_t *ctx)
{
	if (ctx->lookahead.type != TK_NONE)
	{
		free_tk(ctx);
		ctx->tk = ctx->lookahead;
		ctx->lookahead.type = TK_NONE;
	}
	else
	{
		ctx->tk.type = lex(ctx, &ctx->tk.v);
	}
}

void seval_lex_peek(lexer_t *ctx)
{
	if (ctx->lookahead.type != TK_NONE)
	{
		printf("warn: internal: peeked twice");
	}
	else
	{
		ctx->lookahead.type = lex(ctx, &ctx->lookahead.v);
	}
}
