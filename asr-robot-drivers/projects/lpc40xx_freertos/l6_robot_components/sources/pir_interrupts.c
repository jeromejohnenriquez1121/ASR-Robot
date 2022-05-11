#include "pir_interrupts.h"

#include "FreeRTOS.h"
#include "gpio.h"
#include "lightbulb.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "task.h"

// Define Relay Module GPIO pins
const uint32_t PORT_NUM = 2;
const uint32_t PIN_NUM = 7;

// Define time constants
const TickType_t init_pir_sensor_time = 10 * 1000;
const TickType_t turn_light_off_time = 60 * 1000;

static gpio_s relay_module;
static TickType_t time_duration_on = 0;

/**************************************************************************************/
/********************************* Private Functions **********************************/
/**************************************************************************************/

// This is the interrupt function
static void pir_interrupts__turn_off_light(void) {
  const uint32_t relay_module_pin = (1 << PIN_NUM);

  // Interrupts only happen in port numbers 0 and 2
  if (PORT_NUM == 0) {
    LPC_GPIOINT->IO0IntClr |= (1 << relay_module_pin);
  } else if (PORT_NUM == 2) {
    LPC_GPIOINT->IO2IntClr |= (1 << relay_module_pin);
  }

  time_duration_on = xTaskGetTickCount();
  // Send light on to sd card;

  time_duration_on = 0;

  lightbulb__turn_off_light();
  vTaskDelay(turn_light_off_time);
}

static void set_up_relay_module(void) {
  relay_module = gpio__construct_with_function(PORT_NUM, PIN_NUM, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_as_output(relay_module);
}

static void set_up_interrupts(void) {
  const uint32_t relay_module_pin = (1 << PIN_NUM);

  if (PORT_NUM == 0) {
    LPC_GPIOINT->IO0IntEnF |= (1 << relay_module_pin);
  } else if (PORT_NUM == 2) {
    LPC_GPIOINT->IO2IntEnF |= (1 << relay_module_pin);
  }

  NVIC_EnableIRQ(GPIO_IRQn);

  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO, pir_interrupts__turn_off_light, NULL);
}

/**************************************************************************************/
/********************************* Public Functions ***********************************/
/**************************************************************************************/

void pir_interrupts__freertos_task(void *parameter) {
  set_up_relay_module();
  set_up_interrupts();

  // Wait 10 seconds for PIR sensor(s) to initialize
  vTaskDelay(init_pir_sensor_time);

  lightbulb__turn_on_light();

  while (1) {
  }
}
