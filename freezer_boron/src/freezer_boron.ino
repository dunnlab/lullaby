#include <Adafruit_DHT.h>
#include <Adafruit_MAX31856.h>

#define ALARM_NO_PIN A0
#define ALARM_NC_PIN A1
#define SPI_SS_MAX31856 A2
#define SPI_SCLK A3
#define SPI_MISO A4
#define SPI_MOSI A5


#define DHT_DATA_PIN D2
#define LED_PIN D7



// DHT TYPES
//#define DHT_TYPE DHT11	// DHT 11
#define DHT_DEVICE_TYPE DHT22		// DHT 22 (AM2302)
//#define DHT_TYPE DHT21    // DHT 21 (AM2301)

// internal variables
DHT dht(DHT_DATA_PIN, DHT_DEVICE_TYPE);
int led_on_state = 0;

// public variables
double temp_tc = 0;
double temp_tc_cj = 0;
double temp_amb = 0;
double humid_amb = 0;
bool equip_alarm = FALSE;
bool equip_alarm_new = FALSE;
FuelGauge fuel;
double batt_percent = 0;

// Use software SPI: CS, DI, DO, CLK
// Pin labels are CS, SDI, SDO, SCK
// formal labels: SS, MISO, MOSI, SCLK
Adafruit_MAX31856 maxthermo = Adafruit_MAX31856(
	SPI_SS_MAX31856,
	SPI_MISO,
	SPI_MOSI,
	SPI_SCLK );


void setup() {

	Particle.variable( "temp_tc", &temp_tc, DOUBLE );
	Particle.variable( "temp_tc_cj", &temp_tc_cj, DOUBLE );
	Particle.variable( "temp_amb", &temp_amb, DOUBLE );
	Particle.variable( "humid_amb", &humid_amb, DOUBLE );
	Particle.variable( "equip_alarm", &equip_alarm, BOOLEAN );
	Particle.variable( "batt_percent", &batt_percent, DOUBLE );

	pinMode( LED_PIN, OUTPUT );

	pinMode( ALARM_NO_PIN, INPUT_PULLUP );
	pinMode( ALARM_NC_PIN, INPUT_PULLUP );

	Serial.begin( 9600 );

	// Start sensors
	dht.begin();
	maxthermo.begin();
	maxthermo.setThermocoupleType(MAX31856_TCTYPE_K);
}

void loop() {

	// alternate the LED_PIN between high and low
	// to show that we're still alive
	digitalWrite(LED_PIN, (led_on_state) ? HIGH : LOW);
	led_on_state = !led_on_state;

	// DHT22 max read time is 0.5Hz
	delay(60000);

	// Check equipment alarms
	if( (digitalRead(ALARM_NC_PIN) == HIGH) or (digitalRead(ALARM_NO_PIN) == LOW) )
	{
		equip_alarm_new = TRUE;
	}
	else
	{
		equip_alarm_new = FALSE;
	}

	if( equip_alarm != equip_alarm_new ){
		equip_alarm = equip_alarm_new;
		Particle.publish("new_equip_alarm", String(equip_alarm));
	}

	// Particle.publish("equip_alarm", String(equip_alarm), PRIVATE);


	batt_percent = fuel.getSoC();
	Particle.publish("batt_percent", String(batt_percent), PRIVATE);

	// Read DHT data
	// Reading temperature or humidity takes about 250 milliseconds
	humid_amb = dht.getHumidity();
	temp_amb = dht.getTempCelcius();

	// Check if any reads failed
	if (isnan(humid_amb) || isnan(temp_amb)) {
		Particle.publish("FAULT_DHT", "Failed to read from DHT sensor!");
	}

	// Particle.publish("humid_amb", String(humid_amb), PRIVATE);
	// Particle.publish("temp_amb", String(temp_amb), PRIVATE);

	// Read thermocouple data
	temp_tc = maxthermo.readThermocoupleTemperature();
	temp_tc_cj = maxthermo.readCJTemperature();

	// Particle.publish("temp_tc", String(temp_tc), PRIVATE);
	// Particle.publish("temp_tc_cj", String(temp_tc_cj), PRIVATE);
	// Check and print any faults
	uint8_t fault = maxthermo.readFault();
	if (fault) {
		if (fault & MAX31856_FAULT_CJRANGE) Particle.publish("FAULT_Thermo", "Cold Junction Range Fault");
		if (fault & MAX31856_FAULT_TCRANGE) Particle.publish("FAULT_Thermo", "Thermocouple Range Fault");
		if (fault & MAX31856_FAULT_CJHIGH)  Particle.publish("FAULT_Thermo", "Cold Junction High Fault");
		if (fault & MAX31856_FAULT_CJLOW)   Particle.publish("FAULT_Thermo", "Cold Junction Low Fault");
		if (fault & MAX31856_FAULT_TCHIGH)  Particle.publish("FAULT_Thermo", "Thermocouple High Fault");
		if (fault & MAX31856_FAULT_TCLOW)   Particle.publish("FAULT_Thermo", "Thermocouple Low Fault");
		if (fault & MAX31856_FAULT_OVUV)    Particle.publish("FAULT_Thermo", "Over/Under Voltage Fault");
		if (fault & MAX31856_FAULT_OPEN)    Particle.publish("FAULT_Thermo", "Thermocouple Open Fault");
	}



}
