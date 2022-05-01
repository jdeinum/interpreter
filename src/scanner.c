#include <stdio.h>
#include <string.h>

#include "../include/common.h"
#include "../include/scanner.h"

typedef struct {
	const char* start; // start of the lexame
	const char* current; // current character
	int line; // line for error reporting
} Scanner;

Scanner scanner; // global scanner so we don't have to pass it around


void initScanner(const char* source) {
	scanner.start = source;
	scanner.current = source;
	scanner.line = 1;
}

// reached end of source file
static bool isAtEnd() {
	return *scanner.current == '\0';
}

// create a token to return to our compiler
static Token makeToken(TokenType type) {
	Token token;
	token.type = type;
	token.start = scanner.start;
	token.length = (int)(scanner.current - scanner.start);
	token.line = scanner.line;
	return token;
}

// return an error token with a specific error message
static Token errorToken(const char* message) {
	Token token;
	token.type = TOKEN_ERROR;
	token.start = message;
	token.length = (int)strlen(message);
	token.line = scanner.line;
	return token;
}

// return a token and simultaneously advance the scanner to the next token
static char advance() {
	scanner.current++;
	return scanner.current[-1];
}

// match the next character to check for 2 character tokens, i.e <= , !=, ...
static bool match(char expected) {
	if (isAtEnd()) return false;
	if (*scanner.current != expected) return false;
		
	scanner.current++;
	return true;
}

// check the current character in the source code 
static char peek() {
	return *scanner.current;
}

// look at the next character 
static char peekNext() {
  if (isAtEnd()) return '\0';
  return scanner.current[1];
}

// skip over whitespace, as well as comments
static void skipWhitespace() {
	for (;;) {

		// look at the next character
		char c = peek();
		switch (c) {

			// skip over spaces and tabs
			case ' ':
			case '\r':
			case '\t':
				advance();
				break;

			// skip over newlines, but increment the line count for debugging 
			// purposes
			case '\n':
				scanner.line++;
				advance();
				break;

			// remove comments
			case '/':
				if (peekNext() == '/') {
					// A comment goes until the end of the line.
					// Don't consume the newline, since we want SkipWhitespace
					// to increment our line count
					while (peek() != '\n' && !isAtEnd()) advance();
				} else {
					return;
				}

				break;

			default:
				return;
		}
	}
}

static Token string() {
	while (peek() != '"' && !isAtEnd()) {
		if (peek() == '\n') scanner.line++;
		advance();
	}

	if (isAtEnd()) return errorToken("Unterminated string.");

	// The closing quote.
	advance();
	return makeToken(TOKEN_STRING);
}

static bool isDigit(char c) {
	return c >= '0' && c <= '9';
}

// looking at either an integer or a floating point number
static Token number() {
	while (isDigit(peek())) advance();

	// Look for a fractional part.
	if (peek() == '.' && isDigit(peekNext())) {
		// Consume the ".".
		advance();

		while (isDigit(peek())) advance();
	}

	return makeToken(TOKEN_NUMBER);
}

static bool isAlpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}


static TokenType checkKeyword(int start, int length, const char* rest,
		TokenType type) {
	if (scanner.current - scanner.start == start + length &&
			memcmp(scanner.start + start, rest, length) == 0) {
		return type;
	}

	return TOKEN_IDENTIFIER;
}

// match identifiers using a trie, i.e just go letter by letter, as soon as you 
// can see that it isn't a built in identifier, you can tell the compiler that
// it's a user defined identifier
static TokenType identifierType() {
	switch (scanner.start[0]) {
		case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
		case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
		case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
		case 'f':
			if (scanner.current - scanner.start > 1) {
				switch (scanner.start[1]) {
					case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
					case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
					case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
				}
			}
			break;
		case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
		case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
		case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
		case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
		case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
		case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
		case 't':
			if (scanner.current - scanner.start > 1) {
				switch (scanner.start[1]) {
					case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
					case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
				}
			}
			break;
		case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
		case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
	}

	return TOKEN_IDENTIFIER;
}


static Token identifier() {
	while (isAlpha(peek()) || isDigit(peek())) advance();
	return makeToken(identifierType());
}


// return the type of token we are dealing with
Token scanToken() {

	skipWhitespace();

	scanner.start = scanner.current; // current points at the first character
									 //	of the lexom

	if (isAtEnd()) return makeToken(TOKEN_EOF);
	
	// get the current character while simuletaneously incrementing the pointer
	char c = advance();

	if (isAlpha(c)) return identifier();

	if (isDigit(c)) return number();

	switch (c) {

		// single character tokens
		case '(': return makeToken(TOKEN_LEFT_PAREN);
		case ')': return makeToken(TOKEN_RIGHT_PAREN);
		case '{': return makeToken(TOKEN_LEFT_BRACE);
		case '}': return makeToken(TOKEN_RIGHT_BRACE);
		case ';': return makeToken(TOKEN_SEMICOLON);
		case ',': return makeToken(TOKEN_COMMA);
		case '.': return makeToken(TOKEN_DOT);
		case '-': return makeToken(TOKEN_MINUS);
		case '+': return makeToken(TOKEN_PLUS);
		case '/': return makeToken(TOKEN_SLASH);
		case '*': return makeToken(TOKEN_STAR);


		// 2 character tokens
		case '!':
			return makeToken(
					match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
		case '=':
			return makeToken(
					match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
		case '<':
			return makeToken(
					match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
		case '>':
			return makeToken(
					match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);

		// strings
		case '"': return string();


	}

	return errorToken("Unexpected character.");
}


