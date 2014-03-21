// neo_pixel_control.ino
// a lot pulled from goggles.pde from NeoPixel examples

#include <Adafruit_NeoPixel.h>
#include <Console.h>
// may need to comment out Bridge.h if not using arduino YUN, as well as in setup()
#include <Bridge.h>

#define PIN         7

#define LED_OFFSET  0 // Change these if the first pixel is not

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

// Gamma correction improves appearance of midrange colors
const uint8_t PROGMEM gamma8[] = {
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,
      1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,
      3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,
      7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12,
     13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20,
     20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29,
     30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42,
     42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
     58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 75,
     76, 77, 78, 80, 81, 82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96,
     97, 99,100,102,103,105,106,108,109,111,112,114,115,117,119,120,
    122,124,125,127,129,130,132,134,136,137,139,141,143,145,146,148,
    150,152,154,156,158,160,162,164,166,168,170,172,174,176,178,180,
    182,184,186,188,191,193,195,197,199,202,204,206,209,211,213,215,
    218,220,223,225,227,230,232,235,237,240,242,245,247,250,252,255
};

uint32_t
  ledColor[16][3];    // Background colors for eyes
int16_t
  base_hue     = 0; // Initial hue around perimeter (0-1535)
uint8_t
  ledBrightness[16],    // Brightness map -- eye colors get scaled by these
  brightness   = 80; // Global brightness (0-255)

int height = 0;
float pitch = 0.0;
float roll = 0.0;
int tilt_factor = 125;
float led_rad[16];

const float pi = 3.14159;

void set_angles(){
  int i;
  float tot_rad = 0;
  for(i=0; i<16; i++){
    led_rad[i] = tot_rad;
    tot_rad += ( 22.5 / 180.0 ) * pi;
  }
}

void setup() {
  Serial.begin(9600);
  Bridge.begin();
  // Console.begin();
  // while (!Console){
  //     ; // wait for Console port to connect.
  // }
  // Console.println("ready!");
  set_angles();
  pixels.begin();
}

void loop() {
  uint8_t i, r, g, b, a;
  int     h;

  // loop through serial characters and assign possitions
  if(Serial.available() && Serial.read() == '#'){
    height = Serial.parseInt();
    if(Serial.read() == ','){
      pitch = Serial.parseFloat();
    }
    if(Serial.read() == ','){
      roll = Serial.parseFloat();
    }
  }

  // only map between 50-600 height values
  base_hue = map(height, 50, 600, 0, 1536);

  // loop through
  for(h=base_hue, i=0; i<16; i++) {
    // take the position of the led, then calculate
    float dist = sin(led_rad[i]) * pitch;
    int ledAdjust = round((float)tilt_factor * dist);
    h = constrain(h + ledAdjust, 0, 1536);

    // re-use some vars for the roll calculation, this should be DRYer
    dist = sin(led_rad[i] + pi/2) * roll;
    ledAdjust = round((float)tilt_factor * dist);
    h = constrain(h + ledAdjust, 0, 1536);

    a = h;
    switch((h >> 8) % 6) {
      case 0: ledColor[i][0] = 255; ledColor[i][1] =   a; ledColor[i][2] =   0; break;
      case 1: ledColor[i][0] =  ~a; ledColor[i][1] = 255; ledColor[i][2] =   0; break;
      case 2: ledColor[i][0] =   0; ledColor[i][1] = 255; ledColor[i][2] =   a; break;
      case 3: ledColor[i][0] =   0; ledColor[i][1] =  ~a; ledColor[i][2] = 255; break;
      case 4: ledColor[i][0] =   a; ledColor[i][1] =   0; ledColor[i][2] = 255; break;
      case 5: ledColor[i][0] = 255; ledColor[i][1] =   0; ledColor[i][2] =  ~a; break;
    }
  }

  // set brightness accross the board
  memset(ledBrightness, brightness, sizeof(ledBrightness));

  for(i=0; i<16; i++) {
    a = ledBrightness[i] + 1;
    // ring RGB color
    r = ledColor[i][0];          // Initial background RGB color
    g = ledColor[i][1];
    b = ledColor[i][2];
    if(a) {
      r = (r * a) >> 8;          // Scale by brightness map
      g = (g * a) >> 8;
      b = (b * a) >> 8;
    }
    pixels.setPixelColor(((i + LED_OFFSET) & 15),
      pgm_read_byte(&gamma8[r]), // Gamma correct and set pixel
      pgm_read_byte(&gamma8[g]),
      pgm_read_byte(&gamma8[b]));

  }
  pixels.show();

  delay(10);
}











