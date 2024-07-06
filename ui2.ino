#include "M5Dial.h"
#include "evaimg.h"
#include "stdlib.h"


uint16_t eva_img[IMAGE_WIDTH*IMAGE_HEIGHT];  
uint16_t background_img[IMAGE_WIDTH*IMAGE_HEIGHT];  
int  r = 5;

long oldposition = 0;

void store_image(uint16_t* imageData) {
    // Copy the pre-defined image data into the imageData array
    for (int i = 0; i < 240 * 240; i++) {
//        imageData[i] = evaimg[i];
         imageData[i] = pgm_read_word(&(evaimg[i]));
    }
}

void background_image_store(uint16_t* imageData) {
    // Copy the pre-defined image data into the imageData array
    for (int i = 0; i < 240 * 240; i++) {
//        imageData[i] = evaimg[i];
         imageData[i] = pgm_read_word(&(backgroundimg[i]));
    }
}

int
int reverse_mode = 0;


void setup() {
//  auto cfg = M5.config();
  M5.begin(); 
  auto cfg = M5.config();
  M5Dial.begin(cfg, true, false);
  store_image(eva_img);
  background_image_store(background_img);
    M5Dial.Display.fillScreen(TFT_BLACK); 
    M5Dial.Display.drawBitmap(0,0,240,240,eva_img);
    delay(2000);
    M5Dial.Display.drawBitmap(0,0,240,240,background_img);
    
    M5Dial.Display.setTextFont(&fonts::Orbitron_Light_32);
    M5Dial.Display.setCursor(93,80);
    M5Dial.Display.setTextSize(2);
    M5Dial.Display.setTextColor(WHITE);
    M5Dial.Display.println("P");
    M5Dial.Display.setTextSize(1);
        M5Dial.Display.setCursor(33,60);
    M5Dial.Display.println("P");
    
     M5Dial.Display.setCursor(77,24);
    M5Dial.Display.println("R");
    
        M5Dial.Display.setCursor(137,24);
    M5Dial.Display.println("N");
    
        M5Dial.Display.setCursor(180,60);
    M5Dial.Display.println("D");


    M5Dial.Display.fillCircle(66,92,r);//P
//    M5Dial.Display.fillCircle(171,92,r);//D
//    M5Dial.Display.fillCircle(97,65,r);//R
//    M5Dial.Display.fillCircle(144,65,r);//N
    M5Dial.Speaker.tone(10000, 300);
delay(2000);
}

void loop(){
  
    M5Dial.update();
    long newposition = M5Dial.Encoder.read();
    if(newposition>oldposition){
      
    }
}
