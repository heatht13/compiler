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
        if(treeptr->nodeKind == DeclK) {
            table->insert(treeptr->attr.name, treeptr);
        }
        else if(treeptr->nodeKind == StmtK && treeptr->subkind.stmt == CompoundK) {
            table->enter("PassScope");
            for( i=0; i<MAXCHILDREN; i++) {
                if(treeptr->child[i] != NULL) {
                    passTypes(treeptr->child[i], treeptr, table);
                }
            }
            table->leave();
        }
        for( i=0; i<MAXCHILDREN; i++) {
            if(treeptr->child[i] != NULL/*&& treeptr->child[i]->expType == UndefinedType*/) {
                passTypes(treeptr->child[i], treeptr, table);
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
                    } //WORK NEEDED
                    else if(strcmp(treeptr->attr.string, (char*)"[") == 0) {
                        if(treeptr->child[0]->expType == UndefinedType) {
                            passTypes(treeptr->child[0], treeptr, table);
                        }
                        if(treeptr->child[1]->expType == UndefinedType) {
                            passTypes(treeptr->child[1], treeptr, table);
                        }
                        else {
                            treeptr->expType = treeptr->child[0]->expType;
                            if(treeptr->child[0]->type) {
                                treeptr->type = strdup(treeptr->child[0]->type);
                            }
                            treeptr->isStatic = treeptr->child[0]->isStatic;
                            treeptr->isInit = treeptr->child[0]->isInit;
                            treeptr->varSize = treeptr->child[0]->varSize;
                            //treeptr->isArray = treeptr->child[0]->isArray;
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
                        if(treeptr->child[0]->expType == UndefinedType) {
                            passTypes(treeptr->child[0], treeptr, table);
                        }
                        if(treeptr->child[1]->expType == UndefinedType) {
                            passTypes(treeptr->child[1], treeptr, table);
                        }
                        else {
                            treeptr->expType = treeptr->child[0]->expType;
                            if(treeptr->child[0]->type) {
                                treeptr->type = strdup(treeptr->child[0]->type);
                            }
                            if(treeptr->child[1]->isArray) {
                                treeptr->isArray = 1;
                            }
                            else {
                                treeptr->isArray = 0;
                            }
                            //treeptr->isStatic = treeptr->child[0]->isStatic;
                            //treeptr->isInit = 1;
                            temp = (TreeNode *)table->lookup(treeptr->child[0]->attr.name);
                            if(temp) {
                                temp->isInit = 1;
                            }
                            treeptr->varSize = treeptr->child[0]->varSize;
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
                //case(ConstantK):
                    //Nothing to pass up here yet?
                   // break;
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
                        treeptr->varSize = temp->varSize;
                        treeptr->declUsed = temp->declUsed;
                        treeptr->initWarn = temp->initWarn;
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
                        //Below Code is a fast, but not best fi for handling inits
                        //temp->child[0] = treeptr->child[1];
                    }
                    temp = NULL;
                    treeptr->varSize = treeptr->child[0]->varSize;
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
                        treeptr->varSize = temp->varSize;
                        treeptr->isStatic = temp->isStatic;
                        treeptr->numParams = temp->numParams;
                        treeptr->isIO = temp->isIO;
                    }
                    temp = NULL;
                    break;
                default:
                        //printf("Error in passType: %s doesn't have a valid ExpKind.\n", treeptr->attr.string);
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
    static TreeNode *temp3;
    static TreeNode *temp4;
    static int isConstParm;
    static char *scopeName;
    table->debug(false);
    if(treeptr == NULL){
        return;
    }
    int i;
    switch(treeptr->nodeKind) {
//------------- Declarations -------------------
        case(DeclK):
            switch(treeptr->subkind.decl) {
                case(VarK):
                    if(!table->insert(treeptr->attr.name, treeptr)) {
                        temp = (TreeNode *)table->lookup(treeptr->attr.name);
                        printf("ERROR(%d): Symbol \'%s\' is already declared at line %d.\n", treeptr->line, treeptr->attr.name, temp->line);
                        numErrors++;
                        temp = NULL;
                    }
                    else {
                        scopeName = strdup(table->getScope());
                        if(strcmp(scopeName, (char *)"Global") == 0) {
                            treeptr->varKind = Global;
                            treeptr->offset = goffset;
                            if(treeptr->isArray) {
                                treeptr->offset = goffset-1;
                            }
                            goffset-=treeptr->varSize;
                        }
                        else if (treeptr->isStatic) {
                            treeptr->varKind = LocalStatic;
                            treeptr->offset = goffset;
                            if(treeptr->isArray) {
                                treeptr->offset = goffset-1;
                            }
                            goffset-=treeptr->varSize;
                        }
                        else {
                            treeptr->varKind = Local;
                            treeptr->offset = foffset;
                            if(treeptr->isArray) {
                                treeptr->offset = foffset-1;
                            }
                            foffset-=treeptr->varSize;
                        }
                        if(treeptr->child[0] && treeptr->child[0]->nodeKind == ExpK) {
                            if(treeptr->child[0]->subkind.exp != ConstantK) {
                                printf("ERROR(%d): Initializer for variable \'%s\' is not a constant expression.\n", treeptr->line, treeptr->child[0]->attr.name);
                                numErrors++;
                            }
                            else if(treeptr->expType != treeptr->child[0]->expType) {
                                printf("ERROR(%d): Initializer for variable \'%s\' of type %s is of type %s\n", treeptr->line, treeptr->child[0]->attr.name, treeptr->child[0]->type, treeptr->child[1]->type);
                            numErrors++;
                            }
                            else if(treeptr->isArray ^ treeptr->child[0]->isArray) {
                                if(treeptr->isArray) {
                                    printf("ERROR(%d): Initializer for variable \'%s\' requires both operands be arrays or not but lhs is an array and rhs is not an array.\n", treeptr->line, treeptr->child[0]->attr.name);
                                    numErrors++;
                                }
                                else {
                                    printf("ERROR(%d): Initializer for variable\'%s\' requires both operands be arrays or not but lhs is not an array and rhs is an array.\n", treeptr->line, treeptr->child[0]->attr.name);
                                    numErrors++;
                                }
                            }
                        }
                    }
                    traverseChildS(treeptr, table);
                    traverseSibS(treeptr, parent, table);
                    break;
                case(FuncK):
                    if( !table->insert(treeptr->attr.name, treeptr)) {
                        temp = (TreeNode *)table->lookup(treeptr->attr.name);
                        printf("ERROR(%d): Symbol \'%s\' is already declared at line %d.\n", treeptr->line, treeptr->attr.name, temp->line);
                        numErrors++;
                        temp = NULL;
                    }
                    else {
                        foffset = -2;//leave room for return frame ptr and return address
                        treeptr->varKind = Global;
                        treeptr->offset = 0;
                        table->enter(treeptr->attr.name); 
                        temp = treeptr->child[0]; //params of function
                        paramcounter = 0;
                        while(temp) {
                            paramcounter++;
                            temp = temp->sibling;
                        }
                        temp = NULL;
                        treeptr->numParams = paramcounter;
                        treeptr->varSize = -paramcounter-2;//adds two words for ret ticket
                        needCompound = 1;
                        traverseChildS(treeptr, table);
                        table->applyToAll(checkUsed);
                        table->leave();
                        traverseSibS(treeptr, parent, table);
                        break;
                    }
                case(ParamK):
                    if( !table->insert(treeptr->attr.name, treeptr)) {
                        temp = (TreeNode *)table->lookup(treeptr->attr.name);
                        printf("ERROR(%d): Symbol \'%s\' is already declared at line %d.\n", treeptr->line, treeptr->attr.name, temp->line);
                        numErrors++;
                        temp = NULL;
                    }
                    else {
                        treeptr->varKind = Parameter;
                        treeptr->offset = foffset;
                        foffset-=treeptr->varSize;
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
                    //IOinsert(IOroot, table);
                    treeptr->offset = foffset;
                    semanticAnalysis(treeptr->child[0], treeptr, table);
                    semanticAnalysis(treeptr->child[2], treeptr, table);
                    treeptr->varSize = foffset;
                    table->applyToAll(checkUsed);
                    table->leave();
                    foffset = treeptr->offset;
                    treeptr->offset = 0;
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
                        static bool found = false;
                        if(parent->expType != Void) {
                            temp = treeptr->child[1]; //stmtlist
                            if(temp == NULL) {
                                printf("WARNING(%d): Expecting to return type %s but function \'%s\' has no return statement.\n", parent->line, parent->type, parent->attr.name);
                                numWarnings++;
                            }
                            else {
                                while(!found && temp) {
                                    temp2 = temp;
                                    while( !found && temp2) {
                                        if(temp2->subkind.stmt == ReturnK) {
                                            found = true;
                                        }
                                        temp2 = temp2->sibling;
                                    }
                                    temp = temp->child[0];
                                }
                                if(!found) {
                                    printf("WARNING(%d): Expecting to return type %s but function \'%s\' has no return statement.\n", parent->line, parent->type, parent->attr.name);
                                    numWarnings++;
                                }
                            }
                            temp = NULL;
                            temp2 = NULL;
                        }
                        //not setting foffset or tree offset here, just using for varSize
                        treeptr->offset = foffset;
                        traverseChildS(treeptr, table);
                        treeptr->varSize = foffset;
                        foffset = treeptr->offset;
                        treeptr->offset = 0;
                    }
                    else {
                        if(parent->nodeKind == StmtK && parent->subkind.stmt == ForK) {
                            //Bypass scope creation, ForStmt already created one
                        }
                        else {
                            table->enter("CompoundK");
                            //IOinsert(IOroot, table);
                            treeptr->offset = foffset;
                            traverseChildS(treeptr, table);
                            treeptr->varSize = foffset;
                            table->applyToAll(checkUsed);
                            table->leave();
                            foffset = treeptr->offset;
                            treeptr->offset = 0;
                            traverseSibS(treeptr, parent, table);
                        }
                    }
                    break;
                case(ReturnK):
                    scopeName = strdup(table->getScope());
                    temp = (TreeNode *)table->lookup(scopeName);
                    if(temp && temp->subkind.decl == FuncK) {
                        expType0 = temp->expType;
                        /*if( treeptr->child[0] != NULL && treeptr->child[0]->isArray) {
                            printf("ERROR(%d): Cannot return an array.\n", treeptr->line);
                            numErrors++;
                        }*/ 
                        if(expType0 == Void && treeptr->child[0] != NULL) {
                            printf("ERROR(%d): Function \'%s\' at line %d is expecting no return value, but return has a value.\n", treeptr->line, temp->attr.name, temp->line);
                            numErrors++;
                        }
                        else if(expType0 != Void && treeptr->child[0] == NULL) {
                            printf("ERROR(%d): Function \'%s\' at line %d is expecting to return type %s but return has no value.\n", treeptr->line, temp->attr.name, temp->line, temp->type);
                            numErrors++;
                        }
                        else if(treeptr->child[0] != NULL && expType0 != treeptr->child[0]->expType) {
                            printf("ERROR(%d): Function \'%s\' at line %d is expecting to return type %s but returns type %s.\n", treeptr->line, temp->attr.name, temp->line, temp->type, treeptr->child[0]->type);
                            numErrors++;
                        }
                    }
                    else {
                    }
                    temp = NULL;
                    expType0 = -1;
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
                expType0 = treeptr->child[0]->expType;
            }
            if(treeptr->child[1]) {
                expType1 = treeptr->child[1]->expType;
            }
            if(treeptr->subkind.exp != InitK && treeptr->subkind.exp != CallK) {
                //Ensure ExpK is a unary OpK
                if(treeptr->child[0] && treeptr->child[0]->subkind.exp == IdK && treeptr->child[1] == NULL) {
                    temp = (TreeNode *)table->lookup(treeptr->child[0]->attr.name);
                    if(temp) {
                        if(temp->isInit == 0 && temp->initWarn == 0) {
                            printf("WARNING(%d): Variable \'%s\' may be uninitialized when used here.\n", treeptr->line, treeptr->child[0]->attr.name);
                            numWarnings++;
                            temp->initWarn = 1;
                        } 
                    }
                    temp = NULL;
                }
                //Ensure ExpK is either an assignK or opK
                if(treeptr->child[1] && treeptr->child[1]->subkind.exp == IdK) {
                    temp = (TreeNode *)table->lookup(treeptr->child[1]->attr.name);
                    if(temp) {
                        if(temp->isInit == 0 && temp->initWarn == 0) {
                            printf("WARNING(%d): Variable \'%s\' may be uninitialized when used here.\n", treeptr->line, treeptr->child[1]->attr.name);
                            numWarnings++;
                            temp->initWarn = 1;
                        }
                    }
                    temp = NULL;
                }
            }
            switch(treeptr->subkind.exp) {
                case(OpK):
                    if(strcmp(treeptr->attr.string, (char*)"=") == 0) {
                        //can have arrays, nbut both must be arrays
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else {
                            if(treeptr->child[0]->isArray ^ treeptr->child[1]->isArray) {
                                if(treeptr->child[0]->isArray) {
                                    printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is an array and rhs is not an array.\n", treeptr->line, treeptr->attr.string);
                                    numErrors++;
                                }
                                else {
                                    printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is not an array and rhs is an array.\n", treeptr->line, treeptr->attr.string);
                                    numErrors++;
                                }
                            }
                            if(expType0 != expType1) {
                                printf("ERROR(%d): \'%s\' requires operands of the same type but lhs is type %s and rhs is type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"!=") == 0) {
                        //can have arrays
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else {
                            if(treeptr->child[0]->isArray ^ treeptr->child[1]->isArray) {
                                if(treeptr->child[0]->isArray) {
                                    printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is an array and rhs is not an array.\n", treeptr->line, treeptr->attr.string);
                                    numErrors++;
                                }
                                else {
                                    printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is not an array and rhs is an array.\n", treeptr->line, treeptr->attr.string);
                                    numErrors++;
                                }
                            }
                            if(expType0 != expType1) {
                                printf("ERROR(%d): \'%s\' requires operands of the same type but lhs is type %s and rhs is type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"<")== 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else {
                            if(treeptr->child[0]->isArray ^ treeptr->child[1]->isArray) {
                                if(treeptr->child[0]->isArray) {
                                    printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is an array and rhs is not an array.\n", treeptr->line, treeptr->attr.string);
                                    numErrors++;
                                }
                                else {
                                    printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is not an array and rhs is an array.\n", treeptr->line, treeptr->attr.string);
                                    numErrors++;
                                }
                            }
                            if(expType0 != expType1) {
                                printf("ERROR(%d): \'%s\' requires operands of the same type but lhs is type %s and rhs is type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char*)"<=") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else {
                            if(treeptr->child[0]->isArray ^ treeptr->child[1]->isArray) {
                                if(treeptr->child[0]->isArray) {
                                    printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is an array and rhs is not an array.\n", treeptr->line, treeptr->attr.string);
                                    numErrors++;
                                }
                                else {
                                    printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is not an array and rhs is an array.\n", treeptr->line, treeptr->attr.string);
                                    numErrors++;
                                }
                            }
                            if(expType0 != expType1) {
                                printf("ERROR(%d): \'%s\' requires operands of the same type but lhs is type %s and rhs is type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char*)">") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else {
                            if(treeptr->child[0]->isArray ^ treeptr->child[1]->isArray) {
                                if(treeptr->child[0]->isArray) {
                                    printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is an array and rhs is not an array.\n", treeptr->line, treeptr->attr.string);
                                    numErrors++;
                                }
                                else {
                                    printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is not an array and rhs is an array.\n", treeptr->line, treeptr->attr.string);
                                    numErrors++;
                                }
                            }
                            if(expType0 != expType1) {
                                printf("ERROR(%d): \'%s\' requires operands of the same type but lhs is type %s and rhs is type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char*)">=") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else {
                            if(treeptr->child[0]->isArray ^ treeptr->child[1]->isArray) {
                                if(treeptr->child[0]->isArray) {
                                    printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is an array and rhs is not an array.\n", treeptr->line, treeptr->attr.string);
                                    numErrors++;
                                }
                                else {
                                    printf("ERROR(%d): \'%s\' requires both operands be arrays or not but lhs is not an array and rhs is an array.\n", treeptr->line, treeptr->attr.string);
                                    numErrors++;
                                }
                            }
                            if(expType0 != expType1) {
                                printf("ERROR(%d): \'%s\' requires operands of the same type but lhs is type %s and rhs is type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"+") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else {
                            if( treeptr->child[0]->isArray || treeptr->child[1]->isArray) {
                                printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            if( expType0 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                                numErrors++;
                            }
                            if(expType1 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"-") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else {
                            if( treeptr->child[0]->isArray || treeptr->child[1]->isArray) {
                                printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            if(expType0 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                                numErrors++;
                            }
                            if(expType1 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"*") == 0) {
                        if(treeptr->child[1] != NULL) {
                            if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                                //Bypass any errors
                            }
                            else {
                                if( treeptr->child[0]->isArray || treeptr->child[1]->isArray) {
                                    printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                                    numErrors++;
                                }
                                if(expType0 != Integer) {
                                    printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                                    numErrors++;
                                }
                                if(expType1 != Integer) {
                                    printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                                    numErrors++;
                                }
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
                        else {
                            if( treeptr->child[0]->isArray || treeptr->child[1]->isArray) {
                                printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            if(expType0 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                                numErrors++;
                            }
                            if(expType1 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"%") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else {
                            if( treeptr->child[0]->isArray || treeptr->child[1]->isArray) {
                                printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            if(expType0 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                                numErrors++;
                            }
                            if(expType1 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"[") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else {
                            if(!treeptr->child[0]->isArray) {
                                printf("ERROR(%d): Cannot index nonarray \'%s\'.\n", treeptr->line, treeptr->child[0]->attr.name);
                                numErrors++;
                            }
                            else {
                                treeptr->isArray = 1;
                            }
                            if(expType1 != Integer) {
                                printf("ERROR(%d): Array \'%s\' should be indexed by type int but got type %s.\n", treeptr->line, treeptr->child[0]->attr.name, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"sizeof") == 0) { //not done
                        if( expType0 == UndefinedType || expType0 == Void) {
                            //Bypass any errors
                        }
                        else {
                            if(!treeptr->child[0]->isArray) {
                                printf("ERROR(%d): The operation \'%s\' only works with arrays.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"?") == 0) {
                       if( expType0 == UndefinedType || expType0 == Void) {
                            //Bypass any errors
                        }
                        else {
                            if( treeptr->child[0]->isArray) {
                                printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            if(expType0 != Integer) {
                                printf("ERROR(%d): Unary \'%s\' requires an operand of type int but was given type %s.\n", treeptr->child[0]->line, treeptr->attr.string, treeptr->child[0]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"chsign") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void) {
                            //Bypass any errors
                        }
                        else {
                            if( treeptr->child[0]->isArray) {
                                printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            if(expType0 != Integer) {
                                printf("ERROR(%d): Unary \'%s\' requires an operand of type int but was given type %s.\n", treeptr->child[0]->line, treeptr->attr.string, treeptr->child[0]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"not") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void) {
                            //Bypass any errors
                        }
                        else {
                            if( treeptr->child[0]->isArray) {
                                printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            if(expType0 != Boolean) {
                                printf("ERROR(%d): Unary \'%s\' requires an operand of type bool but was given type %s.\n", treeptr->child[0]->line, treeptr->attr.string, treeptr->child[0]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"and") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else {
                            if( treeptr->child[0]->isArray || treeptr->child[1]->isArray) {
                                printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            if(expType0 != Boolean) {
                                printf("ERROR(%d): \'%s\' requires operands of type bool but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                                numErrors++;
                            }
                            if(expType1 != Boolean) {
                                printf("ERROR(%d): \'%s\' requires operands of type bool but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"or") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else {
                            if( treeptr->child[0]->isArray || treeptr->child[1]->isArray) {
                                printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            if(expType0 != Boolean) {
                                printf("ERROR(%d): \'%s\' requires operands of type bool but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                                numErrors++;
                            }
                            if(expType1 != Boolean) {
                                printf("ERROR(%d): \'%s\' requires operands of type bool but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    break;
                case(ConstantK):
                    if(treeptr->isArray && treeptr->expType == Char) {
                        treeptr->varKind = Global;
                        if(treeptr->isArray) {
                            //attr.string already holds and accounts for '\0'
                            treeptr->varSize = (int)strlen(treeptr->attr.string)-1;
                        }
                        else {
                            treeptr->varSize = 1;
                        }
                        if(parent->nodeKind == DeclK && parent->subkind.decl == VarK) {
                            if(parent->varKind == Global) {
                                goffset+=parent->varSize-1;
                                treeptr->offset = goffset;
                                goffset-=treeptr->varSize;
                                parent->offset = goffset;
                                goffset-=parent->varSize;
                            }
                            else {
                                treeptr->offset = goffset;
                                goffset-=treeptr->varSize;
                            }   
                        }
                        //else if(parent->nodeKind == ExpK) {
                            treeptr->offset = goffset;
                            goffset-=treeptr->varSize;
                        //}
                    }
                    traverseSibS(treeptr, parent, table);
                    break;
                case(IdK):
                    temp = (TreeNode *)table->lookup(treeptr->attr.name);
                    if(temp) {
                        if(temp->subkind.decl == FuncK) {
                            printf("ERROR(%d): Cannot use funcion \'%s\' as a variable.\n", treeptr->line, treeptr->attr.name);
                            numErrors++;
                            break;
                        }
                        else if(parent->subkind.exp == InitK) {
                            temp->isInit = 1;
                        }
                        else if (temp->isInit == 0 && temp->initWarn == 0 && parent->child[1] == NULL) {
                            printf("WARNING(%d): Variable \'%s\' may be uninitialized when used here.\n", treeptr->line, treeptr->attr.name);
                            numWarnings++;
                            temp->initWarn = 1;
                        }
                        /*if(strcmp(treeptr->attr.name, (char *)"fpa") == 0) {
                            printf("treeptr: %s, treeptrs varKind: %d, temp: %s, temps varKind: %d\n", treeptr->attr.name, treeptr->varKind, temp->attr.name, temp->varKind);
                            treeptr->varKind = temp->varKind;
                            printf("new varkind: %d\n", treeptr->varKind);
                        }*/
                        temp->declUsed = 1;
                        treeptr->varKind = temp->varKind;
                        treeptr->varSize = temp->varSize;
                        treeptr->offset = temp->offset;
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
                        else {
                            if( treeptr->child[0]->isArray) {
                                printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            if(expType0 != Integer) {
                                printf("ERROR(%d): Unary \'%s\' requires an operand of type int but was given type %s.\n", treeptr->child[0]->line, treeptr->attr.string, treeptr->child[0]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"--") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void) {
                            //Bypass any errors
                        }
                        else {
                            if( treeptr->child[0]->isArray) {
                                printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            if(expType0 != Integer) {
                                printf("ERROR(%d): Unary \'%s\' requires an operand of type int but was given type %s.\n", treeptr->child[0]->line, treeptr->attr.string, treeptr->child[0]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"<-")  == 0) {
		        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else{
                            if(expType0 != expType1) {
                                printf("ERROR(%d): \'%s\' requires operands of the same type but lhs is type %s and rhs is type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type, treeptr->child[1]->type);
                                numErrors++;
                            }
   	                    if(treeptr->child[0]->isArray ^ treeptr->child[1]->isArray) {
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
                            else if(expType0 == expType1) {
                                temp = (TreeNode *)table->lookup(treeptr->child[0]->attr.name);
                                if(temp) {
                                    temp->isInit = 1;
                                }
                            }
                        }
                        //doesn't make sense, but below code is done to match expected output
                        /*if(treeptr->child[0]->isArray || treeptr->child[1]->isArray) {
                            treeptr->isArray = 1;
                        }*/
                        temp = NULL;
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"+=") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else {
                            if( treeptr->child[0]->isArray) {
                                printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            if(expType0 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                                numErrors++;
                            }
                            if(expType1 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"-=") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else {
                            if( treeptr->child[0]->isArray) {
                                printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            if(expType0 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                                numErrors++;
                            }
                            if(expType1 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"*=") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else {
                            if( treeptr->child[0]->isArray) {
                                printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            if(expType0 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                                numErrors++;
                            }
                            if(expType1 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    else if(strcmp(treeptr->attr.string, (char *)"/=") == 0) {
                        if( expType0 == UndefinedType || expType0 == Void || expType1 == UndefinedType) {
                            //Bypass any errors
                        }
                        else {
                            if( treeptr->child[0]->isArray) {
                                printf("Error(%d): The operation \'%s\' does not work with arrays.\n", treeptr->line, treeptr->attr.string);
                                numErrors++;
                            }
                            if(expType0 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but lhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[0]->type);
                                numErrors++;
                            }
                            if(expType1 != Integer) {
                                printf("ERROR(%d): \'%s\' requires operands of type int but rhs is of type %s.\n", treeptr->line, treeptr->attr.string, treeptr->child[1]->type);
                                numErrors++;
                            }
                        }
                        traverseChildS(treeptr, table);
                        traverseSibS(treeptr, parent, table);
                    }
                    break;
                case(InitK): /*NEED TO LOOKUP CHILDREN AND INIT THEM*/
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
                    else {
                        temp = (TreeNode *)table->lookup(treeptr->child[0]->attr.name);
                        if(temp) {
                            temp->isInit = 1;
                        }
                        temp = NULL;
                    }
                    traverseChildS(treeptr, table);
                    traverseSibS(treeptr, parent, table);
                    break;
                case(CallK):
                    paramcounter = 0;
                    temp = (TreeNode *)table->lookup(treeptr->attr.name);  /*Func Lookup Local*/
                    if(temp) {
                        temp->declUsed = 1;
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
                        else /*if(temp->isIO == 0)*/{
                            line = temp->line;
                            paramcounter = 1;
                            temp = temp->child[0];
                            temp2 = treeptr->child[0];
                            while(temp && temp2) {
                                if(temp2->subkind.exp == IdK) {
                                    temp3 = (TreeNode*)table->lookup(temp2->attr.name);
                                    isConstParm = 0;
                                }
                                else if(temp2->subkind.exp == OpK && (strcmp(temp2->attr.string, (char *)"[") == 0 || strcmp(temp2->attr.string, (char *)"sizeof") == 0)) {
                                    temp4 = (TreeNode*)table->lookup(temp2->child[0]->attr.name);
                                    temp2->child[0]->isArray = 1; //shouldn't do this, but passTypes is broken! Look at futureTips.txt
                                    isConstParm = 0;
                                }
                                else if(temp2->child[0]) {
                                    //catches function call within param of other function call
                                    static TreeNode *copy;
                                    static TreeNode *copy2;
                                    copy = temp; //retain pointer for use after
                                    copy2 = temp2;//traverseChildS call below
                                    traverseChildS(treeptr, table);
                                    temp = copy;  //gather old pointer
                                    temp2 = copy2; //gather old pointer
                                }
                                else {
                                    //Error catcher
                                    isConstParm = 1;
                                }
                                if((temp3 || temp4) && isConstParm == 0) {
                                    if(temp->expType != temp2->expType) {
                                        printf("ERROR(%d): Expecting type %s in parameter %i of call to \'%s\' declared on line %d but got type %s.\n", temp2->line, temp->type, paramcounter, treeptr->attr.name, line, temp2->type);
                                        numErrors++;
                                    }
                                    else if(temp->isIO) {
                                        //Bypass IO function calls from array errors for now
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
                                    if(temp3) {
                                        temp3->declUsed = 1;
                                        temp2->varKind = temp3->varKind;
                                        temp2->varSize = temp3->varSize;
                                        temp2->offset = temp3->offset;
                                    }
                                    else if(temp4) {
                                        temp4->declUsed = 1;
                                        temp2->child[0]->varKind = temp4->varKind;
                                        temp2->child[0]->varSize = temp4->varSize;
                                        temp2->child[0]->offset = temp4->offset;
                                        //below code passes info up to "["
                                        /*temp2->varKind = temp4->varKind;
                                        temp2->varSize = temp4->varSize;
                                        temp2->offset = temp4->offset;*/
                                    }
                                    temp3 = NULL;
                                    temp4 = NULL;
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
