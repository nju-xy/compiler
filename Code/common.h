#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
// #include <assert.h>

#include "debug.h"
#include "syntax_tree.h"
#include "symbol_table.h"
#include "intercode.h"
#include "semantic.h"
#include "mips.h"

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

void yyerror(const char *msg);
void yyerror2(const char *msg);
int yylex();



#endif
