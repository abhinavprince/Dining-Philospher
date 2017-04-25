#include<stdio.h>
#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>

// Number of philosphers
#define N 5

// States of philosphers
#define THINKING 0
#define HUNGRY 1
#define EATING 2

// Neighbouring philosphers
#define LEFT (ph_num+4)%N
#define RIGHT (ph_num+1)%N

// Semophore to control access to execute take_fork() and put_fork() 
sem_t mutex;

// Semophore for philosphers
sem_t S[N];

// Eat count for individual philosphers
int count[5];

// Total food count
int FOOD = 0;

// creates thread for each philospher
void * philospher(void *num);

void take_fork(int);
void put_fork(int);

// checks the condition variable for taking the fork
void test(int);

// States of each philosphers
int state[N];

// Serial number of philosphers
int phil_num[N]={0,1,2,3,4};

int main()
{
    int i;

    // Deaclaration of threads 
    pthread_t thread_id[N];

    // Initiallize Semophores
    sem_init(&mutex,0,1);
    for(i=0;i<N;i++)
        sem_init(&S[i],0,0);

    // creates threads for all philosphers
    for(i=0;i<N;i++)
    {
        pthread_create(&thread_id[i],NULL,philospher,&phil_num[i]);
       
    }

    // waits for the thread to exit
    for(i=0;i<N;i++)
        pthread_join(thread_id[i],NULL);

    // outputs food count for each philosphers
    for(i=0;i<N;i++)
	printf("Philospher %d ate %d \n",i,count[i]);
    printf("\n");
}


/***
*   args[0] : Serial number of philospher
*   checks availablity of food
*   thinks for t ms
*   takes fork and starts eating
*   puts fork
***/
void *philospher(void *num)
{   
   
    while(FOOD <= 200)
    {
        int *i = num;
        usleep(10000);
        take_fork(*i);
        put_fork(*i);
    }
}



/***
*   waits for mutex lock to avoid race condition between put_fork() and take_fork()
*   sets state = HUNGRY
*   tests for condition variable to synchronise fork distribution
*   takes fork and eat
***/
void take_fork(int ph_num)
{
    sem_wait(&mutex);
    state[ph_num] = HUNGRY;
    test(ph_num);
    sem_post(&mutex);
    sem_wait(&S[ph_num]);
    usleep(10000);
}


/***
*   checks if fork is available for philospher
*   and if fork is free then signal philospher
**/
void test(int ph_num)
{
    if (state[ph_num] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
    {
        state[ph_num] = EATING;
        usleep(20000);
        sem_post(&S[ph_num]);
    }
}


/***
*   waits for mutex lock
*   sets state = THINKING
*   increases food count
*   signal neighbours for availablity of fork
*   prints status of philosphers
*   release mutex lock
***/
void put_fork(int ph_num)
{
    sem_wait(&mutex);
    state[ph_num] = THINKING;

    count[ph_num]++;
    FOOD++;
  
    test(LEFT);
    test(RIGHT);

	printf("#Eating Count = %d \n", FOOD);
	int i;
	for(i=0;i<5;i++){
		if(state[i]==EATING)
			printf("Philosopher %d is eating\n", i);
		else if(state[i]==HUNGRY)
			printf("Philosopher %d is waiting and calling pickup()\n", i);
		else if(state[i]==THINKING)
			printf("Philosopher %d is thinking\n", i);
	}

    sem_post(&mutex);
}


















