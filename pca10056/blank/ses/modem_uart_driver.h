/*
 * modem_uart_driver.h
 *
 *  Created on: Jul 20, 2019
 *      Author: jar0d
 */

#ifndef MODEM_INC_MODEM_UART_DRIVER_H_
#define MODEM_INC_MODEM_UART_DRIVER_H_

//Dependencies
#include "ppp/ppp_hdlc.h"
#include "nrf_drv_uart.h"
//C++ guard
#ifdef __cplusplus
   extern "C" {
#endif

//UART driver
 const UartDriver uartDriver;
extern nrf_drv_uart_t UARTE_inst0;
extern NetInterface *interface;
//External interrupt related functions
error_t uartInit(void);
void uartEnableIrq(void);
void uartDisableIrq(void);
void uartStartTx(void);
error_t uartDeinit(void);
void uart_drv_event_handler(nrf_drv_uart_event_t * p_event, void* p_context);

//C++ guard
#ifdef __cplusplus
  }
#endif


#endif /* MODEM_INC_MODEM_UART_DRIVER_H_ */
