#include "pir.h"
#include "lightbulb.h"
#include "time_duration.h"

#include "FreeRTOS.h"
#include "gpio.h"
#include "queue.h"
#include "task.h"
#include <stdbool.h>
#include <stdio.h>

// If somehow port and pin number
// needs to change throughout project,
// define here:
#define PORT_NUM 1
#define PIN_NUM 0

#define DEBUG_ENABLE 0

// Global variables:
QueueHandle_t data_queue;

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
  // Wait for CLI output
  vTaskDelay(500);

  lightbulb__init();

  TickType_t tick_count = 0;
  char *get_duration = "";

  pir__init_pin();

  printf("Started PIR FreeRTOS task.\n");

  while (1) {
    TickType_t temp_count = tick_count;

    // If there is no movement
    while (!pir__get_sensor()) {
      lightbulb__turn_on_light();
      tick_count = xTaskGetTickCount() - temp_count;

      vTaskDelay(500);
    }

    if (tick_count != 0) {
      xQueueSend(data_queue, &get_duration[0], 0);
    }

    // If there is movement:
    lightbulb__turn_off_light();

    // Reset variables
    *get_duration = "";
    tick_count = 0;
    vTaskDelay(500);
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
