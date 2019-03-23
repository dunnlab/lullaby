
// With some code derived from:
// https://build.particle.io/shared_apps/5a1d9e5310c40e5c02001232

// Hardware list - BOM
//
// https://store.particle.io/products/boron-lte Paticle Boron
//
// https://www.adafruit.com/product/938 Monochrome 1.3" 128x64 OLED graphic display
//		- Solder jumpers to put it in i2c mode
//		- Connect to i2c bus and DISPLAY_RESET
//
// https://www.adafruit.com/product/2652 BME280 I2C or SPI Temperature Humidity Pressure Sensor[ID:2652]
// 		- Connect to SPI pins
//
// https://www.adafruit.com/product/3263 Universal Thermocouple Amplifier MAX31856
// 		- Connect to i2c bus
//



#include <Adafruit_MAX31856.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DiagnosticsHelperRK.h>

#define ALARM_NO_PIN A0
#define ALARM_NC_PIN A1
#define SPI_SS_MAX31856 A2
#define SPI_SCLK A3
#define SPI_MISO A4
#define SPI_MOSI A5

#define I2C_SDA D0
#define I2C_SCL D1
#define DISPLAY_RESET D2
#define BUTTON_DISPLAY D3
#define BUTTON_FUNCTION D4
#define ONE_WIRE D5
#define LED_A D6
#define LED_B D7

// public variables
const unsigned long UPDATE_PERIOD_MS = 5000;
unsigned long lastUpdate = 0;
int led_on_state = 0;

// Define board type
// 0 Unknown
// 1 Boron
// 2 Not Boron
int board_type = 0;

double temp_tc = 0;
double temp_tc_cj = 0;
double temp_amb = 0;
double humid_amb = 0;
bool equip_alarm = FALSE;
bool equip_alarm_last = FALSE;
FuelGauge fuel;
double batt_percent = 0;

bool usb_power_last = FALSE;
bool usb_power = TRUE;

bool equip_spec = FALSE;
double alarm_temp_min;
double alarm_temp_max;
bool low_t_alarm = FALSE;
bool low_t_alarm_last = FALSE;
bool high_t_alarm = FALSE;
bool high_t_alarm_last = FALSE;

float alarm_ambient_t_max = 30.0;
float alarm_ambient_h_max = 75.0;
bool amb_t_alarm = FALSE;
bool amb_t_alarm_last = FALSE;
bool amb_h_alarm = FALSE;
bool amb_h_alarm_last = FALSE;

bool fault_bme = FALSE;
bool fault_thermocouple = FALSE;
bool BMEsensorReady = FALSE;


// Use software SPI: CS, DI, DO, CLK
// Pin labels are CS, SDI, SDO, SCK
// formal labels: SS, MISO, MOSI, SCLK
Adafruit_MAX31856 maxthermo = Adafruit_MAX31856(
	SPI_SS_MAX31856,
	SPI_MISO,
	SPI_MOSI,
	SPI_SCLK );

Adafruit_BME280 bme; // I2C

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (1 if sharing Arduino reset pin)
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,OLED_RESET);
Adafruit_SSD1306 display(128, 64, DISPLAY_RESET);
//char buf[64];

// EEPROM addresses
int alarm_temp_min_address = 1;
int alarm_temp_max_address = 5;




void setup() {

	Particle.variable( "temp_tc", &temp_tc, DOUBLE );
	Particle.variable( "temp_tc_cj", &temp_tc_cj, DOUBLE );
	Particle.variable( "temp_amb", &temp_amb, DOUBLE );
	Particle.variable( "humid_amb", &humid_amb, DOUBLE );
	Particle.variable( "equip_alarm", &equip_alarm, BOOLEAN );
	Particle.variable( "batt_percent", &batt_percent, DOUBLE );
	Particle.variable( "usb_power", &usb_power, BOOLEAN );
	Particle.variable( "equip_spec", &equip_spec, BOOLEAN );
	Particle.variable( "low_t_alarm", &low_t_alarm, BOOLEAN );
	Particle.variable( "amb_t_alarm", &amb_t_alarm, BOOLEAN );
	Particle.variable( "amb_h_alarm", &amb_h_alarm, BOOLEAN );
	Particle.variable( "fault_bme", &fault_bme, BOOLEAN );
	Particle.variable( "alarm_t_min", &alarm_temp_min, DOUBLE );
	Particle.variable( "alarm_t_max", &alarm_temp_max, DOUBLE );

	Particle.function("clear_eeprom", clear_eeprom);
	Particle.function("SetAlarmTMax", write_alarm_temp_max);
	Particle.function("SetAlarmTMin", write_alarm_temp_min);


	pinMode( LED_B, OUTPUT );

	pinMode( ALARM_NO_PIN, INPUT_PULLUP );
	pinMode( ALARM_NC_PIN, INPUT_PULLUP );

	alarm_temp_min = read_eeprom_temp( alarm_temp_min_address, -1000 );
	alarm_temp_max = read_eeprom_temp( alarm_temp_max_address, 1000 );

	Serial.begin( 9600 );

	// Start sensors
	maxthermo.begin();
	maxthermo.setThermocoupleType(MAX31856_TCTYPE_K);
	BMEsensorReady = bme.begin();

	if ( ! BMEsensorReady ){
		Particle.publish("FAULT_BME", "BME sensor is not ready", PRIVATE);
		fault_bme = TRUE;
	}

	// Display
	display.begin(SSD1306_SWITCHCAPVCC, 0x3D);

	// Publish some hardware stuff
	#if (PLATFORM_ID == PLATFORM_BORON)

	Particle.publish("Power_stat", String(DiagnosticsHelper::getValue(DIAG_ID_SYSTEM_POWER_SOURCE)), PRIVATE);
	board_type = 1;

	#else
	pinMode(PWR, INPUT);
	Particle.publish("Power_CHG", String(digitalRead(CHG)), PRIVATE);
	Particle.publish("Power_PWR", String(digitalRead(PWR)), PRIVATE);
	board_type = 2;

	#endif

	Particle.publish("BOARD", String(board_type), PRIVATE);




}

void loop() {
	if (millis() - lastUpdate >= UPDATE_PERIOD_MS) {
		// alternate the LED between high and low
		// to show that we're still alive
		digitalWrite(LED_B, (led_on_state) ? HIGH : LOW);
		led_on_state = !led_on_state;


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


		// Read Ambient data
		// Reading temperature or humidity takes about 250 milliseconds

		temp_amb = bme.readTemperature(); // degrees C
		humid_amb = bme.readHumidity(); // %


		// Check if any reads failed
		fault_bme = FALSE;
		if (isnan(temp_amb)) {
			Particle.publish("FAULT_BME", "Failed to read from DHT sensor temperature.", PRIVATE);
			fault_bme = TRUE;
		}

		if (isnan(humid_amb)) {
			Particle.publish("FAULT_BME", "Failed to read from DHT sensor humidity.", PRIVATE);
			fault_bme = TRUE;
		}

		// Read thermocouple data
		temp_tc = maxthermo.readThermocoupleTemperature();
		temp_tc_cj = maxthermo.readCJTemperature();

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

		// Update sensor alarms
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


		if ( (temp_amb > alarm_ambient_t_max) && ! fault_bme ){
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


		if ( (humid_amb > alarm_ambient_h_max && ! fault_bme ) ){
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

		// Create a status summary for display. Shows only one status at a time,
		// put higher priority items later in sequence so they overwrite
		// lower priority items.
		String monitor_status = "System is nominal";
		bool nominal = TRUE;

		if (fault_bme){
			monitor_status =          "FAULT: Ambient sensor";
		}

		if (fault_thermocouple){
			monitor_status =          "FAULT: Thermocouple";
		}

		if ( abs( temp_tc_cj - temp_amb  ) > 3 ){
			monitor_status =          "WARN: Temp mismatch";
		}

		if ( amb_h_alarm ){
			monitor_status =          "ALARM: Ambient humid";
		}

		if ( amb_t_alarm ){
			monitor_status =          "ALARM: Ambient temp";
		}

		if ( ! usb_power ){
			monitor_status =          "FAULT: No power";
		}

		if ( equip_alarm ){
			monitor_status =          "ALARM: Equip. alarm";
		}

		if ( low_t_alarm ){
			monitor_status =          "ALARM: Low temp";
		}

		if ( high_t_alarm ){
			monitor_status =          "ALARM: High temp";
		}

		if ( monitor_status != "System is nominal" ){
			nominal = FALSE;
		}

		// Update display
		display.clearDisplay();
		// Size 1 has xx characters per row
		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.invertDisplay(! nominal);
		display.setCursor(0,0);
		display.println(monitor_status);
		display.println(String::format("Ambient  Temp: %.1f C", temp_amb));
		display.println(String::format("Ambient Humid: %.0f %%", humid_amb));
		display.println("Internal Temp:");
		display.println("");
		display.setTextSize(3);
		display.println(String::format("%.1f C", temp_tc));

		display.display();

	}
}


// EEPROM handlers
// These allow for non-volatile storage of board-specific settings
// EEPROM that has not been written will have a value of 0xFFFFFFFF,
// need to chehck for that and set a default value of EEPROM has not
// been initialized.

double read_eeprom_temp( int address, float default_temp ){
	float temp_read = default_temp;
	uint32_t temp_read_int;
	EEPROM.get( address, temp_read_int );
	if ( temp_read_int != 0xFFFFFFFF ){
		EEPROM.get( address, temp_read );
	}
	// Stored as a float but used as a double so that it works with Particle.variable()
	double temp_read_double = temp_read;
	return (temp_read_double);
}

int write_alarm_temp_max ( String temp_string ) {
	float temp = temp_string.toFloat();
	float address = alarm_temp_max_address;
	EEPROM.put( address, temp);

	// Check the written value and load it into global variable
	float written = read_eeprom_temp( address, -1000000 );
	if ( abs( written - temp ) < 0.1 ){
		// All is well
		alarm_temp_max = written;
		return( 0 );
	}
	else{
		// There was a problem
		return( 1 );
	}
}

int write_alarm_temp_min ( String temp_string ) {
	float temp = temp_string.toFloat();
	float address = alarm_temp_min_address;
	EEPROM.put( address, temp);

	// Check the written value and load it into global variable
	float written = read_eeprom_temp( address, -1000000 );
	if ( abs( written - temp ) < 0.1 ){
		// All is well
		alarm_temp_min = written;
		return( 0 );
	}
	else{
		// There was a problem
		return( 1 );
	}
}

int clear_eeprom( String extra ){
	EEPROM.clear();
	return(0);
}




// Xenon and Boron do power management and status checks differently
// Use conditional directives to pick thhe right approachh and compile time
// See thread at https://community.particle.io/t/battery-charging-indicator-not-working/48345/5


#if (PLATFORM_ID == PLATFORM_BORON)

bool isUsbPowered() {
	// builds on https://community.particle.io/t/boron-battery-connected/47789/9
	int powerSource = DiagnosticsHelper::getValue(DIAG_ID_SYSTEM_POWER_SOURCE);

	if ( (powerSource == 5) || (powerSource == 0) ){
		return( FALSE );
	}
	else{
		return( TRUE );
	}

}

#else

bool isUsbPowered() {
	return( digitalRead(PWR) == HIGH );
}

#endif
