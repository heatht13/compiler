#include "treeDef.h"
#include "scanType.h"
#include "astUtils.h"
#include "IO.h"
#include "ourgetopt.h"
#include "symbolTable.h"
#include "semantics.h"
#include "c-.tab.h"
#include "yyerror.h"
#include <string.h>
#include <stdio.h>

extern int yylex();
extern int yydebug;
extern FILE *yyin;
extern int line;
extern int numErrors;
extern int numWarnings;
extern TreeNode *tree;
extern TreeNode *temp;
