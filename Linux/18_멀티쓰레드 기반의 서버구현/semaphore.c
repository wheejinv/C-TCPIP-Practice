#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void* read(void *arg);
void* accu(void *arg);
static sem_t sem_one;
static sem_t sem_two;
static int   num;

int main(int argc, char *argv[])
{
  pthread_t id_t1, id_t2;

  // 세마포어를 두 개 생성. 하나는 세마포어 값이 0이고, 다른 하나는 1이다.
  // 두 개의 세마포어가 필요한 이유를 이해해야 함.
  sem_init(&sem_one, 0, 0);
  sem_init(&sem_two, 0, 1);

  pthread_create(&id_t1, NULL, read, NULL);
  pthread_create(&id_t2, NULL, accu, NULL);

  pthread_join(id_t1, NULL);
  pthread_join(id_t2, NULL);

  sem_destroy(&sem_one);
  sem_destroy(&sem_two);
  return 0;
}

void* read(void *arg)
{
  int i;

  for (i = 0; i < 5; i++) {
    fputs("Input num: ", stdout);

    // sem_two -1 감소
    sem_wait(&sem_two);
    scanf("%d", &num);
    sem_post(&sem_one); // 1 증가
  }
  return NULL;
}

void* accu(void *arg)
{
  int sum = 0, i;

  for (i = 0; i < 5; i++) {
    // sem_one이 0인 경우 값이 반환되지 않으므로 블로킹 상태에 놓임.
    sem_wait(&sem_one);
    sum += num;
    sem_post(&sem_two);
  }
  printf("Result: %d \n", sum);
  return NULL;
}
