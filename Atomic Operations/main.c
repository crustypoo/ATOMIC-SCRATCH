/*
 * main.c
 *
 *  Created on: Sep 26, 2016
 *      Author: aliu
 */

#include "atomic_functions.c"

#define NUM_THREADS 3

typedef struct entry_type{

}entry_type;

typedef struct GLOBAL_STRUCT{
	uint16_t wIndex;
	uint16_t commitIndex;
	uint16_t rIndex;
	bool r_wrapBool;
	bool w_wrapBool;
} GLOBAL_STRUCT;

void * producer_thread(void * ptr);
void * consumer_thread(void * ptr);

/*
 * function: _cas_and_add()
 * return type: bool
 * inputs:
 * 		- uint * ptr
 * 		- uint expected
 *
 * Desc: Function performs an ATOMIC Compare and Swap operation on ptr to value in expected. If CAS is successful,
 * 		 then value pointed to by ptr is incremented by one
 *
 */
bool _cas_and_increment(uint * ptr, uint expected){
	uint desired = expected++;
	bool ret = __atomic_compare_exchange(ptr, &expected, &desired, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
	return ret;
}

GLOBAL_STRUCT GS;
pthread_mutex_t p_exit;

int main(void){

	pthread_t threads[NUM_THREADS];

	uint32_t t = UINT32_MAX + (UINT32_MAX / 2);

	printf("%u\n", (UINT32_MAX + 1) - 1);

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	GS.commitIndex = 0;
	GS.wIndex = 0;
	GS.rIndex = 0;
	GS.r_wrapBool = false;
	GS.w_wrapBool = true;


	pthread_mutex_init(&p_exit, NULL);
	pthread_mutex_lock(&p_exit);

	int i;
	for(i = 0; i < NUM_THREADS; i++){
		pthread_create(&threads[i], &attr, producer_thread, (void *)&GS);
	}

	while(__atomic_load_n(&GS.wIndex, __ATOMIC_RELAXED) < UINT16_MAX){
		printf("main_wait\n");
		usleep(500);
	}

	usleep(5000);

	pthread_mutex_unlock(&p_exit);

	for(i = 0; i < NUM_THREADS; i++){
		pthread_join(threads[i], NULL);
	}

	printf("wIndex: %d\n", GS.wIndex);
	printf("w_wrapBool: %d\n", GS.w_wrapBool);

	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&p_exit);

	pthread_exit(NULL);
}


void * producer_thread(void * ptr){
	GLOBAL_STRUCT * global = (GLOBAL_STRUCT *)ptr;
	uint16_t store;
	bool _comp = true;

	/*
	 * Initial producer thread device initializations go here
	 */

	while (pthread_mutex_trylock(&p_exit)){

		/*
		 * Gather data first, if data is available to write to buffer, then go on
		 */

		uint16_t cur = __atomic_add_fetch(&global->wIndex, 1, __ATOMIC_RELAXED);
		if (cur < store){
			_comp = __atomic_xor_fetch(&global->w_wrapBool, _comp, __ATOMIC_RELAXED);
			printf("Changed: %d\n",_comp);
		}
		printf("cur: %d\n", cur);
		usleep(500);

		/*
		 * Check if write operation is safe
		 * Block until commitIndex !=
		 */

		/*
		 * Write data to claimed space
		 */

		uint16_t commit_new = cur++;

		while(__atomic_compare_exchange(&global->commitIndex, &cur, &commit_new, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED) == false){
			printf("wait\n");
		}
		printf("Commit: %d\n", commit_new);
		store = cur;
	}
	pthread_mutex_unlock(&p_exit);
	pthread_exit(NULL);
}

void * consumer_thread(void * ptr){
	GLOBAL_STRUCT * global = (GLOBAL_STRUCT *)ptr;
	uint16_t store;
	bool _comp = true;
	while(pthread_mutex_trylock(&p_exit)){
		/*
		 * Claim next read spot by incrementing read by value of 1
		 */
		uint16_t _cur = __atomic_fetch_add(&global->rIndex, 1, __ATOMIC_RELAXED);
		/*
		 * Block until _cur != commitIndex
		 */
		while (__atomic_load_n(&global->commitIndex, __ATOMIC_RELAXED) == _cur){
			/*
			 * Block while commit index = read index
			 */
		}

		/*
		 * Read and sort data function here
		 */

		store = _cur;
	}

	pthread_mutex_unlock(&p_exit);
}
