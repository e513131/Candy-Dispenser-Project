#include <Servo.h>

#define MOTOR_PIN 11 //motor controlling ramp and windmill
#define CONT_SENSOR_ECHO 3 //sensor detecting if candy has fallen
#define CONT_SENSOR_TRIG 4
#define CANDY_SENSOR_ECHO 5 // sensor detecting if user's candy container is present
#define CANDY_SENSOR_TRIG 6
#define TEST_CONT 10

#define ECHO_TO 30000

Servo candyMotor;

boolean isRepeatedUser;
double distance;
double duration;

void setup() {
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(CANDY_SENSOR_ECHO, INPUT); 
  pinMode(CANDY_SENSOR_TRIG, OUTPUT); 

  pinMode(CONT_SENSOR_ECHO, INPUT);
  pinMode(CONT_SENSOR_TRIG, OUTPUT);

  pinMode(TEST_CONT, INPUT);

  candyMotor.attach(MOTOR_PIN);

  isRepeatedUser = false;

  Serial.begin(9600);
}

double getDistance(int echoPin, int trigPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  double duration = pulseIn(echoPin, HIGH);
  double distance = duration * 0.034 / 2;

  return distance;
}

boolean isReady() {
  return getDistance(CONT_SENSOR_ECHO, CONT_SENSOR_TRIG) < 20;
}

boolean isDispensed() {
  return getDistance(CANDY_SENSOR_ECHO, CANDY_SENSOR_TRIG) < 20;
}

void moveCandy(){
  while(!isDispensed() && isReady()) {
    candyMotor.write(map(60,-100,100,1000,2000));
    Serial.println("running");
    delay(100);
  }
//  candyMotor.write(map(-20,-100,100,1000,2000));
//  delay(500);
  candyMotor.write(0);
  delay(50);
//  Serial.print("stopped");
//  Serial.print("Candy Distance: ");
//  Serial.println(getDistance(CANDY_SENSOR_ECHO, CANDY_SENSOR_TRIG));
//  Serial.print("Container Distance: ");
//  Serial.println(getDistance(CONT_SENSOR_ECHO, CONT_SENSOR_TRIG));
}

void loop() {
  if(!isReady() && isRepeatedUser) { // resets user
    delay(1500);
    if(!isReady()) {
      isRepeatedUser = false;
      Serial.println("R"); 
    }
  }
  else if(isReady() && isRepeatedUser) { // error
    Serial.println("A");
  }
  else if(Serial.available() > 0 && !isRepeatedUser) { // dispenses on command 
    String data = Serial.readStringUntil('\n');
    
    if(data=="detected from raspi!") { 
      Serial.println("I");
      
      moveCandy();
      isRepeatedUser = true;
    }
  }
  Serial.end();
  Serial.begin(9600);
  delay(500);
}
