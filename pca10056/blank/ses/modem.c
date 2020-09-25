/**
 * @file modem.c
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

//Dependencies
//#include "sam4s.h"



#include "modem.h"
#include "core/net.h"
#include "debug.h"


#define SIM7000E
//Application configuration
//#define APP_PPP_PIN_CODE "1234"

//#define APP_PPP_APN "iliad"
//#define APP_PPP_APN "NXT17.NET"

#define APP_PPP_PHONE_NUMBER "*99#" //"*99***1#"//"+393408362323"// "*99***1#"
#define APP_PPP_PRIMARY_DNS "8.8.8.8"//"0.0.0.0"
#define APP_PPP_SECONDARY_DNS "0.0.0.0"

#define APN_PP_APN "iliad"

#define APP_PPP_TIMEOUT 30000 // used also in AT-COMMAND reply timeout

/**
 * @brief Modem initialization
 * @param[in] interface Underlying network interface
 * @return Error code
 **/


NetInterface *interface;
uint32_t status_register = 0x0;

char_t modem_buffer[4096] ={};
char_t send_buffer[256] ={};
char httpbincer[]="-----BEGIN CERTIFICATE-----\n\
MIIFbjCCBFagAwIBAgIQC6tW9S/J9yHIw1v8WOnMPDANBgkqhkiG9w0BAQsFADBG\n\
MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRUwEwYDVQQLEwxTZXJ2ZXIg\n\
Q0EgMUIxDzANBgNVBAMTBkFtYXpvbjAeFw0yMDAxMTgwMDAwMDBaFw0yMTAyMTgx\n\
MjAwMDBaMBYxFDASBgNVBAMTC2h0dHBiaW4ub3JnMIIBIjANBgkqhkiG9w0BAQEF\n\
AAOCAQ8AMIIBCgKCAQEApFxnGvqYGUel320/nRE281GA6WAOVwY+Npl79AIz45bH\n\
XcxNu+LeMEuGBvrl2EuccQJGXpCY8+sCzFRmcCZsMtTzUdj6R/QbWR7OFjf6Z6w1\n\
AiKccc7iKlRUF/tWAuoLr1b6L9+JfAkJAUL35VV7/vIs9IZ8uWJDhEB2wU6rRZO+\n\
2RBvHGM7oeBNda1/maukjLNYmJ+pxSnrsRTMh3dHUCxZ47h2UZhj2SWCPlW+SMsY\n\
NM/JkURnzSy0lgq/woVeM5g4nOpWuljO1scJ0ZRbR3I+3JveGEd3sQi8e6HkWFtI\n\
mGklhirXxE/t86GP86s+XbwnABIML12h09M3mTJqEwIDAQABo4IChjCCAoIwHwYD\n\
VR0jBBgwFoAUWaRmBlKge5WSPKOUByeWdFv5PdAwHQYDVR0OBBYEFE1H1xvaOuX7\n\
0DFAys411lS5yO+lMCUGA1UdEQQeMByCC2h0dHBiaW4ub3Jngg0qLmh0dHBiaW4u\n\
b3JnMA4GA1UdDwEB/wQEAwIFoDAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUH\n\
AwIwOwYDVR0fBDQwMjAwoC6gLIYqaHR0cDovL2NybC5zY2ExYi5hbWF6b250cnVz\n\
dC5jb20vc2NhMWIuY3JsMCAGA1UdIAQZMBcwCwYJYIZIAYb9bAECMAgGBmeBDAEC\n\
ATB1BggrBgEFBQcBAQRpMGcwLQYIKwYBBQUHMAGGIWh0dHA6Ly9vY3NwLnNjYTFi\n\
LmFtYXpvbnRydXN0LmNvbTA2BggrBgEFBQcwAoYqaHR0cDovL2NydC5zY2ExYi5h\n\
bWF6b250cnVzdC5jb20vc2NhMWIuY3J0MAwGA1UdEwEB/wQCMAAwggEEBgorBgEE\n\
AdZ5AgQCBIH1BIHyAPAAdgDuS723dc5guuFCaR+r4Z5mow9+X7By2IMAxHuJeqj9\n\
ywAAAW+2QhJbAAAEAwBHMEUCIQCBAIJ4tACBrdHwB4ZnGIGTy3/9FxuZ9GIoHgfX\n\
5RjefQIgeXY+x7oWQmIShXCrBSdqeTYXrsxQcWE6ZpAyQxcsdUUAdgCHdb/nWXz4\n\
jEOZX73zbv9WjUdWNv9KtWDBtOr/XqCDDwAAAW+2QhKqAAAEAwBHMEUCIBDMYim2\n\
sF8eHpW1Z7/yQ1liTwa8IRSjidBd9ZVIwe6mAiEA7DPOTaRgc/cH3OzIGSu6dLae\n\
e5F/YRkmC9TikWiWTC8wDQYJKoZIhvcNAQELBQADggEBAIIERQYarXyjBoi2yjhr\n\
7WTjbsPLTUSUFVK2f+qckuDJfoX9bW1PLShve5R8WWgIBu4eZYyUDS+BzXXSV2wg\n\
NscAL9TkxobI/N0HiI5iGtJuI8dVIsFSRMG9IWRz96/pqRcTMz5GlIhAurB3aR+S\n\
MnwAYsrNBHG+rqgUwNVn0h2XoVJe3VxrV2QwTH5kzBwG/Ju1+Khqkvs+9/M3UrJg\n\
qPGwissH0W8HgYWhKVISkN2ui55RgbHXQHYDX0uYgGK6iRMxHHlOR1vOqRgEvVGF\n\
5g8CcK3EzritKaHkD6bf0pnSE/E7cjKXzgB4l+58dsNcIVo9YgID4xYGS6paetso\n\
XHE=\n\
-----END CERTIFICATE-----";


error_t modemSendAtCommandPRE(NetInterface *interface,const char_t *command, char_t *response, size_t size, char* str);
error_t modemSendAtCommandPLUS(NetInterface *interface,const char_t *command, char_t *response, size_t size, char* str);
error_t modemSendAtCommandHTTP(NetInterface *interface,const char_t *command, char_t *response, size_t size);


error_t modemInit(NetInterface *interface)
{
   error_t error;



   status_register = 0x0;
   TRACE_INFO("Modem Init...\r\n");


   //Debug message
   TRACE_INFO("Modem Init...\r\n");

   TRACE_INFO("Power On Unit...\r\n");

//#ifdef SIM7000E
////   HAL_GPIO_WritePin(MODEM_ON_GPIO_Port,MODEM_ON_Pin,GPIO_PIN_SET);
////   osDelayTask(1000);
////   TRACE_INFO("Power Key 1500 ms to shut down if ON or power ON if OFF...\r\n");
////   HAL_GPIO_WritePin(POWERKEY_GPIO_Port,POWERKEY_Pin,GPIO_PIN_SET);
////   osDelayTask(1500);
////   HAL_GPIO_WritePin(POWERKEY_GPIO_Port,POWERKEY_Pin,GPIO_PIN_RESET);
//
////   TRACE_INFO("Power Key 500 ms to power ON if OFF or keep ON if ON...\r\n");
////   HAL_GPIO_WritePin(POWERKEY_GPIO_Port,POWERKEY_Pin,GPIO_PIN_SET);
////   osDelayTask(500);
////   HAL_GPIO_WritePin(POWERKEY_GPIO_Port,POWERKEY_Pin,GPIO_PIN_RESET);
////
////   TRACE_INFO("Waiting 10000 ms for modem initialization...\r\n");
////   osDelayTask(10000);
//#else
//   //osDelayTask(30000);
//
//   TRACE_INFO("Checking if modem is already on...");
//   error = modemSendAtCommand(interface, "AT\r", modem_buffer, sizeof(modem_buffer));
//   if (error){ // if no reply, modem is off, so switch it on
//	   HAL_GPIO_WritePin(MODEM_ON_GPIO_Port,MODEM_ON_Pin,GPIO_PIN_SET);
//	   osDelayTask(1000);
//	   TRACE_INFO("Power Key 2500 ms...\r\n");
//	   HAL_GPIO_WritePin(POWERKEY_GPIO_Port,POWERKEY_Pin,GPIO_PIN_SET);
//	   osDelayTask(2500);
//	   TRACE_INFO("Waiting 4000 ms for modem initialization...\r\n");
//	   HAL_GPIO_WritePin(POWERKEY_GPIO_Port,POWERKEY_Pin,GPIO_PIN_RESET);
//	   osDelayTask(4000);
//   }
//
//#endif
//   //Debug message
//
//#ifdef MODEM_FW_UPDATE
//   TRACE_INFO("FW BLOCKED FOR MODEM UPDATE...\r\n");
//   while(1);
//#endif
//
   TRACE_INFO("Modem Initializing starts:\r\n");
  
       ///// MESSAGES FOR SYNCHRONIZATION /////
       for(uint8_t i=0;i<4;i++){
       error = pppSendAtCommand(interface, "AT\r");
       vTaskDelay(1000);
       }
       /////////////////////////////////////////

       error = modemSendAtCommand(interface, "AT\r", modem_buffer, 3);

//     while(1){
//        error = modemSendAtCommand(interface, "AT\r", modem_buffer, 3);
//        //Any error to report?
//       if(!error)break;
//        
//     }
//
//      
//    //error = modemSendAtCommand(interface, "AT+CRESET\r", modem_buffer, sizeof(modem_buffer));
//  
//
//   //Module identification
//   error = modemSendAtCommand(interface, "AT+CFUN=1,0\r", modem_buffer, 12);
//
//#ifdef SIM7000E
//   error = modemSendAtCommand(interface, "AT+CGSN\r", modem_buffer, sizeof(modem_buffer));
//#else
//   error = modemSendAtCommand(interface, "AT+QGSN\r", modem_buffer, sizeof(modem_buffer));
//#endif
//   //Any error to report?
//   if(error){
//	   return error;
//   }
//#ifdef SIM7000E
//   char* resOK = strstr(modem_buffer, "OK");
//     if (resOK!=NULL){
//  	   char* ptrcgsn = strstr(modem_buffer, "+CGSN");
//  	   if (ptrcgsn != NULL){
//
//  		   ptrcgsn+=5;
//  		   char field[32];
//  		   memset(field,0,32);
//  		   strcpy(field,ptrcgsn);
//  		   uint32_t start = 0;
//  		   uint32_t fieldLen = 0;
//  		   uint32_t lastIndexFound = 0;
//		   uint8_t found=0;
//
//  		   char subfield[32];
//  		   memset(subfield,0,32);
//
//
//		
//
//
//
//  	   }
//
//     }
//#else
//   char* resOK = strstr(modem_buffer, "OK");
//   if (resOK!=NULL){
//	   char* pointer = strstr(modem_buffer, "+QGSN:");
//	   if (pointer != NULL){
//
//		   char field[32];
//		   memset(field,0,32);
//		   uint32_t fieldLen = 0;
//		   uint32_t lastIndexFound = 0;
//
//		   uint8_t found = findField(modem_buffer, 0, strnlen(modem_buffer,256) ,'\"',field, &fieldLen ,&lastIndexFound);
//		   if (found){
//
//			   memset(field,0,32);
//			   uint32_t start = lastIndexFound + 1;
//			   fieldLen = 0;
//
//			   found = findField(modem_buffer, start, strnlen(modem_buffer,256) ,'\"',field, &fieldLen ,&lastIndexFound);
//
//			   if (found && isANumber(field) && strnlen(field,32)==15){
//
//				   if (memcmp(unitDescriptor.imei,field,15)){
//					   memcpy(unitDescriptor.imei,field,15);
//					   if(!writeDescriptor(&unitDescriptor)){
//						   TRACE_DEBUG("Error: Can't update IMEI on memory\n");
//					   }
//				   }
//
//			   }
//		   }
//
//
//	   }
//
//   }
//
//#endif
//
//
//      
//   //Module identification
//   error = modemSendAtCommand(interface, "AT+CGMM\r", modem_buffer, sizeof(modem_buffer));
//   //Any error to report?
//   if(error)
//   {
//	   return error;
//   }
//
    error = modemSendAtCommand(interface, "ATE1V1\r", modem_buffer, sizeof(modem_buffer));  



 

//   //Module identification
    error = modemSendAtCommand(interface, "ATI\r", modem_buffer, sizeof(modem_buffer));
//    //Any error to report?
//    if(error)
//    {
// 	   return error;
//    }
//   //Get software version
   error = modemSendAtCommand(interface, "AT+CGMR\r", modem_buffer, sizeof(modem_buffer));
//   //Any error to report?
//   if(error)
//      {
//   	   return error;
//      }
//
//   //Enable verbose mode
   error = modemSendAtCommand(interface, "AT+CMEE=2\r", modem_buffer, sizeof(modem_buffer));
//   //Any error to report?
//   if(error)
//      {
//   	   return error;
//      }
//
//   //Enable hardware flow control
//   error = modemSendAtCommand(interface, "AT+IFC=0,0\r", modem_buffer, sizeof(modem_buffer));
//   //Any error to report?
//   if(error)
//      {
//   	   return error;
//      }
//
//
//
//   //Query the ICCID of the SIM card
//   error = modemSendAtCommand(interface, "AT+CCID\r", modem_buffer, sizeof(modem_buffer));
//   //Any error to report?
//   if(error)
//      {
//   	   return error;
//      }
//
//   //Check if the SIM device needs the PIN code (if no, reply is :"EADYOK")
//   error = modemSendAtCommand(interface, "AT+CPIN?\r", modem_buffer, sizeof(modem_buffer));
//   //Any error to report?
//   if(error)
//      {
//   	   return error;
//      }
//
//   //Check whether the PIN code is required
//   if(strstr(modem_buffer, "+CPIN: SIM PIN") != NULL)
//   {
//
//      //Debug message
//      TRACE_DEBUG("PIN code is required!\r\n");
//      //Report an error
//      return ERROR_FAILURE;
//    }
//
//   //Check if the ATH command is enables
//     error = modemSendAtCommand(interface, "AT+CPSI?\r", modem_buffer, sizeof(modem_buffer));
//     //Any error to report?
//     if(error)
//        {
//     	   return error;
//        }
//
//  
//
//      // preferred mode selection
//     error = modemSendAtCommand(interface, "AT+CNMP=2\r", modem_buffer, sizeof(modem_buffer));
//        //Any error to report?
//	if(error)
//	   {
//		   return error;
//	   }
//
//  
//
//
//



//
//
//      //Any error to report?
//      if(error)
//   	{
//   		return error;
//   	}

  

      //////     APN MANUAL CONFIGURATION
    error = modemSendAtCommand(interface, "AT+CGNAPN\r", modem_buffer, sizeof(modem_buffer));    
    error = modemSendAtCommand(interface, "AT+CFUN=0\r", modem_buffer, sizeof(modem_buffer)); 
//
    error = modemSendAtCommand(interface, "AT+CGDCONT=1,\"IP\",\"wap.tim.it\"\r", modem_buffer, sizeof(modem_buffer)); 
    error = modemSendAtCommand(interface, "AT+CFUN=1\r", modem_buffer, sizeof(modem_buffer)); 
    osDelayTask(5000);


   //Wait for the module to be registered
   while(1)
   {
      //Check if the module is registered

	  error = modemSendAtCommand(interface, "AT+CGATT?\r", modem_buffer, sizeof(modem_buffer));  
		//Any error to report?
		if(error)
			  {
			   return error;
			  }

	   error = modemSendAtCommand(interface, "AT+CSQ\r", modem_buffer, sizeof(modem_buffer));
			   //Any error to report?
		if(error)
			 {
			   return error;
			 }

      error = modemSendAtCommand(interface, "AT+CGREG?\r", modem_buffer, sizeof(modem_buffer));
      //Any error to report?
      if(error)
            {
         	   return error;
            }

      //Check registration status
      if(strstr(modem_buffer, "+CGREG: 0,0") != NULL)
      {
    	 
    	 //provato ad usare i COPS ma ritorna CMEERROR: SIM NOT INSERTED
//    	 error = modemSendAtCommand(interface, "AT+COPS=2\r", modem_buffer, sizeof(modem_buffer));
//    	 error = modemSendAtCommand(interface, "AT+COPS=0\r", modem_buffer, sizeof(modem_buffer));
    	 return ERROR_FAILURE;
//    	 if(error)
//    	       {
//    	    	   SetBit(status_register,MODEM_STATUS_FAILURE);
//    	    	   return error;
//    	       }

         //Not registered
      }
       if(strstr(modem_buffer, "+CGREG: 0,1") != NULL)
      {
         //Registered (home network)
    	  break;
      }
      else if(strstr(modem_buffer, "+CGREG: 0,5") != NULL)
      {
         //Registered (roaming)
         break;
      }


      //Successful initialization
      osDelayTask(1000);
   } // end wait module to be registered



    error = modemSendAtCommand(interface, "AT+CGATT?\r", modem_buffer, sizeof(modem_buffer));  

    
    //Query the APN delivered by the network after the CAT-M or NB-IOT network is successfully registered.
    error = modemSendAtCommand(interface, "AT+CGNAPN\r", modem_buffer, sizeof(modem_buffer));  
//

    memcpy(&send_buffer[0],"AT+CNCFG=0,1,\"ibox.tim.it\"\r\0",256);
//
    error = modemSendAtCommand(interface,send_buffer , modem_buffer, strlen(send_buffer));
//
    error = modemSendAtCommandPLUS(interface, "AT+CNACT=0,1\r", modem_buffer, sizeof(modem_buffer),"ACTIVE");  
//
    error = modemSendAtCommand(interface, "AT+CNACT?\r", modem_buffer, sizeof(modem_buffer)); 
   //Successful processing

   return NO_ERROR;
}


/**
 * @brief Establish a PPP session
 * @param[in] interface Underlying network interface
 * @return Error code
 **/

error_t modemCall(NetInterface *interface)
{
   error_t error;
   char post_buffer[256];

 
    ////////////// SMS PROCEDURE TESTED ON SIM7000-BK  ////////////////////////////////////
//    error = modemSendAtCommand(interface, "AT+CMGF=1\r", modem_buffer, sizeof(modem_buffer));
//    error = modemSendAtCommand(interface, "AT+CMGS=\"+393408362323\"\r", modem_buffer, sizeof(modem_buffer));
//    sprintf(&modem_buffer[0],"CiaoDaSIM7000-tick:%u\x1A",osGetSystemTime());
//    error = modemSendAtCommand(interface, modem_buffer, modem_buffer, sizeof(modem_buffer));
    /////////////////////////////////////////////////////////////////////////////////////////




//    error = modemSendAtCommand(interface, "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r", modem_buffer, sizeof(modem_buffer));
//    error = modemSendAtCommand(interface, "AT+SAPBR=3,1,\"APN\",\"wap.tim.it\"\r", modem_buffer, sizeof(modem_buffer));
//    if((modemSendAtCommand(interface, "AT+SAPBR=2,1\r", modem_buffer, sizeof(modem_buffer)))!=NO_ERROR){
//      while((modemSendAtCommand(interface, "AT+SAPBR=1,1\r", modem_buffer, sizeof(modem_buffer)))!=NO_ERROR)
//      {
//      vTaskDelay(1000);
//      };
//    }
//    error = modemSendAtCommand(interface, "AT+SAPBR=2,1\r", modem_buffer, sizeof(modem_buffer));
//    while((modemSendAtCommand(interface, "AT+HTTPINIT\r", modem_buffer, sizeof(modem_buffer)))!=NO_ERROR){
//      modemSendAtCommand(interface,"AT+HTTPTERM\r",modem_buffer, sizeof(modem_buffer)); 
//    }
//error = modemSendAtCommand(interface, "AT+CSSLCFG=\"sslversion\",1,3\r", modem_buffer, sizeof(modem_buffer));
//error = modemSendAtCommand(interface, "AT+SHSSL=1,\"\"\r", modem_buffer, sizeof(modem_buffer));

//error = modemSendAtCommand(interface, "AT+SHCONF=\"URL\",\"http://httpbin.org\"\r", modem_buffer, sizeof(modem_buffer));
//error = modemSendAtCommand(interface, "AT+SHCONF=\"BODYLEN\",1024\r", modem_buffer, sizeof(modem_buffer));
//error = modemSendAtCommand(interface, "AT+SHCONF=\"HEADERLEN\",350\r", modem_buffer, sizeof(modem_buffer));
//error = modemSendAtCommand(interface, "AT+SHCONN\r", modem_buffer, sizeof(modem_buffer));
//error = modemSendAtCommand(interface, "AT+SHSTATE?\r", modem_buffer, sizeof(modem_buffer));
//error = modemSendAtCommand(interface, "AT+SHCHEAD\r", modem_buffer, sizeof(modem_buffer));
//error = modemSendAtCommand(interface, "AT+SHAHEAD=\"Content-Type\",\"application/x- www-form-urlencoded\"\r", modem_buffer, sizeof(modem_buffer));
//error = modemSendAtCommand(interface, "AT+SHAHEAD=\"Cache-control\",\"no-cache\"\r", modem_buffer, sizeof(modem_buffer));
//error = modemSendAtCommand(interface, "AT+SHAHEAD=\"Connection\",\"keep-alive\"\r", modem_buffer, sizeof(modem_buffer));
//error = modemSendAtCommand(interface, "AT+SHAHEAD=\"Accept\",\"*/*\"\r", modem_buffer, sizeof(modem_buffer));
//error = modemSendAtCommand(interface, "AT+SHBOD=\" {\"title\":\"Hello http server\"}\",29\r", modem_buffer, sizeof(modem_buffer));
//error = modemSendAtCommand(interface, "AT+SHREQ=\"/post\",3\r", modem_buffer, sizeof(modem_buffer));
//error = modemSendAtCommand(interface, "AT+SHREAD=0,457\r", modem_buffer, sizeof(modem_buffer));
//error = modemSendAtCommand(interface, "AT+SHDISC\r", modem_buffer, sizeof(modem_buffer));





    //////////////////// HTTP CONFIG ///////////////////////////////
    
//    error = modemSendAtCommand(interface, "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r", modem_buffer, sizeof(modem_buffer));
//    error = modemSendAtCommand(interface, "AT+SAPBR=3,1,\"APN\",\"wap.tim.it\"\r", modem_buffer, sizeof(modem_buffer));
//    if((modemSendAtCommand(interface, "AT+SAPBR=2,1\r", modem_buffer, sizeof(modem_buffer)))!=NO_ERROR){
//      while((modemSendAtCommand(interface, "AT+SAPBR=1,1\r", modem_buffer, sizeof(modem_buffer)))!=NO_ERROR)
//      {
//      vTaskDelay(1000);
//      };
//    }
//    error = modemSendAtCommand(interface, "AT+SAPBR=2,1\r", modem_buffer, sizeof(modem_buffer));
//    while((modemSendAtCommand(interface, "AT+HTTPINIT\r", modem_buffer, sizeof(modem_buffer)))!=NO_ERROR){
//      modemSendAtCommand(interface,"AT+HTTPTERM\r",modem_buffer, sizeof(modem_buffer)); 
//    };



    ////////// WRITE AND READ EXAMPLE ///////////////

//    error = modemSendAtCommand(interface, "AT+CFSINIT\r", modem_buffer, sizeof(modem_buffer)); 
//    error = modemSendAtCommand(interface, "AT+CFSWFILE=3,\"pippo.txt\",0,4,10000\r", modem_buffer, sizeof(modem_buffer)); 
//    error = modemSendAtCommand(interface, "ciao", modem_buffer, sizeof(modem_buffer)); 
//    error = modemSendAtCommand(interface, "AT+CFSRFILE=3,\"pippo.txt\",0,4,10000\r", modem_buffer, sizeof(modem_buffer)); 
//    while(1){}

    ////////// END WRITE AND READ EXAMPLE ///////////////



    //////////////// END HTTP CONFIG ///////////////////////////////////////

    //////// HTTP-S  PRELIMINARY FUNCTION ///////////////
    //memset(httpbincer,0)
    
    error = modemSendAtCommand(interface, "AT+CFSGFRS?\r", modem_buffer, sizeof(modem_buffer)); 
    error = modemSendAtCommand(interface, "AT+CFSINIT\r", modem_buffer, sizeof(modem_buffer)); 
    error = modemSendAtCommand(interface, "AT+CFSWFILE=?\r", modem_buffer, sizeof(modem_buffer)); 
    memset(send_buffer,0,256);
    sprintf(&send_buffer[0],"AT+CFSWFILE=3,\"httpbin_root_ca.cer\",0,%u,10000\r",strlen(httpbincer));
    error = modemSendAtCommand(interface, send_buffer, modem_buffer, sizeof(modem_buffer)); 
    error = modemSendAtCommand(interface, httpbincer, modem_buffer, sizeof(modem_buffer)); 
    error = modemSendAtCommand(interface, "AT+CFSTERM\r", modem_buffer, sizeof(modem_buffer));
    //error = modemSendAtCommand(interface, "AT+CSSLCFG=\"sslversion\",1,3\r", modem_buffer, sizeof(modem_buffer));
    error = modemSendAtCommand(interface, "AT+CSSLCFG=\"convert\",2,\"httpbin_root_ca.cer\"\r", modem_buffer, sizeof(modem_buffer));



    /////////////////////////////////////////////////////////////

    ////////// SIM7070 HTTP GET /////////////////////////////////
    
    error = modemSendAtCommand(interface, "AT+SHSSL=1,\"httpbin_root_ca.cer\"\r", modem_buffer, sizeof(modem_buffer));
    error = modemSendAtCommand(interface, "AT+SHCONF=\"URL\",\"https://httpbin.org\"\r", modem_buffer, sizeof(modem_buffer)); 
    error = modemSendAtCommand(interface, "AT+SHCONF=\"BODYLEN\",1024\r", modem_buffer, sizeof(modem_buffer)); 
    error = modemSendAtCommand(interface, "AT+SHCONF=\"HEADERLEN\",350\r", modem_buffer, sizeof(modem_buffer)); 
    error = modemSendAtCommand(interface, "AT+SHCONF=\"TIMEOUT\",30\r", modem_buffer, sizeof(modem_buffer));
    error = modemSendAtCommand(interface, "AT+SHCONN\r", modem_buffer, sizeof(modem_buffer));
    error = modemSendAtCommand(interface, "AT+SHCONN\r", modem_buffer, sizeof(modem_buffer)); 
    error = modemSendAtCommand(interface, "AT+SHSTATE?\r", modem_buffer, sizeof(modem_buffer)); 
    error = modemSendAtCommand(interface, "AT+SHCHEAD\r", modem_buffer, sizeof(modem_buffer)); 
    error = modemSendAtCommand(interface, "AT+SHAHEAD=\"User-Agent\",\"curl/7.47.0\"\r", modem_buffer, sizeof(modem_buffer)); 
    error = modemSendAtCommand(interface, "AT+SHAHEAD=\"Cache-control\",\"no-cache\"\r", modem_buffer, sizeof(modem_buffer)); 
    error = modemSendAtCommand(interface, "AT+SHAHEAD=\"Connection\",\"keep-alive\"\r", modem_buffer, sizeof(modem_buffer)); 
    error = modemSendAtCommand(interface, "AT+SHAHEAD=\"Accept\",\"*/*\"\r", modem_buffer, sizeof(modem_buffer)); 
    error = modemSendAtCommandPLUS(interface, "AT+SHREQ=\"/get?user=jack&password=123\",1\r", modem_buffer, sizeof(modem_buffer),"SHREQ");
    char *token=strtok(modem_buffer, ",");
    uint8_t cnt=1;
    while(cnt<2+1){
      token = strtok(NULL, ",");
      cnt++;
    }
    uint16_t getlen=atoi(token);
    memset(send_buffer,0,256);
    sprintf(&send_buffer[0],"AT+SHREAD=0,%u\r",getlen);
    error = modemSendAtCommandHTTP(interface, send_buffer, modem_buffer, sizeof(modem_buffer));
    error = modemSendAtCommand(interface, "AT+SHDISC\r", modem_buffer, sizeof(modem_buffer));
    ////////// END SIM7070 HTTP GET /////////////////////////////



    ///////////////////// SIM7070 HTTP POST///////////////////////////
//    error = modemSendAtCommand(interface, "AT+CSSLCFG=\"sslversion\",1,3\r", modem_buffer, sizeof(modem_buffer));
//    error = modemSendAtCommand(interface, "AT+SHSSL=1,\"\"\r", modem_buffer, sizeof(modem_buffer));
//    error = modemSendAtCommand(interface, "AT+SHCONF=\"URL\",\"http://httpbin.org\"\r", modem_buffer, sizeof(modem_buffer)); 
//    error = modemSendAtCommand(interface, "AT+SHCONF=\"BODYLEN\",1024\r", modem_buffer, sizeof(modem_buffer)); 
//    error = modemSendAtCommand(interface, "AT+SHCONF=\"HEADERLEN\",350\r", modem_buffer, sizeof(modem_buffer)); 
//    error = modemSendAtCommand(interface, "AT+SHCONN\r", modem_buffer, sizeof(modem_buffer)); 
//    error = modemSendAtCommand(interface, "AT+SHSTATE?\r", modem_buffer, sizeof(modem_buffer)); 
//    error = modemSendAtCommand(interface, "AT+SHCHEAD\r", modem_buffer, sizeof(modem_buffer)); 
//    error = modemSendAtCommand(interface, "AT+SHAHEAD=\"Content-Type\",\"application/xwww-form-urlencoded\"\r", modem_buffer, sizeof(modem_buffer)); 
//    error = modemSendAtCommand(interface, "AT+SHAHEAD=\"Cache-control\",\"no-cache\"\r", modem_buffer, sizeof(modem_buffer)); 
//    error = modemSendAtCommand(interface, "AT+SHAHEAD=\"Connection\",\"keep-alive\"\r", modem_buffer, sizeof(modem_buffer)); 
//    error = modemSendAtCommand(interface, "AT+SHAHEAD=\"Accept\",\"*/*\"\r", modem_buffer, sizeof(modem_buffer)); 
//    error = modemSendAtCommand(interface, "AT+SHCPARA\r", modem_buffer, sizeof(modem_buffer)); 
//    error = modemSendAtCommand(interface, "AT+SHPARA=\"product\",\"apple\"\r", modem_buffer, sizeof(modem_buffer));
//    error = modemSendAtCommand(interface, "AT+SHPARA=\"price\",\"1\"\r", modem_buffer, sizeof(modem_buffer));
//    error = modemSendAtCommandPLUS(interface, "AT+SHREQ=\"/post\",3\r", modem_buffer, sizeof(modem_buffer),"SHREQ");

//    char *token=strtok(modem_buffer, ",");
//    uint8_t cnt=1;
//    while(cnt<2+1){
//      token = strtok(NULL, ",");
//      cnt++;
//    }
//    uint16_t getlen=atoi(token);
//    sprintf(&send_buffer[0],"AT+SHREAD=0,%u\r",getlen);
//    error = modemSendAtCommandHTTP(interface, send_buffer, modem_buffer, sizeof(modem_buffer));
//    error = modemSendAtCommand(interface, "AT+SHDISC\r", modem_buffer, sizeof(modem_buffer));

    ////////// END HTTP POST /////////////////////////////

    ///// FINAL HTTP //////////////////////////
//    error = modemSendAtCommand(interface,"AT+HTTPREAD\r",modem_buffer , sizeof(modem_buffer)); 
//    error = modemSendAtCommand(interface,"AT+HTTPTERM\r",modem_buffer, sizeof(modem_buffer)); 
    /////////////////////////////////////////

    //////////////////// EMAIL ////////////////////////
//    error = modemSendAtCommand(interface,"AT+CNTPCID?\r",modem_buffer , sizeof(modem_buffer));
//    error = modemSendAtCommand(interface,"AT+CNTP=\"ntp1.inrim.it\",8\r",modem_buffer , sizeof(modem_buffer));
//    error = modemSendAtCommandPLUS(interface,"AT+CNTP\r",modem_buffer , sizeof(modem_buffer),"+CNTP");
//
//    error = modemSendAtCommand(interface,"AT+CCLK?\r",modem_buffer , sizeof(modem_buffer));
//    error = modemSendAtCommand(interface,"AT+EMAILCID=0\r",modem_buffer , sizeof(modem_buffer));
//    error = modemSendAtCommand(interface,"AT+EMAILTO=3\r",modem_buffer , sizeof(modem_buffer));
//    error = modemSendAtCommand(interface,"AT+SMTPSRV=\"mail.sim.com\",25\r",modem_buffer , sizeof(modem_buffer));
//    error = modemSendAtCommand(interface,"AT+SMTPAUTH=1,\"ohn\",\"123456\"\r",modem_buffer , sizeof(modem_buffer));
//    error = modemSendAtCommand(interface,"AT+SMTPFROM=\"john@sim.com\",\"john\"\r",modem_buffer , sizeof(modem_buffer));
//    error = modemSendAtCommand(interface,"AT+SMTPRCPT=0,0,\"paracchino@hotmail.it\",\"ste\"\r",modem_buffer , sizeof(modem_buffer));
//    error = modemSendAtCommand(interface,"AT+SMTPSUB=\"test\"\r",modem_buffer , sizeof(modem_buffer));
//    error = modemSendAtCommand(interface,"AT+SMTPBODY=19\r",modem_buffer , sizeof(modem_buffer));
//    error = modemSendAtCommand(interface,"AT+SMTPSEND\r",modem_buffer , sizeof(modem_buffer));
   

    /////////// END EMAIL  ////////////////////////////



//        sprintf(modem_buffer, "AT+CGDCONT=1,\"PPP\",\"%s\"\r", APN_PP_APN);
//   //Format AT+CGDCONT command
//
//	 //Send AT command
//	 error = modemSendAtCommand(interface, modem_buffer, modem_buffer, sizeof(modem_buffer));
//
//	 error = modemSendAtCommand(interface, "AT+CGDCONT?\r", modem_buffer, sizeof(modem_buffer));
//
//#ifdef m2msupportmethod
//    error = modemSendAtCommand(interface, "AT+CGATT=1\r", modem_buffer, sizeof(modem_buffer));
//    error = modemSendAtCommand(interface, "AT+CGACT=1,1\r", modem_buffer, sizeof(modem_buffer));
//    error = modemSendAtCommand(interface, "AT+CGPADDR=1\r", modem_buffer, sizeof(modem_buffer));
//    error = modemSendAtCommand(interface, "AT+CGDCONT?\r", modem_buffer, sizeof(modem_buffer));
//    if(error)
//	{
//		SetBit(status_register,MODEM_STATUS_CONNECTION_FAILED);
//		return error;
//	}
//#else
//   //Format ATDT command
//   sprintf(modem_buffer, "ATD%s\r", APP_PPP_PHONE_NUMBER);
//   error=modemSendAtCommand(interface, modem_buffer, modem_buffer, sizeof(modem_buffer));
//   //Any error to report?
//   if(error)
//     	{
//     		return error;
//     	}
//
//      //Check response
//      else if(strstr(modem_buffer, "NO CARRIER") != NULL)
//      {
//         //Report an error
//         return ERROR_NO_CARRIER;
//      }
//      else if(strstr(modem_buffer, "CONNECT") == NULL)
//      {
//         //Report an error
//         return ERROR_FAILURE;
//      }
//#endif



   return error;

}

error_t modem_PPPopen(NetInterface *interface)
{

	error_t error;
    Ipv4Addr ipv4Addr;

   //Clear local IPv4 address
   ipv4StringToAddr("0.0.0.0", &ipv4Addr);
   ipv4SetHostAddr(interface, ipv4Addr);

   //Clear peer IPv4 address
   ipv4StringToAddr("0.0.0.0", &ipv4Addr);
   ipv4SetDefaultGateway(interface, ipv4Addr);

   //Set primary DNS server
   ipv4StringToAddr(APP_PPP_PRIMARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(interface, 0, ipv4Addr);

   //Set secondary DNS server
   ipv4StringToAddr(APP_PPP_SECONDARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(interface, 1, ipv4Addr);

   //Set username and password
   pppSetAuthInfo(interface, "", "");

   //Debug message
   TRACE_INFO("Establishing PPP connection...\r\n");

   //Establish a PPP connection
   error = pppConnect(interface);
   //Any error to report?
   if(error)
   {
	   TRACE_INFO("PPP connection error: %u \r\n",error);
      return error;
   }

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Close a PPP session
 * @param[in] interface Underlying network interface
 * @return Error code
 **/

error_t modemClosecall(NetInterface *interface)
{
   error_t error = NO_ERROR;


   //Debug message


   sprintf(modem_buffer, "+++\r");
   	//Send AT command
   	error=modemSendAtCommand(interface, modem_buffer, modem_buffer, sizeof(modem_buffer));
   //Format ATDT command
   sprintf(modem_buffer, "ATH0\r");
	//Send AT command
	error=modemSendAtCommand(interface, modem_buffer, modem_buffer, sizeof(modem_buffer));
	//Any error to report?

	if(error || strstr(modem_buffer, "OK") == NULL)
        {
		return ERROR_FAILURE;
	}


	TRACE_DEBUG("Call closed!\r\n");

   return error;
}

/**


/**
 * @brief Send an AT command to the modem
 * @param[in] interface Underlying network interface
 * @param[in] command AT command
 * @param[in] response Pointer to the modem_buffer where to copy the modem's response
 * @param[in] size Size of the response modem_buffer
 * @return Error code
 **/


error_t modemSendAtCommand(NetInterface *interface,const char_t *command, char_t *response, size_t size)
{
   error_t error;
   size_t n;

   //Debug message
    
   //if (size<50){
   TRACE_DEBUG("AT command:  %s\n",command);
  //}
   //Send AT command
   error = pppSendAtCommand(interface, command);
    vTaskDelay(5);
   //Check status code
   if(!error)
   {
      //Size of the response modem_buffer
      n = 0;

      //Loop as long as necessary
      while(n < size)
      {
         //Wait for a response from the modem
         error = pppReceiveAtCommand(interface, response + n, size - n);

         //Check status code
         if(error)
         {
            //Exit immediately
            break;
         }


         //Status string received?
         if(strstr(response, "OK") ||
            strstr(response, "CONNECT") ||
            strstr(response, "RING") ||
            strstr(response, "DEACTIVE") ||
            strstr(response, "NO CARRIER") ||
            strstr(response, "~!NO CARRIER") ||
            strstr(response, "ERROR") ||
            strstr(response, "DOWNLOAD") ||
            strstr(response, "NO ANSWER") ||
	    strstr(response, "NORMAL POWER DOWN")
			)
         {

            TRACE_DEBUG("AT response: %s\n", response);
            //Exit immediately

            break;
         }



         //Update the length of the response
         n = strlen(response);

         
      }

   }

   //Return status code
   return error;
}


error_t modemSendAtCommandPRE(NetInterface *interface,const char_t *command, char_t *response, size_t size,char* str)
{
   error_t error;
   size_t n;

   //Debug message
  
   TRACE_DEBUG("AT command:  %s\n",command);
  
   //Send AT command
   error = pppSendAtCommand(interface, command);
    vTaskDelay(5);
   //Check status code
   if(!error)
   {
      //Size of the response modem_buffer
      n = 0;

      //Loop as long as necessary
      while(n < size)
      {
         //Wait for a response from the modem
         error = pppReceiveAtCommand(interface, response + n, size - n);

         //Check status code
         if(error)
         {
            //Exit immediately
            break;
         }

  

         //Status string received?
         if(strstr(response, str))
         {

            TRACE_DEBUG("AT response: %s\n", response);
            //Exit immediately

            break;
         }



         //Update the length of the response
         n = strlen(response);

         
      }

   }

   //Return status code
   return error;
}

error_t modemSendAtCommandPLUS(NetInterface *interface,const char_t *command, char_t *response, size_t size, char* str)
{
   error_t error;
   size_t n;

   //Debug message
  
   TRACE_DEBUG("AT command:  %s\n",command);
  
   //Send AT command
   error = pppSendAtCommand(interface, command);
    vTaskDelay(5);
   //Check status code
   if(!error)
   {
      //Size of the response modem_buffer
      n = 0;

      //Loop as long as necessary
      while(n < size)
      {
         //Wait for a response from the modem
         error = pppReceiveAtCommand(interface, response + n, size - n);

         //Check status code
         if(error)
         {
            //Exit immediately
            break;
         }

         if(strstr(response, "SIM not inserted")){
        	 error=ERROR_SIM_NOT_INSERTED;
        	 TRACE_DEBUG("AT response: %s", response);
        	 break;
         }


         //Status string received?
         if(strstr(response, "OK") ||
            strstr(response, "CONNECT") ||
            strstr(response, "RING") ||
            strstr(response, "+HTTPREAD:") ||
            strstr(response, "NO CARRIER") ||
            strstr(response, "~!NO CARRIER") ||
            strstr(response, "ERROR") ||
            strstr(response, "DOWNLOAD") ||
            strstr(response, "NO ANSWER") ||
	    strstr(response, "NORMAL POWER DOWN")
			)
         {

            TRACE_DEBUG("AT response: %s\n", response);
            //Exit immediately

            break;
         }



         //Update the length of the response
         n = strlen(response);
      }

    }
      if (error) return error;

      n=0;
       //Loop as long as necessary
      while(n < size)
      {
         //Wait for a response from the modem
         error = pppReceiveAtCommand(interface, response + n, size - n);

         //Check status code
         if(error)
         {
            //Exit immediately
            break;
         }

         if(strstr(response, str) ||
         strstr(response, "ERROR")){
        	
       

            TRACE_DEBUG("AT PLUS MESSAGE: %s\n", response);
            //Exit immediately

            break;
         }



         //Update the length of the response
         n = strlen(response);
      
      }

   //Return status code
   return error;
}

error_t modemSendAtCommandHTTP(NetInterface *interface,const char_t *command, char_t *response, size_t size)
{
   error_t error;
   size_t n;

   //Debug message
  
   TRACE_DEBUG("AT command:  %s\n",command);
  
   //Send AT command
   error = pppSendAtCommand(interface, command);
    vTaskDelay(5);
   //Check status code
   if(!error)
   {
      //Size of the response modem_buffer
      n = 0;

      //Loop as long as necessary
      while(n < size)
      {
         //Wait for a response from the modem
         error = pppReceiveAtCommand(interface, response + n, size - n);

         //Check status code
         if(error)
         {
            //Exit immediately
            break;
         }


         //Status string received?
         if(strstr(response, "OK") ||
            strstr(response, "CONNECT") ||
            strstr(response, "RING") ||
            strstr(response, "+HTTPREAD:") ||
            strstr(response, "NO CARRIER") ||
            strstr(response, "~!NO CARRIER") ||
            strstr(response, "ERROR") ||
            strstr(response, "DOWNLOAD") ||
            strstr(response, "NO ANSWER") ||
	    strstr(response, "NORMAL POWER DOWN")
			)
         {

            TRACE_DEBUG("AT response: %s\n", response);
            //Exit immediately

            break;
         }



         //Update the length of the response
         n = strlen(response);
      }


    
    
      char *token=strstr(response+n+3+1, "\r\n");
    
      
      TRACE_DEBUG("AT HTTP MEX1:\n\r %s\n", token);
      //token = strstr(token, "\r\n");
      //TRACE_DEBUG("AT HTTP MEX2:\n\r %s\n", token);

   }

   //Return status code
   return error;
}

void modemDeinit(){
	error_t error;


#ifdef SIM7000E
//	TRACE_DEBUG("Power-off modem: PWRKEY 1500ms...\r\n");
//	HAL_GPIO_WritePin(POWERKEY_GPIO_Port,POWERKEY_Pin,GPIO_PIN_SET);
//    osDelayTask(1500);

//   TRACE_INFO("Power Key 1500 ms to shut down if ON or power ON if OFF...\r\n");
//   HAL_GPIO_WritePin(POWERKEY_GPIO_Port,POWERKEY_Pin,GPIO_PIN_SET);
//   osDelayTask(1500);
//   HAL_GPIO_WritePin(POWERKEY_GPIO_Port,POWERKEY_Pin,GPIO_PIN_RESET);

//	TRACE_DEBUG("Power-off modem: Waiting 10s for modem network logout...\r\n");
//	osDelayTask(10000);
#else
    TRACE_DEBUG("Power-off modem: PWRKEY 800ms...\r\n");
    HAL_GPIO_WritePin(POWERKEY_GPIO_Port,POWERKEY_Pin,GPIO_PIN_SET);
    osDelayTask(800);
    HAL_GPIO_WritePin(POWERKEY_GPIO_Port,POWERKEY_Pin,GPIO_PIN_RESET);
    TRACE_DEBUG("Power-off modem: Waiting for modem network logout...\r\n");
    osDelayTask(5000);
#endif

//////// poweroff not required id PWRKEY USED
//   HAL_GPIO_WritePin(MODEM_ON_GPIO_Port,MODEM_ON_Pin,GPIO_PIN_RESET);
//   TRACE_DEBUG("Power-off modem: power shut down\r\n");
//   osDelayTask(2000);



}


PppSettings pppSettings;
PppContext pppContext;
error_t modem_initEnvironment(void){

  error_t error;
  error = netInit();

  if(error)
   {
	  //Debug message
	  TRACE_DEBUG("Failed to initialize TCP/IP stack! (E:%u)\r\n",error);
   }

   //Configure the first network interface
   interface = &netInterface[0];

   //Get default PPP settings
   pppGetDefaultSettings(&pppSettings);
   //Select the underlying interface
   pppSettings.interface = interface;
   //Default async control character map
   pppSettings.accm = 0x00000000;
   //Allowed authentication protocols
   pppSettings.authProtocol = PPP_AUTH_PROTOCOL_PAP | PPP_AUTH_PROTOCOL_CHAP_MD5;

   //Initialize PPP
   error = pppInit(&pppContext, &pppSettings);
   //Any error to report?
   if(error)
   {
	  //Debug message
	  TRACE_DEBUG("Failed to initialize PPP!(E:%u)\r\n",error);
   }
  interface = &netInterface[0];
  error =netSetInterfaceName(interface, "test0");
  error =netSetUartDriver(interface, &uartDriver);
  //Initialize network interface
   error = netConfigInterface(interface);
   //Any error to report?
   if(error)
   {
	  //Debug message
	  TRACE_DEBUG("Failed to configure interface (E:%u)\r\n",error);
   }

   //Set timeout for blocking operations
   pppSetTimeout(interface, APP_PPP_TIMEOUT);
   return error;
}
