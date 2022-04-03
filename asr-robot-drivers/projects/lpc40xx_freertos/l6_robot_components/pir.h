#pragma once

/*
 **************************************************************************************************
 ****************************************** PIR Sensor ********************************************
 **************************************************************************************************
 *
 * PIR sensor is a LOW PRIORITY FreeRTOS task. So the Wifi module task is blocked until it receives
 * something from this task. This task will run continously until it sends data to the wifi module.
 */

void pir__freertos_task(void *parameter);