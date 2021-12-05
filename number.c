#include <stdlib.h>
#include <stdio.h>
#include "number.h"

static int gcd(int64_t a, int64_t b)
{
	return b == 0 ? labs(a) : gcd(b, a % b);
}

static int try_add(int64_t *r, int64_t a, int64_t b)
{
	*r = a + b;

	// overflow invariants
	if ((*r < 0 && a > 0 && b > 0) || (*r > 0 && a < 0 && b < 0))
		return -1;
	return 0;
}

static int try_mul(int64_t *r, int64_t a, int64_t b)
{
	*r = a * b;

	// overflow invariants
	if (a != 0 && *r / a != b)
		return -1;
	return 0;
}

// returns 0 if the symbol is now an integer
int seval_simplify(symbol_t *symbol)
{
	switch (symbol->type)
	{
	case AT_INTEGER:
		return 0;
	case AT_ADD:
		if (seval_simplify(symbol->operation.lhs) == 0 && seval_simplify(symbol->operation.rhs) == 0)
		{
			int64_t r;
			if (try_add(&r, symbol->operation.lhs->integer, symbol->operation.rhs->integer) == 0)
			{
				free(symbol->operation.lhs);
				free(symbol->operation.rhs);

				symbol->type = AT_INTEGER;
				symbol->integer = r;
				return 0;
			}
		}
		return -1;
	case AT_MULTIPLY:
		// TODO: distributive property
		if (seval_simplify(symbol->operation.lhs) == 0 && seval_simplify(symbol->operation.rhs) == 0)
		{
			int64_t r;
			if (try_mul(&r, symbol->operation.lhs->integer, symbol->operation.rhs->integer) == 0)
			{
				free(symbol->operation.lhs);
				free(symbol->operation.rhs);

				symbol->type = AT_INTEGER;
				symbol->integer = r;
				return 0;
			}
		}
		return -1;
	case AT_DIVIDE:
		// TODO: distributive property
		if (seval_simplify(symbol->operation.lhs) == 0 && seval_simplify(symbol->operation.rhs) == 0)
		{
			int64_t a = symbol->operation.lhs->integer;
			int64_t b = symbol->operation.rhs->integer;

			// if it divides evenly
			if (a % b == 0)
			{
				int64_t r = a / b;
				free(symbol->operation.lhs);
				free(symbol->operation.rhs);

				symbol->type = AT_INTEGER;
				symbol->integer = r;
				return 0;
			}
			else
			{
				int64_t f = gcd(a, b);
				symbol->operation.lhs->integer /= f;
				symbol->operation.rhs->integer /= f;
				return -1;
			}
		}
		return -1;
	}

	printf("encountered unimplemented ast type");
	return -1;
}