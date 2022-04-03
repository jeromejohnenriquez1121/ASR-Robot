#pragma once

#include <stdint.h>

typedef struct {
  char *time[5];
  int hours;
  int mins;
} time_duration_s;

time_duration_s get_time_duration(uint64_t ticks);