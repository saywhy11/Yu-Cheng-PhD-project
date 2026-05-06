// Imperial College London - Department of Chemistry
// Motor Homing with limitswitch 
// Yu Cheng 2024
#include <ezButton.h>
const int ls_1= 10;
ezButton limitSwitch1(ls_1);  // create ezButton object that attach to pin 7;
const int ls_2= 4;
ezButton limitSwitch2(ls_2);
const byte numChars = 32;
char receivedChars[numChars];
char messageFromPC = 0;
int integerFromPC = 0;
float floatFromPC = 0.0;
boolean newData = false;
const int dirPin1 = 2;
const int stepPin1 = 3;
const int dirPin2 = 8;
const int stepPin2 = 9;
const int motorSpeed = 1000; //500ms = 1mm/s
// Functions to run the motor

//Sets the direction and speed of motor
void setMotor1Direction (int value) {
  digitalWrite(dirPin1, value);
  if (value == 1) 
  Serial.println("C");
  if (value == 0)
  Serial.println("A");
}


void setMotor2Direction (int value) {
  digitalWrite(dirPin2, value);
  if (value == 1) 
  Serial.println("C");
  if (value == 0)
  Serial.println("A");
}


void stop1(int readtime){
  digitalWrite(stepPin1, LOW);
  delayMicroseconds(readtime);

}

void setDistance1 (float distance){//not changed
  for (int x = 0; x < distance; x++)
  {
    digitalWrite(stepPin1, HIGH);
    delayMicroseconds(motorSpeed);
    digitalWrite(stepPin1, LOW);
    delayMicroseconds(motorSpeed);
    //Serial.println( x);
    if (x == distance-1)
    Serial.println("InPosition");
  }
}

void setDistance2 (float distance){//not changed
  for (int x = 0; x < distance; x++)
  {
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(motorSpeed);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(motorSpeed);
    //Serial.println( x);
    if (x == distance-1)
    Serial.println("InPosition");
  }
}
void Homing() {
  // Check if any limit switch is pressed
    // Update the state of the limit switches
  limitSwitch1.loop();
  limitSwitch2.loop();
  if (limitSwitch1.isPressed() || limitSwitch2.isPressed()) {
    Serial.println("Cannot do homing now!!! Limit switch is pressed.");
    return; // Exit the function if any limit switch is pressed
  
  }
  else { 
    Serial.println("Homing is starting in 5 seconds!");
    delay(5000);
    }

  bool isStopped1 = false;
  bool isStopped2 = false;
  int x = 0;
  int y = 0;
  const int Homesteps = 1;

  // Home stepmotor1
  while (!isStopped1) {
    limitSwitch1.loop();
    setMotor1Direction(1);
    setMotor2Direction(1);
    setDistance1(Homesteps);
    if (limitSwitch1.isPressed()) {
      isStopped1 = true;
      Serial.println("Stepper 1 stopped");
    } else {
      x += 1; // Increment x only if limit switch is not pressed
    }
  }

  // Home stepmotor2
  if (isStopped1) {
    while (!isStopped2) {
      limitSwitch2.loop();
      setDistance2(Homesteps);
      if (limitSwitch2.isPressed()) {
        isStopped2 = true;
        Serial.println("Stepper 2 stopped");
        Serial.println("HomeXsteps is");
        Serial.println(x * Homesteps); // Multiply by Homesteps before printing
        Serial.println("HomeYsteps is");
        Serial.println(y * Homesteps); // Multiply by Homesteps before printing
        Serial.println("Homing is done!");
        return;
      } else {
        y += 1; // Increment y only if limit switch is not pressed
      }
    }
  }
}


/****** Functions to handle serial communication with the computer*******/


void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}


void parseData() {      // split the data into its parts

  char* atofIndx;

  //First character represents command
  messageFromPC = receivedChars[0];

  //Remainder of received data converted to number
  atofIndx = receivedChars + 1;
  floatFromPC = atof(atofIndx);

}

// Process commands received from the computer

void showParsedData() {
  /*
    Serial.print("Message ");
    Serial.println(messageFromPC);
    Serial.print("Float ");
    Serial.println(floatFromPC);
  */

  if (messageFromPC == 'M') {
    setMotor1Direction (floatFromPC);
  }

  else if (messageFromPC == 'N') {
    setMotor2Direction (floatFromPC);
  }

  else if (messageFromPC == 'L'){
    setDistance1 (floatFromPC);
  }
  
  else if (messageFromPC == 'P'){
    setDistance2 (floatFromPC);
  }
  else if (messageFromPC == 'S'){
    stop1 (floatFromPC);
  }
  else if (messageFromPC == 'H'){
    Homing();
  }  
  else {
    Serial.println("?");
  }

}

void setup() {
  // put your setup code here, to run once:
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  Serial.begin(115200);
  while (!Serial) {
  delay(1);}
  Serial.print("We are Connected");
  //Serial.begin(9600);
  limitSwitch1.setDebounceTime(50); // set debounce time to 50 milliseconds
  limitSwitch2.setDebounceTime(50); // set debounce time to 50 milliseconds
}

void loop() {
  recvWithStartEndMarkers();
  if (newData == true) {
    parseData();
    showParsedData();
    newData = false;
  }
  
}
