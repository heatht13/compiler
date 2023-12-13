
#ifndef TREEDEF_H
#define TREEDEF_H

//Enumerate types of declaration, statements, and expressions for use in AST, bison file

typedef int OpKind;

typedef enum {DeclK, StmtK, ExpK} NodeKind;

typedef enum {VarK, FuncK, ParamK} DeclKind;

typedef enum {NullK, IfK, WhileK, ForK, CompoundK, ReturnK, BreakK, RangeK} StmtKind;

typedef enum {OpK, ConstantK, IdK, AssignK, InitK, CallK} ExpKind;

typedef enum {NoType, Void, Integer, Boolean, Char, CharInt, Equal, UndefinedType} ExpType;

typedef enum {None, Local, Global, Parameter, LocalStatic} VarKind;

#define MAXCHILDREN 3

typedef struct treeNode {
    struct treeNode *child[MAXCHILDREN];	//node's children
    struct treeNode *sibling;			//node's siblings

    int line;					//store node's line number
    NodeKind nodeKind;
    union {					//store subkind
        DeclKind decl;
        StmtKind stmt;
        ExpKind exp;
    } subkind;

    union {					//store attribute
        OpKind op;
        int value;
        unsigned char cvalue;
        char *string;
        char *name;
    } attr;

    int varSize;				//store size
    char *type;					//store type int, bool, char, or void
    ExpType expType;				//don't need anymore
    int isArray;				//mark if array
    int isStatic;				//mark if static
    int isInit;
    int declUsed;
    int initWarn;
    int numParams;
    int isIO;
    VarKind varKind;
    int offset;
} TreeNode;

#endif

