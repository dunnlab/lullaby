// Modified from:
// http://diotlabs.daraghbyrne.me/3-working-with-sensors/DS18B20/
// https://github.com/daraghbyrne/DesigningIoT/tree/master/4.%20Basics%20of%20Sensors/c.%20Temperature%20DS18B20

// This #include statement was automatically added by the Spark IDE.
#include "OneWire.h"

// This #include statement was automatically added by the Spark IDE.
#include "spark-dallas-temperature.h"


#include <blynk.h>
#include "TOKENS.h"

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = BLYNK_AUTH_TOKEN;

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

WidgetLED alarm_led(V6);


#include <math.h>

void setup()
{

  Blynk.begin(auth);
  Blynk.setProperty(V6, "color", "#D3435C");

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
  // Serial.begin(9600);

}

void loop()
{
  Blynk.run();

  // Check the alarm
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

  for( int i = 0; i < onewire_device_count; i++ ) {
    DeviceAddress deviceAddress;
    if ( !dallas.getAddress( deviceAddress, i ) )
    {
        Particle.publish("warning", "address not valid");
    }
    char rom_address [16];
    sprintf(&rom_address[0], "%02X%02X%02X%02X%02X%02X%02X%02X", deviceAddress[0], deviceAddress[1], deviceAddress[2], deviceAddress[3], deviceAddress[4], deviceAddress[5], deviceAddress[6], deviceAddress[7]);
    // sprintf(&output1[0], "%x", deviceAddress1);
    // Particle.publish("address", String(rom_address));

    float tempC = dallas.getTempCByIndex(i);
    // convert to double
    temperature = (double)tempC;

    // Print out
    Serial.print( "Temp in C = ");
    Serial.print( tempC );

    if (tempC > -127.0 ){
      Blynk.virtualWrite(V5, tempC);
      Particle.publish("Temperature_"+ String(rom_address), String(tempC));
    }
    else
    {
      Particle.publish("Warning_"+ String(rom_address),"disconnected");
    }

  }


  // Publish
  Particle.publish("alarm", String(alarm));


  if ( alarm ){
    alarm_led.on();
  }
  else
  {
    alarm_led.off();
  }
  //Blynk.virtualWrite(V6, alarm);

  delay(5000);

}
