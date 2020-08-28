/**
 * @file modem.h
 * @brief Modem configuration
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
 **/

#ifndef _MODEM_H
#define _MODEM_H

//Dependencies
#include "core/net.h"
//#include "modem_uart_driver.h"
//C++ guard
#ifdef __cplusplus
   extern "C" {
#endif



#define SIM7000E
// Status Register Bits

#define AT_CHECKMAXNUM_ON_POWERON 40

#define MODEM_STATUS_INIT 0
#define MODEM_STATUS_REGISTERING 1
#define MODEM_STATUS_REGISTERED 2
#define MODEM_STATUS_REGISTRATION_FAILED 3
#define MODEM_STATUS_CONNECTING 4
#define MODEM_STATUS_CONNECTED 5
#define MODEM_STATUS_CONNECTION_FAILED 6
#define MODEM_STATUS_DISCONNECTING 7
#define MODEM_STATUS_DISCONNECTION_FAILED 8
#define MODEM_STATUS_FAILURE 31

//




//Modem related functions
error_t modemInit(NetInterface *interface);
error_t modemCall(NetInterface *interface);
error_t modemClosecall(NetInterface *interface);
error_t modem_PPPopen(NetInterface *interface);
uint32_t getModemStatus();
void modemDeinit(void);

error_t modemSendAtCommand(NetInterface *interface,const char_t *command, char_t *response, size_t size);
error_t modem_initEnvironment(void);

extern const UartDriver uartDriver;
//C++ guard
#ifdef __cplusplus
   }
#endif

#endif
