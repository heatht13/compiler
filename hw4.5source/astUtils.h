
#ifndef ASTUTILS_H
#define ASTUTILS_H

#include "treeDef.h"
#include "scanType.h"

TreeNode *giveSibling( TreeNode *siblingList, TreeNode *sibling);
void setType( TreeNode *treeptr, TokenData *tokenData, int isStatic, int isVarDecl);
TreeNode *newDeclNode( DeclKind kind, TokenData *token);
TreeNode *newStmtNode( StmtKind kind, TokenData *token);
TreeNode *newExpNode( ExpKind kind, TokenData *token);
void printTree( TreeNode *treeptr, int indent, int sib);
void printTypes( TreeNode *treeptr, int indent, int sib);

#endif
