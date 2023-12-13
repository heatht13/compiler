#include "main.h"

int numWarnings = 0;
int numErrors = 0;
int syntaxBool = 0;
TreeNode *tree;
TreeNode *IOroot;
TreeNode *temp = NULL;

int main(int argc, char *argv[]) {

    int c, d = 0, D = 0, p = 0, P = 0;
    /*check flags until error or input is done*/
    while ((c = ourGetopt(argc, argv, (char *)"dDhpP")) != -1) {
        switch (c) {
            case 'd':   /*yydebug flag*/
                ++d;
                break;

            case 'D':

                break;

            case 'h':
                printf("usage: -c [options] [sourcefile]\noptions:\n");
                printf("-d      - turn on parser debugging\n");
                printf("-D      - turn on symbol table debugging\n");
                printf("-h      - print this usage message\n");
                printf("-p      - print the abstract syntax tree\n");
                printf("-P      - print the abstract syntax tree plus type information\n");
                return 0;
            case 'p':   /*print tree flag*/
                ++p;
                break;

            case 'P':   /*print tree with types flag*/
                ++P;
                break;
            default:
                printf("Error: invalid option/flag used\n");
                break;
        }
    }

    if( d == 1) {       /*set yydebug*/
        yydebug = 1;
    }
    if (optind < argc) { /*while we run out of arguments*/
        if(yyin = fopen(argv[optind], "r")) {
            /*success*/
            //printf("====================================\nFILE: %s\n", argv[optind]);
        }
        else {
            printf("Error: failed to open \'%s\'\n", argv[optind]);
            exit(1);
        }
        initErrorProcessing();
        yyparse();      /*yyparse() calls yylex() to scan, then parses input*/
    }
    if (numErrors == 0) {
        if( p == 1) {   /*print AST tree with indent and sibling values of 1*/
            printTree(tree, 1, 1);
        }

        SymbolTable typeTable;
        static SymbolTable *typeTablePtr = &typeTable;

        //static SymbolTable *typeTable = (SymbolTable *)malloc(sizeof(SymbolTable));
        typeTablePtr->debug(false);
        typeTablePtr->enter("Global");

        IOroot = NULL;
        IOast(&IOroot);
        IOinsert(IOroot, typeTablePtr);

        passTypes(tree, tree, typeTablePtr);
        //typeTablePtr->print(pointerPrintStr);
        typeTablePtr = NULL;
        SymbolTable semTable;
        static SymbolTable *semTablePtr = &semTable;
        semTablePtr->debug(false);
        IOinsert(IOroot, semTablePtr);
        semanticAnalysis(tree, tree, semTablePtr);

        temp = (TreeNode *)semTablePtr->lookupGlobal((char *)"main");
        if(temp == NULL) {
            printf("ERROR(LINKER): A function named \'main\' with no parameters must be defined.\n");
            numErrors++;
        }
        else if(temp->child[0] != NULL) {
            printf("ERROR(LINKER): A function named \'main\' with no parameters must be defined.\n");
            numErrors++;
        }
        temp = NULL;
        semTablePtr->applyToAll(checkUsed);
        semTablePtr = NULL;

        if(P == 1 && numErrors == 0) {
            printTypes(tree, 1, 1);
        }
    }

    printf("Number of warnings: %d\n", numWarnings);
    printf("Number of errors: %d\n", numErrors);
    return 0;

}
