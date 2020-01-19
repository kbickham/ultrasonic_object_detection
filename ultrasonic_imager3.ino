
/*
  _   _            __  __             _____   _   _   
| | | |          |  \/  |           |  ___| | | | |  
| |_| |__   ___  | .  . | ___  ___  | |_ ___| |_| |_ 
| __| '_ \ / _ \ | |\/| |/ _ \/ __| |  _/ _ \ __| __|
| |_| | | |  __/ | |  | | (_) \__ \ | ||  __/ |_| |_ 
 \__|_| |_|\___| \_|  |_/\___/|___/ \_| \___|\__|\__|
                                                                                                         
     fettBox [Open Source MCU Control box/interface]
     K.Bickham January 2020 
     Email: nBED@gmx.com
     Website: www.n-bed.com (currently down)    

     
*/

#include <Servo.h>
#include <NewPing.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <stdio.h>  

// MCU PINOUT DECLARATIONS
#define ENCODER_CLK_PIN  2 //the clk attach to pin2
#define ENCODER_DT_PIN   3 //the dt attach to pin3
#define ENCODER_SW_PIN   4 //the number of the button
#define PUSHBUTTON_PIN   5
#define SERVO_INPUT_PIN  11    //pin wired to data line on servo
#define ULTRAS_TRIG_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ULTRAS_ECHO_PIN  13  // Arduino pin tied to echo pin on the ultrasonic sensor.

//INPUT PARAMETERS
#define BTN_DEBOUNCE_T 80

//LCD PARAMETERS
#define MENUMAX 3
#define MENU_REFRESH_T 50
 const char* selecttxt = "==select  mode==";
 const char* Menu_Layer_One_Txt[3]= {"Still Scan","Auto Scan","Manual Scan"};
 const char* Menu_Layer_Two_Txt[3]= {"Servo Pos: ", "Object :", "Stop"};

//SERVO PARAMETERS
#define SERVO_MIN_POS 0  //min position for servo
#define SERVO_STILL_POS 30
#define SERVO_MAX_POS 65  //max position for servo  
#define SERVO_DELAY 90 //delay between motions in milliseconds

//ULTRASONIC SENSOR PARAMETERS
#define MAX_DIST 300   // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.



typedef struct
{// int menuL; 
  unsigned long timerM;
  bool menuL;
  int menuV,subV,selV,tempV;
}menust, *pmenust;



typedef struct serv
{ int pin = SERVO_INPUT_PIN;
  int pos = SERVO_MIN_POS;    //INITIAL SERV.posIITION IN MIN ANGLE
  int state =0;
  bool scan_state;
  bool scan_complete = false;
  unsigned long timer = 0;
} serv, *pserv;

typedef struct
{ menust MENU; 
}fetbox, *pfetbox;

serv SERV; //instantiate my data object for servo (need to combine with class in servo.h
fetbox FBOX;

LiquidCrystal_I2C lcd(0x27,16,2);  //set address to 0x27 16X2 char LCD 
Servo serv0;  // Instantiate servo object from class in Servo.h
NewPing sonar(ULTRAS_TRIG_PIN, ULTRAS_ECHO_PIN, MAX_DIST); // NewPing setup of pins and maximum distance.

//////////////////////////////////////////////////////////////////////////
////// Finishing project qc: ALL VARIABLES GO INTO STRUCTS,        ///////             
//     C STYLED DEF CONSTS, OR FIND ANOTHER SOLUTION.                   //
//////////////////////////////////////////////////////////////////////////
          int change;                                                   //
          int distance = 0;                                             //
          int ladder;                                                   //
          int menuchange;                                               //
unsigned long buttonT,selectT=0;                                        //
      uint8_t menuA,menustate;                                          //
         bool select=false;                                             //
          int machinestate;                                             //
          int oldA = HIGH; //set the oldA as HIGH                       //
          int oldB = HIGH; //set the oldB as HIGH                       //
          int oldchange;                                                //
unsigned long refresht;                                                 //
//////////////////////////////////////////////////////////////////////////  


void setup()
{ pinMode(PUSHBUTTON_PIN,INPUT);//initialize the buttonPin as an intput
  pinMode(ENCODER_CLK_PIN, INPUT); //set ENCODER_CLK_PIN,dePin,ENCODER_SW_PIN as INPUT
  pinMode(ENCODER_DT_PIN, INPUT);
  pinMode(ENCODER_SW_PIN, INPUT);
  serv0.attach(SERVO_INPUT_PIN);

  Serial.begin(9600);
     
  lcd.init();        // initialize the lcd 
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("fetBox v0.4a");
  lcd.setCursor(0,1);
  lcd.print("initializing....");
  //SERV.state=1; //setting state for 1
  lcd.clear(); 
  SERV.timer = millis();
  FBOX.MENU.timerM = millis();
  selectT = millis();
  refresht=millis();
  buttonT = millis();
  menuF();
};

void loop()
{
   //INPUT
   buttonREAD();
   dialIN();   
  
   //PROCESSING
   syscntrl();
  
   //OUTPUT
   if (oldchange != change or select == true or FBOX.MENU.menuL==1 )
        {   Serial.println("output conditional met");
            lcd.clear();
            menuF();  //took out of dialIN
        };
  
};




void dialIN()
  {  oldchange=change;
     change = getEncoderTurn();
   switch (FBOX.MENU.menuL)
      {  
        case 0:      
                  FBOX.MENU.tempV = FBOX.MENU.tempV + change;
                  FBOX.MENU.tempV=abs(FBOX.MENU.tempV%3);
                  break;
        case 1:   //syscntrl();
                    break;
      };
  };
      

int getEncoderTurn(void)
{// static int oldA = HIGH; //set the oldA as HIGH
 // static int oldB = HIGH; //set the oldB as HIGH
  int result = 0;
  int newA = digitalRead(ENCODER_DT_PIN);//read the value of ENCODER_CLK_PIN to newA
  int newB = digitalRead(ENCODER_CLK_PIN);//read the value of ENCODER_DT_PIN to newB
  if (newA != oldA || newB != oldB) //if the value of ENCODER_CLK_PIN or the ENCODER_DT_PIN has changed
    {if (oldA == HIGH && newA == LOW)
        {result = (oldB * 2 - 1);   
           };       
     }; 
  oldA = newA;
  oldB = newB;
  return result;
};

void buttonREAD()
{ 
  
  if (millis() - selectT > BTN_DEBOUNCE_T)
      {    if (0 == digitalRead(PUSHBUTTON_PIN))   
                  {  select=true;  
                     selectT=millis();                    
                   };
      };
};

void menuF()
{  if (select==true)
          { lcd.clear();  
             FBOX.MENU.menuL=!FBOX.MENU.menuL;              
            // select=false; 
             //change=0;
             if (FBOX.MENU.menuL == 0)
               { SERV.state=FBOX.MENU.tempV; 
                         }else{ SERV.state=3; };
             FBOX.MENU.timerM=millis();
          };
 if (select==true or (millis() - FBOX.MENU.timerM) >= MENU_REFRESH_T)         
   {       switch (FBOX.MENU.menuL)
        {  
          case 0:                             
                                      lcd.setCursor(0,0); 
                                      lcd.print(Menu_Layer_One_Txt[FBOX.MENU.tempV]);
                                      lcd.setCursor(0,1);
                                      lcd.print(selecttxt);      
                                      break;
          case 1:                     lcd.setCursor(0,0);
                                      lcd.print(Menu_Layer_Two_Txt[0]);
                                      lcd.setCursor(11,0);
                                      lcd.print(SERV.pos);
      
                                      lcd.setCursor(0,1);
                                      lcd.print(Menu_Layer_Two_Txt[1]);
                                      lcd.setCursor(11,1);
                                      lcd.print(distance);                            
                                      break;                                              
        };
    FBOX.MENU.timerM=millis();
   };

 
  select=false; 
};

void syscntrl()
{ if (FBOX.MENU.menuL==1)
    {
        distance= sonar.ping_cm();
                switch (SERV.state) 
            {   case 0: scanStill();
                        break;
                case 1: scanUltra();
                        break;
                case 2: scanManual();
                        break;
                case 3: Serial.println("Doing Nothing");
                        break;
                default: Serial.println("Error Detected");
                        break;
            };
    };
};

void scanStill()
{Serial.println("scanstill");
  if (millis() - SERV.timer >= SERVO_DELAY )
     { if (SERV.pos != SERVO_STILL_POS)
         SERV.pos = SERVO_STILL_POS;
         SERV.timer=millis();
     };
 
};

void scanUltra()
  {Serial.println("ultra");
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
         // distance= sonar.ping_cm();
         // Serial.print("Dist: ");
         //* Serial.println(distance);
          //output to 16x2 would go here in full integrated system
          SERV.timer = millis();
     };
  };

  void scanManual()
{Serial.println("manual");
  if (millis() - SERV.timer >= SERVO_DELAY )
     { // ROTARY ENCODER DRIVEN CODE
       //INSERT HERE
     };
 
};
