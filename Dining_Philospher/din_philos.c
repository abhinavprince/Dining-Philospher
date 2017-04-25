#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define THINK_LOOP 100000000
#define EAT_LOOP 500000000
#define NPHILO 5
#define RIGHT(philoID) ((philoID+1)%NPHILO)
#define LEFT(philoID) philoID
#define RIGHTNEIGHB(philoID) ((philoID+1)%NPHILO)
#define LEFTNEIGHB(philoID) ((philoID+NPHILO-1)%NPHILO)

void *func(int n);
void disp_philo_states();
void chopstickStates(int n);
pthread_t philosopher[NPHILO];
pthread_mutex_t mutex,chopstickLock[NPHILO];
int eatCount[NPHILO] = {0,0,0,0,0};
typedef enum{ 
	HUNGRY,
	EATING,
	THINKING
} philos_state;

typedef enum {
    FREE = 0,
    IN_USE = 1
} Sticks;

philos_state state[NPHILO];
Sticks chopstick[NPHILO];

pthread_cond_t self[NPHILO];

int main()
{
	int i,k;
	pthread_mutex_init(&mutex,NULL);
	for(i=0;i<NPHILO;i++){
		state[i] = THINKING;
		chopstick[i] = FREE;
		pthread_mutex_init(&chopstickLock[i],NULL);
		pthread_cond_init(&self[i],NULL);
	}
	for(i=0;i<NPHILO;i++)
	{
		k=pthread_create(&philosopher[i],NULL,(void *)func,(void *)i);
		if(k!=0)
		{
			printf("\n Thread creation error \n");
			exit(1);
		}
	}
	for(i=0;i<NPHILO;i++){
		pthread_join(philosopher[i], NULL);
	}
	return 0;
}
void takechopstick(int i){
	pthread_mutex_lock(&mutex);
	disp_philo_states();
	// chopstickStates(i);
	state[i] = HUNGRY;
	while(chopstick[LEFT(i)] == IN_USE || chopstick[RIGHT(i)] == IN_USE){
		pthread_cond_wait(&self[i], &mutex);
	}
	state[i] = EATING;
	chopstick[LEFT(i)] = IN_USE;
	chopstick[RIGHT(i)] = IN_USE;
	pthread_mutex_unlock(&mutex);
}
void releasechopstick(int i){
	pthread_mutex_lock(&mutex);
	disp_philo_states();
	// chopstickStates(i);
	state[i] = THINKING;
	chopstick[LEFT(i)] = FREE;
	chopstick[RIGHT(i)] = FREE;

	pthread_cond_signal(&self[LEFTNEIGHB(i)]);
	pthread_cond_signal(&self[RIGHTNEIGHB(i)]);

	pthread_mutex_unlock(&mutex);
}
void think(int n){
	int i;
	printf("%d statrted THINKING \n",n );
    for(i = 0; i < THINK_LOOP; i++) {
      
    }
	printf("%d ended THINKING \n",n );
}
void eat(int n){
	int i;
	eatCount[n]++;
	printf("%d statrted EATING \n",n );
    for(i = 0; i < EAT_LOOP; i++) {
        
    }
	printf("%d ended EATING \n",n );
}
void *func(int n)
{
	while(1){
		think(n);
		takechopstick(n);
		eat(n);
		releasechopstick(n);
	}
}
char convertStates(philos_state philoState)
{
    if(philoState == EATING) {
        return 'E';
    }
    else if(philoState == THINKING) {
        return 'T';
    }
    else {
        return 'H';
    }
    return '-'; // error
}

void disp_philo_states()
{
    int i;
    for(i = 0; i < 5; i++) {
        if(state[i] == EATING && (state[LEFTNEIGHB(i)] == EATING || state[RIGHTNEIGHB(i)] == EATING)) {
            printf("OUPS! Something went wrong...\n\n");
            break;
        }
    }

    for(i = 0; i < 5; i++) {
        printf("%d%c ", i, convertStates(state[i]));
    }
    printf("\n");
    printf("Current Eat count\n");
    for(i = 0; i < 5; i++) {
        printf("%d->%d ", i, eatCount[i]);
    }
    printf("\n\n");
}
char convertChops(Sticks s){
	if(s == FREE) return 'F';
	else return 'U';
	return '-';
}
void chopstickStates(int n){
	int i;
	printf("called by thread %d \n",n );
	for(i = 0; i < 5; i++) {
        printf("%d%c ", i, convertChops(chopstick[i]));
    }
    printf("\n\n");
}
