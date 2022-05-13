#include <stdio.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "board_io.h"
#include "common_macros.h"
#include "ff.h"
#include "gpio.h"
#include "periodic_scheduler.h"
#include "sj2_cli.h"

#include "delay.h"
#include "new_pir.h"
#include "pir.h"
#include "string.h"
#include "write_time_off_data.h"
#include "write_time_on_data.h"

/************************************************************************************************************/
/********************************************* Main Function ************************************************/
/************************************************************************************************************/

int main(void) {
  sj2_cli__init();
  fprintf(stderr, "SJTwo init\n");

  xTaskCreate(pir_write__freertos_task, "Writes to SD card", 4096 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);

  vTaskStartScheduler();

  return 1; // Should never return
}
