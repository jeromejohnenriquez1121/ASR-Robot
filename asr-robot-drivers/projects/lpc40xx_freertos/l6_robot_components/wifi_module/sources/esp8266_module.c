#include "esp8266_module.h"
#include "FreeRTOS.h"
#include "clock.h"
#include "delay.h"
#include "gpio.h"
#include "uart.h"
#include <stdio.h>
#include <stdlib.h>

// Define variables here:
#define TX_PORT_NUM 0
#define TX_PIN_NUM 0
#define RX_PORT_NUM 0
#define RX_PIN_NUM 1

const static char *ssid = "ssid";
const static char *password = "password";
const static char *ip_address = "0.0.0.0";
const static uint16_t tcp_port_num = 2000;
const static size_t buffer_size = 128;

static gpio_s tx_pin;
static gpio_s rx_pin;
static uart_e esp_uart = UART__3;

/**************************************************************************************/
/********************************* Private Functions **********************************/
/**************************************************************************************/

// Send command functions
static void esp8266_module__send_command(char *command) {
  for (size_t i = 0; i < buffer_size; i++) {
    uart__polled_put(esp_uart, command[i]);
  }

  fprintf(stderr, "esp uart command sent %s\n", &command[0]);
}

static void esp8266_module__read(char *input) { uart__polled_get(esp_uart, &input[0]); }

static bool esp8266_module__send_successful_command(char *command, char *expected_response_message,
                                                    char *error_message) {
  char *actual_response = "";

  while (&actual_response[0] == "") {
    esp8266_module__send_command(&command[0]);
    delay__ms(500);
    esp8266_module__read(&actual_response[0]);
  }

  if (*actual_response == *expected_response_message) {
    return true;
  }

  fprintf(stderr, "Error message: %s", &error_message[0]);
  return false;
}

static bool esp8266__connect_to_wifi(void) {
  char wifi_command[128] = "";
  snprintf(&wifi_command[0], buffer_size, "AP+CWJAP=\"%s\",\"%s\"", &ssid[0], &password[0]);

  if (esp8266_module__send_successful_command(&wifi_command[0], "OK", "FAIL")) {
    fprintf(stderr, "Connect to Wifi success.\n");
    return true;
  } else {
    fprintf(stderr, "Connect to Wifi fail.\n");
    return false;
  }
}

static bool esp8266__connect_to_tcp(void) {
  char tcp_command[128] = "";
  snprintf(&tcp_command[0], buffer_size, "AP+CIPSTART=\"TCP\",\"%s\",\"%u\"", &ip_address[0], tcp_port_num);

  if (esp8266_module__send_successful_command(&tcp_command[0], "OK", "ERROR")) {
    fprintf(stderr, "Connect to TCP/IP success.\n");
    return true;
  } else {
    fprintf(stderr, "Connect to TCP/IP fail.\n");
    return false;
  }
}

static void esp8266__send_data(char *buffer) {
  for (uint16_t i = 0; i < buffer_size; i++) {
    uart__polled_put(esp_uart, buffer[i]);
  }

  fprintf(stderr, "Sent to esp: %s\n", &buffer[0]);
}

// Initialize pins and module
static void esp8266_module__set_pins(void) {
  tx_pin = gpio__construct_with_function(TX_PORT_NUM, TX_PIN_NUM, GPIO__FUNCTION_2);
  rx_pin = gpio__construct_with_function(RX_PORT_NUM, RX_PIN_NUM, GPIO__FUNCTION_2);
}

static void esp8266_module__init(void) {
  const uint32_t baud_rate = 115200;
  uart__init(esp_uart, clock__get_peripheral_clock_hz(), baud_rate);

  // Reset ESP8266
  esp8266_module__send_command("AT+RST");
  fprintf(stderr, "Reset ESP8266\n");

  // Turn off echo
  while (!esp8266_module__send_successful_command("ATE0", "OK", "FAIL")) {
    fprintf(stderr, "Failed ATE0\n");
    vTaskDelay(1000);
  }

  // Set ESP8266 to UART
  while (!esp8266_module__send_successful_command("AST+UART_CUR", "OK", "FAIL")) {
    fprintf(stderr, "Failed AST+UART_CUR\n");
    vTaskDelay(1000);
  }

  // Set ESP8266 as single connection
  while (!esp8266_module__send_successful_command("AST+CIPMUX=0", "OK", "ERROR")) {
    fprintf(stderr, "Failed AST+CIPMUX=0\n");
    vTaskDelay(1000);
  }

  // Set ESP8266 as station mode
  while (!esp8266_module__send_successful_command("AST+CWMODE=0", "OK", "ERROR")) {
    fprintf(stderr, "Failed AST+CWMODE=0\n");
    vTaskDelay(1000);
  }

  // Set ESP8266 wifi and tcp
  while (!esp8266__connect_to_wifi()) {
    fprintf(stderr, "Cannot connect to wifi\n");
    vTaskSuspend(NULL);
  }

  while (!esp8266__connect_to_tcp()) {
    fprintf(stderr, "Cannot connect to TCP\n");
    vTaskSuspend(NULL);
  }

  // Test ESP8266
}

/**************************************************************************************/
/********************************* Public Functions ***********************************/
/**************************************************************************************/

void esp8266_module__freertos_task(void *parameter) {
  vTaskDelay(5000);
  esp8266_module__set_pins();
  esp8266_module__init();

  while (1) {
    esp8266__send_data("Hello world\n");
    vTaskDelay(1000);
  }
}