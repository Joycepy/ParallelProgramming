#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "ListNode.h"
#define NUM_THREADS 16
#define NUM_ITER 10

typedef struct{
	int	threadId;
    Node* listHead;
    rwl_t* rwl_;
} threadParm_t;
int n_iter = NUM_ITER;

void *rwl_Find(void *parm)
{
    threadParm_t *p = (threadParm_t *) parm;
    int r = p->threadId;
    Node* ListHead = p->listHead;
    rwl_t* rwl_ = p->rwl_;
    for(int i = 0; i < n_iter; i++){
        int x = rand()% (10);
        rw_Find(ListHead, rwl_, x);
        //printf("Thread: %d, In Iter: %d, find %d\n",r,i,x);
    }
    pthread_exit(NULL);
}

void *rwl_Insert(void *parm)
{
    threadParm_t *p = (threadParm_t *) parm;
    int r = p->threadId;
    Node* ListHead = p->listHead;
    rwl_t* rwl_ = p->rwl_;
    for(int i = 0; i < n_iter; i++){
        int x = rand()% (10000);
        rw_Insert(ListHead, rwl_, x, 1);
        //printf("In Iter: %d, Insert %d\n",i,x);
    }
    pthread_exit(NULL);
}

void *rwl_Delete(void *parm)
{
    threadParm_t *p = (threadParm_t *) parm;
    int r = p->threadId;
    Node* ListHead = p->listHead;
    rwl_t* rwl_ = p->rwl_;
    for(int i = 0; i < n_iter; i++){
        rw_Delete(ListHead, rwl_, 1);
    }
    pthread_exit(NULL);
}

int main(){
    Node* dummy_head = (Node*) malloc(sizeof(Node));
    rwl_t * rwl_ = (rwl_t *) malloc(sizeof(rwl_t));
    rwl_init(rwl_);
    pthread_t thread[NUM_THREADS];
    threadParm_t threadParm[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++){
        threadParm[i].threadId = i;
        threadParm[i].listHead = dummy_head;
        threadParm[i].rwl_ = rwl_;
        int isSucc;
        if((i % 2) == 0){
            isSucc = pthread_create(&thread[i], NULL, rwl_Find, (void *)&threadParm[i]);
        }
        if((i % 4) == 1){
            isSucc = pthread_create(&thread[i], NULL, rwl_Insert, (void *)&threadParm[i]);
        }
        if((i % 4) == 3){
            isSucc = pthread_create(&thread[i], NULL, rwl_Delete, (void *)&threadParm[i]);
        }
        if(isSucc != 0){
            printf("thread[%d] fail to be created\n",isSucc);
        }
    }
	for (int i=0; i<NUM_THREADS; i++)
		pthread_join(thread[i], NULL);
    rwl_destroy(rwl_);
    return 0;
}
