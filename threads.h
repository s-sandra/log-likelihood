#ifndef threads_H
#define threads_H

#include <pthread.h>
#include <assert.h>
#include <stdlib.h>

void Pthread_create(pthread_t* thread, const pthread_attr_t* attr, 	 
		            void* (*func)(void*), void* arg) {
    int thr = pthread_create(thread, attr, func, arg);
    assert(thr == 0);
}

void Pthread_join(pthread_t thread, void** retval) {
    int thr = pthread_join(thread, retval);
    assert(thr == 0);
}

void Pthread_rwlock_init(pthread_rwlock_t* lock, const pthread_rwlockattr_t* attr) {
    int lck = pthread_rwlock_init(lock, attr);
    assert(lck == 0);
}

void Pthread_rwlock_unlock(pthread_rwlock_t* lock) {
    int lck = pthread_rwlock_unlock(lock);
    assert(lck == 0);
}

void Pthread_rwlock_wrlock(pthread_rwlock_t* lock) {
    int lck = pthread_rwlock_wrlock(lock);
    assert(lck == 0);
}

void Pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* attr) {
    int mtx = pthread_mutex_init(mutex, attr);
    assert(mtx == 0);
}

void Pthread_mutex_lock(pthread_mutex_t* mutex) {
    int lock = pthread_mutex_lock(mutex);
    assert(lock == 0);
}
                                                                                
void Pthread_mutex_unlock(pthread_mutex_t* mutex) {
    int lock = pthread_mutex_unlock(mutex);
    assert(lock == 0);
}
                                                                            
#endif /* threads_H */
