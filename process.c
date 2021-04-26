#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int id;
//void handler(int signum);
int main(int agrc, char * argv[])
{
   //////signal (SIGUSR1, handler); 
    initClk();

    
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    remainingtime = atoi(argv[2]);
    id = atoi(argv[1]);
    double ft = remainingtime + getClk();
printf("\nProcess:process of id = %d starting........\n",id);
    //printf("\nProcess:el sa3a dlw2ty= %d and w hakhls  = %d \n",x,finishtime);
    
    while ( getClk() < ft)
    {
        // remainingtime = ??;
    }
    
     /*    
    while( (double)remainingtime  > ft )
    {
      //
    }
     */
    // when finish send signal to schedule that process end
    // int parent_ID = getppid();
    //kill(SIGUSR1,getppid());
    printf("\nProcess:process of id = %d ,finishing......\n",id);
     kill(getppid(),SIGUSR2);
             ///////////////// printf("\nProcess:process of id = %d ,finishingaked......\n",id);//
    destroyClk(false);
    //  exit(0);
}
/*
void handler(int signum)
{
     
      int stat_loc; 
      int pid= wait(&stat_loc); 
      int index= stat_loc >> 8;   
      int child = pid-getpid();
      
     printf("\nProcess:process of id = %d ,finishing bggg......\n",id);   
     destroyClk(false);
   signal (SIGUSR1,handler); 

}
*/
