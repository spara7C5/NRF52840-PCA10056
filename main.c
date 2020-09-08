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


#include "nrf_drv_timer.h"
#include "nrf_queue.h"
#include "nrf_drv_pwm.h"

#include "modem.h"
#include "core/net.h"
#include "debug.h"

#include "cmsis_os2.h"

#if LEDS_NUMBER <= 2
#error "Board is not equipped with enough amount of LEDs"
#endif
/* When UART is used for communication with the host do not use flow control.*/
#define UART_HWFC APP_UART_FLOW_CONTROL_DISABLED

#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */


extern NetInterface *interface;

const nrf_drv_timer_t TIMER_DATA = NRF_DRV_TIMER_INSTANCE(0);
void timer_dummy_event_handler(nrf_timer_event_t event_type, void* p_context){}
NRF_QUEUE_DEF(uint32_t, datain_queue, 8, NRF_QUEUE_MODE_OVERFLOW); // analog readouts of bit timelengths
//static SemaphoreHandle_t datain_semaph;
//static SemaphoreHandle_t butt_semaph;
//static SemaphoreHandle_t message_semaph;
#define DATAIN_SIGNAL 0x00000001 // used to save cpu time (avoid polling the queue)
#define MESS_PROC_SIGNAL 0x00000002 // used as mutex to protect buffer

#define BUTT_SIGNAL 0x00000001 // used to save cpu time (block the thread until the button)
#define MESS_REC_SIGNAL 0x00000002 // used as mutex to protect buffer
uint8_t datain_buff;
#define MESS_REPLY_TIMEOUT 1000 //1 sec

static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);


extern void uart_drv_event_handler(nrf_drv_uart_event_t * p_event, void* p_context);
nrf_drv_uart_t UARTE_inst0=NRF_DRV_UART_INSTANCE(0);
//nrf_drv_uart_t UARTE_inst1=NRF_DRV_UART_INSTANCE(1);

typedef enum {
  SUCCESS=0,
  WRONG_BIT_TIME,
  WRONG_MEX_PROCESS,

}error_smartbutt_t;

#define BITZEROMIN  4000  
#define BITZEROMAX  5000
#define BITONEMIN    6000
#define BITONEMAX   10000



#define TASK_DELAY        100           /**< Task delay. Delays a LED0 task for 200 ms */
#define TIMER_PERIOD      1000          /**< Timer period. LED1 timer will expire after 1000 ms */

TaskHandle_t  modem_task_handle;   /**< Reference to LED0 toggling FreeRTOS task. */  
TimerHandle_t dataIN_task_handle;  
TimerHandle_t main_task_handle;  
UBaseType_t stackwater;



#define BUZZPRESS vTaskDelay(100); nrf_gpio_pin_set(BUZZ); nrf_delay_ms(400); nrf_gpio_pin_clear(BUZZ) 
#define BUZZERR  for(uint8_t i=0;i<3;i++){nrf_gpio_pin_set(BUZZ); nrf_delay_ms(50); nrf_gpio_pin_clear(BUZZ);nrf_delay_ms(50);} 



////////////////////   utils  ///////////////////////

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

//////////////////////////////////////////////////////


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

 char mex[]="DETECTED\n";
 char a='a';
 int b='0';
static void modem_task_function (void * pvParameter)
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
		while(1)vTaskDelay(1000);
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


uint32_t datastarttime=0;
bool datagotinit=0;
uint32_t dataendtime=0;
uint32_t dataperiod=0;
ret_code_t err_code;

void gpiote_callback(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action){
//  bsp_board_led_invert(BSP_BOARD_LED_3);
//  nrf_gpio_pin_write(LED_4,0); //light up the LED with 0
//  bsp_board_led_on(BSP_BOARD_LED_3);
  
  if (pin==DATA_IN){
    osThreadFlagsSet(dataIN_task_handle,DATAIN_SIGNAL);
    if(datagotinit==0){
      datastarttime=nrfx_timer_capture(&TIMER_DATA,NRF_TIMER_CC_CHANNEL0);
       //nrf_drv_timer_clear(&TIMER_LED);
      datagotinit=1;
    }else{
        datagotinit=0;
        dataendtime=nrfx_timer_capture(&TIMER_DATA,NRF_TIMER_CC_CHANNEL0);
      if(dataendtime>datastarttime){
        dataperiod=(dataendtime-datastarttime)/16;
        ret_code_t err_code = nrf_queue_push(&datain_queue, &dataperiod);
        APP_ERROR_CHECK(err_code);
       // __NOP();
        }else{
        __NOP();
        //TODO: resend data or error or difference?
        }
     }

  }else if(pin==BUTT){
    osThreadFlagsSet(main_task_handle,BUTT_SIGNAL);
      

  }

 }//end gpio_callback

void bsp_ext_init(void){

    nrfx_err_t err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_gpio_cfg_output(BUZZ);
    nrf_gpio_pin_clear(BUZZ);
    

    nrf_drv_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    in_config.pull = GPIO_PIN_CNF_PULL_Pulldown;
    err_code = nrf_drv_gpiote_in_init(DATA_IN, &in_config, gpiote_callback);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(DATA_IN, true);

    nrf_drv_gpiote_in_config_t butt_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    err_code = nrf_drv_gpiote_in_init(BUTT, &butt_config, gpiote_callback);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(BUTT, true);

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


static void timer_init(void){

      uint32_t err_code = NRF_SUCCESS;
   nrf_drv_timer_config_t timer_cfg = {                                                                                    
      .frequency          = (nrf_timer_frequency_t)NRFX_TIMER_DEFAULT_CONFIG_FREQUENCY,
      .mode               = (nrf_timer_mode_t)NRF_TIMER_MODE_TIMER,          
      .bit_width          = (nrf_timer_bit_width_t)NRFX_TIMER_DEFAULT_CONFIG_BIT_WIDTH,
      .interrupt_priority = NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY,                    
      .p_context          = NULL                                                       
  };
      err_code = nrf_drv_timer_init(&TIMER_DATA, &timer_cfg, timer_dummy_event_handler);
      APP_ERROR_CHECK(err_code);

      //time_ticks = nrf_drv_timer_ms_to_ticks(&TIMER_DATA, time_ms);

      //nrf_drv_timer_extended_compare(
       //    &TIMER_LED, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

      nrf_drv_timer_enable(&TIMER_DATA);

}


static error_smartbutt_t process_message(uint8_t* mex){

  return SUCCESS;
}

static void dataIN_task_function (void * pvParameter){

    uint32_t dataperiodout=0;
    ret_code_t err_code=NRF_SUCCESS;
    uint8_t bit_cnt=0;

    while(1){
      osThreadFlagsWait(DATAIN_SIGNAL,0,osWaitForever);
      bit_cnt=0;
      while (bit_cnt<(8+1)){
         err_code= nrf_queue_generic_pop(&datain_queue,&dataperiodout,false);
         if(err_code==NRF_SUCCESS){
            if(BITZEROMIN<dataperiodout && dataperiodout<BITZEROMAX)datain_buff&=~(0b0000001<<bit_cnt);
            else if (BITONEMIN<dataperiodout && dataperiodout<BITONEMAX)datain_buff|=(0b0000001<<bit_cnt);
            else break;
            bit_cnt++;
         }else break; // error in queue (rare)
      }

      if (bit_cnt==8){ //all bits are valid
          osThreadFlagsSet(main_task_handle,MESS_REC_SIGNAL);
          osThreadFlagsWait(MESS_PROC_SIGNAL,0,osWaitForever); // stop writing on datain_buff 
      }// the "else" is managed by the main function which timeouts waiting for vali message

      
      
    }
}





static void main_task_function (void * pvParameter){

  while(1){
    //uint32_t flag= osThreadFlagsGet ();
    osThreadFlagsWait(BUTT_SIGNAL,0,osWaitForever);
    
    BUZZPRESS;
    if(osThreadFlagsWait(MESS_REC_SIGNAL,0,MESS_REPLY_TIMEOUT)==osErrorTimeout) {
      BUZZERR
    }
    
    TRACE_INFO("message:"BYTE_TO_BINARY_PATTERN"\n",BYTE_TO_BINARY(datain_buff));
    osThreadFlagsSet(dataIN_task_handle,MESS_PROC_SIGNAL);
  }
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
    uart_tool_init();
    //uart_modem_init();
    timer_init();
    bsp_ext_init();
     

 
    /* Create task for LED0 blinking with priority set to 2 */
    //UNUSED_VARIABLE(xTaskCreate(modem_task_function, "MODEM", configMINIMAL_STACK_SIZE +500 , NULL, 2, &modem_task_handle));
    UNUSED_VARIABLE(xTaskCreate(dataIN_task_function, "DATAIN", configMINIMAL_STACK_SIZE +200 , NULL, 2, &dataIN_task_handle));
    UNUSED_VARIABLE(xTaskCreate(main_task_function, "MAIN", configMINIMAL_STACK_SIZE +200 , NULL, 2, &main_task_handle));


    
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



//static void demo3(void)
//{
//    //NRF_LOG_INFO("Demo 3");
//
//    /*
//     * This demo uses only one channel, which is reflected on LED 1.
//     * The LED blinks three times (200 ms on, 200 ms off), then it stays off
//     * for one second.
//     * This scheme is performed three times before the peripheral is stopped.
//     */
//
//    nrf_drv_pwm_config_t const config0 =
//    {
//        .output_pins =
//        {
//            BUZZ | NRF_DRV_PWM_PIN_INVERTED, // channel 0
//            NRF_DRV_PWM_PIN_NOT_USED,             // channel 1
//            NRF_DRV_PWM_PIN_NOT_USED,             // channel 2
//            NRF_DRV_PWM_PIN_NOT_USED,             // channel 3
//        },
//        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
//        .base_clock   = NRF_PWM_CLK_125kHz,
//        .count_mode   = NRF_PWM_MODE_UP,
//        .top_value    = 25000,
//        .load_mode    = NRF_PWM_LOAD_COMMON,
//        .step_mode    = NRF_PWM_STEP_AUTO
//    };
//    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config0, NULL));
//    m_used |= USED_PWM(0);
//
//    // This array cannot be allocated on stack (hence "static") and it must
//    // be in RAM (hence no "const", though its content is not changed).
//    static uint16_t /*const*/ seq_values[] =
//    {
//        0x8000,
//             0,
//        0x8000,
//             0,
//        0x8000,
//             0
//    };
//    nrf_pwm_sequence_t const seq =
//    {
//        .values.p_common = seq_values,
//        .length          = NRF_PWM_VALUES_LENGTH(seq_values),
//        .repeats         = 0,
//        .end_delay       = 4
//    };
//
//    (void)nrf_drv_pwm_simple_playback(&m_pwm0, &seq, 3, NRF_DRV_PWM_FLAG_STOP);
//}