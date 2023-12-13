#include "IO.h"

void IOast(TreeNode **root) {
    IONode( root, (char *)"input", (char *)"int", Integer);
    IONode( root, (char *)"inputb", (char *)"bool", Boolean);
    IONode( root, (char *)"inputc", (char *)"char", Char);
    IONode( root, (char *)"output", (char *)"void", Void);
    IONode( root, (char *)"outputb", (char *)"void", Void);
    IONode( root, (char *)"outputc", (char *)"void", Void);
    IONode( root, (char *)"outnl", (char *)"void", Void);
}

void IONode(TreeNode **head, char *name, char *type, ExpType expType) {
    
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    int i; 
    if(node == NULL) {
        printf("Error: Out of memory creating IOtree\n");
    }
    else {
        for( i=0; i<MAXCHILDREN; i++) {
            node->child[i] = NULL;
        }
        if(strcmp(name, (char *)"output") == 0) {
            IONode(&node->child[0], (char *)"dummyi", (char *)"int", Integer);
            node->child[0]->subkind.decl = ParamK;
            node->numParams = 1;
        }
        else if(strcmp(name, (char *)"outputb") == 0) {
            IONode(&node->child[0], (char *)"dummyb", (char *)"bool", Boolean);
            node->child[0]->subkind.decl = ParamK;
            node->numParams = 1;
        }     
        else if(strcmp(name, (char *)"outputc") == 0) {
            IONode(&node->child[0], (char *)"dummyc", (char *)"char", Char);
            node->child[0]->subkind.decl = ParamK;
            node->numParams = 1;
        }
        else {
            node->numParams = 0;
        }
        node->sibling = NULL;
        node->nodeKind = DeclK;
        node->subkind.decl = FuncK;
        if(name) {
            node->attr.name = strdup(name);
        }
        node->line = -1;
        if(type) {
            node->type = strdup(type);
        }
        node->expType = expType;
        node->isArray = 0;
        node->isStatic = 0;
        node->isInit = 1;
        node->declUsed = 1;
        node->initWarn = 1;
        node->isIO = 1;
    }
    TreeNode *ptr = *head;
    if(ptr) {
        while( ptr->sibling != NULL) {
            ptr = ptr->sibling;
        }
        ptr->sibling = node;
    }
    else {
        *head = node;
    }
}

void IOinsert(TreeNode *root, SymbolTable *table) {
    int i;
    while(root) {
        table->insert(root->attr.name, root);
        for( i=0; i<MAXCHILDREN; i++) {
            if(root->child[i] != NULL) {
                table->insert(root->child[i]->attr.name, root->child[i]);
            }
        }
        root = root->sibling;
    }
}
