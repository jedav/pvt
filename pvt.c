// psychomotor vigilance task
// see https://en.wikipedia.org/wiki/Psychomotor_vigilance_task
// and https://humanresearchroadmap.nasa.gov/evidence/reports/Sleep.pdf
// performance of > 500ms is a fault, <100 ms is a false start
// 250ms is roughly human average reaction time to visual stimulus
// roughly 2-10 seconds between tasks firing

#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/errno.h>

uint64_t results[10000];

uint64_t gettime_micros(void) {
  struct timespec tick;
  clock_gettime(CLOCK_MONOTONIC_RAW, &tick);
  return tick.tv_sec * 1000000 + tick.tv_nsec / 1000;
}

void rand_sleep(void) {
  int delay = rand() % 8000000;
  delay += 2000000;
  usleep(delay);
}


uint64_t test_user(void) {
  char c;
  puts("Go!");
  uint64_t start_time = gettime_micros();
  read(0, &c, 1u);
  uint64_t stop_time = gettime_micros();
  uint64_t result = stop_time - start_time;
  printf("\nElapsed time: %llu us\n", result);
  return result;
}


int main(int argc, char **argv) {
  uint64_t duration = 10u;
  unsigned int false_starts = 0u;
  if (argc >= 2) {
    duration = strtoull(argv[1], NULL, 10);
    if (duration == EINVAL || duration == ERANGE) {
      printf("Usage: %s length_of_test_in_minutes\n", argv[0]);
      exit(1);
    }
  }
  duration *= 60u;  // minutes, not seconds
  srand(time(NULL));
  int res_idx = 0;
  uint64_t start_time = gettime_micros();
  uint64_t curr_time = gettime_micros();
  while ((curr_time - start_time) < duration * 1000000u) {
    rand_sleep();
    uint64_t restime = test_user();
    if (restime >= 100000u) {
      results[res_idx++] = restime;
    } else {
      puts("False start");
      false_starts += 1;
    }
    curr_time = gettime_micros();
  }
  int i=0;
  for(i=0;i<res_idx;i++) {
    dprintf(2, "%llu\n", results[i]);
  }
  if (false_starts > 0) {
    dprintf(2, "False starts: %lu\n", false_starts);
  }

}

