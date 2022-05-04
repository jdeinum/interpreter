#include <stdio.h>
#include <stdlib.h>

#include "../include/common.h"
#include "../include/compiler.h"
#include "../include/scanner.h"
#include "../include/debug.h"


typedef struct {
	Token current;
	Token previous; // keep track of both the current and previous tokens
	bool hadError;
	bool panicMode;
} Parser;


// since enum options are assigned inccrementing values, we have our full
// precedence table here.
typedef enum {
	PREC_NONE,
	PREC_ASSIGNMENT,  // =
	PREC_OR,          // or
	PREC_AND,         // and
	PREC_EQUALITY,    // == !=
	PREC_COMPARISON,  // < > <= >=
	PREC_TERM,        // + -
	PREC_FACTOR,      // * /
	PREC_UNARY,       // ! -
	PREC_CALL,        // . ()
	PREC_PRIMARY
} Precedence;


// C's syntax for function pointers are a bit unclear
// Just hide it behind a typedef to make it more clear.
typedef void (*ParseFn)();


// TODO
typedef struct {
	ParseFn prefix;
	ParseFn infix;
	Precedence precedence;
} ParseRule;


// some definitions
static void expression();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);


// Global Declarations that we'll use
Parser parser;
Chunk* compilingChunk;

static Chunk* currentChunk() {
	return compilingChunk;
}

static void errorAt(Token* token, const char* message) {
	if (parser.panicMode) return; // handle one error at a time
	parser.panicMode = true;
	fprintf(stderr, "[line %d] Error", token->line);

	if (token->type == TOKEN_EOF) {
		fprintf(stderr, " at end");
	} else if (token->type == TOKEN_ERROR) {
		// Nothing.
	} else {
		fprintf(stderr, " at '%.*s'", token->length, token->start);
	}

	fprintf(stderr, ": %s\n", message);
	parser.hadError = true;
}


// error at the current token
static void errorAtCurrent(const char* message) {
	errorAt(&parser.current, message);
}

// error at the previous token, i.e we didn't find something we were expecting
static void error(const char* message) {
	errorAt(&parser.previous, message);
}

// advance to the next token, saving the previous token in case we need to 
// remember what it is
static void advance() {
	parser.previous = parser.current;

	for (;;) {
		parser.current = scanToken();
		if (parser.current.type != TOKEN_ERROR) break;

		errorAtCurrent(parser.current.start);
	}
}

// check whether the current token is one we expect, if so, we can  safely move
// on to the next token, otherwise, we throw an error.
static void consume(TokenType type, const char* message) {
	if (parser.current.type == type) {
		advance();
		return;
	}

	errorAtCurrent(message);
}

// used to push bytes into our virtual chunks
static void emitByte(uint8_t byte) {
	writeChunk(currentChunk(), byte, parser.previous.line);
}

// used for basic testing facilties
static void emitReturn() {
	emitByte(OP_RETURN);
}


// used for basic testing facilties
static void endCompiler() {
	emitReturn();

	#ifdef DEBUG_PRINT_CODE
	if (!parser.hadError) {
		disassembleChunk(currentChunk(), "code");
	}
	#endif
}




// many tokens will require us to push 2 values on our chunk stack
static void emitBytes(uint8_t byte1, uint8_t byte2) {
	emitByte(byte1);
	emitByte(byte2);
}

// add an OP_CONSTANT as well as the constant itself to our chunk, return
// the index so we can later retrieve it.
static uint8_t makeConstant(Value value) {
	int constant = addConstant(currentChunk(), value);
	if (constant > UINT8_MAX) {
		error("Too many constants in one chunk.");
		return 0;
	}

	return (uint8_t)constant;
}


// TODO
static void emitConstant(Value value) {
	emitBytes(OP_CONSTANT, makeConstant(value));
}


// Parse tokens until we find a token with a lower precedence. Use the correct 
// rule to emit the correct bytecode
static void parsePrecedence(Precedence precedence) {
	advance();
	ParseFn prefixRule = getRule(parser.previous.type)->prefix;
	if (prefixRule == NULL) {
	  error("Expect expression.");
	  return;
	}

	prefixRule();

	while (precedence <= getRule(parser.current.type)->precedence) {
		advance();
		ParseFn infixRule = getRule(parser.previous.type)->infix;
		infixRule();
	}
}


// parse a single expression
static void expression() {
	parsePrecedence(PREC_ASSIGNMENT);
}

// convert the 'number' to a usable value for clox
static void number() {
	double value = strtod(parser.previous.start, NULL);
	emitConstant(NUMBER_VAL(value));
}


// only a single operand
static void unary() {
	TokenType operatorType = parser.previous.type;

	// Compile the operand.
	parsePrecedence(PREC_UNARY);

	// Emit the operator instruction.
	switch (operatorType) {
		case TOKEN_MINUS: emitByte(OP_NEGATE); break;
		case TOKEN_BANG: emitByte(OP_NOT); break;
		default: return; // Unreachable.
	}
}

// a grouped expression is of the form ( expr1, expr2, .. )
// we simply chew through the opening parenthesis and treat the rest as an 
// expression
static void grouping() {
	expression();
	consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression");
}

// binary function
static void binary() {
	TokenType operatorType = parser.previous.type;
	ParseRule* rule = getRule(operatorType);

	// +1 since we are using left associativity, i.e
	// 1 + 2 + 3 = ((1 + 2) + 3) , so we need a precedence level 1 higher than 
	// the current operation
	parsePrecedence((Precedence) (rule->precedence + 1));

	// we can represent >= , != , and <= as negations of the remaining
	// operators
	switch (operatorType) {
		case TOKEN_PLUS: emitByte(OP_ADD); break;
		case TOKEN_MINUS: emitByte(OP_SUBTRACT);break;
		case TOKEN_STAR: emitByte(OP_MULTIPLY); break;
		case TOKEN_SLASH: emitByte(OP_DIVIDE); break;
		case TOKEN_BANG_EQUAL:    emitBytes(OP_EQUAL, OP_NOT); break;
		case TOKEN_EQUAL_EQUAL:   emitByte(OP_EQUAL); break;
		case TOKEN_GREATER:       emitByte(OP_GREATER); break;
		case TOKEN_GREATER_EQUAL: emitBytes(OP_LESS, OP_NOT); break;
		case TOKEN_LESS:          emitByte(OP_LESS); break;
		case TOKEN_LESS_EQUAL:    emitBytes(OP_GREATER, OP_NOT); break;
		default: return;
	}
}

static void literal() {
	switch (parser.previous.type) {
	  case TOKEN_FALSE: emitByte(OP_FALSE); break;
	  case TOKEN_NIL: emitByte(OP_NIL); break;
	  case TOKEN_TRUE: emitByte(OP_TRUE); break;
	  default: return; // Unreachable.
	}
}

// get the strings characters directly from the lexeme / source
// +1 / -2 to trim the quotation marks
static void string() {
	emitConstant(OBJ_VAL(copyString(parser.previous.start + 1,
                                  parser.previous.length - 2)));
}

// specify which rules we use for each token
// columns: prefix rule | infix rules | precedence 
ParseRule rules[] = {
	[TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
	[TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
	[TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE},
	[TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
	[TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
	[TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
	[TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
	[TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
	[TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
	[TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
	[TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
	[TOKEN_BANG]          = {unary,    NULL,   PREC_NONE},
	[TOKEN_BANG_EQUAL]    = {NULL,     binary, PREC_EQUALITY},
	[TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
	[TOKEN_EQUAL_EQUAL]   = {NULL,     binary,   PREC_EQUALITY},
	[TOKEN_GREATER]       = {NULL,     binary,   PREC_COMPARISON},
	[TOKEN_GREATER_EQUAL] = {NULL,     binary,   PREC_COMPARISON},
	[TOKEN_LESS]          = {NULL,     binary,   PREC_COMPARISON},
	[TOKEN_LESS_EQUAL]    = {NULL,     binary,   PREC_COMPARISON},
	[TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
	[TOKEN_STRING]        = {string,   NULL,   PREC_NONE},
	[TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
	[TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
	[TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
	[TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
	[TOKEN_FALSE]         = {literal,  NULL,   PREC_NONE},
	[TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
	[TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
	[TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
	[TOKEN_NIL]           = {literal,  NULL,   PREC_NONE},
	[TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
	[TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
	[TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
	[TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
	[TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
	[TOKEN_TRUE]          = {literal,  NULL,   PREC_NONE},
	[TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
	[TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
	[TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
	[TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};


// find which function we use to handle this token
static ParseRule* getRule(TokenType type) {
	return &rules[type];
}



// driver function for our compiler
bool compile(const char* source, Chunk* chunk) {
	initScanner(source);

	parser.hadError = false;
	parser.panicMode = false;
	compilingChunk = chunk;

	advance(); // move to first token
	expression();
	consume(TOKEN_EOF, "Expect end of expression.");


	endCompiler();
	return !parser.hadError;
}

