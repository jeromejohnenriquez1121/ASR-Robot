#include "new_pir.h"

#include "gpio.h"
#include "lightbulb.h"
#include "time_duration.h"

#include "FreeRTOS.h"
#include "delay.h"
#include "ff.h"
#include "string.h"
#include "task.h"
#include <stdint.h>
#include <stdio.h>

#define PORT_NUM 2
#define PIN_NUM 7

// Private function and variable initializations:
static gpio_s pir_pin;
static void pir__init_pin();
static bool pir__get_sensor();
static void write_file(char *line);

static FIL file;
static UINT br;
static char file_name[12] = "asr.txt";

/**************************************************************************************/
/********************************* Public Functions ***********************************/
/**************************************************************************************/

void pir_write__freertos_task(void *parameter) {
  lightbulb__init();
  pir__init_pin();

  fprintf(stderr, "Start PIR freertos task\n");

  bool movement;

  lightbulb__turn_on_light();
  vTaskDelay(5 * 1000);

  while (1) {
    // If there is NO movement, turn on light:
    movement = pir__get_sensor();
    if (movement) {
      lightbulb__turn_off_light();
      vTaskDelay(10 * 1000);

    } else {
      lightbulb__turn_on_light();
      movement = false;
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

static void write_file(char *line) {
  FRESULT fr = f_open(&file, &file_name[0], FA_OPEN_APPEND | FA_WRITE);

  if (fr == FR_OK) {
    f_write(&file, &line[0], strlen(&line[0]), &br);
  }

  f_close(&file);
}
