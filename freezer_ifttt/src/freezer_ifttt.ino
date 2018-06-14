
// Create variables to read alarm status. Assumes that
// common is ground
int alarm_nc_pin = D1;
int alarm_no_pin = D2;
bool alarm = FALSE;
bool alarm_new = FALSE;


void setup()
{

  // Register a Particle Core variable here
  Particle.variable("alarm", &alarm, BOOLEAN);

  pinMode (alarm_no_pin, INPUT_PULLUP);
  pinMode (alarm_nc_pin, INPUT_PULLUP);

  // Particle.publish("alarm", String(alarm));

}

void loop()
{

  // Check the alarm
  if( (digitalRead(alarm_nc_pin) == HIGH) or (digitalRead(alarm_no_pin) == LOW) )
  {
    alarm_new = TRUE;
  }
  else
  {
    alarm_new = FALSE;
  }

  if( alarm != alarm_new ){
    alarm = alarm_new;
    Particle.publish("alarm", String(alarm));
  }



  delay(1000);

}
