#include "codegen.h"
static int toffset;

void generate(SymbolTable *table) {
    toffset = 0;
    emitSkip(1);
    IOgen(IOroot);
    codeGen(tree, table);
    initGen(table);
}

void initGen(SymbolTable *table) {
    backPatchAJumpToHere(0, (char *)"jump to init [backpatch]");
    emitComment((char *)"INIT");
    emitRM((char *)"LDA", 1, goffset, 0, (char *)"set first frame at end of globals");
    emitRM((char *)"ST", 1, 0, 1,(char *)"store old frame pointer (point to self)");
    emitComment((char *)"INIT GLOBALS AND STATICS");
    globalGen(tree);
    emitComment((char *)"END INIT GLOBALS AND STATICS");
    emitRM((char *)"LDA", 3, 1, 7, (char *)"return address in ac");
    temp = (TreeNode *)table->lookupGlobal((char *)"main");
    emitGotoAbs(temp->offset, (char *)"jump to main");
    temp = NULL;
    emitRO((char *)"HALT", 0, 0, 0, (char *)"DONE!");
    emitComment((char *)"END INIT");
}

void globalGen(TreeNode *globalptr) {
    static int i;
    if(globalptr->nodeKind == DeclK && globalptr->subkind.decl == VarK && (globalptr->varKind == Global || globalptr->varKind == LocalStatic)) {
        if(globalptr->isArray) {
            emitRM((char *)"LDC", 3, globalptr->varSize-1, 6, (char *)"load into R3 the size of global array", globalptr->attr.name);
            emitRM((char *)"ST", 3, globalptr->offset+1, 0, (char *)"store into data memory the size of global array", globalptr->attr.name);
        }
        else {
            //emitRM((char *)"LDC", 3, globalptr->varSize, 6, (char *)"load into R3 the size of global variable", globalptr->attr.name);
            //emitRM((char *)"ST", 3, globalptr->offset, 1, (char *)"store into data memory the size of global variable", globalptr->attr.name);
        }
    }
    for( i=0; i<MAXCHILDREN; i++) {
        if(globalptr->child[i] != NULL) {
            globalGen(globalptr->child[i]);
        }
    }
    if(globalptr->sibling) {
        globalGen(globalptr->sibling);
    }
}

void codeGen(TreeNode *scopedNode, SymbolTable *table) {
    if(scopedNode == NULL) {
        return;
    }
    static int i;
    int toffsetCopy;
    static int parmoffset;
    int comptoffset = 0;
    int fortoffset = 0;
    static int breakLoc;
    int tempLoc1 = 0;
    int tempLoc2 = 0;
    int tempLoc3 = 0;
    static bool addStdClosing = true;
    static bool isFunc;
    static bool isArg = false;
    TreeNode *tempNode;
    switch(scopedNode->nodeKind) {
        case(DeclK):
            if(scopedNode->subkind.decl == VarK) {
                if(scopedNode->isArray) {
                    if(scopedNode->varKind == Global || scopedNode->varKind == LocalStatic) {
                        //emitRM((char *)"LDC", 3, scopedNode->varSize-1, 6, (char *)"load into R3 the size of array", scopedNode->attr.name);
                        //emitRM((char *)"ST", 3, scopedNode->offset+1, 0, (char *)"store into data memory the size of array", scopedNode->attr.name);
                    }
                    else {
                        emitRM((char *)"LDC", 3, scopedNode->varSize-1, 6, (char *)"load into R3 the size of array", scopedNode->attr.name);
                        emitRM((char *)"ST", 3, scopedNode->offset+1, 1, (char *)"store into temp the size of array", scopedNode->attr.name);
                    }
                }
                else {
                    if(scopedNode->varKind == Global || scopedNode->varKind == LocalStatic) {
                        //emitRM((char *)"LDC", 3, scopedNode->varSize, 6, (char *)"load into R3 the size of variable", scopedNode->attr.name);
                        //emitRM((char *)"ST", 3, scopedNode->offset, 0, (char *)"store into data memory the size of variable", scopedNode->attr.name);
                    }
                    else {
                        //emitRM((char *)"LDC", 3, scopedNode->varSize, 6, (char *)"load into R3 the size of variable", scopedNode->attr.name);
                        //emitRM((char *)"ST", 3, scopedNode->offset, 1, (char *)"store into data memory the size of variable", scopedNode->attr.name);
                    }
                }
            }
            else if(scopedNode->subkind.decl == FuncK) {
                emitComment((char *)"** ** ** ** ** ** ** ** ** ** ** ** **");
                emitComment((char *)"FUNCTION", (char *)scopedNode->attr.name);
                isFunc = true;
                scopedNode->offset = emitWhereAmI();
                emitComment((char *)"FUNC LOC", scopedNode->offset);
                toffset = scopedNode->varSize;
                emitComment((char *)"TOFF set:", toffset);
                emitRM((char *)"ST", 3, -1, 1, (char *)"store return address");
                codeGen(scopedNode->child[1], table);
                if(addStdClosing) {
                    emitComment((char *)"add standard closing in case there is no return statement");
                    emitRM((char *)"LDC", 2, 0, 6, (char *)"set return value to 0");
                    emitRM((char *)"LD", 3, -1, 1, (char *)"load return address");
                    emitRM((char *)"LD", 1, 0, 1, (char *)"adjust frame pointer");
                    emitGoto(0, 3, (char *)"return");
                }
                else {
                    emitRM((char *)"LD", 3, -1, 1, (char *)"load return address");
                    emitRM((char *)"LD", 1, 0, 1 ,(char *)"adjust frame pointer");
                    emitGoto(0, 3, (char *)"return");
                }
                emitComment((char *)"END FUNCTION", (char *)scopedNode->attr.name);
            }
            else if(scopedNode->subkind.decl == ParamK) {
                //DO NOTHING
            }
            travSibGen(scopedNode, table);
            break;
        case(StmtK):
            switch (scopedNode->subkind.stmt) {
                case(CompoundK):
                    comptoffset = toffset;
                    emitComment((char *)"COMPOUND");
                    if(isFunc) {
                        toffset = scopedNode->varSize;
                        emitComment((char *)"TOFF set:", toffset);
                        codeGen(scopedNode->child[0], table);
                        emitComment((char *)"Compound Body");
                        codeGen(scopedNode->child[1], table);
                        isFunc = false;
                    }
                    else {
                        toffset = toffset + scopedNode->varSize;
                        emitComment((char *)"TOFF set:", toffset);
                        codeGen(scopedNode->child[0], table);
                        emitComment((char *)"Compound Body");
                        emitComment((char *)"TOFF set:", toffset);
                        codeGen(scopedNode->child[1], table);
                    }
                    travSibGen(scopedNode, table);
                    toffset = comptoffset;
                    emitComment((char *)"TOFF set:", toffset);
                    emitComment((char *)"END COMPOUND");
                    break;
                case(IfK):
                    emitComment((char *)"IF");
                    codeGen(scopedNode->child[0], table);
                    emitComment((char *)"THEN");
                    tempLoc1 = emitSkip(1);
                    codeGen(scopedNode->child[1], table);
                    tempLoc2 = emitSkip(1);
                    backPatchAJumpToHere((char *)"JZR", 3, tempLoc1, (char *)"if false, jump past the THEN part [backpatch]");
                    if(scopedNode->child[2]) {
                        emitComment((char *)"ELSE");
                        codeGen(scopedNode->child[2], table);
                    }
                    backPatchAJumpToHere(tempLoc2, (char *)"if true, jump past the ELSE part [backpatch]");
                    emitComment((char *)"END IF");
                    break;
               /* case(ForK):
                    emitComment((char *)"FOR");
                    codeGen(scopedNode->child[1], table);
                    tempLoc1 = emitWhereAmI();
                    emitRM((char *)"LD", 3, toffset+1, 1, (char *)"load incrementer value into R3");
                    emitRM((char *)"LD", 5, toffset+2, 1, (char *)"load end of range value into R5");
                    emitRM((char *)"LD", 4, toffset+3, 1, (char *)"load start of range value (index) into R4");
                    emitRO((char *)"SLT", 3, 4, 5, (char *)"op <");
                    emitRM((char *)"JNZ", 3, 1, 7, (char *)"jump to loop body");
                    tempLoc2 = emitSkip(1);
                    emitComment((char *)"DO");
                    codeGen(scopedNode->child[2], table);
                    emitRM((char *)"LD", 3, toffset+3, 1, (char *)"load start of range value (index) into R3");
                    emitRM((char *)"LD", 5, toffset+1, 1, (char *)"load incrementer value into R5");
                    emitRO((char *)"ADD", 3, 3, 5, (char *)"increment index value");
                    emitRM((char *)"ST", 3, toffset+3, 1, (char *)"store new index value into the start of range (index) location");
                    emitGotoAbs(tempLoc1, (char *)"go to beginning of loop");
                    backPatchAJumpToHere(tempLoc2, (char *)"jump past loop [backpatch]");
                    emitComment((char *)"END FOR");
                    break;*/
                case(WhileK):
                    emitComment((char *)"WHILE");
                    tempLoc1 = emitWhereAmI();
                    codeGen(scopedNode->child[0], table);
                    tempLoc2 = emitSkip(1);
                    emitComment((char *)"DO");
                    codeGen(scopedNode->child[1], table);
                    emitGotoAbs(tempLoc1,(char *)"go back to test condition");
                    breakLoc = emitWhereAmI();
                    backPatchAJumpToHere((char *)"JZR", 3, tempLoc2, (char *)"if false, jump out of loop [backpatch]");
                    emitComment((char *)"END WHILE");
                    break;
                /*case(BreakK):
                    emitComment((char *)"BREAK");
                    emitGotoAbs(breakLoc, (char *)"jump out of loop");
                    emitComment((char *)"END BREAK");
                    break;*/
                case(ReturnK):
                    emitComment((char *)"RETURN");
                    addStdClosing = false;
                    if(scopedNode->child[0]) {
                        codeGen(scopedNode->child[0], table);
                        emitRM((char *)"LDA", 2, 0, 3, (char *)"load into return register R2 the return value in R3");
                    }
                    emitRM((char *)"LD", 3, -1, 1, (char *)"load return address");
                    emitRM((char *)"LD", 1, 0, 1, (char *)"adjust frame pointer");
                    emitGoto(0, 3, (char *)"return");
                    addStdClosing = true;
                    emitComment((char *)"END RETURN");
                    break;
                /*case(RangeK):
                    emitComment((char *)"RANGE");
                    codeGen(scopedNode->child[0], table);
                    emitRM((char *)"ST", 3, toffset, 1, (char *)"store start of range into temp");
                    emitComment((char *)"TOFF dec", --toffset);
                    codeGen(scopedNode->child[1], table);
                    emitRM((char *)"ST", 3, toffset, 1, (char *)"store end of of range into temp");
                    emitComment((char *)"TOFF dec", --toffset);
                    if(scopedNode->child[2]) {
                        codeGen(scopedNode->child[2], table);
                        emitRM((char *)"ST", 3, toffset, 1, (char *)"store incrememter into temp");
                    }
                    else {
                        emitRM((char *)"LDC", 3, 1, 6, (char *)"load incrementer value of 1 into R3");
                        emitRM((char *)"ST", 3, toffset, 1, (char *)"store 1 as incrementer into temp");
                    }
                    emitComment((char *)"TOFF dec", --toffset);
                    emitComment((char *)"END RANGE");
                    break;*/
                case(NullK):
                    emitComment((char *)"NULL");
                    emitComment((char *)"END NULL");
                    break;
                default:
                    break;
            }
            travSibGen(scopedNode, table);
            break;
        case(ExpK):
            switch(scopedNode->subkind.exp) {
                case(OpK):
                    if(strcmp(scopedNode->attr.string, (char *)"=") == 0) {
                        codeGen(scopedNode->child[0], table);
                        emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                        emitComment((char *)"TOFF dec:", --toffset);
                        codeGen(scopedNode->child[1], table);
                        emitComment((char *)"TOFF inc:", ++toffset);
                        emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                        emitRO((char *)"TEQ", 3, 4, 3, (char *)"test if R4 and R3 are equal, store outcome into R3");
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"!=") == 0) {
                        codeGen(scopedNode->child[0], table);
                        emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                        emitComment((char *)"TOFF dec:", --toffset);
                        codeGen(scopedNode->child[1], table);
                        emitComment((char *)"TOFF inc:", ++toffset);
                        emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                        emitRO((char *)"TNE", 3, 4, 3, (char *)"test if R4 and R3 are not equal , store outcome into R3");
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"<") == 0) {
                        codeGen(scopedNode->child[0], table);
                        emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                        emitComment((char *)"TOFF dec:", --toffset);
                        codeGen(scopedNode->child[1], table);
                        emitComment((char *)"TOFF inc:", ++toffset);
                        emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                        emitRO((char *)"TLT", 3, 4, 3, (char *)"test if R4 is less than R3, store outcome into R3");
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"<=") == 0) {
                        codeGen(scopedNode->child[0], table);
                        emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                        emitComment((char *)"TOFF dec:", --toffset);
                        codeGen(scopedNode->child[1], table);
                        emitComment((char *)"TOFF inc:", ++toffset);
                        emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                        emitRO((char *)"TLE", 3, 4, 3, (char *)"test if R4 is less than or equal to R3, store outcome into R3");
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)">") == 0) {
                        codeGen(scopedNode->child[0], table);
                        emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                        emitComment((char *)"TOFF dec:", --toffset);
                        codeGen(scopedNode->child[1], table);
                        emitComment((char *)"TOFF inc:", ++toffset);
                        emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                        emitRO((char *)"TGT", 3, 4, 3, (char *)"test if R4 is greater than or equal to R3, store outcome into R3");
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)">=") == 0) {
                        codeGen(scopedNode->child[0], table);
                        emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                        emitComment((char *)"TOFF dec:", --toffset);
                        codeGen(scopedNode->child[1], table);
                        emitComment((char *)"TOFF inc:", ++toffset);
                        emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                        emitRO((char *)"TGE", 3, 4, 3, (char *)"Op >=");
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"+") == 0) {
                        codeGen(scopedNode->child[0], table);
                        emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                        emitComment((char *)"TOFF dec:", --toffset);
                        codeGen(scopedNode->child[1], table);
                        emitComment((char *)"TOFF inc:", ++toffset);
                        emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                        emitRO((char *)"ADD", 3, 4, 3, (char *)"add the values of R4 and R3, store outcome into R3");
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"-") == 0) {
                        codeGen(scopedNode->child[0], table);
                        emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                        emitComment((char *)"TOFF dec:", --toffset);
                        codeGen(scopedNode->child[1], table);
                        emitComment((char *)"TOFF inc:", ++toffset);
                        emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                        emitRO((char *)"SUB", 3, 4, 3, (char *)"subtract from the value of R4 the value of R3, store outcome into R3");
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"*") == 0) {
                        codeGen(scopedNode->child[0], table);
                        emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                        emitComment((char *)"TOFF dec:", --toffset);
                        codeGen(scopedNode->child[1], table);
                        emitComment((char *)"TOFF inc:", ++toffset);
                        emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                        emitRO((char *)"MUL", 3, 4, 3, (char *)"multiply the values of R4 and R3, store outcome into R3");
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"/") == 0) {
                        codeGen(scopedNode->child[0], table);
                        emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                        emitComment((char *)"TOFF dec:", --toffset);
                        codeGen(scopedNode->child[1], table);
                        emitComment((char *)"TOFF inc:", ++toffset);
                        emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                        emitRO((char *)"DIV", 3, 4, 3, (char *)"divide from the value of R4 the value of R3, store outcome into R3");
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"%") == 0) {
                        codeGen(scopedNode->child[0], table);
                        emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                        emitComment((char *)"TOFF dec:", --toffset);
                        codeGen(scopedNode->child[1], table);
                        emitComment((char *)"TOFF inc:", ++toffset);
                        emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                        emitRO((char *)"MOD", 3, 4, 3, (char *)"mod from the value of R4 the value of R3, store outcome into R3");
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"[") == 0) {
                        codeGen(scopedNode->child[0], table);
                        emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                        emitComment((char *)"TOFF dec:", --toffset);
                        codeGen(scopedNode->child[1], table);
                        emitComment((char *)"TOFF inc:", ++toffset);
                        emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                        emitRO((char *)"SUB", 3, 4, 3, (char *)"compute the memory location of array element");
                        emitRM((char *)"LD", 3, 0, 3, (char*)"load array element's value into R3");
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"sizeof") == 0) {
                        emitRM((char *)"LDC", 3, scopedNode->child[0]->varSize-1, 3, (char *)"find the size of the value in R3, store into R3");
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"?") == 0) {
                        codeGen(scopedNode->child[0], table);
                        emitRO((char *)"RND", 3, 3, 3, (char *)"find a random value from a range of 0 to the value of R3, store into R3");
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"chsign") == 0) {
                        codeGen(scopedNode->child[0], table);
                        emitRO((char *)"NEG", 3, 3, 3, (char *)"change the sign of the value of R3, store into R3");
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"not") == 0) {
                        codeGen(scopedNode->child[0], table);
                        emitRM((char *)"LDC", 4, 1, 6, (char *)"load into R4 the value 1");
                        emitRO((char *)"XOR", 3, 3, 4, (char *)"perform the XOR op on the value of  R3, store into R3");
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"and") == 0) {
                        codeGen(scopedNode->child[0], table);
                        emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                        emitComment((char *)"TOFF dec:", --toffset);
                        codeGen(scopedNode->child[1], table);
                        emitComment((char *)"TOFF inc:", ++toffset);
                        emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                        emitRO((char *)"AND", 3, 4, 3, (char *)"perform AND op on the values of R4 and R3, store into R3");
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"or") == 0) {
                        codeGen(scopedNode->child[0], table);
                        emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                        emitComment((char *)"TOFF dec:", --toffset);
                        codeGen(scopedNode->child[1], table);
                        emitComment((char *)"TOFF inc:", ++toffset);
                        emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                        emitRO((char *)"OR", 3, 4, 3, (char *)"perform the OR op on the values of R4 and R3, store into R3");
                    }
                    if(!isArg) {
                        travSibGen(scopedNode, table);
                    }
                    break;
                case(IdK):
                    if(scopedNode->isArray) {
                        if(scopedNode->varKind == Global || scopedNode->varKind == LocalStatic) {
                            emitRM((char *)"LDA", 3, scopedNode->offset, 0, (char *)"load memory location of start of global array into R3");
                        }
                        else {
                            emitRM((char *)"LDA", 3, scopedNode->offset, 1, (char *)"load memory location of start of local array into R3");
                        }
                    }
                    else {
                        if(scopedNode->varKind == Global || scopedNode->varKind == LocalStatic) {
                            emitRM((char *)"LD", 3, scopedNode->offset, 0, (char *)"load value of global variable into R3");
                        }
                        else {
                            emitRM((char *)"LD", 3, scopedNode->offset, 1, (char *)"load value of local variable into R3");
                        }
                    }
                    if(!isArg) {
                        travSibGen(scopedNode, table);
                    }
                    break;
                case(ConstantK):
                    if(scopedNode->expType == Integer) {
                        emitRM((char *)"LDC", 3, atoi(scopedNode->attr.string),  6, (char *)"load integer constant into R3");
                    }
                    else if(scopedNode->expType == Char) {
                        //if(scopedNode->isArray) {
                            //DONT KNOW YET 
                            //emitRM((char *)"LD", 3, scopedNode->offset, 0, (char *)"load char array's memory location into R3");
                        //}
                        //else {
                            emitRM((char *)"LDC", 3, scopedNode->attr.string[1],  6, (char *)"load char constant into R3");
                       // }
                    }
                    else if(scopedNode->expType == Boolean) {
                        if(strcmp(scopedNode->attr.string, (char *)"true") == 0) {
                            emitRM((char *)"LDC", 3, 1,  6, (char *)"load boolean constant of truth value TRUE into R3");
                        }
                        else {
                            emitRM((char *)"LDC", 3, 0,  6, (char *)"load boolean constant of truth value FALSE into R3");
                        }
                    }
                    if(!isArg) {
                        travSibGen(scopedNode, table);
                    }
                    break;
                case(AssignK):
                    if(strcmp(scopedNode->attr.string, (char *)"++") == 0) {
                        if(strcmp(scopedNode->child[0]->attr.string, (char *)"[") == 0) {
                            temp = scopedNode->child[0];
                            codeGen(temp->child[0], table);
                            emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                            emitComment((char *)"TOFF dec:", --toffset);
                            codeGen(temp->child[1], table);
                            emitComment((char  *)"TOFF inc:", ++toffset);
                            emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                            emitRO((char *)"SUB", 5, 4, 3, (char *)"store the offset of indexed array element into R5");
                            codeGen(scopedNode->child[0], table);
                            emitRM((char *)"LDA", 3, 1, 3, (char *)"increment value of R3 by 1");
                            emitRM((char *)"ST", 3, 0, 5, (char *)"store incremented value into array");
                            temp = NULL;
                        }
                        else {
                            codeGen(scopedNode->child[0], table);
                            emitRM((char *)"LDA",3,1,3,(char *)"increment value by one");
                            if(scopedNode->child[0]->varKind == Global || scopedNode->child[0]->varKind == LocalStatic) {
                                emitRM((char *)"ST", 3, scopedNode->child[0]->offset, 0, (char *)"store into global data mem the value of R3");
                            }
                            else {
                                emitRM((char *)"ST", 3, scopedNode->child[0]->offset, 1, (char *)"store into local data mem the value of R3");
                            }
                        }
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"--") == 0) {
                        if(strcmp(scopedNode->child[0]->attr.string, (char *)"[") == 0) {
                            temp = scopedNode->child[0];
                            codeGen(temp->child[0], table);
                            emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                            emitComment((char *)"TOFF dec:", --toffset);
                            codeGen(temp->child[1], table);
                            emitComment((char  *)"TOFF inc:", ++toffset);
                            emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                            emitRO((char *)"SUB", 5, 4, 3, (char *)"store the offset of indexed array element into R5");
                            codeGen(scopedNode->child[0], table);
                            emitRM((char *)"LDA", 3, -1, 3, (char *)"decrement value by 1");
                            emitRM((char *)"ST", 3, 0, 5, (char *)"store decremented value into array");
                            temp = NULL;
                        }
                        else {
                            codeGen(scopedNode->child[0], table);
                            emitRM((char *)"LDA",3,-1,3,(char *)"decrement value by one");
                            if(scopedNode->child[0]->varKind == Global || scopedNode->child[0]->varKind == LocalStatic) {
                                 emitRM((char *)"ST", 3, scopedNode->child[0]->offset, 0, (char *)"store into global data mem the value of R3");
                            }
                            else {
                                 emitRM((char *)"ST", 3, scopedNode->child[0]->offset, 1, (char *)"store into local data mem the value of R3");
                            }
                        }
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"<-")  == 0) {
                        if(strcmp(scopedNode->child[0]->attr.string, (char *)"[") == 0) {             
                            temp = scopedNode->child[0];
                            codeGen(temp->child[0], table);
                            emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                            emitComment((char *)"TOFF dec:", --toffset);
                            codeGen(temp->child[1], table);
                            emitComment((char *)"TOFF inc:", ++toffset);
                            emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                            emitRO((char *)"SUB", 5, 4, 3, (char *)"store the offset of indexed array element into R5");
                            codeGen(scopedNode->child[1], table);
                            emitRM((char *)"ST", 3, 0, 5, (char *)"store value of R3 into array");
                            temp = NULL;
                        }
                        else {
                            codeGen(scopedNode->child[1], table);
                            if(scopedNode->child[0]->varKind == Global || scopedNode->child[0]->varKind == LocalStatic) {
                                emitRM((char *)"ST", 3, scopedNode->child[0]->offset, 0, (char *)"store into global data mem the value of R3");
                            }
                            else {
                                emitRM((char *)"ST", 3, scopedNode->child[0]->offset, 1, (char *)"store into local data mem the value of R3");
                            }
                        }
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"+=") == 0) {
                        if(strcmp(scopedNode->child[0]->attr.string, (char *)"[") == 0) {
                            temp = scopedNode->child[0];
                            codeGen(temp->child[0], table);
                            emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                            emitComment((char *)"TOFF dec:", --toffset);
                            codeGen(temp->child[1], table);
                            emitComment((char  *)"TOFF inc:", ++toffset);
                            emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                            emitRO((char *)"SUB", 5, 4, 3, (char *)"store the offset of indexed array element into R5");
                            codeGen(scopedNode->child[0], table);
                            emitRM((char *)"LDA", 4, 0, 3, (char *)"store into R4 the value of R3");
                            codeGen(scopedNode->child[1], table);
                            emitRO((char *)"ADD", 3, 4, 3, (char *)"add the value of R3 to the value of R4, store outcome into R3");
                            emitRM((char *)"ST", 3, 0, 5, (char *)"store value of R3 into array");
                            temp = NULL;
                        }
                        else {
                            codeGen(scopedNode->child[0], table);
                            emitRM((char *)"LDA", 4, 0, 3, (char *)"store into R4 the value of R3");
                            codeGen(scopedNode->child[1], table);
                            emitRO((char *)"ADD", 3, 4, 3, (char *)"add the value of R3 to the value of R4, store outcome into R3");
                            if(scopedNode->child[0]->varKind == Global || scopedNode->child[0]->varKind == LocalStatic) {
                                emitRM((char *)"ST", 3, scopedNode->child[0]->offset, 0, (char *)"store into global data mem the value of R3");
                            }
                            else {
                                emitRM((char *)"ST", 3, scopedNode->child[0]->offset, 1, (char *)"store into local data mem the value of R3");
                            }
                        }
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"-=") == 0) {
                        if(strcmp(scopedNode->child[0]->attr.string, (char *)"[") == 0) {
                            temp = scopedNode->child[0];
                            codeGen(temp->child[0], table);
                            emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                            emitComment((char *)"TOFF dec:", --toffset);
                            codeGen(temp->child[1], table);
                            emitComment((char  *)"TOFF inc:", ++toffset);
                            emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                            emitRO((char *)"SUB", 5, 4, 3, (char *)"store the offset of indexed array element into R5");       
                            codeGen(scopedNode->child[0], table);
                            emitRM((char *)"LDA", 4, 0, 3, (char *)"store into R4 the value of R3");
                            codeGen(scopedNode->child[1], table);
                            emitRO((char *)"SUB", 3, 4, 3, (char *)"subtract the value of R3 from the value of R4, store outcome into R3");
                            emitRM((char *)"ST", 3, 0, 5, (char *)"store value of R3 into array");
                            temp = NULL;
                        }
                        else {
                            codeGen(scopedNode->child[0], table);
                            emitRM((char *)"LDA", 4, 0, 3, (char *)"store into R4 the value of R3");
                            codeGen(scopedNode->child[1], table);
                            emitRO((char *)"SUB", 3, 4, 3, (char *)"subtract the value of R3 from the value of R4, store outcome into R3");
                            if(scopedNode->child[0]->varKind == Global || scopedNode->child[0]->varKind == LocalStatic) {
                                emitRM((char *)"ST", 3, scopedNode->child[0]->offset, 0, (char *)"store into global data mem the value of R3");
                            }
                            else {
                                emitRM((char *)"ST", 3, scopedNode->child[0]->offset, 1, (char *)"store into local data mem the value of R3");
                            }
                        }
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"*=") == 0) {
                        if(strcmp(scopedNode->child[0]->attr.string, (char *)"[") == 0) {
                            temp = scopedNode->child[0];
                            codeGen(temp->child[0], table);
                            emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                            emitComment((char *)"TOFF dec:", --toffset);
                            codeGen(temp->child[1], table);
                            emitComment((char  *)"TOFF inc:", ++toffset);
                            emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                            emitRO((char *)"SUB", 5, 4, 3, (char *)"store the offset of indexed array element into R5");
                            codeGen(scopedNode->child[0], table);
                            emitRM((char *)"LDA", 4, 0, 3, (char *)"store into R4 the value of R3");
                            codeGen(scopedNode->child[1], table);
                            emitRO((char *)"MUL", 3, 4, 3, (char *)"multiply the values of R4 and R3, store outcome into R3");
                            emitRM((char *)"ST", 3, 0, 5, (char *)"store the value of R3 into array");
                            temp = NULL;
                        }
                        else {
                            codeGen(scopedNode->child[0], table);
                            emitRM((char *)"LDA", 4, 0, 3, (char *)"store into R4 the value of R3");
                            codeGen(scopedNode->child[1], table);
                            emitRO((char *)"MUL", 3, 4, 3, (char *)"multiply the values of R4 and R3, store into R3");
                            if(scopedNode->child[0]->varKind == Global || scopedNode->child[0]->varKind == LocalStatic) {
                                emitRM((char *)"ST", 3, scopedNode->child[0]->offset, 0, (char *)"store into global data mem the value of R3");
                            }
                            else {
                                emitRM((char *)"ST", 3, scopedNode->child[0]->offset, 1, (char *)"store into local data mem the value of R3");
                            }
                        }
                    }
                    else if(strcmp(scopedNode->attr.string, (char *)"/=") == 0) {
                        if(strcmp(scopedNode->child[0]->attr.string, (char *)"[") == 0) {
                            temp = scopedNode->child[0];
                            codeGen(temp->child[0], table);
                            emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the value of R3");
                            emitComment((char *)"TOFF dec:", --toffset);
                            codeGen(temp->child[1], table);
                            emitComment((char  *)"TOFF inc:", ++toffset);
                            emitRM((char *)"LD", 4, toffset, 1, (char *)"load temp value into R4");
                            emitRO((char *)"SUB", 5, 4, 3, (char *)"store the offset of indexed array element into R5");
                            codeGen(scopedNode->child[0], table);
                            emitRM((char *)"LDA", 4, 0, 3, (char *)"store the value of R3 into R4");
                            codeGen(scopedNode->child[1], table);
                            emitRO((char *)"DIV", 3, 4, 3, (char *)"divide the value of R3 from the value of R4, store outcome into R3");
                            emitRM((char *)"ST", 3, 0, 5, (char *)"store the value of R3 into array");
                            temp = NULL;
                        }
                        else {
                            codeGen(scopedNode->child[0], table);
                            emitRM((char *)"LDA", 4, 0, 3, (char *)"store into R4 the value of R3");
                            codeGen(scopedNode->child[1], table);
                            emitRO((char *)"DIV", 3, 4, 3, (char *)"divide the value of R3 from the value of R4, store outcome into R3");
                            if(scopedNode->child[0]->varKind == Global || scopedNode->child[0]->varKind == LocalStatic) {
                                emitRM((char *)"ST", 3, scopedNode->child[0]->offset, 0, (char *)"store into global data mem the value of R3");
                            }
                            else {
                                emitRM((char *)"ST", 3, scopedNode->child[0]->offset, 1, (char *)"store into local data mem the value of R3");
                            }
                        }
                    }
                    if(!isArg) {
                        travSibGen(scopedNode, table);
                    }
                    break;
                case(CallK):
                    emitComment((char *)"CALL", (char *)scopedNode->attr.name);
                    emitRM((char *)"ST", 1, toffset, 1, (char *)"store frame pointer in ghost frame for ", scopedNode->attr.name);
                    tempNode = scopedNode->child[0];
                    toffsetCopy = toffset;
                    emitComment((char *)"TOFF dec:", --toffset);
                    emitComment((char *)"TOFF dec:", --toffset);
                    isArg = true;
                    while(tempNode) {
                        emitComment((char *)"Param");
                        codeGen(tempNode, table);
                        emitRM((char *)"ST", 3, toffset, 1, (char *)"store into temp the parameter value of R3");
                        emitComment((char *)"TOFF dec:", --toffset);
                        tempNode = tempNode->sibling;
                    }
                    isArg = false; 
                    tempNode = NULL;
                    toffset = toffsetCopy;
                    emitComment((char *)"Param end", (char *)scopedNode->attr.name);
                    emitRM((char *)"LDA", 1, toffset, 1, (char *)"ghost frame becomes new active frame");
                    emitRM((char *)"LDA", 3, 1, 7, (char *)"return address in ac");
                    temp = (TreeNode *)table->lookupGlobal(scopedNode->attr.name);
                    emitGotoAbs(temp->offset, (char *)"CALL", temp->attr.name);
                    temp = NULL;
                    emitRM((char *)"LDA", 3, 0, 2, (char *)"store into R3 the return value in R2");
                    emitComment((char *)"Call end", (char *)scopedNode->attr.name);
                    emitComment((char *)"TOFF set:", toffset);
                    if(!isArg) {
                        travSibGen(scopedNode, table);
                    }
                    break;
                case(InitK):
                    //DO NOTHING
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void travChildGen(TreeNode *treeptr, SymbolTable *table) {
    for( int i=0; i<MAXCHILDREN; i++) {
        if(treeptr->child[i]) {
            codeGen(treeptr->child[i], table);
        }
    }
}

void travSibGen(TreeNode *treeptr, SymbolTable *table) {
    if(treeptr->sibling != NULL) {
        codeGen(treeptr->sibling, table);
    }
}

void IOgen(TreeNode *IOtree) {
    temp = IOtree;
    while(temp) {
        temp->offset = emitWhereAmI();
        emitComment((char *)"** ** ** ** ** ** ** ** ** ** ** ** **");
        emitComment((char *)"FUNCTION", temp->attr.name);
        emitComment((char *)"FUNC LOC", temp->offset);
        emitRM((char *)"ST", 3, -1, 1, (char *)"store return address");
        if(strcmp(temp->attr.name, (char *)"input") == 0) {
            emitRO((char *)"IN", 2, 2, 2, (char *)"get int input");
        }
        else if(strcmp(temp->attr.name, (char *)"inputb") == 0) {
            emitRO((char *)"INB", 2, 2, 2, (char *)"get bool input");
        }
        else if(strcmp(temp->attr.name, (char *)"inputc") == 0) {
            emitRO((char *)"INC", 2, 2, 2, (char *)"get char input");
        }
        else if(strcmp(temp->attr.name, (char *)"output") == 0) {
            emitRM((char *)"LD", 3, -2, 1, (char *)"load parameter");
            emitRO((char *)"OUT", 3, 3, 3, (char *)"output int");
        }
        else if(strcmp(temp->attr.name, (char *)"outputb") == 0) {
            emitRM((char *)"LD", 3, -2, 1, (char *)"load parameter");
            emitRO((char *)"OUTB", 3, 3, 3, (char *)"output bool");
        }
        else if(strcmp(temp->attr.name, (char *)"outputc") == 0) {
            emitRM((char *)"LD", 3, -2, 1, (char *)"load parameter");
            emitRO((char *)"OUTC", 3, 3, 3, (char *)"output char");
        }
        else if(strcmp(temp->attr.name, (char *)"outnl") == 0) {
            emitRO((char *)"OUTNL", 3, 3, 3, (char *)"output newline");
        }
        emitRM((char *)"LD", 3, -1, 1, (char *)"load return address");
        emitRM((char *)"LD", 1, 0, 1 ,(char *)"adjust fp");
        emitGoto(0, 3, (char *)"return");
        emitComment((char *)"END FUNCTION", temp->attr.name);
        emitComment((char *)"");

        temp = temp->sibling;
    }
}
