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

#define TRIGGER_PIN A0

void printTitle()
{
  Tft.setDisplayDirect(DOWN2UP);
  Tft.drawString("Counter",20,300,4,WHITE);
}
TouchButton startBtn = TouchButton(165, 210, 60, 100);
TouchButton clearBtn = TouchButton(165, 20, 60, 100);
TouchButton counterBtn = TouchButton(80, 60, 60, 230);


void printButtonCaption( TouchButton *btn, char* text, int color = WHITE, int textSize = 2 )
{
  Tft.setDisplayDirect(DOWN2UP);
  Tft.drawString(text, btn->x + 20,(btn->y + btn->height -8),2,color);
}

void drawButton(  TouchButton *btn, int color )
{
//  Tft.drawRectangle(165, 210, 60, 80,RED);
    Tft.fillRectangle( btn->x, btn->y+btn->height, btn->width, btn->height, color);
}

void printCounter(int count)
{
  char charBuffer[7];
  drawButton( &counterBtn, GREEN );
  itoa(count,charBuffer,10);
  printButtonCaption( &counterBtn, charBuffer, BLACK, 4 );  
}

bool STATUS_RUNNING = false;

void setup() {
  Tft.init();  //init TFT library
//  Tft.setDisplayDirect(LEFT2RIGHT);
//  Tft.drawString("UP",20,20,4,WHITE);
//  Tft.setDisplayDirect(RIGHT2LEFT);
//  Tft.drawString("DOWN",220,220,4,WHITE);
//  Tft.setDisplayDirect(DOWN2UP);
//  Tft.drawString("LEFT",20,300,4,WHITE);
//  Tft.setDisplayDirect(UP2DOWN);
//  Tft.drawString("RIGHT",220,20,4,WHITE);
  printTitle();  

  
  
/*  Tft.drawRectangle(10, 3, 200,60,BLUE);
  Tft.fillRectangle(0, 80, 100,65,YELLOW);
  Tft.drawRectangle(30, 160, 60, 60,RED);*/
  
//  Tft.drawRectangle(165, 210, 60, 80,RED);
  drawButton( &startBtn, RED );
  printButtonCaption( &startBtn, "Start", WHITE );
  
  drawButton( &clearBtn, BLUE );
  printButtonCaption( &clearBtn, "Clear", WHITE );
  

  
/*
//De lado
  Tft.drawString("Happy!",0,160,1,CYAN);
  Tft.drawString("Happy!",0,200,2,WHITE);
  Tft.drawString("Happy!",0,240,4,WHITE);
  */
    Serial.begin(9600);
}

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
    }else{      
      STATUS_RUNNING = !STATUS_RUNNING;
    }
  }
  /* Serial.print("X = "); Serial.print(p.x);
     Serial.print("\tY = "); Serial.print(p.y);
     Serial.print("\tPressure = "); Serial.println(p.z);
  */

  delay(20);
  if( STATUS_RUNNING )
  {
    doTrigger();    
  }
}

int counter = 0;
void doTrigger()
{
  counter ++;
  printCounter(counter);
}
void onClickStart()
{
   STATUS_RUNNING = true;
}

void onClickClear()
{
  STATUS_RUNNING = false;  
  counter = 0;
  printCounter(counter);
}
