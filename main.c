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

#if LEDS_NUMBER <= 2
#error "Board is not equipped with enough amount of LEDs"
#endif
/* When UART is used for communication with the host do not use flow control.*/
#define UART_HWFC APP_UART_FLOW_CONTROL_DISABLED

#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */




nrf_drv_uart_t UARTE_inst0=NRF_DRV_UART_INSTANCE(0);
nrf_drv_uart_t UARTE_inst1=NRF_DRV_UART_INSTANCE(1);


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
TimerHandle_t led_toggle_timer_handle;  /**< Reference to LED1 toggling FreeRTOS timer. */
UBaseType_t stackwater;







#include "lwgsm/lwgsm_includes.h"
#if !LWGSM_CFG_SMS || !LWGSM_CFG_CALL
#error "SMS & CALL must be enabled to run this example"
#endif /* !LWGSM_CFG_SMS || !LWGSM_CFG_CALL */

static lwgsmr_t call_sms_evt_func(lwgsm_evt_t* evt);

/**
 * \brief           SMS entry
 */
static lwgsm_sms_entry_t
sms_entry;

/**
 * \brief           Start CALL & SMS combined example
 */
void
call_sms_start(void) {
    /* Add custom callback */
    lwgsm_evt_register(call_sms_evt_func);

    /* First enable SMS functionality */
    if (lwgsm_sms_enable(NULL, NULL, 1) == lwgsmOK) {
        printf("SMS enabled. Send new SMS from your phone to device.\r\n");
    } else {
        printf("Cannot enable SMS functionality!\r\n");
    }

    /* Then enable call functionality */
    if (lwgsm_call_enable(NULL, NULL, 1) == lwgsmOK) {
        printf("Call enabled. You may now take your phone and call modem\r\n");
    } else {
        printf("Cannot enable call functionality!\r\n");
    }

    /* Now send SMS from phone to device */
    printf("Start by sending SMS message or call device...\r\n");
}

/**
 * \brief           Event function for received SMS or calls
 * \param[in]       evt: GSM event
 * \return          \ref lwgsmOK on success, member of \ref lwgsmr_t otherwise
 */
static lwgsmr_t
call_sms_evt_func(lwgsm_evt_t* evt) {
    switch (lwgsm_evt_get_type(evt)) {
        case LWGSM_EVT_SMS_READY: {               /* SMS is ready notification from device */
            printf("SIM device SMS service is ready!\r\n");
            break;
        }
        case LWGSM_EVT_SMS_RECV: {                /* New SMS received indicator */
            lwgsmr_t res;

            printf("New SMS received!\r\n");    /* Notify user */

            /* Try to read SMS */
            res = lwgsm_sms_read(lwgsm_evt_sms_recv_get_mem(evt), lwgsm_evt_sms_recv_get_pos(evt), &sms_entry, 1, NULL, NULL, 0);
            if (res == lwgsmOK) {
                printf("SMS read in progress!\r\n");
            } else {
                printf("Cannot start SMS read procedure!\r\n");
            }
            break;
        }
        case LWGSM_EVT_SMS_READ: {                /* SMS read event */
            lwgsm_sms_entry_t* entry = lwgsm_evt_sms_read_get_entry(evt);
            if (lwgsm_evt_sms_read_get_result(evt) == lwgsmOK && entry != NULL) {
                /* Print SMS data */
                printf("SMS read. From: %s, content: %s\r\n",
                       entry->number, entry->data
                      );

                /* Try to send SMS back */
                if (lwgsm_sms_send(entry->number, entry->data, NULL, NULL, 0) == lwgsmOK) {
                    printf("SMS send in progress!\r\n");
                } else {
                    printf("Cannot start SMS send procedure!\r\n");
                }

                /* Delete SMS from device memory */
                lwgsm_sms_delete(entry->mem, entry->pos, NULL, NULL, 0);
            }
            break;
        }
        case LWGSM_EVT_SMS_SEND: {                /* SMS send event */
            if (lwgsm_evt_sms_send_get_result(evt) == lwgsmOK) {
                printf("SMS has been successfully sent!\r\n");
            } else {
                printf("SMS has not been sent successfully!\r\n");
            }
            break;
        }

        case LWGSM_EVT_CALL_READY: {              /* Call is ready notification from device */
            printf("SIM device Call service is ready!\r\n");
            break;
        }
        case LWGSM_EVT_CALL_CHANGED: {
            const lwgsm_call_t* call = lwgsm_evt_call_changed_get_call(evt);
            if (call->state == LWGSM_CALL_STATE_INCOMING) {   /* On incoming call */
                lwgsm_call_hangup(NULL, NULL, 0); /* Hangup call */
                lwgsm_sms_send(call->number, "Cannot answer call. Please send SMS\r\n", NULL, NULL, 0);
            }
            break;
        }
        default:
            break;
    }

    return lwgsmOK;
}

























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

    printf("press \"e\" to start Task Toggle LED\n");
    while(1){
      b=getchar();
      if ((char)b==0x65) break;
    }
    printf("Task Toggle LED started\n");
    while(1){

 //     NRF_LOG_PROCESS();
//      nrf_delay_ms(1000);
//      cnt++;
   
   (void)nrf_drv_uart_tx(&UARTE_inst0, &a, 1);
    vTaskDelay(500);
    
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

static char rx_buffer[1];
static char tx_buffer[1];

static void uart_drv_event_handler(nrf_drv_uart_event_t * p_event, void* p_context)
{
    app_uart_evt_t app_uart_event;
    uint32_t err_code;

    switch (p_event->type)
    {
        case NRF_DRV_UART_EVT_RX_DONE:
            // If 0, then this is a RXTO event with no new bytes.
            if(p_event->data.rxtx.bytes == 1)
            {
               
               // A new start RX is needed to continue to receive data
              (void)nrf_drv_uart_rx(&UARTE_inst0, &a, 1);
              (void)nrf_drv_uart_tx(&UARTE_inst0, &a, 1);
               break;
            }



            break;

        case NRF_DRV_UART_EVT_ERROR:
            app_uart_event.evt_type                 = APP_UART_COMMUNICATION_ERROR;
            app_uart_event.data.error_communication = p_event->data.error.error_mask;
            (void)nrf_drv_uart_rx(&UARTE_inst0, rx_buffer, 1);
            
            break;

        case NRF_DRV_UART_EVT_TX_DONE:
            

//            (void)nrf_drv_uart_tx(&UARTE_inst0, tx_buffer, 1);


            break;

        default:
            break;
    }
}


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
      UARTE_inst0.inst_idx=1;
      err_code=nrf_drv_uart_init(&UARTE_inst0,&config,uart_drv_event_handler);
      (void)nrf_drv_uart_rx(&UARTE_inst0, rx_buffer, 1);
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
      err_code=nrf_drv_uart_init(&UARTE_inst1,&config,uart_drv_event_handler);
      (void)nrf_drv_uart_rx(&UARTE_inst1, rx_buffer, 1);
      APP_ERROR_CHECK(err_code);

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
    uart_modem_init();
    call_sms_start();

    char cnt=0;
 
    /* Create task for LED0 blinking with priority set to 2 */
    UNUSED_VARIABLE(xTaskCreate(led_toggle_task_function, "LED0", configMINIMAL_STACK_SIZE +500 , NULL, 2, &led_toggle_task_handle));


    
    /* Start timer for LED1 blinking */
    led_toggle_timer_handle = xTimerCreate( "LED1", TIMER_PERIOD, pdTRUE, NULL, led_toggle_timer_callback);
    UNUSED_VARIABLE(xTimerStart(led_toggle_timer_handle, 0));

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
