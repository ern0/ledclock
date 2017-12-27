# include <stdint.h>
# include "ledclock.hpp"
# ifndef SDL_DISPLAY
#include <Adafruit_NeoPixel.h>
# endif

static uint8_t irqdiv = 0;

# ifdef SDL_DISPLAY
# include "stamp.inc"
# else
static int8_t clockHour = 20;
static int8_t clockMin = 42;
static int8_t clockSec = 20;
# endif
static int8_t clockDeci = 0;

static uint16_t frame = 0;
static bool updateRequest = false;

static uint8_t smallFadeOutPos;
static uint8_t smallFadeInPos;
static uint8_t smallFadeOutValue;
static uint8_t smallFadeInValue;


Adafruit_NeoPixel strip = Adafruit_NeoPixel(LARGE_PIX + SMALL_PIX,PIN,NEO_GRB + NEO_KHZ800);

# include "screen.inc"


  void setup() {

    # ifdef SDL_DISPLAY
      setupEmu();
    # endif
    
    pinMode(FRONT_LEFT,INPUT);
    pinMode(REAR_LEFT,INPUT);
    pinMode(FRONT_RIGHT,INPUT);
    pinMode(REAR_RIGHT,INPUT);
    
    strip.begin();
    strip.setBrightness(50);
    strip.show();

    Serial.begin(38400);

    setupTimerInterrupt();
    
  } // setup()


  void loop() {

    if (!updateRequest) return;
    
    calcSmall();
    calcLarge();
    redrawClock();
    
    updateRequest = false;
  
  } // loop
  

  void setupTimerInterrupt() {
  
    cli();
  
    //set timer2 interrupt at 1000 Hz
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;
    
    OCR2A = 249;
    TCCR2A |= (1 << WGM21);
    TCCR2B |= (1 << CS22);
    TIMSK2 |= (1 << OCIE2A);
      
    sei();

  } // setupTimerInterrupt()


  ISR(TIMER2_COMPA_vect) {
  
    irqdiv++;
    if (irqdiv < 100) return;
    irqdiv = 0;
    
    tick();  // 10 Hz
    
  } // ISR()
  

  void tick() { // 10 Hz

    buttonHandler();
    for (int n = 0; n < 90; n++) ///
      incDeci();

    updateRequest = true;
  
  } // tick()


   void buttonHandler() {
 
    if (frame % 20 == 0) {
      if (analogRead(FRONT_LEFT) > 888) incHour();
      if (analogRead(REAR_LEFT) > 888) decHour();
    } // if hour

    if (frame % 8 == 0) {
      if (analogRead(FRONT_RIGHT) > 888) incMin();
      if (analogRead(REAR_RIGHT) > 888) decMin();
    } // if min

    frame++;

  } // buttonHandler()


  void incDeci() {

    clockDeci++;
    if (clockDeci < 10) return;
    clockDeci = 0;

    incSec();
    
  } // incDeci()


  void incSec() {

    clockSec++;
    if (clockSec < 60) return;
    clockSec = 0;

    incMin();

  } // incSec()


  void incMin() {

    clockMin++;
    if (clockMin < 60) return;
    clockMin = 0;

    incHour();

  } // incMin()


  void decMin() {

    --clockMin;
    if (clockMin > -1) return; 
    clockMin = 59;

    decHour();

  } // decMin()


  void incHour() {

    clockHour++;
    if (clockHour < 12) return;
    clockHour = 0;
    
  } // incHour()


  void decHour() {

    --clockHour;
    if (clockHour > -1) return;
    clockHour = 11;

  } // decHour()


  void calcSmall() {

    smallFadeOutPos = 0;
    smallFadeInPos = 1;
    smallFadeOutValue = 0x33;
    smallFadeInValue = 0xcc;

  } // calcSmall()


  void calcLarge() {

  } // calcLarge()


  void redrawClock() {


    # if 1
      static char buffer[120];
      sprintf(buffer,
        "TIME: %02d:%02d:%02d.%01d "
        "small pos: %d->%d fade: %d:%d \n"
        ,(int)clockHour
        ,(int)clockMin
        ,(int)clockSec
        ,(int)clockDeci
        ,(int)smallFadeOutPos
        ,(int)smallFadeInPos
        ,(int)smallFadeOutValue
        ,(int)smallFadeInValue
      );
      # ifdef SDL_DISPLAY
        printf("%s",buffer);
      # else
        Serial.write(buffer,strlen(buffer));
      # endif
    # endif

    for (int n = 0; n < 12; n++) {

      if (n == smallFadeOutPos) {
        strip.setPixelColor(small[n],0,0,smallFadeOutValue);
        continue;
      }

      if (n == smallFadeInPos) {
        strip.setPixelColor(small[n],0,0,smallFadeInValue);
        continue;        
      }

      if (n < smallFadeOutPos) {
        strip.setPixelColor(small[n],0,0,BRITE_PAST);
      } else {
        strip.setPixelColor(small[n],0,0,BRITE_FUTURE);
      }

    } // for small

    for (int n = 0; n < 24; n++) {
		} // for large


    strip.show();
    
  } // redrawClock() 
  
