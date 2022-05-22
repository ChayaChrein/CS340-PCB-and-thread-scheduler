//Chaya Chrein
// gcc CHREIN_23767794.c -o main.exe -pthread
// ./CHREIN_23767794.exe FCFS FCFS_Section26.txt CHREIN_23767794_FCFS.out
// ./CHREIN_23767794.exe SJF SJF_Section26.txt CHREIN_23767794_SJF.out
// ./CHREIN_23767794.exe PRIORITY PRIORITY_Section26.txt CHREIN_23767794_PRIORITY.out

#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

//MiniPCB class
typedef struct MiniPCB{
  int processNumber;
  int burstOrPriority; //burst time or priority num
  char* operation;
  int opNum; //value for funcPtr
  int param1;
  int param2;
  int returnValue;
} MiniPCB;

//constructor- set all to default values
MiniPCB miniPCB(){
  MiniPCB pcb;
  pcb.processNumber=0;
  pcb.burstOrPriority=0;
  pcb.operation="";
  pcb.opNum=5;
  pcb.param1=0;
  pcb.param2=0;
  pcb.returnValue=0;
  return pcb;
}

//toAsc takes an integer and converts it into a string by using the ascii code
char* toAsc(int number){
 int n = number;
 int digits=0;

  //get the number of digits
 if (number == 0) digits = 1;
  while(n > 0){
     digits++;
     n /= 10;
   }

    //find the degree of the number: 10^digits-1
   int deg = 1;
   for(int i = 1; i < digits; i++){
     deg*=10;
   }

  //need to allocate memory for the new string
   char *str = malloc(digits+1);
   int r;
   int code;

   //convert each digit into it's ascii code
   for(int i = 0; i < digits; i++){
     r = number%deg;
     code = number/deg + 48;
     deg/=10;
     char convert = code;
     str[i] = code;
     number = r;
   }
    //append null character to end of string
   str[digits] = '\0';
   return str;
 }

/*operation functions*/
//summation function
int sum(int x, int y){
  int s=x;
  for (int i=x+1; i<=y; i++){
    s+=i;
  }
  return s;
}

//cumulative product function
int product(int x, int y){
  int s=x;
  for (int i=x+1; i<=y; i++){
    s*=i;
  }
  return s;
}

//exponent function
int power(int x, int y){
  int product=1;
  for(int i=1; i<=y; i++) product*=x;
  return product;
}

//fibonacci function
int fibonacci(int x, int y){
    if (y <= 1) return y;
    return fibonacci(0,y-1) + fibonacci(0,y-2);
}

MiniPCB running; //stores the running process after it's removed from ready queue
int queueSize =15; //works for 15 processes - change based on number of processes
int processesSent=0;
int processesRcv=0;
int s; //index for schedule function
int fd; //global file descriptor for write file

/*
Below are the 3 scheduling algorithms.
  Each one does the following:
    determines the next one that should run
    shifts all other processes that were in later indexes in the queue forwared 1
      so that process is removed from the ready queue
    changes the global variable queueSize to be one less so it will ignore the indexes in the
      ready queue which are greater than or equal queueSize-1
*/
//First come first serve
MiniPCB FCFS(MiniPCB mpcb[]){
  //if there is only one process in the queue return it and remove it from ready queue
  if (queueSize == 1){
    queueSize--; //remove the process from the queue
    return mpcb[0];
  }

  int lowest = mpcb[0].processNumber;//temporarily set the lowest to be the first
  int lowestIndex = 0;//temporarily set the 0th index to be the selected one
  for(int i=1; i<queueSize; i++){
    //find the true lowest process number
    if (mpcb[i].processNumber < lowest){
      lowestIndex = i;
      lowest = mpcb[i].processNumber;
    }
  }
  //set the miniPCB that is to be returned
  MiniPCB toReturn = mpcb[lowestIndex];
  //shift all the processes after the selected one over to "fill the gap"
  for(int i=lowestIndex+1; i<queueSize; i++){
    mpcb[i-1] = mpcb [i];
  }
  MiniPCB mp; //empty pcb
  mpcb[queueSize-1] = mp; //set the last slot in the queue to be the empty pcb
  queueSize--; //"remove last element in the queue" by making it not accessable

  return toReturn;
}

//shortest job first
MiniPCB SJF(MiniPCB mpcb[]){
  //if there is only one process in the queue return it and remove it from ready queue
  if (queueSize == 1){
    queueSize--; //remove the process from the queue
    return mpcb[0];
  }
  int lowest = mpcb[0].burstOrPriority; //temporarily set the shortest job to be the first
  int lowestIndex = 0; //temporarily set the 0th index to be the selected one
  for(int i=1; i<queueSize; i++){
    //find the true shortest job
    if (mpcb[i].burstOrPriority < lowest){
      lowestIndex = i;
      lowest = mpcb[i].burstOrPriority;
    }
  }
  //set the miniPCB that is to be returned
  MiniPCB toReturn = mpcb[lowestIndex];
  //shift all the processes after the selected one over to "fill the gap"
  for(int i=lowestIndex+1; i<queueSize; i++){
    mpcb[i-1] = mpcb [i];
  }
  MiniPCB mp; //empty pcb
  mpcb[queueSize-1] = mp; //set the last slot in the queue to be the empty pcb
  queueSize--; //"remove last element in the queue" by making it not accessable

  return toReturn;
}

//priority scheduler
MiniPCB Priority(MiniPCB mpcb[]){
  //if there is only one process in the queue return it and remove it from ready queue
  if (queueSize == 1){
    queueSize--; //remove the process from the queue
    return mpcb[0];
  }

  int highest = mpcb[0].burstOrPriority; //temporarily set the highest priority job to be the first
  int highestIndex = 0;//temporarily set the 0th index to be the selected one

  for(int i=1; i<queueSize; i++){
    //find the true highest priority job
    if (mpcb[i].burstOrPriority > highest){
      highestIndex = i;
      highest = mpcb[i].burstOrPriority;
    }
  }
  //set the miniPCB that is to be returned
  MiniPCB toReturn = mpcb[highestIndex];
  //shift all the processes after the selected one over to "fill the gap"
  for(int i=highestIndex+1; i<queueSize; i++){
    mpcb[i-1] = mpcb [i];
  }
  MiniPCB mp; //empty pcb
  mpcb[queueSize-1] = mp; //set the last slot in the queue to be the empty pcb
  queueSize--; //"remove last element in the queue" by making it not accessable

  return toReturn;
}

//lock
pthread_mutex_t lock;

/*
Send function
  applies the lock
  sends the miniPCB of the process to the messageQueue and puts it in the next available index
  increases the processesSent counter
  realeases the lock
*/
void send(MiniPCB mpcb, MiniPCB queue[]){
  pthread_mutex_lock(&lock); //lock before actually sending the miniPCB
    queue[processesSent] = mpcb;
    processesSent++;
  pthread_mutex_unlock(&lock); //once process sent and processesSent is updated unlock
}

/*
Receive function
  gets the next miniPCB based on processesRcv counter
  increases processesRcv counter
  returns the miniPCB
*/
MiniPCB rcv(MiniPCB queue[]){
  MiniPCB toReturn;
  toReturn = queue[processesRcv];
  processesRcv++;
  return toReturn;
}

//ready queue
MiniPCB readyQueue[15];
//FIFO message queue
MiniPCB messageQueue[15];
/*create function pointer array - FCFS SJF PRIORITY returns minipcb of process to be executed*/
MiniPCB (*schedule[])(MiniPCB[]) = {FCFS, SJF, Priority};
/*create function pointers array - sum product power fibonacci*/
int (*funcPtr[])(int, int) = {sum, product, power, fibonacci};

/*
Scheduler-Dispatcher thread function
  runs once for each of the 15 processes
  sets the return value in the miniPCB after executing the function
  sends the miniPCB to the messageQueue
*/
void* sdFunc(void* arg){
  for (int process=1; process<=15; process++){
    running = (*schedule[s])(readyQueue); //apply correct scheduler function on ready queue to return next process
    running.returnValue = (*funcPtr[running.opNum])(running.param1, running.param2);
    send(running,messageQueue);
  }
  return 0;
}

/*
Logger thread function
  creates the ouput file
  logs once for each process
  locks before calling the rcv function
  iterates through the messageQueue with the rcv function
  writes data from each mesaage to the output file
  unlocks
  runs again for the next message
*/
void* loggerFunc(void* outputFile){
  //create output file
  if ((fd = creat(outputFile, 0644)) == -1){
   printf("Can't create\n");
   return 0;
  }

  MiniPCB currentMessage; //message to be recieved

  while (processesRcv<15){ //while all processes weren't recieved
    if (processesRcv<processesSent) { //check that logger isn't ahead of sd function
      pthread_mutex_lock(&lock); //lock before calling rcv function
      currentMessage = rcv(messageQueue);

      //log to file
        char* temp=toAsc(currentMessage.processNumber);
        write(fd, temp, strlen(temp));
      if (currentMessage.opNum == 0) {
          write(fd, ", sum, ", 7);
        }
      else if (currentMessage.opNum == 1) {
          write(fd, ", product, ", 11);
        }
      else if (currentMessage.opNum == 2) {
          write(fd, ", power, ", 9);
        }
      else if (currentMessage.opNum == 3) {
          write(fd, ", fibonacci, ", 12);
        }
        temp=toAsc(currentMessage.param1);
        write(fd, temp, strlen(temp));
        write(fd, ", ", 2);
        temp=toAsc(currentMessage.param2);
        write(fd, temp, strlen(temp));
        write(fd, ", ", 2);
        temp=toAsc(currentMessage.returnValue);
        write(fd, temp, strlen(temp));
        write(fd, "\n", 1);

      pthread_mutex_unlock(&lock);//unlock after finishing writing
    }
  }
  return 0;
}


int main(int argc, const char *argv[]) {

//populate the readyQueue based on the inputed scheduler
  if (argc != 4){
    printf("Wrong number of command line arguments\n");
    return 1;
  }
  FILE* f1;
  if ((f1 = fopen(argv[2], "r")) == NULL){
    printf("Can't open %s \n", argv[1]);
    return 2;
  }

  //read in put file line by line
  char * line = NULL;
  size_t len = 0;

  /*
  Each line should populate a miniPCB based on the desired scheduling algorithm
  the line is read in and then divided by commas and reads in the values
  for each part of the miniPCB depending on the scheduling algorithm
  */
    //FCFS algorithm
      if (strcmp(argv[1],"FCFS")==0) {
        s = 0;
        for (int i=0; i<15; i++){
          getline(&line, &len, f1);
          char* temp = strtok(line, ",");
          readyQueue[i].processNumber = atoi(temp);
          temp = strtok(NULL, ",");
          readyQueue[i].operation = temp;
          if (strcmp(temp,"fibonacci")==0) readyQueue[i].opNum =3;
          else if (strcmp(temp,"power")==0) readyQueue[i].opNum = 2;
          else if (strcmp(temp,"product")==0) readyQueue[i].opNum = 1;
          else if (strcmp(temp,"sum")==0) readyQueue[i].opNum = 0;
          temp = strtok(NULL, ",");
          readyQueue[i].param1 = atoi(temp);
          temp = strtok(NULL, ",");
          readyQueue[i].param2 = atoi(temp);
        }
      }
    //SJF algorithm
      else if (strcmp(argv[1],"SJF")==0){
        s = 1;
        for (int i=0; i<15; i++){
          getline(&line, &len, f1);
          char* temp = strtok(line, ",");
          readyQueue[i].processNumber = atoi(temp);
          temp = strtok(NULL, ",");
          readyQueue[i].burstOrPriority = atoi(temp);
          temp = strtok(NULL, ",");
          readyQueue[i].operation = temp;
          if (strcmp(temp,"fibonacci")==0) readyQueue[i].opNum =3;
          else if (strcmp(temp,"power")==0) readyQueue[i].opNum = 2;
          else if (strcmp(temp,"product")==0) readyQueue[i].opNum = 1;
          else if (strcmp(temp,"sum")==0) readyQueue[i].opNum = 0;
          temp = strtok(NULL, ",");
          readyQueue[i].param1 = atoi(temp);
          temp = strtok(NULL, ",");
          readyQueue[i].param2 = atoi(temp);
        }
      }
    //PRIORITY algorithm
      else if (strcmp(argv[1],"PRIORITY")==0){
        s = 2;
        for (int i=0; i<15; i++){
          getline(&line, &len, f1);
          char* temp = strtok(line, ",");
          readyQueue[i].processNumber = atoi(temp);
          temp = strtok(NULL, ",");
          readyQueue[i].burstOrPriority = atoi(temp);
          temp = strtok(NULL, ",");
          readyQueue[i].operation = temp;
          if (strcmp(temp,"fibonacci")==0) readyQueue[i].opNum =3;
          else if (strcmp(temp,"power")==0) readyQueue[i].opNum = 2;
          else if (strcmp(temp,"product")==0) readyQueue[i].opNum = 1;
          else if (strcmp(temp,"sum")==0) readyQueue[i].opNum = 0;
          temp = strtok(NULL, ",");
          readyQueue[i].param1 = atoi(temp);
          temp = strtok(NULL, ",");
          readyQueue[i].param2 = atoi(temp);
        }
      }

 //schedule dispatcher pthread
  pthread_t scheduleDispatcher;
  pthread_create(&scheduleDispatcher, NULL, sdFunc, NULL);

  //logger pthread
  pthread_t logger;
  pthread_create(&logger, NULL, loggerFunc, (void*)argv[3]);

    //terminate program
    close(fd);
    pthread_join(scheduleDispatcher, NULL);
    pthread_join(logger, NULL);
    pthread_mutex_destroy(&lock);
  return 0;
}
