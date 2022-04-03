#include "wifi_module.h"

#include "FreeRTOS.h"
#include "esp32.h"
#include "queue.h"
#include "task.h"
#include "uart.h"
#include <stdio.h>

static const uart_e esp32_uart = UART__3;

// Define connection variables here:
static const char *wifi_ssid = "wifi_name";
static const char *wifi_password = "wifi_password";
static const char *wifi_ip_addr = "0.0.0.0";
static const uint16_t wifi_tcp_port = 0;

static void wifi_module__init(void);

extern QueueHandle_t light_duration_queue;

/**************************************************************************************/
/********************************* Public Functions ***********************************/
/**************************************************************************************/

void wifi_module__freertos_task(void *parameter) {

  // Wait for CLI output
  vTaskDelay(100);

  wifi_module__init();

  char light_duration[5] = "";

  light_duration_queue = xQueueCreate(1, sizeof(light_duration));

  while (1) {
    esp32__tcp_connect(&wifi_ip_addr[0], wifi_tcp_port);

    xQueueReceive(light_duration_queue, &light_duration[0], portMAX_DELAY);
    esp32__cipsend(&light_duration[0], sizeof(&light_duration[0]));
  }
}

/**************************************************************************************/
/********************************* Private Functions **********************************/
/**************************************************************************************/

static void wifi_module__init(void) {
  esp32__init(esp32_uart);
  esp32__send_command("AT+RST");
  esp32__clear_receive_buffer(5000);

  // Test AT command
  esp32__wait_for_successful_command("AT", "OK", "Basic communication test");
  esp32__wait_for_successful_command("ATE0", "OK", "Turn off echo");
  esp32__wait_for_successful_command("AT+CIPMUX=0", "OK", "Single connection");

  // Enter "Station Mode"
  esp32__wait_for_successful_command("AT+CWMODE=1", "OK", "Connect to WIFI");

  fprintf(stderr, "ESP32 initialized...");

  // Connect to SSID
  bool wifi_connect_success = esp32__wifi_connect(&wifi_ssid[0], &wifi_password[0]);
  if (!wifi_connect_success) {
    fprintf(stderr, "Wifi cannot connect.\n");
    vTaskSuspend(NULL);
  }
}
