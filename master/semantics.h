#ifndef SEMANTICS_H
#define SEMANTICS_H
#include "symbolTable.h"

extern int numErrors;
extern int numWarnings;
extern int goffset;
extern int foffset;
extern TreeNode *IOroot;
extern TreeNode *temp;
void traverseChildS(TreeNode *treeptr, SymbolTable *table);
void traverseSibS(TreeNode *treeptr, TreeNode *parent, SymbolTable *table);
void checkUsed(std::string symbol, void *ptr);
void passTypes(TreeNode *treeptr, TreeNode *parent, SymbolTable *table);
void semanticAnalysis(TreeNode *treeptr, TreeNode *parent, SymbolTable *table);
#endif
