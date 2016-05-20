/******************************************************************************
 * James Ross
 * MultiThreading practice
 * exercise from linux command interface
 * exercise: 30-1
 *
 *     Modify the program in listing 30-1(thread_incr.c) so that each loop in 
 * the threads start function outputs the current value of glob and some 
 * identifier that uniquely identifies the thread. The unique identifier for 
 * the thread could be specified as an argument to the pthread_create() call 
 * used to create the thread. 
 *     For this program, that would require changing the argument
 * of the thread's start function to be a pointer to a structure containing the
 * unique identifier and a loop limit value. Run the program, redirecting
 * output to a file, and then inspect the file to see what happens to glob
 * as the kernal scheduler alternated execution between the two threads.
 * ***************************************************************************/


/****************************************************************************** 
 * NOTE: the program being modified is incorrectly incrementing a global
 *       variable from 2 threads. Using this fact, we are observing the results
 *       of the kernal scheduler when things are improperly handled.
 *       Alertations can be seen in reference to 30-1 
 * ***************************************************************************/
#define __SED_ERR__
#define __SED_NUM__
#define __SED_THREAD__
#include "../../../sedhead.h"

#define ID1     1
#define ID2     2

static int glob = 0; // should be volatile

// unique identifier for a thread
struct threadID{
    int uniqueID;
    int loopLim;
};

// threaded function to test with
static void* threadFunc(void *arg)
{
    struct threadID *identity = (struct threadID*)arg;
    int loc = 0;
    int j   = 0;

    for(j = 0; j < (identity -> loopLim); j++){
        printf("thread number: %d\n", identity -> uniqueID);
        loc = glob;
        ++loc;
        glob = loc;
        printf("current global value: %d\n", glob);
    }

    return NULL; // why not destroy? Find out if it should be
} // end treadFunc

int main(int argc, char *argv[])
{
    struct threadID t1ID, t2ID;
    pthread_t t1, t2;
    int errCheck = 0;
    
    t1ID.uniqueID = ID1;
    t2ID.uniqueID = ID2;
    t2ID.loopLim  = t1ID.loopLim = (argc > 1) ? getInt(argv[1],0,"num-loops") 
                                              : 10000000;

    errCheck = pthread_create(&t1, NULL, threadFunc, &t1ID);
    if(0 != errCheck){ errnumExit(errCheck, "main: pthread_create"); }

    errCheck = pthread_create(&t2, NULL, threadFunc, &t2ID);
    if(0 != errCheck){ errnumExit(errCheck, "main: pthread_create"); }

    // does not check equality (althought they wouldnt be equal to main)
    errCheck = pthread_join(t1, NULL);
    if(0 != errCheck){ errnumExit(errCheck, "main: pthread_join: t1"); }

    errCheck = pthread_join(t2, NULL);
    if(0 != errCheck){ errnumExit(errCheck, "main: pthread_join: t2"); }

    printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS); // would remove the loose threads i beleive
}//end main
