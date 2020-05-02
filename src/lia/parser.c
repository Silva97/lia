/**
 * @file    parser.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   The parser of the Lia language
 * @version 0.1
 * @date    2020-05-01
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "lia/parser.h"
#include "lia/error.h"

/**
 * @brief Finds the next token of the type
 * 
 * Returns the token found or the TK_EOF.
 * 
 * @param tk     The token's list
 * @param type   The type to find
 * @return token_t*  The pointer to the token, or to TK_EOF
 */
static token_t *tknext(token_t *tk, token_type_t type)
{
  while (tk->type != TK_EOF && tk->type != type)
    tk = tk->next;
  
  return tk;
}

/**
 * @brief Try match a sequence of token's types.
 * 
 * @param tk         The token's list
 * @param number     The number of tokens to match
 * @param ...        The list of token's types
 * @return int       -1 if match, or the index of the token not matched
 */
int tkseq(token_t *tk, unsigned int number, ...)
{
  va_list ap;
  va_start(ap, number);

  if ( !tk )
    return 0;

  for (int i = 0; i < number; i++) {
    if ( !tk || tk->type != va_arg(ap, token_type_t) ) {
      va_end(ap);
      return i;
    }

    tk = tk->next;
  }

  va_end(ap);
  return -1;
}


inst_t *inst_add(inst_t *list, inst_type_t type)
{
  inst_t *last;

  while (list && list->child) {
    last = list;
    list = list->next;
  }

  if ( !list ) {
    list = calloc(1, sizeof *list);
    last->next = list;
  }

  list->type = type;
  return list;
}

/**
 * @brief Analyzes the syntax of the Lia code
 * 
 * @param filename   The name of the source file
 * @param lia        The Lia struct
 * @return int       The number of errors
 */
int lia_parser(char *filename, lia_t *lia)
{
  metakeyword_t meta;
  token_t *next;
  token_t *this = lia->tklist;
  int errcount = 0;

  token_t *( *metakeys[] )(KEY_ARGS) = {
    [META_NEW] = meta_new,
    [META_IMPORT] = meta_import
  };

  if ( !lia->proctree )
    lia->proctree = calloc(1, sizeof (proc_t));
  if ( !lia->cmdtree )
    lia->cmdtree = calloc(1, sizeof (cmd_t));
  if ( !lia->instlist )
    lia->instlist = calloc(1, sizeof (inst_t));

  while (this->type != TK_EOF) {
    switch (this->type) {
    case TK_SEPARATOR:
      break; // Just ignore it

    case TK_OPENBRACKET:
      this = metanext(this);
      meta = ismetakey(this);
      if (meta == META_NONE) {
        lia_error(filename, this->next->line, this->next->column,
          "Expected a meta-keyword, instead have `%s'", this->next->text);
        
        this = tknext(this, TK_CLOSEBRACKET);
        if (this->next)
          this = this->next;

        errcount++;
        break;
      }

      next = metakeys[meta](filename, this, lia);
      if ( !next ) {
        this = tknext(this, TK_CLOSEBRACKET);
        if (this->next)
          this = this->next;
        
        errcount++;
        break;
      }

      if (next->type != TK_CLOSEBRACKET) {
        lia_error(filename, next->line, next->column,
          "Expected ']', instead have `%s'", next->text);

        this = next;
        errcount++;
        break;
      }

      if ( next->next->type != TK_SEPARATOR && next->next->type != TK_EOF ) {
        lia_error(filename, next->next->line, next->next->column,
          "Expected a instruction separator, instead have `%s'",
          next->next->text);
        
        this = next->next;
        errcount++;
        break;
      }

      this = next->next;
      break;
    
    case TK_ID:
      lia_error(filename, this->line, this->column, "%s", "Not implemented!");
      this = this->next;
      break;
    
    default:
      lia_error(filename, this->line, this->column,
        "Unexpected token `%s'", this->text);
      
      this = tknext(this, TK_SEPARATOR);
      if (this->next)
        this = this->next;

      errcount++;
    }

    if ( !this->next )
      break;

    this = this->next;
  }

  return errcount;
}