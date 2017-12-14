/* Copyright 2016, Eric Pernia.
 * All rights reserved.
 *
 * This file is part sAPI library for microcontrollers.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * Date: 2016-07-03
 */

/*==================[inclusions]=============================================*/

//#include "rtc.h"   // <= own header (optional)
#include "sapi.h"    // <= sAPI header
#include "board.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/* Buffer */
static char uartBuff[10];

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/


/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.

 */
char* itoa(int value, char* result, int base) {
   // check that the base if valid
   if (base < 2 || base > 36) { *result = '\0'; return result; }

   char* ptr = result, *ptr1 = result, tmp_char;
   int tmp_value;

   do {
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
   } while ( value );

   // Apply negative sign
   if (tmp_value < 0) *ptr++ = '-';
   *ptr-- = '\0';
   while(ptr1 < ptr) {
      tmp_char = *ptr;
      *ptr--= *ptr1;
      *ptr1++ = tmp_char;
   }
   return result;
}


/* Enviar fecha y hora en formato "DD/MM/YYYY, HH:MM:SS" */
void showDateAndTime( rtc_t * rtc ){
   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->mday), (char*)uartBuff, 10 ); /* 10 significa decimal */
   /* Envio el dia */
   if( (rtc->mday)<10 )
      uartWriteByte( UART_USB, '0' );
   uartWriteString( UART_USB, uartBuff );
   uartWriteByte( UART_USB, '/' );

   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->month), (char*)uartBuff, 10 ); /* 10 significa decimal */
   /* Envio el mes */
   if( (rtc->month)<10 )
      uartWriteByte( UART_USB, '0' );
   uartWriteString( UART_USB, uartBuff );
   uartWriteByte( UART_USB, '/' );

   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->year), (char*)uartBuff, 10 ); /* 10 significa decimal */
   /* Envio el año */
   if( (rtc->year)<10 )
      uartWriteByte( UART_USB, '0' );
   uartWriteString( UART_USB, uartBuff );


   uartWriteString( UART_USB, ", ");


   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->hour), (char*)uartBuff, 10 ); /* 10 significa decimal */
   /* Envio la hora */
   if( (rtc->hour)<10 )
      uartWriteByte( UART_USB, '0' );
   uartWriteString( UART_USB, uartBuff );
   uartWriteByte( UART_USB, ':' );

   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->min), (char*)uartBuff, 10 ); /* 10 significa decimal */
   /* Envio los minutos */
  // uartBuff[2] = 0;    /* NULL */
   if( (rtc->min)<10 )
      uartWriteByte( UART_USB, '0' );
   uartWriteString( UART_USB, uartBuff );
   uartWriteByte( UART_USB, ':' );

   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->sec), (char*)uartBuff, 10 ); /* 10 significa decimal */
   /* Envio los segundos */
   if( (rtc->sec)<10 )
      uartWriteByte( UART_USB, '0' );
   uartWriteString( UART_USB, uartBuff );


   /* Envio un 'enter' */
   uartWriteString( UART_USB, "\r\n");
}


/* FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE RESET. */
int main(void){

   /* ------------- INICIALIZACIONES ------------- */

   /* Inicializar la placa */
   boardConfig();

   /* Inicializar UART_USB a 115200 baudios */
   uartConfig( UART_USB, 115200 );
 // Configurar la hora del RTC
    //bool_t val;
    bool_t val = 0;
    
    rtc_t rtc;
    rtc.sec=0;
    rtc.min=20;
    rtc.hour=11;
    rtc.mday=7;
    rtc.wday=5;
    rtc.month=12;
    rtc.year=17;  
    //val = 
    rtcConfig( &rtc );
    delay_t delay1s;
    delayConfig( &delay1s, 1000 );
    delay(2000); // El RTC tarda en setear la hora, por eso el delay
   // val = 
    rtcWrite( &rtc ); //Setea la hora

    //Configurar la hora de alarma
    int min=21;
    int hour=11;
    //rtcWrite( &rtcAlarm );
   //unsigned short int state = 1;
   /* ------------- REPETIR POR SIEMPRE ------------- */
   
   while(1) {
     
   gpioWrite( LED1, ON);
      if( delayRead( &delay1s ) ){
        /* Leer fecha y hora */
        val = rtcRead( &rtc );
        if(rtc.hour== hour && rtc.min== min){
          Board_LED_Toggle(LEDB);
        }
        /* Mostrar fecha y hora en formato "DD/MM/YYYY, HH:MM:SS" */
        showDateAndTime( &rtc );
      }
         
        /*  switch (state){        //GPIO8 = enable, GPIO1 = negro, GPIO3 = marron
                                //GPIO 5 = amarillo, GPIO 7 = naranja
            case 1:
               // gpioWrite( GPIO8 , ON );
                gpioWrite( GPIO1 , OFF );
                gpioWrite( LED1, ON);
                gpioWrite( GPIO5 , OFF );
                gpioWrite( LED3, ON);
                delay(250);
                state = 2;
            break;

            case 2:
              //  gpioWrite( GPIO8 , ON );
                gpioWrite( GPIO1 , OFF );
                gpioWrite(LED1, ON);
                gpioWrite( GPIO7 , OFF);
                gpioWrite(LEDB, ON);                
                delay(250);
                state = 3;
            break;

            case 3:
            //    gpioWrite( GPIO8 , ON );
                gpioWrite( GPIO3 , OFF );
                gpioWrite(LED2, ON);
                gpioWrite( GPIO7, OFF );
                gpioWrite(LEDB, ON);                
                delay(250);
                state = 4;
            break;

            case 4:
             //   gpioWrite( GPIO8 , ON);
                gpioWrite( GPIO3 , OFF);
                gpioWrite(LED2, ON);                
                gpioWrite( GPIO5 , OFF);
                gpioWrite(LED3 , ON);                
                delay(250);
                state = 1;
      } */
       // gpioWrite( GPIO0 , ON );
        // delay(500);
         
         /* Mostrar fecha y hora en formato "DD/MM/YYYY, HH:MM:SS" */
        // showDateAndTime( &rtc );
      

   }

   /* NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa no es llamado
      por ningun S.O. */
   return 0 ;
}

/*==================[end of file]============================================*/
