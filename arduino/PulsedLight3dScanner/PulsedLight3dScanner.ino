#include <Servo.h> 
#include <Wire.h>
#include <LIDARLite.h>
 
Servo azimuthServo;  // create servo object to control a servo 
Servo elevationServo; // create servo object to control a servo                 
LIDARLite myLidarLite; 
 
int incrementFlag = 0;
char serialRead;
int pwr_en = 4;
bool runOnce = false;
char *serialValue[32];

void setup() 
{ 
  Serial.begin(115200); 
  
  azimuthServo.attach(9);  // attaches the servo on pin 9 to the servo object 
  elevationServo.attach(10); // attaches the servo on pin 10 to the servo object 
  
  myLidarLite.begin(1,true);    
  
  delay(30);
} 
 


void loop() 
{ 
  if(Serial.available() > 0){
     serialInput();
     if ((String)serialValue[0] == "g"){
       runOnce = true;
       serialValue[0] = "a";
    }
  }
  if(runOnce == true){
      servoAction(
        atoi(serialValue[1]),
        atoi(serialValue[2]),
        atoi(serialValue[3]),
        atoi(serialValue[4]),      
        atoi(serialValue[5]),
        atoi(serialValue[6]),
        atoi(serialValue[7]),
        atoi(serialValue[8])      
        );
        runOnce = false;
  }  
}

int getDistance(){
    int distance = 0;
     distance = myLidarLite.distance();
    return distance;  
}

void servoAction(int azSteps, int azStart, int azEnd, int azDelay, int elSteps, int elStart, int elEnd,int elDelay){
   
   int azimuthPosition = azStart;    // variable to store the servo positionition 
   int elevationPosition = elStart; // variable to store the servo positionition 
   int theDistance = 0; // Store the distance value
   
   for(elevationPosition = elStart;elevationPosition <= elEnd; elevationPosition = elevationPosition + elSteps){
    elevationServo.writeMicroseconds(elevationPosition);
    delay(elDelay);
    for(azimuthPosition = azStart;azimuthPosition <= azEnd; azimuthPosition = azimuthPosition + azSteps){
     azimuthServo.writeMicroseconds(azimuthPosition);
     delay(azDelay);
     if(Serial.available() > 0){
        while(Serial.available() > 0){
          serialRead = Serial.read();
          if(serialRead == 10 || serialRead == 32 ){
          }else{
             goto bailout;
          }
        }
      }
      theDistance = getDistance();
      printValue(azimuthPosition, azStart, elevationPosition, elStart, theDistance, azSteps, elSteps);
    }
  }

  bailout:    
  Serial.println("Finished.........");
}



void printValue(int az, int azStart, int el, int elStart, int distance, int azSteps, int elSteps){
  Serial.print(az+1000);
  Serial.print(el+1000);
  Serial.print(distance+10000);
  Serial.print(azSteps+100);
  Serial.println(elSteps+100);
}





void serialInput() {      // Serial input function routine. Declare inData global String, usage input("message string");
  String inData;                    // Global var buffer contain data from serial input function
  char *pch;
  char *serialArray[32];
  int serialCounter = 0;
  char received;                  // Each character received
  while (received != '\n') {      // When new line character is received (\n = LF, \r = CR)
    if (Serial.available() > 0)   // When character in serial buffer read it
    {
      received = Serial.read();
      inData += received;         // Add received character to inData buffer
    }
  }
  inData.trim();                  // Eliminate \n, \r, blank and other not "printable"
  char *inDataChar = new char[inData.length() + 1]; // create a new char * to copy to
  strcpy(inDataChar, inData.c_str()); // Copy the char so to convert from "const char *" to "char *"
  pch = strtok(inDataChar," ,.-");
  serialCounter = 0;
  while (pch != NULL){
    //Serial.println(atoi(pch));
    serialArray[serialCounter] = pch;
    pch = strtok(NULL, " ,.-");
    serialCounter++;
  }
  for(int i=0;i< serialCounter;i++){
    serialValue[i] = serialArray[i];
  }
}
