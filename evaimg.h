
#ifndef EVAIMG_H
#define EVAIMG_H
#include "esp32-hal.h"
// Define image dimensions
#define IMAGE_WIDTH 240
#define IMAGE_HEIGHT 240

// Function to generate random RGB565 image data
//void generateRandomImage( uint16_t* imageData) {
extern const uint16_t evaimg[IMAGE_WIDTH * IMAGE_HEIGHT];
extern const uint16_t backgroundimg[IMAGE_WIDTH * IMAGE_HEIGHT];


// defining fucntions 
//void store_eva_image_data(uint16_t* imageData);
//void store_background_image_data(uint16_t* imageData)
#endif
