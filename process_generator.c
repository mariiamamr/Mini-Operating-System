#include "headers.h"
#define MAX_LEN 256
FILE* in_File;
int sched_algo_type; int quantum = 0;
int msgq_id; // mytype -> 20 , key -> 333
int msgq_id; // key 444
bool queue_empty = false;
//////////////////////implement struct node, function make new node (as process has process info), function to print processinfo
struct Node
{
    int id; int runTime; int arrivalTime; int prioity; int memory_size;
    struct Node* next;
};
struct Node* addNewNode(int i, int rT, int aT, int p,int mem)
{
    struct Node* new_Node = (struct Node*)malloc(sizeof(struct Node)); // error in malloc-> ask google
    new_Node->id = i;
    new_Node->runTime = rT;
    new_Node->arrivalTime = aT;
    new_Node->prioity = p;
    new_Node->next = NULL;
    new_Node->memory_size = mem;

    return new_Node;
}
///////////
/////////////////////implement struct queue,make empty qeue, enqeue,deqeueue
struct Queue
{
    struct Node* front; struct  Node* tail;
};
struct Queue* initialize_queue()
{
    struct Queue* new_Queue = (struct Queue*)malloc(sizeof(struct Queue*));
    new_Queue->front = NULL;
    new_Queue->tail = NULL;
    return new_Queue;
}
void enqueue(struct Node* new_node, struct Queue* q)
{
    struct Node* n = new_node;
    if (q->tail == NULL)
    {
        q->front = n;
        q->tail = n;
    }
    else
    {
        q->tail->next = n;
        q->tail = n;
    }
}
struct Node* dequeue(struct Queue* q)
{
    struct Node* n;
    if (q->front == NULL)
    {
        n = NULL;
        return NULL;
    }
    else
    {
        n = q->front;
        q->front = q->front->next;
    }
    if (q->front == NULL) q->tail = q->front; // if queue become empty
    return n;
}
//////////////////////
struct msgbuff
{
    long mtype;
    int id;
    int runTime;
    int arrivalTime;
    int prioity;
    int memory_size;
};
////////////////////////

//signals
void clearResources(int);
/////////////////////////
int main(int argc, char * argv[])
{
    printf("\nProcess Genertor Starting.....\n");
    signal(SIGINT, clearResources);
    // TODO Initialization
    struct Queue* my_queue = initialize_queue();
    // 1. Read the input files.
    in_File = fopen("process.txt", "r");
    if (in_File == NULL)
    {
        perror("error in opening the iput file...");
    }
    else
    {
    
        //int id; int aT; int rT; int p; int mem;
        
           char buffer[MAX_LEN]; 
          
          //int id; int runTime; int arrivalTime; int prioity; 
          while(fgets(buffer, sizeof buffer,in_File) !=NULL ) // loop on all lines
          {
              char* comment = buffer; //skip first line 
             int temp; int counter;
             if(comment[0] != '#')
               {  
                     
                 int id; int runTime; int arrivalTime; int prioity; int n =0;int memorySize;
                   
                   while (sscanf(comment,"%d%n",&temp,&counter)) // read each line
                   {
                       switch (n)
                       {
                       case 0:
                           id = temp;  break;
                       case 1:
                           arrivalTime = temp; break;
                       case 2:
                           runTime = temp; break;
                       case 3:
                           prioity = temp; break;
                       case 4:
                          memorySize = temp; break;
                       }

                       comment = comment + counter; 
                       n= n+1;
                   }
                   struct Node* w = addNewNode(id, runTime, arrivalTime, prioity,memorySize);
                   enqueue(w, my_queue);
         printf( "\nprocess of id = %d , arrivaltime = %d,  runtime = %d ,prioity = %d ,memory size = %d",
                     w->id,w->arrivalTime,w->runTime,w->prioity,w->memory_size);
                   //fscanf (input,"%d %d %d %d",&id,&arrivalTime,&runTime,&prioity); // for each line
               }

          }  
         fclose(in_File);     
    }
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.4
    printf("\nprocess genertor:Choose the scheduling algorithim type.. 0:HPF, 1:SRTN,2:RR\n");
    scanf("%d", &sched_algo_type);
    if (sched_algo_type != 0 && sched_algo_type != 1 & sched_algo_type != 2)
    {
        printf("\nprocess genertor:Please,Choose the suitable scheduling algorithim type.. 0:HPF, 1:SRTN,2:RR\n");
        while (sched_algo_type != 0 && sched_algo_type != 1 & sched_algo_type != 2)
        {
            scanf("%d", &sched_algo_type);
        }
    }

    // if RR
    if (sched_algo_type == 2)
    {
        printf("\nprocess genertor:Enter the Quantum value..\n");
        scanf("%d", &quantum);
        if (quantum <= 0)
        {
            printf("\nprocess genertor:Enter the valid value of the Quantum ..\n");
            while (quantum <= 0)
            {
                scanf("%d", &quantum);
            }
        }
    }
    //// create msg queue between process generator and scheduler of key id = 333;
    msgq_id = msgget(333, 0666 | IPC_CREAT); //create message queue and return id
    if (msgq_id == -1)
    {
        perror("Process generator:Error in create");
        exit(-1);
    }
    printf("\nprocess genertor: Message Queue ID = %d\n", msgq_id);

    // 3. Initiate and create the scheduler and clock processes.
    // Do forking to make a child process for CLK and another child process for scheduler....

    int id_child_1 = fork();
    if (id_child_1 == 0)
    {
        // i am the child process (clk)
        // ask google to run file.out>>>
        char* args[] = { "./clk.out",NULL };
        execvp(args[0], args);
        // 4. Use this function after creating the clock process to initialize clock
        //initClk();
    }
    else
    {
        // 4. Use this function after creating the clock process to initialize clock
        initClk();
        // parent -> process genertor
        // do another fork for the scheduler child // and send arguments scheduler type and the quantum value;
        int id_child_2 = fork();
        if (id_child_2 == 0)
        {
            //i am  child process (scheduler) 
            char num1 [20];
            sprintf (num1,"%d",sched_algo_type);   
            char num2 [20];
            sprintf (num2,"%d",quantum);
            char* args[] = { "./scheduler.out",num1,num2,NULL };
            execvp(args[0], args);
        }

        else
        {
                 // i am the parent (process generator)
            // TODO Generation Main Loop
        /*
        while (my_queue->front != NULL)
                {
                    struct Node* ready_process =  dequeue(my_queue);
                        printf( "\nprocess of id = %d , arrivaltime = %d,  runtime = %d ,prioity = %d ,memory size = %d\n",
            ready_process->id,ready_process->arrivalTime,ready_process->runTime,ready_process->prioity,ready_process->memory_size);
                }
        */     
           
                    // {
               
                // 5. Create a data structure for processes and provide it with its parameters.
              // 6. Send the information to the scheduler at the appropriate time.
             // queue not empty send messages to scehduler  
             while(my_queue->front != NULL)
             {
                     if(my_queue->front->arrivalTime == getClk())
                     {
                         struct Node* ready_process =  dequeue(my_queue);
                      int i = ready_process->id; int a = ready_process->arrivalTime; int r = ready_process->runTime;
                      int p = ready_process->prioity; int m = ready_process->memory_size;
                      // make struct msg for the ready process
                      struct msgbuff proc_msg;
                      proc_msg.id = i; proc_msg.arrivalTime = a; proc_msg.memory_size = m;
                      proc_msg.prioity = p; proc_msg.runTime = r; proc_msg.mtype = 20;
                      // message ready to be sent
                      
                      //
                      
 printf( "\nProcess gernator sending process:\nprocess of id = %d , arrivaltime = %d,  runtime = %d ,prioity = %d ,memory size = %d\n",
            ready_process->id,ready_process->arrivalTime,ready_process->runTime,ready_process->prioity,ready_process->memory_size);
                      
                      //

                      int send_val = msgsnd(msgq_id, &proc_msg, sizeof(proc_msg), !IPC_NOWAIT);
                      if (send_val == -1)
                          perror("Process generator:Errror in send process message");

                        //if(my_queue->front == NULL) break;
                     }       
             }
             
               
               struct msgbuff done_msg;
               done_msg.mtype = 20; done_msg.id = -1; done_msg.arrivalTime =  -1;
               done_msg.memory_size  =  -1; done_msg.prioity  =  -1;  done_msg.runTime  =  -1; 
                    int send_val2 = msgsnd(msgq_id, &done_msg, sizeof(done_msg), !IPC_NOWAIT);
                    if (send_val2 == -1)
                        perror("Process generator:Errror in send message 2");
                    else 
               printf("\nProcess generator:Done sending process to scheduler\n"); 
                  
     // }  
          
           //printf("\nProcess generator:????????????\n");
          
  /// waiting for the scheduler when he finishes all scheduling the process and tell that he finished...
    struct msgbuff finish_msg; 
    int rec_val = msgrcv(msgq_id, &finish_msg, sizeof(finish_msg), 30, !IPC_NOWAIT); // send from scheduler by 30
    if (rec_val == -1)
        perror("Process generator:Error in receive message from scheduler ");
    else
        printf("Process generator : recived message from scheduler");

         int x = getClk();
    printf("\nProcess generator:finished time is %d\n", x);
    // 7. Clear clock resources
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0); // clearResources
    destroyClk(true);
      } //// 
   }
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    // remove the message queue 
    // destory clk
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds*)0);
    destroyClk(true);
}
