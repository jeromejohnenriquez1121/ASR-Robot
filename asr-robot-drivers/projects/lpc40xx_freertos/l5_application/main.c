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

// Public Queue Variables:
QueueHandle_t data_queue;

// Private function declarations:
static void init_queues(void) { data_queue = xQueueCreate(1, sizeof(char[12])); }

static FIL file;
static char file_name[12] = "asr.txt";

void write_data__freertos_task(void *parameter) {
  char line[12] = "";

  printf("Start write data freertos task\n");

  while (1) {
    xQueueReceive(data_queue, &line[0], portMAX_DELAY);
    printf("Receive %s\n", &line[0]);
    delay__ms(1000);

    FRESULT fr = f_open(&file, &file_name[0], FA_OPEN_APPEND | FA_WRITE);
    printf("FRESULT %u\n", fr);
  }
}

// static FIL file;
// static char file_name[12] = "asr.txt";

// void producer(void *p) {
//   vTaskDelay(5000);
//   printf("Start send signal\n");
//   vTaskDelay(1000);
//   char line[12] = "ON 12:14";

//   while (1) {
//     xQueueSend(data_queue, &line[0], 0);
//     printf("Sent signal\n");
//     vTaskDelay(2000);
//   }
// }

// void record_data(void *p) {
//   char line[12] = "";

//   while (1) {
//     xQueueReceive(data_queue, &line[0], portMAX_DELAY);
//     printf("Received %s\n", &line[0]);
//     delay__ms(1000);
//     FRESULT fr = f_open(&file, &file_name[0], (FA_WRITE | FA_CREATE_ALWAYS));
//     printf("FRESULT: %u\n", fr);
//   }
// }

static gpio_s pir;

void test_light(void *parameter) {
  pir = gpio__construct_with_function(1, 10, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_as_output(pir);

  while (1) {
    gpio__set(pir);
    printf("Turn off light\n");
    vTaskDelay(5000);
    gpio__reset(pir);
    printf("Turn on light\n");
    vTaskDelay(5000);
  }
}

/************************************************************************************************************/
/********************************************* Main Function ************************************************/
/************************************************************************************************************/

int main(void) {
  sj2_cli__init();
  fprintf(stderr, "SJTwo init\n");

  init_queues();
  fprintf(stderr, "Initialized queues\n");

  // xTaskCreate(pir__freertos_task, "PIR task", 4096 / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(pir_write__freertos_task, "Writes to SD card", 4096 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);
  // xTaskCreate(record_data, "Test write", 4096 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);
  // xTaskCreate(test_light, "Test light", 2048 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);

  vTaskStartScheduler();

  return 1; // Should never return
}
