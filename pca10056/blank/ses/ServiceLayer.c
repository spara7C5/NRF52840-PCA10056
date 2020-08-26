#include "DataLinkLayer.h"
#include "ServiceLayer.h"
#include "semphr.h"


SemaphoreHandle_t sl_send_lockHandle;


void (*callbacks[SL_CALLBACK_NUM])(char, char, const char*, SIMCOM_LENGTH_TYPE) = {0};


bool sl_init(nrf_drv_uart_t *device)
{
  xSemaphoreGive(sl_send_lockHandle);
  return dl_init(device);
}

bool sl_config(char port, void (*callback)(char, char, const char*, SIMCOM_LENGTH_TYPE))
{
  if((int)port < SL_CALLBACK_NUM) {
    callbacks[port] = callback;
    return true;
  } else {
    return false;
  }
}

bool sl_send(char from_port, char to_port, const char *data, SIMCOM_LENGTH_TYPE length)
{
  char sl_send_buf[SL_BUF_LEN];

  xSemaphoreTake(sl_send_lockHandle, portMAX_DELAY);

  sl_send_buf[0] = from_port;
  sl_send_buf[1] = to_port;
  for(SIMCOM_LENGTH_TYPE i = 0; i < length; i++) {
    sl_send_buf[i + 2] = data[i];
  }

  bool result = dl_send(sl_send_buf, length + 2);

  xSemaphoreGive(sl_send_lockHandle);
  portYIELD();

  return result;
}

bool sl_receive_intr()
{
  char sl_receive_buf[SL_BUF_LEN];

  SIMCOM_LENGTH_TYPE length;
  if(dl_receive(sl_receive_buf, &length)) {
    if((unsigned char)sl_receive_buf[1] < SL_CALLBACK_NUM\
      && callbacks[(unsigned char)sl_receive_buf[1]] != 0) {
      callbacks[(unsigned char)sl_receive_buf[1]](sl_receive_buf[0],\
        sl_receive_buf[1],\
        sl_receive_buf + 2,\
        length - 2);
      return true;
    }
  }

  return false;
}
