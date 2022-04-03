#include <stdio.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "board_io.h"
#include "common_macros.h"
#include "periodic_scheduler.h"
#include "sj2_cli.h"

#include "pir.h"
#include "wifi_module.h"

// Variables:
QueueHandle_t data_queue;

static void init_queues(void) { data_queue = xQueueCreate(1, sizeof(char[5])); }

/************************************************************************************************************/
/********************************************* Main Function ************************************************/
/************************************************************************************************************/

int main(void) {
  sj2_cli__init();

  xTaskCreate(pir__freertos_task, "PIR task", 4096 / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(wifi_module__freertos_task, "Wifi Module Task", 4096 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);

  vTaskStartScheduler();

  return 1; // Should never return
}