/**
 * @file    meta_keywords.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   The parsers of the meta-keywords
 * @version 0.1
 * @date    2020-05-01
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdio.h>
#include <string.h>
#include "lia/lia.h"

/**
 * @brief Verify if a token is a keyword
 * 
 * @param tk            The token to verify
 * @return keyword_t    The keyword's type
 * @return KEY_NONE     If is not a keyword
 */
keyword_t iskey(token_t *tk)
{
  if (tk->type != TK_ID)
    return KEY_NONE;
  
  if ( !strcmp(tk->text, "func") )
    return KEY_FUNC;
  if ( !strcmp(tk->text, "load") )
    return KEY_LOAD;
  if ( !strcmp(tk->text, "store") )
    return KEY_STORE;
  if ( !strcmp(tk->text, "push") )
    return KEY_PUSH;
  if ( !strcmp(tk->text, "pop") )
    return KEY_POP;
  if ( !strcmp(tk->text, "call") )
    return KEY_CALL;
  if ( !strcmp(tk->text, "ret") )
    return KEY_RET;
  if ( !strcmp(tk->text, "proc") )
    return KEY_PROC;
  if ( !strcmp(tk->text, "endproc") )
    return KEY_ENDPROC;
  if ( !strcmp(tk->text, "prtab") )
    return KEY_PRTAB;
  if ( !strcmp(tk->text, "ifz") || !strcmp(tk->text, "ifnz") )
    return KEY_IF;
  
  return KEY_NONE;
}

token_t *cmd_verify(KEY_ARGS)
{

}

/** op1 = register */
static token_t *key_op1reg(KEY_ARGS, inst_type_t type)
{
  tk = tk->next;

  if ( !isreg(tk) ) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected a register name, instead have `%s'", tk->text);
    return NULL;
  }

  token_t *next = tk->next;
  inst_t *inst = inst_add(lia->instlist, type);
  inst->child = tk->last;
  tk->next = NULL;

  return next;
}

/** op1 = register/immediate */
static token_t *key_op1ri(KEY_ARGS,  inst_type_t type)
{
  tk = tk->next;

  if ( !isreg(tk) && tk->type != TK_IMMEDIATE && tk->type != TK_CHAR ) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected a register name or immediate value, instead have `%s'", tk->text);
    return NULL;
  }

  token_t *next = tk->next;
  inst_t *inst = inst_add(lia->instlist, type);
  inst->child = tk->last;
  tk->next = NULL;

  return next;
}

/** op1 = TK_ID */
static token_t *key_op1id(KEY_ARGS,  inst_type_t type)
{
  tk = tk->next;

  if (tk->type != TK_ID) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected a procedure name, instead have `%s'", tk->text);
    return NULL;
  }

  token_t *next = tk->next;
  inst_t *inst = inst_add(lia->instlist, type);
  inst->child = tk->last;
  tk->next = NULL;

  return next;
}

/** No arguments */
static token_t *key_opnone(KEY_ARGS,  inst_type_t type)
{
  token_t *next = tk->next;
  inst_t *inst = inst_add(lia->instlist, type);
  inst->child = tk;
  tk->next = NULL;

  return next;
}

token_t *key_func(KEY_ARGS)
{
  tk = tk->next;

  if (tk->type != TK_IMMEDIATE) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected number, instead have `%s'", tk->text);
    return NULL;
  }

  if (tk->value < 0 || tk->value > 9) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected number between 0 and 9, instead: %d", tk->value);
    return NULL;
  }

  token_t *next = tk->next;
  inst_t *inst = inst_add(lia->instlist, INST_FUNC);
  inst->child = tk->last;
  tk->next = NULL;

  return next;
}

token_t *key_load(KEY_ARGS)
{
  return key_op1reg(tk, file, lia, INST_LOAD);
}

token_t *key_store(KEY_ARGS)
{
  return key_op1ri(tk, file, lia, INST_STORE);
}

token_t *key_push(KEY_ARGS)
{
  return key_op1ri(tk, file, lia, INST_PUSH);
}

token_t *key_pop(KEY_ARGS)
{
  return key_op1reg(tk, file, lia, INST_POP);
}

token_t *key_call(KEY_ARGS)
{
  return key_op1id(tk, file, lia, INST_CALL);
}

token_t *key_proc(KEY_ARGS)
{
  return key_op1id(tk, file, lia, INST_PROC);
}

token_t *key_endproc(KEY_ARGS)
{
  return key_opnone(tk, file, lia, INST_ENDPROC);
}

token_t *key_prtab(KEY_ARGS)
{
  return key_opnone(tk, file, lia, INST_PRTAB);
}

token_t *key_ret(KEY_ARGS)
{
  token_t *next;
  tk = tk->next;

  if (tk->type == TK_IMMEDIATE) {
    next = tk->next;
    tk->next = NULL;
  } else if (tk->type != TK_SEPARATOR && tk->type != TK_EOF) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected a literal number, instead have `%s'", tk->text);
    return NULL;
  } else {
    next = tk;
    tk->last->next = NULL;
  }

  inst_t *inst = inst_add(lia->instlist, INST_RET);
  inst->child = tk->last;

  return next;
}

token_t *key_if(KEY_ARGS)
{

}