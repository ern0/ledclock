# include <stdint.h>
# include "ledclock.hpp"
# ifndef SDL_DISPLAY
#include <Adafruit_NeoPixel.h>
# endif

static uint8_t irqdiv = 0;

//# ifdef SDL_DISPLAY
//# include "stamp.inc"
//# else
static int8_t clockHour = 15 % 12;
static int8_t clockMin = 45;
static int8_t clockSec = 35;
//# endif
static int8_t clockCenti = 0;

static int8_t showHour;
static int8_t showMin;
static int8_t showSec;
static int8_t showDeci;
static int8_t showCenti;

static uint16_t frame = 0;
static bool updateRequest = false;

static int8_t hourFadeOutPos;
static int8_t hourFadeInPos;
static uint8_t hourFadeOutValue;
static uint8_t hourFadeInValue;

static int8_t minFadeOutPos;
static int8_t minFadeInPos;
static uint8_t minFadeOutValue;
static uint8_t minFadeInValue;

static int8_t secFadeOutPos;
static int8_t secFadeInPos;
static uint8_t secFadeOutValue;
static uint8_t secFadeInValue;


Adafruit_NeoPixel strip = Adafruit_NeoPixel(LARGE_PIX + SMALL_PIX,PIN,NEO_GRB + NEO_KHZ800);

# include "screen.inc"
# ifdef SDL_DISPLAY
# include "test.inc"
# endif

  void setup() {

    # ifdef SDL_DISPLAY
      setupEmu();
    # endif

    # if TEST
      performTest();
    # endif    
    
    pinMode(FRONT_LEFT,INPUT);
    pinMode(REAR_LEFT,INPUT);
    pinMode(FRONT_RIGHT,INPUT);
    pinMode(REAR_RIGHT,INPUT);
    
    strip.begin();
    strip.setBrightness(47);
    strip.show();

    Serial.begin(38400);

    setupTimerInterrupt();
    
  } // setup()


  void loop() {

    if (!updateRequest) return;    

    snapshotTime();

    calcHour();
    calcMin();
    calcSec();

    redrawSmall();
    redrawLarge();
    strip.show();    
  
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
    if (irqdiv < 10) return;
    irqdiv = 0;
    
for (int n = 0; n < 3; n++)
    tick();  // 100 Hz
    
  } // ISR()
  

  void tick() {  // 100 Hz

    buttonHandler();
    incCenti();

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


  void snapshotTime() {

    showHour = clockHour;
    showMin = clockMin;
    showSec = clockSec; 
    showCenti = clockCenti;
    showDeci = showCenti / 10;

  } // snapshotTime()


  void incCenti() {

    clockCenti++;
    if (clockCenti < 100) return;
    clockCenti = 0;

    incSec();
    
  } // incCenti()


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


  void calcHour() {

    const uint16_t fullSlots = (12 * 60 * 60);
    const uint16_t pixelSlots = fullSlots / SMALL_PIX;
    const uint16_t pixelToByte = (pixelSlots / 256) + 1;

    uint16_t actualSlot =
      (showHour * 60 * 60) +
      (showMin * 60) +
      (showSec)
    ;

    hourFadeOutPos = actualSlot / pixelSlots;
    hourFadeInPos = hourFadeOutPos + 1;
    if (hourFadeInPos >= SMALL_PIX) hourFadeInPos = 0;

    hourFadeInValue = (actualSlot % pixelSlots) / pixelToByte;
    hourFadeOutValue = 255 - hourFadeInValue;
    if (hourFadeInValue < DARK_SMALL) hourFadeInValue = DARK_SMALL;
    if (hourFadeOutValue < DARK_SMALL) hourFadeOutValue = DARK_SMALL;

  } // calcHour()


  void calcMin() {

    const uint16_t fullSlots = (60 * 60 * 10);
    const uint16_t pixelSlots = fullSlots / LARGE_PIX;
    const uint16_t pixelToByte = (pixelSlots / 256) + 1;

    uint16_t actualSlot =
      (showMin * 60 * 10) +
      (showSec * 10) +
      (showDeci)
    ;

    minFadeOutPos = actualSlot / pixelSlots;
    minFadeInPos = minFadeOutPos + 1;
    if (minFadeInPos >= LARGE_PIX) minFadeInPos = 0;

    minFadeInValue = (actualSlot % pixelSlots) / pixelToByte;
    minFadeOutValue = 255 - minFadeInValue;
    if (minFadeInValue < DARK_LARGE) minFadeInValue = DARK_LARGE;
    if (minFadeOutValue < DARK_LARGE) minFadeOutValue = DARK_LARGE;

  } // calcMin()


  void calcSec() {

    const uint16_t fullSlots = (60 * 100);
    const uint16_t pixelSlots = fullSlots / LARGE_PIX;
    const uint16_t pixelToByte = (pixelSlots / 256) + 1;

    uint16_t actualSlot =
      (showSec * 100) +
      (showCenti)
    ;

    secFadeOutPos = actualSlot / pixelSlots;
    secFadeInPos = secFadeOutPos + 1;
    if (secFadeInPos >= LARGE_PIX) secFadeInPos = 0;

    secFadeInValue = (actualSlot % pixelSlots) / pixelToByte;
    secFadeOutValue = 255 - secFadeInValue;
    if (secFadeInValue < DARK_LARGE) secFadeInValue = DARK_LARGE;
    if (secFadeOutValue < DARK_LARGE) secFadeOutValue = DARK_LARGE;

  } // calcSec()


  void redrawSmall() {

    for (int n = 0; n < SMALL_PIX; n++) {

      if (n == hourFadeOutPos) {
        strip.setPixelColor(
          small[n]
          ,0
          ,hourFadeOutValue / 2
          ,hourFadeOutValue
        );
        continue;
      }

      if (n == hourFadeInPos) {
        strip.setPixelColor(
          small[n]
          ,0
          ,hourFadeInValue / 2
          ,hourFadeInValue
        );
        continue;        
      }

      strip.setPixelColor(
        small[n]
        ,0
        ,0
        ,DARK_SMALL
      );

    } // for small

   
  } // redrawSmall() 


  void redrawLarge() {

    for (int n = 0; n < LARGE_PIX; n++) {

      if (n == secFadeOutPos) {

        if (n == minFadeOutPos) {
          mixPix(n,minFadeOutValue,secFadeOutValue);
          continue;
        }

        if (n == minFadeInPos) {
          mixPix(n,minFadeInValue,secFadeOutValue);
          continue;
        }

        mixPix(n,DARK_LARGE,secFadeOutValue);
        continue;
      }

      if (n == secFadeInPos) {

        if (n == minFadeOutPos) {
          mixPix(n,minFadeOutValue,secFadeInValue);
          continue;
        }

        if (n == minFadeInPos) {
          mixPix(n,minFadeInValue,secFadeInValue);
          continue;
        }

        mixPix(n,DARK_LARGE,secFadeInValue);
        continue;
      }
      
      if (n == minFadeOutPos) {
        strip.setPixelColor(
          large[n]
          ,minFadeOutValue
          ,minFadeOutValue
          ,0
        );
        continue;
      }

      if (n == minFadeInPos) {
        strip.setPixelColor(
          large[n]
          ,minFadeInValue
          ,minFadeInValue
          ,0
        );
        continue;        
      }

      strip.setPixelColor(
        large[n]
        ,DARK_LARGE
        ,DARK_LARGE
        ,0
      );

    } // for large

  } // redrawLarge()


  void mixPix(uint8_t pos,uint8_t min,uint8_t sec) {

    uint8_t red;
    uint8_t green;

    if (min == DARK_LARGE) {
      red = ( sec > min ? sec : min );
      green = min;
    } else {
      green = sec / 2;
      red = sec;
    }

    strip.setPixelColor(
      large[pos]
      ,red
      ,green
      ,0
    );

  } // mixPix()
 
  
