

/*==================[inclusions]=============================================*/

//#include "rtc.h"   // <= own header (optional)
#include "sapi.h"    // <= sAPI header
#include "stopwatch.h"
#include <string.h>  

#define LED 0
#define GRANDE  30
#define PEQUENIA 15
#define NIVEL_MINIMO  500
#define NIVEL_MAXIMO  650
#define ESP01_RX_BUFF_SIZE  1024
#define VALOR_MAXIMO_ULTRASONICO  1300
#define VALOR_RECIPIENTE_COMIDA_LLENO 1000


static char uartBuff[10];
char espResponseBuffer[ ESP01_RX_BUFF_SIZE ];   //Buffer de respuesta del modulo wifi
uint32_t espResponseBufferSize = ESP01_RX_BUFF_SIZE;
bool_t retVal;

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

bool_t recipiente_lleno(void){
      StopWatch_Init(); 
      int total = 0;
      int n = 0;
      int i = 0;
      int count,tiempo, aux,  x, promedio;   
      while (n< 10 && i<50){
        x = StopWatch_Start();
          gpioWrite( GPIO8 , ON);     //Señal trigger
          StopWatch_DelayUs(20);
          gpioWrite( GPIO8 , OFF);
          while(!gpioRead(GPIO6)){} //Espera que llegue la señal
          aux = StopWatch_Elapsed(x);
          while(gpioRead(GPIO6) && StopWatch_TicksToUs(StopWatch_Elapsed(x) - aux) < 200000){} //Espera a que baje la señal
          count = StopWatch_Elapsed(x) - aux;
          tiempo = StopWatch_TicksToUs(count) - 20;
          if (tiempo < VALOR_MAXIMO_ULTRASONICO) {
            total = total + tiempo;
            n++;
          }
          i++;
      }
      if(n == 10) {
        promedio = total / n;
        itoa(promedio, uartBuff, 10 );
        uartWriteString( UART_USB, uartBuff );
        uartWriteString( UART_USB, "\r\n" );
        bool_t res = promedio < VALOR_RECIPIENTE_COMIDA_LLENO;
        return (res == 1);
      }
      else return -1; 
}

void esp01CleanRxBuffer( void ){
   espResponseBufferSize = ESP01_RX_BUFF_SIZE;
   memset( espResponseBuffer, 0, espResponseBufferSize );
}

bool_t esp01ConnectToServer( char* url, char* port ){

   bool_t retVal = FALSE;

   // AT+CIPSTART="TCP","url",port ---------------------------

   // Limpiar Buffer (es necesario antes de usar
   // "receiveBytesUntilReceiveStringOrTimeoutBlocking")
   esp01CleanRxBuffer();

   uartWriteString( UART_USB, ">>>> Conectando al servidor \"" );
   uartWriteString( UART_USB, url );
   uartWriteString( UART_USB, "\", puerto \"" );
   uartWriteString( UART_USB, port );
   uartWriteString( UART_USB,"\"..." );

   uartWriteString( UART_USB, ">>>> AT+CIPSTART=\"TCP\",\"" );
   uartWriteString( UART_USB, url );
   uartWriteString( UART_USB,"\"," );
   uartWriteString( UART_USB,port );
   uartWriteString( UART_USB, "\r\n" );

   uartWriteString( UART_232, "AT+CIPSTART=\"TCP\",\"" );
   uartWriteString( UART_232, url );
   uartWriteString( UART_232, "\"," );
   uartWriteString( UART_232, port );
   uartWriteString( UART_232, "\r\n" );

   // No poner funciones entre el envio de comando y la espera de respuesta
   retVal = receiveBytesUntilReceiveStringOrTimeoutBlocking(
               UART_232,
               "CONNECT\r\n\r\nOK\r\n", 15,
               espResponseBuffer, &espResponseBufferSize,
               300
            );
   if( !retVal ){
      uartWriteString( UART_USB, ">>>>    Error: No se puede conectar al servidor: \"" );
      uartWriteString( UART_USB, url );
      uartWriteString( UART_USB, "\"," );
      uartWriteString( UART_USB, port );
      uartWriteString( UART_USB, "\"!!\r\n" );
   }
   // Imprimo todo lo recibido
   uartWriteString( UART_USB, espResponseBuffer );
   return retVal;
}

bool_t esp01ConnectToWifiAP( char* wiFiSSID, char* wiFiPassword ){

   bool_t retVal = FALSE;
   char* index = 0;

   // AT+CWJAP="wiFiSSID","wiFiPassword" ---------------------

   // Limpiar Buffer (es necesario antes de usar
   // "receiveBytesUntilReceiveStringOrTimeoutBlocking")
   esp01CleanRxBuffer();

   // Conectar a la red Wi-Fi. se envia AT+CWJAP="wiFiSSID","wiFiPassword"
   uartWriteString( UART_USB, ">>>> Conectando a la red Wi-Fi: \"" );
   uartWriteString( UART_USB, wiFiSSID );
   uartWriteString( UART_USB, "\"..." );
   
   uartWriteString( UART_232, "AT+CWJAP=\"" );
   uartWriteString( UART_232, wiFiSSID );
   uartWriteString( UART_232, "\",\"" );
   uartWriteString( UART_232, wiFiPassword );
   uartWriteString( UART_232, "\"\r\n" );

   // No poner funciones entre el envio de comando y la espera de respuesta
   retVal = receiveBytesUntilReceiveStringOrTimeoutBlocking(
               UART_232,
               "WIFI CONNECTED\r\nWIFI GOT IP\r\n\r\nOK\r\n", 35,
               espResponseBuffer, &espResponseBufferSize,
               10000
            );
   if( retVal ){

      // Imprimo todo lo recibido filtrando la parte que muestra el password, llega:

      // AT+CWJAP="wiFiSSID","wiFiPassword"
      //
      // WIFI DISCONNECT ----> Opcional
      // WIFI CONNECTED
      // WIFI GOT IP
      //
      // OK

      // Pero imprimo:

      // WIFI CONNECTED
      // WIFI GOT IP
      //
      // OK

      index = strstr( (const char*)espResponseBuffer, (const char*)"WIFI CONNECTED" );
      if( index != 0 ){
         // Muestro desde " WIFI CONNECTED" en adelante
          uartWriteString( UART_USB, index );
      } else{
         // Muestro todo en caso de error
          uartWriteString( UART_USB, espResponseBuffer );
      }
   } else{
       uartWriteString( UART_USB, ">>>>    Error: No se puede conectar a la red: \"" );
       uartWriteString( UART_USB, wiFiSSID );
       uartWriteString( UART_USB, "\"!!\r\n" );

      // Muestro todo en caso de error
       uartWriteString( UART_USB, espResponseBuffer );
   }
   return retVal;
}

/* FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE RESET. */
int main(void){

   /* ------------- INICIALIZACIONES ------------- */

   /* Inicializar la placa */
   boardConfig();
   gpioConfig( ENET_RXD1  , GPIO_OUTPUT ); //Cable negro motor
   gpioConfig( ENET_TXEN  , GPIO_OUTPUT ); //Cable marron motor
   gpioConfig( ENET_MDC , GPIO_OUTPUT ); //Cable amarillo motor
   gpioConfig( GPIO3 , GPIO_OUTPUT ); //Cable naranja motor
   gpioConfig( GPIO0 , GPIO_OUTPUT ); //Enable motor
   gpioConfig( GPIO6 , GPIO_INPUT ); //Entrada ultrasonico
   gpioConfig( GPIO8 , GPIO_OUTPUT ); //salida ultrasonico
   gpioConfig( GPIO7 , GPIO_OUTPUT ); //Alimentacion bomba de agua
   gpioConfig( GPIO5 , GPIO_OUTPUT ); //Enable bomba de agua
   /* Inicializar UARTs a 115200 baudios */
   uartConfig( UART_USB, 115200 );
   uartConfig( UART_232, 115200 );
   /* Establecer modo de funcionamiento ESP */
   uartWriteString( UART_232, "AT+CWMODE=1\r\n" );
   /* Inicializar ADC para censar nivel de agua */
   adcConfig( ADC_ENABLE );
   uint16_t muestra = 0;
   char ch = '|';
   char *res;
   bool_t medida = 0;
   bool_t alimentando = 0;
   bool_t recipienteLleno = 0;
   bool_t bombaPrendida = 0;
   bool_t val = 0;
   int state = 1;
   /* Configurar la hora de alarma inicial */
   int minAlarma=15;
   int horaAlarma=12; 
   /* Configurar tamaño porcion inicial */
   int porcion = GRANDE;
   /* Estructura RTC */
   rtc_t rtc;
   rtc.sec=0;
   rtc.min=04;
   rtc.hour=12;
   rtc.mday=8;
   rtc.wday=5;
   rtc.month=2;
   rtc.year=18;  
   /* Inicializar RTC */
   val = rtcConfig( &rtc );
   delay_t delay1s;
   delayConfig( &delay1s, 1000 );
   delay(2000); // El RTC tarda en setear la hora, por eso el delay
   if(esp01ConnectToWifiAP( "Trabajo Final", "ilcapogutierrez")){   //Conectar a la red wifi y al servidor
    delay(1000);
    esp01ConnectToServer("192.168.0.105", "1337");
   }
   /* ------------- REPETIR POR SIEMPRE ------------- */
   while(1) {
        esp01CleanRxBuffer();
        retVal = receiveBytesUntilReceiveStringOrTimeoutBlocking(  //Espera hasta que el servidor envie un mensaje, o que pasen 100 milisegundos
                  UART_232,
                  "|||\r\n", 6,
                  espResponseBuffer, &espResponseBufferSize,
                  100
                );
        if (retVal){
          res = strchr(espResponseBuffer, ch); //Busca el comienzo del mensaje enviado por el servidor
          int hora_act = 0;
          int min_act = 0;
          int hora_a = 0;
          int min_a = 0;
          switch (res[1]){
            case 'A':    //Actualiza hora actual, hora de alarma y racion
              hora_act += (res[3] - '0') * 10;
              hora_act += (res[4] - '0');
              rtc.hour = hora_act;
              min_act += (res[6] - '0') * 10;
              min_act += (res[7] - '0');
              rtc.min = min_act;
              rtc.sec = 0;
              rtcWrite(&rtc);
              hora_a += (res[9] - '0') * 10;
              hora_a += (res[10] - '0');
              horaAlarma = hora_a;
              min_a += (res[12]- '0') * 10;
              min_a += (res[13] - '0');
              minAlarma = min_a;
              if(res[15] == 'g'){
                porcion = GRANDE;
              } else if (res[15]== 'p'){
                porcion = PEQUENIA;
              }
            break;
            case 'B':   //Actualiza hora actual y racion
              hora_act += (res[3] - '0') * 10;
              hora_act += (res[4] - '0');
              rtc.hour = hora_act;
              min_act += (res[6] - '0') * 10;
              min_act += (res[7] - '0');
              rtc.min = min_act;
              rtc.sec = 0;
              rtcWrite(&rtc);
              if(res[9] == 'g'){
                porcion = GRANDE;
              } else if (res[9] == 'p'){
                porcion = PEQUENIA;
              }
            break;
            case 'C':   //Actualiza hora de alarma y racion
              hora_a += (res[3] - '0') * 10;
              hora_a += (res[4] - '0');
              horaAlarma = hora_a;
              min_a += (res[6]- '0') * 10;
              min_a += (res[7] - '0');
              minAlarma = min_a;
              if(res[9] == 'g'){
                porcion = GRANDE;
              } else if (res[9] == 'p'){
                porcion = PEQUENIA;
              }
            break;
            case 'D':     //Actualiza solo la racion
              if(res[3] == 'g'){
                porcion = GRANDE;
              } else if (res[3] == 'p'){
                porcion = PEQUENIA;
              }
            break;
          }
          uartWriteString( UART_USB,  res);
          itoa(horaAlarma, uartBuff, 10 );
          uartWriteString( UART_USB, uartBuff);
        }
        val = rtcRead( &rtc );
        if(rtc.sec == 59 && (alimentando == 0)){          //Se mide el nivel de comida una vez por minuto, salvo cuando el motor esta activo
          medida = recipiente_lleno();
          if (medida != -1) recipienteLleno = medida;     //Si la funcion devuelve -1, hubo un error en la medicion y no actualiza la variable
        }
        if(rtc.hour == horaAlarma && rtc.min == minAlarma){   //Llega la hora de alimentar
          if (rtc.sec < porcion && (recipienteLleno ==0)) {   //Activa el motor si el recipiente no esta lleno, durante la el tiempo establecido por el tamaño de porcion
            alimentando = 1;
            gpioWrite( GPIO0 , ON );  //Driver del motor enabled
            gpioWrite( LEDB, ON );
            switch (state){ 
              case 1:            
                gpioWrite( LEDB, ON );
                gpioWrite( GPIO3 , ON );
                gpioWrite( ENET_TXEN , ON );      
                gpioWrite( ENET_RXD1 , OFF );
                gpioWrite( ENET_MDC , OFF );
                state = 2;
              break;
              case 2:
                gpioWrite( LEDB, ON );
                gpioWrite( GPIO3 , OFF );
                gpioWrite( ENET_TXEN , ON );      
                gpioWrite( ENET_RXD1, OFF );
                gpioWrite( ENET_MDC, ON );  
                state = 3;
              break;
              case 3:
                gpioWrite( LEDB, ON );
                gpioWrite( GPIO3 , OFF );
                gpioWrite( ENET_TXEN , OFF );      
                gpioWrite( ENET_RXD1 , ON );
                gpioWrite( ENET_MDC , ON );  
                state = 4;
              break;
              case 4:
                gpioWrite( LEDB, ON );
                gpioWrite( GPIO3 , ON );
                gpioWrite( ENET_TXEN , OFF );      
                gpioWrite( ENET_RXD1 , ON );
                gpioWrite( ENET_MDC, OFF );  
                state = 1;
              break;
            }
          } else { 
                  alimentando = 0;
                  gpioWrite( GPIO0 , OFF );
                  gpioWrite( LEDB, OFF );
          }
      }
      muestra = adcRead( CH1 );   //Toma una muestra del sensor de nivel de agua
      itoa(muestra, uartBuff, 10 );
      uartWriteString( UART_USB, uartBuff);
      uartWriteString( UART_USB, "\r\n");
      if (muestra < NIVEL_MINIMO){    //Si el nivel medido es menor a un nivel minimo aceptable, prende la bomba
        gpioWrite (GPIO7 , ON);   
        gpioWrite (GPIO5 , ON);
        gpioWrite( LED1 , ON);
        bombaPrendida = 1;
      }
      if ((muestra > NIVEL_MAXIMO) && (bombaPrendida == 1)){ //Si el nivel medido supera al nivel maximo, y la bomba esta prendida, la apaga
        gpioWrite (GPIO7 , OFF);   
        gpioWrite (GPIO5 , OFF);
        gpioWrite( LED1 , OFF);
        bombaPrendida = 0;
      }
      showDateAndTime( &rtc );
   }

   /* NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa no es llamado
      por ningun S.O. */
   return 0 ;
}

/*==================[end of file]============================================*/
