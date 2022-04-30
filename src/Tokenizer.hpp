#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <cstddef>
#include <cstdint>

struct Token
{
  enum Type
  {
    COLON,
    SEMICOLON,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    COMMA,

    ASSIGN,
    OR,
    AND,
    EQUAL,
    DIFFERENT,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    PLUS,
    MINUS,
    MULT,
    DIVIDE,
    MOD,

    IF,
    INT32_TYPE,
    ELSE,
    FUN,
    RETURN,
    WHILE,

    IDENTIFIER,
    INTEGER,

    END_OF_FILE,
    TOKEN_COUNT
  };

  Token::Type type;
  const char *begin;
  size_t size;

  uint32_t prec_level() const;
};

#define TOKENIZER_BUFFER_LIMIT 4

struct Tokenizer
{
  const char *at;
  Token buffer[TOKENIZER_BUFFER_LIMIT];
  uint32_t start;
  uint32_t count;

  Token peek(uint32_t lookahead);

  Token next_token();

  void consume();

private:
  static const Token token_list[Token::TOKEN_COUNT - 3];

  Token get_token();
};

#endif // TOKENIZER_HPP
