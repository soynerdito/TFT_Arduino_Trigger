#include <stdint.h>
#include <TouchScreen.h> 
#include <TFT.h>
#include "TouchButton.h"
#include <TimerOne.h>

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

#define TRIGGER_PIN A5

bool STATUS_RUNNING = false;
unsigned long triggerDelay = 500000;

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
TouchButton startBtn = TouchButton(165, 210, 60, 100);
TouchButton clearBtn = TouchButton(165, 20, 60, 100);
TouchButton counterBtn = TouchButton(80, 60, 60, 230);

//Speed controls
TouchButton speedUpBtn = TouchButton(10, 10, 40, 40);
TouchButton speedDownBtn = TouchButton(60, 10, 40, 40);

int counter = 0;

void printButtonCaption( TouchButton *btn, char* text, int color = WHITE, int textSize = 2 )
{
  Tft.setDisplayDirect(DOWN2UP);
  Tft.drawString(text, btn->x + 20,(btn->y + btn->height -8),textSize,color);
}

void drawButton(  TouchButton *btn, int color )
{
  //Tft.drawRectangle(165, 210, 60, 80,RED);
  Tft.fillRectangle( btn->x, btn->y+btn->height, btn->width, btn->height, color);
}

unsigned long tickCount=0;

void setup() {
  Tft.init();  //init TFT library
  //Show Title
  printTitle();

  
  //Refresh Buttons
  refreshStartButton();
  
  
  drawButton( &clearBtn, BLUE );
  printButtonCaption( &clearBtn, STR_CLEAR, WHITE );
  // Speed controls
  drawButton( &speedUpBtn, CYAN );
  printButtonCaption( &speedUpBtn, STR_PLUS, BLACK, 3 );
  drawButton( &speedDownBtn, CYAN );
  printButtonCaption( &speedDownBtn, STR_MINUS, BLACK, 3 );
  
  //Initialize trigger pin
  digitalWrite(TRIGGER_PIN, HIGH);
  pinMode(TRIGGER_PIN, OUTPUT);
  //Causes Refresh of speed into screen
  delayChange(0);
  Serial.begin(9600);

  
}

int lastTriggerCount =0;
void loop() {
  // put your main code here, to run repeatedly:
    // a point object holds x y and z coordinates
  TSPoint p = ts.getPoint();

  if (p.z > ts.pressureThreshhold) {
     Serial.print("Raw X = "); Serial.print(p.x);
     Serial.print("\tRaw Y = "); Serial.print(p.y);
     Serial.print("\tPressure = "); Serial.println(p.z);
  }
  
 
  p.x = map(p.x, TS_MINX, TS_MAXX, 240, 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, 320, 0);
  
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > ts.pressureThreshhold ){
    //Check button clicked
    if( startBtn.isClicked(p.x,p.y) ) {
      onClickStart();  
    }else if( clearBtn.isClicked(p.x,p.y) ) {
      onClickClear();  
    }else if( speedUpBtn.isClicked(p.x,p.y) ) {
      delayChange(5000);
    }else if( speedDownBtn.isClicked(p.x,p.y) ) {
      delayChange(-5000);
    }else{      
      refreshCounterDisplay();      
    }
  }
  /* Serial.print("X = "); Serial.print(p.x);
     Serial.print("\tY = "); Serial.print(p.y);
     Serial.print("\tPressure = "); Serial.println(p.z);
  */
  /*delay(triggerDelay);*/  
  //delay(1000);
  if( STATUS_RUNNING && lastTriggerCount != counter && (millis() - tickCount) > 1000 )
  {
    refreshCounterDisplay();
    lastTriggerCount = counter;
    tickCount = millis();
  }
}


void delayChange(unsigned long speedChange)
{
  triggerDelay += speedChange;
  //Do not allow it to go bellow 0
  triggerDelay = triggerDelay<5000?50000:triggerDelay;
  //Set Timmer
  
  //Timer1.stop();
  //Timer1.initialize(triggerDelay);
  Timer1.setPeriod(triggerDelay);
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
  drawButton( &startBtn, (STATUS_RUNNING?RED:GREEN) );
  printButtonCaption( &startBtn, (STATUS_RUNNING?STR_STOP:STR_START), WHITE );
}
void onClickStart()
{
  STATUS_RUNNING = !STATUS_RUNNING;
  Serial.println("Clicked Start");
  if( STATUS_RUNNING ){    
    Timer1.initialize(triggerDelay);
    Timer1.attachInterrupt( doTrigger );    
  }else{
    Timer1.stop();
  }
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
}

int triggering = 0;
void doTrigger(){
  if(!STATUS_RUNNING || triggering>0 ){ 
    Serial.println("SKIP TRIGGER");
    return; }
  triggering = 1;
  Serial.println("Trigger");
  //digitalWrite( TRIGGER_PIN, digitalRead( TRIGGER_PIN ) ^ 1 );  
  digitalWrite( TRIGGER_PIN, LOW );  
  delay(300);
  digitalWrite( TRIGGER_PIN, HIGH );
  counter++;
  triggering = 0;
}
void refreshCounterDisplay()
{  
  char charBuffer[10];
  drawButton( &counterBtn, GREEN );
  ltoa(counter,charBuffer,10);
  printButtonCaption( &counterBtn, charBuffer, BLACK, 4 );    
}

