#pragma once

/*
 **************************************************************************************************
 ****************************************** Light Bulb ********************************************
 **************************************************************************************************
 *
 *  Light bulb has simple commands to turn on and off the bulb, and initialize the GPIO pin.
 */

void lightbulb__turn_on_light(void);

void lightbulb__turn_off_light(void);

void lightbulb__init(void);