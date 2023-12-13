%{

#include "treeDef.h"
#include "scanType.h"
#include "astUtils.h"
#include "ourgetopt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
extern int yyparse();
extern int yydebug;
extern FILE *yyin;
extern int line;
extern int errors;
static TreeNode *tree;


#define YYERROR_VERBOSE
void yyerror(const char *msg) {
    printf("ERROR(%d): %s\n", line, msg);
    errors++;
}

TreeNode * giveSibling( TreeNode *siblingList, TreeNode *sibling) {

    TreeNode *ptr = siblingList;

    if (ptr != NULL) {
        while (ptr->sibling != NULL) {
            ptr = ptr->sibling;
        }
        ptr->sibling = sibling;
        ptr = siblingList;
    }
    else {
        ptr = sibling;
    }
    return ptr;
}

%}

%union {
    TokenData *tokenData;	//terminals
    ExpType type;		//passing types
    TreeNode *treeptr;		//non-terminals
}
	/*lexer tokens*/
%token <tokenData> NUMCONST ID CHARCONST STRINGCONST BOOLCONST
%token <tokenData> AND OR NOT begin END IF THEN ELSE WHILE DO FOR TO BY
%token <tokenData> BREAK RETURN INT CHAR BOOL STATIC TRUE FALSE
%token <tokenData> ASGN ADDASGN DIFFSGN MULSGN DIVSGN INC DEC
%token <tokenData> LPAREN RPAREN COLON SEMI COMMA QUES LBRAC RBRAC
%token <tokenData> EQ NEQ LEQ GEQ LESS GREAT CROSS DASH ASTER DIV MOD
%type <tokenData> assignop relop sumop mulop unaryop
%type <tokenData> typeSpec
%type <treeptr> program declList decl varDecl scopedVarDecl varDeclList varDeclInit
%type <treeptr> varDeclId funDecl parms parmList parmTypeList parmIdList parmId
%type <treeptr> stmt full unfull fullSelectStmt unfullSelectStmt fullIterStmt
%type <treeptr> unfullIterStmt expStmt compoundStmt localDecls stmtList iterRange
%type <treeptr> returnStmt breakStmt exp simpleExp andExp unaryRelExp relExp sumExp
%type <treeptr> mulExp unaryExp factor mutable immutable call args argList constant
%%

program          : declList
			{ tree = $1;}
                 ;
declList         : declList decl
			{ $$ = giveSibling($1, $2);}
                 | decl
			{ $$ = $1;}
                 ;
decl             : varDecl
			{ $$ = $1;}
                 | funDecl
			{ $$ = $1;}                 
                 ;
varDecl          : typeSpec varDeclList SEMI
			{
			  TreeNode *ptr = $2;
			  while(ptr) {
			      ptr->expType = (ExpType)$1->nvalue;
			      ptr->type = strdup($1->tokenstr);
			      ptr = ptr->sibling;
			  }
			  $$ = $2;
			}
                 ;
scopedVarDecl    : STATIC typeSpec varDeclList SEMI
			{
			  TreeNode *ptr = $3;
			  while(ptr) {
			      ptr->expType = (ExpType)$2->nvalue;
			      ptr->type = strdup($2->tokenstr);
			      ptr->isStatic = 1;
			      ptr = ptr->sibling;
			  }
			  $$ = $3; 
			}
                 | typeSpec varDeclList SEMI
			{
			  TreeNode *ptr = $2;
			  while(ptr) {
			      ptr->expType = (ExpType)$1->nvalue;
			      ptr->type = strdup($1->tokenstr);
			      ptr = ptr->sibling;
                          }
			  $$ = $2;
			}
                 ;
varDeclList      : varDeclList COMMA varDeclInit
			{ $$ = giveSibling($1, $3);}
                 | varDeclInit
			{ $$ = $1;}
                 ;
varDeclInit      : varDeclId
			{ $$ = $1;}
                 | varDeclId COLON simpleExp
			{ $$ = $1;
			  $$->child[0] = $3;
			}
                 ;
varDeclId        : ID
			{ $$ = newDeclNode(VarK, UndefinedType, $1);}
                 | ID LBRAC NUMCONST RBRAC
			{
			  $$ = newDeclNode(VarK, UndefinedType, $1);
			  $$->arraySize = $3->nvalue;
			  $$->isArray = 1;
			}
                 ;
typeSpec         : BOOL
			{
			  $$->nvalue = (ExpType)Boolean;
			  $$->tokenstr = "bool";}
                 | CHAR
			{
			  $$->nvalue = (ExpType)Char;
			  $$->tokenstr = "char";}
                 | INT
			{
			  $$->nvalue = (ExpType)Integer;
			  $$->tokenstr = "int";}
                 ;
funDecl          : typeSpec ID LPAREN parms RPAREN compoundStmt
			{ 
			  $$ = newDeclNode(FuncK, (ExpType)$1->nvalue, $2);
			  $$->type = strdup($1->tokenstr);
			  $$->child[0] = $4;
			  $$->child[1] = $6;
			}
                 | ID LPAREN parms RPAREN compoundStmt
			{
			  $$ = newDeclNode(FuncK, Void, $1);
                          $$->type = "void";
			  $$->child[0] = $3;
			  $$->child[1] = $5;
			}
                 ;
parms            : parmList
			{ $$ = $1;}
                 |
			{ $$ = NULL;}
                 ;
parmList         : parmList SEMI parmTypeList
			{ $$ = giveSibling($1, $3);}
                 | parmTypeList
			{ $$ = $1;}
                 ;
parmTypeList     : typeSpec parmIdList
			{
                          TreeNode *ptr = $2;
                          while(ptr) {
                              ptr->expType = (ExpType)$1->nvalue;
                              ptr->type = strdup($1->tokenstr);
                              ptr = ptr->sibling;
                          }
                          $$ = $2;
			}
                 ;
parmIdList       : parmIdList COMMA parmId
			{ $$ = giveSibling($1, $3);}
                 | parmId
			{ $$ = $1;}
                 ;
parmId           : ID
			{ $$ = newDeclNode(ParamK, UndefinedType, $1);}
                 | ID LBRAC RBRAC
			{
			  $$ = newDeclNode(ParamK, UndefinedType, $1);
			  $$->isArray = 1;
			}
                 ;
stmt             : full
			{ $$ = $1;}
                 | unfull
			{ $$ = $1;}
                 ;
full             : expStmt
			{ $$ = $1;}
                 | compoundStmt
			{ $$ = $1;}
                 | returnStmt
			{ $$ = $1;}
                 | breakStmt
			{ $$ = $1;}
                 | fullSelectStmt
			{ $$ = $1;}
                 | fullIterStmt
			{ $$ = $1;}
                 ;
unfull           : unfullSelectStmt
			{ $$ = $1;}
                 | unfullIterStmt
			{ $$ = $1;}
                 ;
fullSelectStmt   : IF simpleExp THEN full ELSE full
			{
			  $$ = newStmtNode(IfK, $1);
			  $$->child[0] = $2;
			  $$->child[1] = $4;
			  $$->child[2] = $6;
			}
                 ;
unfullSelectStmt : IF simpleExp THEN stmt
			{
			  $$ = newStmtNode(IfK, $1);
			  $$->child[0] = $2;
			  $$->child[1] = $4;
			}
                 | IF simpleExp THEN full ELSE unfull
			{
			  $$ = newStmtNode(IfK, $1);
			  $$->child[0] = $2;
			  $$->child[1] = $4;
			  $$->child[2] = $6;
			}
                 ;
fullIterStmt     : WHILE simpleExp DO full
			{
			  $$ = newStmtNode(WhileK, $1);
			  $$->child[0] = $2;
			  $$->child[1] = $4;
			}
                 | FOR ID ASGN iterRange DO full
			{
			  $$ = newStmtNode(ForK, $1);
                          $$->child[0] = newDeclNode(VarK, Integer,  $2);
			  $$->child[0]->type = strdup("int");
			  $$->child[1] = $4;
			  $$->child[2] = $6;
			}
                 ;
unfullIterStmt   : WHILE simpleExp DO unfull
			{
			  $$ = newStmtNode(WhileK, $1);
			  $$->child[0] = $2;
			  $$->child[1] = $4;
			}
                 | FOR ID ASGN iterRange DO unfull
			{
			  $$ = newStmtNode(ForK, $1);
			  $$->child[0] = newDeclNode(VarK, Integer, $2);
			  $$->child[0]->type = strdup("int");
			  $$->child[1] = $4;
			  $$->child[2] = $6;
			}
                 ;
expStmt          : exp SEMI
			{ $$ = $1;}
                 | SEMI
			{ $$ = NULL;}
                 ;
compoundStmt     : begin localDecls stmtList END
			{
			  $$ = newStmtNode(CompoundK, $1);
			  $$->child[0] = $2;
			  $$->child[1] = $3;
			}
                 ;
localDecls       : localDecls scopedVarDecl
			{ $$ = giveSibling($1, $2);}
                 |
			{ $$ = NULL;}
                 ;
stmtList         : stmtList stmt
			{ $$ = giveSibling($1, $2);}
                 |
			{ $$ = NULL;}
                 ;
iterRange        : simpleExp TO simpleExp
			{
			  $$ = newStmtNode(RangeK, $2);
			  $$->child[0] = $1;
			  $$->child[1] = $3;
			}
                 | simpleExp TO simpleExp BY simpleExp
			{
			  $$ = newStmtNode(RangeK, $2);
                          $$->child[0] = $1;
                          $$->child[1] = $3;
                          $$->child[2] = $5;
			}
                 ;
returnStmt       : RETURN SEMI
			{ $$ = newStmtNode(ReturnK, $1);}
                 | RETURN exp SEMI
			{
			  $$ = newStmtNode(ReturnK, $1);
			  $$->child[0] = $2;
			}
                 ;
breakStmt        : BREAK SEMI
			{ $$ = newStmtNode(BreakK, $1);}
                 ;
exp              : mutable assignop exp
			{
			  $$ = newExpNode(AssignK, $2);
			  $$->child[0] = $1;
			  $$->child[1] = $3;
			}
                 | mutable INC
			{
			  $$ = newExpNode(AssignK, $2);
			  $$->child[0] = $1;
			}
                 | mutable DEC
			{
			  $$ = newExpNode(AssignK, $2);
			  $$->child[0] = $1;
			}
                 | simpleExp
			{ $$ = $1;}
                 ;
assignop         : ASGN
			{ $$ = $1;}
                 | ADDASGN
			{ $$ = $1;}
                 | DIFFSGN
			{ $$ = $1;}
                 | MULSGN
			{ $$ = $1;}
                 | DIVSGN
			{ $$ = $1;}
                 ;
simpleExp        : simpleExp OR andExp
			{
			  $$ = newExpNode(OpK, $2);
			  $$->child[0] = $1;
			  $$->child[1] = $3;
			}
                 | andExp
			{ $$ = $1;}
                 ;
andExp           : andExp AND unaryRelExp
			{
			  $$ = newExpNode(OpK, $2);
			  $$->child[0] = $1;
			  $$->child[1] = $3;
			}
                 | unaryRelExp
			{ $$ = $1;}
                 ;
unaryRelExp      : NOT unaryRelExp
			{
			  $$ = newExpNode(OpK, $1);
			  $$->child[0] = $2;
			}
                 | relExp
			{ $$ = $1;}
                 ;
relExp           : sumExp relop sumExp
			{
			  $$ = newExpNode(OpK, $2);
			  $$->child[0] = $1;
			  $$->child[1] = $3;
			}
                 | sumExp
			{ $$ = $1;}
                 ;
relop            : LESS
			{ $$ = $1;}
                 | LEQ
			{ $$ = $1;}
                 | GREAT
			{ $$ = $1;}
                 | GEQ
			{ $$ = $1;}
                 | EQ
			{ $$ =$1;}
                 | NEQ
			{ $$ = $1;}
                 ;
sumExp           : sumExp sumop mulExp
			{
			  $$ = newExpNode(OpK, $2);
			  $$->child[0] = $1;
			  $$->child[1] = $3;
			}
                 | mulExp
			{ $$ = $1;}
                 ;
sumop            : CROSS
			{ $$ = $1;}
                 | DASH
			{ $$ = $1;}
                 ;
mulExp           : mulExp mulop unaryExp
			{
			  $$ = newExpNode(OpK, $2);
			  $$->child[0] = $1;
			  $$->child[1] = $3;
			}
                 | unaryExp
			{ $$ = $1;}
                 ;
mulop            : ASTER
			{ $$ = $1;}
                 | DIV
			{ $$ = $1;}
                 | MOD
			{ $$ = $1;}
                 ;
unaryExp         : unaryop unaryExp
			{
			  $$ = newExpNode(OpK, $1);
			  $$->child[0] = $2;
			}
                 | factor
			{ $$ = $1;}
                 ;
unaryop          : DASH
			{
			  $1->tokenstr = "chsign";
			  $$ = $1;
			}
                 | ASTER
			{
			  $1->tokenstr = "sizeof";
			  $$ = $1;
			}
                 | QUES
			{ $$ = $1;}
                 ;
factor           : mutable
			{ $$ = $1;}
                 | immutable
			{ $$ = $1;}
                 ;
mutable          : ID
			{
			  $$ = newExpNode(IdK, $1);
			  $$->attr.name = strdup($1->tokenstr);
			}
                 | ID LBRAC exp RBRAC
			{
			  $$ = newExpNode(OpK, $2);
			  $$->child[0] = newExpNode(IdK, $1);
			  $$->child[1] = $3;
			  //$$->isArray = 1;
			  //$$->arraySize = $3->nvalaue; //dont know how to store this
			}
                 ;
immutable        : LPAREN exp RPAREN
			{ $$ = $2;}
                 | call
			{ $$ = $1;}
                 | constant
			{ $$ = $1;}
                 ;
call             : ID LPAREN args RPAREN
			{
			  $$ = newExpNode(CallK, $1);
			  $$->attr.name = strdup($1->tokenstr);
			  $$->child[0] = $3;
			}
                 ;
args             : argList
			{ $$ = $1;}
                 |
			{ $$ = NULL;}
                 ;
argList          : argList COMMA exp
			{ $$ = giveSibling($1, $3);}
                 | exp
			{ $$ = $1;}
                 ;
constant         : NUMCONST
			{
			  $$ = newExpNode(ConstantK, $1);
			}
                 | CHARCONST
                        {
                          $$ = newExpNode(ConstantK, $1);
			  $$->attr.string = $1->modified; //overwrite subkind member
                        }
                 | STRINGCONST
                        {
                          $$ = newExpNode(ConstantK, $1);
			  $$->attr.string = $1->modified; //overwrite subkind member
                        }
                 | TRUE
                        {
                          $$ = newExpNode(ConstantK, $1);
                        }
                 | FALSE
                        {
                          $$ = newExpNode(ConstantK, $1);
                        }
                 ;
%%


int main(int argc, char *argv[]) {

    int c, d = 0, p = 0;

    //check flags until error or input is done
    while ((c = ourGetopt(argc, argv, (char *)"dp")) != -1) {
        switch (c) {
            case 'd':	//yydebug flag
                ++d;
                break;

            case 'p':   //print tree flag
                ++p;
                break;

            default:
                printf("Error: invalid option (flag) used\n");
                break;
        }
    }


    if( d == 1) {       //set yydebug
        yydebug = 1;
    }

    if (optind < argc) { //while we run out of arguments
        if(yyin = fopen(argv[optind], "r")) {
            //success
        }
        else {
            printf("Error: failed to open \'%s\'\n", argv[optind]);
            exit(1);
        }
        yyparse();	//yyparse() calls yylex() to scan, then parses input
    }

    if( p == 1) {	//print AST tree with indent and sibling values of 1
        printTree(tree, 1, 1);
    }



    return 0;

}

