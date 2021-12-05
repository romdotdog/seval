#include <stdlib.h>
#include <stdio.h>
#include "number.h"

/*
 * NOTE: 4-slash comments are made from @fabriciopashaj
 */

static int gcd(int64_t a, int64_t b)
{
    //// llabs cuz' labs is for long while int64_t is long long
    //// at least on my device
	return b == 0 ? llabs(a) : gcd(b, a % b);
}

static int try_add(int64_t *r, int64_t a, int64_t b)
{
	*r = a + b;

    //// just use ternary operator
	// overflow invariants
	return ((*r < 0 && a > 0 && b > 0) || (*r > 0 && a < 0 && b < 0)) ?
            -1 : 0;
}

static int try_mul(int64_t *r, int64_t a, int64_t b)
{
	*r = a * b;

    //// it's enough simple to just use ternary
	// overflow invariants
	return (a != 0 && *r / a != b) ? -1 : 0;
}

// returns 0 if the symbol is now an integer
int seval_simplify(symbol_t *symbol)
{
    //// return spaghetti yuck
    //// embrace single-entry-single-exit functions
    int s = 0;
	switch (symbol->type)
	{
	case AT_INTEGER:
		break;
	case AT_ADD:
		if (seval_simplify(symbol->operation.lhs) == 0 &&
            seval_simplify(symbol->operation.rhs) == 0)
		{
			int64_t r;
			if (try_add(&r, symbol->operation.lhs->integer,
                        symbol->operation.rhs->integer) == 0)
			{
				free(symbol->operation.lhs);
				free(symbol->operation.rhs);

				symbol->type = AT_INTEGER;
				symbol->integer = r;
				break;
			}
		}
		s = -1;
        break;
	case AT_MULTIPLY:
		// TODO: distributive property
		if (seval_simplify(symbol->operation.lhs) == 0 &&
            seval_simplify(symbol->operation.rhs) == 0)
		{
			int64_t r;
			if (try_mul(&r, symbol->operation.lhs->integer,
                        symbol->operation.rhs->integer) == 0)
			{
				free(symbol->operation.lhs);
				free(symbol->operation.rhs);

				symbol->type = AT_INTEGER;
				symbol->integer = r;
				break;
			}
		}
        s = -1;
        break;
	case AT_DIVIDE:
		// TODO: distributive property
		if (seval_simplify(symbol->operation.lhs) == 0 &&
            seval_simplify(symbol->operation.rhs) == 0)
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
			}
			else
			{
				int64_t f = gcd(a, b);
				symbol->operation.lhs->integer /= f;
				symbol->operation.rhs->integer /= f;
				s = -1;
			}
		}
		break;
    //// I think the problem was that enums are basically just numbers
    //// and the compiler wants a default fallback
    default:
        s = -1;
	    printf("encountered unimplemented ast type");
	}

    return s;
}
