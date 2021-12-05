#include "number.h"
typedef struct printer
{
	char *buf;
	size_t offset;
	size_t cap;
} printer_t;

int seval_print(symbol_t *res);