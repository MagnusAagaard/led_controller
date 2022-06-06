#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_NeoPixel.h>
#include <avr/pgmspace.h>

//Before code optimization:
//Prog storage: 20428 bytes (63%)
//Dynamic: 1684 bytes (82%)

//After fist:
//Prog storage: 18018 bytes (55%)
//Dynamic: 896 bytes (43%)
//Leaving 1152 bytes for LEDs?
//Current max #LEDs: 337
//Each LED uses around 3 bytes of SRAM
//And ~ 140 bytes extra for housekeeping? Or maybe reserved for the system not to crash

// To use the TCP version of rosserial_arduino
#define ROSSERIAL_ARDUINO_TCP

// LED data line pin
// 10, 11, 12, 13 reserved for Ethernet shield
#define PIN 6

#include <ros.h>
#include <led_controller/Color.h>

// Set the shield settings
const byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0x72, 0xFB };

// Set the rosserial socket server IP address (PC running led_controller node)
//IPAddress server(192,168,2,69);
IPAddress server(192,168,12,245);
// Set the rosserial socket server port
const uint16_t serverPort PROGMEM = 11411;

//Num subs, num pubs, intput buffer size (bytes), output buffer size (bytes)
//note that output buffer has to be atleast ~ 90 to establish connection (handshake)
ros::NodeHandle_<ArduinoHardware, 1, 0, 10, 100> nh;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(148, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(58, PIN, NEO_GRB + NEO_KHZ800);
//MIR: 58 LEDS
//SKAB: 148 LEDS
//94 leds on long strip


// Housekeeper variables for LED values
uint8_t r = 1;
uint8_t g = 1;
uint8_t b = 1;

void callback(const led_controller::Color& msg){
  r = msg.r;
  g = msg.g;
  b = msg.b;
}


ros::Subscriber<led_controller::Color> sub("led_controller/status", &callback);

void setup()
{
  strip.begin();
  strip.setBrightness(100);
  strip.show(); // Initialize all pixels to 'off'

  // Connect the Ethernet
  // If we need to know the IP address of the Arduino, we need to assign one
  //Ethernet.begin(mac, IPAddress(192, 168, 2, 13));
  // Else we can let the DHCP handle it
  Ethernet.begin(mac);
  
  // Let some time for the Ethernet Shield to be initialized
  delay(1000);

  // Set the connection to rosserial socket server
  nh.getHardware()->setConnection(server, serverPort);
  nh.initNode();
  nh.subscribe(sub);
  
}

void loop()
{
  lightLEDs(r, g, b);
  nh.spinOnce();
  if(!nh.connected())
  {
    //strip.clear();
    //strip.show();
    // If not connected show rainbow
    r = 1;
    g = 1;
    b = 1;
  }
  delay(50);
}

void lightLEDs(uint8_t re, uint8_t gr, uint8_t bl){
  if (re == 1 && gr == 1 && bl == 1){
    rainbow(20);
  }
  else{
    for(uint16_t i = 0; i < strip.numPixels(); i++){
      strip.setPixelColor(i, strip.Color(re,gr,bl));
    }
    strip.show();
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j+127) & 255));
    }
    strip.show();
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) { //Returns a color, used for cycling through them
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
