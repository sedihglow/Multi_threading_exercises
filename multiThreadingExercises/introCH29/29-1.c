/*
    James Ross
    Exercise from linux programming interface
    29-1

    What possible outcomes might there be if a thread executes the following
    code:
        pthread_join(pthread_self(),NULL);

    Write a program to see what actually happens on Linux. If we have a 
    variable, tis, containing a thread ID, how can a thread prevent itself from
    making a call, pthread_join(tis,NULL), that is equivalent to the above
    statment?
*/

#define __SED_THREAD__
#define __SED_ERR__
#define __SED_NUM__
#include "../sedhead.h"

/******************************* NOTES ***************************************
 *  pthread_create(pthread_t *thread, const pthread_attr_t *attr,
 *                 void *(*start)(void*), void *arg);
 *  // pthread_create(&thread, NULL, function, &arg);
 *  pthread_exit(voit *retVal); // exits thread, returns retVal to join
 *  pthread_t pthread_self(void); // returns thread ID of calling thread
 *  int pthread_equal(pthread_t t1, pthread_t t2); // >0 if t1 && t2 are ==
 *  int pthread_join(pthread_t thread, void **retval); // 0 success else errno
 *
 *  Function call in exercise calls pthread_join on its own thread ID placing
 *  the return value in a NULL pointer.
 *
 *  pthread_join suspends execution on the calling thread untill the thread ID
 *  passed to pthread_join is terminated. If a thread calls this on itself then
 *  the thread will wait for itself to finish executing. Infinit loop equiv.
 *
 *  Lets see what happens..
 *
 *  Resource deadlock 
 *  ERROR: [EDEADLK/EDEADLOCK Resource deadlock avoided]
 *
 *  When i do not error check the returns to the pthread functions the
 *  functionality of the program and its thread seems undefined. Main would
 *  still finish up and the program would close but would not always get what
 *  i want from the thread.
 *
 *  TO PREVENT 
 *  First never used pthread_self inside of pthread_join.
 *
 *  Before calling pthread_join we can prevent this from happening by ensuring
 *  that the thread ID being passed into pthread_join is not equal to the
 *  current threads ID.
******************************************************************************/

/* arg is a char */
static void*  testThread(void *arg){
    char *finalArg = (char*) arg;
    int errCheck = 0;
    pthread_t pretendUnknownID = pthread_self();

    errCheck = pthread_equal(pthread_self(), pretendUnknownID);
    if(0 != errCheck){ errnumExit(errCheck, "testThread: Threads ID's are equal");}

    printf("\nfrom testThread, pre pthread_join: %s\n", finalArg);

    errCheck = pthread_join(pretendUnknownID, NULL);
    if(0 != errCheck){ errnumExit(errCheck, "testThread: pthread_join error");}

    printf("\nfrom testThread, post pthread_join: %s\n", finalArg);

    finalArg[0] = 'G';
    pthread_exit((void*)finalArg);
} //end testThread

int main(int argc, char *argv[])
{ 
    pthread_t t1 = 0;
    void *result = NULL; // result from joining thread
    int errCheck = 0;

    printf("\nFrom main pre create\n");

    errCheck = pthread_create(&t1, NULL, testThread, "This is the arg");
    if(0 != errCheck){ errnumExit(errCheck, "main: pthread_create: testThread");}

    printf("\nFrom main post create\n");

    errCheck = pthread_equal(pthread_self(), t1);
    if(0 != errCheck){ errnumExit(errCheck, "main: Threads ID's are equal");}

    errCheck = pthread_join(t1, &result);
    if(0 != errCheck){ errnumExit(errCheck, "main: pthread_join error");}

    printf("\nFrom main post join\n");
    exit(EXIT_SUCCESS);
} //end main

