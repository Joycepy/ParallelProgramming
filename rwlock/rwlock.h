#include <pthread.h>//多线程、互斥锁所需头文件

typedef struct rwl_t{
    pthread_mutex_t mutex;

    pthread_cond_t read;
    pthread_cond_t write;

    int read_now;
    int read_wait;
    int write_now;
    int write_wait;
};

void rwl_init(rwl_t* rwlock);
void rwl_destroy(rwl_t* rwlock);
void rwl_rdlock(rwl_t* rwlock);
void rwl_wrlock(rwl_t* rwlock);
void rwl_unlock(rwl_t* rwlock);
