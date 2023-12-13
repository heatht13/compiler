%{

#include "main.h"

#define YYERROR_VERBOSE

%}

%union {
    TokenData *tokenData;	//terminals
    ExpType type;		//passing types
    TreeNode *treeptr;		//non-terminals
}
	/*lexer tokens*/
%token <tokenData> NUMCONST ID CHARCONST STRINGCONST BOOLCONST
%token <tokenData> AND OR NOT Begin END IF THEN ELSE WHILE DO FOR
%token <tokenData> TO BY BREAK RETURN INT CHAR BOOL STATIC ASGN
%token <tokenData> ADDASGN DIFFASGN MULASGN DIVASGN INC DEC LPAREN
%token <tokenData> RPAREN COLON SEMI COMMA QUES LBRAC RBRAC EQ
%token <tokenData> NEQ LEQ GEQ LESS GREAT CROSS DASH ASTER DIV MOD
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
			{
                          $$ = $1;
			  tree = $$;
			}
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
                 | error
			{ $$ = NULL;}
                 ;
varDecl          : typeSpec varDeclList SEMI
			{
			  setType($2, $1, 0, 1); /*assign typespec info to variable list*/
			  $$ = $2;
			  yyerrok;
			}
                 | error varDeclList SEMI
			{
			  $$ = NULL;
			  yyerrok;
			}
                 | typeSpec error SEMI
			{
			  $$ = NULL;
			  yyerrok;
			}
                 ;
scopedVarDecl    : STATIC typeSpec varDeclList SEMI
			{
			  setType($3, $2, 1, 1); /*set typespec and static info to var list*/
			  $$ = $3;
                          yyerrok;
			}
                 | typeSpec varDeclList SEMI
			{
			  setType($2, $1, 0, 1); /*set typespec info to variable list*/
			  $$ = $2;
                          yyerrok;
			}
            
                 ;
varDeclList      : varDeclList COMMA varDeclInit
			{
			  $$ = giveSibling($1, $3);
			  yyerrok;
			}
                 | varDeclInit
			{ $$ = $1;}
                 | varDeclList COMMA error
			{ $$ = NULL;}
                 | error
			{ $$ = NULL;}
                 ;
varDeclInit      : varDeclId
			{ $$ = $1;}
                 | varDeclId COLON simpleExp
			{
			  $$ = newExpNode(InitK, $2);
			  $$->child[0] = $1;
			  $$->child[1] = $3;
			}
                 | error COLON simpleExp
			{
			  $$ = NULL;
			  yyerrok;
			}
                 | varDeclId COLON error
			{
			  $$ = NULL;
			  //yyerrok;
			}
                 ;
varDeclId        : ID
			{
			  $$ = newDeclNode(VarK, $1);
			}
                 | ID LBRAC NUMCONST RBRAC
			{
			  $$ = newDeclNode(VarK, $1);
			  $$->arraySize = $3->nvalue;
			  $$->isArray = 1;
			}
                 | ID LBRAC error
			{ $$ = NULL;}
                 | error RBRAC
			{
			  $$ = NULL;
			  yyerrok;
			}
                 ;
typeSpec         : BOOL
			{
			  $$->nvalue = (ExpType)Boolean;
			  $$->tokenstr = (char *)"bool";}
                 | CHAR
			{
			  $$->nvalue = (ExpType)Char;
			  $$->tokenstr = (char *)"char";}
                 | INT
			{
			  $$->nvalue = (ExpType)Integer;
			  $$->tokenstr = (char *)"int";}
                 ;
funDecl          : typeSpec ID LPAREN parms RPAREN compoundStmt
			{ 
			  $$ = newDeclNode(FuncK, $2);
			  $$->expType = (ExpType)$1->nvalue;
			  if($1->tokenstr != NULL) {
			      $$->type = strdup($1->tokenstr); //tokenstr will never be null
			  }
			  $$->child[0] = $4;
			  $$->child[1] = $6;
			}
                 | ID LPAREN parms RPAREN compoundStmt
			{
			  $$ = newDeclNode(FuncK, $1);
			  $$->expType = Void;
                          $$->type = (char *)"void";
			  $$->child[0] = $3;
			  $$->child[1] = $5;
			}
                 | typeSpec error
			{ $$ = NULL;}
                 | typeSpec ID LPAREN error
			{ $$ = NULL;}
                 | ID LPAREN error
			{ $$ = NULL;}
                 | ID LPAREN parms RPAREN error
			{ $$ = NULL;}
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
                 | parmList SEMI error
			{ $$ = NULL;}
                 | error
			{ $$ = NULL;}
                 ;
parmTypeList     : typeSpec parmIdList
			{
			  setType( $2, $1, 0, 0);
                          $$ = $2;
			}
                 | typeSpec error
			{ $$ = NULL;}
                 ;
parmIdList       : parmIdList COMMA parmId
			{
			  $$ = giveSibling($1, $3);
			  yyerrok;
			}
                 | parmId
			{ $$ = $1;}
                 | parmIdList COMMA error
			{ $$ = NULL;}
                 | error
			{ $$ = NULL;}
                 ;
parmId           : ID
			{ $$ = newDeclNode(ParamK, $1);}
                 | ID LBRAC RBRAC
			{
			  $$ = newDeclNode(ParamK, $1);
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
                 | IF error
			{ $$ = NULL;}
                 | IF error ELSE full
			{
			  $$ = NULL;
			  yyerrok;
			}
                 | IF error THEN full ELSE full
			{
			  $$ = NULL;
			  yyerrok;
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
                 | IF error THEN stmt
			{
			  $$ = NULL;
			  yyerrok;
			}
                 | IF error THEN full ELSE unfull
			{
			  $$ = NULL;
			  yyerrok;
			}
                 ;
fullIterStmt     : WHILE simpleExp DO full
			{
			  $$ = newStmtNode(WhileK, $1);
			  $$->child[0] = $2;
			  $$->child[1] = $4;
			}
                 | WHILE error DO full
			{
			  $$ = NULL;
			  yyerrok;
			}
                 | WHILE error
			{ $$ = NULL;}
                 | FOR ID ASGN iterRange DO full
			{
			  $$ = newStmtNode(ForK, $1);
			  $$->child[0] = newDeclNode(VarK, $2);
                          $$->child[0]->expType = Integer;
                          $$->child[0]->type = strdup((char *)"int");
			  $$->child[1] = $4;
                          $$->child[2] = $6;
			}
                 | FOR ID ASGN error DO full
			{
			  $$ = NULL;
			  yyerrok;
			}
                 | FOR error
			{ $$ = NULL;}
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
			  $$->child[0] = newDeclNode(VarK, $2);
                          $$->child[0]->expType = Integer;
                          $$->child[0]->type = strdup((char *)"int");
			  $$->child[1] = $4;
                          $$->child[2] = $6;
			}
                 ;
expStmt          : exp SEMI
			{ $$ = $1;}
                 | SEMI 
			{ $$ = NULL;}
                 | error SEMI
			{
			  $$ = NULL;
			  yyerrok;
			}
                 ;
compoundStmt     : Begin localDecls stmtList END
			{
			  $$ = newStmtNode(CompoundK, $1);
			  $$->child[0] = $2;
			  $$->child[1] = $3;
			  yyerrok;
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
                 | simpleExp TO error
			{ $$ = NULL;}
                 | error BY error
			{
			  $$ = NULL;
			  yyerrok;
			}
                 | simpleExp TO simpleExp BY error
			{ $$ = NULL;}
                 ;
returnStmt       : RETURN SEMI
			{ $$ = newStmtNode(ReturnK, $1);}
                 | RETURN exp SEMI
			{
			  $$ = newStmtNode(ReturnK, $1);
			  $$->child[0] = $2;
			  yyerrok;
			}
                 | RETURN error SEMI
			{
			  $$ = NULL;
			  yyerrok;
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
                 | error assignop exp
			{
			  $$ = NULL;
			  yyerrok;
			}
                 | mutable assignop error
			{ $$ = NULL;}
                 | error INC
			{
			  $$ = NULL;
			  yyerrok;
			}
                 | error DEC
			{
			  $$ = NULL;
			  yyerrok;
			}
                 ;
assignop         : ASGN
			{ $$ = $1;}
                 | ADDASGN
			{ $$ = $1;}
                 | DIFFASGN
			{ $$ = $1;}
                 | MULASGN
			{ $$ = $1;}
                 | DIVASGN
			{ $$ = $1;}
                 ;
simpleExp        : simpleExp OR andExp
			{
			  $$ = newExpNode(OpK, $2);
			  $$->child[0] = $1;
			  $$->child[1] = $3;
			}
                 | simpleExp OR error
			{ $$ = NULL;}
                 | andExp
			{ $$ = $1;}
                 ;
andExp           : andExp AND unaryRelExp
			{
			  $$ = newExpNode(OpK, $2);
			  $$->child[0] = $1;
			  $$->child[1] = $3;
			}
                 | andExp AND error
			{ $$ = NULL;}
                 | unaryRelExp
			{ $$ = $1;}
                 ;
unaryRelExp      : NOT unaryRelExp
			{
			  $$ = newExpNode(OpK, $1);
			  $$->child[0] = $2;
			}
                 | NOT error
			{ $$ = NULL;}
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
                 | sumExp relop error
			{ $$ = NULL;}
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
                 | sumExp sumop error
			{ $$ = NULL;}
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
                 | mulExp mulop error
			{ $$ = NULL;}
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
                 | unaryop error
			{ $$ = NULL;}
                 | factor
			{ $$ = $1;}
                 ;
unaryop          : DASH
			{
			  $1->tokenstr = (char *)"chsign";
			  $$ = $1;
			}
                 | ASTER
			{
			  $1->tokenstr = (char *)"sizeof";
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
			}
                 | ID LBRAC exp RBRAC
			{
			  $$ = newExpNode(OpK, $2);
			  $$->child[0] = newExpNode(IdK, $1);
			  $$->child[1] = $3;
			}
                 ;
immutable        : LPAREN exp RPAREN
			{
			  $$ = $2;
			  yyerrok;
			}
                 | LPAREN error
			{ $$ = NULL;}
                 | call
			{ $$ = $1;}
                 | constant
			{ $$ = $1;}
                 ;
call             : ID LPAREN args RPAREN
			{
			  $$ = newExpNode(CallK, $1);
			  if($1->tokenstr != NULL) {
			      $$->attr.name = strdup($1->tokenstr);
			  }
			  $$->child[0] = $3;
			}
                 | error LPAREN
			{
			  $$ = NULL;
			  yyerrok;
			}
                 ;
args             : argList
			{ $$ = $1;}
                 |
			{ $$ = NULL;}
                 ;
argList          : argList COMMA exp
			{
			  $$ = giveSibling($1, $3);
			  yyerrok;
			}
                 | argList COMMA error
			{ $$ = NULL;}
                 | exp
			{ $$ = $1;}
                 ;
constant         : BOOLCONST
			{
			  $$ = newExpNode(ConstantK, $1);
			  $$->expType = Boolean;
			  $$->type = strdup((char *)"bool");
			}
                 | NUMCONST
			{
			  $$ = newExpNode(ConstantK, $1);
			  $$->expType = Integer;
                          $$->type = strdup((char *)"int");
			}
                 | CHARCONST
                        {
                          $$ = newExpNode(ConstantK, $1);
			  if($1->modified != NULL) {
			      $$->attr.string = strdup($1->modified); //overwrite subkind member
			  }
			  $$->expType = Char;
                          $$->type = strdup((char *)"char");
                        }
                 | STRINGCONST
                        {
                          $$ = newExpNode(ConstantK, $1);
			  if($1->modified != NULL) {
			      $$->attr.string = strdup($1->modified); //overwrite subkind member
			  }
                        }
                 ;
%%

