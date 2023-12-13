#ifndef _SYMBOLTABLE_H_
#define _SYMBOLTABLE_H_
#include <map>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

// // // // // // // // // // // // // // // // // // // // 
//
// Some sample void * printing routines.   User should supply their own.
//
void pointerPrintNothing(void *data);
void pointerPrintAddr(void *data);
void pointerPrintLongInteger(void *data);
void pointerPrintStr(void *data);

// // // // // // // // // // // // // // // // // // // // 
//
// Introduction
//
// This symbol table library supplies basic insert and lookup for
// symbols linked to void * pointers of data.
//
// Plenty of room for improvement inlcuding: better debugging setup,
// passing of refs rather than values and purpose built char *
// routines, and C support.
//
// WARNING: lookup will return NULL pointer if key is not in table.
// This means the void * cannot have zero as a legal value! Attempting
// to save a NULL pointer will get a error.
//
// A main() is commented out and has testing code in it.
//
// Robert Heckendorn   Apr 3, 2021
//


// // // // // // // // // // // // // // // // // // // // 
//
// Class: SymbolTable
//
// Is a stack of scopes.   The global scope is created when the table is
// is constructed and remains for the lifetime of the object instance.
// SymbolTable manages nested scopes as a result.
// 

typedef struct SymbolTable {
    struct Scope;
    std::vector<Scope *> stack;
    stack = (Scope *)malloc
    int debugFlg;

    SymbolTable();
    void debug(int state);                          // sets the debug flags
    int test();                                     // runs tests to validate the SymbolTable class
    int depth();                                     // what is the depth of the scope stack?
    void print(void (*printData)(void *));           // print all scopes using data printing function
    void enter(const char *name);                    // enter a scope with given name
    void leave();                                    // leave a scope (not allowed to leave global)
    void *lookup(const char *sym);                   // returns ptr associated with sym anywhere in symbol table
                                                     // returns NULL if symbol not found
    void *lookupGlobal(const char *sym);             // returns ptr associated with sym in globals
                                                     // returns NULL if symbol not found
    int insert(const char *sym, void *ptr);         // inserts new ptr associated with symbol sym in current scope
                                                     // returns false if already defined
    int insertGlobal(const char *sym, void *ptr);   // inserts a new ptr associated with symbol sym 
                                                     // returns false if already defined
    void applyToAll(void (*action)(const char *, void *));        // apply func to all symbol/data pairs in local scope
    void applyToAllGlobal(void (*action)(const char *, void *));  // apply func to all symbol/data pairs in global scope
} SymbolTable;

#endif
