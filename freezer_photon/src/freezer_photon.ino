// Modified from:
// http://diotlabs.daraghbyrne.me/3-working-with-sensors/DS18B20/
// https://github.com/daraghbyrne/DesigningIoT/tree/master/4.%20Basics%20of%20Sensors/c.%20Temperature%20DS18B20

// This #include statement was automatically added by the Spark IDE.
#include "OneWire.h"

// This #include statement was automatically added by the Spark IDE.
#include "spark-dallas-temperature.h"

// -----------------
// Read temperature
// -----------------

// Data wire is plugged into port 0 on the Arduino
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(D0);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature dallas(&oneWire);

// Create a variable that will store the temperature value
double temperature = 0.0;

// Create variables to read alarm status. Assumes that
// common is ground
int alarm_nc_pin = D1;
int alarm_no_pin = D2;
bool alarm = FALSE;

#include <math.h>

void setup()
{
  // Register a Particle Core variable here
  Particle.variable("temperature", &temperature, DOUBLE);
  Particle.variable("alarm", &alarm, BOOLEAN);

  pinMode (alarm_no_pin, INPUT_PULLUP);
  pinMode (alarm_nc_pin, INPUT_PULLUP);

  // setup the library
  dallas.begin();

  Serial.begin(9600);

}

void loop()
{

  if( (digitalRead(alarm_nc_pin) == HIGH) or (digitalRead(alarm_no_pin) == LOW) )
  {
    alarm = TRUE;
  }
  else
  {
    alarm = FALSE;
  }

  // Request temperature conversion (traditional)
  dallas.requestTemperatures();

  sin( 23423 );

  // get the temperature in Celcius
  float tempC = dallas.getTempCByIndex(0);
  // convert to double
  temperature = (double)tempC;

  // Print out
  Serial.print( "Temp in C = ");
  Serial.print( tempC );

  // Publish
  Particle.publish("temperature", String(tempC));
  Particle.publish("alarm", String(alarm));

  delay(5000);

}
