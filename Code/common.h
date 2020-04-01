#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
// #include <assert.h>

#define DEBUG

#include "debug.h"
#include "syntax_tree.h"
#include "symbol_table.h"
#include "senmantic.h"

void yyerror(const char *msg);
void yyerror2(const char *msg);
int yylex();



#endif
