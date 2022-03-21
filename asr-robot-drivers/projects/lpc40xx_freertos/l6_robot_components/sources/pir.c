#include "pir.h"

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

#define DEBUG_ENABLE 1

static gpio_s pir_pin;

QueueHandle_t light_pir_queue;

static void pir__init_pin(void);
static bool pir__get_sensor(void);

static void pir__debug_print(char *line);

/**************************************************************************************/
/********************************* Public Functions ***********************************/
/**************************************************************************************/

void pir__freertos_task(void *parameter) {
  int movement_signal = 0;
  int no_movement_signal = 1;
  TickType_t tick_count = 0;

  pir__init_pin();

  printf("Started PIR FreeRTOS task.\n");

  while (1) {
    TickType_t temp_count = tick_count;

    if (pir__get_sensor()) {
      xQueueSend(light_pir_queue, &movement_signal, 0);
      tick_count += xTaskGetTickCount() - temp_count;

    } else if (!pir__get_sensor()) {
      xQueueSend(light_pir_queue, &no_movement_signal, 0);
      tick_count = 0;
    }
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

static void pir__debug_print(char *line) {
#if DEBUG_ENABLE
  printf("%s\n", &line[0]);
#endif
}