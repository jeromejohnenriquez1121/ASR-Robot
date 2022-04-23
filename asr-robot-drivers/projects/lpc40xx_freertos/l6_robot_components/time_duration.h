#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>

char *get_time_duration(TickType_t ticks);
