%{
#include "scanType.h"
#include <stdio.h>

extern int yylex();
extern FILE *yyin;
extern int line;
extern int errors;

#define YYERROR_VERBOSE
void yyerror(const char *msg) {
    printf("ERROR(%d): %s\n", line, msg);
    errors++;
}
%}

%union {
    TokenData *tokenData;
    double value;
}

%token <tokenData> BOOLCONST NUMCONST ID CHARCONST STRINGCONST WORDOP OTHER
%type <value> token

%%
list  : token list 
      | token
      ;

token : BOOLCONST	{ printf("Line %d Token: %s Value: %d Input: %s\n", $1->linenum, $1->tokenclass, $1->nvalue, $1->tokenstr); }
      | NUMCONST	{ printf("Line %d Token: %s Value: %d Input: %s\n", $1->linenum, $1->tokenclass, $1->nvalue, $1->tokenstr); }
      | ID		{ printf("Line %d Token: %s Value: %s\n", $1->linenum, $1->tokenclass, $1->tokenstr); }
      | CHARCONST       { printf("Line %d Token: %s Value: \'%c\' Input: %s\n", $1->linenum, $1->tokenclass, $1->cvalue, $1->tokenstr); }
      | STRINGCONST	{ printf("Line %d Token: %s Value: %s Len: %d Input: %s\n", $1->linenum, $1->tokenclass, $1->modified, $1->nvalue, $1->tokenstr); }
      | WORDOP		{ printf("Line %d Token: %s\n", $1->linenum, $1->modified); }
      | OTHER		{ printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }	
      ;

%%
extern int yybug;
int main(int argc, char *argv[]) {
    if (argc > 1) {
        if(yyin = fopen(argv[1], "r")) {
	    //success
        }
        else {
            printf("Error: failed to open \'%s\'\n", argv[1]);
            exit(1);
	}
    }

    errors = 0;
    yyparse();		//call yylex() and scan and parse input

    return 0;

}

