// Interfacing DHT11 sensor with PIC16

// Internal oscillator used @ 8MHz  Have to adjust for 4MHz

// Configuration words: CONFIG1 = 0x2CD4

//                      CONFIG2 = 0x0700

  

// DHT11 sensor connection (here data pin is connected to pin RB0)

#define DHT11_PIN RB5

#define DHT11_PIN_Direction  TRISB5

// End DHT11 sensor connection

#define _XTAL_FREQ 4000000.0    /*for 4mhz*/

 

#include <stdint.h>

uint8_t temperature[20] = "Temp = 00.0 C  ";

uint8_t humidity[20] = "RH   = 00.0 %  ";

uint8_t T_Byte1, T_Byte2, RH_Byte1, RH_Byte2, CheckSum ;

                 

void Start_Signal(void) {

  DHT11_PIN_Direction = 0;                    // Configure connection pin as output

  DHT11_PIN = 0;                              // Connection pin output low

  __delay_ms(25);                               // Wait 25 ms

  DHT11_PIN = 1;                              // Connection pin output high

  __delay_us(25);                               // Wait 25 us

  DHT11_PIN_Direction = 1;                    // Configure connection pin as input

}

unsigned short Check_Response() {

  TMR1H = 0;                                  // Reset Timer1

  TMR1L = 0;

  T1CON |= (1<<0);                           // Enable Timer1 module                          

  while(!DHT11_PIN && TMR1L < 100);           // Wait until DHT11_PIN becomes high (cheking of 80µs low time response)

  if(TMR1L > 99)                              // If response time > 99µS  ==> Response error

    return 0;                                 // Return 0 (Device has a problem with response)

  else {    TMR1H = 0;                        // Reset Timer1

    TMR1L = 0;

    while(DHT11_PIN && TMR1L < 100);          // Wait until DHT11_PIN becomes low (cheking of 80µs high time response)

    if(TMR1L > 99)                            // If response time > 99µS  ==> Response error

      return 0;                               // Return 0 (Device has a problem with response)

    else

      return 1;                               // Return 1 (response OK)

  }

}

uint8_t Read_Data(uint8_t * dht_data) {

  short i;

  *dht_data = 0;

  for(i = 0; i < 8; i++){

    TMR1H = 0;                                // Reset Timer1

    TMR1L = 0;

    while(!DHT11_PIN)                         // Wait until DHT11_PIN becomes high

      if(TMR1L > 100) {                       // If low time > 100  ==>  Time out error (Normally it takes 50µs)

        return 1;

      }

    TMR1H = 0;                                // Reset Timer1

    TMR1L = 0;

    while(DHT11_PIN)                          // Wait until DHT11_PIN becomes low

      if(TMR1L > 100) {                       // If high time > 100  ==>  Time out error (Normally it takes 26-28µs for 0 and 70µs for 1)

        return 1;                             // Return 1 (timeout error)

      }

     if(TMR1L > 50)                           // If high time > 50  ==>  Sensor sent 1

       *dht_data |= (1 << (7 - i));           // Set bit (7 - i)

  }

  return 0;                                   // Return 0 (data read OK)

}