#include "treeDef.h"
#include "astUtils.h"
#include "symbolTable.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
void IOast(TreeNode **root);
void IONode(TreeNode **head, char *name, char *type, ExpType expType);
void IOinsert(TreeNode *root, SymbolTable *table);
