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
#define SENSOR_ADDRESS_LEN 8
#define SENSOR_ID_LEN 16
// published variable STRING (maximum string length is 622 bytes)
#define MAX_PARTICLE_STR_LEN 622
#define PUBLISH_TTL 60 // default time to live

//need null terminator, separator for each sensor
const int max_sensors = (MAX_PARTICLE_STR_LEN)/(SENSOR_ID_LEN+1);

// string out formats
const char* temp_fmt = ",%.2f";
const char* temp_fmt_ns = "%.2f"; //no separator
const char* id_fmt = ",%s";
const char* id_fmt_ns = "%s"; //no separator
const char* addr_fmt = "%02X";

// Data wire is plugged into port 0 on the Arduino
// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire one_wire(D0);
int sensor_count = 0;
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature dallas(&one_wire);

// array of sensor addresses
uint8_t sensor_address_array[max_sensors][SENSOR_ADDRESS_LEN];
// sensor id array, each is SENSOR_ID_LEN chars wide
char sensor_id_array[max_sensors][SENSOR_ID_LEN];
// temperature array
float temperature_array[max_sensors];
// for "serialized" results
char sensor_ids[MAX_PARTICLE_STR_LEN];
char temperatures[MAX_PARTICLE_STR_LEN];

// Create variables to read alarm status. Assumes that
// common is ground
uint8_t alarm_nc_pin = D1;
uint8_t alarm_no_pin = D2;
bool alarm = FALSE; // easier to send to particle API
bool over_max_sensors = FALSE;

template <class T>
int join_array(T arr_to_join, int arr_len, char *buffer,
                  int buffer_len, const char *fmt, const char *fmt_ns);

void setup() {
  pinMode (alarm_no_pin, INPUT_PULLUP);
  pinMode (alarm_nc_pin, INPUT_PULLUP);
  dallas.begin();

  // Get sensor addresses, store them and their strings
  one_wire.reset_search();
  while (one_wire.search(sensor_address_array[sensor_count])) {
    if (dallas.validAddress(sensor_address_array[sensor_count])) {
      temperature_array[sensor_count] = nanf("");
      join_array<>(sensor_address_array[sensor_count], SENSOR_ADDRESS_LEN,
                   sensor_id_array[sensor_count], SENSOR_ID_LEN, addr_fmt, addr_fmt);
      sensor_count++;
    }
    if (sensor_count == max_sensors){
      break;
    }
  }

  // test if we have more than allowed sensors attached
  if (sensor_count < dallas.getDeviceCount()){
    over_max_sensors = TRUE;
  }
  join_array(temperature_array, sensor_count,
               temperatures, MAX_PARTICLE_STR_LEN, temp_fmt, temp_fmt_ns);
  join_array(sensor_id_array, sensor_count,
               sensor_ids, MAX_PARTICLE_STR_LEN, id_fmt, id_fmt_ns);

  // Register Particle variables
  Particle.variable("temperatures", temperatures);
  Particle.variable("sensor_ids", sensor_ids);
  Particle.variable("sensor_count", sensor_count);
  Particle.variable("alarm", alarm);
}

void loop() {
  // I'm alive!
  Particle.publish("heart_beat", NULL, PUBLISH_TTL, PRIVATE);
  if (sensor_count > 1) {
    // Check the alarm
    if((digitalRead(alarm_nc_pin) == HIGH) or (digitalRead(alarm_no_pin) == LOW)) {
      alarm = TRUE;
      Particle.publish("alarm", "Freezer offline or out of temperature range!",
                       PUBLISH_TTL, PRIVATE);
    } else {
      alarm = FALSE;
    }

    if (over_max_sensors) {
      Particle.publish("warn", "Too many sensors!", PUBLISH_TTL, PRIVATE);
    }

    // Loop thorugh the temperature sensors and publish data from each
    for (int i = 0; i < sensor_count; i++ ) {
      if (!dallas.requestTemperaturesByAddress(sensor_address_array[i])) {
          Particle.publish("disconnect", sensor_id_array[i], PUBLISH_TTL, PRIVATE);
          temperature_array[i] = nanf("");
      } else {
      // read temp with current address
      temperature_array[i] = dallas.getTempC(sensor_address_array[i]);
        if (temperature_array[i] <= DISCON_TEMP) {
          temperature_array[i] = nanf("");
        }
      }
    }
    join_array(temperature_array, sensor_count,
                 temperatures, MAX_PARTICLE_STR_LEN,
                 temp_fmt, temp_fmt_ns);
    Particle.publish("temperatures",  temperatures, PUBLISH_TTL, PRIVATE);
    Particle.publish("sensor_ids",  sensor_ids, PUBLISH_TTL, PRIVATE);
  } else {
    Particle.publish("warn", "No sensors detected!", PUBLISH_TTL, PRIVATE);
  }
  delay(5000);
}

template <class T>
int join_array(T arr_to_join, int arr_len, char *buffer,
                  int buffer_len, const char *fmt, const char *fmt_ns) {
    /*
    Function Parameters
    float *arr_to_join: The array to join
    int arr_len: the length of the array
    char *buffer: buffer to write joined array to
    int buffer_len: length of the buffer
    const char *fmt: format string with leading separator if you want
    const char *fmt_ns: format string without trailing separator

    Returns the number of chars written (sum of returns from snprintf)
    */
    unsigned int written = 0;
    for(int i = 0; i < arr_len; i++) {
        written += snprintf(buffer + written, buffer_len - written,
                            (i != 0 ? fmt : fmt_ns),
                            arr_to_join[i]);
        if(written == buffer_len) {
            break;
        }
    }
    return written;
}
