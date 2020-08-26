#include "PhysicalLayer.h"

#include "semphr.h"
#include "task.h"

char ph_send_queue_buf[PH_BUF_LEN];
char ph_receive_queue_buf[PH_BUF_LEN];

char ph_send_dma_buf[PH_BUF_LEN];
extern char ph_receive_it_buf[1];
char ph_receive_fifo_buf[PH_BUF_LEN];

char_queue ph_send_queue;
char_queue ph_receive_queue;
fifo ph_receive_fifo;
bool ph_initialized = false;

SemaphoreHandle_t ph_send_lockHandle;



extern char rx_buffer[1];
extern nrf_drv_uart_t UARTE_inst1;

bool ph_init(nrf_drv_uart_t *device)
{
  if(ph_initialized) {
    return false;
  }

  init_char_queue(&ph_send_queue, ph_send_queue_buf, PH_BUF_LEN);
  init_char_queue(&ph_receive_queue, ph_receive_queue_buf, PH_BUF_LEN);
  init_fifo(&ph_receive_fifo, ph_receive_fifo_buf, PH_BUF_LEN);

  (void)nrf_drv_uart_rx(device, rx_buffer, 1);

  ph_initialized = true;
  return true;
}

bool islocked=0;

bool ph_send(char data, bool lock)
{
  if(!ph_initialized) {
    return false;
  }

  if (lock && !islocked){
    xSemaphoreTake(ph_send_lockHandle, portMAX_DELAY);
    islocked=1;
  }
  

  bool result = in_char_queue(&ph_send_queue, data);

  if (lock==0){
    islocked=0;
    xSemaphoreGive(ph_send_lockHandle);
    portYIELD();
  }
  
  return result;
}

bool ph_receive(char *data)
{
  if(!ph_initialized) {
    return false;
  }

  return out_char_queue(&ph_receive_queue, data);
}

bool ph_receive_intr(char data)
{
  if(!ph_initialized) {
    return false;
  }

  return in_fifo(&ph_receive_fifo, data);
}

void ph_send_intr()
{
  /*
    You must call this function timely to send the data in the queue
    This function must be modified to use different types of physical devices
  */
  char c;
  SIMCOM_LENGTH_TYPE count = 0;

 

  xSemaphoreTake(ph_send_lockHandle, portMAX_DELAY);

  while(out_char_queue(&ph_send_queue, &c)) {
    ph_send_dma_buf[count] = c;
    count++;
  }
  if (count!=0){
  (void)nrf_drv_uart_tx(&UARTE_inst1, ph_send_dma_buf, count);
}
  
  xSemaphoreGive(ph_send_lockHandle);
  taskYIELD();
}
