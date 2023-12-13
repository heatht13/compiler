#ifndef CODEGEN_H
#define CODEGEN_H

#include "treeDef.h"
#include "emitcode.h"
#include "symbolTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int goffset;
extern int foffset;
extern TreeNode *tree;
extern TreeNode *IOroot;
extern TreeNode *temp;

void generate(SymbolTable *table);
void initGen(SymbolTable *table);
void globalGen(TreeNode *globalptr);
void codeGen(TreeNode *scopedNode, SymbolTable *table);
void travChildGen(TreeNode *treeptr, SymbolTable *table);
void travSibGen(TreeNode *treeptr, SymbolTable *table);
void addStdClosing();
void IOgen(TreeNode *IOtree);

#endif
