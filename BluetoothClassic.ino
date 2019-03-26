// Ben Goenner
// 11/13/2018
// Embedded Systems Project

// Use bluetooth dongle. Connect power and ground.
// Connect Tx of the dongle to pin 0 of the Arduino and connect Rx to pin 1.
// Wire pin 7 to resistor in series with LED. Wire LED to ground.

#include <SPI.h>

char inData = '0';
int led = 7; // LED on pin 7
int ss = 10; // slave select on pin 10

// set up the speed, data order, and data mode
SPISettings settingsA(2000000, MSBFIRST, SPI_MODE3); // CPOL and CPHA = 1, rate = 2 MHz

void setup() {
  Serial1.begin(9600);
  Serial1.println("Hello"); // say Hello to the andrid tablet to be nice
  pinMode(led, OUTPUT); // LED is an output
  pinMode (ss, OUTPUT); // set the Slave Select Pins as outputs:
  
  // initialize SPI:
  SPI.setClockDivider(SPI_CLOCK_DIV16); // make the SPI clock 2 MHz
  SPI.begin();
}

void loop() {
  if (Serial1.available()) {
    // blink LED so you know data was bluetoothed to arduino
    digitalWrite(led,HIGH);
    delay(1000);
    digitalWrite(led,LOW);
    
    inData = Serial1.read(); // save the message into inData
    delay(1000);
    SPI.beginTransaction(settingsA);
    digitalWrite (ss, LOW);
    SPI.transfer(inData); // SPI inData to nucleo
    digitalWrite (ss, HIGH);
    SPI.endTransaction();
  }
}


