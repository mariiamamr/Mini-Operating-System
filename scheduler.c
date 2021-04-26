#include "headers.h"
#include <stdio.h>
#include <math.h>
// read the alarm
//////////////////////////VARIABLES NEEDED//////////////////////////
FILE* out_File1; // for shceduler.log format -> "At time x process y started arr w total z remain y wait w"
FILE* out_File2; // for scheduler.perf       -> eror fl power w sqrt
FILE *output3; //for memory.log
int schu_type; int quatntum;
int msgq_id; // mytype -> 20 , key -> 333
struct proc_PCB* Head_PCB; //
struct proc_PCB* Head_PCB_runn = NULL; // form current runing process in hpf
struct proc_PCB* Head_PCB_runn2 = NULL; // form current runing process in RR
bool done_recv = false;
bool sch_start = false;
int  sch_start_time = -1;
bool process_running = false;
int  process_running_pid = -1;
int time_taken_SRTN = 0;
struct itimerval timer;
//struct itimerval timer;
//struct itimerval itv;
//static struct timeval start;
bool process_running_2 = false; // for RR
int  process_running_pid_2 = -1; // for RR
int numb_total_p = 0;
int total_rt = 0;
int total_wt = 0;
double total_wta = 0;
int i_wta = 0;
double a_wta[256];
int e=1023;
//////////////////////////////////////////////////////////////////
///////////////////////////STRUCTS NEEDED/////////////////////
struct msgbuff
{
    long mtype;
    int id;
    int runTime;
    int arrivalTime;
    int prioity;
    int memory_size;
};
struct proc_PCB
{
    int pid;
    int id;
    int runTime;
    int remainTime;
    int arrivalTime;
    int prioity;
    int memory_size;
    struct proc_PCB* next;
};
/////////////////////////////////////////////////////////////////
/////////////////////////////FUNCTIONS NEEDED///////////////////
struct proc_PCB* add_new_process(int pd, int id, int rt, int at, int p ,int mem)
{
    struct proc_PCB* new_proc = (struct proc_PCB*)malloc(sizeof(struct proc_PCB));
    new_proc->pid = pd;
    new_proc->id = id;
    new_proc->arrivalTime = at;
    new_proc->remainTime = rt; // remaingtime ..
    new_proc->runTime = rt;
    new_proc->memory_size = mem;
    new_proc->prioity = p;
    new_proc->next = NULL;
    return new_proc;
}
struct proc_PCB* delete_current_process(int pd,struct proc_PCB** Head) // to delete send pid of the process to be deleted and a refrence to the head
{
    struct proc_PCB* curr_h = *Head;
    struct proc_PCB* curr_hnext =NULL;
    if (curr_h == NULL) return NULL;
    else
    {
        if (curr_h->pid == pd)
        {
            (*Head) = (*Head)->next;
            return curr_h;
        }
        
        while (curr_h->next != NULL)
        {
            if (curr_h->next->pid == pd)
            {
                curr_hnext = curr_h->next;
                curr_h->next = curr_hnext->next;
                return curr_hnext;
            }
            
                curr_h = curr_h->next;
            
        }      
    }
    return curr_hnext;
}
struct proc_PCB* search_for_process(int pd, struct proc_PCB* Head) // by pid of the process and head
{
    struct proc_PCB* to_find = NULL;
    struct proc_PCB* curr_h = Head;

    while (curr_h != NULL)
    {
        if (curr_h->pid == pd)
        {
            to_find = curr_h;
            break;
        }           
        else
            curr_h = curr_h->next;
    }
    return to_find;
}
///////////////////////////////////////////////////////////FUNCTIONS FOR EACH TYPE ALGORI/////////////////////////////////////////////////////////////////////////
void run_process(struct proc_PCB** Head)
{
   if ( schu_type ==0  && process_running_pid == -1 && process_running == false) //HPF
   {
        Head_PCB_runn = Head_PCB;
        int pdd = Head_PCB->pid;
        process_running_pid = pdd;
        process_running = true;
        kill(pdd, SIGCONT);
        int x = getClk();
        int d = Head_PCB->id;
        int a = Head_PCB->arrivalTime;
        int r = Head_PCB->runTime;
        int rr = Head_PCB->remainTime;
        int w = 0;
        fprintf(out_File1,"\nAt time %d process %d started arr %d total %d remain %d wait %d",x,d,a,r,rr,w);
        printf("\nScheduler:Process of id = %d ,started from run process\n",d);

   } 
   ///////////////// end of HPF

   if( schu_type == 1 ) //SRTF
   {
      time_taken_SRTN = getClk();
      //// if the new process has less remining time so it will be at the head, 
                               //so we have to stop the running process (not the head any more)
    int d = (Head_PCB)->pid; 
    if (process_running_pid != -1 && process_running == true && process_running_pid != d && Head_PCB->next != NULL)// preeamptive
    {
           // the running process is not the head and the head has remaining time less than the running one
        int p_s = Head_PCB->next->pid;       
        kill(p_s, SIGSTOP);
         process_running_pid = -1;
        process_running = false;
        
        /*
        int donetime = getClk() - time_taken_SRTN ;
        int newtime = Head_PCB->remainTime - donetime;
        Head_PCB->remainTime = newtime;
        */
        // print to the file
        int x = getClk();
        int d = Head_PCB->next->id;
        int a = Head_PCB->next->arrivalTime;
        int r = Head_PCB->next->runTime;
        int rr = Head_PCB->next->remainTime;
        int w = x - a - r + rr;
        fprintf(out_File1,"\nAt time %d process %d stopped arr %d total %d remain %d wait %d", x, d, a, r, rr, w);
         printf("\nScheduler:Process of id = %d ,stopped from run process\n",d);
        // then run the head process
       // int p_r = (*Head)->pid;
       // process_running_pid = (*Head)->pid;
       // kill(p_r, SIGCONT);
    }
     /// //////////////   
      if (process_running_pid == -1 && process_running == false)
     {
        
        // no process running so run the ready process
        time_taken_SRTN = getClk();
        process_running_pid = Head_PCB->pid;
        process_running = true;
        

        
        kill(Head_PCB->pid, SIGCONT);     
        // print to the file
        int x = getClk();
        int d = Head_PCB->id;
        int a = Head_PCB->arrivalTime;
        int r = Head_PCB->runTime;
        int rr = Head_PCB->remainTime;
        int w = 0;

        
        if(r == rr || a == getClk())
        {
           fprintf(out_File1,"\nAt time %d process %d started arr %d total %d remain %d wait %d", x, d, a, r, rr, w);
           printf("\nScheduler: STRN Process of id = %d ,started from run process\n",d);
        }
          
        else
        {
            fprintf(out_File1,"\nAt time %d process %d resumed arr %d total %d remain %d wait %d", x, d, a, r, rr, abs(x-a-r+rr));
            printf("\nScheduler: STRN Process of id = %d ,resumed from run process\n",d);
        }
         
          
         
         
     }
  }
  ///// end of SRTF
   if ( schu_type == 2 && process_running_pid_2 == -1 && process_running_2 == false) //RR
   {
 
        // no process running so run the ready process
        process_running_pid_2 = Head_PCB->pid;
        process_running_2 = true;
        Head_PCB_runn2 = Head_PCB;
        kill(process_running_pid_2, SIGCONT);// set timer // ask google 
setitimer(ITIMER_REAL, &timer, NULL); 
        //alarm(quatntum);
        // print to the file
        int x = getClk();
        int d = (*Head)->id;
        int a = (*Head)->arrivalTime;
        int r = (*Head)->runTime;
        int rr = (*Head)->remainTime;
        int w = 0;
        if (r == rr )
          fprintf(out_File1, "\nAt time %d process %d started arr %d total %d remain %d wait %d", x, d, a, r, rr, w);
        else
          fprintf(out_File1,"\nAt time %d process %d started arr %d total %d remain %d wait %d", x, d, a, r, rr, abs(x-a-r+rr));
          
        printf("\nScheduler: RR Process of id = %d ,started from run process\n",d);
   }
   /// end of RR   
}
void HPF(struct proc_PCB* p, struct proc_PCB** Head)
{
    /// ////////////////// first add it correcttly
    printf("\nScheduler: insert new pcb in HPF  \n");
    struct proc_PCB** h = Head;
    while( (*h) != NULL)
    {
        if ( p->prioity > (*h)->prioity)
        {
            (*h) = (*h)->next;
        }
        else
        {
            break;
        }
    }
   p->next = (*h);
   //(*Head) = p;   //comment it
    (*h) = p;
//// if no running process is running the ready one 
    run_process(&Head_PCB);
}
/////////////////////
void SRTN(struct proc_PCB* p, struct proc_PCB** Head)
{
    // if there are process running so, calculate remaining time in it 
    //as if the comming process have running time less than it
    if (schu_type == 1&& process_running_pid != -1 && process_running == true && Head_PCB!= NULL)
    {
        int timetaken = getClk() - time_taken_SRTN;
        (*Head)->remainTime = (*Head)->remainTime - timetaken;
    }
    
    if(schu_type == 2)
    {
           //////////////// add the new arrived node at the end
           printf("\nScheduler: insert new pcb in RR  \n");
        if(Head_PCB == NULL) Head_PCB = p;
        else
        {
            while( Head_PCB->next != NULL) Head_PCB = Head_PCB->next;
            Head_PCB = p;
        }
       run_process(&Head_PCB);
        return;
    }

printf("\nScheduler: insert new pcb in SRTN  \n");
    ///////////////////// add it correcttly
    struct proc_PCB** h = Head;
    while ((*h) != NULL)
    {
        if (p->remainTime > (*h)->remainTime)
        {
            (*h) = (*h)->next;
        }
        else
        {
            break;
        }
    }

    p->next = (*h);
    // (*Head) = p;
    (*h) = p;
//// if no running process is running the ready one 
    run_process(&Head_PCB);
}
//////
void RR(struct proc_PCB* p, struct proc_PCB** Head)
{
   //////////////// add the new arrived node at the end
    struct proc_PCB* h = *Head;
    while ( h->next!= NULL)
    {
        h = h->next;
    }
    h->next = p;
    /////////
    //// if no running process is running the ready one 
    run_process(&Head_PCB);
}
/////////////////////////////////////////////SIGNALS//////////////////////////////////////////////

/// ////////////////////////////////////////////////////////////////////////
void alarm_RR(int signum)
{
    printf("\nScheduler: entered alarm handler");
    int dp= process_running_pid_2;
    struct proc_PCB* currentp = search_for_process(dp, Head_PCB);
    int t = getClk();
    int pd = process_running_pid_2;    
    int rold = currentp->remainTime;
    int rnew = rold - quatntum;
    currentp->remainTime = rnew;
    process_running_2 = false;
    // stop the current process and run the next one
          // start from the first again or complete the queue    
    if (currentp->next == NULL)    // no process after it
    {       
        if (Head_PCB != NULL && Head_PCB->pid == process_running_pid_2) 
         // only the current running process is the remining process // the same...
        {
            process_running_2 = true; process_running_pid_2 = Head_PCB->pid;
setitimer(ITIMER_REAL, &timer, NULL); 
        }
        else if (Head_PCB != NULL && Head_PCB->pid != process_running_pid_2) // start from first again
        {
            // stop the running process
            int xx = getClk();
            int dd = process_running_pid_2;
            kill(dd, SIGSTOP);           
            int ii = currentp->id; int arr = currentp->arrivalTime; int run = currentp->runTime;
            int rem = currentp->remainTime; int w = xx - arr - run + rem;
            fprintf(out_File1, "At time %d process %d stopeed arr %d total %d remain %d wait %d ",t, ii, arr, run, rem, w); 
               printf("\nScheduler: process of id = %d stopped",ii);
            // run the next process
            process_running_2 = true;
            process_running_pid_2 = Head_PCB->pid;
            int pdd = process_running_pid_2;
                        kill(pdd, SIGCONT);        
            int inew = Head_PCB->id; int arrnew = Head_PCB->arrivalTime; int runnew = Head_PCB->runTime;
            int remnew = Head_PCB->remainTime;
            int wnew = xx - arrnew - runnew + remnew;
            if(remnew == run)
              {
                 printf("\nScheduler: process of id = %d started ",inew);
                         fprintf(out_File1, "At time %d process %d started arr %d total %d remain %d wait %d "
                            , t, inew, arrnew, runnew, remnew, wnew);
setitimer(ITIMER_REAL, &timer, NULL);
              }

            else
              {
              printf("\nScheduler: process of id = %d resumed ",inew);
                 fprintf(out_File1, "At time %d process %d resumed arr %d total %d remain %d wait %d "
                    , t, inew, arrnew, runnew, remnew, wnew);
setitimer(ITIMER_REAL, &timer, NULL);
              }
       }
    }
    else
    {
        ///////// there are process has a turn
        // stop the running one
        int dp = process_running_pid_2;
                    kill(dp, SIGSTOP);
        int i = currentp->id; int a = currentp->arrivalTime; int ru = currentp->runTime;
        int re = currentp->remainTime; int wt = t - a - ru + re;
        fprintf(out_File1, "At time %d process %d stopeed arr %d total %d remain %d wait %d ", t, i, a, ru, re, wt);
               printf("\nScheduler: process of id = %d stopped ",i);
        // run the next
        int dpnew = currentp->next->pid;
                    kill(currentp->next->pid, SIGCONT);
        process_running_pid_2 = currentp->next->pid; process_running_2 = true;
        int idnew = currentp->next->id; int anew = currentp->next->arrivalTime; int runew = currentp->next->runTime;
        int renew = currentp->next->remainTime; int wtnew = t - anew - runew + renew;
        if(runew == renew )
        {
           fprintf(out_File1, "At time %d process %d started arr %d total %d remain %d wait %d ", t, idnew, anew, runew, renew, wtnew);
           printf("\nScheduler: process of id = %d started ",i);
        }            
        else
        {
         fprintf(out_File1, "At time %d process %d resumed arr %d total %d remain %d wait %d ", t, idnew, anew, runew, renew, wtnew);
         printf("\nScheduler: process of id = %d started ",i);
        }
 // setitimer(ITIMER_REAL, &timer, NULL);
    }

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////-----------Mem-----------////////////////////////////
struct memory_tree {
    int id;
    int begin; 
    int end;
    int process_size;
    int node_size; 

    bool divide;
    bool available; //if it's empty or not
    struct memory_tree *left; //left pointer
    struct memory_tree *right; //right pointer
};

//////////////---------Functions---------/////////////
// (1)
//intialization
//every newnode must be intiallized and return the address of the first byte
struct memory_tree *Node(int start,int last) {
    //int last=start-1;
    //printf("\nNode Node\n");
    //struct
    struct memory_tree *node = (struct memory_tree *) malloc(
            sizeof(struct memory_tree)); //allocate requested memory and return pointer to it
    node->id = -1;
    node->begin = last - start + 1; //so it started at 0
    node->end = last; //ended at 1024-1;
    node->process_size = -1; node->node_size = start;
    node->divide = 0; node->available = 1; //==1 is available
    node->left = NULL;  node->right = NULL;

    return node;
}

//(2)
struct memory_tree *find(struct memory_tree **root, int start) // return node, where to allocate
{
    struct memory_tree *node = *root;
    if (node == NULL) {
        return NULL;
    }
    if (node->available == 1 && node->divide == 0 && start <= node->node_size &&
        start > node->node_size / 2) //can be allocated at the node, makan da fady w monaseb
    {
        return node; //this is the place to allocate
    }
    struct memory_tree *left_tree = find(&node->left, start);
    if (left_tree == NULL) {
        return find(&node->right, start); //right node is the empty 'check the function on it'
    }
    //else
    return left_tree; //left node is empty here

}

//(3)
int memory_allocation(struct memory_tree **root, struct proc_PCB *pcb) ///////////////pcb
{
    //allocate el process baa
    struct memory_tree *node = *root;

    if (pcb->memory_size <= node->node_size) {
        if (node->available == 1) //empty
        {
            if (node->divide == 0) //not splitted before
            {

                if (pcb->memory_size <= node->node_size / 2) {
                    //still not allocated as it'start not the fit place to the process to be allocated
                    //continue splitting and entering the function
                    struct memory_tree *left = Node(node->node_size / 2, (node->end - (node->node_size / 2))); //makan el left
                    struct memory_tree *right = Node(node->node_size / 2, node->end); //makan el right
                    node->available = 1; //fadya 
                    node->divide = 1; //splitted
                    node->left = left; node->right = right;
                    memory_allocation(&node->left, pcb);//continous recursive searching
                } else //awel ma el process size elee 3ayza a3melo allocate yebaa akbr mn splitted
                {
                    //allocate node
                    node->id = pcb->id;
                    node->available = 0; //not free
                    node->divide = 0;
                    node->process_size = pcb->memory_size;
                    node->left = NULL; node->right = NULL;
                    fprintf(output3, "At time %d allocated %d bytes for process %d from %d to %d\n", getClk(),
                            node->process_size, node->id, node->begin, node->end); //output3 which is memory.log
                    //printf("allocate!!!!!"); 
                    return 1;
                }
            }

        }
    }
    return 0;
}

//(4)
//traverse
void search_tree(struct memory_tree **root) //visit lehad el akher
{
    struct memory_tree *node = *root;
    if (node == NULL) {
        return;
    }
    if (node->available == 0) {
        if (node->left->available == 1 && node->right->available == 1) {
            node->available = 1;
            node->id = -1; //No process exist so mark
        } else {
            search_tree(&node->left); search_tree(&node->right);
        }
    }
}

//(5)
//add leaf fl main b3d receive el process 
void insert(struct memory_tree **root, struct proc_PCB *pcb) {
    struct memory_tree *node = *root;
    struct memory_tree *destination = NULL;
    int space = pcb->memory_size;
    while (space < 1024) {
        destination = find(&node, space); //dummy place bageboo mn function find
        if (destination != NULL) {
            memory_allocation(&destination,
                              pcb); //allocate fl makan daa el node dee "da el makan el ha allocate fee el node de" "node of process" bl process
            search_tree(&node); //--start visit from the node to mark amaken fadya mn msh fadyaa--// available or not
            return;
        }
        space = space * 2;
    }
}

//(6)
int memory_deallocation(struct memory_tree **head, struct proc_PCB *node) {
    struct memory_tree *n = *head;
    if (n == NULL) {
        return 0;
    }
    if (n->id == node->id) {
        //printf("\n process_id of the deallocated process : %d \n", n->id);
        fprintf(output3, "At time %d freed %d bytes for process %d from %d to %d\n", getClk(), n->process_size,
                n->id, n->begin, n->end);
         n->id = -1;   n->available = 1; n->divide = 0;
        n->left = NULL; n->right = NULL;
        
        return 1;
    }
    int left = memory_deallocation(&n->left, node);  int right = memory_deallocation(&n->right, node);
    if (left == 1) {
        n->available = 1;
        if (n->right->available == 1 && n->right->divide == 0) {

            n->divide = 0;
            n->left = NULL; n->right = NULL;
            return 1;
        }
    } else if (right == 1) {
        n->available = 1;
        if (n->left->available == 1 && n->left->divide == 0) {
        //printf("\nprocesss\n");
            n->divide = 0;
            n->left = NULL;  n->right = NULL;
            return 1;
        }
    }
    return 0;
}
///////////////////////---------end of memory functions-----------------////////////////////////////
struct memory_tree *root;
////////////////////////////////////////////////////////////////////////////////////////////////////
void end_process_handler(int signum); 
////////////////////////
int main(int argc, char * argv[])
{
    
   root=Node(1024,e);
   printf("\nScheduler Starting.....\n");
   initClk();
   ///////////////////mem
   output3=fopen("memory.log","w+");/////////////////////////////////////////////////////////////mem
   fprintf(output3,"#At time x allocated y bytes for process z from i to j\n");
   /////////////////////
   //signal(SIGCHLD,end_process_handler);
   signal(SIGUSR2,end_process_handler);
   signal(SIGALRM,alarm_RR);
   // signal bt3t processsssssssssssssssssssssssssssssssssssssss
   //TODO implement the scheduler :)
   // taking arguments
   if (argc > 1)
   {
       schu_type = atoi(argv[1]);
       if (schu_type == 2) quatntum = atoi(argv[2]);
   }
///////////////////
   if(schu_type == 0 || schu_type == 1)
      printf("\nScheduler: the Schedule type = %d\n", schu_type);
   if(schu_type == 2)
       printf("\nScheduler: the Schedule type = %d and Quantum value = %d\n", schu_type, quatntum);
///////////////////   

   timer.it_value.tv_sec = quatntum; // If I do this timer stops
   timer.it_value.tv_usec = 100000000;
   timer.it_interval.tv_sec = 0;
   timer.it_interval.tv_usec = 0;
   setitimer(ITIMER_REAL, &timer, NULL);
 /*  
    // Configure the timer to expire after 250 msec... 
 timer.it_value.tv_sec = 0;
 timer.it_value.tv_usec = quatntum*1000;
// ... and every 250 msec after that. 
 timer.it_interval.tv_sec = 0;
 timer.it_interval.tv_usec = quatntum*1000;
 //Start a virtual timer. It counts down whenever this process is
   executing.
 setitimer (ITIMER_VIRTUAL, &timer, NULL);
 alarm(quatntum);
*/



/////////
   out_File1 = fopen("scheduler.log", "w+");
   if (out_File1 == NULL)
   {
       printf("\nScheduler: error in open scheduler.log");
   }
   else
   {
       fprintf(out_File1,"#At time x process y state arr w total z remain y wait k\n");
   }
////////////
   /// create msg queue between it and the process generator
   msgq_id = msgget(333, 0666 | IPC_CREAT); //create message queue and return id
   if (msgq_id == -1)
   {
       perror("Scheduler:Error in create msg queue");
       exit(-1);
   }
   printf("\nScheduler:Message Queue ID = %d\n", msgq_id);
   //////////////////////////////////////////////////////////////////////////
   // start reciving the messages from the process genertor......
   int f = 0;
   while ( !( Head_PCB == NULL && done_recv == true ))
   {
       if (done_recv == false)
       {
          f++;
printf("\nScheduler: START RECEVINGGGG PROCESS for the = %d\n",f); 
            int b = getClk();  printf("\nScheduler: el sa3a dlw2ty = %d\n",b);         
           // start recive msgs
           struct msgbuff p_msg;
           int rec_v = msgrcv(msgq_id, &p_msg, sizeof(p_msg), 20, !IPC_NOWAIT); // send from pg by 20
//printf("\nScheduler: ana hnaaa hshuf fe msg wla la el mara ell = %d\n",f);
           if (rec_v == -1)
               printf("\nScheduler:Error in receive message from process genertor ");
           else if (p_msg.id == -1)
           {
               printf("\nScheduler: finished recieving from process genertor");
                   done_recv = true;
           }
           else
           {
printf("\nScheduler: Trying to revive message for number = %d\n",f);  
               if (sch_start == false)
               {
                   sch_start = true;
                   sch_start_time = getClk();
               }
               int id = p_msg.id; int at = p_msg.arrivalTime; int rt = p_msg.runTime;
               int pp = p_msg.prioity; int mem = p_msg.memory_size;
printf("\nScheduler: recived process of id = %d and at = %d from process genertor",id,at);
int c = getClk();  printf("\nScheduler: el sa3a dlw2ty = %d\n",c); 
               // start forking for each process...
               int curr_p_pd = fork();
               if (curr_p_pd == 0)
               {
                   // child process -> current process
                   char num1[20];
                   sprintf(num1, "%d", id);
                   char num2[20];
                   sprintf(num2, "%d", rt);
                   char* args[] = { "./process.out",num1,num2,NULL };
                   execvp(args[0], args);
               }
               else
              {
                   // parent -> scheduler
                   // stop the process from running by signal SIGSTOP tell make the PCB wrt to the type of the algoritim
                   kill(curr_p_pd, SIGSTOP);
                   
                   struct proc_PCB* p = add_new_process(curr_p_pd, id, rt, at, pp, mem);/////////////
                   ////////////////////////////----mem
                   insert(&root,p);
                   
                   ///////////////////////////
                   
                   
                   if (schu_type == 0)
                   {
printf("\nScheduler:choosing type of algorithim for the %d time\n",f);
                       HPF(p, &Head_PCB);
                   }
                   else if (schu_type == 1)
                   {
 printf("\nScheduler:choosing type of algorithim for the %d time\n",f);                  
                       SRTN(p, &Head_PCB);
                   }
                   else if(schu_type == 2)
                   {
 printf("\nScheduler:choosing type of algorithim for the %d time \n",f);                  
                       SRTN(p, &Head_PCB);  // msh rady yedkhol gwa RR fa handle it gwa SRTN
                   }
printf("\nScheduler:done choosing type of algorithim for the %d time\n",f);
               }
           }
       }
   }

   // close filessssss matnsoshhh
   // finished
   // calculations
  // while (Head_PCB != NULL) {}
   //
   //if (Head_PCB == NULL)
   //{
       out_File2 = fopen("scheduler.perf", "w+");
       if (out_File2 == NULL)
       {
           printf("\nScheduler: Error in open scheduler.perf file");
       }


       int x = getClk();
       double p = 0;
       int c = (total_rt * 100) / (x - sch_start);
       double wta = total_wta / numb_total_p;
       double waiting = (double)total_wt / numb_total_p;

       for (int j = 0; j < i_wta; j++)
       {
           //p = p + 0;//
           double m = a_wta[j] - wta;
           double a = pow(m,2); // power(m,2);
           p = p+a;
       }
       double mean = p / i_wta;
       double std = sqrt(mean);  //sqrt mean//(double)(pow(mean, 0.5));

       fprintf(out_File2, "CPU utlization = %d\n", c);
       fprintf(out_File2, "AVG WTA = %.2f\n", wta);
       fprintf(out_File2, "AVG waiting = %.2f\n", waiting);
       fprintf(out_File2, "std WTA = %.2f\n", std);

       fclose(out_File2);
       fclose(out_File1);
       fclose(output3);

       // tell process genertor that we finish
       printf("\nScheduler:ana khalst\n");
       struct msgbuff msg; msg.mtype = 30;
       int send_val = msgsnd(msgq_id, &msg, sizeof(msg), !IPC_NOWAIT);
       if (send_val == -1)
           perror("Scheduler:Errror in send end message");
  //} 
    //upon termination release the clock resources.
    
    //msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0); // clearResources
    //destroyClk(true);
    
}


//////////////// signals
void end_process_handler(int signum)
 {
    numb_total_p++;
    /*
    int numb_total_p = 0;
    int total_rt = 0;
    int total_wt = 0;
    double total_wta = 0;
    int i_wta = 0; _
        double a_wta[256];
    */
 
//
 //int stat_loc ;
 //int pi = wait(&stat_loc);
// if(!(stat_loc & 0x00FF))
//{
printf("\nScheduler: arrived end process handler \n");
//if(process_running_pid == pi )
  if( (process_running_pid != -1 && process_running == true) || (process_running_pid_2 != -1 && process_running_2 == true) )
{
   
    if (schu_type == 1)
    {
        // HPF OR SRTN besmellah
        // for calculations
        printf("\nScheduler: arrived end process handler 2\n");
        int x = getClk();
        int proc_pid = process_running_pid;  
 
        //printf("\nScheduler:2222\n");        
        struct proc_PCB* proc = delete_current_process(proc_pid, &Head_PCB); 
                      //printf("\nScheduler: of ipd = %d\n",proc->id); 
                      ////////////////memory
        memory_deallocation(&root,proc);
                       //printf("\nScheduler:22\n");
        process_running_pid = -1;  process_running = false;   
                      // printf("\nScheduler:3\n");       
        int i = proc->id;
                       //printf("\nScheduler:4\n");
        int rt = proc->runTime;
                       //printf("\nScheduler:5\n");
        int at = proc->arrivalTime;
                         // printf("\nScheduler:6\n");
        int rr = 0;      
        int wt = x - at - rt;
        int wtar = (x - at) / rt;
        total_rt = total_rt + rt;
        total_wt = total_wt + wt;
        total_wta = total_wta + wtar;
        a_wta[i_wta] = wtar; i_wta++;
        printf("\nScheduler:Process of id = %d ,finished at time = %d \n",proc->id,x );
        fprintf(out_File1, "\nAt time %d process %d finished arr %d total %d remain %d wait %d ",
         x, i, at, rt, rr, abs(wt));
        
       //////////////// kill(proc_pid, SIGSTOP;

      // continue the remaining process
        if (Head_PCB != NULL)
        {  
             run_process(&Head_PCB);          
        }
        else
        {
          if (Head_PCB == NULL && done_recv == true )
          {
             //return;
          }
          else
           {
            process_running_pid = -1;  process_running = false;
            //signal(SIGCHLD,end_process_handler);
            //return;
           }
        }
    } 
    else if ( schu_type == 0 )
    {
    
        // HPF 
        // for calculations
        printf("\nScheduler: arrived end process handler 2 of id = %d\n",Head_PCB_runn->id);
        int x = getClk();
        int proc_pid = process_running_pid;  
 
        //printf("\nScheduler:2222\n");        
        struct proc_PCB* proc = Head_PCB_runn ;       
         printf("\nScheduler: of id = %d arrived end process handler 2\n",proc->id); 
         /////////memory
       memory_deallocation(&root,proc);
       struct proc_PCB* procc = delete_current_process(Head_PCB_runn->pid, &Head_PCB); 
                       //printf("\nScheduler:22\n");
        process_running_pid = -1;  process_running = false;   
                      // printf("\nScheduler:3\n");       
        int i = proc->id;
                       //printf("\nScheduler:4\n");
        int rt = proc->runTime;
                       //printf("\nScheduler:5\n");
        int at = proc->arrivalTime;
                         // printf("\nScheduler:6\n");
        int rr = 0;      
        int wt = x - at - rt;
        int wtar = (x - at) / rt;
        total_rt = total_rt + rt;
        total_wt = total_wt + wt;
        total_wta = total_wta + wtar;
        a_wta[i_wta] = wtar; i_wta++;
        printf("\nScheduler:Process of id = %d ,finished at time = %d \n",proc->id,x );
        fprintf(out_File1, "\nAt time %d process %d finished arr %d total %d remain %d wait %d ",
         x, i, at, rt, rr, abs(wt));
        
       //////////////// kill(proc_pid, SIGSTOP;

      // continue the remaining process
        
        if (Head_PCB != NULL)
        {  
             run_process(&Head_PCB);          
        }
        else
        {
          if (Head_PCB == NULL && done_recv == true )
          {
             //return;
          }
          else
           {
            process_running_pid = -1;  process_running = false;
            //signal(SIGCHLD,end_process_handler);
            //return;
           }
        } 
    
    
    
    
    } 
    else
    {
        /// else RR
        
        int proc_pid = process_running_pid_2;
        struct proc_PCB* proc = Head_PCB_runn2;
        struct proc_PCB* procc = delete_current_process(proc_pid, &Head_PCB);
        //////////memory
        memory_deallocation(&root,proc);
        int i = proc->id;
        int rt = proc->runTime;
        int at = proc->arrivalTime;
        int rr = 0;
        int x = getClk();
        int wt = x - at - rt;
        int wtar = (double)(x - at) / rt;

        total_rt = total_rt + rt;
        total_wt = total_wt + wt;
        total_wta = total_wta + wtar;
        a_wta[i_wta] = wtar; 
        i_wta++;

        fprintf(out_File1, "\nAt time %d process %d finished arr %d total %d remain %d wait %d ",
                    x, i, at, rt, rr, wt);
        process_running_pid_2 = -1;  process_running_2 = false;



       // timer 3mla errrrror
        // continue the remaining process
        if (proc->next != NULL)
        {
            kill(proc->next->pid, SIGCONT);
            //alarm(quatntum);
            Head_PCB_runn2 = proc->next;
            printf("\nScheduler: new process will run..");
            process_running_pid_2 = proc->next->pid;  process_running_2 = true;
  setitimer(ITIMER_REAL, &timer, NULL);    //?????????????????????????????
            int run = proc->next->runTime; int rem = proc->next->remainTime;
            int arr = proc->next->arrivalTime; int waitt = 0;  int ii = proc->next->id;
            int xx = getClk();
            if(run == rem)
              fprintf(out_File1, "\nAt time %d process %d started arr %d total %d remain %d wait %d ",
               xx, ii, arr, run, rem, abs(xx - arr - run + rem));
            else
                fprintf(out_File1, "\nAt time %d process %d resumed arr %d total %d remain %d wait %d ", 
                xx, ii, arr, run, rem, abs(xx - arr - run + rem));
            
        }
        else if(Head_PCB != NULL)
        {
             run_process(&Head_PCB);// start again from the begin of the pcb queue
        }
        else if (Head_PCB == NULL && done_recv == true )
        {
             return;
        }
        
    }
    
 //  }
 }
  signal(SIGUSR2,end_process_handler);
   //signal(SIGCHLD,end_process_handler);   
}

/////////////////// RR
void last(struct proc_PCB* p, struct proc_PCB** Head)
{
  printf("\nahhhhhhhh");
	   struct proc_PCB * h = *Head;
	   if(h == NULL )
	   {
	    h = p;
	    run_process(&Head_PCB);
	    return;
	   }
	   while (h->next != NULL)
	   {
	      h = h->next;
	   }
	  h->next=p;
	  run_process(&Head_PCB);
}

