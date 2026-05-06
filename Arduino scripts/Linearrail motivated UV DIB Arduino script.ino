// Imperial College London - Department of Chemistry
// Motor Control for 2 Motors
// Niall McIntyre 2023 (Adapted from Nick Brooks)

const byte numChars = 32;
char receivedChars[numChars];

char messageFromPC = 0;
int integerFromPC = 0;
float floatFromPC = 0.0;

boolean newData = false;

const int dirPin1 = 2;
const int stepPin1 = 3;
const int MS1 = 5;
const int MS2 = 6;
const int motorSpeed = 500;




// Functions to run the motor

//Sets the direction and speed of motor
void setMotor1Direction (int value) {
  digitalWrite(dirPin1, value);
  if (value == 1) 
  Serial.println("C");
  if (value == 0)
  Serial.println("A");
}

void stop1(int readtime){
  digitalWrite(stepPin1, LOW);
  delayMicroseconds(readtime);

}

void setDistance1 (float distance)
{ digitalWrite(MS1, HIGH);
  digitalWrite(MS2, LOW);// set microstep as half step roughly 50 steps for 1mm
  int x = 0 ;
  while (x<distance)
  {
   digitalWrite(stepPin1, HIGH);
   delayMicroseconds(motorSpeed);
   digitalWrite(stepPin1, LOW);
   delayMicroseconds(motorSpeed);
   x = x + 1;
   //Serial.println(x);
   if (x >= distance) 
   {
    Serial.println(x);
    Serial.println("InPosition");
    break;
    
  
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
    
  }

  else if (messageFromPC == 'L'){
    setDistance1 (floatFromPC);
  }
  
  else if (messageFromPC == 'P'){
    
  }
  else if (messageFromPC == 'S'){
    stop1 (floatFromPC);
  }
  else {
    Serial.println("?");
  }

}






void setup() {
  // put your setup code here, to run once:
	pinMode(stepPin1, OUTPUT);
	pinMode(dirPin1, OUTPUT);
  pinMode(MS1, OUTPUT);
	pinMode(MS2, OUTPUT);
  Serial.begin(115200);
  while (!Serial) {
  delay(1);}
  Serial.print("We are Connected");
  

}

void loop() {
  recvWithStartEndMarkers();
  if (newData == true) {
    parseData();
    showParsedData();
    newData = false;
  }
}