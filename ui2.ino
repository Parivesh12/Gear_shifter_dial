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
volatile int diff_encoder = 0;       // difference of direction
volatile int initial_direction=-1;   //+1 clockwise direction
volatile int final_direction=0;       //-1 anticlockwise direction


volatile int n_times_changed_encd_pos_clk = 4;
volatile int n_times_changed_encd_pos_anticlk = 4;

// Timer variables
hw_timer_t *timer = NULL;
volatile bool timer_flag = false;

int time_interval_durring_switching_mode=0;
const int minimum_value_of_time_bw_encd_pos=2; // in units sof 50 milliseconds
volatile int time_interval_bw_encoding_position_clk = minimum_value_of_time_bw_encd_pos;
volatile int time_interval_bw_encoding_position_anticlk = minimum_value_of_time_bw_encd_pos;
volatile int time_interval_bw_encoding_position_clk_atend=0;
volatile int time_interval_bw_encoding_position_anticlk_atend=0;

const int time_interval_bw_modes = 6;  // in units of 50 milliseconds
volatile int number_of_50_milliseconds_clk = time_interval_bw_modes;
volatile int number_of_50_milliseconds_anticlk = time_interval_bw_modes;


// Interuppt service routine for timer
void IRAM_ATTR onTimer(){
  timer_flag = true;
}


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
        n_times_changed_encd_pos_clk=0;
        number_of_50_milliseconds_clk=0;
        n_times_changed_encd_pos_anticlk=0;
        number_of_50_milliseconds_anticlk=0;

}



void setup() {
//  auto cfg = M5.config();
  M5.begin(); 
  auto cfg = M5.config();
  M5Dial.begin(cfg, true, false);

timer = timerBegin(0, 80, true);  // Timer 0, prescaler 80, count up
    timerAttachInterrupt(timer, &onTimer, true);  // Attach ISR
    timerAlarmWrite(timer, 50000, true);  // 50 mili second period (50000 microseconds)
    timerAlarmEnable(timer);  // Start timer
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
    M5Dial.Display.fillCircle(66,92,radius_of_cursor);//P
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
    
    if(timer_flag){
      timer_flag = false;
      number_of_50_milliseconds_clk++;
      number_of_50_milliseconds_anticlk++;
      time_interval_bw_encoding_position_clk++;
      time_interval_bw_encoding_position_anticlk++;
    }

    if(newposition>oldposition){
      oldposition=newposition;
      final_direction=+1;
      diff_encoder = abs(final_direction-initial_direction);
      if(diff_encoder==2){
        n_times_changed_encd_pos_clk=4;
      }
//      else{
//        n_times_changed_encd_pos_clk=0;
//      }
      initial_direction = final_direction;
      time_interval_bw_encoding_position_clk_atend = time_interval_bw_encoding_position_clk;
//      Serial.println("clk");
//      Serial.println(time_interval_bw_encoding_position_clk_atend);
//      Serial.println("clk");
      time_interval_bw_encoding_position_clk=0;
      n_times_changed_encd_pos_anticlk=0;
      n_times_changed_encd_pos_clk++;
//      if(n_times_changed_encd_pos_clk>=4){
//      oldposition=newposition;
    Serial.print("clk");
    Serial.println();
    Serial.print(number_of_50_milliseconds_clk);
    Serial.println();
    Serial.print(n_times_changed_encd_pos_clk);
    Serial.println();
    Serial.print(time_interval_bw_encoding_position_clk_atend);
    Serial.println();
   Serial.print("^");
   Serial.println();
     if((number_of_50_milliseconds_clk>=time_interval_bw_modes)&&
   (n_times_changed_encd_pos_clk>=4)&&
   (time_interval_bw_encoding_position_clk_atend >= minimum_value_of_time_bw_encd_pos)){
    Serial.print("^");
    Serial.println();
    Serial.print(number_of_50_milliseconds_clk);
    Serial.println();
    Serial.print(n_times_changed_encd_pos_clk);
    Serial.println();
    Serial.print(time_interval_bw_encoding_position_clk_atend);
    Serial.println();
    Serial.print("it will go ");
    Serial.println();
   Serial.print("clk");
   Serial.println();
   }
   if((number_of_50_milliseconds_clk>=time_interval_bw_modes)&&
   (n_times_changed_encd_pos_clk>=4)&&
   (time_interval_bw_encoding_position_clk_atend >= minimum_value_of_time_bw_encd_pos)){
       if(gear_mode==8){ // currently in Parking mode
        update_mode(97,65,4,reverse);  //set to Reverse
//        n_times_changed_encd_pos_clk=0;
//       number_of_50_milliseconds_clk=0;
        
       }
       else if(gear_mode==4){   // currently in Reverse mode
        update_mode(144,65,2,neutral); // set Neutral mode
//        n_times_changed_encd_pos_clk=0;
//       number_of_50_milliseconds_clk=0;
        
       }
       else if(gear_mode==2){ // currently in Neutral mode
        update_mode(171,92,1,drive);// set Drive mode
//        n_times_changed_encd_pos_clk=0;
//        number_of_50_milliseconds_clk=0;
       }
      }
       
//    delay(1000);

      
    }
    else if(newposition<oldposition){
      oldposition=newposition;
      final_direction=-1;
      diff_encoder = abs(final_direction-initial_direction);
      if(diff_encoder==2){
        n_times_changed_encd_pos_anticlk=4;
      }
//      else{
//        n_times_changed_encd_pos_anticlk=0;
//      }
      initial_direction = final_direction;
      time_interval_bw_encoding_position_anticlk_atend = time_interval_bw_encoding_position_anticlk;

      time_interval_bw_encoding_position_anticlk=0;      
      n_times_changed_encd_pos_clk=0;
      n_times_changed_encd_pos_anticlk++;
          Serial.print("anti");
          Serial.println(); 
    Serial.print(number_of_50_milliseconds_anticlk);
    Serial.println();
    Serial.print(n_times_changed_encd_pos_anticlk);
    Serial.println();
    Serial.print(time_interval_bw_encoding_position_anticlk_atend);
    Serial.println();
   Serial.print("^");
   Serial.println();
   if((number_of_50_milliseconds_anticlk>=time_interval_bw_modes)&&
   (n_times_changed_encd_pos_anticlk>=4)&&
   (time_interval_bw_encoding_position_anticlk_atend >= minimum_value_of_time_bw_encd_pos)){
        Serial.print("^");
        Serial.println();
    Serial.print(number_of_50_milliseconds_clk);
    Serial.println();
    Serial.print(n_times_changed_encd_pos_clk);
    Serial.println();
    Serial.print(time_interval_bw_encoding_position_clk_atend);
    Serial.println();
    Serial.print("it will go ");
    Serial.println();
   Serial.print("anti");
   Serial.println();
   }
   if((number_of_50_milliseconds_anticlk>=time_interval_bw_modes)&&
   (n_times_changed_encd_pos_anticlk>=4)&&
   (time_interval_bw_encoding_position_anticlk_atend >= minimum_value_of_time_bw_encd_pos)){/////////////
      if(gear_mode==1){ // currently in Drive mode
         update_mode(144,65,2,neutral); // set Neutral mode
//         n_times_changed_encd_pos_anticlk=0;
//          number_of_50_milliseconds_anticlk=0;

      }
      else if(gear_mode==2){ // currently in Neutral mode
        update_mode(97,65,4,reverse);  //set to Reverse mode
//        n_times_changed_encd_pos_anticlk=0;
//         number_of_50_milliseconds_anticlk=0;
      }
      else if(gear_mode==4){ // currently in Reverse mode
           update_mode(66,92,8,park); //set Parking mode
//           n_times_changed_encd_pos_anticlk=0;
//             number_of_50_milliseconds_anticlk=0;
      }
//      delay(1000); 
    }
  


}
}
