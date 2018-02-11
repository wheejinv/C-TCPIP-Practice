#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREAD 100

void* thread_inc(void *arg);
void* thread_des(void *arg);

long long num = 0;
// 뮤텍스의 참조 값 저장을 위한 변수. 이렇게 전역변수로 선언된 이유는 뮤텍스의 접근이
// thread_inc, thread_des 이렇게 두 함수 내에서 이뤄지기 때문.
pthread_mutex_t mutex;

int main(int argc, char *argv[])
{
  pthread_t thread_id[NUM_THREAD];
  int i;

  pthread_mutex_init(&mutex, NULL);

  for (i = 0; i < NUM_THREAD; i++) {
    if (i % 2) {
      pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
    } else {
      pthread_create(&(thread_id[i]), NULL, thread_des, NULL);
    }
  }

  for (i = 0; i < NUM_THREAD; i++) {
    pthread_join(thread_id[i], NULL);
  }

  printf("result: %lld \n", num);
  // 뮤텍스의 소멸
  pthread_mutex_destroy(&mutex);

  return 0;
}

void* thread_inc(void *arg)
{
  int i;

  // 실제 임계영역은 한줄이지만, 여기서는 반목분까지 임계영역으로 포함해서
  // lock, unlock 함수를 호출하고 있다.
  pthread_mutex_lock(&mutex);

  for (i = 0; i < 50000; i++) {
    num += 1;
  }
  pthread_mutex_unlock(&mutex);
  return NULL;
}

void* thread_des(void *arg)
{
  int i;

  for (i = 0; i < 50000; i++) {
    pthread_mutex_lock(&mutex);
    // 임계 영역에 해당하는 한줄만 뮤텍스의 lock, unlock 함수로 감싸고 있음.
    num -= 1;
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}
