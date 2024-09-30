// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 100      // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300.0   // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL) // coefficent to convert duration to distance

unsigned long last_sampling_time;   // unit: msec

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);  // sonar TRIGGER
  pinMode(PIN_ECHO, INPUT);   // sonar ECHO
  digitalWrite(PIN_TRIG, LOW);  // turn-off Sonar 
  
  // initialize serial port
  Serial.begin(57600);
}

void loop() {
  float distance;

  // wait until next sampling time. 
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  distance = USS_measure(PIN_TRIG, PIN_ECHO); // read distance

  // LED brightness control based on distance
  if (distance > _DIST_MAX || distance == 0.0) {
      distance = _DIST_MAX + 10.0;    // Set Higher Value
      analogWrite(PIN_LED, 255);      // LED OFF
  } else if (distance < _DIST_MIN) {
      distance = _DIST_MIN - 10.0;    // Set Lower Value
      analogWrite(PIN_LED, 255);      // LED OFF
  } else {
      int brightness = 0;
      if (distance < 150.0) {
          // Gradually increase brightness from 100mm to 150mm
          brightness = map(distance, 100, 150, 255, 127); // 150mm에서 50% 밝기
      } else if (distance <= 200.0) {
          // Maximum brightness at 200mm
          brightness = map(distance, 150, 200, 127, 0); // 200mm에서 최대 밝기
      } else if (distance <= 250.0) {
          // Decrease brightness from 200mm to 250mm
          brightness = map(distance, 200, 250, 0, 127); // 250mm에서 50% 밝기
      } else {
          // Decrease brightness from 250mm to 300mm
          brightness = map(distance, 250, 300, 127, 255); // 300mm에서 최소 밝기
      }
      analogWrite(PIN_LED, brightness);  // Adjust brightness based on distance
  }

  // output the distance to the serial port
  Serial.print("Min:");        Serial.print(_DIST_MIN);
  Serial.print(",distance:");  Serial.print(distance);
  Serial.print(",Max:");       Serial.print(_DIST_MAX);
  Serial.println("");
  
  // do something here
  delay(50); // Assume that it takes 50ms to do something.
  
  // update last sampling time
  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}
