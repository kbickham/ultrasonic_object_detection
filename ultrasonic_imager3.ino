/*
 _   _           ___  ___           ______   _   _   
| | | |          |  \/  |           |  ___| | | | |  
| |_| |__   ___  | .  . | ___  ___  | |_ ___| |_| |_ 
| __| '_ \ / _ \ | |\/| |/ _ \/ __| |  _/ _ \ __| __|
| |_| | | |  __/ | |  | | (_) \__ \ | ||  __/ |_| |_ 
 \__|_| |_|\___| \_|  |_/\___/|___/ \_| \___|\__|\__|
                                                     
                                                     
  ULTRACHEAP ULTRASONIC OBJECT DETECTION AND RANGEFINDING
  by Kris Bickham (the Mos Fett) Jan 2020

  Credits to the follow:
                        BARRAGAN <http://barraganstudio.com>
                        Scott Fitzgerald 
                    
*/

#include <Servo.h>
#include <Wire.h>
#include <NewPing.h>

#define SERVO_INPUT_PIN 3  //pin wired to data line on servo
#define SERVO_MIN_POS 30  //min position for servo
#define SERVO_MAX_POS 135  //max position for servo 
#define SERVO_DELAY 100 //delay between motions in milliseconds
#define TRIG_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     13  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DIST 300   // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

typedef struct serv
{ int pin = SERVO_INPUT_PIN;
  int pos = SERVO_MIN_POS; //INITIAL SERV.posIITION IN MIN ANGLE
  int state =0;
  bool scan_state;
  bool scan_complete = false;
  unsigned long timer = 0;
} serv, *pserv;

int distance = 0;
/*Function Protypes ...to do
 * void scanUltra() {};
 * void syscntrl() {};
 * void scanStill()
 */
 
Servo serv0;  // Instantiate servo object from class in Servo.h
serv SERV; //instantiate my data object for servo (need to combine with class in servo.h
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DIST); // NewPing setup of pins and maximum distance.

void setup() 
{  Serial.begin(9600);
  serv0.attach(3);  // attaches the servo on pin 9 to the servo object
  pinMode (9, OUTPUT);
  SERV.state=1; //setting state for 1
  SERV.timer = millis();
}

void loop()
{ //TAKE INPUT
  syscntrl();
   //PROCESS
   //  NEXT GEN TO DO: build map and convert from polar into caresian 
  //output to display devices
};

void syscntrl()
{   
          switch (SERV.state) 
      {   case 0: scanStill();
                  break;
          case 1: scanUltra();
                  break;
          case 2: scanManual();
                  break;
          default: Serial.println("Error Detected");
                  break;
      }
};

void scanStill()
{
  if (millis() - SERV.timer >= SERVO_DELAY )
     { if (SERV.pos != 90)
         SERV.pos = 90;
         SERV.timer=millis();
     };
 
};

void scanUltra()
  {
  if (millis() - SERV.timer >= SERVO_DELAY )
     {
         switch (SERV.scan_state)
          { case false:  if (SERV.pos <= SERVO_MAX_POS) //ASCENDING
                            { serv0.write(SERV.pos);
                              SERV.pos++;
                            Serial.print("f - Servo POS ");
                            Serial.println(SERV.pos);
                             }else{ SERV.scan_state= true; 
                                                          };
                         break;

            case true:   if (SERV.pos >= SERVO_MIN_POS) 
                            { serv0.write(SERV.pos);
                              SERV.pos--;
                               Serial.print("t - Servo POS ");
                               Serial.println(SERV.pos);
                             }else{ SERV.scan_state= false;
                                                          };//DESCENING
                         break;
          };
          distance= sonar.ping_cm();
          Serial.print("Dist: ");
          Serial.println(distance);
          //output to 16x2 would go here in full integrated system
          SERV.timer = millis();
     };
  };

  void scanManual()
{
  if (millis() - SERV.timer >= SERVO_DELAY )
     { // ROTARY ENCODER DRIVEN CODE
       //INSERT HERE
     };
 
};

//OLD BLOCKING FUNCTION
//bool scanUltra()
//  {    
//          for (SERV.pos = SERVO_MIN_POS;SERV.pos <= SERVO_MAX_POS; SERV.pos += 1) 
//            { serv0.write(SERV.pos);              // tell servo to go to SERV.position in variable 'SERV.pos'
//              delay(15);                       // waits 15ms for the servo to reach the SERV.position
//            };
//          for (SERV.pos = SERVO_MAX_POS;SERV.pos >= SERVO_MIN_POS; SERV.pos -= 1) 
//              { serv0.write(SERV.pos);              // tell servo to go to SERV.position in variable 'SERV.pos'
//                delay(30);                       // waits 15ms for the servo to reach the SERV.position
//              };
//};
