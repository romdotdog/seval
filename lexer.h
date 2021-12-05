#include "number.h"
#include <stddef.h>

//// why would you put the brace at a new line this ain't java
typedef enum tokentype
{
	TK_NONE,
	TK_PLUS,
	TK_MINUS,
	TK_ASTERISK,
	TK_SLASH,
	TK_NUMBER,
	TK_LEFTPAREN,
	TK_RIGHTPAREN
} tokentype_t;

typedef struct token
{
	tokentype_t type;
	symbol_t v;
} token_t;

typedef struct lexer
{
	char *src;
    //// a bit useless tho, the function should keep it as a local variable
    //// so that the compiler might do opts like use registers
	char current;
	size_t size;
	size_t offset;
	token_t lookahead;
	token_t tk;
} lexer_t;

void seval_lex_init(lexer_t *ctx, char *src, size_t size);
void seval_lex_peek(lexer_t *ctx);
void seval_lex_next(lexer_t *ctx);
