/*******************************************************************************
 * James Ross
 * Thread safe binary search tree.
 * Originally written for exercise 30-2 in linux programming interface
 *
 * filename: threadSafeNoBalBST.h (too long of a name? well.. could shorten.)
 *
 * TODO: Do a thread safe red black tree!
 * TODO: Allocate all memory with allignment for improved cache performance on
 *       threads.
 *
 *
 * NOTE: Instead of doing all type bs.
 * -Pass the pointer void to data, set pointer passed ** to null, so cannot
 *  free it. Have a char* in the struct with type information.
 *  When bst returns data, return by reference the data, return char* type name
 *  since the programmer will know what types their program uses, it makes it
 *  much easier to parse which type it is. Still less efficient than just doing
 *  an explicit type, but want to try and make something flexable.
 *
 ******************************************************************************/

#ifndef __TS_BST__
#define __TS_BST__
#define __SED_THREAD__
#define __SED_ERR__
#define __SED_NUM__
#define __SED_BOOL__
#include "../../../sedhead.h"

#define SUCCESS 0
#define FAILURE 1
#define CHILD   2
#define LEFT    0
#define RIGHT   1

//nodes of BST
typedef struct BSTnode{
    void *data; 
    char *type; // string denoting what type of data it is
    char *key;  // string containing a node indicator for data
    pthread_mutex_t *mtx;
    struct BSTnode *child[CHILD];
}bstNode_s;

//root of a BST
typedef struct BSTroot{
    pthread_mutex_t *treeMTX;
    struct BSTnode *root;
}bstRoot_s;

/******************************************************************************
 *                          intBST
 *     initializes the root of a tree to NULL, will alloc if passed NULL.
 *     TONOTE : this version will exit program on malloc failure.
 *     Returns: 0 on success, or the error number on error
 *     Errors : malloc failure, see errno or malloc man page
 ******************************************************************************/
void initBST(bstRoot_s **tree);

/******************************************************************************
 *                          nodeInitBST
 *     Initializes a BST node. If malloc fails toInit will be NULL
 *     Returns: 0 on success, or the error number on error
 *     Errors : see pthread_mutex_int and malloc man pages for error causes
 ******************************************************************************/
int nodeInitBST(bstNode_s **toInit);

/******************************************************************************
 *                          freeBST
 *     Frees the entire BST.
 *     Errors:
 ******************************************************************************/
void freeBST(bstRoot_s **tree);

/******************************************************************************
 *                          addBST
 *     Adds a node to the BST.
 *     Returns:
 *     Errors :
 ******************************************************************************/
int addBST(bstRoot_s *tree, char *key, void *data, char *type);

/******************************************************************************
 *                         removeBST
 *     Removes a single node from the BST
 *     Returns: 0 on success, -1 on failure
 *     Errors :
 ******************************************************************************/
int removeNode(bstRoot_s *tree, char *key);

/******************************************************************************
 *                         lookupBST
 *     Checks if the desired node exists. Places data at key in void **data
 *     Returns: 1 if found, 0 if failure.
 *     Errors :
 ******************************************************************************/
Bool lookupBST(bstRoot_s *tree, char *key, void **data);

void displayBST(bstRoot_s *tree);
#endif
/*******************************************************************************
 *                          EOF
 ******************************************************************************/
