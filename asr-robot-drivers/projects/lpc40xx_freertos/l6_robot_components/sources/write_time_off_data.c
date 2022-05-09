// #include "write_time_off_data.h"

// #include <stdbool.h>
// #include <stdio.h>

// #include "FreeRTOS.h"
// #include "task.h"

// #include "board_io.h"
// #include "common_macros.h"
// #include "delay.h"
// #include "ff.h"
// #include "periodic_scheduler.h"
// #include "queue.h"

// #include "string.h"

// QueueHandle_t time_off_data_queue;

// static FIL file;
// static char file_name[12] = "asr.txt";

// void write_time_off_data__freertos_task(void *parameter) {
//   char line[12] = "";

//   while (1) {
//     xQueueReceive(time_off_data_queue, &line[0], portMAX_DELAY);
//     vTaskDelay(500);

//     printf("Received %s\n", &line[0]);
//     strcat(&line[0], "OFF\n");

//     UINT bytes_written = 0;
//     FRESULT f_result = f_open(&file, &file_name[0], (FA_OPEN_APPEND | FA_WRITE));
//     printf("FRESULT: %u\n", f_result);

//     if (f_result == FR_OK) {
//       f_write(&file, &line[0], strlen(&line[0]), &bytes_written);
//     }

//     memset(&line[0], 0, strlen(&line[0]));

//     f_close(&file);
//   }
// }