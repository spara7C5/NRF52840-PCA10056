#include "Verify.h"
#include "DataLinkLayer.h"
#include "PhysicalLayer.h"
#include "semphr.h"
#include "portmacro.h"
#include "task.h"


// Buffer Format:
// [STX] [CRC] [DATA[n]] [ETX]

 SemaphoreHandle_t dl_send_lockHandle;

bool dl_init(nrf_drv_uart_t *device)
{
  return ph_init(device);
}

bool dl_receive(char *data, SIMCOM_LENGTH_TYPE *length)
{
  char dl_receive_buf[DL_BUF_LEN];

  SIMCOM_LENGTH_TYPE length_tmp;
  char c;

  static SIMCOM_LENGTH_TYPE i = 0;
  static bool start_found = false;
  static bool end_found = false;
  static bool esc_found = false;
  while(ph_receive(&c)) {
    /*
      if the buffer if full
    */
    if(i >= DL_BUF_LEN){
      i = 0;
      start_found = false;
      end_found = false;
      esc_found = false;
      continue;
    }

    if(!start_found) {
      if(c == 0x02) {
        start_found = true;
        dl_receive_buf[i] = c;
        i++;
        continue;
      }
    } else {
      if(i < 2) {
        dl_receive_buf[i] = c;
        i++;
        continue;
      } else {
        if(esc_found) {
          esc_found = false;
          dl_receive_buf[i] = c;
          i++;
          continue;
        } else {
          if(c== 0x1B) {
            esc_found = true;
            continue;
          } else if(c == 0x02) {
            dl_receive_buf[0] = c;
            i = 1;
            start_found = true;
            esc_found = false;
            end_found = false;
            continue;
          } else if(c == 0x03) {
            end_found = true;
            length_tmp = i;
            i = 0;
            break;
          } else {
            dl_receive_buf[i] = c;
            i++;
            continue;
          }
        }
      }
    }
  }

  if(start_found && end_found) {
    start_found = false;
    end_found = false;
    esc_found = false;

    if(verify(dl_receive_buf + 2, length_tmp - 2) == dl_receive_buf[1]) {
      for(SIMCOM_LENGTH_TYPE tmp = 2; tmp < length_tmp; tmp++) {
        data[tmp - 2] = dl_receive_buf[tmp];
      }
      *length = length_tmp - 2;
      return true;
    }
  }

  return false;
}

bool dl_send(const char *data, SIMCOM_LENGTH_TYPE length)
{
  char dl_send_buf[DL_BUF_LEN];

  SIMCOM_LENGTH_TYPE i, j;

  if((SIMCOM_DLENGTH_TYPE)(length << 1) + 3 > DL_BUF_LEN) {
    return false;
  }
  xSemaphoreTake(dl_send_lockHandle, portMAX_DELAY);
  

  // STX
  //dl_send_buf[0] = 0x02;
  //dl_send_buf[1] = verify(data, length);

  for(i = 0, j = 0; i < length; i++, j++) {
//    if(data[i] <= 0x1F) {
//      dl_send_buf[j] = 0x1B;
//      j++;
//    }

    dl_send_buf[j] = data[i];
  }

  // ETX
  //dl_send_buf[j] = 0x03;

  bool lock=1;

  for(i = 0; i < j + 1; i++) {
    SIMCOM_LENGTH_TYPE count = 0;

    if (i==j) lock=0;
    while(!ph_send(dl_send_buf[i],lock)) {

      if(count > DL_RETRY_TIMES) {
    	xSemaphoreGive(dl_send_lockHandle);
    	portYIELD();
        return false;
      }
      count++;
    }


  }


  xSemaphoreGive(dl_send_lockHandle);
  portYIELD();

  return true;
}
