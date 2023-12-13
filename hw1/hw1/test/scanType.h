#ifndef SCANTYPE_H
#define SCANTYPE_H
struct TokenData {
    int tokenclass;       //type of token ie. NUMBER. ID, CHARCONST, etc
    int linenum;          //line token was on
    char *tokenstr;       //lexeme (actual inputted legal token)
    char cvalue;	  //value of characters
    int nvalue;           //value of numbers and bools
};
#endif
