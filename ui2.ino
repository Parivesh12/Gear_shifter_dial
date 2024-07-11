#include "M5Dial.h"
#include "evaimg.h"
#include "stdlib.h"

//  variable initialization
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

int min_encd =3;
volatile int n_times_changed_encd_pos_clk = min_encd;
volatile int n_times_changed_encd_pos_anticlk = min_encd;

// Timer variables
hw_timer_t *timer = NULL;



const int minimum_value_of_time_bw_encd_pos=3; // in units sof 50 milliseconds
volatile int time_interval_bw_encoding_position_clk = minimum_value_of_time_bw_encd_pos;
volatile int time_interval_bw_encoding_position_anticlk = minimum_value_of_time_bw_encd_pos;
volatile int time_interval_bw_encoding_position_clk_atend=0;
volatile int time_interval_bw_encoding_position_anticlk_atend=0;

const int time_interval_bw_modes = 6;  // in units of 50 milliseconds
volatile int number_of_50_milliseconds_clk = time_interval_bw_modes;
volatile int number_of_50_milliseconds_anticlk = time_interval_bw_modes;


int park_mode_color = 0x2bd2;
int neutral_mode_color = 0x2c8b;
int reverse_mode_color = 0x9465;
int drive_mode_color = 0x9147;


// Interuppt service routine for timer
void IRAM_ATTR onTimer(){

      number_of_50_milliseconds_clk++;
      number_of_50_milliseconds_anticlk++;
      time_interval_bw_encoding_position_clk++;
      time_interval_bw_encoding_position_anticlk++;

}


void Display_background(int mode_color) {
    M5Dial.Display.fillCircle(120,120,50, mode_color);  
    for(int i=1;i<7;i++){
    
      M5Dial.Display.drawCircle(120,120,112+i, mode_color);  
}
}


void removec(){
  
    M5Dial.Display.fillCircle(66,92,radius_of_cursor);//P
    M5Dial.Display.fillCircle(171,92,radius_of_cursor);//D
    M5Dial.Display.fillCircle(97,65,radius_of_cursor);//R
    M5Dial.Display.fillCircle(144,65,radius_of_cursor);//N
}


void update_mode(int x_coordinate_of_cursor, int y_coordinate_of_cursor, int Gear_mode, const char* Print_mode, int mode_color){

        Display_background(mode_color);
        n_times_changed_encd_pos_clk=0;
        number_of_50_milliseconds_clk=0;
        n_times_changed_encd_pos_anticlk=0;
        number_of_50_milliseconds_anticlk=0;
        
        M5Dial.Display.setTextSize(1);
        M5Dial.Display.setCursor(33,60);
        M5Dial.Display.println("P");
        M5Dial.Display.setCursor(77,24);
        M5Dial.Display.println("R");
        M5Dial.Display.setCursor(137,24);
        M5Dial.Display.println("N");
        M5Dial.Display.setCursor(180,60);
        M5Dial.Display.println("D");
//        clearText(mode_color);
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



void setup() {

  M5.begin(); 
  auto cfg = M5.config();
  M5Dial.begin(cfg, true, false);

    timer = timerBegin(0, 80, true);  // Timer 0, prescaler 80, count up
    timerAttachInterrupt(timer, &onTimer, true);  // Attach ISR
    timerAlarmWrite(timer, 50000, true);  // 50 mili second period (50000 microseconds)
    timerAlarmEnable(timer);  // Start timer
    M5Dial.Display.fillScreen(TFT_BLACK); 
    M5Dial.Display.drawBitmap(0,0,240,240,evaimg);
    delay(1000);
    M5Dial.Display.fillScreen(TFT_BLACK); 


    Display_background(0x2bd2);
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



void loop(){
  M5Dial.update();
    long newposition = M5Dial.Encoder.read();


    if(newposition>oldposition){
      oldposition=newposition;
      final_direction=+1;
      diff_encoder = abs(final_direction-initial_direction);
      if(diff_encoder==2){
        n_times_changed_encd_pos_clk=min_encd;
      }

      initial_direction = final_direction;
      time_interval_bw_encoding_position_clk_atend = time_interval_bw_encoding_position_clk;

      time_interval_bw_encoding_position_clk=0;
      n_times_changed_encd_pos_anticlk=0;
      n_times_changed_encd_pos_clk++;

      

    if((number_of_50_milliseconds_clk>=time_interval_bw_modes)&&
       (n_times_changed_encd_pos_clk>=min_encd)&&
       (time_interval_bw_encoding_position_clk_atend >= minimum_value_of_time_bw_encd_pos)){
       if(gear_mode==8){ // currently in Parking mode
        
        update_mode(97,65,4,reverse, reverse_mode_color);  //set to Reverse
        
       }
       else if(gear_mode==4){   // currently in Reverse mode
        
        update_mode(144,65,2,neutral, neutral_mode_color); // set Neutral mode

        
       }
       else if(gear_mode==2){ // currently in Neutral mode
        
        update_mode(171,92,1,drive, drive_mode_color);// set Drive mode

       }
      }
       


      
    }
    else if(newposition<oldposition){
      oldposition=newposition;
      final_direction=-1;
      diff_encoder = abs(final_direction-initial_direction);
      if(diff_encoder==2){
        n_times_changed_encd_pos_anticlk=min_encd;
      }

      initial_direction = final_direction;
      time_interval_bw_encoding_position_anticlk_atend = time_interval_bw_encoding_position_anticlk;

      time_interval_bw_encoding_position_anticlk=0;      
      n_times_changed_encd_pos_clk=0;
      n_times_changed_encd_pos_anticlk++;
        
   if((number_of_50_milliseconds_anticlk>=time_interval_bw_modes)&&
      (n_times_changed_encd_pos_anticlk>=min_encd)&&
      (time_interval_bw_encoding_position_anticlk_atend >= minimum_value_of_time_bw_encd_pos)){/////////////
        
      if(gear_mode==1){ // currently in Drive mode
        
         update_mode(144,65,2,neutral, neutral_mode_color); // set Neutral mode


      }
      else if(gear_mode==2){ // currently in Neutral mode
        
         update_mode(97,65,4,reverse, reverse_mode_color);  //set to Reverse mode

      }
      else if(gear_mode==4){ // currently in Reverse mode
        
         update_mode(66,92,8,park, park_mode_color); //set Parking mode

      }

    }
  


}
}
