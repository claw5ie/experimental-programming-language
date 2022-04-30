#include <cassert>
#include "Parser.hpp"

void parse_while(Tokenizer &tokenizer);

void parse_if(Tokenizer &tokenizer);

void parse_params(Tokenizer &tokenizer);

void parse_body(Tokenizer &tokenizer);

void parse_fun(Tokenizer &tokenizer);

void parse_var(Tokenizer &tokenizer);

void parse_level(uint32_t level, Tokenizer &tokenizer);

void parse_expr(Tokenizer &tokenizer);

void parse_while(Tokenizer &tokenizer)
{
  assert(tokenizer.next_token().type == Token::WHILE);
  parse_expr(tokenizer);
  assert(tokenizer.next_token().type == Token::COLON);
  parse_body(tokenizer);
  assert(tokenizer.next_token().type == Token::SEMICOLON);
}

void parse_if(Tokenizer &tokenizer)
{
  assert(tokenizer.next_token().type == Token::IF);
  parse_expr(tokenizer);
  assert(tokenizer.next_token().type == Token::COLON);

  Token token = tokenizer.peek(0);

  if (token.type == Token::ELSE)
  {
    goto parse_else;
  }
  else
  {
    parse_body(tokenizer);
    token = tokenizer.peek(0);
  }

  if (token.type == Token::ELSE)
  {
  parse_else:
    tokenizer.consume();
    parse_body(tokenizer);
  }

  assert(tokenizer.next_token().type == Token::SEMICOLON);
}

void parse_params(Tokenizer &tokenizer)
{
  assert(tokenizer.next_token().type == Token::OPEN_PAREN);
  Token token = tokenizer.next_token();
  bool first_arg = true;

  while (token.type != Token::CLOSE_PAREN && token.type != Token::END_OF_FILE)
  {
    if (!first_arg && token.type == Token::COMMA)
      token = tokenizer.next_token();

    assert(token.type == Token::IDENTIFIER);
    assert(tokenizer.next_token().type == Token::COLON);
    assert(tokenizer.next_token().type == Token::INT32_TYPE);

    token = tokenizer.next_token();
    first_arg = false;
  }

  assert(token.type == Token::CLOSE_PAREN);
}

void parse_body(Tokenizer &tokenizer)
{
  Token token = tokenizer.peek(0);

  while (
    token.type != Token::SEMICOLON && token.type != Token::END_OF_FILE
    )
  {
    if (token.type == Token::FUN)
    {
      parse_fun(tokenizer);
    }
    else if (token.type == Token::RETURN)
    {
      tokenizer.consume();
      parse_expr(tokenizer);
      assert(tokenizer.next_token().type == Token::SEMICOLON);
    }
    else if (token.type == Token::IF)
    {
      parse_if(tokenizer);
    }
    else if (token.type == Token::WHILE)
    {
      parse_while(tokenizer);
    }
    else if (token.type == Token::ELSE)
    {
      return;
    }
    else if (
      token.type == Token::IDENTIFIER && tokenizer.peek(1).type == Token::COLON
      )
    {
      parse_var(tokenizer);
    }
    else
    {
      parse_expr(tokenizer);
      assert(tokenizer.next_token().type == Token::SEMICOLON);
    }

    token = tokenizer.peek(0);
  }
}

void parse_fun(Tokenizer &tokenizer)
{
  assert(tokenizer.next_token().type == Token::FUN);
  assert(tokenizer.next_token().type == Token::IDENTIFIER); // identifier
  assert(tokenizer.peek(0).type == Token::OPEN_PAREN); // (
  parse_params(tokenizer);  // args + )
  assert(tokenizer.next_token().type == Token::COLON); // :
  assert(tokenizer.next_token().type == Token::INT32_TYPE); // type
  parse_body(tokenizer); // body
  assert(tokenizer.next_token().type == Token::SEMICOLON); // ;
}

void parse_var(Tokenizer &tokenizer)
{
  assert(tokenizer.next_token().type == Token::IDENTIFIER);
  assert(tokenizer.next_token().type == Token::COLON);
  assert(tokenizer.next_token().type == Token::INT32_TYPE); // type
  Token token = tokenizer.next_token(); // = or ;

  if (token.type == Token::SEMICOLON)
  {
    return;
  }
  else if (token.type == Token::EQUAL)
  {
    parse_expr(tokenizer);
    assert(tokenizer.next_token().type == Token::SEMICOLON); // ;
  }
  else if (token.type == Token::OPEN_BRACKET)
  {
    parse_expr(tokenizer);
    assert(tokenizer.next_token().type == Token::CLOSE_BRACKET);
    assert(tokenizer.next_token().type == Token::SEMICOLON);
  }
  else
  {
    assert(false);
  }
}

void parse_args(Tokenizer &tokenizer)
{
  assert(tokenizer.next_token().type == Token::OPEN_PAREN);

  Token token = tokenizer.peek(0);

  while (token.type != Token::CLOSE_PAREN && token.type != Token::END_OF_FILE)
  {
    parse_expr(tokenizer);
    token = tokenizer.peek(0);

    if (token.type == Token::COMMA)
    {
      tokenizer.consume();
      token = tokenizer.peek(0);
    }
  }

  assert(token.type == Token::CLOSE_PAREN);
}

void parse_level(uint32_t level, Tokenizer &tokenizer)
{
  if (level < 7)
  {
    parse_level(level + 1, tokenizer);
    auto token = tokenizer.peek(0);

    // if (token.prec_level() == level && token.is_right_assoc())
    //   parse_level(level, tokenizer);

    while (token.prec_level() == level)
    {
      tokenizer.consume();
      parse_level(level + 1, tokenizer);
      token = tokenizer.peek(0);
    }

    return;
  }
  else
  {
    auto token = tokenizer.next_token();

    switch (token.type)
    {
    case Token::OPEN_PAREN:
    {
      parse_level(0, tokenizer);

      token = tokenizer.next_token();
      assert(token.type == Token::CLOSE_PAREN);

      return;
    }
    case Token::MINUS:
      parse_level(level, tokenizer);
      return;
    case Token::INTEGER:
      return;
    case Token::IDENTIFIER:
    {
      token = tokenizer.peek(0);

      if (token.type == Token::OPEN_PAREN)
      {
        parse_args(tokenizer);
        assert(tokenizer.next_token().type == Token::CLOSE_PAREN);
      }
      else if (token.type == Token::OPEN_BRACKET)
      {
        tokenizer.consume();
        parse_expr(tokenizer);
        assert(tokenizer.next_token().type == Token::CLOSE_BRACKET);
      }
      else
      {

      }
    } break;
    default:
      assert(false);
    }
  }
}

void parse_expr(Tokenizer &tokenizer)
{
  parse_level(0, tokenizer);
}

void parse(Tokenizer &tokenizer)
{
  auto token = tokenizer.peek(0);

  while (token.type != Token::END_OF_FILE)
  {
    if (token.type == Token::FUN)
      parse_fun(tokenizer);
    else if (
      token.type == Token::IDENTIFIER && tokenizer.peek(1).type == Token::COLON
      )
      parse_var(tokenizer);
    else
      assert(false);

    token = tokenizer.peek(0);
  }
}
