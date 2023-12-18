%{
#include "scanType.h"
#include "treeDef.h"
#include "c-.tab.h"
#include <string.h>

int line = 1;
int charlen = 0;
extern int numErrors;
extern int numWarnings;
//----------------- Function Declarations ----------------------
//create struct tokenData instance
static int makeToken( int line, int tokenClass, char *input);    

//strip all escape backslashes
void removeBackSlash(char *toModify);
%}

%option noyywrap

%%

"and"				{       return makeToken(line, AND, yytext); }
"or"				{       return makeToken(line, OR, yytext); }
"not"				{       return makeToken(line, NOT, yytext); }
"begin"				{       return makeToken(line, Begin, yytext); }
"end"				{       return makeToken(line, END, yytext); }
"bool"				{       return makeToken(line, BOOL, yytext); }
"int"				{       return makeToken(line, INT, yytext); }
"if"				{       return makeToken(line, IF, yytext); }
"then"				{       return makeToken(line, THEN, yytext); }
"else"				{       return makeToken(line, ELSE, yytext); }
"return"			{       return makeToken(line, RETURN, yytext); }
"while"				{       return makeToken(line, WHILE, yytext); }
"do"				{       return makeToken(line, DO, yytext); }
"char"				{       return makeToken(line, CHAR, yytext); }
"static"			{       return makeToken(line, STATIC, yytext); }
"for"				{       return makeToken(line, FOR, yytext); }
"to"				{       return makeToken(line, TO, yytext); }
"by"				{       return makeToken(line, BY, yytext); }
"break"				{       return makeToken(line, BREAK, yytext); }
"true"				{       return makeToken(line, BOOLCONST, yytext); }
"false"				{       return makeToken(line, BOOLCONST, yytext); }

[A-Za-z][A-Za-z0-9]*		{	return makeToken(line, ID, yytext); }
[0-9]+				{	return makeToken(line, NUMCONST, yytext); }
\'([^\n\']|\\.)*\'		{	return makeToken(line, CHARCONST, yytext); }
\"([^\n\\\"]|\\.)*\"		{	return makeToken(line, STRINGCONST, yytext); }

\<\-				{       return makeToken(line, ASGN, yytext); }
\+\=				{       return makeToken(line, ADDASGN, yytext); }
\-\=				{       return makeToken(line, DIFFASGN, yytext); }
\*\=				{       return makeToken(line, MULASGN, yytext); }
\/\=				{       return makeToken(line, DIVASGN, yytext); }
\+\+   	                        {       return makeToken(line, INC, yytext); }
\-\-                            {       return makeToken(line, DEC, yytext); }

\!\=                            {       return makeToken(line, NEQ, yytext); }
\<\=                            {       return makeToken(line, LEQ, yytext); }
\>\=                            {       return makeToken(line, GEQ, yytext); }
\=                              {       return makeToken(line, EQ, yytext); }
\<                              {       return makeToken(line, LESS, yytext); }
\>                              {       return makeToken(line, GREAT, yytext); }

\?                               {       return makeToken(line, QUES, yytext); }
\+                               {       return makeToken(line, CROSS, yytext); }
\-                               {       return makeToken(line, DASH, yytext); }
\*                               {       return makeToken(line, ASTER, yytext); }
\/                               {       return makeToken(line, DIV, yytext); }
\%                               {       return makeToken(line, MOD, yytext); }

\(  				{       return makeToken(line, LPAREN, yytext); }
\)  				{       return makeToken(line, RPAREN, yytext); }
\:				{       return makeToken(line, COLON, yytext); }
\;				{       return makeToken(line, SEMI, yytext); }
\,				{       return makeToken(line, COMMA, yytext); }
\[				{       return makeToken(line, LBRAC, yytext); }
\]				{       return makeToken(line, RBRAC, yytext); }

\n				{	line++; }

[ \t\r]*|##[^\n]*			;	//ignore all whitespace and cooments

.				{	numErrors++;
					printf("ERROR(%d): Invalid or misplaced input character: \'%c\'. Character Ignored.\n", line,  yytext[0]); }


%%


static int makeToken( int line, int tokenClass, char *input) {
    yylval.tokenData = (TokenData *)malloc(sizeof(TokenData));
    yylval.tokenData->linenum = line;
    yylval.tokenData->tokenstr = strdup(input);
    yylval.tokenData->nvalue = 0;
    yylval.tokenData->cvalue = '\0';
    yylval.tokenData->modified = strdup(input);

    if(tokenClass == BOOLCONST) {	//set value
        yylval.tokenData->tokenclass = (char *)"BOOLCONST";
        if(strcmp(input, (char *)"true") == 0) {
            yylval.tokenData->nvalue = 1;
        }
        else {
            yylval.tokenData->nvalue = 0;
        }
    }
    else if(tokenClass == ID) {
        yylval.tokenData->tokenclass = (char *)"ID";
    } 
    else if(tokenClass == NUMCONST) {
        yylval.tokenData->tokenclass = (char *)"NUMCONST";
        yylval.tokenData->nvalue = atof(input);
    }
    else if(tokenClass == CHARCONST) {
        removeBackSlash(yylval.tokenData->modified);	//take out escapes
        charlen = strlen(yylval.tokenData->modified) - 2;
        yylval.tokenData->nvalue = charlen;
        if(charlen > 1) {				//not char value, except \n,\0
            printf("WARNING(%d): character is %d characters long and not a single character: \'%s\'. The first char will be used.\n", line, charlen, yylval.tokenData->modified);
            numWarnings++;
        }
        if(strcmp(yylval.tokenData->modified, (char*)"''") == 0) {
            yylval.tokenData->cvalue = '\0';
            printf("ERROR(%d): Empty character ''. Characters ignored.\n", line);
            numErrors++;
            return yylex();
        }
        else {
            yylval.tokenData->cvalue = yylval.tokenData->modified[1];
        }
        yylval.tokenData->tokenclass = (char *)"CHARCONST";
    }
    else if(tokenClass == STRINGCONST) {
        removeBackSlash(yylval.tokenData->modified);	//take out escapes
        yylval.tokenData->tokenclass = (char *)"STRINGCONST";
        yylval.tokenData->nvalue = strlen(yylval.tokenData->modified) - 2;
    }
    return tokenClass;
}

void removeBackSlash(char *toModify) {

    char *read = toModify;	//reads input string
    char *write = toModify;	//writes to output string
    int pair = 1;	//ensures only one backslash acts as escape

    while( *read) {	
        *write = *read++;
        if(*read != '\\') {
            if(*write != '\\') {
                write++;
            }
            else {
                if(*read == 'n') {	//catches new line char
                    *write = '\n';
                    write++;
                    read++;
                    continue;
                }
                else if(*read == '0') {	//catches null char
                    *write = ' ';
                    write++;
                    read++;
                    continue;
                }
            }
            *write = *read;	//write read char to write's position
        }
        else {			//read char is a '\'
            pair = 1 - pair;	//find pair every other time
            if(1 - pair) {	//ensure we havent found a matching blackslash
                if(*write != '\\') {
                    write++;
                }
                
            }
            else {		//we already have a matching backslash
                write++;
            }           
        }
    }
    *write++;
    *write = '\0';		//terminate 
}
