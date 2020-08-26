#include <string.h>

#include "FIFO.h"
#include "PhysicalLayer.h"
#include "ServiceLayer.h"
#include "SimCom.h"




 char ph_receive_it_buf[];

extern char rx_buffer[1];
extern nrf_drv_uart_t UARTE_inst1;



/*
 * Test
 */
void callback0(char from, char to, const char* data, SIMCOM_LENGTH_TYPE length)
{
  sl_send(to,from,data,length);
}

void StartSendTask(void * argument)
{
  for(;;)
  {
	  ph_send_intr();
  }
}

void StartReceiveTask(void * argument)
{
  for(;;)
  {
	  char c;

	  if(out_fifo(&ph_receive_fifo, &c)) {
		  in_char_queue(&ph_receive_queue, c);
		  sl_receive_intr();

	  } else {
                  //(void)nrf_drv_uart_rx(&UARTE_inst1, ph_receive_it_buf, 1);
		  //HAL_UART_Receive_IT(uart_device, ph_receive_it_buf, 1);
		  vTaskDelay(1);
	  }
  }
}

bool simcom_init(nrf_drv_uart_t *device)
{
	sl_config(0, callback0);

	bool state = sl_init(device);

	return state;
}
