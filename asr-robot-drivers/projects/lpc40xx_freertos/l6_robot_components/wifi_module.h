#pragma once

/*
 **************************************************************************************************
 ***************************************** Wifi Module ********************************************
 **************************************************************************************************
 *
 * Wifi module is a HIGH priority FreeRTOS task. This task is blocked until it the pir sensor
 * sends data to the wifi module by a queue.
 */

void wifi_module__freertos_task(void *parameter);