#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

#include <JPEGDecoder.h>
// Return the minimum of two values a and b
#define minimum(a,b)     (((a) < (b)) ? (a) : (b))

// Include the sketch header file that contains the image stored as an array of bytes
// More than one image array could be stored in each header file.
#include "anh.h"
#include "nen1.h"
#include "nen2.h"
#include "nen3.h"
#include "nen4.h"
#include "nen5.h"
#include "reset.h"
const int button = 34;
int buttonstate = HIGH;
const int button1 = 35;
int button1state = HIGH;
const int button2 = 32;
int button2state = HIGH;
const int button3 = 33;
int button3state = HIGH;
const int button4 = 25;
int button4state = HIGH;
int chooseScreen2;
int currentScreen = 1; 
int chooseScreen3_1;
int chooseScreen3_2 = 1;
bool flag = false;
bool co = false;
int flag1;
int flag2;
int chooseScreen3_1_temp = 1;
bool button_pressed = false;
int m = 00;
int s = 00;
int a = 0;
int m_temp;
int s_temp;
int tt_button = 0;
int last ;
int buttonpress = 1;

typedef enum {
    ON,
    OFF
} TimeState;

struct TG {
    int num;
    TimeState state;
};

struct PG {
    struct TG phutgiay[2];
    struct TG phutgiay2[4];
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
  if(currentScreen == 4){
    if(millis()-last>=200){
      if (buttonpress == 0){
        buttonpress = 1;
        tft.fillCircle(182, 185, 20, TFT_ORANGE);
        tft.fillRoundRect(175, 176, 5, 20, 0, TFT_WHITE); 
        tft.fillRoundRect(185, 176, 5, 20, 0, TFT_WHITE); 
      }
      else {
        buttonpress = 0;
        int downArrowX1 = 191;
        int downArrowY1 = 185;
        tft.fillCircle(182, 185, 20, TFT_ORANGE);
        tft.fillTriangle(downArrowX1 , downArrowY1 , downArrowX1 - 15, downArrowY1 - 7, downArrowX1 - 15, downArrowY1 + 7, TFT_WHITE);
      }
      last = millis();
    }
  }
}

void manhinh()
{
  flag = false;
  drawArrayJpeg(anh, sizeof(anh), 0, 0); // Draw a jpeg image stored in memory at x,y
  
}

void manhinh2(int chooseScreen2)
{

  if(!flag){
    for (int i = 0; i < 6; ++i) {
      person.left_hand.fingers[i].state = INACTIVE;
      person.right_hand.fingers[i].state = INACTIVE;
        Serial.println("a");
    }
    flag = true;
  }

  drawArrayJpeg(nen1, sizeof(nen1), 0, 0); // Draw a jpeg image stored in memory at x,y
  int downArrowX = 30;
  int downArrowY = 215;
  tft.fillTriangle(downArrowX , downArrowY , downArrowX + 25, downArrowY - 15, downArrowX + 25, downArrowY + 15, 0x053b50);

  if(chooseScreen2 == 1) 
    tft.fillRoundRect(40, 95, 120, 70, 10, 0x053b50);
  else
    tft.fillRoundRect(50, 105, 100, 50, 10, 0x053b50); 

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("TAY PHAI", 100, 130);


   if(chooseScreen2 == 2) 
   tft.fillRoundRect(160, 95, 120, 70, 10, 0x053b50);
  else
     tft.fillRoundRect(170, 105, 100, 50, 10, 0x053b50);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("TAY TRAI", 220, 130);


 if(chooseScreen2 == 3) 
    tft.fillRoundRect(90, 165, 140, 70, 10, 0x053b50);
  else
    tft.fillRoundRect(100, 175, 120, 50, 10, 0x053b50);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("THOI GIAN", 160, 200);
}

void manhinh3(int hand){
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
    drawArrayJpeg(nentayphai, sizeof(nentayphai), 0, 0);
  else
    drawArrayJpeg(nentaytrai, sizeof(nentaytrai), 0, 0);
   // Draw a jpeg image stored in memory at x,y
  
   if( hand == 1){
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
  }else{
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
  }

  if( hand == 1){
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
  }else{
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
  }

  if( hand == 1){
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
  }else{
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
  }

 if( hand == 1){
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
  }else{
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
  }

  if( hand == 1){
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
  }else{
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
  }

  if( hand == 1){
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
  }else{
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

void manhinh5() {
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
  int downArrowX = 30;
  int downArrowY = 215;
  tft.fillTriangle(downArrowX , downArrowY , downArrowX + 25, downArrowY - 15, downArrowX + 25, downArrowY + 15, 0x053b50);

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
  tft.drawString("(phut)", 115, 102.5); 

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
  tft.drawString("(giay)", 205, 102.5); 

    tft.setCursor(194,60);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM); 
    tft.print(s/10); 
    tft.print(s%10); 

  if (chooseScreen3_2 == 3)
    tft.drawBitmap(250,50,reset,40,30,TFT_RED );//CHON ROI
  else
    tft.drawBitmap(250,50,reset,40,30,TFT_BLACK );// CHUA CHON

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
  tft.drawString("phut", 50, 161.5); 

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
  tft.drawString("phut", 125, 161.5); 

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
  tft.drawString("phut", 200, 161.5); 

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
  tft.drawString("phut", 275, 161.5); 

  int downArrowX1 = 171;
  int downArrowY1 = 215;
  tft.fillCircle(162, 215, 17, 0x053b50);
  tft.fillTriangle(downArrowX1 , downArrowY1 , downArrowX1 - 15, downArrowY1 - 7, downArrowX1 - 15, downArrowY1 + 7, TFT_WHITE);

  // for(int m=00; m<=60; m++){
  //   delay(1000);
  //   tft.fillRoundRect(80, 45, 70, 45, 7.5, 0x053b50); 
  //   tft.setCursor(105,60);
  //   tft.setTextColor(TFT_WHITE);
  //   tft.setTextSize(2);
  //   tft.setTextDatum(MC_DATUM); 
  //   tft.print(m/10); 
  //   tft.print(m%10); 
  // }
        

}

void manhinh6(){
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
  if(buttonpress == 1){
    int downArrowX1 = 191;
    int downArrowY1 = 185;
    tft.fillCircle(182, 185, 20, TFT_ORANGE);
    tft.fillTriangle(downArrowX1 , downArrowY1 , downArrowX1 - 15, downArrowY1 - 7, downArrowX1 - 15, downArrowY1 + 7, TFT_WHITE);
  }

  tft.setTextColor(0x053b50);
  tft.setTextSize(4);
  tft.setTextDatum(MC_DATUM);  
  tft.drawString(":", 162, 79); 

  int downArrowX = 80;
  int downArrowY = 185;
  tft.fillTriangle(downArrowX , downArrowY , downArrowX + 25, downArrowY - 15, downArrowX + 25, downArrowY + 15, 0x053b50);


  // for(int i=m; i>=0; i--){
  //   delay(1000);
  //   tft.fillRoundRect(90, 60, 60, 35, 7.5, 0x053b50); 
  //   tft.setCursor(107,70);
  //   tft.setTextColor(TFT_WHITE);
  //   tft.setTextSize(2);
  //   tft.setTextDatum(MC_DATUM); 
  //   tft.print(m/10); 
  //   tft.print(m%10); 
  // }
       
    
      // if (buttonstate == 0 ) 
      // {
      //   printf("qwertyuio");
      //   m_temp = e;
      //   s_temp = i;
      //   tft.fillRoundRect(170, 60, 60, 35, 7.5, 0x053b50); 
      //   tft.setCursor(185,70);
      //   tft.setTextColor(TFT_WHITE);
      //   tft.setTextSize(2);
      //   tft.setTextDatum(MC_DATUM); 
      //   tft.print(s_temp/10); 
      //   tft.print(s_temp%10); 

      //   tft.fillRoundRect(95, 60, 60, 35, 7.5, 0x053b50); 
      //   tft.setCursor(107,70);
      //   tft.setTextColor(TFT_WHITE);
      //   tft.setTextSize(2);
      //   tft.setTextDatum(MC_DATUM); 
      //   tft.print(m_temp/10); 
      //   tft.print(m_temp%10); 

      //   break;
      // }

    }
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(button, INPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);

  attachInterrupt(digitalPinToInterrupt(button), buttonCallback,FALLING);

  tft.init();
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);
  manhinh();
}
void loop (){

  buttonstate = digitalRead(button);
  button3state = digitalRead(button3);//back
  button1state = digitalRead(button1);//left
  button2state = digitalRead(button2);//right
  button4state = digitalRead(button4);//next
//enter
  if(buttonstate == 0 && button_pressed == false && currentScreen == 1)
  {
    Serial.println(chooseScreen2); 
    Serial.println(currentScreen);
      chooseScreen2=1;
      currentScreen++;
      delay(100);
      manhinh2(chooseScreen2);
    
    button_pressed = true;

  }
  if(buttonstate == 1){
    button_pressed = false;
  }


  //man hinh 2
  // qua phai
  if(currentScreen == 2 && button2state ==0){
    chooseScreen2++;
    if(chooseScreen2 == 4 )
      chooseScreen2 = 1;
    Serial.println(chooseScreen2);
    delay(100);   
    manhinh2(chooseScreen2);
  }
  // qua trai
  if(currentScreen == 2 && button1state ==0){
    chooseScreen2--;
    if(chooseScreen2 == 0)
      chooseScreen2 = 3;
    Serial.println(chooseScreen2);
     delay(100); 
    manhinh2(chooseScreen2);
  }
  // vao man hinh tay phai, tay trai (manhinh3)
  if(buttonstate == 0 && button_pressed == false && currentScreen == 2 && (chooseScreen2 == 1 || chooseScreen2 == 2)){
      currentScreen++;
      chooseScreen3_1=1;
      delay(100);
      manhinh3(chooseScreen2);
      flag1 = 2;
      flag2 = 1;
    button_pressed = true;
  }
  if(buttonstate == 1){
    button_pressed = false;
  }
  // vao man hinh tg tap(manhinh5)
  if(currentScreen == 2 && chooseScreen2 == 3 && buttonstate == 0 && button_pressed == false){
    currentScreen++;
    delay(100);
    manhinh5();
    flag2 = 2;
    flag1 = 1;
    button_pressed = true;
  }  
  if(buttonstate == 1){
    button_pressed = false;
  }

//man hinh 3
  if(currentScreen == 3 && button2state ==0 && flag1 == 2 ){//phai
    chooseScreen3_1++;
    if(chooseScreen3_1 == 7 ){
      chooseScreen3_1 = 1;
    }
    Serial.println(chooseScreen3_1); 
     delay(100); 
    manhinh3(chooseScreen2);
  }

  if(currentScreen == 3 && button1state ==0 && flag1 == 2){//TRAI
    chooseScreen3_1--;
    if(chooseScreen3_1 == 0)
      chooseScreen3_1 = 6;
    Serial.println(chooseScreen3_1);
     delay(100); 
    manhinh3(chooseScreen2);
  }

  if(currentScreen == 3 && chooseScreen3_1 >= 1 && chooseScreen3_1 <=6 && buttonstate == 0 && button_pressed == false && flag1 == 2){
      if(chooseScreen2 == 1){  
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

      if(chooseScreen2 == 2 ){
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
      delay(100);
       manhinh3(chooseScreen2);
       button_pressed = true;
  }
  if(buttonstate == 1){
    button_pressed = false;
  }
// manhinh5
if(currentScreen == 3 && button2state ==0 && flag2 == 2 && a == 0){//phai
    chooseScreen3_2++;
    if(chooseScreen3_2 == 8 ){
      chooseScreen3_2 = 1;
    }
     delay(100); 
    manhinh5();
  }

  if(currentScreen == 3 && button1state ==0 && flag2 == 2 && a == 0){//TRAI
    chooseScreen3_2--;
    if(chooseScreen3_2 == 0)
      chooseScreen3_2 = 7;
     delay(100); 
    manhinh5();
  }

  if(currentScreen == 3 && chooseScreen3_2 >= 1 && chooseScreen3_2 <=7 && buttonstate == 0 && button_pressed == false && flag2 == 2){
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
    manhinh5();
    button_pressed = true;
  }
  if(buttonstate == 1){
    button_pressed = false;
  }
  
  if(currentScreen == 3 && button2state ==0 && flag2 == 2 && a == 1){//phai tăng
    if(chooseScreen3_2 == 1){
      m++;
      if(m > 60 ){
        m = 00;
        s = 00;
      }
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
    manhinh5();
  }

  if(currentScreen == 3 && button1state ==0 && flag2 == 2 && a == 1){//TRAI giảm
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
    manhinh5();
  }

  //back
  if(button3state == 0){
    switch (currentScreen) {
      case 2:
        delay(100); 
        manhinh();
        currentScreen--;
        break;
      // case 3:
      //   delay(100);
      //   chooseScreen2 = 1;
      //   chooseScreen3_1 = 1;
      //   chooseScreen3_2 = 1;
      //   manhinh2(chooseScreen2);
      //   currentScreen--;
      //   break;
      default:
        break;
    }
  }
  if(button3state == 0 && currentScreen == 3 && flag2 == 2){
      delay(100);
      chooseScreen2 = 1;
      chooseScreen3_1 = 1;
      chooseScreen3_2 = 1;
      manhinh2(chooseScreen2);
      currentScreen--;
  }

  if(button3state == 0 && currentScreen == 4){
      delay(100);
      manhinh5();
      currentScreen--;
  }
  //next
  if(button4state == 0 && currentScreen == 3 && flag1 == 2){
      delay(100);
      chooseScreen2 = 1;
      chooseScreen3_1 = 1;
      chooseScreen3_2 = 1;
      manhinh2(chooseScreen2);
      currentScreen--;
  }

  if(button4state == 0 && currentScreen == 3 && flag2 == 2){
    delay(50);
    manhinh6();
    currentScreen++;
    buttonpress = 0;
  }

  if(buttonpress == 0 && currentScreen == 4){
    if(millis()-last>=800){
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
      if(m <0)
      {
        m = 00;
        s = 00;

      }
      last = millis();
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
        manhinh5();
        currentScreen--;
      }
      }
}















