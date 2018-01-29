// Modified from:
// http://diotlabs.daraghbyrne.me/3-working-with-sensors/DS18B20/
// https://github.com/daraghbyrne/DesigningIoT/tree/master/4.%20Basics%20of%20Sensors/c.%20Temperature%20DS18B20

// This #include statement was automatically added by the Spark IDE.
#include "OneWire.h"

// This #include statement was automatically added by the Spark IDE.
#include "spark-dallas-temperature.h"

#include "TOKENS.h"

#include <math.h>

// -----------------
// Read temperature
// -----------------

// Data wire is plugged into port 0 on the Arduino
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(D0);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature dallas(&oneWire);
int onewire_device_count = 0;

// Create a variable that will store the temperature value
double temperature = 0.0;

// Create variables to read alarm status. Assumes that
// common is ground
int alarm_nc_pin = D1;
int alarm_no_pin = D2;
bool alarm = FALSE;


void setup()
{

  // Register a Particle Core variable here
  Particle.variable("temperature", &temperature, DOUBLE);
  Particle.variable("alarm", &alarm, BOOLEAN);

  pinMode (alarm_no_pin, INPUT_PULLUP);
  pinMode (alarm_nc_pin, INPUT_PULLUP);

  // setup the library
  // dallas.setResolution(9);
  dallas.begin();

  onewire_device_count = dallas.getDeviceCount();
  Particle.publish("onewire_device_count", String(onewire_device_count));

}

void loop()
{

  // Check the alarm
  if( (digitalRead(alarm_nc_pin) == HIGH) or (digitalRead(alarm_no_pin) == LOW) )
  {
    alarm = TRUE;
  }
  else
  {
    alarm = FALSE;
  }
  Particle.publish("alarm", String(alarm));

  // Loop thorugh the temperature sensors and publish data from each
  dallas.requestTemperatures();

  for( int i = 0; i < onewire_device_count; i++ ) {
    DeviceAddress deviceAddress;

    if ( !dallas.getAddress( deviceAddress, i ) )
    {
        Particle.publish("Warning", "address not valid");
    }

    char rom_address [16];
    sprintf(&rom_address[0], "%02X%02X%02X%02X%02X%02X%02X%02X", deviceAddress[0], deviceAddress[1], deviceAddress[2], deviceAddress[3], deviceAddress[4], deviceAddress[5], deviceAddress[6], deviceAddress[7]);

    float tempC = dallas.getTempCByIndex(i);
    // convert to double
    temperature = (double)tempC;

    if (tempC > -127.0 ){
      Particle.publish("Temperature_"+ String(rom_address), String(tempC));
    }
    else
    {
      Particle.publish("Warning_"+ String(rom_address),"disconnected");
    }

  }

  delay(5000);

}
