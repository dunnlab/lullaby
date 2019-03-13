#include <Adafruit_DHT.h>
#include <Adafruit_MAX31856.h>

#define ALARM_NO_PIN A0
#define ALARM_NC_PIN A1
#define SPI_SS_MAX31856 A2
#define SPI_SCLK A3
#define SPI_MISO A4
#define SPI_MOSI A5

#define I2C_SDA D0
#define I2C_SCL D1
#define DHT_DATA_PIN D2
#define JUMPER_A D3
#define JUMPER_B D4
#define JUMPER_C D5
#define ONE_WIRE D6
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
bool equip_alarm_last = FALSE;
FuelGauge fuel;
double batt_percent = 0;

PMIC pmic;
bool usb_power_last = FALSE;
bool usb_power = TRUE;

bool equip_spec = FALSE;
int equip_t_nom = -1000;
double alarm_temp_min = -1000;
double alarm_temp_max = 1000;
bool low_t_alarm = FALSE;
bool low_t_alarm_last = FALSE;
bool high_t_alarm = FALSE;
bool high_t_alarm_last = FALSE;

double alarm_ambient_t_max = 30.0;
double alarm_ambient_h_max = 75.0;
bool amb_t_alarm = FALSE;
bool amb_t_alarm_last = FALSE;
bool amb_h_alarm = FALSE;
bool amb_h_alarm_last = FALSE;

bool fault_dht = FALSE;
bool fault_thermocouple = FALSE;


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
	Particle.variable( "usb_power", &usb_power, BOOLEAN );
	Particle.variable( "equip_spec", &equip_spec, BOOLEAN );
	Particle.variable( "equip_t_nom", &equip_t_nom, INT );
	Particle.variable( "low_t_alarm", &low_t_alarm, BOOLEAN );
	Particle.variable( "amb_t_alarm", &amb_t_alarm, BOOLEAN );
	Particle.variable( "amb_h_alarm", &amb_h_alarm, BOOLEAN );


	pinMode( LED_PIN, OUTPUT );

	pinMode( ALARM_NO_PIN, INPUT_PULLUP );
	pinMode( ALARM_NC_PIN, INPUT_PULLUP );

	pinMode( JUMPER_A, INPUT_PULLUP );
	pinMode( JUMPER_B, INPUT_PULLUP );
	pinMode( JUMPER_C, INPUT_PULLUP );

	// Jumpers are used for settings. They are pulled HIGH, so adding a jumper
	// gives a low

	// Nominal temp					JUMPER_A	JUMPER_B	JUMPER_C
	// -70									HIGH			HIGH			HIGH
	// -20									LOW				HIGH			HIGH
	// 4										HIGH			LOW				HIGH
	// 16										LOW 			LOW				HIGH
	// No specification			NA				NA				LOW

	if ( digitalRead( JUMPER_C) == HIGH ){
		equip_spec = TRUE;
	}

	if ( digitalRead( JUMPER_A) == HIGH && digitalRead( JUMPER_A) == HIGH ){
		equip_t_nom = -70;
		alarm_temp_min = -90.0;
		alarm_temp_max = -60.0;
	} else if ( digitalRead( JUMPER_A) == LOW && digitalRead( JUMPER_A) == HIGH ){
		equip_t_nom = -20;
		alarm_temp_min = -27.0;
		alarm_temp_max = -13.0;
	} else if ( digitalRead( JUMPER_A) == HIGH && digitalRead( JUMPER_A) == LOW ){
		equip_t_nom = 4;
		alarm_temp_min = 2;
		alarm_temp_max = 8;
	} else if ( digitalRead( JUMPER_A) == LOW && digitalRead( JUMPER_A) == LOW ){
		equip_t_nom = 16;
		alarm_temp_min = 13;
		alarm_temp_max = 20;
	}

	Serial.begin( 9600 );

	// Start sensors
	pmic.begin();
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
	delay(10000);

	// Check equipment alarms
	if( (digitalRead(ALARM_NC_PIN) == HIGH) or (digitalRead(ALARM_NO_PIN) == LOW) )
	{
		equip_alarm = TRUE;
	}
	else
	{
		equip_alarm = FALSE;
	}

	if( equip_alarm != equip_alarm_last ){
		equip_alarm_last = equip_alarm;
		if ( equip_alarm ){
			Particle.publish("ALARM_EQIP", "ALARM: Equipment in alarm!!!", PRIVATE);
		}
		else
		{
			Particle.publish("ALARM_EQIP", "CLEAR: Equipment alarm stopped.", PRIVATE);
		}

	}

	// Particle.publish("equip_alarm", String(equip_alarm), PRIVATE);

	// Power state
	batt_percent = fuel.getSoC();
	// Particle.publish("batt_percent", String(batt_percent), PRIVATE);

	usb_power = isUsbPowered();
	//Particle.publish("usb_power", String(usb_power), PRIVATE);
	//Particle.publish("byte_power", String(pmic.getSystemStatus()), PRIVATE);

	if (usb_power != usb_power_last) {
		if (usb_power){
			Particle.publish("ALARM_POWER", "CLEAR: Monitor external power restored.", PRIVATE);
		}
		else
		{
			Particle.publish("ALARM_POWER", "ALARM: No monitor external power!!!", PRIVATE);
		}
		usb_power_last = usb_power;
	}


	// Read DHT data
	// Reading temperature or humidity takes about 250 milliseconds
	humid_amb = dht.getHumidity();
	delay(1000);
	temp_amb = dht.getTempCelcius();

	// Check if any reads failed
	fault_dht = FALSE;
	if (isnan(temp_amb)) {
		Particle.publish("FAULT_DHT", "Failed to read from DHT sensor temperature.", PRIVATE);
		fault_dht = TRUE;
	}

	if (isnan(humid_amb)) {
		Particle.publish("FAULT_DHT", "Failed to read from DHT sensor humidity.", PRIVATE);
		fault_dht = TRUE;
	}

	// Particle.publish("humid_amb", String(humid_amb), PRIVATE);
	// Particle.publish("temp_amb", String(temp_amb), PRIVATE);

	// Read thermocouple data
	temp_tc = maxthermo.readThermocoupleTemperature();
	temp_tc_cj = maxthermo.readCJTemperature();

	// Particle.publish("temp_tc", String(temp_tc), PRIVATE);
	// Particle.publish("temp_tc_cj", String(temp_tc_cj), PRIVATE);
	// Check and print any faults
	fault_thermocouple = FALSE;
	uint8_t fault = maxthermo.readFault();
	if (fault) {
		fault_thermocouple = TRUE;
		if (fault & MAX31856_FAULT_CJRANGE) Particle.publish("FAULT_Thermo", "Cold Junction Range Fault", PRIVATE);
		if (fault & MAX31856_FAULT_TCRANGE) Particle.publish("FAULT_Thermo", "Thermocouple Range Fault", PRIVATE);
		if (fault & MAX31856_FAULT_CJHIGH)  Particle.publish("FAULT_Thermo", "Cold Junction High Fault", PRIVATE);
		if (fault & MAX31856_FAULT_CJLOW)   Particle.publish("FAULT_Thermo", "Cold Junction Low Fault", PRIVATE);
		if (fault & MAX31856_FAULT_TCHIGH)  Particle.publish("FAULT_Thermo", "Thermocouple High Fault", PRIVATE);
		if (fault & MAX31856_FAULT_TCLOW)   Particle.publish("FAULT_Thermo", "Thermocouple Low Fault", PRIVATE);
		if (fault & MAX31856_FAULT_OVUV)    Particle.publish("FAULT_Thermo", "Over/Under Voltage Fault", PRIVATE);
		if (fault & MAX31856_FAULT_OPEN)    Particle.publish("FAULT_Thermo", "Thermocouple Open Fault", PRIVATE);
	}

	// Update temperature alarms
	if ( (temp_tc < alarm_temp_min) && equip_spec && ! fault_thermocouple ){
		low_t_alarm = TRUE;
	} else{
		low_t_alarm = FALSE;
	}

	if ( low_t_alarm != low_t_alarm_last ){
		if (low_t_alarm){
			Particle.publish("ALARM_TEMP", "ALARM: Internal temperature below minimum.", PRIVATE);
		}
		else
		{
			Particle.publish("ALARM_TEMP", "CLEAR: Internal temperature no longer below minimum.", PRIVATE);
		}
		low_t_alarm_last = low_t_alarm;
	}


	if ( (temp_tc > alarm_temp_max) && equip_spec && ! fault_thermocouple ){
		high_t_alarm = TRUE;
	} else{
		high_t_alarm = FALSE;
	}

	if ( high_t_alarm != high_t_alarm_last ){
		if (high_t_alarm){
			Particle.publish("ALARM_TEMP", "ALARM: Internal temperature above maximum.", PRIVATE);
		}
		else
		{
			Particle.publish("ALARM_TEMP", "CLEAR: Internal temperature no longer above maximum.", PRIVATE);
		}
		high_t_alarm_last = high_t_alarm;
	}


	if ( (temp_amb > alarm_ambient_t_max) && ! fault_dht ){
		amb_t_alarm = TRUE;
	} else{
		amb_t_alarm = FALSE;
	}

	if ( amb_t_alarm != amb_t_alarm_last ){
		if (amb_t_alarm){
			Particle.publish("ALARM_AMB", "ALARM: Ambient temperature above maximum.", PRIVATE);
		}
		else
		{
			Particle.publish("ALARM_AMB", "CLEAR: Ambient temperature no longer above maximum.", PRIVATE);
		}
		amb_t_alarm_last = amb_t_alarm;
	}


	if ( (humid_amb > alarm_ambient_h_max && ! fault_dht ) ){
		amb_h_alarm = TRUE;
	} else{
		amb_h_alarm = FALSE;
	}

	if ( amb_h_alarm != amb_h_alarm_last ){
		if (amb_t_alarm){
			Particle.publish("ALARM_AMB", "ALARM: Ambient humidity above maximum.", PRIVATE);
		}
		else
		{
			Particle.publish("ALARM_AMB", "CLEAR: Ambient humidity no longer above maximum.", PRIVATE);
		}
		amb_h_alarm_last = amb_h_alarm;
	}

}

bool isUsbPowered() {
	byte systemStatus = pmic.getSystemStatus();
	// observed states when charging include 36, 52, and 180, all of which have
	// bits 32 and 4 high. That corresponds to hex 0x24
	return ((systemStatus & 0x24) == 0x24);
}
