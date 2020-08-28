/*
 * modem_uart_driver.c
 *
 *  Created on: Jul 20, 2019
 *      Author: jar0d
 *
 * @section License
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (C) 2010-2019 Oryx Embedded SARL. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.9.2
 *
 */



//Dependencies
#include <stdio.h>


#include "core/net.h"
#include "ppp/ppp_hdlc.h"
#include "modem_uart_driver.h"
#include "debug.h"
#include "ppp/ppp_hdlc.h"
#include "nrf_delay.h"
#include "modem.h"


char rx_buffer[1];
char tx_buffer[1];

/**
 * @brief UART driver
 **/


const UartDriver uartDriver =
{
   uartInit,
   uartEnableIrq,
   uartDisableIrq,
   uartStartTx
};

/**
 * @brief UART configuration
 * @return Error code
 **/


error_t uartInit(void)
{




	//Debug message


	(void)nrf_drv_uart_rx(&UARTE_inst0, rx_buffer, 1);
	//Successful processing


	return NO_ERROR;

}

/**
 * @brief Enable UART interrupts
 **/

void uartEnableIrq(void)
{
   //Enable USART1 interrupts
   //NVIC_EnableIRQ(USART2_IRQn);
}


/**
 * @brief Disable UART interrupts
 **/

void uartDisableIrq(void)
{
   //Disable USART1 interrupt
   //NVIC_DisableIRQ(USART2_IRQn);
}


/**
 * @brief Start transmission
 **/

void uartStartTx(void)
{

  int_t c;
   //Enable TXE interrupt
   



     if (!nrf_drv_uart_tx_in_progress(&UARTE_inst0))
     {
            (void)pppHdlcDriverReadTxQueue(interface, &c);
		 //Valid character read?
		 if(c != EOF)
		 {
			//Send data byte
			(void)nrf_drv_uart_tx(&UARTE_inst0, &c, 1);
                        
		 }
		 else
		 {
			//Disable TXE interrupt
			 //(void)nrf_drv_uart_tx(&UARTE_inst0, &a, 1);
		 }

     }
 }



error_t uartDeinit(void){

	

	//Debug message

	


}

//void modem_uart_irqHandler()
//{
//
//   int_t c;
//   bool_t flag;
//   NetInterface *interface;
//
//   //Enter interrupt service routine
//   osEnterIsr();
//
//   //This flag will be set if a higher priority task must be woken
//   flag = FALSE;
//
//   //Point to the PPP network interface
//   interface = &netInterface[0];
//
//   /******** Transmission **********/
//
//   if (LL_USART_IsActiveFlag_TXE(USART2) && LL_USART_IsEnabledIT_TXE(USART2)){
//
//		//Get next character
//		flag |= pppHdlcDriverReadTxQueue(interface, &c);
//
//		 //Valid character read?
//		 if(c != EOF)
//		 {
//			//Send data byte
//			LL_USART_TransmitData8(USART2,c);
//		 }
//		 else
//		 {
//			//Disable TXE interrupt
//			 LL_USART_DisableIT_TXE(USART2);
//		 }
//
//   }
//
//   /******** Reception **********/
//
//
//   if (LL_USART_IsActiveFlag_RXNE(USART2) && LL_USART_IsEnabledIT_RXNE(USART2))
//   {
//
//	   	 c = LL_USART_ReceiveData8(USART2);
//	   	//Process incoming character
//	   	flag |= pppHdlcDriverWriteRxQueue(interface, c);
//
//   }
//
//   //ORE interrupt?
//   if (LL_USART_IsActiveFlag_ORE(USART2) && LL_USART_IsEnabledIT_RXNE(USART2)){
//	   //Clear ORE interrupt flag
//	   LL_USART_ClearFlag_ORE(USART2);
//   }
//
//   /*
//   else{
//
//	    LL_USART_ClearFlag_ORE(USART2);
//		LL_USART_ClearFlag_NE(USART2);
//		LL_USART_ClearFlag_FE(USART2);
//		LL_USART_ClearFlag_PE(USART2);
//
//   }*/
//
//   //Leave interrupt service routine
//   osExitIsr(flag);
//
//}



 


void uart_drv_event_handler(nrf_drv_uart_event_t * p_event, void* p_context)
{
    int_t c;
    uint32_t err_code;

    switch (p_event->type)
    {
        case NRF_DRV_UART_EVT_RX_DONE:
            // If 0, then this is a RXTO event with no new bytes.
            if(p_event->data.rxtx.bytes == 1)
            {
               
               // A new start RX is needed to continue to receive data
              (void) pppHdlcDriverWriteRxQueue(interface, p_event->data.rxtx.p_data[0]);
              
              
              
            }
            (void)nrf_drv_uart_rx(&UARTE_inst0, &rx_buffer, 1);


            break;

        case NRF_DRV_UART_EVT_ERROR:
            //app_uart_event.evt_type                 = APP_UART_COMMUNICATION_ERROR;
            //app_uart_event.data.error_communication = p_event->data.error.error_mask;
            //(void)nrf_drv_uart_rx(&UARTE_inst0, rx_buffer, 1);
            
            break;

        case NRF_DRV_UART_EVT_TX_DONE:
             (void)pppHdlcDriverReadTxQueue(interface, &c);
              (void)nrf_drv_uart_tx(&UARTE_inst0, &c, 1);


            break;

        default:
            break;
    }
}
