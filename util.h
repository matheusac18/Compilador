#ifndef _UTIL_H_
#define _UTIL_H_

TreeNode *newStmtNode(StmtKind);
TreeNode *newExpNode(ExpKind);
char *printToken(TokenType token, const char* tokenString);
void printTreeR( TreeNode * tree );
char * copyString( char * );
void insereEscopo(TreeNode *tree, char *escopo);

#endif
