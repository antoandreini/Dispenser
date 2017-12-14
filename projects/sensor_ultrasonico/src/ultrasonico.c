#include "sapi.h"
#include "board.h"
#include "stopwatch.h"

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
 static char uartBuff[10];

int main(void){
    boardConfig();
    StopWatch_Init(); 
    uartConfig( UART_USB, 115200 );
    int count,tiempo, aux, i;   
    while(1) {
        uartWriteString( UART_USB, "Dio la vuelta\r\n" );
        i = StopWatch_Start();
        gpioWrite( GPIO5 , ON);     //Señal trigger
        StopWatch_DelayUs(20);
        gpioWrite( GPIO5 , OFF);
        while(!gpioRead(GPIO2)){} //Espera que llegue la señal
        aux = StopWatch_Elapsed(i);
        while(gpioRead(GPIO2) && StopWatch_TicksToUs(StopWatch_Elapsed(i) - aux) < 200000){} //Espera a que baje la señal
        count = StopWatch_Elapsed(i) - aux;
        tiempo = StopWatch_TicksToUs(count) - 20;
        uartWriteString( UART_USB, "Termino de llegar el echo\r\n" ); //Termina el echo, hay que ver el tiempo transcurrido
        itoa(tiempo, uartBuff, 10 );
        uartWriteString( UART_USB, uartBuff );
        uartWriteString( UART_USB, "\r\n" );
        delay(1000);
    }
    return 0;
}