/*
 * UartModem.c
 *
 * Created: 7/1/2014 4:09:05 PM
 *  Author: User
 */ 


#include <string.h>
#include <stdio.h>
#include <math.h>
#include <asf.h>
#include "Utilities.h"
#include "conf_board.h"
#include "UartModem.h"

extern char modemBuffer[100];
extern uint8_t modemBufferIndex;

const char plusuri[3]             = "+++";
const char check_1[3]             = "AT\r";

const char set_text_mode[10]      = "AT+CMGF=1\r";
const char ask_cgmi_string[8]     = "AT+CGMI\r";
const char ask_cgmm_string[8]     = "AT+CGMM\r";

//const char enable_auto_reg_status[10]  = "AT+CREG=2\r";
const char disable_auto_reg_status[10] = "AT+CREG=0\r";
const char get_reg_status[8]      = "AT+CREG?\r";	 


const char set_err_report[10]      = "AT+CMEE=2\r";
const char start_tcpip_stack[12]   = "AT+WIPCFG=1\r";
const char open_gprs_bearer[13]    = "AT+WIPBR=1,6\r";
	 	
const char set_bearer_apn[30]      = "AT+WIPBR=2,6,11,\"internet\"\r";
const char set_bearer_user[25]     = "AT+WIPBR=2,6,0,\"tzuser\"\r";
const char set_bearer_pass[25]     = "AT+WIPBR=2,6,1,\"sierra\"\r";
	 	
const char set_dest_bearer_ip[40]  = "AT+WIPBR=2,5,16,\"109.168.1.116\"\r";
	 	
const char get_bearer_apn[16]      = "AT+WIPBR=3,6,11\r";
const char get_local_bearer_ip[16] = "AT+WIPBR=3,5,15\r";
const char get_dest_bearer_ip[16]  = "AT+WIPBR=3,5,16\r";
	 	
const char start_gprs_bearer[15]   = "AT+WIPBR=4,6,0\r";
	 	
const char create_tcp_client[42]   = "AT+WIPCREATE=2,1,\"109.168.1.116\",80\r\r";

const char modem_registered_response[9] = "+CREG: 1,";

const char deactivate_ppstk[12] = "AT*PSSTKI=0\r";

const char ask_for_csmp[9] = "AT+CSMP?\r";

const char set_provider_format[12] = "AT+COPS=1,0\r";

const char sms_number[24] = "AT+CMGS=\"+972544783937\"\r";


const char get_dev_version[8]         = "AT+CGMR\r";
const char check_if_pin_needed[9]     = "AT+CPIN?\r";
const char ask_cimi_string[8]         = "AT+CIMI\r";
const char get_signal_strength[8]     = "AT+CSQ\r";
const char modem_reg_resp_header[8]   = "AT+CREG?";
const char enable_auto_reg_status[10] = "AT+CREG=1\r";
const char get_network_provider[9]    = "AT+COPS?\r";
const char get_serving_netinfo[12]    = "AT*PSENGI=0\r";
const char get_neighbor_netinfo[12]   = "AT*PSENGI=1\r";
const char set_err_report_1[10]       = "AT+CMEE=1\r";

//At+cgmr
//At+cpin?
//At+cimi
//At+csq
//At+creg?
//At+creg=1
//At+cops?
//At*psengi=0
//At*psengi=1
//At+cmee=1






//**********************
void cleanModemBuffer()
{
	//for (int v=0; v < sizeof(modemBuffer); v++)
	//   modemBuffer[v] = 0xFF;
	   
    modemBufferIndex = 0;	   
}



//************************
uint8_t modem_activation()
{
	uint8_t retval = 99;	
	
	delay_ms(500);
	
	bool modsta = gpio_pin_is_low(MODEM_STATUS_PIN);
	if (modsta == true)
	{
 	 // Modem OFF - we have to wake it up
		
		MODEM_ON_OFF_PIN_TO_LOW;
	
	    gsm_modem_wake_up_count = 15;
	    while (gsm_modem_wake_up_count > 0)
	    {
		   if  (GpioPoll(MODEM_STATUS_PIN))
		   {
			  if (gpio_pin_is_low(MODEM_STATUS_PIN))       // changed from high
			  {
				 delay_s(1);
			  }
			  else
			  {			  
		      // MODEM_ON_OFF_PIN_TO_HIGH;
				 delay_s(1);
				 retval = 0;
				 break;
			  }
		   }
	    }		
	}
	else
	{
  	   // Modem already ON
		  retval = 0;
	}
	
	return(retval);
}


//**************************
uint8_t hardware_power_off()
{
	uint8_t retval = 99;
	
	bool modsta = gpio_pin_is_low(MODEM_STATUS_PIN);
	if (modsta == false)
	{
 	 // Modem ON - we want to put WISMO_READY to Low
		
		MODEM_ON_OFF_PIN_TO_HIGH;
		delay_s(1);		
		
		gsm_modem_wake_up_count = 15;                        // it's not wake up but power off - just use the same variable ! to be changed...
		while (gsm_modem_wake_up_count > 0)
		{
			if  (GpioPoll(MODEM_STATUS_PIN))
			{
				if (gpio_pin_is_low(MODEM_STATUS_PIN))       // OK - WISMO_READY changed to low
				{
					retval = 0;
					break;
				}
				else
				{
					delay_s(1);
				}
			}
		}
	}
	else
	{
	 // Modem already OFF
		retval = 0;
	}
	
	return(retval);
}



//**************************
uint8_t software_power_off()
{
	uint8_t retval = 99;
    char ask_cpof_string[8];	
	
	strncpy(ask_cpof_string, "AT+CPOF\r", sizeof(ask_cpof_string));
	
	bool modsta = gpio_pin_is_low(MODEM_STATUS_PIN);
	if (modsta == false)
	{
 	 // Modem ON - we want to put WISMO_READY to Low
		
		MODEM_ON_OFF_PIN_TO_LOW;
		delay_ms(5500);		
		
		for (int j = 0; j < sizeof(ask_cpof_string); j++)
		   usart_putchar(USART_SERIAL_MODEM, ask_cpof_string[j]);
		
		gsm_modem_wake_up_count = 15;                        // it's not wake up but power off - just use the same variable ! to be changed...
		while (gsm_modem_wake_up_count > 0)
		{
			if  (GpioPoll(MODEM_STATUS_PIN))
			{
				if (gpio_pin_is_low(MODEM_STATUS_PIN))       // OK - WISMO_READY changed to low
				{
					retval = 0;
					break;
				}
				else
				{
					delay_s(1);
				}
			}
		}
	}
	else
	{
	 // Modem already OFF
		retval = 0;
	}
	
	return(retval);
}



//*********************
bool modem_registered()
{
	 //if ((modemBuffer[2] == "+") && (modemBuffer[7] == "?"))
	 //{
		 //loop_was_broken = false;
		 //for (int k=0; k < sizeof(modem_reg_resp_header); k++)
		 //{
			 //if (modemBuffer[k] != modem_reg_resp_header[k])
			 //{
				 //loop_was_broken = true;
				 //break;
			 //}
		 //}
		 //if  (!loop_was_broken)
		 //{
			 //if ((modemBuffer[18] == "1") && (modemBuffer[11] == "+"))
			 //
			 //connected_modem = true;
			 //break;
		 //}
		 //
		 //if (connected_modem)
		 //break;
	 //}
}


//********************
void test_connection()
{	
	bool connected_modem = false;
	bool loop_was_broken = false;
	
	//while (true)
	//{
		 //cleanModemBuffer();
		 //for (int j = 0; j < sizeof(get_signal_strength); j++)
		    //usart_putchar(USART_SERIAL_MODEM, get_signal_strength[j]);
			 //
	     //delay_ms(1000);
	
	
		 //cleanModemBuffer();
		 //for (int j = 0; j < sizeof(get_reg_status); j++)
		     //usart_putchar(USART_SERIAL_MODEM, get_reg_status[j]);
			 //
		 //
	
	   //  if (modem_registered)
		//   break;

		 //delay_ms(1000);
	//}
	
	
	
	while (true)
	{
	
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(get_signal_strength); j++)
   	   usart_putchar(USART_SERIAL_MODEM, get_signal_strength[j]);
		  
    delay_ms(500); 
		  	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(start_tcpip_stack); j++)
	   usart_putchar(USART_SERIAL_MODEM, start_tcpip_stack[j]);
	   
	delay_ms(500);
	
    modemBufferIndex = 0;
    for (int j = 0; j < sizeof(open_gprs_bearer); j++)
       usart_putchar(USART_SERIAL_MODEM, open_gprs_bearer[j]);
	   
    delay_ms(500);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(get_signal_strength); j++)
	   usart_putchar(USART_SERIAL_MODEM, get_signal_strength[j]);
	
	delay_ms(500);
	   
  //-------------------------------------------------------------	   
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(set_bearer_apn); j++)
	   usart_putchar(USART_SERIAL_MODEM, set_bearer_apn[j]);
	   
	delay_ms(500);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(set_bearer_user); j++)
	   usart_putchar(USART_SERIAL_MODEM, set_bearer_user[j]);
	
	delay_ms(500);	
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(get_signal_strength); j++)
	   usart_putchar(USART_SERIAL_MODEM, get_signal_strength[j]);
	
	delay_ms(500);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(set_bearer_pass); j++)
	   usart_putchar(USART_SERIAL_MODEM, set_bearer_pass[j]);
  	
	delay_ms(500);
	   
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(set_dest_bearer_ip); j++)
	   usart_putchar(USART_SERIAL_MODEM, set_dest_bearer_ip[j]);
  //--------------------------------------------------------------   	   
	   
    delay_ms(500);

    modemBufferIndex = 0;
    for (int j = 0; j < sizeof(get_bearer_apn); j++)
       usart_putchar(USART_SERIAL_MODEM, get_bearer_apn[j]);	   
	   
	delay_ms(500);

	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(get_local_bearer_ip); j++)
	   usart_putchar(USART_SERIAL_MODEM, get_local_bearer_ip[j]);
	   
	delay_ms(500);

	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(get_dest_bearer_ip); j++)
	   usart_putchar(USART_SERIAL_MODEM, get_dest_bearer_ip[j]);
  //--------------------------------------------------------------	  
  
    delay_ms(500); 
  
    modemBufferIndex = 0;
    for (int j = 0; j < sizeof(get_signal_strength); j++)
       usart_putchar(USART_SERIAL_MODEM, get_signal_strength[j]);
  
	delay_ms(500);

	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(start_gprs_bearer); j++)
	   usart_putchar(USART_SERIAL_MODEM, start_gprs_bearer[j]);
	   
    delay_ms(500);

	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(create_tcp_client); j++)
	   usart_putchar(USART_SERIAL_MODEM, create_tcp_client[j]);
	   
	delay_ms(500);
	   
	//modemBufferIndex = 0;
	//for (int j = 0; j < sizeof(sms_buffer); j++)
	   //usart_putchar(USART_SERIAL_MODEM, sms_buffer[j]);
	//
	delay_ms(500);   
	   
	}
}


//********************
void modem_init_test()
{	
	//delay_s(1);
	//for (int j = 0; j < 3; j++)
	   //usart_putchar(USART_SERIAL_MODEM, plusuri[j]);
	//delay_s(1);	 
	 
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(get_signal_strength); j++)
	   usart_putchar(USART_SERIAL_MODEM, get_signal_strength[j]);
	
	delay_ms(1000);	 	 
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(deactivate_ppstk); j++)
	   usart_putchar(USART_SERIAL_MODEM, deactivate_ppstk[j]);
	   
	delay_ms(1000);

	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(ask_cgmi_string); j++)
	   usart_putchar(USART_SERIAL_MODEM, ask_cgmi_string[j]);
	
	delay_ms(1000);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(ask_cgmm_string); j++)
	   usart_putchar(USART_SERIAL_MODEM, ask_cgmm_string[j]);
	
	delay_ms(1000);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(ask_cimi_string); j++)
   	   usart_putchar(USART_SERIAL_MODEM, ask_cimi_string[j]);
	
	delay_ms(1000);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(check_if_pin_needed); j++)
	   usart_putchar(USART_SERIAL_MODEM, check_if_pin_needed[j]);
	
	delay_ms(1000);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(set_err_report); j++)
	   usart_putchar(USART_SERIAL_MODEM, set_err_report[j]);
	
	delay_ms(1000);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(disable_auto_reg_status); j++)
	   usart_putchar(USART_SERIAL_MODEM, disable_auto_reg_status[j]);
	
	delay_ms(1000);
	
	modemBufferIndex = 0; 
	for (int j = 0; j < sizeof(get_reg_status); j++)
	   usart_putchar(USART_SERIAL_MODEM, get_reg_status[j]);
	   
   	delay_ms(1000);
	   
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(set_text_mode); j++)
	   usart_putchar(USART_SERIAL_MODEM, set_text_mode[j]);
	   
	delay_ms(1000);
	   
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(ask_for_csmp); j++)
	   usart_putchar(USART_SERIAL_MODEM, ask_for_csmp[j]);
	   
	delay_ms(1000);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(set_provider_format); j++)
	   usart_putchar(USART_SERIAL_MODEM, set_provider_format[j]);
		
    delay_ms(1000);
	 
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(get_network_provider); j++)
	   usart_putchar(USART_SERIAL_MODEM, get_network_provider[j]);
	
	delay_ms(1000);
}



//********************
void modem_rssi_test()
{
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(enable_auto_reg_status); j++)
   	   usart_putchar(USART_SERIAL_MODEM, enable_auto_reg_status[j]);	
	
	for (int i = 0; i < 7; i++)
	{
		modemBufferIndex = 0;
		for (int j = 0; j < sizeof(get_signal_strength); j++)
		   usart_putchar(USART_SERIAL_MODEM, get_signal_strength[j]);
		
		delay_ms(2000);
	}
}



//***********************
void modem_matthew_test()
{
	//At+cgmr
	//At+cpin?
	//At+cimi
	//At+csq
	//At+creg?
	//At+creg=1
	//At+cops?
	//At*psengi=0
	//At*psengi=1
	//At+cmee=1
	//
	//Then send an SMS? That will give me a better idea about what is going on.

	
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(get_dev_version); j++)
	   usart_putchar(USART_SERIAL_MODEM, get_dev_version[j]);
	
	delay_ms(2000);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(check_if_pin_needed); j++)
	   usart_putchar(USART_SERIAL_MODEM, check_if_pin_needed[j]);
	   
	delay_ms(2000);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(ask_cimi_string); j++)
	   usart_putchar(USART_SERIAL_MODEM, ask_cimi_string[j]);
	
	delay_ms(2000);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(get_signal_strength); j++)
	   usart_putchar(USART_SERIAL_MODEM, get_signal_strength[j]);
	
	delay_ms(2000);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(modem_reg_resp_header); j++)
	   usart_putchar(USART_SERIAL_MODEM, modem_reg_resp_header[j]);
	
	delay_ms(2000);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(enable_auto_reg_status); j++)
	   usart_putchar(USART_SERIAL_MODEM, enable_auto_reg_status[j]);
	
	delay_ms(2000);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(get_network_provider); j++)
	   usart_putchar(USART_SERIAL_MODEM, get_network_provider[j]);
	
	delay_ms(2000);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(get_serving_netinfo); j++)
	   usart_putchar(USART_SERIAL_MODEM, get_serving_netinfo[j]);
	
	delay_ms(2000);
	
	modemBufferIndex = 0;
	for (int j = 0; j < sizeof(get_neighbor_netinfo); j++)
	   usart_putchar(USART_SERIAL_MODEM, get_neighbor_netinfo[j]);
	
	delay_ms(2000);
	
	//modemBufferIndex = 0;
	//for (int j = 0; j < sizeof(set_err_report_1); j++)
	   //usart_putchar(USART_SERIAL_MODEM, set_err_report_1[j]);
	//
	//delay_ms(2000);
	
	//modemBufferIndex = 0;
	//for (int j = 0; j < sizeof(sms_number); j++)
	   //usart_putchar(USART_SERIAL_MODEM, sms_number[j]);
}



//********************
void gprs_first_test()
{	
	const char sms_buffer[33] = "AT+CMGS=\"+972544783937\"\rHELLO\x1A";	
	
	const char set_csmp[20]   = "AT+CSMP=17,167,0,16\r\n";
	
	const char sms_number[24] = "AT+CMGS=\"+972544783937\"\r";
	const char salutare[8]    = "HELLO";		
	
   //modem_rssi_test();
 
    modem_matthew_test();
	
	
	 
  //-------------------------------------------------------------------- 
	//delay_ms(3000);
//
	//modemBufferIndex = 0;
	//for (int j = 0; j < sizeof(set_csmp); j++)
	   //usart_putchar(USART_SERIAL_MODEM, set_csmp[j]);
	
	delay_ms(3000);

	//modemBufferIndex = 0;
	//for (int j = 0; j < sizeof(set_text_mode); j++)
	   //usart_putchar(USART_SERIAL_MODEM, set_text_mode[j]);
	
	//delay_ms(3000);
	
	//modemBufferIndex = 0;
	//for (int j = 0; j < sizeof(sms_number); j++)
	   //usart_putchar(USART_SERIAL_MODEM, sms_number[j]);
	   
	//modemBufferIndex = 0;
	//for (int j = 0; j < sizeof(sms_buffer); j++)
	   //usart_putchar(USART_SERIAL_MODEM, sms_buffer[j]);
  //--------------------------------------------------------------------	   
	   
	   
    //modemBufferIndex = 0;
    //for (int j = 0; j < sizeof(sms_buffer); j++)
       //usart_putchar(USART_SERIAL_MODEM, sms_buffer[j]);   	   
	
	//modemBufferIndex = 0;
	//for (int j = 0; j < sizeof(salutare); j++)
	   //usart_putchar(USART_SERIAL_MODEM, salutare[j]);	
	
	//usart_putchar(USART_SERIAL_MODEM, 0x1A);
	 

 //-----------------------------------------------------------	   
    //while(true)
	//{		

     	//modemBufferIndex = 0;
	    //for (int j = 0; j < sizeof(sms_buffer); j++)
	       //usart_putchar(USART_SERIAL_MODEM, sms_buffer[j]);
	//
	    //delay_ms(500);	
		
		//modemBufferIndex = 0;
		//for (int j = 0; j < sizeof(sms_buffer); j++)
	   	   //usart_putchar(USART_SERIAL_MODEM, sms_buffer[j]);
		
		//modemBufferIndex = 0;
		//for (int j = 0; j < sizeof(salutare); j++)
	   	   //usart_putchar(USART_SERIAL_MODEM, salutare[j]);
			
		//delay_ms(500);
	//}
  //----------------------------------------------------------	 
  
  
  //test_connection();
}




	
		 //strncpy(plusuri,             "+++",         sizeof(plusuri));
		 // strncpy(check_1,             "AT\r",        sizeof(check_1));
	
	 //strncpy(check_cgmi_string,   "AT+CGMI=?\r", sizeof(check_cgmi_string));
	 //strncpy(ask_cgmi_string,     "AT+CGMI\r",   sizeof(ask_cgmi_string));		
	
	
	
	//uint8_t frec = usart_getchar(USART_SERIAL_MODEM);
	//if (frec != 0)
	//{
	//delay_s(1);
	//}
	//}
//}





////---------------------------------------------------------------------------------------------------
//char plusuri[3];
//sprintf(plusuri, "+++", 3);
//
//char plusuri_l[5];
//sprintf(plusuri_l, "+++\r\n", 5);
//
//char plusuri_s[4];
//sprintf(plusuri_s, "+++\r", 4);
//
//char plusuri_z[4];
//sprintf(plusuri_z, "+++\x1A", 4);
//
//uint8_t hexa_plusuri_z[4] = {0x2B, 0x2B, 0x2B, 0x1A};                                                      //+++\z
////----------------------------------------------------------------------------------------------------
//
//
////----------------------------------------------------------------------------------------------------
//char ascii_set_text_mode_l[11];
//sprintf(ascii_set_text_mode_l, "AT+CMGF=1\r\n", 11);
//
//char ascii_set_text_mode_s[10];
//sprintf(ascii_set_text_mode_s, "AT+CMGF=1\r", 10);
//
//char ascii_set_text_mode_z[10];
//sprintf(ascii_set_text_mode_z, "AT+CMGF=1\x1A", 10);
//
//uint8_t hexa_set_text_mode_z[10] = {0x41, 0x54, 0x2B, 0x43, 0x4D, 0x47, 0x46, 0x3D, 0x31, 0x1A};          //AT+CMGF=1\z
////----------------------------------------------------------------------------------------------------
//
//
////----------------------------------------------------------------------------------------------------
//char ascii_check_cgmi_string_l[11];
//sprintf(ascii_check_cgmi_string_l, "AT+CGMI=?\r\n", 11);
//
//char ascii_check_cgmi_string_s[10];
//sprintf(ascii_check_cgmi_string_s, "AT+CGMI=?\r", 10);
//
//char ascii_check_cgmi_string_z[10];
//sprintf(ascii_check_cgmi_string_z, "AT+CGMI=?\x1A", 10);
//
//uint8_t hexa_check_cgmi_string_z[10] = {0x41, 0x54, 0x2B, 0x43, 0x47, 0x4D, 0x49, 0x3D, 0x3F, 0x1A};     //AT+CGMI=?\z
////-----------------------------------------------------------------------------------------------------
//
//
////-----------------------------------------------------------------------------------------------------
//char ascii_ask_cgmi_string_l[11];
//sprintf(ascii_ask_cgmi_string_l, "AT+CGMI\r\n", 11);
//
//char ascii_ask_cgmi_string_s[10];
//sprintf(ascii_ask_cgmi_string_s, "AT+CGMI\r", 10);
//
//char ascii_ask_cgmi_string_z[10];
//sprintf(ascii_ask_cgmi_string_z, "AT+CGMI\x1A", 10);
//
//uint8_t hexa_ask_cgmi_string_z[8] = {0x41, 0x54, 0x2B, 0x43, 0x47, 0x4D, 0x49, 0x1A};                 //AT+CGMI\z
////-----------------------------------------------------------------------------------------------------
//
//
////----------------------------------------------------------------------------------------------------
//char ascii_check_cimi_string_l[11];
//sprintf(ascii_check_cimi_string_l, "AT+CIMI=?\r\n", 11);
//
//char ascii_check_cimi_string_s[10];
//sprintf(ascii_check_cimi_string_s, "AT+CIMI=?\r", 10);
//
//char ascii_check_cimi_string_z[8];
//sprintf(ascii_check_cimi_string_z, "AT+CIMI=?\x1A", 8);
//
//uint8_t hexa_check_cimi_string_z[10] = {0x41, 0x54, 0x2B, 0x43, 0x49, 0x4D, 0x49, 0x3D, 0x3F, 0x1A};     //AT+CIMI=?\z
////-----------------------------------------------------------------------------------------------------
//
//
//char ascii_ask_cimi_string_z[8];
//sprintf(ascii_ask_cimi_string_z, "AT+CIMI\x1A", 8);
//
//uint8_t hexa_ask_cimi_string_z[8]    = {0x41, 0x54, 0x2B, 0x43, 0x49, 0x4D, 0x49, 0x1A};                 //AT+CIMI\z
////-----------------------------------------------------------------------------------------------------
//
//
//char sms_buffer[33];
//strcpy(sms_buffer, "AT+CMGS =\"+972544783937\"\r\nhello\n\x1A");
//
//
//for (int j = 0; j < 3; j++)
//usart_putchar(USART_SERIAL_MODEM, plusuri[j]);
//
//
//for (int j = 0; j < 4; j++)
//usart_putchar(USART_SERIAL_MODEM, hexa_plusuri_z[j]);
//
//
//while(true)
//{
	////-----------------------------------------------------------------------------
	//for (int j = 0; j < 11; j++)
	//usart_putchar(USART_SERIAL_MODEM, ascii_set_text_mode_l[j]);
	//
	//delay_ms(100);
	//
	//for (int j = 0; j < 10; j++)
	//usart_putchar(USART_SERIAL_MODEM, ascii_set_text_mode_s[j]);
	//
	//delay_ms(100);
	//
	//for (int j = 0; j < 10; j++)
	//usart_putchar(USART_SERIAL_MODEM, hexa_set_text_mode_z[j]);
	//
	//delay_ms(100);
	////--------------------------------------------------------------------------------
	//
	//
	////--------------------------------------------------------------------------------
	//for (int j = 0; j < 11; j++)
	//usart_putchar(USART_SERIAL_MODEM, ascii_check_cgmi_string_l[j]);
	//
	//delay_ms(100);
	//
	//for (int j = 0; j < 10; j++)
	//usart_putchar(USART_SERIAL_MODEM, ascii_check_cgmi_string_s[j]);
	//
	//delay_ms(100);
	//
	//for (int j = 0; j < 10; j++)
	//usart_putchar(USART_SERIAL_MODEM, hexa_check_cgmi_string_z[j]);
	//
	//delay_ms(100);
	//
	//for (int j = 0; j < 8; j++)
	//{
		//usart_putchar(USART_SERIAL_MODEM, hexa_ask_cgmi_string_z[j]);
	//}
	//
	//delay_ms(100);
	////--------------------------------------------------------------------------------
//
//
	////--------------------------------------------------------------------------------
	//for (int j = 0; j < 11; j++)
	//{
		//usart_putchar(USART_SERIAL_MODEM, ascii_check_cimi_string_l[j]);
	//}
//
	//delay_ms(100);
//
	//for (int j = 0; j < 10; j++)
	//{
		//usart_putchar(USART_SERIAL_MODEM, ascii_check_cimi_string_s[j]);
	//}
//
	//delay_ms(100);
//
	//for (int j = 0; j < 10; j++)
	//{
		//usart_putchar(USART_SERIAL_MODEM, hexa_check_cimi_string_z[j]);
	//}
	//
	//delay_ms(100);
	//
	//for (int j = 0; j < 8; j++)
	//{
		//usart_putchar(USART_SERIAL_MODEM, hexa_ask_cimi_string_z[j]);
	//}
//
	//delay_ms(100);
	////--------------------------------------------------------------------------------