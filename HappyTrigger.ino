#include <stdint.h>
#include <TouchScreen.h> 

#include <TFT.h>  

#include "TouchButton.h"

#ifdef SEEEDUINO
  #define YP A2   // must be an analog pin, use "An" notation!
  #define XM A1   // must be an analog pin, use "An" notation!
  #define YM 14   // can be a digital pin, this is A0
  #define XP 17   // can be a digital pin, this is A3 
#endif

#ifdef MEGA
  #define YP A2   // must be an analog pin, use "An" notation!
  #define XM A1   // must be an analog pin, use "An" notation!
  #define YM 54   // can be a digital pin, this is A0
  #define XP 57   // can be a digital pin, this is A3 
#endif 

//Measured ADC values for (0,0) and (210-1,320-1)
//TS_MINX corresponds to ADC value when X = 0
//TS_MINY corresponds to ADC value when Y = 0
//TS_MAXX corresponds to ADC value when X = 240 -1
//TS_MAXY corresponds to ADC value when Y = 320 -1

#define TS_MINX 140
#define TS_MAXX 900

#define TS_MINY 120
#define TS_MAXY 940


// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// The 2.8" TFT Touch shield has 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 450);

#define TRIGGER_PIN 19

bool STATUS_RUNNING = false;
unsigned long triggerDelay = 500;

char STR_CLEAR[] = "Clear";
char STR_START[] = "Start";
char STR_STOP[] = "Stop";
char STR_PLUS[] = "+";
char STR_MINUS[] = "-";
char STR_COUNTER[] = "Counter";

void printTitle()
{
  Tft.setDisplayDirect(DOWN2UP);  
  Tft.drawString(STR_COUNTER,20,300,4,WHITE);
}
TouchButton startBtn = TouchButton(&Tft, 165, 210, 60, 100, GREEN);
TouchButton clearBtn = TouchButton(&Tft, 165, 20, 60, 100, BLUE, WHITE, STR_CLEAR, 2 );
TouchButton counterBtn = TouchButton(&Tft, 80, 60, 60, 230, GREEN, BLACK, NULL, 4 );

//Speed controls
TouchButton speedUpBtn = TouchButton(&Tft, 10, 10, 40, 40, CYAN, BLACK,STR_PLUS, 3 );
TouchButton speedDownBtn = TouchButton(&Tft, 60, 10, 40, 40, CYAN, BLACK,STR_MINUS, 3 );

int counter = 0;
unsigned long tickCount=0;

void setup() {
  Tft.init();  //init TFT library
  //Show Title
  printTitle();
    
  //Draw buttons for first time
  refreshStartButton();
  clearBtn.draw( );
  // Speed controls  
  speedUpBtn.setTextMargin(10,8);
  speedDownBtn.setTextMargin(10,8);
  speedUpBtn.draw();
  speedDownBtn.draw();
  
  //Initialize trigger pin
  digitalWrite(TRIGGER_PIN, HIGH);
  pinMode(TRIGGER_PIN, OUTPUT);
  //Causes Refresh of speed into screen
  delayChange(0);
  Serial.begin(115200);

}

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 350;    // the debounce time; increase if the output flickers

unsigned long triggerMillis = 0;
int lastTriggerCount =0;
unsigned long debounceMillis = 0;
bool screenPressed = false;

void loop() {
  // put your main code here, to run repeatedly:
    // a point object holds x y and z coordinates
  TSPoint p = ts.getPoint();

  /*if (p.z > ts.pressureThreshhold) {
     Serial.print("Raw X = "); Serial.print(p.x);
     Serial.print("\tRaw Y = "); Serial.print(p.y);
     Serial.print("\tPressure = "); Serial.println(p.z);
  }*/
  p.x = map(p.x, TS_MINX, TS_MAXX, 240, 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, 320, 0);

  
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > ts.pressureThreshhold && !screenPressed && (millis() - lastDebounceTime) > debounceDelay ){
    lastDebounceTime = millis();
    screenPressed = true;
    /*Serial.print("Map X = "); Serial.print(p.x);
    Serial.print("\tMap Y = "); Serial.print(p.y);
    Serial.print("\tPressure = "); Serial.println(p.z);*/
    
    //Check button clicked
    if( startBtn.isClicked(p.x,p.y) ) {
      onClickStart();  
    }else if( clearBtn.isClicked(p.x,p.y) ) {
      onClickClear();  
    }else if( speedUpBtn.isClicked(p.x,p.y) ) {      
      delayChange(-10);
    }else if( speedDownBtn.isClicked(p.x,p.y) ) {      
      delayChange(10);
    }else{      
      refreshCounterDisplay();      
    }
  }else{
    screenPressed = false;
  }
  /* Serial.print("X = "); Serial.print(p.x);
     Serial.print("\tY = "); Serial.print(p.y);
     Serial.print("\tPressure = "); Serial.println(p.z);
  */
  //delay(1500);
  if( STATUS_RUNNING && (millis() - triggerMillis) > triggerDelay ){
    doTrigger();
    triggerMillis = millis();
    Serial.println(triggerMillis);
  }
  /*delay(triggerDelay);*/  
  //delay(1000);
  /*if( STATUS_RUNNING && lastTriggerCount != counter && (millis() - tickCount) > 60000 )
  {
    refreshCounterDisplay();
    lastTriggerCount = counter;
    tickCount = millis();
  }*/

  
}

void speedDown(unsigned long speedChange)
{
  
}
void speedUp(unsigned long speedChange)
{
  
}

void delayChange(int speedChange)
{
  Serial.println("Trigger Delay Change");
  Serial.println(triggerDelay);
  if( speedChange<0 && abs(speedChange) > triggerDelay ){
    //Do not allow it to go bellow 0
    triggerDelay = triggerDelay;
    Serial.println("Do not allow go negative"); 
  }else{
    triggerDelay += speedChange;   
  }
  
  Serial.println(triggerDelay); 
  //Set Timmer  
  //Timer1.setPeriod(triggerDelay);
  //Timer1.attachInterrupt( doTrigger ); // attach the service routine here  
  //Timer1.start();
  char speedBuffer[10];
  ltoa(triggerDelay, speedBuffer, 10 );
  // Draw speed counter display
  Tft.fillRectangle( 141, 100, 20, 90, BLACK);
  Tft.drawRectangle( 141, 10, 20, 90, YELLOW);
  Tft.drawString(speedBuffer,150,70,1,YELLOW);
}

void refreshStartButton()
{
  startBtn.setColor((STATUS_RUNNING?RED:GREEN));
  startBtn.setCaption( (STATUS_RUNNING?STR_STOP:STR_START) );
  startBtn.draw();
}

void onClickStart()
{
  STATUS_RUNNING = !STATUS_RUNNING;
  Serial.println("Clicked Start");  
  refreshStartButton();
  //Timer1.initialize(180000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  delayChange(0);
  refreshCounterDisplay();
}

void onClickClear()
{
  STATUS_RUNNING = false;  
  counter = 0;
  refreshCounterDisplay();
  refreshStartButton();
}

void doTrigger(){
  if(!STATUS_RUNNING ){ 
    Serial.println("SKIP TRIGGER");
    return; 
  }
  Serial.println("Trigger");  
  digitalWrite( TRIGGER_PIN, LOW );  
  delay(10);
  digitalWrite( TRIGGER_PIN, HIGH );
  counter++;
}

void refreshCounterDisplay()
{  
  char charBuffer[10];
  ltoa(counter,charBuffer,10);
  counterBtn.setCaption( charBuffer );
  counterBtn.draw();
}

