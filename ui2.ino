#include "M5Dial.h"
#include "evaimg.h"
#include "stdlib.h"

//
uint16_t eva_img[IMAGE_WIDTH*IMAGE_HEIGHT];  
uint16_t background_img[IMAGE_WIDTH*IMAGE_HEIGHT];  
int  radius_of_cursor = 5;
int millisecond =100;
int frequency_of_sound=10000;
const char* park = "P";
const char* reverse = "R";
const char* neutral = "N";
const char* drive = "D";
int gear_mode=8;  //set Parking bit(3) to high at begining 
long oldposition = 0;

void store_eva_image_data(uint16_t* imageData) {
    // Copy the pre-defined image data into the imageData array
    for (int i = 0; i < 240 * 240; i++) {
//        imageData[i] = evaimg[i];
         imageData[i] = pgm_read_word(&(evaimg[i]));
    }
}

void store_background_image_data(uint16_t* imageData) {
    // Copy the pre-defined image data into the imageData array
    for (int i = 0; i < 240 * 240; i++) {
//        imageData[i] = evaimg[i];
         imageData[i] = pgm_read_word(&(backgroundimg[i]));
    }
}

void update_mode(int x_coordinate_of_cursor, int y_coordinate_of_cursor, int Gear_mode, const char* Print_mode){
        clearText();
        M5Dial.Display.setColor(BLACK);
        gear_mode=Gear_mode; //set Drive bit(0) high
        M5Dial.Display.fillCircle(144,65,radius_of_cursor);//N
        removec();
        M5Dial.Display.setColor(WHITE);
        M5Dial.Display.fillCircle(x_coordinate_of_cursor, y_coordinate_of_cursor,radius_of_cursor);//D
        M5Dial.Display.setCursor(93,80);
        M5Dial.Display.setTextSize(2);
        M5Dial.Display.println(Print_mode);
        M5Dial.Speaker.tone(frequency_of_sound, millisecond);
}


//int gear_mode=8;  //set Parking bit(3) to high at begining 
//long oldposition = 0;
//void update_gear_mode(){
    



void setup() {
//  auto cfg = M5.config();
  M5.begin(); 
  auto cfg = M5.config();
  M5Dial.begin(cfg, true, false);
  store_eva_image_data(eva_img);  //store image data IN eva_img in RGB565 fromat 
  store_background_image_data(background_img); //store image data IN background_img in RGB565 fromat 
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

//    M5Dial.Display.setTextColor(BLUE);
//    M5Dial.Display.println("D");
//      M5Dial.Display.setColor(BLUE);
    M5Dial.Display.fillCircle(66,92,radius_of_cursor);//P
//    M5Dial.Display.fillCircle(171,92,radius_of_cursor);//D
//    M5Dial.Display.fillCircle(97,65,radius_of_cursor);//R
//    M5Dial.Display.fillCircle(144,65,radius_of_cursor);//N
//    M5Dial.Speaker.tone(frequency_of_sound, 1000);
//delay(5000);
}

void clearText() {
    M5Dial.Display.fillCircle(120,120,40, 0x2bd2);  // Adjust rectangle size and position as needed
}


void removec(){
  
    M5Dial.Display.fillCircle(66,92,radius_of_cursor);//P
    M5Dial.Display.fillCircle(171,92,radius_of_cursor);//D
    M5Dial.Display.fillCircle(97,65,radius_of_cursor);//R
    M5Dial.Display.fillCircle(144,65,radius_of_cursor);//N
}
void loop(){
  M5Dial.update();
    long newposition = M5Dial.Encoder.read();
    if(newposition>oldposition){
      oldposition=newposition;
     
       if(gear_mode==8){ // in P mode
        update_mode(97,65,4,reverse);  //set to R
        
       }
       else if(gear_mode==4){   // in R mode
        update_mode(144,65,2,neutral); // set N
        
       }
       else if(gear_mode==2){ // in N mode///////////////////////////
        update_mode(171,92,1,drive);///D mode
       }
//    delay(1000);

      
    }
    else if(newposition<oldposition){
      oldposition=newposition;
      
      if(gear_mode==1){///Nmode//////////////////////////////////
         update_mode(144,65,2,neutral); // set N

      }
      else if(gear_mode==2){
        update_mode(97,65,4,reverse);  //set to R
      }
      else if(gear_mode==4){
           update_mode(66,92,8,park); //set P
      }
//      delay(1000); 
    }
  
  

}
