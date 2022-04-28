#include <stdio.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "board_io.h"
#include "common_macros.h"
#include "periodic_scheduler.h"
#include "sj2_cli.h"

#include "pir.h"
#include "string.h"
#include "write_time_off_data.h"
#include "write_time_on_data.h"

// Public Queue Variables:
QueueHandle_t time_on_data_queue;
QueueHandle_t time_off_data_queue;

// Private function declarations:
static void init_queues(void) {
  time_on_data_queue = xQueueCreate(1, sizeof(char[12]));
  time_off_data_queue = xQueueCreate(1, sizeof(char[12]));
}

/************************************************************************************************************/
/********************************************* Main Function ************************************************/
/************************************************************************************************************/

int main(void) {
  sj2_cli__init();
  fprintf(stderr, "SJTwo init\n");

  init_queues();
  fprintf(stderr, "Initialized queues\n");

  xTaskCreate(pir__freertos_task, "PIR task", 4096 / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(write_time_on_data__freertos_task, "Writes to SD card", 4096 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);
  xTaskCreate(write_time_off_data__freertos_task, "Writes to SD card", 4096 / sizeof(void *), NULL, PRIORITY_HIGH,
              NULL);

  vTaskStartScheduler();

  return 1; // Should never return
}
