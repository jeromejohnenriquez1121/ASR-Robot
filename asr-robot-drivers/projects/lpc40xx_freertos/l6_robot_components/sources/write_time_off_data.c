#include "write_time_off_data.h"

#include "FreeRTOS.h"
#include "delay.h"
#include "queue.h"
#include "string.h"
#include <stdio.h>

#include "ff.h"

QueueHandle_t time_off_data_queue;

void write_time_off_data__freertos_task(void *parameter) {
  char line[12] = "";
  FIL file;

  while (1) {
    xQueueReceive(time_off_data_queue, &line[0], portMAX_DELAY);
    vTaskDelay(500);

    printf("Received %s\n", &line[0]);
    strcat(&line[0], "OFF\n");

    UINT bytes_written = 0;
    FRESULT f_result = f_open(&file, "asr.txt", (FA_WRITE | FA_OPEN_APPEND));

    if (f_result == FR_OK) {
      f_write(&file, &line[0], strlen(&line[0]), &bytes_written);
    }

    memset(&line[0], 0, strlen(&line[0]));

    f_close(&file);
  }
}