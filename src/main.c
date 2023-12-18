#include "main.h"

int numWarnings = 0;
int numErrors = 0;
int goffset = 0;
int foffset = 0;
int syntaxBool = 0;
TreeNode *tree;
TreeNode *IOroot;
TreeNode *temp = NULL;
FILE *code;
int namelen = 0;
char *outname;

int main(int argc, char *argv[]) {

    int c, d = 0, D = 0, M = 0, p = 0, P = 0;
    /*check flags until error or input is done*/
    while ((c = ourGetopt(argc, argv, (char *)"dDhMpP")) != -1) {
        switch (c) {
            case 'd':   /*yydebug flag*/
                ++d;
                break;

            case 'D':   /*symbol table debug*/
                ++D;
                break;

            case 'h':
                printf("usage: -c [options] [sourcefile]\noptions:\n");
                printf("-d      - turn on parser debugging\n");
                printf("-D      - turn on symbol table debugging\n");
                printf("-h      - print this usage message\n");
                printf("-M      - print the abstract syntax tree plus type and memory information\n");
                printf("-p      - print the abstract syntax tree\n");
                printf("-P      - print the abstract syntax tree plus type information\n");
                return 0;

            case 'M': /*print tree with types, mems, locs, and size info*/
                ++M;
                break;

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

        if(numErrors == 0) {
            if(P == 1) {
                printTypes(tree, 1, 1);
            }
            if(M == 1) {
                printMem(tree, 1, 1);
                printf("Offset for end of global space: %d\n", goffset);
            }
            if(argv[optind] != NULL) {
                namelen = strlen(argv[optind])+1;
                outname = strdup(argv[optind]);
                outname[namelen-3] = 't';
                outname[namelen-2] = 'm';
            }
            else {
                outname = (char *)"out.tm";
            }
            code = fopen(outname, "w");
            generate(semTablePtr);
            fclose(code);
                       
        }
        semTablePtr = NULL;
    }

    printf("Number of warnings: %d\n", numWarnings);
    printf("Number of errors: %d\n", numErrors);

    return 0;
}
