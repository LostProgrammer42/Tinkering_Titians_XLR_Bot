/* 
   This is the code for ESP32 on the bot, follow the steps on how to upload the code on ESP32  
                                 *****IMPORTANT*****
                     The code is incomplete and contains fill in the blanks 
                      You need to fill in the blanks at appropriate places 
   
*/

#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

uint8_t newMACAddress[] = {0xb0, 0xa7, 0x32, 0x2a, 0xbf, 0x0c};


// Define a structure to hold IMU (Inertial Measurement Unit) data
typedef struct {
  float gx, gy, gz;
} IMUData;

IMUData myMessage; // Create a variable to store received IMU data
int cmd = 0;       // Initialize motor control command variable
int spd = 0;       // Initialize motor speed variable

// Function to handle received data from another device
void onDataReceiver(const uint8_t* mac, const uint8_t* incomingData, int len) {
  Serial.println("Message received.");
  // Copy the received data into the myMessage variable
  memcpy(&myMessage, incomingData, sizeof(myMessage));
  
  // Display received data
  Serial.println("=== Data ===");
  Serial.print("Mac address: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(mac[i], HEX);  // Display MAC address
    if (i < 5) Serial.print(":");
  }
  Serial.println();
  Serial.print("gx: ");
  Serial.print(myMessage.gx);    // Display x-axis gyro value
  Serial.print(", gy: ");
  Serial.print(myMessage.gy);    // Display y-axis gyro value
  Serial.print(", gz: ");
  Serial.println(myMessage.gz);   // Display z-axis gyro value
}

// Function to update motor control based on received IMU data
void updateMotorControl() {
  float gx = myMessage.gx;
  float gy = myMessage.gy;
  float gz = myMessage.gz;

  // Motor control logic based on IMU data
  if ((gz != 0) && (gx != 0) && (abs(gy) < 2)) {
    spd = constrain(abs(map((atan2(gx, gz) * 180 / PI), 0, 90, 0, 255)), 0, 255);
    cmd = (gx > 0) ? 1 : 2; // Forward or backward
  } else if ((gz != 0) && (gy != 0) && (abs(gx) < 2)) {
    spd = constrain(abs(map((atan2(gy, gz) * 180 / PI), 0, 90, 0, 255)), 0, 255);
    cmd = (gy > 0) ? 3 : 4; // Right or left
  } else {
    cmd = 0; // Stop
    spd = 0;
  }


  // Adjust motor speed thresholds
  if (spd > 60 && spd < 150) {
    spd = 150;
  }
  if (spd > 150 && spd < 255) {
    spd = 255;
  }

  // Display motor control information
  Serial.print("cmd: ");
  Serial.print(cmd);   // Display motor command
  Serial.print(", speed: ");
  Serial.println(spd); // Display motor speed
}

// Pin assignments for motor control
// These pins are the Enable pins of the L298N motor driver which connects to esp32 gpio pins to implement the PWM function

const int ENA = 12;  // choose the GPIO pin of esp32;
const int ENB = 14;  // choose the GPIO pin of esp32;
// These pins are the input pins of l298N on the left side
const int IN1 = 13;  // Choose your GPIO pin of esp32 for the input 1
const int IN2 = 25;  // Choose your GPIO pin of esp32 for the input 2

// These pins are the input pins of l298N on the right side
const int IN3 = 19;  // Choose your GPIO pin of esp32 for the input 3
const int IN4 = 18;  // Choose your GPIO pin of esp32 for the input 4
int counter = 0;





// Setup function
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);

  Serial.println("Setup started");

  // Configure motor control pins as outputs
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);    //fill in the blanks 
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);    //fill in the blanks   
  pinMode(IN3, OUTPUT);    //fill in the blanks 
  pinMode(IN4, OUTPUT);



  Serial.print("Mac Address: ");
  Serial.print(WiFi.macAddress());
  Serial.println(" ESP32 ESP-Now Broadcast");

  // Initialize ESP-NOW communication
  if (esp_now_init() != 0) {
    Serial.println("Problem during ESP-NOW init");
    return;
  }

  // Register the data receiver callback function
  esp_now_register_recv_cb(onDataReceiver);
}

// Function to apply motor control based on command and speed
void applyMotorControl() {
  switch (cmd) {

    // You have to develop the logic that, when the IMU is tilted front to go forward, Then the esp32 executes the following commands
    // Refer to get electrified slides for more help
    case 1:  // Forward
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      break;
    case 2:  // Backward
      digitalWrite(IN1,LOW);
      digitalWrite(IN2,HIGH);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      break;
    case 3:  // Right
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      break;
    case 4:  // Left
      digitalWrite(IN1,LOW);
      digitalWrite(IN2,HIGH);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      break;
    default:  // Stop
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      spd = 0;
      break;
  }

  // Apply the calculated motor speed to both motors
  // fill in the blanks to finalize the code
  analogWrite(ENA, spd);
  analogWrite(ENB, spd);
 

}
void reset_comms(){
  if(counter ==100){
  esp_now_register_recv_cb(onDataReceiver);
  delay(1);
  counter = 0;}
  counter +=1;
}
// Main loop
void loop() {
  // Continuously update and apply motor control
  
  updateMotorControl();
  applyMotorControl();
  reset_comms();

  


  delay(100); // Delay to control loop speed
}