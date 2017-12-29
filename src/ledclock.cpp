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

static int8_t hourFadeOutPos;
static int8_t hourFadeInPos;
static uint8_t hourFadeOutValue;
static uint8_t hourFadeInValue;


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
    
    calcHour();
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
    for (int n = 0; n < 900; n++) ///
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


  void calcHour() {

    const uint16_t nofPixels = 12;
    const uint16_t fullSlots = (12 * 60 * 60);
    const uint16_t pixelSlots = fullSlots / nofPixels;
    const uint16_t pixelToByte = pixelSlots / 256;

    uint16_t actualSlot =
      (clockHour * 60 * 60) +
      (clockMin * 60) +
      (clockSec)
    ;

    hourFadeOutPos = actualSlot / pixelSlots;
    hourFadeInPos = hourFadeOutPos + 1;
    if (hourFadeInPos >= nofPixels) hourFadeInPos = 0;

    hourFadeInValue = ( actualSlot % pixelSlots ) / pixelToByte;    
    if (hourFadeInValue < BRITE_LOW) hourFadeInValue = BRITE_LOW;

    hourFadeOutValue = 256 - hourFadeInValue;
    if (hourFadeOutValue < BRITE_LOW) hourFadeOutValue = BRITE_LOW;

    printf(
      "%02d:%02d:%02d "
      "full=%d actual=%d %d->%d %d:%d \n"
      ,(int)clockHour
      ,(int)clockMin
      ,(int)clockSec
      ,(int)fullSlots
      ,(int)actualSlot
      ,hourFadeOutPos
      ,hourFadeInPos
      ,hourFadeOutValue
      ,hourFadeInValue
    );

  } // calcHour()


  void redrawClock() {

    # if 0
      static char buffer[120];
      sprintf(buffer,
        "%02d:%02d:%02d "
        "H: %d->%d $%02X:$%02X \n"
        ,(int)clockHour
        ,(int)clockMin
        ,(int)clockSec
        ,(int)hourFadeOutPos
        ,(int)hourFadeInPos
        ,(int)hourFadeOutValue
        ,(int)hourFadeInValue
      );
      # ifdef SDL_DISPLAY
        printf("%s",buffer);
      # else
        Serial.write(buffer,strlen(buffer));
      # endif
    # endif

    redrawSmall();
    redrawLarge();

    strip.show();

  } // redrawClock()


  void redrawSmall() {

    for (int n = 0; n < 12; n++) {

      if (n == hourFadeOutPos) {
        strip.setPixelColor(small[n],0,0,hourFadeOutValue);
        continue;
      }

      if (n == hourFadeInPos) {
        strip.setPixelColor(small[n],0,0,hourFadeInValue);
        continue;        
      }

      strip.setPixelColor(small[n],0,0,BRITE_LOW);

    } // for small

   
  } // redrawSmall() 


  void redrawLarge() {

    for (int n = 0; n < 24; n++) {
      
      /// TODO

    } // for large
 
  } // redrawLarge()
  
