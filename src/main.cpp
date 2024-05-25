#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
// #define CF_S24  &Satisfy_24
#include "PCF8575.h"

PCF8575 pcf8575(0x27);
TFT_eSPI tft = TFT_eSPI();

#include <JPEGDecoder.h>
// Return the minimum of two values a and b
#define minimum(a,b)     (((a) < (b)) ? (a) : (b))

// Include the sketch header file that contains the image stored as an array of bytes
// More than one image array could be stored in each header file.
#include "anhnen.h"
#include "nen_default.h"
#include "nen_custom.h"
#include "nentgtap.h"
#include "nendangtap.h"
#include "nentayphai.h"
#include "nentaytrai.h"
#include "reset.h"
#include "nenchucmung.h"
#include "nenmain.h"
const int button = 36;
int buttonstate = HIGH;
const int button1 = 39;
int button1state = HIGH;
const int button2 = 34;     
int button2state = HIGH;
const int button3 = 35;
int button3state = HIGH;
const int button4 = 32;
int button4state = HIGH;
const int dongco_L = 14;
const int dongco_R = 12;
bool dongco_L_state;
bool dongco_R_state;
bool van_L_state;
bool van6_L_state;
bool van_R_state;
bool van6_R_state;
int chooseScreen1;
int chooseScreen2;
int chooseScreen2_2;
int currentScreen = 1; 
int chooseScreen3_1;
int chooseScreen3_2 = 1;
bool flag = false;
bool co = false;
bool co1 = false;
int flag1;
int flag2;
int flag3;
bool button_pressed = false;
bool button1_pressed = false;
bool button2_pressed = false;
bool button3_pressed = false;
bool button4_pressed = false;
int m = 00;
int s = 00;
int a = 0;
int last ;
int last2 ;
int buttonpress = 1;
int asd = 0;

typedef enum {
    ON,
    OFF
} TimeState;

struct TG {
    TimeState state;
};

struct PG {
    struct TG phutgiay[2];
    struct TG phutgiay2[4];
    struct TG timedefault[5];
};

struct PG thoigian;


typedef enum {
    ACTIVE,
    INACTIVE
} FingerState;

struct Finger {
    int num;
    FingerState state;
};

struct Hand {
    struct Finger fingers[6];
};

struct Human {
    struct Hand left_hand; 
    struct Hand right_hand; 
};

struct Human person;

void renderJPEG(int xpos, int ypos) {

  // retrieve infomration about the image
  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;

  uint32_t min_w = minimum(mcu_w, max_x % mcu_w);
  uint32_t min_h = minimum(mcu_h, max_y % mcu_h);

  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;

  // record the current time so we can measure how long it takes to draw an image
  uint32_t drawTime = millis();

  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;

  // read each MCU block until there are no more
  while (JpegDec.readSwappedBytes()) {
	  
    // save a pointer to the image block
    pImg = JpegDec.pImage ;

    // calculate where the image block should be drawn on the screen
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;  // Calculate coordinates of top left corner of current MCU
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    // check if the image block size needs to be changed for the right edge
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;

    // check if the image block size needs to be changed for the bottom edge
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;

    // copy pixels into a contiguous block
    if (win_w != mcu_w)
    {
      uint16_t *cImg;
      int p = 0;
      cImg = pImg + win_w;
      for (int h = 1; h < win_h; h++)
      {
        p += mcu_w;
        for (int w = 0; w < win_w; w++)
        {
          *cImg = *(pImg + w + p);
          cImg++;
        }
      }
    }

    // draw image MCU block only if it will fit on the screen
    if (( mcu_x + win_w ) <= tft.width() && ( mcu_y + win_h ) <= tft.height())
    {
      tft.pushRect(mcu_x, mcu_y, win_w, win_h, pImg);
    }
    else if ( (mcu_y + win_h) >= tft.height()) JpegDec.abort(); // Image has run off bottom of screen so abort decoding
  }

  // calculate how long it took to draw the image
  drawTime = millis() - drawTime;

  // print the results to the serial port
  // Serial.print(F(  "Total render time was    : ")); Serial.print(drawTime); Serial.println(F(" ms"));
  // Serial.println(F(""));
}
void drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos) {

  int x = xpos;
  int y = ypos;

  JpegDec.decodeArray(arrayname, array_size);
  
//  jpegInfo(); // Print information from the JPEG file (could comment this line out)
  
  renderJPEG(x, y);
  
  //Serial.println("#########################");
}

void IRAM_ATTR buttonCallback(){
  if(currentScreen == 5){
    if(millis()-last>=200){
      if (buttonpress == 0){
        buttonpress = 1;
        for(int i=0; i<=4; i++){
          if(person.left_hand.fingers[i].state == ACTIVE){
            dongco_L_state = !dongco_L_state;
            digitalWrite(dongco_L, dongco_L_state);
            Serial.println("stop dong co tay trai");
            break;
          }
        }
        for(int i=0; i<=4; i++){
          if(person.right_hand.fingers[i].state == ACTIVE){
            dongco_R_state = !dongco_R_state;
            digitalWrite(dongco_R, dongco_R_state);
            Serial.println("stop dong co tay phai");
            break;
          }
        }
        int downArrowX1 = 206;
        int downArrowY1 = 185;
        tft.fillCircle(197, 185, 20, TFT_ORANGE);
        tft.fillTriangle(downArrowX1 , downArrowY1 , downArrowX1 - 15, downArrowY1 - 7, downArrowX1 - 15, downArrowY1 + 7, TFT_WHITE);
      }
      else {
        buttonpress = 0;
        for(int i=0; i<=4; i++){
          if(person.left_hand.fingers[i].state == ACTIVE){
            dongco_L_state = !dongco_L_state;
            digitalWrite(dongco_L, dongco_L_state);
            Serial.println("chay dong co tay trai");
            break;
          }
        }
        for(int i=0; i<=4; i++){
          if(person.right_hand.fingers[i].state == ACTIVE){
            dongco_R_state = !dongco_R_state;
            digitalWrite(dongco_R, dongco_R_state);
            Serial.println("chay dong co tay phai");
            break;
          }
        }
        tft.fillCircle(197, 185, 20, TFT_ORANGE);
        tft.fillRoundRect(190, 176, 5, 20, 0, TFT_WHITE); 
        tft.fillRoundRect(200, 176, 5, 20, 0, TFT_WHITE); 
      }
      last = millis();
    }
  }
}

void manhinh()
{
  flag = false;
  drawArrayJpeg(anhnen, sizeof(anhnen), 0, 0); // Draw a jpeg image stored in memory at x,y
  
}
void manhinh_main(){
  drawArrayJpeg(nen_main, sizeof(nen_main), 0, 0); // Draw a jpeg image stored in memory at x,y

  if(chooseScreen1 == 1) 
    tft.fillRoundRect(18, 95, 142, 70, 10, 0x053b50);
  else
    tft.fillRoundRect(28, 105, 122, 50, 10, 0x053b50); 

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("DEFAULT", 88, 130);


  if(chooseScreen1 == 2) 
    tft.fillRoundRect(160, 95, 144, 70, 10, 0x053b50);
  else
     tft.fillRoundRect(170, 105, 124, 50, 10, 0x053b50);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("CUSTOM", 231, 130);

  int downArrowX = 10;
  int downArrowY = 20;
  tft.fillTriangle(downArrowX , downArrowY , downArrowX + 20, downArrowY - 12, downArrowX + 20, downArrowY + 12, 0x053b50);

  pcf8575.digitalWrite(P0, HIGH);
  pcf8575.digitalWrite(P1, HIGH);
  pcf8575.digitalWrite(P2, HIGH);
  pcf8575.digitalWrite(P3, HIGH);
  pcf8575.digitalWrite(P4, HIGH);
  pcf8575.digitalWrite(P5, HIGH);
  pcf8575.digitalWrite(P6, HIGH);

  pcf8575.digitalWrite(P9, HIGH);
  pcf8575.digitalWrite(P10, HIGH);
  pcf8575.digitalWrite(P11, HIGH);
  pcf8575.digitalWrite(P12, HIGH);
  pcf8575.digitalWrite(P13, HIGH);
  pcf8575.digitalWrite(P14, HIGH);
  pcf8575.digitalWrite(P15, HIGH);

  for(int i=0; i<=4; i++){
    person.left_hand.fingers[i].state = INACTIVE;
    person.right_hand.fingers[i].state = INACTIVE;
    thoigian.timedefault[i].state = OFF;
  }
  for(int i=0; i<=3; i++)
  {
    thoigian.phutgiay2[i].state = OFF;
  }
  for(int i=0; i<=1; i++)
  {
    thoigian.phutgiay[i].state = OFF;
  }
  m=00;
  s=00;
}

void manhinh_default(int chooseScreen2_2){
  if(!co1){
    for (int i = 0; i < 5; ++i) {
      thoigian.timedefault[i].state = OFF;
    }
    co1 = true;
  }
  drawArrayJpeg(nen_default, sizeof(nen_default), 0, 0); // Draw a jpeg image stored in memory at x,y

  if(thoigian.timedefault[0].state == ON && chooseScreen2_2 == 1)
    tft.fillRoundRect(5, 42.5, 105, 65, 7.5, TFT_ORANGE); // CHON O LAI
  if(thoigian.timedefault[0].state == OFF && chooseScreen2_2 == 1)
    tft.fillRoundRect(5, 42.5, 105, 65, 7.5, 0x053b50);  // DANG O DO
  if(thoigian.timedefault[0].state == ON && chooseScreen2_2 != 1)
    tft.fillRoundRect(12.5, 50, 90, 50, 7.5, TFT_ORANGE); // CHON ROI DI
  if(thoigian.timedefault[0].state == OFF && chooseScreen2_2 != 1)
    tft.fillRoundRect(12.5, 50, 90, 50, 7.5, 0x053b50); // DI ROI

  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("L-HAND", 57.5, 65);
  tft.drawString("10 mins", 57.5, 85);

  if(thoigian.timedefault[1].state == ON && chooseScreen2_2 == 2)
    tft.fillRoundRect(107.5, 42.5, 105, 65, 7.5, TFT_ORANGE);// CHON O LAI
  if(thoigian.timedefault[1].state == OFF && chooseScreen2_2 == 2)
    tft.fillRoundRect(107.5, 42.5, 105, 65, 7.5, 0x053b50); // DANG O DO
  if(thoigian.timedefault[1].state == ON && chooseScreen2_2 != 2)
    tft.fillRoundRect(115, 50, 90, 50, 7.5, TFT_ORANGE); // CHON ROI DI
  if(thoigian.timedefault[1].state == OFF && chooseScreen2_2 != 2)
    tft.fillRoundRect(115, 50, 90, 50, 7.5, 0x053b50); // DI ROI

  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("R-HAND", 160, 65);
  tft.drawString("10 mins", 160, 85);

  if(thoigian.timedefault[2].state == ON && chooseScreen2_2 == 3)
    tft.fillRoundRect(210, 42.5, 105, 65, 7.5, TFT_ORANGE); // CHON O LAI
  if(thoigian.timedefault[2].state == OFF && chooseScreen2_2 == 3)
    tft.fillRoundRect(210, 42.5, 105, 65, 7.5, 0x053b50); // DANG O DO
  if(thoigian.timedefault[2].state == ON && chooseScreen2_2 != 3)
    tft.fillRoundRect(217.5, 50, 90, 50, 7.5, TFT_ORANGE); // CHON ROI DI
  if(thoigian.timedefault[2].state == OFF && chooseScreen2_2 != 3)
    tft.fillRoundRect(217.5, 50, 90, 50, 7.5, 0x053b50); // DI ROI

  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("BOTH", 262.5, 65);
  tft.drawString("15 mins", 262.5, 85);

  if(thoigian.timedefault[3].state == ON && chooseScreen2_2 == 4)
    tft.fillRoundRect(2.5, 112.5, 157.5, 65, 7.5, TFT_ORANGE); // CHON O LAI
  if(thoigian.timedefault[3].state == OFF && chooseScreen2_2 == 4)
    tft.fillRoundRect(2.5, 112.5, 157.5, 65, 7.5, 0x053b50); // DANG O DO
  if(thoigian.timedefault[3].state == ON && chooseScreen2_2 != 4)
    tft.fillRoundRect(10, 120, 142.5, 50, 7.5, TFT_ORANGE); // CHON ROI DI
  if(thoigian.timedefault[3].state == OFF && chooseScreen2_2 != 4)
    tft.fillRoundRect(10, 120, 142.5, 50, 7.5, 0x053b50); // DI ROI

  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("FINGER(L)", 81.25, 135);
  tft.drawString("10 mins", 81.25, 155);

if(thoigian.timedefault[4].state == ON && chooseScreen2_2 == 5)
  tft.fillRoundRect(160, 112.5, 157.5, 65, 7.5, TFT_ORANGE); 
if(thoigian.timedefault[4].state == OFF && chooseScreen2_2 == 5)
  tft.fillRoundRect(160, 112.5, 157.5, 65, 7.5, 0x053b50); 
if(thoigian.timedefault[4].state == ON && chooseScreen2_2 != 5)
  tft.fillRoundRect(167.5, 120, 142.5, 50, 7.5, TFT_ORANGE); 
if(thoigian.timedefault[4].state == OFF && chooseScreen2_2 != 5)
  tft.fillRoundRect(167.5, 120, 142.5, 50, 7.5, 0x053b50); 

  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("FINGER(R)", 238.75, 135);
  tft.drawString("10 mins", 238.75, 155);

  int downArrowX1 = 171;
  int downArrowY1 = 215;
  tft.fillCircle(162, 215, 17, 0x053b50);
  tft.fillTriangle(downArrowX1 , downArrowY1 , downArrowX1 - 15, downArrowY1 - 7, downArrowX1 - 15, downArrowY1 + 7, TFT_WHITE);

  int downArrowX = 10;
  int downArrowY = 20;
  tft.fillTriangle(downArrowX , downArrowY , downArrowX + 20, downArrowY - 12, downArrowX + 20, downArrowY + 12, 0x053b50);
}

void manhinh_custom(int chooseScreen2)
{
  if(!flag){
    for (int i = 0; i < 6; ++i) {
      person.left_hand.fingers[i].state = INACTIVE;
      person.right_hand.fingers[i].state = INACTIVE;
        Serial.println("a");
    }
    flag = true;
  }

  drawArrayJpeg(nen_custom, sizeof(nen_custom), 0, 0); // Draw a jpeg image stored in memory at x,y
  int downArrowX = 10;
  int downArrowY = 20;
  tft.fillTriangle(downArrowX , downArrowY , downArrowX + 20, downArrowY - 12, downArrowX + 20, downArrowY + 12, 0x053b50);

  if(chooseScreen2 == 1) 
    tft.fillRoundRect(18, 95, 142, 70, 10, 0x053b50);
  else
    tft.fillRoundRect(28, 105, 122, 50, 10, 0x053b50); 

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("LEFT HAND", 88, 130);


    if(chooseScreen2 == 2) 
      tft.fillRoundRect(160, 95, 146, 70, 10, 0x053b50);
    else
      tft.fillRoundRect(170, 105, 126, 50, 10, 0x053b50);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("RIGHT HAND", 233, 130);


    if(chooseScreen2 == 3) 
      tft.fillRoundRect(90, 165, 140, 70, 10, 0x053b50);
    else
      tft.fillRoundRect(100, 175, 120, 50, 10, 0x053b50);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("TIME", 160, 200);
}

void manhinh_hand(int hand){
    printf("\nLeft hand:\n");
    for (int i = 0; i < 5; ++i) {
        if (person.left_hand.fingers[i].state == ACTIVE)
          printf("Finger %d: ACTIVE ",i + 1);
        else 
          printf("Finger %d: INACTIVE ",i + 1);
    }

    printf("\nRight hand:\n");
    for (int i = 0; i < 5; ++i) {
        if (person.right_hand.fingers[i].state == ACTIVE)
          printf("Finger %d: ACTIVE ",i + 1);
        else 
          printf("Finger %d: INACTIVE ",i + 1);
    }

  if(hand == 1)
    drawArrayJpeg(nentaytrai, sizeof(nentaytrai), 0, 0);
  else
    drawArrayJpeg(nentayphai, sizeof(nentayphai), 0, 0);
  
   if( hand == 1){
    if(person.left_hand.fingers[5].state == ACTIVE && chooseScreen3_1 == 6){
      tft.fillRoundRect(185, 170, 50, 45, 7.5, TFT_ORANGE); // chọn o do
      tft.fillRoundRect(80, 125, 40, 35, 7.5, TFT_ORANGE);
      tft.fillRoundRect(135, 125, 40, 35, 7.5, TFT_ORANGE);
      tft.fillRoundRect(80, 175, 40, 35, 7.5, TFT_ORANGE);
      tft.fillRoundRect(135, 175, 40, 35, 7.5, TFT_ORANGE);
      tft.fillRoundRect(190, 125, 40, 35, 7.5, TFT_ORANGE);
    }
    if(person.left_hand.fingers[5].state == ACTIVE && chooseScreen3_1 != 6){
      tft.fillRoundRect(190, 175, 40, 35, 7.5, TFT_ORANGE);//chon r di
    }
    if(person.left_hand.fingers[5].state == INACTIVE && chooseScreen3_1 == 6){
      if (person.left_hand.fingers[0].state == ACTIVE && person.left_hand.fingers[1].state == ACTIVE && person.left_hand.fingers[2].state == ACTIVE && person.left_hand.fingers[3].state == ACTIVE && person.left_hand.fingers[4].state == ACTIVE){
        tft.fillRoundRect(185, 170, 50, 45, 7.5, 0x053b50); //dang o do
        tft.fillRoundRect(80, 125, 40, 35, 7.5, 0x053b50);//di roi
        tft.fillRoundRect(135, 125, 40, 35, 7.5, 0x053b50);//di roi
        tft.fillRoundRect(80, 175, 40, 35, 7.5, 0x053b50);//di roi
        tft.fillRoundRect(135, 175, 40, 35, 7.5, 0x053b50);//di roi
        tft.fillRoundRect(190, 125, 40, 35, 7.5, 0x053b50);//di roi
      }else{
        tft.fillRoundRect(185, 170, 50, 45, 7.5, 0x053b50); //dang o do
      }
    }
    if(person.left_hand.fingers[5].state == INACTIVE && chooseScreen3_1 != 6){
      tft.fillRoundRect(190, 175, 40, 35, 7.5, 0x053b50);//di roi
    }
  }else{
    if(person.right_hand.fingers[5].state == ACTIVE && chooseScreen3_1 == 6){
      tft.fillRoundRect(185, 170, 50, 45, 7.5, TFT_ORANGE); // chọn o do
      tft.fillRoundRect(80, 125, 40, 35, 7.5, TFT_ORANGE);
      tft.fillRoundRect(135, 125, 40, 35, 7.5, TFT_ORANGE);
      tft.fillRoundRect(80, 175, 40, 35, 7.5, TFT_ORANGE);
      tft.fillRoundRect(135, 175, 40, 35, 7.5, TFT_ORANGE);
      tft.fillRoundRect(190, 125, 40, 35, 7.5, TFT_ORANGE);
    }
    if(person.right_hand.fingers[5].state == ACTIVE && chooseScreen3_1 != 6){
      tft.fillRoundRect(190, 175, 40, 35, 7.5, TFT_ORANGE);//chon r di
    }
    if(person.right_hand.fingers[5].state == INACTIVE && chooseScreen3_1 == 6){
      if (person.right_hand.fingers[0].state == ACTIVE && person.right_hand.fingers[1].state == ACTIVE && person.right_hand.fingers[2].state == ACTIVE && person.right_hand.fingers[3].state == ACTIVE && person.right_hand.fingers[4].state == ACTIVE){
        tft.fillRoundRect(185, 170, 50, 45, 7.5, 0x053b50); //dang o do
        tft.fillRoundRect(80, 125, 40, 35, 7.5, 0x053b50);//di roi
        tft.fillRoundRect(135, 125, 40, 35, 7.5, 0x053b50);//di roi
        tft.fillRoundRect(80, 175, 40, 35, 7.5, 0x053b50);//di roi
        tft.fillRoundRect(135, 175, 40, 35, 7.5, 0x053b50);//di roi
        tft.fillRoundRect(190, 125, 40, 35, 7.5, 0x053b50);//di roi
      }else{
        tft.fillRoundRect(185, 170, 50, 45, 7.5, 0x053b50); //dang o do
      }
    }
    if(person.right_hand.fingers[5].state == INACTIVE && chooseScreen3_1 != 6){
      tft.fillRoundRect(190, 175, 40, 35, 7.5, 0x053b50);//di roi
    }
  }

  if( hand == 1){
    if(person.left_hand.fingers[0].state == ACTIVE && chooseScreen3_1 == 1){
      tft.fillRoundRect(75, 120, 50, 45, 7.5, TFT_ORANGE);//CHON O LAI
    }
    if(person.left_hand.fingers[0].state == ACTIVE && chooseScreen3_1 != 1){
      tft.fillRoundRect(80, 125, 40, 35, 7.5, TFT_ORANGE);//chon r di
    }
    if(person.left_hand.fingers[0].state == INACTIVE && chooseScreen3_1 == 1){
      tft.fillRoundRect(75, 120, 50, 45, 7.5, 0x053b50);//DANG O DO
    }
    if(person.left_hand.fingers[0].state == INACTIVE && chooseScreen3_1 != 1){
      tft.fillRoundRect(80, 125, 40, 35, 7.5, 0x053b50);//di roi
    }
  }else{
    if(person.right_hand.fingers[0].state == ACTIVE && chooseScreen3_1 == 1){
      tft.fillRoundRect(75, 120, 50, 45, 7.5, TFT_ORANGE);//CHON O LAI
    }
    if(person.right_hand.fingers[0].state == ACTIVE && chooseScreen3_1 != 1){
      tft.fillRoundRect(80, 125, 40, 35, 7.5, TFT_ORANGE);//chon r di
    }
    if(person.right_hand.fingers[0].state == INACTIVE && chooseScreen3_1 == 1){
      tft.fillRoundRect(75, 120, 50, 45, 7.5, 0x053b50);//DANG O DO
    }
    if(person.right_hand.fingers[0].state == INACTIVE && chooseScreen3_1 != 1){
      tft.fillRoundRect(80, 125, 40, 35, 7.5, 0x053b50);//di roi
    }
  }

  if( hand == 1){
    if(person.left_hand.fingers[1].state == ACTIVE && chooseScreen3_1 == 2){
      tft.fillRoundRect(130, 120, 50, 45, 7.5, TFT_ORANGE);//CHON O LAI
    }
    if(person.left_hand.fingers[1].state == ACTIVE && chooseScreen3_1 != 2){
      tft.fillRoundRect(135, 125, 40, 35, 7.5, TFT_ORANGE);//chon r di
    }
    if(person.left_hand.fingers[1].state == INACTIVE && chooseScreen3_1 == 2){
      tft.fillRoundRect(130, 120, 50, 45, 7.5, 0x053b50); //dang o do
    }
    if(person.left_hand.fingers[1].state == INACTIVE && chooseScreen3_1 != 2){
      tft.fillRoundRect(135, 125, 40, 35, 7.5, 0x053b50);//di roi
    }
  }else{
    if(person.right_hand.fingers[1].state == ACTIVE && chooseScreen3_1 == 2){
      tft.fillRoundRect(130, 120, 50, 45, 7.5, TFT_ORANGE);//CHON O LAI
    }
    if(person.right_hand.fingers[1].state == ACTIVE && chooseScreen3_1 != 2){
      tft.fillRoundRect(135, 125, 40, 35, 7.5, TFT_ORANGE);//chon r di
    }
    if(person.right_hand.fingers[1].state == INACTIVE && chooseScreen3_1 == 2){
      tft.fillRoundRect(130, 120, 50, 45, 7.5, 0x053b50); //dang o do
    }
    if(person.right_hand.fingers[1].state == INACTIVE && chooseScreen3_1 != 2){
      tft.fillRoundRect(135, 125, 40, 35, 7.5, 0x053b50);//di roi
    }
  }

 if( hand == 1){
    if(person.left_hand.fingers[2].state == ACTIVE && chooseScreen3_1 == 3){
      tft.fillRoundRect(185, 120, 50, 45, 7.5, TFT_ORANGE); //chọn o lai
    }
    if(person.left_hand.fingers[2].state == ACTIVE && chooseScreen3_1 != 3){
      tft.fillRoundRect(190, 125, 40, 35, 7.5, TFT_ORANGE);//chon r di
    }
    if(person.left_hand.fingers[2].state == INACTIVE && chooseScreen3_1 == 3){
      tft.fillRoundRect(185, 120, 50, 45, 7.5, 0x053b50); //dang o do
    }
    if(person.left_hand.fingers[2].state == INACTIVE && chooseScreen3_1 != 3){
      tft.fillRoundRect(190, 125, 40, 35, 7.5, 0x053b50);//di roi
    }
  }else{
    if(person.right_hand.fingers[2].state == ACTIVE && chooseScreen3_1 == 3){
      tft.fillRoundRect(185, 120, 50, 45, 7.5, TFT_ORANGE); //chọn o lai
    }
    if(person.right_hand.fingers[2].state == ACTIVE && chooseScreen3_1 != 3){
      tft.fillRoundRect(190, 125, 40, 35, 7.5, TFT_ORANGE);//chon r di
    }
    if(person.right_hand.fingers[2].state == INACTIVE && chooseScreen3_1 == 3){
      tft.fillRoundRect(185, 120, 50, 45, 7.5, 0x053b50); //dang o do
    }
    if(person.right_hand.fingers[2].state == INACTIVE && chooseScreen3_1 != 3){
      tft.fillRoundRect(190, 125, 40, 35, 7.5, 0x053b50);//di roi
    }
  }

  if( hand == 1){
    if(person.left_hand.fingers[3].state == ACTIVE && chooseScreen3_1 == 4){
      tft.fillRoundRect(75, 170, 50, 45, 7.5, TFT_ORANGE); //CHON O DO
    }
    if(person.left_hand.fingers[3].state == ACTIVE && chooseScreen3_1 != 4){
      tft.fillRoundRect(80, 175, 40, 35, 7.5, TFT_ORANGE);//chon r di
    }
    if(person.left_hand.fingers[3].state == INACTIVE && chooseScreen3_1 == 4){
      tft.fillRoundRect(75, 170, 50, 45, 7.5, 0x053b50); //DANG O DO
    }
    if(person.left_hand.fingers[3].state == INACTIVE && chooseScreen3_1 != 4){
      tft.fillRoundRect(80, 175, 40, 35, 7.5, 0x053b50);//di roi
    }
  }else{
    if(person.right_hand.fingers[3].state == ACTIVE && chooseScreen3_1 == 4){
      tft.fillRoundRect(75, 170, 50, 45, 7.5, TFT_ORANGE); //CHON O DO
    }
    if(person.right_hand.fingers[3].state == ACTIVE && chooseScreen3_1 != 4){
      tft.fillRoundRect(80, 175, 40, 35, 7.5, TFT_ORANGE);//chon r di
    }
    if(person.right_hand.fingers[3].state == INACTIVE && chooseScreen3_1 == 4){
      tft.fillRoundRect(75, 170, 50, 45, 7.5, 0x053b50); //DANG O DO
    }
    if(person.right_hand.fingers[3].state == INACTIVE && chooseScreen3_1 != 4){
      tft.fillRoundRect(80, 175, 40, 35, 7.5, 0x053b50);//di roi
    }
  }

  if( hand == 1){
    if(person.left_hand.fingers[4].state == ACTIVE && chooseScreen3_1 == 5){
      tft.fillRoundRect(130, 170, 50, 45, 7.5, TFT_ORANGE); //chọn o do
    }
    if(person.left_hand.fingers[4].state == ACTIVE && chooseScreen3_1 != 5){
      tft.fillRoundRect(135, 175, 40, 35, 7.5, TFT_ORANGE);//chon r di
    }
    if(person.left_hand.fingers[4].state == INACTIVE && chooseScreen3_1 == 5){
      tft.fillRoundRect(130, 170, 50, 45, 7.5, 0x053b50); //dang o do
    }
    if(person.left_hand.fingers[4].state == INACTIVE && chooseScreen3_1 != 5){
      tft.fillRoundRect(135, 175, 40, 35, 7.5, 0x053b50);//di roi
    }
  }else{
    if(person.right_hand.fingers[4].state == ACTIVE && chooseScreen3_1 == 5){
      tft.fillRoundRect(130, 170, 50, 45, 7.5, TFT_ORANGE); //chọn o do
    }
    if(person.right_hand.fingers[4].state == ACTIVE && chooseScreen3_1 != 5){
      tft.fillRoundRect(135, 175, 40, 35, 7.5, TFT_ORANGE);//chon r di
    }
    if(person.right_hand.fingers[4].state == INACTIVE && chooseScreen3_1 == 5){
      tft.fillRoundRect(130, 170, 50, 45, 7.5, 0x053b50); //dang o do
    }
    if(person.right_hand.fingers[4].state == INACTIVE && chooseScreen3_1 != 5){
      tft.fillRoundRect(135, 175, 40, 35, 7.5, 0x053b50);//di roi
    }
  }
 
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2.5);
  tft.setTextDatum(MC_DATUM);  
  tft.drawString("ALL", 210, 192.5);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2.5);
  tft.setTextDatum(MC_DATUM);  
  tft.drawString("1", 100, 142.5);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2.5);
  tft.setTextDatum(MC_DATUM);  
  tft.drawString("2", 155, 142.5);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2.5);
  tft.setTextDatum(MC_DATUM);  
  tft.drawString("3", 210, 142.5);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2.5);
  tft.setTextDatum(MC_DATUM);  
  tft.drawString("4", 100, 192.5);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2.5);
  tft.setTextDatum(MC_DATUM);  
  tft.drawString("5", 155, 192.5);

  int downArrowX1 = 291;
  int downArrowY1 = 215;
  tft.fillTriangle(downArrowX1 , downArrowY1 , downArrowX1 - 18, downArrowY1 - 10, downArrowX1 - 18, downArrowY1 + 10, 0x053b50);

}

void manhinh_time() {
  if(!co){
    for (int i = 0; i < 2; ++i) {
      thoigian.phutgiay[i].state = OFF;
        Serial.println("b");
    }
    for (int i = 0; i < 4; ++i){
      thoigian.phutgiay2[i].state = OFF;
        Serial.println("c");
    }
    co = true;
  }

    printf("\nMIN SEC:\n");
    for (int i = 0; i < 2; ++i) {
        Serial.println(thoigian.phutgiay[i].state == ON ? "ON" : "OFF");
    }
    printf("\nMIN SEC2:\n");
    for (int i = 0; i < 4; ++i) {
        Serial.println(thoigian.phutgiay2[i].state == ON ? "ON" : "OFF");
    }

  drawArrayJpeg(nentgtap, sizeof(nentgtap), 0, 0); // Draw a jpeg image stored in memory at x,y
  int downArrowX = 10;
  int downArrowY = 20;
  tft.fillTriangle(downArrowX , downArrowY , downArrowX + 20, downArrowY - 12, downArrowX + 20, downArrowY + 12, 0x053b50);

  if (thoigian.phutgiay[0].state == ON && chooseScreen3_2 == 1)   
    tft.fillRoundRect(80, 45, 70, 45, 7.5, TFT_ORANGE); // CHON O LAI
  if (thoigian.phutgiay[0].state == ON && chooseScreen3_2 != 1)
    tft.fillRoundRect(85, 50, 60, 35, 7.5, TFT_ORANGE); // CHON ROI DI 
  if (thoigian.phutgiay[0].state == OFF && chooseScreen3_2 == 1)
    tft.fillRoundRect(80, 45, 70, 45, 7.5, 0x053b50); // DANG O DO
  if (thoigian.phutgiay[0].state == OFF && chooseScreen3_2 != 1)
    tft.fillRoundRect(85, 50, 60, 35, 7.5, 0x053b50); // DI ROI

  tft.setTextColor(0x053b50);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);  
  tft.drawString("(mins)", 115, 102.5); 

  //   tft.fillRoundRect(80, 45, 70, 45, 7.5, 0x053b50); 
    tft.setCursor(104,60);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM); 
    tft.print(m/10); 
    tft.print(m%10); 

  tft.setTextColor(0x053b50);
  tft.setTextSize(4);
  tft.setTextDatum(MC_DATUM);  
  tft.drawString(":", 162, 70); 

  if (thoigian.phutgiay[1].state == ON && chooseScreen3_2 == 2)
    tft.fillRoundRect(170, 45, 70, 45, 7.5, TFT_ORANGE); // CHON O LAI  
  if (thoigian.phutgiay[1].state == ON && chooseScreen3_2 != 2)
    tft.fillRoundRect(175, 50, 60, 35, 7.5, TFT_ORANGE); // CHON ROI DI
  if (thoigian.phutgiay[1].state == OFF && chooseScreen3_2 == 2)
    tft.fillRoundRect(170, 45, 70, 45, 7.5, 0x053b50); // DANG O DO  
  if (thoigian.phutgiay[1].state == OFF && chooseScreen3_2 != 2)
    tft.fillRoundRect(175, 50, 60, 35, 7.5, 0x053b50); // DI ROI

  tft.setTextColor(0x053b50);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);  
  tft.drawString("(secs)", 205, 102.5); 

    tft.setCursor(194,60);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM); 
    tft.print(s/10); 
    tft.print(s%10); 

  if (chooseScreen3_2 == 3)
    tft.drawBitmap(250,55,reset,30,23,TFT_RED );//CHON ROI
  else
    tft.drawBitmap(250,55,reset,30,23,TFT_BLACK );// CHUA CHON

  if(thoigian.phutgiay2[0].state == ON && chooseScreen3_2 == 4)
    tft.fillRoundRect(15, 125, 70, 55, 7.5, TFT_ORANGE); //CHON O LAI
  if(thoigian.phutgiay2[0].state == ON && chooseScreen3_2 != 4)
    tft.fillRoundRect(20, 130, 60, 45, 7.5, TFT_ORANGE); //CHON ROI DI
  if(thoigian.phutgiay2[0].state == OFF && chooseScreen3_2 == 4)
    tft.fillRoundRect(15, 125, 70, 55, 7.5, 0x053b50); //DANG O DO
  if(thoigian.phutgiay2[0].state == OFF && chooseScreen3_2 != 4)
    tft.fillRoundRect(20, 130, 60, 45, 7.5, 0x053b50); // DI ROI

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);  
  tft.drawString("1", 50, 143.5); 
  tft.drawString("mins", 50, 161.5); 

  if(thoigian.phutgiay2[1].state == ON && chooseScreen3_2 == 5)
    tft.fillRoundRect(90, 125, 70, 55, 7.5, TFT_ORANGE); //CHON O LAI
  if(thoigian.phutgiay2[1].state == ON && chooseScreen3_2 != 5)
    tft.fillRoundRect(95, 130, 60, 45, 7.5, TFT_ORANGE); //CHON ROI DI
  if(thoigian.phutgiay2[1].state == OFF && chooseScreen3_2 == 5)
    tft.fillRoundRect(90, 125, 70, 55, 7.5, 0x053b50); // DANG O DO
  if(thoigian.phutgiay2[1].state == OFF && chooseScreen3_2 != 5)
    tft.fillRoundRect(95, 130, 60, 45, 7.5, 0x053b50); // DI ROI

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);  
  tft.drawString("5", 125, 143.5); 
  tft.drawString("mins", 125, 161.5); 

  if(thoigian.phutgiay2[2].state == ON && chooseScreen3_2 == 6)
    tft.fillRoundRect(165, 125, 70, 55, 7.5, TFT_ORANGE); //CHON O LAI
  if(thoigian.phutgiay2[2].state == ON && chooseScreen3_2 != 6)
    tft.fillRoundRect(170, 130, 60, 45, 7.5, TFT_ORANGE); //CHON ROI DI
  if(thoigian.phutgiay2[2].state == OFF && chooseScreen3_2 == 6)
    tft.fillRoundRect(165, 125, 70, 55, 7.5, 0x053b50); //DANG O DO
  if(thoigian.phutgiay2[2].state == OFF && chooseScreen3_2 != 6)
    tft.fillRoundRect(170, 130, 60, 45, 7.5, 0x053b50); // DI ROI

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);  
  tft.drawString("15", 200, 143.5); 
  tft.drawString("mins", 200, 161.5); 

  if(thoigian.phutgiay2[3].state == ON && chooseScreen3_2 == 7)
    tft.fillRoundRect(240, 125, 70, 55, 7.5, TFT_ORANGE); //CHON O LAI
  if(thoigian.phutgiay2[3].state == ON && chooseScreen3_2 != 7)
    tft.fillRoundRect(245, 130, 60, 45, 7.5, TFT_ORANGE); // CHON ROI DI
  if(thoigian.phutgiay2[3].state == OFF && chooseScreen3_2 == 7)
    tft.fillRoundRect(240, 125, 70, 55, 7.5, 0x053b50); // DANG O DO
  if(thoigian.phutgiay2[3].state == OFF && chooseScreen3_2 != 7)
    tft.fillRoundRect(245, 130, 60, 45, 7.5, 0x053b50); //DI ROI

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);  
  tft.drawString("30", 275, 143.5); 
  tft.drawString("mins", 275, 161.5); 

  int downArrowX1 = 171;
  int downArrowY1 = 215;
  tft.fillCircle(162, 215, 17, 0x053b50);
  tft.fillTriangle(downArrowX1 , downArrowY1 , downArrowX1 - 15, downArrowY1 - 7, downArrowX1 - 15, downArrowY1 + 7, TFT_WHITE);

}

void manhinh_timing(){
  drawArrayJpeg(nendangtap, sizeof(nendangtap), 0, 0); // Draw a jpeg image stored in memory at x,y
  tft.fillRoundRect(90, 60, 60, 35, 7.5, 0x053b50); 
  tft.setTextColor(0x053b50);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);  
  tft.drawString("(mins)", 115, 107.5); 

  tft.fillRoundRect(170, 60, 60, 35, 7.5, 0x053b50); 
  tft.setTextColor(0x053b50);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);  
  tft.drawString("(secs)", 205, 107.5); 

        tft.fillCircle(197, 185, 20, TFT_ORANGE);
        tft.fillRoundRect(190, 176, 5, 20, 0, TFT_WHITE); 
        tft.fillRoundRect(200, 176, 5, 20, 0, TFT_WHITE); 

  tft.setTextColor(0x053b50);
  tft.setTextSize(4);
  tft.setTextDatum(MC_DATUM);  
  tft.drawString(":", 162, 79); 

  int downArrowX = 95;
  int downArrowY = 185;
  tft.fillTriangle(downArrowX , downArrowY , downArrowX + 25, downArrowY - 15, downArrowX + 25, downArrowY + 15, 0x053b50);

}

void manhinh_congra(){
  drawArrayJpeg(nenchucmung, sizeof(nenchucmung), 0, 0); // Draw a jpeg image stored in memory at x,y

  pcf8575.digitalWrite(P0, HIGH);
  pcf8575.digitalWrite(P1, HIGH);
  pcf8575.digitalWrite(P2, HIGH);
  pcf8575.digitalWrite(P3, HIGH);
  pcf8575.digitalWrite(P4, HIGH);
  pcf8575.digitalWrite(P5, HIGH);
  pcf8575.digitalWrite(P6, HIGH);

  pcf8575.digitalWrite(P9, HIGH);
  pcf8575.digitalWrite(P10, HIGH);
  pcf8575.digitalWrite(P11, HIGH);
  pcf8575.digitalWrite(P12, HIGH);
  pcf8575.digitalWrite(P13, HIGH);
  pcf8575.digitalWrite(P14, HIGH);
  pcf8575.digitalWrite(P15, HIGH);

for(int i=0; i<=4; i++){
  person.left_hand.fingers[i].state = INACTIVE;
  person.right_hand.fingers[i].state = INACTIVE;
  thoigian.timedefault[i].state = OFF;
}
for(int i=0; i<=3; i++)
{
  thoigian.phutgiay2[i].state = OFF;
}
for(int i=0; i<=1; i++)
{
  thoigian.phutgiay[i].state = OFF;
}

  int downArrowX = 10;
  int downArrowY = 20;
  tft.fillTriangle(downArrowX , downArrowY , downArrowX + 20, downArrowY - 12, downArrowX + 20, downArrowY + 12, 0x053b50);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

    pcf8575.begin();
    pcf8575.pinMode(P0, OUTPUT);
    pcf8575.pinMode(P1, OUTPUT);
    pcf8575.pinMode(P2, OUTPUT);
    pcf8575.pinMode(P3, OUTPUT);
    pcf8575.pinMode(P4, OUTPUT);
    pcf8575.pinMode(P5, OUTPUT);
    pcf8575.pinMode(P6, OUTPUT);
    pcf8575.pinMode(P7, OUTPUT);

    pcf8575.pinMode(P8, OUTPUT);
    pcf8575.pinMode(P9, OUTPUT);
    pcf8575.pinMode(P10, OUTPUT);
    pcf8575.pinMode(P11, OUTPUT);
    pcf8575.pinMode(P12, OUTPUT);
    pcf8575.pinMode(P13, OUTPUT);
    pcf8575.pinMode(P14, OUTPUT);
    pcf8575.pinMode(P15, OUTPUT);

  pinMode(button, INPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  pinMode(dongco_L, OUTPUT);
  pinMode(dongco_R, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(button), buttonCallback,FALLING);

  tft.init();
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);
  manhinh();
}
void loop (){

  buttonstate = digitalRead(button);//enter
  button3state = digitalRead(button3);//back
  button1state = digitalRead(button1);//left
  button2state = digitalRead(button2);//right
  button4state = digitalRead(button4);//next
  // enter vao manhinh_main
  if(buttonstate == 0 && button_pressed == false && currentScreen == 1)
  {
    chooseScreen1=1;
    currentScreen++;
    delay(100);
    manhinh_main();
    button_pressed = true;
  }
  if(buttonstate == 1){
    button_pressed = false;
  }
  // manhinh_main (qua phai)
  if(currentScreen == 2 && button2state ==0 && button2_pressed == false){
    chooseScreen1++;
    if(chooseScreen1 == 3 )
      chooseScreen1 = 1;
    delay(100);   
    manhinh_main();
    button2_pressed = true;
  }
  if(button2state == 1){
    button2_pressed = false;
  }
  // manhinh_main (qua trai)
  if(currentScreen == 2 && button1state ==0 && button1_pressed == false){
    chooseScreen1--;
    if(chooseScreen1 == 0)
      chooseScreen1 = 2;
    delay(100); 
    manhinh_main();
    button1_pressed = true;
  }
  if(button1state == 1){
    button1_pressed = false;
  }
//enter vao default
if(buttonstate == 0 && button_pressed == false && currentScreen == 2 && chooseScreen1 == 1)
  {
    Serial.println(chooseScreen2_2); 
    Serial.println(currentScreen);
      chooseScreen2_2=1;
      flag1 = 1;
      flag2 = 1;
      flag3 = 2;
      currentScreen = currentScreen + 2;
      delay(100);
      manhinh_default(chooseScreen2_2);
    button_pressed = true;
  }
  if(buttonstate == 1){
    button_pressed = false;
  }

//man hinh default
  // qua phai
  if(currentScreen == 4 && button2state ==0 && chooseScreen1 == 1 && flag3 == 2 && button2_pressed == false){
    chooseScreen2_2++;
    if(chooseScreen2_2 == 6 )
      chooseScreen2_2 = 1;
    Serial.println(chooseScreen2_2);
    delay(100);   
    manhinh_default(chooseScreen2_2);
    button2_pressed = true;
  }
  if(button2state == 1){
    button2_pressed = false;
  }
  // qua trai
  if(currentScreen == 4 && button1state ==0 && chooseScreen1 == 1 && flag3 == 2 && button1_pressed == false){
    chooseScreen2_2--;
    if(chooseScreen2_2 == 0)
      chooseScreen2_2 = 5;
    Serial.println(chooseScreen2_2);
     delay(100); 
    manhinh_default(chooseScreen2_2);
    button1_pressed = true;
  }
  if(button1state == 1){
    button1_pressed = false;
  }
// chon chuc nang man hinh default
  if(currentScreen == 4 && chooseScreen2_2 >= 1 && chooseScreen2_2 <=5 && buttonstate == 0 && button_pressed == false && flag3 == 2){
      if(chooseScreen2_2==1){
        if(thoigian.timedefault[0].state == ON){
          thoigian.timedefault[0].state = OFF;
          m=00;
          s=00;
        }
        else{
          thoigian.timedefault[0].state = ON;
          thoigian.timedefault[1].state = OFF;
          thoigian.timedefault[2].state = OFF;
          thoigian.timedefault[3].state = OFF;
          thoigian.timedefault[4].state = OFF;
          person.left_hand.fingers[0].state = ACTIVE;
          person.left_hand.fingers[1].state = ACTIVE;
          person.left_hand.fingers[2].state = ACTIVE;
          person.left_hand.fingers[3].state = ACTIVE;
          person.left_hand.fingers[4].state = ACTIVE;
          m=10;
          s=00;
        }
      }
      if (chooseScreen2_2==2){
        if(thoigian.timedefault[1].state == ON){
          thoigian.timedefault[1].state = OFF;
          m=00;
          s=00; 
        }
        else{
          thoigian.timedefault[0].state = OFF;
          thoigian.timedefault[1].state = ON;
          thoigian.timedefault[2].state = OFF;
          thoigian.timedefault[3].state = OFF;
          thoigian.timedefault[4].state = OFF;
          person.right_hand.fingers[0].state = ACTIVE;
          person.right_hand.fingers[1].state = ACTIVE;
          person.right_hand.fingers[2].state = ACTIVE;
          person.right_hand.fingers[3].state = ACTIVE;
          person.right_hand.fingers[4].state = ACTIVE;
          m=10;
          s=00; 
        }
      }
      if (chooseScreen2_2==3){
        if(thoigian.timedefault[2].state == ON){
          thoigian.timedefault[2].state = OFF;
          m=00;
          s=00;
        }
        else{
          thoigian.timedefault[0].state = OFF;
          thoigian.timedefault[1].state = OFF;
          thoigian.timedefault[2].state = ON;
          thoigian.timedefault[3].state = OFF;
          thoigian.timedefault[4].state = OFF;

          person.left_hand.fingers[0].state = ACTIVE;
          person.left_hand.fingers[1].state = ACTIVE;
          person.left_hand.fingers[2].state = ACTIVE;
          person.left_hand.fingers[3].state = ACTIVE;
          person.left_hand.fingers[4].state = ACTIVE;

          person.right_hand.fingers[0].state = ACTIVE;
          person.right_hand.fingers[1].state = ACTIVE;
          person.right_hand.fingers[2].state = ACTIVE;
          person.right_hand.fingers[3].state = ACTIVE;
          person.right_hand.fingers[4].state = ACTIVE;

          m=15;
          s=00; 
        }
      }
      if(chooseScreen2_2==4){
        if(thoigian.timedefault[3].state == ON){
          thoigian.timedefault[3].state = OFF;
          m=00;
          s=00; 
        }
        else{
          thoigian.timedefault[0].state = OFF;
          thoigian.timedefault[1].state = OFF;
          thoigian.timedefault[2].state = OFF;
          thoigian.timedefault[3].state = ON;
          thoigian.timedefault[4].state = OFF;
          m=10;
          s=00; 
        }
      }
      if(chooseScreen2_2==5){
        if(thoigian.timedefault[4].state == ON){
          thoigian.timedefault[4].state = OFF;
          m=00;
          s=00; 
        }
        else{
          thoigian.timedefault[0].state = OFF;
          thoigian.timedefault[1].state = OFF;
          thoigian.timedefault[2].state = OFF;
          thoigian.timedefault[3].state = OFF;
          thoigian.timedefault[4].state = ON;
          m=10;
          s=00; 
        }
      }
      delay(100);
      manhinh_default(chooseScreen2_2);
      button_pressed = true;
  }
  if(buttonstate == 1){
    button_pressed = false;
  }
//enter custom
  if(buttonstate == 0 && button_pressed == false && currentScreen == 2 && chooseScreen1 == 2)
  {
    Serial.println(chooseScreen2); 
    Serial.println(currentScreen);
      chooseScreen2=1;
      currentScreen++;
      delay(100);
      manhinh_custom(chooseScreen2);
    button_pressed = true;
  }
  if(buttonstate == 1){
    button_pressed = false;
  }
  //man hinh custom
    // qua phai
  if(currentScreen == 3 && button2state ==0 && chooseScreen1 == 2 && button2_pressed == false){
    chooseScreen2++;
    if(chooseScreen2 == 4 )
      chooseScreen2 = 1;
    Serial.println(chooseScreen2);
    delay(100);   
    manhinh_custom(chooseScreen2);
    button2_pressed = true;
  }
  if(button2state == 1){
    button2_pressed = false;
  }
    // qua trai
  if(currentScreen == 3 && button1state ==0 && chooseScreen1 == 2 && button1_pressed == false){
    chooseScreen2--;
    if(chooseScreen2 == 0)
      chooseScreen2 = 3;
    Serial.println(chooseScreen2);
     delay(100); 
    manhinh_custom(chooseScreen2);
    button1_pressed = true;
  }
  if(button1state == 1){
    button1_pressed = false;
  }
  // vao man hinh tay phai, tay trai (manhinh_hand)
  if(buttonstate == 0 && button_pressed == false && currentScreen == 3 && (chooseScreen2 == 1 || chooseScreen2 == 2)){
      currentScreen++;
      chooseScreen3_1=1;
      delay(100);
      manhinh_hand(chooseScreen2);
      flag1 = 2;
      flag2 = 1;
      flag3 = 1;
    button_pressed = true;
  }
  if(buttonstate == 1){
    button_pressed = false;
  }
  // vao man hinh tg tap(manhinh_time)
  if(currentScreen == 3 && chooseScreen2 == 3 && buttonstate == 0 && button_pressed == false){
    currentScreen++;
    delay(100);
    manhinh_time();
    flag2 = 2;
    flag1 = 1;
    flag3 = 1;
    button_pressed = true;
  }  
  if(buttonstate == 1){
    button_pressed = false;
  }

//man hinh tay phai, tay trai (manhinh_hand)
  if(currentScreen == 4 && button2state ==0 && flag1 == 2 && button2_pressed == false){//phai
    chooseScreen3_1++;
    if(chooseScreen3_1 == 7 ){
      chooseScreen3_1 = 1;
    }
    Serial.println(chooseScreen3_1); 
     delay(100); 
    manhinh_hand(chooseScreen2);
    button2_pressed = true;
  }
  if(button2state == 1){
    button2_pressed = false;
  }

  if(currentScreen == 4 && button1state ==0 && flag1 == 2 && button1_pressed == false){//TRAI
    chooseScreen3_1--;
    if(chooseScreen3_1 == 0)
      chooseScreen3_1 = 6;
    Serial.println(chooseScreen3_1);
     delay(100); 
    manhinh_hand(chooseScreen2);
    button1_pressed = true;
  }
  if(button1state == 1){
    button1_pressed = false;
  }
  // chon chuc nang man hinh hand
  if(currentScreen == 4 && chooseScreen3_1 >= 1 && chooseScreen3_1 <=6 && buttonstate == 0 && button_pressed == false && flag1 == 2){
      if(chooseScreen2 == 1){  
        if(chooseScreen3_1 <=5){
          if(person.left_hand.fingers[chooseScreen3_1-1].state == ACTIVE)
            person.left_hand.fingers[chooseScreen3_1-1].state = INACTIVE;
          else
            person.left_hand.fingers[chooseScreen3_1-1].state = ACTIVE;
        }
        if(chooseScreen3_1 == 6) {
            if (person.left_hand.fingers[5].state == ACTIVE){
              person.left_hand.fingers[5].state = INACTIVE;
              person.left_hand.fingers[0].state = INACTIVE;
              person.left_hand.fingers[1].state = INACTIVE;
              person.left_hand.fingers[2].state = INACTIVE;
              person.left_hand.fingers[3].state = INACTIVE;
              person.left_hand.fingers[4].state = INACTIVE;
            }
            else{
              person.left_hand.fingers[5].state = ACTIVE;
              person.left_hand.fingers[0].state = ACTIVE;
              person.left_hand.fingers[1].state = ACTIVE;
              person.left_hand.fingers[2].state = ACTIVE;
              person.left_hand.fingers[3].state = ACTIVE;
              person.left_hand.fingers[4].state = ACTIVE;
            }
        }
        button_pressed = true;
      }

      if(chooseScreen2 == 2 ){
        if(chooseScreen3_1 <=5){
          if(person.right_hand.fingers[chooseScreen3_1-1].state == ACTIVE)
            person.right_hand.fingers[chooseScreen3_1-1].state = INACTIVE; 
          else
            person.right_hand.fingers[chooseScreen3_1-1].state = ACTIVE;
        }
        if(chooseScreen3_1 == 6) {
            if (person.right_hand.fingers[5].state == ACTIVE){
              person.right_hand.fingers[5].state = INACTIVE;
              person.right_hand.fingers[0].state = INACTIVE;
              person.right_hand.fingers[1].state = INACTIVE;
              person.right_hand.fingers[2].state = INACTIVE;
              person.right_hand.fingers[3].state = INACTIVE;
              person.right_hand.fingers[4].state = INACTIVE;
            }
            else{
              person.right_hand.fingers[5].state = ACTIVE;
              person.right_hand.fingers[0].state = ACTIVE;
              person.right_hand.fingers[1].state = ACTIVE;
              person.right_hand.fingers[2].state = ACTIVE;
              person.right_hand.fingers[3].state = ACTIVE;
              person.right_hand.fingers[4].state = ACTIVE;
            }
        }
        button_pressed = true;
      }
      delay(100);
       manhinh_hand(chooseScreen2);
       button_pressed = true;
  }
  if(buttonstate == 1){
    button_pressed = false;
  }
// manhinh_time
if(currentScreen == 4 && button2state ==0 && flag2 == 2 && a == 0 && button2_pressed == false){//phai
    chooseScreen3_2++;
    if(chooseScreen3_2 == 8 ){
      chooseScreen3_2 = 1;
    }
     delay(100); 
    manhinh_time();
    button2_pressed = true;
}
if(button2state == 1){
    button2_pressed = false;
}

if(currentScreen == 4 && button1state ==0 && flag2 == 2 && a == 0 && button1_pressed == false){//TRAI
    chooseScreen3_2--;
    if(chooseScreen3_2 == 0)
      chooseScreen3_2 = 7;
     delay(100); 
    manhinh_time();
    button1_pressed = true;
}
if(button1state == 1){
  button1_pressed = false;
}
// chon chuc nang man hinh time
  if(currentScreen == 4 && chooseScreen3_2 >= 1 && chooseScreen3_2 <=7 && buttonstate == 0 && button_pressed == false && flag2 == 2){
    if (chooseScreen3_2==3){
      m = 00;
      s = 00;
      thoigian.phutgiay2[0].state = OFF;
      thoigian.phutgiay2[1].state = OFF;
      thoigian.phutgiay2[2].state = OFF;
      thoigian.phutgiay2[3].state = OFF;
    }
    if (chooseScreen3_2 <=2){
      if(thoigian.phutgiay[chooseScreen3_2-1].state == ON){
        thoigian.phutgiay[chooseScreen3_2-1].state = OFF;
        a = 0;
      }
      else{
        thoigian.phutgiay[chooseScreen3_2-1].state = ON;
        thoigian.phutgiay2[0].state = OFF;
        thoigian.phutgiay2[1].state = OFF;
        thoigian.phutgiay2[2].state = OFF;
        thoigian.phutgiay2[3].state = OFF;
        a = 1;
      }
    }
    if (chooseScreen3_2 >=4){
      if(chooseScreen3_2==4){
        if(thoigian.phutgiay2[0].state == ON){
          thoigian.phutgiay2[0].state = OFF;
          m=00;
          s=00;
        }
        else{
          thoigian.phutgiay2[0].state = ON;
          thoigian.phutgiay2[1].state = OFF;
          thoigian.phutgiay2[2].state = OFF;
          thoigian.phutgiay2[3].state = OFF;
          m=01;
          s=00;
        }
      }
      if (chooseScreen3_2==5){
        if(thoigian.phutgiay2[1].state == ON){
          thoigian.phutgiay2[1].state = OFF;
          m=00;
          s=00; 
        }
        else{
          thoigian.phutgiay2[0].state = OFF;
          thoigian.phutgiay2[1].state = ON;
          thoigian.phutgiay2[2].state = OFF;
          thoigian.phutgiay2[3].state = OFF;
          m=05;
          s=00; 
        }
      }
      if (chooseScreen3_2==6){
        if(thoigian.phutgiay2[2].state == ON){
          thoigian.phutgiay2[2].state = OFF;
          m=00;
          s=00;
        }
        else{
          thoigian.phutgiay2[0].state = OFF;
          thoigian.phutgiay2[1].state = OFF;
          thoigian.phutgiay2[2].state = ON;
          thoigian.phutgiay2[3].state = OFF;
          m=15;
          s=00; 
        }
      }
      if(chooseScreen3_2==7){
        if(thoigian.phutgiay2[3].state == ON){
          thoigian.phutgiay2[3].state = OFF;
          m=00;
          s=00; 
        }
        else{
          thoigian.phutgiay2[0].state = OFF;
          thoigian.phutgiay2[1].state = OFF;
          thoigian.phutgiay2[2].state = OFF;
          thoigian.phutgiay2[3].state = ON;
          m=30;
          s=00; 
        }
      }
    }
    delay(100);
    manhinh_time();
    button_pressed = true;
  }
  if(buttonstate == 1){
    button_pressed = false;
  }
  
  if(currentScreen == 4 && button2state ==0 && flag2 == 2 && a == 1){//phai tăng
    if(chooseScreen3_2 == 1){
      m++;
      if(m == 60 )
        s = 00;
      if(m>60)
        m=00;
    }
    if(chooseScreen3_2 == 2){
      s++;
      if(s == 60 ){
        s = 00;
        m++;
      }
      if (m >=60){
        m = 60;
        s = 00;
      }
    }
    delay(100); 
    manhinh_time();
  }

  if(currentScreen == 4 && button1state ==0 && flag2 == 2 && a == 1){//TRAI giảm
    if (chooseScreen3_2 == 1){
      m--;
      if(m <= 00 )
        m = 60;
    }
    if (chooseScreen3_2 == 2){
      s--;
      if(s <= 00 ){
        s = 59;
        m--;
      }
      if (m<=00)
        m = 00;
    }
     delay(100); 
    manhinh_time();
  }

  //back
  if(button3state == 0){
    switch (currentScreen) {
      case 2:
        delay(100); 
        manhinh();
        currentScreen--;
        break;
      case 3:
        delay(100);
        manhinh_main();
        currentScreen--;
        break;
      default:
        break;
    }
  }
  // back ve man hinh custom
  if(button3state == 0 && currentScreen == 4 && flag2 == 2){
      delay(100);
      chooseScreen2 = 1;
      chooseScreen3_1 = 1;
      chooseScreen3_2 = 1;
      manhinh_custom(chooseScreen2);
      currentScreen--;
  }
  // back ve man hinh main
  if(button3state == 0 && currentScreen == 4 && flag3 == 2){
      delay(100);
      chooseScreen2_2 = 1;
      manhinh_main();
      currentScreen = currentScreen - 2;;
  }
  // back ve man hinh time tu man hinh dang tap
  if(button3state == 0 && currentScreen == 5 && flag2 == 2){
      for(int i=0; i<=3; i++)
      {
        thoigian.phutgiay2[i].state = OFF;
      }
      for(int i=0; i<=1; i++)
      {
        thoigian.phutgiay[i].state = OFF;
        m = 00;
        s = 00;
      }
      delay(100);
      manhinh_time();
      currentScreen--;
  }
  // back ve man hinh default tu man hinh dang tap
  if(button3state == 0 && currentScreen == 5 && flag3 == 2){
      for(int i=0; i<=4; i++)
      {
        thoigian.timedefault[i].state = OFF;
      }
      m = 00;
      s = 00;
      chooseScreen2_2 = 1;
      delay(100);
      manhinh_default(chooseScreen2_2);
      currentScreen--;
  }
  //next
  if(button4state == 0 && currentScreen == 4 && flag1 == 2){
      delay(100);
      chooseScreen2 = 1;
      chooseScreen3_1 = 1;
      chooseScreen3_2 = 1;
      manhinh_custom(chooseScreen2);
      currentScreen--;
  }

  if(button4state == 0 && currentScreen == 4 && (flag2 == 2 || flag3 == 2) && (m>00 || s>00)){
    delay(50);
    manhinh_timing();
    currentScreen++;
    buttonpress = 0;
    
    for(int i = 0; i<=4; i++){
      if(person.left_hand.fingers[i].state == ACTIVE) {
        van_L_state = LOW;
        van6_L_state = HIGH;
        dongco_L_state = HIGH;
        pcf8575.digitalWrite(P0, van_L_state);
        pcf8575.digitalWrite(P6, van6_L_state);

        if(person.left_hand.fingers[0].state == ACTIVE){
          pcf8575.digitalWrite(P1, LOW);             
          Serial.println("van1_L");
        }
        if(person.left_hand.fingers[1].state == ACTIVE){
          pcf8575.digitalWrite(P2, LOW); 
          Serial.println("van2_L");
        }
        if(person.left_hand.fingers[2].state == ACTIVE){
          pcf8575.digitalWrite(P3, LOW); 
          Serial.println("van3_L");
        }
        if(person.left_hand.fingers[3].state == ACTIVE){
          pcf8575.digitalWrite(P4, LOW); 
          Serial.println("van4_L");
        }
        if(person.left_hand.fingers[4].state == ACTIVE){
          pcf8575.digitalWrite(P5, LOW); 
          Serial.println("van5_L");
        }
        digitalWrite(dongco_L, dongco_L_state);
        Serial.println("chay dong co tay trai");
        break;
      }
    }
    for(int i = 0; i<=4; i++){
      if(person.right_hand.fingers[i].state == ACTIVE) {
        van_R_state = LOW;
        van6_R_state = HIGH;
        dongco_R_state = HIGH;
        pcf8575.digitalWrite(P9, van_R_state);
        pcf8575.digitalWrite(P15, van6_R_state);

        if(person.right_hand.fingers[0].state == ACTIVE){
          pcf8575.digitalWrite(P10, LOW);             
          Serial.println("van1_R");
        }
        if(person.right_hand.fingers[1].state == ACTIVE){
          pcf8575.digitalWrite(P11, LOW); 
          Serial.println("van2_R");
        }
        if(person.right_hand.fingers[2].state == ACTIVE){
          pcf8575.digitalWrite(P12, LOW); 
          Serial.println("van3_R");
        }
        if(person.right_hand.fingers[3].state == ACTIVE){
          pcf8575.digitalWrite(P13, LOW); 
          Serial.println("van4_R");
        }
        if(person.right_hand.fingers[4].state == ACTIVE){
          pcf8575.digitalWrite(P14, LOW); 
          Serial.println("van5_R");
        }
        digitalWrite(dongco_R, dongco_R_state);
        Serial.println("chay dong co tay phai");
        break;
      }
    }
  }

// manhinh_timing
  if(buttonpress == 0 && currentScreen == 5){
    if(millis()-last>=900){
      tft.fillRoundRect(90, 60, 60, 35, 7.5, 0x053b50); 
      tft.setCursor(108,70);
      tft.setTextColor(TFT_WHITE);
      tft.setTextSize(2);
      tft.setTextDatum(MC_DATUM); 
      tft.print(m/10); 
      tft.print(m%10); 

      tft.fillRoundRect(170, 60, 60, 35, 7.5, 0x053b50); 
      tft.setCursor(187,70);
      tft.setTextColor(TFT_WHITE);
      tft.setTextSize(2);
      tft.setTextDatum(MC_DATUM); 
      tft.print(s/10); 
      tft.print(s%10); 
      s--;

      if(s < 0){
        s=59;
        m--;
      }
      last = millis();
    }

    if(millis()-last2>=4000){
        for(int i=0; i<=4; i++){
          if(person.left_hand.fingers[i].state == ACTIVE){
            van_L_state = !van_L_state;
            pcf8575.digitalWrite(P0, van_L_state);

            van6_L_state = !van6_L_state;
            pcf8575.digitalWrite(P6, van6_L_state);
            Serial.println("co bop tay trai");
            break;
          }
        }
        for(int i=0; i<=4; i++){
          if(person.right_hand.fingers[i].state == ACTIVE){
            van_R_state = !van_R_state;
            pcf8575.digitalWrite(P9, van_R_state);

            van6_R_state = !van6_R_state;
            pcf8575.digitalWrite(P15, van6_R_state);
            Serial.println("co bop tay phai");
            break;
          }
        }
        last2 = millis();
    }

      tft.fillRoundRect(193, 20, 40, 10, 2.5, TFT_WHITE); 
      tft.setTextColor(0x053b50);
      tft.setCursor(194,15);
      tft.setTextSize(2);
      tft.printf(".");
      delay(300);
      tft.setCursor(194,15);
      tft.printf("..");
      delay(300);
      tft.setCursor(194,15);
      tft.printf("...");
      delay(300);
        if(m <0)
      {
        m = 00;
        s = 00;

        digitalWrite(dongco_L, LOW);
        digitalWrite(dongco_R, LOW);
        //delay(100);
        manhinh_congra();
        currentScreen++;
      }
  }
  //manhinh_congra back
  if(button3state == 0 && currentScreen == 6){
      delay(100);
      manhinh_main();
      currentScreen = currentScreen - 4;
  }
}















