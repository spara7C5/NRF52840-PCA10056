/**
 * Copyright (c) 2015 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 * @defgroup blinky_example_main main.c
 * @{
 * @ingroup blinky_example_freertos
 *
 * @brief Blinky FreeRTOS Example Application main file.
 *
 * This file contains the source code for a sample application using FreeRTOS to blink LEDs.
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "bsp.h"
#include "nordic_common.h"
#include "nrf_drv_clock.h"
#include "sdk_errors.h"
#include "app_error.h"
#include "app_uart.h"
#include "nrf_drv_gpiote.h"
#if defined (UART_PRESENT)
#include "nrf_drv_uart.h"
#endif
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "PhysicalLayer.h"
#include "SimCom.h"
#include "semphr.h"
#include "modem.h"
#include "core/net.h"

#if LEDS_NUMBER <= 2
#error "Board is not equipped with enough amount of LEDs"
#endif
/* When UART is used for communication with the host do not use flow control.*/
#define UART_HWFC APP_UART_FLOW_CONTROL_DISABLED

#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */


extern NetInterface *interface;


extern void uart_drv_event_handler(nrf_drv_uart_event_t * p_event, void* p_context);
nrf_drv_uart_t UARTE_inst0=NRF_DRV_UART_INSTANCE(0);
//nrf_drv_uart_t UARTE_inst1=NRF_DRV_UART_INSTANCE(1);


void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}


#define TASK_DELAY        100           /**< Task delay. Delays a LED0 task for 200 ms */
#define TIMER_PERIOD      1000          /**< Timer period. LED1 timer will expire after 1000 ms */

TaskHandle_t  led_toggle_task_handle;   /**< Reference to LED0 toggling FreeRTOS task. */
TaskHandle_t  modemRX_task_handle;   
TaskHandle_t  modemTX_task_handle;   
TimerHandle_t led_toggle_timer_handle;  /**< Reference to LED1 toggling FreeRTOS timer. */
UBaseType_t stackwater;


 extern SemaphoreHandle_t dl_send_lockHandle;
 extern SemaphoreHandle_t sl_send_lockHandle;
 extern SemaphoreHandle_t ph_send_lockHandle;

/**@brief LED0 task entry function.
 *
 * @param[in] pvParameter   Pointer that will be used as the parameter for the task.
 */

 char mex[]="DETECTED\n";
 char a='a';
 int b='0';
static void led_toggle_task_function (void * pvParameter)
{
UNUSED_PARAMETER(pvParameter);



error_t res = modem_initEnvironment();
    
	if (res!=NO_ERROR){
		printf("Error initializing Network Stack. Rebooting\n");
		while(1);
	}
   osDelayTask(10);    
  res=modemInit(interface);


    if (res!=NO_ERROR){
		printf("Error in modem Init. Rebooting\n");
		while(1);
	}

  res=modemCall(interface);


    while(1){
    vTaskDelay(100);
    }


    printf("press \"e\" to start Task Toggle LED\n");
    while(1){
      b=getchar();
      if ((char)b==0x65) break;
      vTaskDelay(100);
    }
    printf("Task Toggle LED started\n");
    while(1){

 //     NRF_LOG_PROCESS();
//      nrf_delay_ms(1000);
//      cnt++;
   
   //(void)nrf_drv_uart_tx(&UARTE_inst0, &a, 1);
    vTaskDelay(1000);
    
    
    }
    



    UNUSED_PARAMETER(pvParameter);
    while (true)
    {
        stackwater=uxTaskGetStackHighWaterMark(NULL);
        bsp_board_led_invert(BSP_BOARD_LED_0);
        
        /* Delay a task for a given number of ticks */
        vTaskDelay(TASK_DELAY);

        /* Tasks must be implemented to never return... */
//        if(nrf_gpio_pin_read(TEST_PIN_1)){
//          for(int j=0;j<sizeof(mex);j++){
//            app_uart_put(mex[j]);
//          }
//          printf(mex);
//        }
    }
}

/**@brief The function to call when the LED1 FreeRTOS timer expires.
 *
 * @param[in] pvParameter   Pointer that will be used as the parameter for the timer.
 */

 


static void led_toggle_timer_callback (void * pvParameter)
{

    
    UNUSED_PARAMETER(pvParameter);
    bsp_board_led_invert(BSP_BOARD_LED_1);
    
//    nrf_gpio_pin_dir_t d= nrf_gpio_pin_dir_get(TEST_PIN);
//     uint32_t in=nrf_gpio_pin_out_read(TEST_PIN);
//     nrf_gpio_pin_toggle(TEST_PIN);
//    uint32_t fin=nrf_gpio_pin_out_read(TEST_PIN);
//(void)nrf_drv_uart_tx(&UARTE_inst0, &a, 1);
//(void)nrf_drv_uart_tx(&UARTE_inst1, &a, 1);
}


void gpiote_callback(void){
//  bsp_board_led_invert(BSP_BOARD_LED_3);
  nrf_gpio_pin_write(LED_4,0); //light up the LED with 0
//  bsp_board_led_on(BSP_BOARD_LED_3);
}

void bsp_ext_init(void){

//    nrf_gpio_cfg_output(TEST_PIN);
//    nrf_gpio_cfg_input(TEST_PIN_1,NRF_GPIO_PIN_PULLDOWN);
//    nrfx_err_t err_code = nrf_drv_gpiote_init();
//    APP_ERROR_CHECK(err_code);
//
//    nrf_drv_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
//    in_config.pull = NRF_GPIO_PIN_PULLDOWN;
//
//    err_code = nrf_drv_gpiote_in_init(TEST_PIN_2, &in_config, gpiote_callback);
//    APP_ERROR_CHECK(err_code);
//
//    nrf_drv_gpiote_in_event_enable(TEST_PIN_2, true);

    }

/////////////////////////////////////////////////////////////////////////////////////////////////////




   void uart_tool_init(){

      ret_code_t err_code;
  
      nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;
      config.baudrate = (nrf_uart_baudrate_t)NRF_UART_BAUDRATE_115200;
      config.hwfc = (UART_HWFC == APP_UART_FLOW_CONTROL_DISABLED) ? NRF_UART_HWFC_DISABLED : NRF_UART_HWFC_ENABLED;
      config.interrupt_priority = APP_IRQ_PRIORITY_LOWEST;
      config.parity = false ? NRF_UART_PARITY_INCLUDED : NRF_UART_PARITY_EXCLUDED;
      config.pselcts = CTS_PIN_NUMBER;
      config.pselrts = RTS_PIN_NUMBER;
      config.pselrxd = RX_PIN_NUMBER;
      config.pseltxd = TX_PIN_NUMBER;
      //UARTE_inst0.inst_idx=1;
      err_code=nrf_drv_uart_init(&UARTE_inst0,&config,uart_drv_event_handler);
      
      APP_ERROR_CHECK(err_code);
      //NRFX_LOG_WARNING("Hello");
   }

////////////////////////////////////////////////////////////////////////////////////////////////////



   void uart_modem_init(){

      ret_code_t err_code;
  
      nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;
      config.baudrate = (nrf_uart_baudrate_t)NRF_UART_BAUDRATE_115200;
      config.hwfc = (UART_HWFC == APP_UART_FLOW_CONTROL_DISABLED) ? NRF_UART_HWFC_DISABLED : NRF_UART_HWFC_ENABLED;
      config.interrupt_priority = APP_IRQ_PRIORITY_LOWEST;
      config.parity = false ? NRF_UART_PARITY_INCLUDED : NRF_UART_PARITY_EXCLUDED;
      config.pselcts = MODEM_CTS;
      config.pselrts = MODEM_RTS;
      config.pselrxd = MODEM_RX;
      config.pseltxd = MODEM_TX;
      //UARTE_inst1.uarte.drv_inst_idx=1;
//      err_code=nrf_drv_uart_init(&UARTE_inst1,&config,uart_drv_event_handler);
//      (void)nrf_drv_uart_rx(&UARTE_inst1, rx_buffer, 1);
//      APP_ERROR_CHECK(err_code);

   }


 static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}







int main(void)
{

    uint32_t t=0;
    t=NRFX_UART_ENABLED_COUNT;
    ret_code_t err_code;
    log_init();
    /* Initialize clock driver for better time accuracy in FREERTOS */
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    /* Configure LED-pins as outputs */
    bsp_board_init(BSP_INIT_LEDS);
    bsp_ext_init();
    uart_tool_init();
    //uart_modem_init();
    
    
     

 
    /* Create task for LED0 blinking with priority set to 2 */
    UNUSED_VARIABLE(xTaskCreate(led_toggle_task_function, "LED0", configMINIMAL_STACK_SIZE +500 , NULL, 2, &led_toggle_task_handle));
    // UNUSED_VARIABLE(xTaskCreate(StartReceiveTask, "MODEMRX", configMINIMAL_STACK_SIZE +500 , NULL, 2, &modemRX_task_handle));
    //  UNUSED_VARIABLE(xTaskCreate(StartSendTask, "MODEMTX", configMINIMAL_STACK_SIZE +500 , NULL, 2, &modemTX_task_handle));


    
    /* Start timer for LED1 blinking */
    //led_toggle_timer_handle = xTimerCreate( "LED1", TIMER_PERIOD, pdTRUE, NULL, led_toggle_timer_callback);
    //UNUSED_VARIABLE(xTimerStart(led_toggle_timer_handle, 0));

    /* Activate deep sleep mode */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    /* Start FreeRTOS scheduler. */
    vTaskStartScheduler();

    while (true)
    {
        /* FreeRTOS should not be here... FreeRTOS goes back to the start of stack
         * in vTaskStartScheduler function. */
    }
}

/**
 *@}
 **/
