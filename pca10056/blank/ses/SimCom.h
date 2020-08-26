#ifndef _SIMCOM_H
#define _SIMCOM_H

#include <stdbool.h>
#include <stdint.h>
#include "nrf_drv_uart.h"


/*
  You can modify this for your own project
*/
#define SIMCOM_LENGTH_TYPE uint16_t
#define SIMCOM_DLENGTH_TYPE uint32_t

bool sl_init(nrf_drv_uart_t *device);
bool simcom_init(nrf_drv_uart_t *device);
void StartReceiveTask(void  * argument);
void StartSendTask(void * argument);

#endif
