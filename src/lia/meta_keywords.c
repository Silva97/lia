/**
 * @file    meta_keywords.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Auxiliary functions to parsing meta-keywords.
 * @version 0.1
 * @date    2020-05-01
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdlib.h>
#include <string.h>
#include "lia/lia.h"

/**
 * @brief Verify if a token is a meta-keyword
 * 
 * @param tk              The token to verify
 * @return metakeyword_t  The meta-keyword's type
 * @return META_NONE      If is not a meta-keyword
 */
metakeyword_t ismetakey(token_t *tk)
{
  if (tk->type != TK_ID)
    return META_NONE;
  
  if ( !strcmp(tk->text, "new") )
    return META_NEW;
  if ( !strcmp(tk->text, "import") )
    return META_IMPORT;
  if ( !strcmp(tk->text, "macro") )
    return META_MACRO;
  
  return META_NONE;
}

/**
 * @brief Returns next token ignoring separators
 * 
 * @param tk          The tokens' list
 * @return token_t*   The next token
 */
token_t *metanext(token_t *tk)
{
  tk = tk->next;
  while (tk->type == TK_SEPARATOR)
    tk = tk->next;

  return tk;
}

/**
 * @brief Inserts in a list of tokens.
 * 
 * @param list      The list to insert
 * @param type      The type of the token
 * @return mtk_t*   The new added item
 */
mtk_t *macro_tkseq_add(mtk_t *list, token_type_t type, char *name)
{
  if ( !list ) {
    list = calloc(1, sizeof *list);
    list->type = type;
    list->name = name;
    return list;
  }

  while (list->next)
    list = list->next;

  list->next = calloc(1, sizeof *list);
  list->next->last = list;
  list->next->type = type;
  list->next->name = name;
  return list->next;
}
