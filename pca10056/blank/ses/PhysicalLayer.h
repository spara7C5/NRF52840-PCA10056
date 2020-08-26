#ifndef _PHYSICALLAYER_H
#define _PHYSICALLAYER_H

#include <stdbool.h>

#include "CharQueue.h"
#include "FIFO.h"
#include "SimCom.h"



#define PH_BUF_LEN 500

extern char_queue ph_receive_queue;
extern fifo ph_receive_fifo;

/*
  These functions should be called only by the data link layer
*/
bool ph_init(nrf_drv_uart_t *device);
bool ph_receive(char *data);
bool ph_send(char data, bool lock);

/*
  When data received by the device, call this function
  to tell the physical layer
*/
bool ph_receive_intr(char data);

/*
  Modify it and call it timely for your own project
*/
void ph_send_intr();


#endif
