
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "astUtils.h"


TreeNode * giveSibling( TreeNode *siblingList, TreeNode *sibling) {

    TreeNode *ptr = siblingList;

    if (ptr != NULL) {
        while (ptr->sibling != NULL) {
            ptr = ptr->sibling;
        }
        if(sibling != NULL) {
            ptr->sibling = sibling;
        }
        return siblingList;
    }
    else {
        return sibling;
    }
}


void setType( TreeNode *treeptr, TokenData *tokenData, int isStatic, int isVarDecl) {
    while(treeptr) {
        treeptr->expType = (ExpType)tokenData->nvalue;
        if(tokenData->tokenstr != NULL) {
            treeptr->type = strdup(tokenData->tokenstr);
        }
        else {
            treeptr->type = NULL;
        }
        treeptr->isStatic = isStatic;
        //below is used to handle initialized var declarations, parms cant do this
        if(isVarDecl) {
            if(treeptr->child[0]) {
                treeptr->child[0]->expType  = (ExpType)tokenData->nvalue;
                if(tokenData->tokenstr != NULL) {
                    treeptr->child[0]->type = strdup(tokenData->tokenstr);
                }
                else {
                    treeptr->child[0]->type = NULL;
                }
                treeptr->child[0]->isStatic = isStatic;
            }
        }
        treeptr = treeptr->sibling;
    }
}

//creates a new declaration node for use in the AST*/
TreeNode *newDeclNode(DeclKind kind, TokenData *token) {
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));

    int i;
    if(node == NULL) {			//No memory to create node
        printf("Error: Out of memory at line %d\n", token->linenum);
    }
    else {
        for( i=0; i<MAXCHILDREN; i++) {	//nullify node's children
            node->child[i] = NULL;
        }
        node->sibling = NULL;		//nullify node's sibling
        node->nodeKind = DeclK;		//assign node's nodekind member
        node->subkind.decl = kind;	//assign node's subkind
        if(token != NULL) {		//ensure we have token
            node->line = token->linenum;	//store line number
            /*do not use token->modified as this is an ID kind*/
            if(token->tokenstr != NULL) {
                node->attr.name = strdup(token->tokenstr);	//store declaration name
            }
        }
        else {				/*Ensure token is present*/
            node->line = -1;
            node->attr.name = NULL;
        }
        /*initialize everything to null*/
        node->arraySize = -1;
        node->type = NULL;
        node->expType = UndefinedType;
        node->isArray = 0;
        node->isStatic = 0;
        node->isInit = 0;
        node->declUsed = 0;
        node->initWarn = 0;
        node->numParams = 0;
        if(kind == FuncK) {
            node->isInit = 1;
        }
        node->isIO = 0;
    }
    return node;
}

//creates a new statement node for use in the AST
TreeNode *newStmtNode(StmtKind kind, TokenData *token) {
    TreeNode *node = (TreeNode *) malloc(sizeof(TreeNode));

    int i;
    if(node==NULL) {                    //No memory to create node
        printf("Error: Out of memory at line %d\n", token->linenum);
    }
    else {
        for ( i=0; i<MAXCHILDREN; i++) {	//nullify node's children
            node->child[i] = NULL;		
        }
        node->sibling = NULL;		//nullify node's sibling
        node->nodeKind = StmtK;		//assign node's nodekind member
        node->subkind.stmt = kind;	//assign node's subkind member
        if(token != NULL) {		//ensure we have token
            node->line = token->linenum;	//store line number
            if(token->tokenstr != NULL) {
                node->attr.string = strdup(token->tokenstr);  //store statement
            }
        }
        else {                          /*Ensure token is present*/
            node->line = -1;
            node->attr.string = NULL;
        }
        /*initialize everything to null*/
        node->arraySize = -1;
        node->type = NULL;
        node->expType = UndefinedType;
        node->isArray = 0;
        node->isStatic = 0;
        node->isInit = 0;
        node->declUsed = 0;
        node->initWarn = 0;
        node->numParams = 0;
        node->isIO = 0;
    }
    return node;
}

//creates a new expression node for use in the AST
TreeNode *newExpNode(ExpKind kind, TokenData *token) {
    TreeNode *node = (TreeNode *) malloc(sizeof(TreeNode));

    int i;
    if(node==NULL) {                    //No memory to create node
        printf("Error: Out of memory at line %d\n",token->linenum);
    }
    else {
        for( i=0; i<MAXCHILDREN; i++) {		//nullify node's children for now
            node->child[i] = NULL;
        }
        node->sibling = NULL;		//nullify node's children for now
        node->nodeKind = ExpK;		//assign node's nodekind member
        node->subkind.exp = kind;	//assign node's subkind member
        if(token != NULL) {		//ensure we have token
            node->line = token->linenum;	//store line number
            if(token->tokenstr != NULL) {
                node->attr.string = strdup(token->tokenstr); //store inputted expression
                if(kind == IdK) {		//if we have an ID
                    node->attr.name = strdup(token->tokenstr); //store it in name field
                }
            }
        }
        else {                          /*Ensure token is present*/
            node->line = -1;
            node->attr.name = NULL;
        }
        /*initialize everything to null*/
        node->arraySize = -1;
        node->type = NULL;
        node->expType = UndefinedType;
        node->isArray = 0;
        node->isStatic = 0;
        node->isInit = 0;
        node->declUsed = 0;
        node->initWarn = 0;
        if(kind == CallK) {
            node->isInit = 1;
        }
        node->numParams = 0;
        node->isIO = 0;
    }
    return node;
}

//Prints tree recursively using inorder traversal.
void printTree(TreeNode *treeptr, int indent, int sib) {
    if (treeptr == NULL) {
        return;
    }
    switch(treeptr->nodeKind) {		//determine if node is a declaration, statement, or expression node
        case (DeclK):
            switch(treeptr->subkind.decl) {		//determine if declaration is a variable, function, or parameter
                case (VarK):
                    if(treeptr->isArray) {
                        printf("Var: %s of array of type %s [line: %d]\n", treeptr->attr.name, treeptr->type, treeptr->line);
                    }
                    else if(treeptr->isStatic) {
                        printf("Var: %s of static type %s [line: %d]\n", treeptr->attr.name, treeptr->type, treeptr->line);
                    }
                    else {
                        printf("Var: %s of type %s [line: %d]\n", treeptr->attr.name, treeptr->type, treeptr->line);
                    }
                    break;
                case (FuncK):
                    printf("Func: %s returns type %s [line: %d]\n", treeptr->attr.name, treeptr->type, treeptr->line);
                    break;
                case (ParamK):
                    if(treeptr->isArray) {
                        printf("Parm: %s of array of type %s [line: %d]\n", treeptr->attr.name, treeptr->type, treeptr->line);
                    }
                    else {
                        printf("Parm: %s of type %s [line: %d]\n", treeptr->attr.name, treeptr->type, treeptr->line);
                    }
                    break;
                default:
                    printf("Error: No decl kind at line: %d\n", treeptr->line);
                    break;
            }
            break;
        case (StmtK):
            switch(treeptr->subkind.stmt) {		//determine if statement is a null, if, while, for, compound, return, break, or range
                case (NullK):
                    printf("Null [line: %d]\n", treeptr->line);
                    break;
                case (IfK):
                    printf("If [line: %d]\n", treeptr->line);
                    break;
                case (WhileK):
                    printf("While [line: %d]\n", treeptr->line);
                    break;
                case (ForK):
                    printf("For [line: %d]\n", treeptr->line);
                    break;
                case (CompoundK):
                    printf("Compound [line: %d]\n", treeptr->line);
                    break;
                case (ReturnK):
                    printf("Return [line: %d]\n", treeptr->line);
                    break;
                case (BreakK):
                    printf("Break [line: %d]\n", treeptr->line);
                    break;
                case (RangeK):
                    printf("Range [line: %d]\n", treeptr->line);
                    break;
                default:
                    printf("Error: No stmt kind at line %d\n", treeptr->line);
                    break;
            }
            break;
        case (ExpK):
            switch(treeptr->subkind.exp) {		//determine if expression is an operator, constant, Id, assign, initializer, or call
                case (OpK):
                    printf("Op: %s [line: %d]\n", treeptr->attr.string, treeptr->line);
                    break;
                case (ConstantK):
                    printf("Const %s [line: %d]\n", treeptr->attr.value, treeptr->line);
                    break;
                case (IdK):
                    printf("Id: %s [line: %d]\n", treeptr->attr.name, treeptr->line);
                    break;
                case (AssignK): //Not Done
                    printf("Assign: %s [line: %d]\n", treeptr->attr.value, treeptr->line);
                    break;
                case (InitK): //Not Done
                    printf("Init: %s [line: %d]\n", treeptr->attr.name, treeptr->line);
                    break;
                case (CallK):
                    printf("Call: %s [line: %d]\n", treeptr->attr.name , treeptr->line);
                    break;
                default:
                    printf("Error: No exp kind at line %d\n", treeptr->line);
                    break;
            }
            break;
        default:
            printf("Error, no nodekind at line %d\n", treeptr->line);
            break;
    }
    int i, k;
    for (i = 0; i < MAXCHILDREN; i++) {
        if (treeptr->child[i] != NULL) {	//if node has child
            for(k = 0; k < indent; k++) {	//for each parent, ident once
                printf(".   ");
            }
            printf("Child: %d  ", i);		//give child number
            printTree(treeptr->child[i], indent+1, 1); //call again to look for child's child
        }
    }
    if (treeptr->sibling != NULL) {		// node has a sibling
        for(k = 0; k < indent-1; k++) {		// for each parent, indent once
            printf(".   ");
        }
        printf("Sibling: %d  ", sib);		//give sibling number
        printTree(treeptr->sibling, indent, sib+1); //call again to look for sibling's sibling

    }
    return;
}


//Prints tree recursively with type information using inorder traversal.
void printTypes(TreeNode *treeptr, int indent, int sib) {
    if (treeptr == NULL) {
        return;
    }
    switch(treeptr->nodeKind) {		//determine if node is a declaration, statement, or expression node
        case (DeclK):
            switch(treeptr->subkind.decl) {		//determine if declaration is a variable, function, or parameter
                case (VarK):
                    if(treeptr->isArray) {
                        printf("Var: %s is array of type %s [line: %d]\n", treeptr->attr.name, treeptr->type, treeptr->line);
                    }
                    else if(treeptr->isStatic) {
                        printf("Var: %s of static type %s [line: %d]\n", treeptr->attr.name, treeptr->type, treeptr->line);
                    }
                    else {
                        printf("Var: %s of type %s [line: %d]\n", treeptr->attr.name, treeptr->type, treeptr->line);
                    }
                    break;
                case (FuncK):
                    printf("Func: %s returns type %s [line: %d]\n", treeptr->attr.name, treeptr->type, treeptr->line);
                    break;
                case (ParamK):
                    if(treeptr->isArray) {
                        printf("Parm: %s is array of type %s [line: %d]\n", treeptr->attr.name, treeptr->type, treeptr->line);
                    }
                    else {
                        printf("Parm: %s of type %s [line: %d]\n", treeptr->attr.name, treeptr->type, treeptr->line);
                    }
                    break;
                default:
                    printf("Error: No decl kind at line: %d\n", treeptr->line);
                    break;
            }
            break;
        case (StmtK):
            switch(treeptr->subkind.stmt) {		//determine if statement is a null, if, while, for, compound, return, break, or range
                case (NullK):
                    printf("Null [line: %d]\n", treeptr->line);
                    break;
                case (IfK):
                    printf("If [line: %d]\n", treeptr->line);
                    break;
                case (WhileK):
                    printf("While [line: %d]\n", treeptr->line);
                    break;
                case (ForK):
                    printf("For [line: %d]\n", treeptr->line);
                    break;
                case (CompoundK):
                    printf("Compound [line: %d]\n", treeptr->line);
                    break;
                case (ReturnK):
                    printf("Return [line: %d]\n", treeptr->line);
                    break;
                case (BreakK):
                    printf("Break [line: %d]\n", treeptr->line);
                    break;
                case (RangeK):
                    printf("Range [line: %d]\n", treeptr->line);
                    break;
                default:
                    printf("Error: No stmt kind at line %d\n", treeptr->line);
                    break;
            }
            break;
        case (ExpK):
            switch(treeptr->subkind.exp) {		//determine if expression is an operator, constant, Id, assign, initializer, or call
                case (OpK):
                    printf("Op: %s of type %s [line: %d]\n", treeptr->attr.string, treeptr->type, treeptr->line);
                    break;
                case (ConstantK):
                    printf("Const %s of type %s [line: %d]\n", treeptr->attr.value, treeptr->type, treeptr->line);
                    break;
                case (IdK):
                    printf("Id: %s of type %s [line: %d]\n", treeptr->attr.name, treeptr->type, treeptr->line);
                    break;
                case (AssignK): //Not Done
                    printf("Assign: %s of type %s [line: %d]\n", treeptr->attr.value, treeptr->type, treeptr->line);
                    break;
                case (InitK): //Not Done
                    printf("Init: %s of type %s [line: %d]\n", treeptr->attr.name, treeptr->type, treeptr->line);
                    break;
                case (CallK):
                    printf("Call: %s of type %s [line: %d]\n", treeptr->attr.name, treeptr->type, treeptr->line);
                    break;
                default:
                    printf("Error: No exp kind at line %d\n", treeptr->line);
                    break;
            }
            break;
        default:
            printf("Error, no nodekind at line %d\n", treeptr->line);
            break;
    }
    int i, k;
    for (i = 0; i < MAXCHILDREN; i++) {
        if (treeptr->child[i] != NULL) {	//if node has child
            for(k = 0; k < indent; k++) {	//for each parent, ident once
                printf(".   ");
            }
            printf("Child: %d  ", i);		//give child number
            printTypes(treeptr->child[i], indent+1, 1); //call again to look for child's child
        }
    }
    if (treeptr->sibling != NULL) {		// node has a sibling
        for(k = 0; k < indent-1; k++) {		// for each parent, indent once
            printf(".   ");
        }
        printf("Sibling: %d  ", sib);		//give sibling number
        printTypes(treeptr->sibling, indent, sib+1); //call again to look for sibling's sibling

    }
    return;
}
