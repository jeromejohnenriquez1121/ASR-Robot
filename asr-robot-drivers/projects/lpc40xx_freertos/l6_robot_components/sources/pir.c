#include "pir.h"
#include "lightbulb.h"
#include "time_duration.h"

#include "FreeRTOS.h"
#include "gpio.h"
#include "queue.h"
#include "task.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// If somehow port and pin number
// needs to change throughout project,
// define here:
#define PORT_NUM 1
#define PIN_NUM 0

// Enable debugging:
#define DEBUG_ENABLE 10

// Global variables:
QueueHandle_t time_on_data_queue;
QueueHandle_t time_off_data_queue;
// Private variables:
static gpio_s pir_pin;

// Private function declarations:
static void pir__init_pin(void);
static bool pir__get_sensor(void);

/**************************************************************************************/
/********************************* Public Functions ***********************************/
/**************************************************************************************/

void pir__freertos_task(void *parameter) {
  vTaskDelay(5000);
  lightbulb__init();

  char time_on[12] = "";
  char time_off[12] = "";
  pir__init_pin();

  printf("Started PIR FreeRTOS task.\n");

  TickType_t ticks = 0;

  while (1) {
    // If there is NO movement, turn on light
    ticks = xTaskGetTickCount();
    while (!pir__get_sensor()) {
      TickType_t temp = xTaskGetTickCount() - ticks;
      printf("LIGHT ON: %lu\n", temp);
      if (pir__get_sensor()) {
        strcpy(&time_on[0], &get_time_duration(temp)[0]);
        xQueueSend(time_on_data_queue, &time_on[0], 0);
        printf("Send to on queue\n");
        vTaskDelay(500);
      }
      vTaskDelay(100);
    }

    // If there is movement, turn off light
    ticks = xTaskGetTickCount();
    while (pir__get_sensor()) {
      TickType_t temp = xTaskGetTickCount() - ticks;
      if (!pir__get_sensor()) {
        strcpy(&time_off[0], &get_time_duration(temp)[0]);
        xQueueSend(time_off_data_queue, &time_off[0], 0);
        printf("Send to off queue\n");
        vTaskDelay(500);
      }
    }
    vTaskDelay(100);
  }
}
/**************************************************************************************/
/********************************* Private Functions **********************************/
/**************************************************************************************/

static void pir__init_pin(void) {
  pir_pin = gpio__construct_with_function(PORT_NUM, PIN_NUM, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_as_input(pir_pin);
}

static bool pir__get_sensor(void) { return gpio__get(pir_pin); }
