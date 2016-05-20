/*******************************************************************************
 * James Ross
 * Thread safe binary search tree.
 * Originally written for exercise 30-2 in linux programming interface
 *
 * filename: threadSafeNoBalBST.c 
 * 
 * implements the functions in threadSafeNoBST.h and its helper functions
 ******************************************************************************/

#include "threadSafeNoBalBST.h"

//********************** prototypes ********************************************
/*******************************************************************************
 *                      PLACE_DIRECTION
 *  handle locks and calls placeNewNode function for placeNewNodes recursion.
 ******************************************************************************/
static inline int place_direction(bstNode_s *bstNode_sPtr, char *newKey, 
                                  void *nData, char *nType, int direct);

//********************** macros ************************************************

//******************* static functions *****************************************
/*******************************************************************************
 *                    freeAllNodes
 *  Frees all the nodes in a BST recursivly.
 *  Use with 1 thread, or multiple threads with different subtrees. Will return
 *  an error value if a thread being removed is allready being removed by
 *  another.
 ******************************************************************************/
static void freeAllNodes(bstNode_s *node){//#{{{
    int retVal = 0;

    if(node == NULL){ return; }

    freeAllNodes(node -> child[RIGHT]);
    freeAllNodes(node -> child[LEFT]);

    // if at a leaf, deallocate the node
    if(NULL == node -> child[RIGHT] && NULL == node -> child[LEFT]){
        // check if node is being deleted by another thread before continuing
        retVal = pthread_mutex_trylock(node -> mtx);
        if(SUCCESS == retVal){ 
            pthread_mutex_destroy(node -> mtx);
            FREE_ALL(node -> data, node -> type, node -> key, node);
            node = NULL;
        }//end if
        else{ 
            errMsg("While freeing all, locked node was encountered"); 
            pthread_exit(&retVal);
        }//end else
    }//end if
}//end freeAllNodes #}}}

/*******************************************************************************
 *                    placeNewNode 
 *  Goes down the tree and inserts a new node.
 *  nData becomes null after fuction executes to prevent freeing used data in
 *  client.
 ******************************************************************************/
static int placeNewNode(bstNode_s **node, char *newKey, void *ndata, char *ntype){//#{{{
    size_t len = strlen(newKey) + 1;

    if(NULL == *node){
        nodeInitBST(node);
        (*node) -> key = (char*) malloc(sizeof(char)*len);
        strncpy((*node) -> key, newKey, len); // set key
        (*node) -> data = ndata;              // set data
        ndata = NULL;                         // prevent post call free
        len = strlen(ntype) + 1;
        (*node) -> type = (char*) malloc(sizeof(char)*len);
        strncpy((*node) -> type, ntype, len); // set type
        return SUCCESS;
    }//end for

    if(strncmp((*node) -> key, newKey, len) < 0){
        return place_direction(*node, newKey, ndata, ntype, LEFT);}
    else{//greater than or equal to
        return place_direction(*node, newKey, ndata, ntype, RIGHT);}
    return FAILURE;
}//end placeNewNode #}}}

static inline int place_direction(bstNode_s *node, char *newKey,//#{{{
                                  void *nData, char *nType, int direct){
    Bool locked = false;                                                       
    int ret     = 0;                                                           

    if(NULL == (node) -> child[direct]){ /* lock node prior to NULL */   
        ret = pthread_mutex_lock((node) -> mtx);
        if(SUCCESS != ret){ errnumExit(ret,"place_direction: mtx lock failure");}
        locked = true;                                                         
        if(NULL != (node) -> child[direct]){/* allocated post lock?*/    
            ret = pthread_mutex_unlock((node) -> mtx);
            if(SUCCESS != ret){ errnumExit(ret,"place_direction: mtx unlock failure");}
            locked = false;                                                    
        }/* end if */                                                          
    }/* end if */                                                              
    ret = placeNewNode(&(node) -> child[direct], newKey, nData, nType);  
    if(true == locked){                                                        
        ret = pthread_mutex_unlock((node) -> mtx);                             
        if(SUCCESS != ret){ errnumExit(ret,"place_direction: mtx unlock failure");}
        locked = false;                                                        
    }/* end if */
    return ret;                                                                
}//end placeDirection #}}}

//******************* header functions *****************************************
void initBST(bstRoot_s **tree){//#{{{
    int retVal = 0;

    if(NULL == *tree){ *tree = (bstRoot_s*)malloc(sizeof(bstRoot_s)); }
    if(NULL == *tree){ errExit("initBST: bstRoot_s malloc failure"); }
    (*tree) -> treeMTX = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    if(NULL == (*tree) -> treeMTX){ errExit("initBST: treeMTX malloc failure"); }
    retVal = pthread_mutex_init((*tree) -> treeMTX, NULL);
    if(SUCCESS != retVal){ errExit("initBST: failed to init treeMTX"); }
    (*tree) -> root = NULL;
}//end initBST #}}}

int nodeInitBST(bstNode_s **toInit){//#{{{
    int retVal = SUCCESS;

    //printf("nodeInitBST\n");

    if(NULL == toInit) { 
        errMsg("nodeInitBST: toInit == NULL, nothing to init");
        return FAILURE;
    }//end if 
    if(NULL == *toInit){ *toInit = (bstNode_s*)malloc(sizeof(bstNode_s)); }
    if(NULL == *toInit){ errExit("nodeInitBST: bstNode_s malloc failure"); }
    (*toInit) -> data  = NULL;
    (*toInit) -> type  = NULL;
    (*toInit) -> key   = NULL;
    (*toInit) -> child[RIGHT] = (*toInit) -> child[LEFT] = NULL;
    (*toInit) -> mtx = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    if(NULL == (*toInit) -> mtx){ errExit("nodeInitBST: mtx malloc failure"); }
    retVal = pthread_mutex_init((*toInit) -> mtx, NULL);
    if(SUCCESS != retVal){ errnumExit(retVal,"nodeInitBST: mtx init failure"); }
    return SUCCESS;
}//end nodeInitBST #}}}

void freeBST(bstRoot_s **tree){//#{{{
    if(NULL == tree) { return; }
    if(NULL == *tree){ return; }
    freeAllNodes(((*tree) -> root));
    *tree = NULL;
}//end freeBST #}}}

int addBST(bstRoot_s *tree, char *key, void *data, char *type){//#{{{
    int ret    = 0;
    int pnnRet = 0;

    if(NULL == tree){ errMsg("addBST: No tree to insert data into.");}
    /* if the root is NULL, lock the tree, otherwise do your thing */
    if(NULL != tree -> root){ pnnRet = placeNewNode(&(tree -> root), key, data, type);}//end if
    else{ 
        ret = pthread_mutex_lock(tree -> treeMTX);
        if(SUCCESS != ret){ errnumExit(ret, "addBST: treeMTX lock failure");}
        pnnRet = placeNewNode(&(tree -> root), key, data, type); 
        ret = pthread_mutex_unlock(tree -> treeMTX);
        if(SUCCESS != ret){ errnumExit(ret, "addBST: treeMTX unlock failure");}
    }//end else
    return pnnRet;
}//end addBST #}}}

int removeNode(bstRoot_s *tree, char *key){//#{{{

}//end removeNode #}}}

Bool lookupBST(bstRoot_s *tree, char *key, void **data){//#{{{

}//end lookupBST #}}}

void dispInOrder(bstNode_s *node, unsigned int *count){//#{{{
    if(NULL == node){ return; }
    else{ ++(*count); }
    dispInOrder(node -> child[LEFT], count);
    /* display node contents */
    printf("data: %p type: %s key: %s\n", node -> data, node -> type, node -> key);
    dispInOrder(node -> child[RIGHT], count);
}//end dispInOrder #}}}

void displayBST(bstRoot_s *tree){//#{{{
    unsigned int count = 0;

    //printf("displayBST\n");

    if(NULL == tree){
        printf("Tree is empty\n");
        return;
    }//end if
    dispInOrder(tree -> root, &count); 
    printf("total nodes: %u\n", count);
}//end displayBST #}}}
/*******************************************************************************
 *                          EOF
 ******************************************************************************/
