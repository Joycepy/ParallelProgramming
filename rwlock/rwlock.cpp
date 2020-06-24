#include "rwlock.h"
#include <stdio.h>
void rwl_init(rwl_t* rwlock){
    //初始化互斥锁
    pthread_mutex_init(&(rwlock->mutex), NULL);
    //初始化条件变量
    pthread_cond_init(&(rwlock->read), NULL);
    pthread_cond_init(&(rwlock->write), NULL);

    rwlock->read_now = 0;
    rwlock->read_wait = 0;
    rwlock->write_now = 0;
    rwlock->write_wait = 0;
}

void rwl_destroy(rwl_t* rwlock){
    pthread_mutex_destroy(&(rwlock->mutex));
    pthread_cond_destroy(&(rwlock->read));
    pthread_cond_destroy(&(rwlock->write));
}

void rwl_rdlock(rwl_t* rwlock){
    pthread_mutex_lock(&(rwlock->mutex));
    printf("Try readLock.\nThe number of write_now: %d\n", rwlock->write_now);
    if(rwlock->write_now == 0) rwlock->read_now++;
    else{
        rwlock->read_wait++;
        printf("=====Read Start Wait=====\n");
        pthread_cond_wait(&(rwlock->read), &(rwlock->mutex));
        printf("=====Read Finish Wait=====\n");
        rwlock->read_wait--;
        rwlock->read_now++;
    }
    printf("The number of read_now: %d\n", rwlock->read_now);
    pthread_mutex_unlock(&(rwlock->mutex));
}

void rwl_wrlock(rwl_t* rwlock){
    pthread_mutex_lock(&(rwlock->mutex));//加锁
    //No write and read
    printf("Try writeLock.\nThe number of write_now: %d\n", rwlock->write_now);
    if(rwlock->read_now == 0 && rwlock->write_now == 0) rwlock->write_now++;
    else{
        rwlock->write_wait++;
        printf("=====Write Start Wait=====\n");
        pthread_cond_wait(&(rwlock->write),&(rwlock->mutex));//等待条件变量的成立
        printf("=====Write Finish Wait=====\n");
        rwlock->write_wait--;
        rwlock->write_now++;
    }
    pthread_mutex_unlock(&(rwlock->mutex));
}

void rwl_unlock(rwl_t* rwlock){
    pthread_mutex_lock(&(rwlock->mutex));
    if(rwlock->read_now > 1) rwlock->read_now--;
    else if (rwlock->read_now == 1){
        rwlock->read_now--;
        if(rwlock->write_wait > 0) pthread_cond_signal(&(rwlock->write));
    }else{
        rwlock->write_now--;
        //唤醒其他因条件变量而产生的阻塞
        if(rwlock->read_wait > 0) pthread_cond_broadcast(&(rwlock->read));
        else if(rwlock->write_wait > 0) pthread_cond_signal(&rwlock->write);
    }
    pthread_mutex_unlock(&(rwlock->mutex));
}
