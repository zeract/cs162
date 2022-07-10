#include <stdio.h>
#include <sys/resource.h>

int main() {
    struct rlimit lim,lim1,lim2;
    if(getrlimit(RLIMIT_STACK,&lim)==0){
        printf("stack size: %ld\n",lim.rlim_cur );
    }
    if(getrlimit(RLIMIT_NPROC,&lim1)==0){
        printf("process limit: %ld\n", lim1.rlim_cur);
    }
    if(getrlimit(RLIMIT_NOFILE,&lim2)==0){
        printf("max file descriptors: %ld\n",lim2.rlim_cur);
    }
    
    return 0;
}
