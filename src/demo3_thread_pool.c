/**************************************************************
 * Description: Thread Pool
 * Date:
 * Others: 
**************************************************************/

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "../include/demo3_thread_pool.h"

//#define THREAD_POOL_DEBUG

#ifdef THREAD_POOL_DEBUG
#define DEMO_THREAD_Print(...) fprintf (stderr,"line %d - ",__LINE__); fprintf (stderr, __VA_ARGS__); fprintf (stderr,"\n")
#else
#define DEMO_THREAD_Print(...)
#endif /* THREAD_POOL_DEBUG */

#define THREAD_POOL_QUEUE_SIZE 10000
struct threadpool_task
{
	void (*routine_cb)(void*);
	void *data;
};

struct threadpool_queue
{
	unsigned int head;
	unsigned int tail;

	unsigned int num_of_cells;

	void *cells[THREAD_POOL_QUEUE_SIZE];
};

struct threadpool
{
	struct threadpool_queue tasks_queue;
	struct threadpool_queue free_tasks_queue;

	struct threadpool_task tasks[THREAD_POOL_QUEUE_SIZE];

	pthread_t *thr_arr;

	unsigned short num_of_threads;
	volatile unsigned short stop_flag;

	pthread_mutex_t free_tasks_mutex;
	pthread_mutex_t mutex;
	pthread_cond_t free_tasks_cond;
	pthread_cond_t cond;
};

// test case
static pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
static int count;
void fast_task(void *ptr)
{
	int *pval = (int*)ptr;
	int i;

	for (i = 0; i < 1000; i++) {
		(*pval)++;
	}

	pthread_mutex_lock(&count_mutex);

	count++;

	pthread_mutex_unlock(&count_mutex);
}
void slow_task(void *ptr)
{
	printf("slow task: count value is %d.\n",count);

	pthread_mutex_lock(&count_mutex);

	count++;

	pthread_mutex_unlock(&count_mutex);
}

#define ARR_SIZE 1000000
void demo3_thread_pool_test()
{
	struct threadpool *pool;
	int arr[ARR_SIZE], i, ret, failed_count = 0;

	for (i = 0; i < ARR_SIZE; i++) {
		arr[i] = i;
	}

	if ((pool = threadpool_init(10)) == NULL) {
		printf("Error! Failed to create a thread pool struct.\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < ARR_SIZE; i++) {
		if (i % 10000 == 0) {
			/* blocking. */
			ret = threadpool_add_task(pool,slow_task,arr + i,1);
		}
		else {
			/* non blocking. */
			ret = threadpool_add_task(pool,fast_task,arr + i,0);
		}

		if (ret == -1) {
			printf("An error had occurred while adding a task.");
			exit(EXIT_FAILURE);
		}

		if (ret == -2) {
			failed_count++;
		}
	}

	threadpool_free(pool,1);
}


static void threadpool_queue_init(struct threadpool_queue *queue)
{
	int i;

	for (i = 0; i < THREAD_POOL_QUEUE_SIZE; i++)
	{
		queue->cells[i] = NULL;
	}

	queue->head = 0;
	queue->tail = 0;
	queue->num_of_cells = 0;
}

/* queue 尾添加 */
static int threadpool_queue_enqueue(struct threadpool_queue *queue, void *data)
{
	if (queue->num_of_cells == THREAD_POOL_QUEUE_SIZE) {
		DEMO_THREAD_Print("The queue is full, unable to add data to it.");
		return -1;
	}

	if (queue->cells[queue->tail] != NULL) {
		DEMO_THREAD_Print("A problem was detected in the queue (expected NULL, but found a different value).");
		return -1;
	}

	queue->cells[queue->tail] = data;

	queue->num_of_cells++;
	queue->tail++;

	if (queue->tail == THREAD_POOL_QUEUE_SIZE) {
		queue->tail = 0;
	}

	return 0;
}
/* queue头部取出 */
static void *threadpool_queue_dequeue(struct threadpool_queue *queue)
{
	void *data;

	if (queue->num_of_cells == 0) {
			DEMO_THREAD_Print("Tried to dequeue from an empty queue.");
			return NULL;
	}

	data = queue->cells[queue->head];

	queue->cells[queue->head] = NULL;
	queue->num_of_cells--;

	if (queue->num_of_cells == 0) {
		queue->head = 0;
		queue->tail = 0;
	}
	else {
		queue->head++;
		if (queue->head == THREAD_POOL_QUEUE_SIZE) {
			queue->head = 0;
		}
	}

	return data;
}

static int threadpool_queue_is_empty(struct threadpool_queue *queue)
{
	if (queue->num_of_cells == 0) {
		return 1;
	}

	return 0;
}

static int threadpool_queue_getsize(struct threadpool_queue *queue)
{
	return queue->num_of_cells;
}

static void threadpool_task_init(struct threadpool_task *task)
{
	task->data = NULL;
	task->routine_cb = NULL;
}

static struct threadpool_task* threadpool_task_get_task(struct threadpool *pool)
{
	struct threadpool_task* task;

	if (pthread_mutex_lock(&(pool->mutex))) {
		perror("pthread_mutex_lock: ");
		return NULL;
	}

	while (threadpool_queue_is_empty(&(pool->tasks_queue))) {
		if (pool->stop_flag)
		{
			if (pthread_cond_broadcast(&(pool->cond))) {
				perror("pthread_cond_broadcast: ");
			}

			if (pthread_mutex_unlock(&(pool->mutex))) {
				perror("pthread_mutex_unlock: ");
				return NULL;
			}

			return NULL;
		}

		if (pthread_cond_wait(&(pool->cond),&(pool->mutex))) {
			perror("pthread_cond_wait: ");
			if (pthread_mutex_unlock(&(pool->mutex))) {
				perror("pthread_mutex_unlock: ");
			}

			return NULL;
		}
	}

	if ((task = (struct threadpool_task*)threadpool_queue_dequeue(&(pool->tasks_queue))) == NULL) {
		DEMO_THREAD_Print("Failed to obtain a task from the jobs queue.");
	}

	if (pthread_mutex_unlock(&(pool->mutex))) {
		perror("pthread_mutex_unlock: ");
		return NULL;
	}

	return task;
}

static void *worker_thr_routine(void *data)
{
	struct threadpool *pool = (struct threadpool*)data;
	struct threadpool_task *task;

	while (1) {
		task = threadpool_task_get_task(pool);
		if (task == NULL) {
			if (pool->stop_flag) {
				break;
			}
			else {
				DEMO_THREAD_Print("Warning an error has occurred when trying to obtain a worker task.");
				DEMO_THREAD_Print("The worker thread has exited.");
				break;
			}
		}

		if (task->routine_cb) {
			task->routine_cb(task->data);

			threadpool_task_init(task);
			if (pthread_mutex_lock(&(pool->free_tasks_mutex))) {
				perror("pthread_mutex_lock: ");
				DEMO_THREAD_Print("The worker thread has exited.");
				break;
			}

			if (threadpool_queue_enqueue(&(pool->free_tasks_queue),task)) {
				DEMO_THREAD_Print("Failed to enqueue a task to free tasks queue.");
				if (pthread_mutex_unlock(&(pool->free_tasks_mutex))) {
					perror("pthread_mutex_unlock: ");
				}

				DEMO_THREAD_Print("The worker thread has exited.");
				break;
			}

			if (threadpool_queue_getsize(&(pool->free_tasks_queue)) == 1) {
				if (pthread_cond_broadcast(&(pool->free_tasks_cond))) {
					perror("pthread_cond_broadcast: ");
					if (pthread_mutex_unlock(&(pool->free_tasks_mutex))) {
						perror("pthread_mutex_unlock: ");
					}

					break;
				}
			}

			if (pthread_mutex_unlock(&(pool->free_tasks_mutex))) {
				perror("pthread_mutex_unlock: ");
				DEMO_THREAD_Print("The worker thread has exited.");
				break;
			}
		}
	}

	return NULL;
}

static void *stop_worker_thr_routines_cb(void *ptr)
{
	int i;
	struct threadpool *pool = (struct threadpool*)ptr;

	if (pthread_mutex_lock(&(pool->mutex))) {
		perror("pthread_mutex_lock: ");
		DEMO_THREAD_Print("Warning: Memory was not released.");
		DEMO_THREAD_Print("Warning: Some of the worker threads may have failed to exit.");
		return NULL;
	}

	pool->stop_flag = 1;

	while (!threadpool_queue_is_empty(&(pool->tasks_queue))) {
		if (pthread_cond_wait(&(pool->cond),&(pool->mutex))) {
			perror("pthread_cond_wait: ");
			if (pthread_mutex_unlock(&(pool->mutex))) {
				perror("pthread_mutex_unlock: ");
			}

			return NULL;
		}
	}

    /* 唤醒 */
	if (pthread_cond_broadcast(&(pool->cond))) {
		perror("pthread_cond_broadcast: ");
		DEMO_THREAD_Print("Warning: Memory was not released.");
		DEMO_THREAD_Print("Warning: Some of the worker threads may have failed to exit.");
		return NULL;
	}

	if (pthread_mutex_unlock(&(pool->mutex))) {
		perror("pthread_mutex_unlock: ");
		DEMO_THREAD_Print("Warning: Memory was not released.");
		DEMO_THREAD_Print("Warning: Some of the worker threads may have failed to exit.");
		return NULL;
	}

	for (i = 0; i < pool->num_of_threads; i++) {
		if (pthread_join(pool->thr_arr[i],NULL)) {
			perror("pthread_join: ");
		}
	}

	/* 释放 */
	free(pool->thr_arr);
	free(pool);

	return NULL;
}

struct threadpool* threadpool_init(int num_of_threads)
{
	struct threadpool *pool;
	int i;

	/* Create the thread pool struct. */
	if ((pool = (struct threadpool*)malloc(sizeof(struct threadpool))) == NULL) {
		perror("malloc: ");
		return NULL;
	}

	pool->stop_flag = 0;

	/* Init the mutex and cond vars. */
	if (pthread_mutex_init(&(pool->free_tasks_mutex),NULL)) {
		perror("pthread_mutex_init: ");
		free(pool);
		return NULL;
	}
	if (pthread_mutex_init(&(pool->mutex),NULL)) {
		perror("pthread_mutex_init: ");
		free(pool);
		return NULL;
	}
	if (pthread_cond_init(&(pool->free_tasks_cond),NULL)) {
		perror("pthread_mutex_init: ");
		free(pool);
		return NULL;
	}
	if (pthread_cond_init(&(pool->cond),NULL)) {
		perror("pthread_mutex_init: ");
		free(pool);
		return NULL;
	}

	/* Init the jobs queue. */
	threadpool_queue_init(&(pool->tasks_queue));

	/* Init the free tasks queue. */
	threadpool_queue_init(&(pool->free_tasks_queue));
	/* Add all the free tasks to the free tasks queue. */
	for (i = 0; i < THREAD_POOL_QUEUE_SIZE; i++) {
		threadpool_task_init((pool->tasks) + i);
		if (threadpool_queue_enqueue(&(pool->free_tasks_queue),(pool->tasks) + i)) {
			DEMO_THREAD_Print("Failed to a task to the free tasks queue during initialization.");
			free(pool);
			return NULL;
		}
	}

	/* Create the thr_arr. */
	if ((pool->thr_arr = (pthread_t*)malloc(sizeof(pthread_t) * num_of_threads)) == NULL) {
		perror("malloc: ");
		free(pool);
		return NULL;
	}

	/* Start the worker threads. */
	for (pool->num_of_threads = 0; pool->num_of_threads < num_of_threads; (pool->num_of_threads)++) {
		if (pthread_create(&(pool->thr_arr[pool->num_of_threads]),NULL,worker_thr_routine,pool)) {
			perror("pthread_create:");

			threadpool_free(pool,0);

			return NULL;
		}
	}

	return pool;
}

int threadpool_add_task(struct threadpool *pool, void (*routine)(void*), void *data, int blocking)
{
	struct threadpool_task *task;

	if (pool == NULL) {
		DEMO_THREAD_Print("The threadpool received as argument is NULL.");
		return -1;
	}

	if (pthread_mutex_lock(&(pool->free_tasks_mutex))) {
		perror("pthread_mutex_lock: ");
		return -1;
	}

	/* Check if the free task queue is empty. */
	while (threadpool_queue_is_empty(&(pool->free_tasks_queue))) {
		if (!blocking) {
			/* Return immediately if the command is non blocking. */
			if (pthread_mutex_unlock(&(pool->free_tasks_mutex))) {
				perror("pthread_mutex_unlock: ");
				return -1;
			}

			return -2;
		}

		/* blocking is set to 1, wait until free_tasks queue has a task to obtain. */
		if (pthread_cond_wait(&(pool->free_tasks_cond),&(pool->free_tasks_mutex))) {
			perror("pthread_cond_wait: ");
			if (pthread_mutex_unlock(&(pool->free_tasks_mutex))) {
				perror("pthread_mutex_unlock: ");
			}

			return -1;
		}
	}

	/* Obtain an empty task. */
	if ((task = (struct threadpool_task*)threadpool_queue_dequeue(&(pool->free_tasks_queue))) == NULL) {
		DEMO_THREAD_Print("Failed to obtain an empty task from the free tasks queue.");
		if (pthread_mutex_unlock(&(pool->free_tasks_mutex))) {
			perror("pthread_mutex_unlock: ");
		}

		return -1;
	}

	if (pthread_mutex_unlock(&(pool->free_tasks_mutex))) {
		perror("pthread_mutex_unlock: ");
		return -1;
	}

	task->data = data;
	task->routine_cb = routine;

	/* Add the task, to the tasks queue. */
	if (pthread_mutex_lock(&(pool->mutex))) {
		perror("pthread_mutex_lock: ");
		return -1;
	}

	if (threadpool_queue_enqueue(&(pool->tasks_queue),task)) {
		DEMO_THREAD_Print("Failed to add a new task to the tasks queue.");
		if (pthread_mutex_unlock(&(pool->mutex))) {
			perror("pthread_mutex_unlock: ");
		}
		return -1;
	}

	if (threadpool_queue_getsize(&(pool->tasks_queue)) == 1) {
		/* Notify all worker threads that there are new jobs. */
		if (pthread_cond_broadcast(&(pool->cond))) {
			perror("pthread_cond_broadcast: ");
			if (pthread_mutex_unlock(&(pool->mutex))) {
				perror("pthread_mutex_unlock: ");
			}

			return -1;
		}
	}

	if (pthread_mutex_unlock(&(pool->mutex))) {
		perror("pthread_mutex_unlock: ");
		return -1;
	}

	return 0;
}

void threadpool_free(struct threadpool *pool, int blocking)
{
	pthread_t thr;

	if (blocking) {
		stop_worker_thr_routines_cb(pool);
	}
	else {
		if (pthread_create(&thr,NULL,stop_worker_thr_routines_cb,pool)) {
			perror("pthread_create: ");
			DEMO_THREAD_Print("Warning! will not be able to free memory allocation. This will cause a memory leak.");
			pool->stop_flag = 1;
		}
	}
}
