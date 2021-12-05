#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

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
    int s = 0;
#define num ctx->tk.v.operation.lhs
#define denom ctx->tk.v.operation.rhs
	enum
	{
		INTEGER,
		FRACTIONAL
	} state = INTEGER;

	ctx->tk.type = TK_NUMBER;
	ctx->tk.v.type = AT_DIVIDE;

    //// I wouldn't use malloc so freely, but anyway
	num = malloc(sizeof(symbol_t));
	num->type = AT_INTEGER;
	num->integer = 0;

	denom = malloc(sizeof(symbol_t));
	denom->type = AT_INTEGER;
	denom->integer = 1;

	char c = ctx->current;
    bool finished = false;
	do
	{
		switch (c)
		{
		case '0'...'9':;
            //// I woud buffer the number and call strtoll on the buffer
            //// but whatever
			int val = c - '0';
            //// this is much simpler
			if (state == FRACTIONAL)
                denom->integer *= 10;
			num->integer = num->integer * 10 + val;
			c = next(ctx);
			break;
		case '.':
			if (state == FRACTIONAL)
			{
				printf("error: two decimal points found in number");
                //// now you need to decide, either 1 or -1 for failure
                //// doing a status_t enum would be better imo
				s = 1;
                break;
			}

			state = FRACTIONAL;
			c = next(ctx);
			break;
        default:
            finished = true;
		}
	} while (!finished && ctx->offset < ctx->size);

	return s;
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

//// we return status and pass the entire token to function, not just
//// symbol. aslo we need some sort of common header that will have
//// common definitions, like a status_t instead of just int
static int lex(lexer_t *ctx, token_t *token)
{
    int s = 0;
    tokentype_t tk_type = TK_NONE;
	if (ctx->offset >= ctx->size)
    {
        //// let's make -2 the status to tell that we're finished
        s = -2;
    } //// did the else like this cuz' that's how you did it
    else
    {
        switch (ctx->current)
        {
        case '+':
            next(ctx);
            tk_type = TK_PLUS;
            break;
        case '-':
            next(ctx);
            tk_type =  TK_MINUS;
            break;
        case '*':
            next(ctx);
            tk_type = TK_ASTERISK;
            break;
        case '/':
            next(ctx);
            tk_type = TK_SLASH;
            break;
        case '(':
            next(ctx);
            tk_type = TK_LEFTPAREN;
            break;
        case ')':
            next(ctx);
            tk_type = TK_RIGHTPAREN;
            break;
        case '0' ... '9':
        case '.':
            read_numeral(ctx);
            tk_type = TK_NUMBER;
            break;
        case ' ':
        case '\t':
        case '\n':
            next(ctx);
            //// this recursion thing is not good, you should probably
            //// put the switch in a loop or smth
            s = lex(ctx, token);
            tk_type = token->type;
            break;
        default:
	        printf("error: unexpected token '%c'\n", ctx->current);
        }
    }

	token->type = tk_type;
    return s;
}

//// doing statuses means doing that up until the top level, not just
//// in 1 or 2 functions, the top level will report errors to the user
//// just an advice
void seval_lex_next(lexer_t *ctx)
{
	if (ctx->lookahead.type != TK_NONE)
	{
		free_tk(ctx);
		ctx->tk = ctx->lookahead;
		ctx->lookahead.type = TK_NONE;
    //// bruh not java, but if you want this style...
	}
    else
    {
		lex(ctx, &ctx->tk);
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
	    lex(ctx, &ctx->lookahead);
	}
}
