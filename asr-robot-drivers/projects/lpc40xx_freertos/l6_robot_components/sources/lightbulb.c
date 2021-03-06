#include "lightbulb.h"

#include "FreeRTOS.h"
#include "delay.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "queue.h"
#include <stdio.h>

// Define port and pin numbers here:
#define PORT_NUM 1
#define PIN_NUM 0
#define VCC_PIN 14

// Private variables:
// The lightbulb pin is the pin to the relay module
static gpio_s lightbulb_pin;
static gpio_s vcc_pin;

/**************************************************************************************/
/********************************* Public Functions ***********************************/
/**************************************************************************************/

void lightbulb__turn_on_light(void) { gpio__set(lightbulb_pin); } // If LOW turn on light

void lightbulb__turn_off_light(void) { gpio__reset(lightbulb_pin); } // If HI turn off light

void lightbulb__init(void) {
  const uint32_t iocon_mode__clear_resistor = ~(3 << 3);
  const uint32_t iocon_mode__enable_pull_down_resistor = (1 << 3);

  lightbulb_pin = gpio__construct_with_function(PORT_NUM, PIN_NUM, GPIO__FUNCITON_0_IO_PIN);
  vcc_pin = gpio__construct_with_function(PORT_NUM, VCC_PIN, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_as_output(lightbulb_pin);
  gpio__set_as_output(vcc_pin);
  gpio__set(vcc_pin);

  LPC_IOCON->P1_10 &= iocon_mode__clear_resistor;
  LPC_IOCON->P1_10 |= iocon_mode__enable_pull_down_resistor;
}
