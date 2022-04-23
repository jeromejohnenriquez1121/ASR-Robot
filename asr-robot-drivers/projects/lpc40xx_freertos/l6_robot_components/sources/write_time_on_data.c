#include "write_time_on_data.h"
#include "ff.h"

#include "FreeRTOS.h"
#include "queue.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

QueueHandle_t time_on_data_queue;

void write_time_on_data__freertos_task(void *parameter) {
  char line[10] = "";
  FIL file;

  while (1) {
    xQueueReceive(time_on_data_queue, &line[0], portMAX_DELAY);
    strcat(&line[0], "ON\n");

    UINT bytes_written = 0;
    FRESULT f_result = f_open(&file, "asr.txt", (FA_WRITE | FA_OPEN_APPEND));

    if (f_result == FR_OK) {
      f_write(&file, &line[0], strlen(line), &bytes_written);
    }

    memset(&line[0], 0, strlen(line));

    f_close(&file);
  }
}
