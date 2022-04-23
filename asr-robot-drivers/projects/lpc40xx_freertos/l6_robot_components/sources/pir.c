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
#define DEBUG_ENABLE 1

// Global variables:
QueueHandle_t time_on_data_queue;
QueueHandle_t time_off_data_queue;
// Private variables:
static gpio_s pir_pin;

// Private function declarations:
static void pir__init_pin(void);
static bool pir__get_sensor(void);

// Debug Purposes:
#if DEBUG_ENABLE
static void lightbulb__debug_print(char *line);
#endif

/**************************************************************************************/
/********************************* Public Functions ***********************************/
/**************************************************************************************/

void pir__freertos_task(void *parameter) {
  vTaskDelay(5000);
  lightbulb__init();

  TickType_t tick_count = 0;
  char get_duration[5] = "";
  char *time_on = "";
  char *time_off = "";

  pir__init_pin();

  printf("Started PIR FreeRTOS task.\n");

  while (1) {
    // If there is no movement
    while (!pir__get_sensor()) {
      TickType_t temp_count = tick_count;
      tick_count += xTaskGetTickCount() - temp_count;
      vTaskDelay(500);
    }

    // Get the time from lookup table and reset string
    *time_on = get_time_duration(tick_count)[0];
    xQueueSend(time_on_data_queue, &time_on[0], 0);
    memset(&time_on[0], 0, sizeof(time_on));

    // If there is movmement
    while (pir__get_sensor()) {
      TickType_t temp_count = tick_count;
      tick_count += xTaskGetTickCount() - temp_count;
      vTaskDelay(500);
    }

    // Get the time from lookup table and reset string
    *time_off = get_time_duration(tick_count)[0];
    xQueueSend(time_off_data_queue, &time_off[0], 0);
    memset(&time_off[0], 0, sizeof(time_off));
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

static void pir__debug_print(char *line) { printf("%s\n", &line[0]); }
