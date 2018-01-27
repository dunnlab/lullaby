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

  for( int i = 0; a < onewire_device_count; i++ ) {
    DeviceAddress deviceAddress;
    if ( !dallas.getAddress( deviceAddress, i ) )
    {
        Particle.publish("warning", "address not valid");
    }
    char output [23];
    sprintf(&output[0], "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", deviceAddress[0], deviceAddress[1], deviceAddress[2], deviceAddress[3], deviceAddress[4], deviceAddress[5], deviceAddress[6], deviceAddress[7]);
    // sprintf(&output1[0], "%x", deviceAddress1);
    Particle.publish("address_1", String(output));

    float tempC = dallas.getTempCByIndex(i);
    // convert to double
    temperature = (double)tempC;

    // Print out
    Serial.print( "Temp in C = ");
    Serial.print( tempC );

    if (tempC > -127.0 ){
      Blynk.virtualWrite(V5, tempC);
      Particle.publish("temperature", String(tempC));
    }
    else
    {
      Particle.publish("DS18B20_status","disconnected");
    }

  }

  // sin( 23423 );
  // delay(1000);

  // Check the probe status
  //DeviceAddress deviceAddress;
  //if ( dallas.getAddress(deviceAddress, 0) )
  //{
  //  Particle.publish("DS18B20_status","connected");
  //  Particle.publish("raw_temp",String(dallas.getTemp(deviceAddress)));
  //}
  //else
  //{
  //  Particle.publish("DS18B20_status","disconnected");
  //}

  DeviceAddress deviceAddress0;
  if ( !dallas.getAddress( deviceAddress0, 0 ) )
  {
      Particle.publish("warning", "address not valid");
  }
  char output0 [16];
  sprintf(&output0[0], "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", deviceAddress0[0], deviceAddress0[1], deviceAddress0[2], deviceAddress0[3], deviceAddress0[4], deviceAddress0[5], deviceAddress0[6], deviceAddress0[7]);
  //sprintf(&output0[0], "%x", deviceAddress0);
  Particle.publish("address_0", String(output0));

  DeviceAddress deviceAddress1;
  if ( !dallas.getAddress( deviceAddress1, 1 ) )
  {
      Particle.publish("warning", "address not valid");
  }
  char output1 [4];
  sprintf(&output1[0], "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", deviceAddress1[0], deviceAddress1[1], deviceAddress1[2], deviceAddress1[3], deviceAddress1[4], deviceAddress1[5], deviceAddress1[6], deviceAddress1[7]);
  // sprintf(&output1[0], "%x", deviceAddress1);
  Particle.publish("address_1", String(output1));

  // get the temperature in Celcius
  float tempC = dallas.getTempCByIndex(0);
  // convert to double
  temperature = (double)tempC;

  // Print out
  Serial.print( "Temp in C = ");
  Serial.print( tempC );

  // Publish
  Particle.publish("alarm", String(alarm));

  if (tempC > -127.0 ){
    Blynk.virtualWrite(V5, tempC);
    Particle.publish("temperature", String(tempC));
  }
  else
  {
    Particle.publish("DS18B20_status","disconnected");
  }


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
