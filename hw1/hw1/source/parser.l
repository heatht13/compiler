%{
#include "scanType.h"
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int line = 1;
int errors = 0;
int charlen = 0;
char buf[10];

//----------------- Function Declarations ----------------------
//create struct tokenData instance
static int makeToken( int line, int tokenClass, char *input);    

//strip all escape backslashes
static void removeBackSlash(char *toModify);

//convert string to uppercase
static void capitalize(char *toCapitalize);
%}

%option noyywrap

%%

and|or|not|begin|end|bool	{       return makeToken(line, WORDOP, yytext); }
int|if|then|else|return|while	{	return makeToken(line, WORDOP, yytext); }
char|static|for|to|by|break	{	return makeToken(line, WORDOP, yytext); }
true|false			{	return makeToken(line, BOOLCONST, yytext); }

[A-Za-z][A-Za-z0-9]*		{	return makeToken(line, ID, yytext); }
[0-9]+				{	return makeToken(line, NUMCONST, yytext); }
\'.*\'				{	return makeToken(line, CHARCONST, yytext); }
\"([^\\\"]|\\.)*\"			{	return makeToken(line, STRINGCONST, yytext); }

\<\=|\>\=|\<\-|\+\=|\-\-        {       return makeToken(line, OTHER, yytext); }
\+\+|\-\=|\*\=|\!\=|\/\=        {       return makeToken(line, OTHER, yytext); }
[\(\):;,\?\[\]\=\<\>\+\-\*\/\%]	{       return makeToken(line, OTHER, yytext); }


\n				{	line++; }

[ \t\r]*|##.*			;	//ignore all whitespace and cooments

.				{	errors++;
					printf("ERROR(%d): Invalid or misplaced input character: \'%c\'. Character Ignored.\n", line,  yytext[0]); }


%%


static int makeToken( int line, int tokenClass, char *input) {
    yylval.tokenData = new TokenData;
    yylval.tokenData->linenum = line;
    yylval.tokenData->tokenstr = strdup(input);

    if(tokenClass == BOOLCONST) {
        yylval.tokenData->tokenclass = (char *)"BOOLCONST";
        if(strcmp("true", input) == 0) {	//set value
            yylval.tokenData->nvalue = 1;
        }
        else if(strcmp("false", input) == 0) {	//set value
            yylval.tokenData->nvalue = 0;
        }
        else {		//this should never be true
            printf("Error: BOOLCONST token does not match \'true\' or \'false\'\n");
            errors++;
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
        yylval.tokenData->modified = strdup(input);
        removeBackSlash(yylval.tokenData->modified);	//take out escapes
        charlen = strlen(yylval.tokenData->modified) - 2;
        if(charlen > 1) {				//not char value, except \n,\0
            printf("WARNING(%d): character is %d characters long and not a single character: \'%s\'. The first char will be used.\n", line, charlen, yylval.tokenData->modified);
        }
        yylval.tokenData->cvalue = yylval.tokenData->modified[1];
        yylval.tokenData->tokenclass = (char *)"CHARCONST";
    }
    else if(tokenClass == STRINGCONST) {
        yylval.tokenData->modified = strdup(input);
        removeBackSlash(yylval.tokenData->modified);	//take out escapes
        yylval.tokenData->tokenclass = (char *)"STRINGCONST";
        yylval.tokenData->nvalue = strlen(yylval.tokenData->modified) - 2;
    }
    else if(tokenClass == WORDOP) {
        yylval.tokenData->modified = strdup(input);
        capitalize(yylval.tokenData->modified);		//convert lower to upper
    }
    else if(tokenClass = OTHER) {
        yylval.tokenData->tokenclass = strdup(input);
	//statements below handle special operators, convert to letter operators
        if( strcmp(input, (char *)"+=") == 0) {
            yylval.tokenData->tokenstr = (char *)"ADDASGN";
        }
        else if( strcmp(input, (char *)"-=") == 0) {
            yylval.tokenData->tokenstr = (char *)"DIFFSGN";
        }
        else if( strcmp(input, (char *)"--") == 0) {
            yylval.tokenData->tokenstr = (char *)"DEC"; 
        }
        else if( strcmp(input, (char *)"<-") == 0) {
            yylval.tokenData->tokenstr = (char *)"ASGN";
        }
        else if( strcmp(input, (char *)"++") == 0) {
            yylval.tokenData->tokenstr = (char *)"INC";
        }
        else if( strcmp(input, (char *)">=") == 0) {
            yylval.tokenData->tokenstr = (char *)"GEQ";
        }
        else if( strcmp(input, (char *)"<=") == 0) {
            yylval.tokenData->tokenstr = (char *)"LEQ";
        }
        else if( strcmp(input, (char *)"!=") == 0) {
            yylval.tokenData->tokenstr = (char *)"NEQ";
        }
    }
    return tokenClass;
}

static void removeBackSlash(char *toModify) {
    /*if(strcmp(toModify, (char *)'\0') == 0 || strcmp(toModify, (char *)'\n') == 0) {
        return;
    }*/

    char *read = toModify;	//reads input string
    char *write = toModify;	//writes to output string
    bool pair = true;	//ensures only one backslash acts as escape

    while( *read) {	
        *write = *read++;	
        if(*read != '\\') {
            if(*write != '\\') {
                write++;
            }
            *write = *read;	//write read char to write's position
        }
        else {			//read char is a '\'
            pair = !pair;	//find pair every other time
            if(!pair) {		//ensure we havent found a matching blackslash
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

static void capitalize(char *toCapitalize) {
    // only works if all characters are lower case
    char *cap = toCapitalize;
    while( *cap) {
        *cap = (*cap) - ' ';	//subtract 32 to equate to uppercase char
        cap++;
    }
}
