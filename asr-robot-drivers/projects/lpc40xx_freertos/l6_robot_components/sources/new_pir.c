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

#define PORT_NUM 1
#define PIN_NUM 0

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

  while (1) {
    // If there is NO movement, turn on light:
    if (!pir__get_sensor()) {
      lightbulb__turn_on_light();
      vTaskDelay(500);

      fprintf(stderr, "Light on\n");

      TickType_t first_time_on = xTaskGetTickCount();

      while (!pir__get_sensor()) {
        ;
      }

      TickType_t total_time_on = xTaskGetTickCount() - first_time_on;
      char line[12] = "ON";
      strcat(&line[0], &get_time_duration(total_time_on)[0]);
      write_file(&line[0]);
    }

    // If there IS movement, turn off light:
    if (pir__get_sensor()) {
      lightbulb__turn_off_light();
      vTaskDelay(10 * 1000);

      fprintf(stderr, "Light off\n");
      TickType_t first_time_off = xTaskGetTickCount();

      while (pir__get_sensor()) {
        vTaskDelay(10 * 1000);
      }

      TickType_t total_time_off = xTaskGetTickCount() - first_time_off;
      char line[12] = "OFF";
      strcat(&line[0], &get_time_duration(total_time_off)[0]);
      write_file(&line[0]);
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

