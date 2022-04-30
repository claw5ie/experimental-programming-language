#include <cassert>
#include <cctype>
#include <cstring>
#include "Tokenizer.hpp"

uint32_t Token::prec_level() const
{
  switch (type)
  {
  case ASSIGN:
    return 0;
  case OR:
    return 1;
  case AND:
    return 2;
  case EQUAL:
  case DIFFERENT:
    return 3;
  case LESS:
  case LESS_EQUAL:
  case GREATER:
  case GREATER_EQUAL:
    return 4;
  case PLUS:
  case MINUS:
    return 5;
  case MULT:
  case DIVIDE:
  case MOD:
    return 6;
  default:
    // Nothing to see here.
    ;
  }

  return uint32_t(-1);
}

Token Tokenizer::peek(uint32_t lookahead)
{
  assert(lookahead < TOKENIZER_BUFFER_LIMIT);

  while (lookahead >= count)
  {
    buffer[(start + count) % TOKENIZER_BUFFER_LIMIT] = get_token();
    count++;
  }

  return buffer[(start + lookahead) % TOKENIZER_BUFFER_LIMIT];
}

Token Tokenizer::next_token()
{
  if (count > 0)
  {
    size_t const index = start;

    consume();

    return buffer[index];
  }
  else
  {
    return get_token();
  }
}

void Tokenizer::consume()
{
  assert(count > 0);

  (++start) %= TOKENIZER_BUFFER_LIMIT;
  --count;
}

Token Tokenizer::get_token()
{
  while (std::isspace(*at))
    at++;

  for (size_t i = 0; i < sizeof(token_list) / sizeof(*token_list); i++)
  {
    auto &expected = token_list[i];

    if (!std::strncmp(at, expected.begin, expected.size))
    {
      const char *const begin = at;
      at += expected.size;
      return { expected.type, begin, expected.size };
    }
  }

  const char *const begin = at;

  if (*at == '\0')
  {
    return { Token::END_OF_FILE, at, 0 };
  }
  else if (std::isdigit(*at))
  {
    while (std::isdigit(*++at))
      ;

    return { Token::INTEGER, begin, size_t(at - begin) };
  }
  else if (std::isalpha(*at))
  {
    while (std::isalpha(*at) || std::isdigit(*at) || *at == '_')
      at++;

    return { Token::IDENTIFIER, begin, size_t(at - begin) };
  }
  else
  {
    assert(false);
  }
}

decltype(Tokenizer::token_list) Tokenizer::token_list = {
  { Token::IF, "if", 2 },
  { Token::INT32_TYPE, "int32", 5 },
  { Token::ELSE, "else", 4 },
  { Token::FUN, "fun", 3 },
  { Token::RETURN, "return", 6 },
  { Token::WHILE, "while", 5 },

  { Token::ASSIGN, ":=", 2 },
  { Token::DIFFERENT, "!=", 2 },
  { Token::LESS_EQUAL, "<=", 2 },
  { Token::GREATER_EQUAL, ">=", 2 },
  { Token::OR, "||", 2 },
  { Token::AND, "&&", 2 },

  { Token::COLON, ":", 1 },
  { Token::SEMICOLON, ";", 1 },
  { Token::OPEN_PAREN, "(", 1 },
  { Token::CLOSE_PAREN, ")", 1 },
  { Token::OPEN_BRACKET, "[", 1 },
  { Token::CLOSE_BRACKET, "]", 1 },
  { Token::EQUAL, "=", 1 },
  { Token::LESS, "<", 1 },
  { Token::GREATER, ">", 1 },
  { Token::COMMA, ",", 1 },
  { Token::PLUS, "+", 1 },
  { Token::MINUS, "-", 1 },
  { Token::MULT, "*", 1 },
  { Token::DIVIDE, "/", 1 },
  { Token::MOD, "%", 1 }
};
