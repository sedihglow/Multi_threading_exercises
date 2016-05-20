#define __SED_LINUX__

#include "../../../sedhead.h"


int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(stderr,"invalid number of arguments. $testScript ./a.out");}
    int i = 0;
    int max = 100;
    
    for(i = 0; i < max; ++i){ system(argv[1]); }

    exit(EXIT_SUCCESS);
}
