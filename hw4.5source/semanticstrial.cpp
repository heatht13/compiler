#include "treeDef.h"
#include "semantics.h"
#include "IO.h"
#include <string.h>
#include <stdlib.h>

void traverseChildS( TreeNode *treeptr, SymbolTable *table) {
    int i;
    for( i=0; i<MAXCHILDREN; i++) {
        /*if( isFor == 1 && i = 1) {
            i++;
        }*/
        if(treeptr->child[i] != NULL) {
            semanticAnalysis(treeptr->child[i], treeptr, table);
        }
    }
    return;
}

void traverseSibS( TreeNode *treeptr, TreeNode *parent, SymbolTable *table) {
    if( treeptr->sibling != NULL) {
        semanticAnalysis(treeptr->sibling, parent, table);
    }
    return;
}

void checkUsed(std::string symbol, void *ptr) {
    TreeNode *treeptr = (TreeNode *)ptr;
    if( !treeptr->declUsed) {
        if( (!treeptr->declUsed) && treeptr->subkind.decl == FuncK) {
            if(strcmp(treeptr->attr.name, (char *)"main") == 0) {
                //Bypass main() function
            }
            else {
            printf("WARNING(%d): The function \'%s\' seems not to be used.\n", treeptr->line, treeptr->attr.name);
            numWarnings++;
            }
        }
        else if( treeptr->subkind.decl == ParamK) {
            printf("WARNING(%d): The parameter \'%s\' seems not to be used.\n", treeptr->line, treeptr->attr.name);
            numWarnings++;
        }
        else if( treeptr->subkind.decl == VarK) {
            printf("WARNING(%d): The variable \'%s\' seems not to be used.\n", treeptr->line, treeptr->attr.name);
            numWarnings++;
        }
    }
    /*else {
        printf("here you go: %s->declUsed: %d\n", treeptr->attr.string, treeptr->declUsed);
    }*/
}

void passTypes(TreeNode *treeptr, TreeNode *parent, SymbolTable *table) {
    int i;
    if(treeptr != NULL) {
        for( i=0; i<MAXCHILDREN; i++) {
            if(treeptr->child[i] != NULL /*&& treeptr->child[i]->expType == UndefinedType*/) {
                passTypes(treeptr->child[i], treeptr,  table);
            }
        }
        if(treeptr->sibling != NULL /*&& treeptr->sibling->expType == UndefinedType*/) {
            passTypes(treeptr->sibling, parent, table);
        }
        if(treeptr->nodeKind == ExpK && treeptr->expType == UndefinedType) {
            switch(treeptr->subkind.exp) {
                case(OpK):
                    if(strcmp(treeptr->attr.string, (char*)"=") == 0) {
                        treeptr->expType = Boolean;
                        treeptr->type = strdup((char *)"bool");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"!=") == 0) {
                        treeptr->expType = Boolean;
                        treeptr->type = strdup((char *)"bool");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"<") == 0) {
                        treeptr->expType = Boolean;
                        treeptr->type = strdup((char *)"bool");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"<=") == 0) {
                        treeptr->expType = Boolean;
                        treeptr->type = strdup((char *)"bool");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)">") == 0) {
                        treeptr->expType = Boolean;
                        treeptr->type = strdup((char *)"bool");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)">=") == 0) {
                        treeptr->expType = Boolean;
                        treeptr->type = strdup((char *)"bool");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"+") == 0) {
                        treeptr->expType = Integer;
                        treeptr->type = strdup((char *)"int");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"-") == 0) {
                        treeptr->expType = Integer;
                        treeptr->type = strdup((char *)"int");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"*") == 0) {
                        treeptr->expType = Integer;
                        treeptr->type = strdup((char *)"int");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"/") == 0) {
                        treeptr->expType = Integer;
                        treeptr->type = strdup((char *)"int");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"%") == 0) {
                        treeptr->expType = Integer;
                        treeptr->type = strdup((char *)"int");
                    } /*WORK NEEDED*/
                    else if(strcmp(treeptr->attr.string, (char*)"[") == 0) {
                        if(treeptr->child[1]->expType == UndefinedType) {
                            passTypes(treeptr->child[1], treeptr, table);
                        }
                        if(treeptr->child[0]->expType == UndefinedType) {
                            passTypes(treeptr->child[0], treeptr,table);
                        }
                        else {
                            treeptr->expType = treeptr->child[0]->expType;
                            if(treeptr->child[0]->type) {
                                treeptr->type = strdup(treeptr->child[0]->type);
                            }
                            treeptr->isArray = treeptr->child[0]->isArray;
                            treeptr->isStatic = treeptr->child[0]->isStatic;
                            treeptr->isInit = treeptr->child[0]->isInit;
                            treeptr->arraySize = treeptr->child[0]->arraySize;
                            /*treeptr->declUsed = treeptr->child[0]->declUsed;
                            treeptr->initWarn = treeptr->child[0]->initWarn;*/
                        }
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"sizeof") == 0) {
                        treeptr->expType = Integer;
                        treeptr->type = strdup((char *)"int");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"?") == 0) {
                        treeptr->expType = Integer;
                        treeptr->type = strdup((char *)"int");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"chsign") == 0) {
                        treeptr->expType = Integer;
                        treeptr->type = strdup((char *)"int");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"not") == 0) {
                        treeptr->expType = Boolean;
                        treeptr->type = strdup((char *)"bool");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"and") == 0) {
                        treeptr->expType = Boolean;
                        treeptr->type = strdup((char *)"bool");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"or") == 0) {
                        treeptr->expType = Boolean;
                        treeptr->type = strdup((char *)"bool");
                    }
                    break;
                case(AssignK):
                    if(strcmp(treeptr->attr.string, (char*)"++") == 0) {
                        treeptr->expType = Integer;
                        treeptr->type = strdup((char *)"int");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"--") == 0) {
                        treeptr->expType = Integer;
                        treeptr->type = strdup((char *)"int");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"<-") == 0) {
                        if(treeptr->child[1]->expType == UndefinedType) {
                            passTypes(treeptr->child[1], treeptr, table);
                        }
                        if(treeptr->child[0]->expType == UndefinedType) {
                            passTypes(treeptr->child[0], treeptr, table);
                        }
                        else {
                            treeptr->expType = treeptr->child[0]->expType;
                            if(treeptr->child[0]->type) {
                                treeptr->type = strdup(treeptr->child[0]->type);
                            }
                            /*if(treeptr->child[0]->isArray == 1 || treeptr->child[1]->isArray == 1) {
                                treeptr->isArray = 1;
                            }
                            else {
                                treeptr->isArray = 0;
                            }*/
                            treeptr->isArray = treeptr->child[0]->isArray;
                            treeptr->isStatic = treeptr->child[0]->isStatic;
                            treeptr->isInit = 1;
                            treeptr->child[0]->isInit = 1;
                            treeptr->arraySize = treeptr->child[0]->arraySize;
                            treeptr->declUsed = treeptr->child[0]->declUsed;
                        }
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"+=") == 0) {
                        treeptr->expType = Integer;
                        treeptr->type = strdup((char *)"int");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"-=") == 0) {
                        treeptr->expType = Integer;
                        treeptr->type = strdup((char *)"int");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"*=") == 0) {
                        treeptr->expType = Integer;
                        treeptr->type = strdup((char *)"int");
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"/=") == 0) {
                        treeptr->expType = Integer;
                        treeptr->type = strdup((char *)"int");
                    }
                    break;
                case(ConstantK):
                    /*(*parent)->expType = (*treeptr)->expType;
                    if((*treeptr)->type != NULL) {
                        (*parent)->type = strdup((*treeptr)->type);
                    }*/
                    break;
                case(IdK):
                    //printf("%s\n", treeptr->attr.name);
                    temp = (TreeNode *)table->lookup(treeptr->attr.name);
                    if(temp) {
                        treeptr->expType = temp->expType;
                        if(temp->type) {
                            treeptr->type = strdup(temp->type);
                        }
                        treeptr->isArray = temp->isArray;
                        treeptr->isStatic = temp->isStatic;
                        treeptr->isInit = temp->isInit;
                        treeptr->arraySize = temp->arraySize;
                        treeptr->declUsed = temp->declUsed;
                        treeptr->initWarn = temp->initWarn;
                    }
                    else {
                        temp = (TreeNode *)table->lookupGlobal(treeptr->attr.name);
                        if(temp) {
                            treeptr->expType = temp->expType;
                            if(temp->type) {
                                treeptr->type = strdup(temp->type);
                            }
                            treeptr->isArray = temp->isArray;
                            treeptr->isStatic = temp->isStatic;
                            treeptr->isInit = temp->isInit;
                            treeptr->arraySize = temp->arraySize;
                            treeptr->declUsed = temp->declUsed;
                            treeptr->initWarn = temp->initWarn;
                        }
                    }
                    temp = NULL;
                    break;
                case(InitK):
                    treeptr->expType = treeptr->child[0]->expType;
                    if(treeptr->child[0]->type != NULL) {
                        treeptr->type = strdup(treeptr->child[0]->type);
                    }
                    treeptr->isArray = treeptr->child[0]->isArray;
                    treeptr->isStatic = treeptr->child[0]->isStatic;
                    treeptr->isInit = 0;
                    temp = (TreeNode *)table->lookup(treeptr->child[0]->attr.name);
                    if(temp) {
                        temp->isInit = 1;
                    }
                    temp = NULL;
                    treeptr->arraySize = treeptr->child[0]->arraySize;
                    treeptr->declUsed = treeptr->child[0]->declUsed;
                    treeptr->initWarn = treeptr->child[0]->initWarn;
                    break;
                case(CallK):
                    temp = (TreeNode *)table->lookup(treeptr->attr.name);
                    if(temp) {
                        treeptr->expType = temp->expType;
                        if(temp->type) {
                            treeptr->type = strdup(temp->type);
                        }
                        treeptr->isArray = temp->isArray;
                        treeptr->isStatic = temp->isStatic;
                        treeptr->isInit = temp->isInit;
                        treeptr->isIO = temp->isIO;
                        /*temp->declUsed = 1;
                        treeptr->declUsed = 0;
                        treeptr->initWarn = 0;
                        treeptr->numParams = 0;*/
                    }
                    else {
                        temp = (TreeNode *)table->lookupGlobal(treeptr->attr.name);
                        if(temp) {
                            treeptr->expType = temp->expType;
                            if(temp->type) {
                                treeptr->type = strdup(temp->type);
                            }
                            treeptr->isArray = temp->isArray;
                            treeptr->isStatic = temp->isStatic;
                            treeptr->isInit = temp->isInit;
                            treeptr->isIO = temp->isIO;
                            /*temp->declUsed = 1;
 *                          treeptr->declUsed = 0;
 *                          treeptr->initWarn = 0;
 *                          treeptr->numParams = 0;*/
                        }
                    }
                    temp = NULL;
                    break;
                default:
                        printf("Error in passType: %s doesn't have a valid ExpKind.\n", treeptr->attr.string);
                    break;
            }
        }
    }
}
 
void semanticAnalysis( TreeNode *treeptr, TreeNode *parent, SymbolTable *table){
    static int needCompound;
    static int expType0 = -1;
    static int expType1 = -1;
    static int paramcounter;
    static int isFor;
    static int line;
    static TreeNode *temp2;
    static TreeNode *child0;
    static TreeNode *child1;
    table->debug(false);
    if(treeptr == NULL){
        return;
    }
    int i;
   // if(treeptr->expType == UndefinedType) {
        passTypes(treeptr, parent, table);
    //}
    switch(treeptr->nodeKind) {
//------------- Declarations -------------------
        case(DeclK):
            switch(treeptr->subkind.decl) {
                case(VarK):
                    if(!table->insert(treeptr->attr.name, treeptr)) {
                        printf("ERROR(%d): Symbol \'%s\' is already declared at line %d.\n", treeptr->line, treeptr->attr.name, treeptr->line);
                        numErrors++;
                    }
                    traverseSibS(treeptr, parent, table);
                    break;
                case(FuncK):
                    if( !table->insert(treeptr->attr.name, treeptr)) {
                        printf("ERROR(%d): Symbol \'%s\' is already declared.\n", treeptr->line, treeptr->attr.name);
                        numErrors++;
                    }
                    table->enter(treeptr->attr.name); 
                    IOinsert(IOroot, table);
                    temp = treeptr->child[0]; //params of function
                    paramcounter = 0;
                    while(temp) {
                        paramcounter++;
                        temp = temp->sibling;
                    }
                    temp = NULL;
                    treeptr->numParams = paramcounter;
                    needCompound = 1;
                    traverseChildS(treeptr, table);
                    table->applyToAll(checkUsed);
                    table->leave();
                    traverseSibS(treeptr, parent, table);
                    break;
                case(ParamK):
                    if( !table->insert(treeptr->attr.name, treeptr)) {
                        printf("ERROR(%d): Symbol \'%s\' is already declared.\n", treeptr->line, treeptr->attr.name);
                        numErrors++;
                    }
                    traverseSibS(treeptr, parent, table);
                    break;
                default:
                    //Debugging Purposes
                    printf("Error: DeclK is not of valid type.\n");
                    break;
            }
            break;
//------------- Statements ---------------------
        case(StmtK):
            switch(treeptr->subkind.stmt) {
                case(NullK):
                    traverseChildS(treeptr, table);
                    traverseSibS(treeptr, parent, table);
                    break;
                case(IfK):
                    if(treeptr->child[0]->isArray) {
                        printf("ERROR(%d): Cannot use array as test condition in if statement.\n", treeptr->line);
                        numErrors++;
                    }
                    else if(treeptr->child[0]->expType != Boolean) {
                        printf("ERROR(%d): Expecting Boolean test condition in if statement but got type %s.\n", treeptr->line, treeptr->child[0]->type);
                        numErrors++;
                    }
                    traverseChildS(treeptr, table);
                    traverseSibS(treeptr, parent, table);
                    break;
                case(WhileK):
                    if(treeptr->child[0]->isArray) {
                        printf("ERROR(%d): Cannot use array as test condition in while statement.\n", treeptr->line);
                        numErrors++;
                    }
                    else if(treeptr->child[0]->expType != Boolean) {
                        printf("ERROR(%d): Expecting Boolean test condition in while statement but got type %s.\n", treeptr->line, treeptr->child[0]->type);
                        numErrors++;
                    }
                    //table->enter(treeptr->attr.string);
                    ///IOinsert(IOroot, table);
                    traverseChildS(treeptr, table);
                    //table->applyToAll(checkUsed);
                    //table->leave();
                    traverseSibS(treeptr, parent, table);
                    break;
                case(ForK):
                    isFor = 1;
                    //semanticAnalysis(treeptr->child[1], treeptr, table);
                    table->enter("ForK"); 
                    IOinsert(IOroot, table);
                    semanticAnalysis(treeptr->child[0], treeptr, table);
                    semanticAnalysis(treeptr->child[2], treeptr, table);
                    table->applyToAll(checkUsed);
                    table->leave();
                    semanticAnalysis(treeptr->child[1], treeptr, table);
                    isFor = 0;
                    traverseSibS(treeptr, parent, table);
                    break;
                case(CompoundK):
                    if(needCompound == 1) {
                        //dont create scope, scope was already created by function decl
                        needCompound = 0;
                        
                        //Force CompoundK t have attributes of parent function
                        treeptr->attr.string = strdup(parent->attr.string);
                        treeptr->attr.name = strdup(parent->attr.name);
                        treeptr->expType = parent->expType;
                        treeptr->type = strdup(parent->type);
                        
                        if(parent->expType != Void) {
                            temp = treeptr->child[1]; //stmtlist
                            if(temp == NULL) {
                                printf("WARNING(%d): Expecting to return type %s but function \'%s\' has no return statement.\n", parent->line, parent->type, parent->attr.name);
                                numWarnings++;
                            }
                            while(temp) {
                                if(temp->subkind.stmt == ReturnK) {
                                    break;
                                }
                                else if(temp->sibling == NULL && temp->subkind.stmt != ReturnK) {
                                    printf("WARNING(%d): Expecting to return type %s but function \'%s\' has no return statement.\n", parent->line, parent->type, parent->attr.name);
                                    numWarnings++;
                                }
                                temp = temp->sibling;
                            }
                            temp = NULL;
                        }
                        traverseChildS(treeptr, table);
                    }
                    else {
                        if(parent->nodeKind == StmtK && parent->subkind.stmt == ForK) {
                            //Bypass scope creation, ForStmt already created one
                        }
                        else {
                            table->enter("CompoundK");
                            IOinsert(IOroot, table);
                            traverseChildS(treeptr, table);
                            table->applyToAll(checkUsed);
                            table->leave();
                            traverseSibS(treeptr, parent, table);
                        }
                    }
                    break;
                case(ReturnK):
                    /*if( treeptr->child[0] != NULL && treeptr->child[0]->isArray) {
                        printf("ERROR(%d): Cannot return an array.\n", treeptr->line);
                        numErrors++;
                    }*/
                    if(parent->expType == Void && treeptr->child[0] != NULL) {
                        printf("ERROR(%d): Function \'%s\' at line %d is expecting no return value, but return has a value.\n", treeptr->line, parent->attr.name, parent->line);
                        numErrors++;
                    }
                    else if(parent->expType != Void && treeptr->child[0] == NULL) {
                        printf("ERROR(%d): Function \'%s\' at line %d is expecting to return type %s but return has no value.\n", treeptr->line, parent->attr.name, parent->line, parent->type);
                        numErrors++;
                    }
                    else if(treeptr->child[0] != NULL && parent->expType != treeptr->child[0]->expType) {
                        printf("ERROR(%d): Function \'%s\' at line %d is expecting to return type %s but returns type %s.\n", treeptr->line, parent->attr.name, parent->line, parent->type, treeptr->child[0]->type);
                        numErrors++;
                    }
                    traverseChildS(treeptr, table);
                    traverseSibS(treeptr, parent, table);
                    break;
                case(BreakK):
                    /*if(parent->subkind.stmt != ForK || parent->subkind.stmt != WhileK) {
                        //printf("%d\n", parent->subkind.stmt);
                        printf("ERROR(%d): Cannot have a break statement outside of loop.\n", treeptr->line);
                        numErrors++;
                    }*/
                    traverseChildS(treeptr, table);
                    traverseSibS(treeptr, parent, table);
                    break;
                case(RangeK):
                    if(treeptr->child[0]->isArray) {
                        printf("ERROR(%d): Cannot use array in position 1 in range of for statement.\n", treeptr->line);
                        numErrors++;
                    }
                    else if(treeptr->child[1]->isArray) {
                        printf("ERROR(%d): Cannot use array in position 2 in range of for statement.\n", treeptr->line);
                        numErrors++;
                    }
                    else if(treeptr->child[2] != NULL && treeptr->child[2]->isArray) {
                        printf("ERROR(%d): Cannot use array in position 3 in range of for statement.\n", treeptr->line);
                        numErrors++;
                    }
                    traverseChildS(treeptr, table);
                    traverseSibS(treeptr, parent, table);
                    break;
                default:
                    //Debugging Purposes
                    printf("StmtK is not of valid type.\n");
                    break;
            }
            break;
//------------- Expressions -------------------
        case(ExpK):
            if(treeptr->child[0]) {
                if(strcmp(treeptr->child[0]->attr.string, (char*)"[") != 0) {
                    expType0 = treeptr->child[0]->expType;
                    if(treeptr->child[0]->subkind.exp == IdK) {
                        child0 = (TreeNode *)table->lookup(treeptr->child[0]->attr.name);
                        if(child0 == NULL) {
                            child0 = (TreeNode *)table->lookupGlobal(treeptr->child[0]->attr.name);
                        }
                        if(child0) {
                            child0->declUsed = 1;
                        }
                    }
                }   
                else {
                    expType0 = treeptr->child[0]->child[0]->expType;
                    if(treeptr->child[0]->child[0]->subkind.exp == IdK) {
                        child0 = (TreeNode *)table->lookup(treeptr->child[0]->child[0]->attr.name);
                        if(child0 == NULL) {
                            child0 = (TreeNode *)table->lookupGlobal(treeptr->child[0]->child[0]->attr.name);
                        }
                        if(child0) {
                            child0->declUsed = 1;
                        }
                    }
                    else { printf("This shouldn't ever occur\n"); }
                }
            }
            if(treeptr->child[1]) {
                if(strcmp(treeptr->child[1]->attr.string, (char*)"[") != 0) {
                    expType1 = treeptr->child[1]->expType;
                    if(treeptr->child[1]->subkind.exp == IdK) {
                        child1 = (TreeNode *)table->lookup(treeptr->child[1]->attr.name);
                        if(child1 == NULL) {
                            child1 = (TreeNode *)table->lookupGlobal(treeptr->child[1]->attr.name);
                        }
                        if(child1) {
                            child1->declUsed = 1;
                        }
                    }
                }
                else {
                    expType1 = treeptr->child[1]->child[1]->expType;
                    if(treeptr->child[1]->child[1]->subkind.exp == IdK) {
                        child1 = (TreeNode *)table->lookup(treeptr->child[1]->child[1]->attr.name);
                        if(child1 == NULL) {
                            child1 = (TreeNode *)table->lookupGlobal(treeptr->child[1]->child[1]->attr.name);
                        }
                        if(child1) {
                            child1->declUsed = 1;
                        }
                    }
                    else { printf("This shouldn't ever occur\n"); }
                }
            } 
            if(treeptr->subkind.exp != InitK) {
                if(child0 && child0->isInit == 0 && child0->initWarn == 0 && child1 == NULL) {
                    printf("WARNING(%d): Variable \'%s\' may be uninitialized when used here.\n", treeptr->line, treeptr->child[0]->attr.name);
                    numWarnings++;
                    child0->initWarn = 1;
                }
                if(child1 && child1->isInit == 0 && child1->initWarn == 0) {
                    printf("WARNING(%d): Variable \'%s\' may be uninitialized when used here.\n", treeptr->line, treeptr->child[1]->attr.name);
                    numWarnings++;
                    temp->initWarn = 1;
                }
            }
            switch(treeptr->subkind.exp) {
                case(OpK):
                    if(strcmp(treeptr->attr.string, (char*)"=") == 0) {
                        //can have arrays, nbut both must be arrays
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if(treeptr->child[0]->isArray ^ treeptr->child[1]->isArray) {
                            if(treeptr->child[0]->isArray) {
                                printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is an array and rhs is not an array.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            else {
                                printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is not an array and rhs is an array.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                        }
                        else if(expType0 != expType1) {
                            printf("ERROR(%d): \'%s\' requires operands of the same type but lhs is type %s and rhs is type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type, child1->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"!=") == 0) {
                        //can have arrays
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if(treeptr->child[0]->isArray ^ treeptr->child[1]->isArray) {
                            if(treeptr->child[0]->isArray) {
                                printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is an array and rhs is not an array.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            else {
                                printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is not an array and rhs is an array.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                        }
                        else if(expType0 != expType1) {
                            printf("ERROR(%d): \'%s\' requires operands of the same type but lhs is type %s and rhs is type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type, treeptr->child[1]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"<")== 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if(treeptr->child[0]->isArray ^ treeptr->child[1]->isArray) {
                            if(treeptr->child[0]->isArray) {
                                printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is an array and rhs is not an array.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            else {
                                printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is not an array and rhs is an array.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                        }
                        else if(expType0 != expType1) {
                            printf("ERROR(%d): \'%s\' requires operands of the same type but lhs is type %s and rhs is type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type, treeptr->child[1]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"<=") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if(treeptr->child[0]->isArray ^ treeptr->child[1]->isArray) {
                            if(treeptr->child[0]->isArray) {
                                printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is an array and rhs is not an array.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            else {
                                printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is not an array and rhs is an array.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                        }
                        else if(expType0 != expType1) {
                            printf("ERROR(%d): \'%s\' requires operands of the same type but lhs is type %s and rhs is type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type, treeptr->child[1]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char*)">") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if(treeptr->child[0]->isArray ^ treeptr->child[1]->isArray) {
                            if(treeptr->child[0]->isArray) {
                                printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is an array and rhs is not an array.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            else {
                                printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is not an array and rhs is an array.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                        }
                        else if(expType0 != expType1) {
                            printf("ERROR(%d): \'%s\' requires operands of the same type but lhs is type %s and rhs is type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type, treeptr->child[1]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char*)">=") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if(treeptr->child[0]->isArray ^ treeptr->child[1]->isArray) {
                            if(treeptr->child[0]->isArray) {
                                printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is an array and rhs is not an array.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            else {
                                printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is not an array and rhs is an array.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                        }
                        else if(expType0 != expType1) {
                            printf("ERROR(%d): \'%s\' requires operands of the same type but lhs is type %s and rhs is type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type, treeptr->child[1]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"+") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if(treeptr->child[0]->isArray || treeptr->child[1]->isArray) {
                            printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                            numErrors++;
                        }
                        else if( expType0 != Integer) {
                            printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                            numErrors++;
                        }
                        else if(expType1 != Integer) {
                            printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"-") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if(treeptr->child[0]->isArray || treeptr->child[1]->isArray) {
                            printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                            numErrors++;
                        }
                        else if(expType0 != Integer) {
                            printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                            numErrors++;
                        }
                        else if(expType1 != Integer) {
                            printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"*") == 0) {
                        if(child1 != NULL) {
                            if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                                //Bypass any errors
                            }
                            else if(treeptr->child[0]->isArray || treeptr->child[1]->isArray) {
                                printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            else if(expType0 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                                numErrors++;
                            }
                            else if(expType1 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        else  {
                            printf("Debugging Note: this token is reserved for binary multiplication. treeptr->attr.string (or token-tokenstr) should be \"sizeof\" here\n");
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"/") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if(treeptr->child[0]->isArray || treeptr->child[1]->isArray) {
                            printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                            numErrors++;
                        }
                        else if(expType0 != Integer) {
                            printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                            numErrors++;
                        }
                        else if(expType1 != Integer) {
                            printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"%") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if(treeptr->child[0]->isArray || treeptr->child[1]->isArray) {
                            printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                            numErrors++;
                        }
                        else if(expType0 != Integer) {
                            printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                            numErrors++;
                        }
                        else if(expType1 != Integer) {
                            printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"[") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if(!treeptr->child[0]->isArray) {
                            printf("ERROR(%d): Cannot index nonarray \'%s\'.\n", treeptr->line, treeptr->child[0]->attr.name);
                            numErrors++;
                        }
                        else if(expType1 != Integer) {
                            printf("ERROR(%d): Array \'%s\' should be indexed by type int but got type %s.\n", treeptr->line, treeptr->child[0]->attr.name, treeptr->child[1]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"sizeof") == 0) { //not done
                        if(child1) {
                            printf("Debugging Error: sizeof should not have child1");
                        }
                        if( expType0 == UndefinedType || expType0 == Void) {
                            //Bypass any errors
                        }
                        else if(child0->isArray == 0) {
                            printf("ERROR(%d): The operation \'%s\' only works with arrays.\n", treeptr->line, treeptr->attr.string);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"?") == 0) {
                        if(child1) {
                            printf("Debugging Error: ? should not have child1");
                        }
                        if( expType0 == UndefinedType || expType0 == Void) {
                            //Bypass any errors
                        }
                        else if(treeptr->child[0]->isArray) {
                            printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                            numErrors++;
                        }
                        else if(expType0 != Integer) {
                            printf("ERROR(%d): Unary \'%s\' requires an operand of type int but was given type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"chsign") == 0) {
                        if(child1) {
                            printf("Debugging Error: chsign should not have child1");
                        }
                        if( expType0 == UndefinedType || expType0 == Void) {
                            //Bypass any errors
                        }
                        else if(treeptr->child[0]->isArray) {
                            printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                            numErrors++;
                        }
                        else if(expType0 != Integer) {
                            printf("ERROR(%d): Unary \'%s\' requires an operand of type int but was given type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"not") == 0) {
                        if(child1) {
                            printf("Debugging Error: not should not have child1");
                        }
                        if( expType0 == UndefinedType || expType0 == Void) {
                            //Bypass any errors
                        }
                        else if(treeptr->child[0]->isArray) {
                            printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                            numErrors++;
                        }
                        else if(expType0 != Boolean) {
                            printf("ERROR(%d): Unary \'%s\' requires an operand of type bool but was given type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"and") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if(treeptr->child[0]->isArray || treeptr->child[1]->isArray) {
                            printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                            numErrors++;
                        }
                        else if(expType0 != Boolean) {
                            printf("ERROR(%d): \'%s\' requires operands of type bool but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                            numErrors++;
                        }
                        else if(expType1 != Boolean) {
                            printf("ERROR(%d): \'%s\' requires operands of type bool but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"or") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if(treeptr->child[0]->isArray || treeptr->child[1]->isArray) {
                            printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                            numErrors++;
                        }
                        else if(expType0 != Boolean) {
                            printf("ERROR(%d): \'%s\' requires operands of type bool but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                            numErrors++;
                        }
                        else if(expType1 != Boolean) {
                            printf("ERROR(%d): \'%s\' requires operands of type bool but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    break;
                case(ConstantK):
                        traverseSibS(treeptr, parent, table);
                    break;
                case(IdK):
                    temp = (TreeNode *)table->lookup(treeptr->attr.name);
                    if(!temp) {
                        temp = (TreeNode *)table->lookupGlobal(treeptr->attr.name);
                        if(!temp) {
                            printf("ERROR(%d): Symbol \'%s\' is not declared.\n", treeptr->line, treeptr->attr.name);
                            numErrors++;
                        }
                        else {
                            if(temp->subkind.decl == FuncK) {
                                printf("Error(%d): Cannot use funcion \'%s\' as a variable.\n", treeptr->line, treeptr->attr.name);
                                numErrors++;
                            }
                            else if(parent->subkind.exp == InitK) {
                                temp->isInit = 1;
                            }
                            else if (temp->isInit == 0 && temp->initWarn == 0 && parent->child[1] == NULL) {
                                printf("WARNING(%d): GIVariable \'%s\' may be uninitialized when used here.\n", treeptr->line, treeptr->attr.name);
                                numWarnings++;
                                temp->initWarn = 1;
                            }
                            temp->declUsed = 1;
                        }
                        temp = NULL;
                    }
                    else {
                        if(temp->subkind.decl == FuncK) {
                            printf("Error(%d): Cannot use funcion \'%s\' as a variable.\n", treeptr->line, treeptr->attr.name);
                            numErrors++;
                        }
                        else if(parent->subkind.exp == InitK) {
                            temp->isInit = 1;
                        }
                        else if (temp->isInit == 0 && temp->initWarn == 0 && parent->child[1] == NULL) {
                            printf("WARNING(%d): IVariable \'%s\' may be uninitialized when used here.\n", treeptr->line, treeptr->attr.name);
                            numWarnings++;
                            temp->initWarn = 1;
                        }
                        temp->declUsed = 1;
                    }
                    temp = NULL;
                    //Ids should not have children or siblings;
                    //If ID array, which has children, this reverts to OpK exp on c-.y 476
                    traverseSibS(treeptr, parent, table);
                    break;
                case(AssignK):
                    if(strcmp(treeptr->attr.string, (char *)"++") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void) {
                            //Bypass any errors
                        }
                        else if( treeptr->child[0]->isArray) {
                            printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                            numErrors++;
                        }
                        else if(expType0 != Integer) {
                            printf("ERROR(%d): Unary \'%s\' requires an operand of type int but was given type %s.\n", treeptr->child[0]->line, treeptr->attr.string, treeptr->child[0]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"--") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void) {
                            //Bypass any errors
                        }
                        else if( treeptr->child[0]->isArray) {
                            printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                            numErrors++;
                        }
                        else if(expType0 != Integer) {
                            printf("ERROR(%d): Unary \'%s\' requires an operand of type int but was given type %s.\n", treeptr->child[0]->line, treeptr->attr.string, treeptr->child[0]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"<-")  == 0) {
		        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if(expType0 != expType1) {
                            printf("ERROR(%d): \'%s\' requires operands of the same type but lhs is type %s and rhs is type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type, treeptr->child[1]->type);
                            numErrors++;
                        }
   	                else if(treeptr->child[0]->isArray ^ treeptr->child[1]->isArray) {
                            if(treeptr->child[0]->isArray) {
                                if(treeptr->child[1]->subkind.exp == ConstantK) {
                                    //bypass error
                                }
                                else {
                                    printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is an array and rhs is not an array.\n", treeptr->line, treeptr->attr.string);
                                     numErrors++;
                                }
                            }
                            else {
                                printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is not an array and rhs is an array.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                        }
                        temp = (TreeNode *)table->lookup(treeptr->child[0]->attr.name);
                        if(temp) {
                            temp->isInit = 1;
                        }
                        temp = NULL;
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"+=") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if( treeptr->child[0]->isArray) {
                            printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                            numErrors++;
                        }
                        else if(expType0 != Integer) {
                            printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                            numErrors++;
                        }
                        else if(expType1 != Integer) {
                            printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"-=") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if( treeptr->child[0]->isArray) {
                            printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                            numErrors++;
                        }
                        else if(expType0 != Integer) {
                            printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                            numErrors++;
                        }
                        else if(expType1 != Integer) {
                            printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"*=") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if( treeptr->child[0]->isArray) {
                            printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                            numErrors++;
                        }
                        else if(expType0 != Integer) {
                            printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                            numErrors++;
                        }
                        else if(expType1 != Integer) {
                            printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"/=") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else if( treeptr->child[0]->isArray) {
                            printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                            numErrors++;
                        }
                        else if(expType0 != Integer) {
                            printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                            numErrors++;
                        }
                        else if(expType1 != Integer) {
                            printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                            numErrors++;
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    break;
                case(InitK):
                    if(treeptr->child[1]->subkind.exp != ConstantK) {
                        printf("ERROR(%d): Initializer for variable \'%s\' is not a constant expression.\n", treeptr->line, treeptr->child[0]->attr.name);
                        numErrors++;
                    }
                    else if(expType0 != expType1) {
                        printf("ERROR(%d): Initializer for variable \'%s\' of type %s is of type %s\n", treeptr->line, treeptr->child[0]->attr.name, treeptr->child[0]->type, treeptr->child[1]->type);
                        numErrors++;
                    }
                    else if(treeptr->child[0]->isArray ^ treeptr->child[1]->isArray) {
                            if(treeptr->child[0]->isArray) {
                                printf("ERROR(%d): Initializer for variable \'%s\' requires both operands be arrays or not but lhs is an array and rhs is not an array.\n", treeptr->line, treeptr->child[0]->attr.name);
                                numErrors++;
                            }
                            else {
                                printf("ERROR(%d): Initializer for variable\'%s\' requires both operands be arrays or not but lhs is not an array and rhs is an array.\n", treeptr->line, treeptr->child[0]->attr.name);
                                numErrors++;
                            }
                        }
                    traverseChildS(treeptr, table);
                    traverseSibS(treeptr, parent, table);
                    break;
                case(CallK):
                    temp = (TreeNode *)table->lookup(treeptr->attr.name);
                    if(!temp) {
                        printf("ERROR(%d): Symbol \'%s\' is not declared.\n", treeptr->line, treeptr->attr.name);
                        numErrors++;
                    }
                    else {
                        temp->declUsed = 1;
                        paramcounter = 0;
                        temp2 = treeptr->child[0]; //args list
                        while(temp2) {
                            paramcounter++;
                            temp2 = temp2->sibling;
                        }
                        if(temp->subkind.decl != FuncK) {
                            printf("ERROR(%d): \'%s\' is a simple variable and cannot be called.\n", treeptr->line, temp->attr.name);
                            numErrors++;
                        }
                        else if(paramcounter != temp->numParams) {
                            if(paramcounter < temp->numParams) {
                                printf("ERROR(%d): Too few parameters passed for function \'%s\' declared on line %d.\n", treeptr->line, treeptr->attr.name, temp->line);
                                numErrors++;
                            }
                            else {
                                printf("ERROR(%d): Too many parameters passed for function \'%s\' declared on line %d.\n", treeptr->line, treeptr->attr.name, temp->line);
                                numErrors++;
                            }
                        }
                        else if(temp->isIO == 0){
                            line = temp->line;
                            paramcounter = 1;
                            temp = temp->child[0];
                            temp2 = treeptr->child[0];
                            while(temp && temp2) {
                                if(temp->expType != temp2->expType) {
                                    printf("ERROR(%d): Expecting type %s in parameter %i of call to \'%s\' declared on line %d but got type %s.\n", temp2->line, temp->type, paramcounter, treeptr->attr.name, line, temp2->type);
                                    numErrors++;
                                }
                                else {
                                    if(temp->isArray == 1 && temp2->isArray == 0) {
                                        printf("ERROR(%d): Expecting array in parameter %i of call to \'%s\' declared on line %d.\n", temp2->line, paramcounter, treeptr->attr.name, line);
                                        numErrors++;
                                    }
                                    else if(temp->isArray == 0 && temp2->isArray == 1) {
                                        printf("ERROR(%d): Not expecting array in parameter %i of call to \'%s\' declared on line %d.\n", temp2->line, paramcounter, treeptr->attr.name, line);
                                        numErrors++;
                                    }
                                }
                                paramcounter++;
                                temp = temp->sibling;
                                temp2 = temp2->sibling;
                            }
                            line = 0;
                        }
                    }
                    temp2 = NULL;
                    temp = NULL;
                    traverseSibS(treeptr, parent, table);
                    break;
                default:
                    //Debugging Purposes
                    printf("Error: ExpK %s is not of valid type.\n", treeptr->attr.string);
                    break;
            }
            break;
        default:
            //Node is not of decl, stmt, or exp type
            printf("This print is default in semanticAnalysis.\n");
            break;
    }
    return;
}
