#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

int prec[6] = {-1, 2, 2, 1, 1, -1};
asttype_t asttypes[6] = {-1, AT_ADD, -1, AT_MULTIPLY, AT_DIVIDE};

// res should be allocated, but operands shouldn't be
static int
primaryexpr(lexer_t *ctx, symbol_t *res)
{
	seval_lex_next(ctx);
	switch (ctx->tk.type)
	{
	case TK_NONE:
		return -1;
	case TK_LEFTPAREN:
		if (seval_parse_expr(ctx, res) != 0)
			return -1;

		seval_lex_next(ctx);
		if (ctx->tk.type != TK_RIGHTPAREN)
		{
			printf("error: expected ')'");
			return -1;
		}

		return 0;
	case TK_MINUS:
		res->type = AT_MULTIPLY;
		res->operation.lhs = malloc(sizeof(symbol_t));
		res->operation.lhs->type = AT_INTEGER;
		res->operation.lhs->integer = -1;

		res->operation.rhs = malloc(sizeof(symbol_t));
		if (primaryexpr(ctx, res->operation.rhs) != 0)
			return -1;
		return 0;
	case TK_NUMBER:
		// clone
		res->type = AT_DIVIDE;
		res->operation.lhs = malloc(sizeof(symbol_t));
		res->operation.rhs = malloc(sizeof(symbol_t));

		memcpy(res->operation.lhs, ctx->tk.v.operation.lhs, sizeof(symbol_t));
		memcpy(res->operation.rhs, ctx->tk.v.operation.rhs, sizeof(symbol_t));
		return 0;
	default:
		printf("error: unexpected token in primaryexpr");
		return -1;
	}
}

static int subexpr(lexer_t *ctx, symbol_t *lhs, int min_prec)
{
	seval_lex_peek(ctx);
	while (prec[ctx->lookahead.type] > min_prec)
	{
		seval_lex_next(ctx);
		tokentype_t op = ctx->tk.type;
		symbol_t *rhs = malloc(sizeof(symbol_t));
		primaryexpr(ctx, rhs);

		seval_lex_peek(ctx);
		while (prec[ctx->lookahead.type] > prec[op])
		{
			subexpr(ctx, rhs, prec[op]);
			seval_lex_peek(ctx);
		}

		if (op == TK_MINUS)
		{
			symbol_t *buf = malloc(sizeof(symbol_t));
			buf->type = rhs->type;
			buf->operation = rhs->operation;

			rhs->type = AT_MULTIPLY;
			rhs->operation.lhs = malloc(sizeof(symbol_t));
			rhs->operation.lhs->type = AT_INTEGER;
			rhs->operation.lhs->integer = -1;
			rhs->operation.rhs = buf;

			op = TK_PLUS;
		}

		symbol_t *buf = malloc(sizeof(symbol_t));
		buf->type = lhs->type;
		buf->operation = lhs->operation;

		lhs->type = asttypes[op];
		lhs->operation.lhs = buf;
		lhs->operation.rhs = rhs;
	}
	return 0;
}

int seval_parse_expr(lexer_t *ctx, symbol_t *res)
{
	if (primaryexpr(ctx, res) != 0)
		return -1;
	return subexpr(ctx, res, 0);
}