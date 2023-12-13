
#ifndef ASTUTILS_H
#define ASTUTILS_H

#include "treeDef.h"
#include "scanType.h"

TreeNode *newDeclNode( DeclKind kind, ExpType type, TokenData *token);

TreeNode *newStmtNode( StmtKind kind, TokenData *token);

TreeNode *newExpNode( ExpKind kind, TokenData *token);

void printTree( TreeNode *treeptr, int indent, int sib);

#endif
