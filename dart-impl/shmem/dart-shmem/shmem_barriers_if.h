#ifndef SHMEM_BARRIER_IF_H_INCLUDED
#define SHMEM_BARRIER_IF_H_INCLUDED

#include <pthread.h>

#include "dart_types.h"
#include "dart_teams_impl.h"

struct sysv_barrier
{
  pthread_mutex_t  mutex;
  pthread_cond_t   cond;
  int num_waiting;
  int num_procs;
};

typedef struct sysv_barrier* sysv_barrier_t;
#define SYSV_BARRIER_NULL ((sysv_barrier_t)0)


struct sysv_team
{
  struct sysv_barrier  barr;
  dart_team_t          teamid;
  int                  inuse;
};


struct syncarea_struct
{
  pthread_mutex_t lock;
  int             shmem_key;
  dart_team_t     nextid;
  
  struct sysv_team teams[MAXNUM_TEAMS];
};

typedef struct syncarea_struct* syncarea_t;


/*
 * called by ONE process only (e.g., by dartrun
 * before launching the worker processes)
 */
int shmem_syncarea_init(int numprocs, void* shm_addr, int shmid);
int shmem_syncarea_delete(int numprocs, void* shm_addr, int shmid);

int shmem_syncarea_setaddr(void *addr);
int shmem_syncarea_get_shmid();

int shmem_syncarea_newteam(dart_team_t *teamid, int numprocs);
int shmem_syncarea_delteam(dart_team_t teamid, int numprocs);

int shmem_syncarea_findteam(dart_team_t teamid);
int shmem_syncarea_barrier_wait(int slot);

int sysv_barrier_create(sysv_barrier_t barrier, int num_procs);
int sysv_barrier_destroy(sysv_barrier_t barrier);
int sysv_barrier_await(sysv_barrier_t barrier);

#endif /* SHMEM_BARRIER_IF_H_INCLUDED */
