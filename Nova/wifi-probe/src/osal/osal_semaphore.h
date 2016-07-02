#ifndef _osal_semaphore_
#define _osal_semaphore_

#include <pthread.h>

/* Binary Semaphores */
typedef struct
{
    int             free;
    pthread_mutex_t id;     /*�����ź���*/
    pthread_cond_t  cv;     /*��������*/
    char            name [OSAL_MAX_API_NAME];
    int             creator;
    int             max_value;  /*SEM_FULL.*/
    int             current_value; /*��ǰֵ��SEM_EMPTY, SEM_FULL*/
}OSAL_bin_sem_record_t;


/* Mutexes */
typedef struct
{
    int             free;
    pthread_mutex_t id;     /*pthread mutex id.*/
    char            name [OSAL_MAX_API_NAME];
    int             creator;    /*�ĸ����񴴽�*/
    int             lock_count; /*Ŀǰ�ж��ٸ�lock Take.*/
    
}OSAL_mut_sem_record_t;

#endif

