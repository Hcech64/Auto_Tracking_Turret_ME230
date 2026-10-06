#include <Adafruit_TMF8806.h>
#include <Servo.h>
Adafruit_TMF8806 tof;
Servo Scanner;

  // Variable Definition
  int two_theta = 0;
  boolean direction = true;
  int dist = 10000;
  int min_dist = 10000;
  int two_theta_min = 0;

void setup() {

//Servo Setup
  Scanner.attach(2);

  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }

  Serial.println(F("TMF8806 Time-of-Flight Sensor Test"));

  if (!tof.begin()) {
    Serial.println(F("Failed to find TMF8806 sensor!"));
    while (1) {
      delay(10);
    }
  }
  Serial.println(F("TMF8806 found!"));

  // Read and print serial number
  uint8_t serial[4];
  if (tof.readSerialNumber(serial, 4)) {
    Serial.print(F("Serial number: 0x"));
    for (int i = 3; i >= 0; i--) {
      if (serial[i] < 0x10) {
        Serial.print(F("0"));
      }
      Serial.print(serial[i], HEX);
    }
    Serial.println();
  }

  // Configure sensor (optional - defaults work well)
  tof.setDistanceMode(TMF8806_MODE_5M); // 5m mode (Not Default)
  tof.setIterations(400);                 // 400k iterations
  tof.setRepetitionPeriod_ms(100);         // ~10Hz

  // Start continuous measurement
  if (!tof.startMeasuring(true)) {
    Serial.println(F("Failed to start measurement!"));
    while (1) {
      delay(10);
    }
  }
  Serial.println(F("Measuring..."));
  Serial.println();
}

void loop() {

  //update Scanner angle
  if(two_theta >= 180*2){
    direction = false;
  }
  if(two_theta <= 0){
    direction = true;
  }
  if(direction){
    two_theta +=1;
    Scanner.write(two_theta/2);
  }
  if(!direction){
    two_theta-=1;
    Scanner.write(two_theta/2);
  }
  //Serial.print("   Theta = ");
  //Serial.print(two_theta);
  

  if (tof.dataReady()) {
    tmf8806_result_t result;
    if (tof.readResult(&result) && result.reliability > 0) {
      Serial.print(F("Distance: "));
      Serial.print(result.distance);
      Serial.print(F(" mm"));
      dist = result.distance;

        if(dist < min_dist){
          min_dist = dist;
          two_theta_min = two_theta;
          Serial.print("MIN: ");
          Serial.print(min_dist);

          Serial.print("THEA MIN: ");
          Serial.print(two_theta_min);
 
        }
    }
    Serial.println(); 

// LOCK ON TO CLOSEST TARGET AFTER 1 PAN
    if(!direction && two_theta <= 4){
      Scanner.write(two_theta_min/2);
      while(true){
        delay(1000);
      }
    }
  }
  
  delay(10);
  
}
