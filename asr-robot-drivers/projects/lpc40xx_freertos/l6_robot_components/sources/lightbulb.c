#include "lightbulb.h"

#include "FreeRTOS.h"
#include "delay.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "queue.h"
#include <stdio.h>

#define PORT_NUM 1
#define PIN_NUM 10
#define DEBUG_ENABLE 1

static gpio_s lightbulb_pin;

QueueHandle_t light_pir_queue;

static void lightbulb__init_pin(void);
static void lightbulb__turn_on_light(void);
static void lightbulb__turn_off_light(void);

static void lightbulb__debug_print(char *line);

typedef enum signal_type { is_movement = 0, no_movement = 1 } signal_type_e;

/**************************************************************************************/
/********************************* Public Functions ***********************************/
/**************************************************************************************/

// 0 is movement, 1 is no movement

void lightbulb__freertos_task(void *parameter) {
  lightbulb__init_pin();
  int signal = 0;

  while (1) {
    xQueueReceive(light_pir_queue, &signal, portMAX_DELAY);
    if (signal == is_movement) {
      lightbulb__turn_off_light();
      lightbulb__debug_print("Light is off.");
    } else if (signal == no_movement) {
      lightbulb__turn_on_light();
      lightbulb__debug_print("Light is on.");
    }
  }
}

/**************************************************************************************/
/********************************* Private Functions **********************************/
/**************************************************************************************/

static void lightbulb__init_pin(void) {
  const uint32_t iocon_mode__clear_resistor = ~(3 << 3);
  const uint32_t iocon_mode__enable_pull_down_resistor = (1 << 3);

  lightbulb_pin = gpio__construct_with_function(PORT_NUM, PIN_NUM, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_as_output(lightbulb_pin);

  LPC_IOCON->P1_10 &= iocon_mode__clear_resistor;
  LPC_IOCON->P1_10 |= iocon_mode__enable_pull_down_resistor;
}

static void lightbulb__turn_on_light(void) { gpio__set(lightbulb_pin); }
static void lightbulb__turn_off_light(void) { gpio__reset(lightbulb_pin); }

static void lightbulb__debug_print(char *line) {
#if DEBUG_ENABLE
  printf("%s\n", &line[0]);
#endif
}