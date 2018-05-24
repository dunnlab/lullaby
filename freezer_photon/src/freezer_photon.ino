// Modified from:
// http://diotlabs.daraghbyrne.me/3-working-with-sensors/DS18B20/
// https://github.com/daraghbyrne/DesigningIoT/tree/master/4.%20Basics%20of%20Sensors/c.%20Temperature%20DS18B20
#include "Particle.h"
#include "OneWire.h"
#include "spark-dallas-temperature.h"
#include <math.h>
//#include "TOKENS.h"

#define DISCON_TEMP -127.0
// temps output can be 123.46 or 12.34
#define FLOAT_MAX_LEN 6
#define SENSOR_ID_LEN 16
// published variable STRING (maximum string length is 622 bytes)
#define MAX_PARTICLE_STR_LEN 622
#define PUBLISH_TTL 60 // default time to live

//need null terminator, separator for each sensor
const unsigned int max_sensors = (MAX_PARTICLE_STR_LEN)/(SENSOR_ID_LEN+1);

// string out formats
const char* temp_fmt = ",%.2f";
const char* temp_fmt_ns = "%.2f"; //no separator
const char* id_fmt = ",%s";
const char* id_fmt_ns = "%s"; //no separator
const char* addr_fmt = "%02X";

// Data wire is plugged into port 0 on the Arduino
// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(D0);
unsigned int onewire_device_count = 0;
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature dallas(&oneWire);

// sensor id array, each is SENSOR_ID_LEN chars wide
char** sensor_id_array = new char* [max_sensors];
// temperature array
float temperature_array[max_sensors];
// for "serialized" results
char sensor_ids[MAX_PARTICLE_STR_LEN];
char temperatures[MAX_PARTICLE_STR_LEN];

// Create variables to read alarm status. Assumes that
// common is ground
uint8_t alarm_nc_pin = D1;
uint8_t alarm_no_pin = D2;
bool alarm = FALSE;

template <class T>
size_t join_array(T arr_to_join, size_t arr_len, char *buffer,
                  size_t buffer_len, const char *fmt, const char *fmt_ns);

void setup()
{

  pinMode (alarm_no_pin, INPUT_PULLUP);
  pinMode (alarm_nc_pin, INPUT_PULLUP);
  dallas.begin();
  onewire_device_count = dallas.getDeviceCount();
  if (onewire_device_count > max_sensors)
  for (size_t i = 0; i < max_sensors; i++) {
    sensor_id_array[i] = new char[SENSOR_ID_LEN];
  }

  // strings for storing serialized arrays
  for( size_t i = 0; i < onewire_device_count; i++ ) {
    // nan temperature_array to start
    temperature_array[i] = nanf("");

    DeviceAddress deviceAddress;
    join_array<>(deviceAddress, 8, sensor_id_array[i],
                 SENSOR_ID_LEN, addr_fmt, addr_fmt);
  }
  join_array<>(temperature_array, onewire_device_count,
               temperatures, FLOAT_MAX_LEN, temp_fmt, temp_fmt_ns);
  join_array<>(sensor_id_array, onewire_device_count,
               sensor_ids, SENSOR_ID_LEN, id_fmt, id_fmt_ns);

  // Register Particle variables
  Particle.variable("temperatures", temperatures);
  Particle.variable("sensor_ids", sensor_ids);
  Particle.publish("sensor_ids", PRIVATE);
  Particle.variable("alarm", alarm);
  Particle.variable("sensor_count", onewire_device_count);
  Particle.publish("sensor_count", PRIVATE);
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
  Particle.publish("alarm", String(alarm), PRIVATE);

  // Loop thorugh the temperature sensors and publish data from each
  dallas.requestTemperatures();

  for (size_t i = 0; i < onewire_device_count; i++ ) {
    DeviceAddress deviceAddress;

    if ( !dallas.getAddress( deviceAddress, i ) )
    {
        Particle.publish("bad_address", sensor_id_array[i],
                         PRIVATE);
        temperature_array[i] = nanf("");
    }

    temperature_array[i] = dallas.getTempCByIndex(i);

    if (temperature_array[i] > DISCON_TEMP ){
      Particle.publish("temp_iter",  temperature_array[i], PRIVATE); // will take out later
    }
    else {
      Particle.publish("disconnected", sensor_id_array[i], PRIVATE);
      temperature_array[i] = nanf("");
    }

  }
  temperatures = join_array<>(temperature_array, onewire_device_count,
                              temperatures, FLOAT_MAX_LEN,
                              temp_fmt, temp_fmt_ns);
  delay(5000);

}

template <class T>
size_t join_array(T arr_to_join, size_t arr_len, char *buffer,
                  size_t buffer_len, const char *fmt, const char *fmt_ns) {
    /*
    Function Parameters
    float *arr_to_join: The array to join
    size_t arr_len: the length of the array
    char *buffer: buffer to write joined array to
    size_t buffer_len: length of the buffer
    const char *fmt: format string with leading separator if you want
    const char *fmt_ns: format string without trailing separator

    Returns the number of chars written (sum of returns from snprintf)
    */
    unsigned int written = 0;
    for(size_t i = 0; i < arr_len; i++) {
        written += snprintf(buffer + written, buffer_len - written,
                            (i != 0 ? fmt : fmt_ns),
                            arr_to_join[i]);
        if(written == buffer_len)
            break;
    }
    return written;
}
