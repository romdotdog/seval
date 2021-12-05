#include <inttypes.h>
#pragma once

typedef enum asttype
{
	AT_INTEGER,
	AT_ADD,
	AT_MULTIPLY,
	AT_DIVIDE
} asttype_t;

typedef struct operation
{
	struct symbol *lhs;
	struct symbol *rhs;
} operation_t;

typedef struct symbol
{
	asttype_t type;
	union
	{
		int64_t integer;
		operation_t operation;
	};
} symbol_t;

int seval_simplify(symbol_t *symbol);
