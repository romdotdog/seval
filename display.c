#include <stdlib.h>
#include <stdio.h>
#include "display.h"

// totally not stolen from https://github.com/fabriciopashaj/c-vector/blob/main/lib.h
#ifndef ALIGNMENT
#define ALIGNMENT ((int)sizeof(size_t))
#endif
#define ALIGN(s) (((s) + (ALIGNMENT)-1) & ~((ALIGNMENT)-1))

static unsigned int llog10(int64_t x)
{
#define log10c(i) (x >= 1e##i) ? i
	x = x < 0 ? -x : x;
    //// I'm not gonna touch this, it's outside of my perception
    //// and I don't know what you were thinking when you did this
	return log10c(18) : log10c(17) : log10c(16) : log10c(15) : log10c(14) : log10c(13) : log10c(12) : log10c(11) : log10c(10) : log10c(9) : log10c(8) : log10c(7) : log10c(6) : log10c(5) : log10c(4) : log10c(3) : log10c(2) : log10c(1) : 0;
#undef log10c
}

static void expand(printer_t *ctx)
{
	ctx->cap = ALIGN(ctx->cap << 1);
    //// you need to check for realloc failure, very important
    //// but maybe latter
	ctx->buf = realloc(ctx->buf, ctx->cap);
}

static int traverse(printer_t *ctx, symbol_t *res)
{
	switch (res->type)
	{
	case AT_DIVIDE:
	{
		traverse(ctx, res->operation.lhs);
		int max_size = 4;
		if (ctx->cap < ctx->offset + max_size)
			expand(ctx);
		ctx->offset += snprintf(ctx->buf + ctx->offset, max_size, " / ");
		traverse(ctx, res->operation.rhs);
		// return 0;
        //// you migh as well just break, you are returning 0 anyways
        break;
	}
	case AT_INTEGER:
	{
		int max_size = llog10(res->integer) + (res->integer < 0) + 2;
		if (ctx->cap < ctx->offset + max_size)
			expand(ctx);
        //// pls keep line width < 80 characters
		ctx->offset += snprintf(ctx->buf + ctx->offset,
                                max_size, "%" PRId64, res->integer);
		//return 0;
        break;
	}
    default:;
	}
	return 0;
}

int seval_print(symbol_t *res)
{
	printer_t printer;
	printer.cap = ALIGN(sizeof(size_t));
	printer.buf = calloc(printer.cap, sizeof(char));
	printer.offset = 0;
	traverse(&printer, res);
	printf("%s\n", printer.buf);
    //// memory leak, you need to free printer.buf and return something
    return 0;
    //// idk if you want a free, I'll just leave it commented
    //// free(printer.buf);
}
