/******************************************************************************
 * James Ross
 * Exercise 30-2
 * book: linux programming interface
 *
 * problem statement:
 * implement a set of thread-safe functions that update and search an
 * unbalanced search tree. This pibrary should include functions (with the
 * obvious purposes_ of the following:
 *      initialize(tree);
 *      add(tree, char *key, void *value);
 *      delete(tree, char *key);
 *      Boolean lookup(char *key, void **value);
 *
 *     In the above prototypes, tree is a structure that points to the root of
 *  the tree (you will need to define a suitable structure for this purpose).
 *  Each element of the tree holds a key-value pair. You will also need to
 *  define the structure for each element to include a mutex that protects that
 *  element so that only one thread at a time can access it. The initialize(),
 *  add() and loopup() functions are reletively simple to implement. The
 *  delete() operation requires a little more effort.
 *
 *  (aside about balanced trees being more difficult to add syncrinization
 *   because of rotations.)
 *
 *   TODO: make a redblack treee thread safe. ;)
 ******************************************************************************/

#include "threadSafeNoBalBST.h"

#define KEY_SIZE 24

#define setArgs(addInfo){                                                      \
    addInfo.toSend = (uint32_t*) malloc(sizeof(uint32_t));                     \
    if(NULL == addInfo.toSend){ errExit("main: failed to malloc");}            \
    *addInfo.toSend = random() % modVal;                                       \
    addInfo.key = (char*) malloc(sizeof(char)*KEY_SIZE);                       \
    if(NULL == addInfo.key){ errExit("main: failed to malloc");}               \
    genRandomString(addInfo.key, KEY_SIZE); /* random key value */             \
    addInfo.type = (char*) malloc(sizeof(char)*4);                             \
    addInfo.type = "int";                                                      \
}//end setArgs

typedef struct nodeInfo{
    bstRoot_s *tree;
    uint32_t *toSend; /* pointer to send as void */
    char *key;        /* key to the node */
    char *type;       /* type the void data is */ 
} addArgsBST_s;

static void* threadAddBST(void *arg){
    addBST(((addArgsBST_s*)arg) -> tree, ((addArgsBST_s*)arg) -> key, 
           ((addArgsBST_s*)arg) -> toSend, ((addArgsBST_s*)arg) -> type);
    pthread_exit(EXIT_SUCCESS);
}//end threadAddBST

/* quick note: srandom should be set before function call. Placing it in the
 *             function call may overright a seed by accident so it is left 
 *             out */
void genRandomString(char *toGen, const int strSize){
    if(NULL == toGen){ noerrExit("genRandomString: char* toGen is NULL");}

    int alphaSize = 0;
    int i = 0;

    /* might not always need to be static. It is static in this case because
     * this function will be called constantly and it prevent having to
     * reallocate the array and refill that memory each time. If this function
     * is rarely used unlike this test function, then the static variable is 
     * not as effective since it exists for the entire program */
    static const char alphaNum[] = {"0123456789"
                                    "abcdefghijklmnopqrstuvwxyz"
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    alphaSize = strlen(alphaNum);
    for(i = 0; i < (strSize-1); ++i){
        toGen[i] = alphaNum[random() % alphaSize];
    }
    toGen[i] = '\0';
}//end genRandomString

/* arg1 = # of inserts, arg 2 = # of removals arg3 = mod */
int main(int argc, char *argv[]){
    pthread_t t1, t2, t3, t4, t5;
    uint32_t insertions  = 0;    /* argv[1] */
    uint32_t removals    = 0;    /* argv[2] */
    uint32_t modVal      = 0;    /* argv[3] */
    uint32_t i           = 0;
    int ret              = 0;
    addArgsBST_s addInfo1 = {.tree = NULL, .toSend = NULL, .key = NULL, .type = NULL};
    addArgsBST_s addInfo2 = {.tree = NULL, .toSend = NULL, .key = NULL, .type = NULL};
    addArgsBST_s addInfo3 = {.tree = NULL, .toSend = NULL, .key = NULL, .type = NULL};
    addArgsBST_s addInfo4 = {.tree = NULL, .toSend = NULL, .key = NULL, .type = NULL};
    addArgsBST_s addInfo5 = {.tree = NULL, .toSend = NULL, .key = NULL, .type = NULL};

    /* seed random for insertion/removal testing. Using time for convenience */
    srandom(time((time_t*)NULL));

    /* check main args */
    if(argc != 4){
        noerrExit("Usage: $ program #inserts #removals modValue\n");}

    /* make sure the same tree is being used for all threads */
    initBST(&addInfo1.tree);
    addInfo2.tree = addInfo1.tree;
    addInfo3.tree = addInfo1.tree;
    addInfo4.tree = addInfo1.tree;
    addInfo5.tree = addInfo1.tree;

    /* convert main args */
    insertions = get32_t(argv[1], 0, (char*)NULL); /* # of insertions */
    removals   = get32_t(argv[2], 0, (char*)NULL); /* # of removals */
    modVal     = get32_t(argv[3], 0, (char*)NULL); /* mod value */

    if(insertions < removals){
        noerr_msg("Warning, more removals than insertions");}
    /* insert */
    for(/*i=0*/; i < insertions; ++i){
        /* set data for each thread */
        setArgs(addInfo1);
        ret = pthread_create(&t1, NULL, threadAddBST, &addInfo1);
        if(FAILURE == ret){ noerrExit("inserting data into tree"); }

        setArgs(addInfo2);
        ret = pthread_create(&t2, NULL, threadAddBST, &addInfo2);
        if(FAILURE == ret){ noerrExit("inserting data into tree"); }

        setArgs(addInfo3);
        ret = pthread_create(&t3, NULL, threadAddBST, &addInfo3);
        if(FAILURE == ret){ noerrExit("inserting data into tree"); }

        setArgs(addInfo4);
        ret = pthread_create(&t4, NULL, threadAddBST, &addInfo4);
        if(FAILURE == ret){ noerrExit("inserting data into tree"); }

        setArgs(addInfo5);
        ret = pthread_create(&t5, NULL, threadAddBST, &addInfo5);
        if(FAILURE == ret){ noerrExit("inserting data into tree"); }

        /* wait for all threads to finish before next iteration */
        if(pthread_join(t1, NULL) == FAILURE){errExit("main: pt_join t1");}
        if(pthread_join(t2, NULL) == FAILURE){errExit("main: pt_join t2");}
        if(pthread_join(t3, NULL) == FAILURE){errExit("main: pt_join t3");}
        if(pthread_join(t4, NULL) == FAILURE){errExit("main: pt_join t4");}
        if(pthread_join(t5, NULL) == FAILURE){errExit("main: pt_join t5");}
    }//end for 

    //displayBST(addInfo1.tree);

    //freeBST(&addInfo.tree);
    
    //displayBST(addInfo.tree);

    exit(EXIT_SUCCESS);
}//end main
