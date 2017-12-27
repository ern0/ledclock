# include <stdint.h>
# include "ledclock.hpp"
# ifndef SDL_DISPLAY
#include <Adafruit_NeoPixel.h>
# endif

uint8_t hourFadeOutPos;
uint8_t hourFadeInPos;
uint16_t hourFadeOutValue;
uint16_t hourFadeInValue;
uint32_t centInHourValue;
uint32_t centOutHourValue;

static uint8_t irqdiv = 0;

# ifdef SDL_DISPLAY
# include "stamp.inc"
# else
static uint8_t clockHour = 20;
static uint8_t clockMin = 42;
static uint8_t clockSec = 20;
# endif
static uint8_t clockCent = 0;

static uint32_t frame = 0;
static bool updateRequest = false;
static uint32_t lastValue = 0;

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


    char b[30];
    centInHourValue = 257 * 257;
    sprintf(b,"%d \n",centInHourValue);
    Serial.print(b);

    setupTimerInterrupt();
    
  } // setup()


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
    
    tick();  // 100 Hz
    
  } // ISR()
  

  void tick() { // 100 Hz

    buttonHandler();
    for (int n = 0; n < 900; n++) ///
    incClock();

    frame += 1;
    updateRequest = true;
  
  } // tick()


  void incClock() {

    clockCent++;
    if (clockCent < 100) return;
    clockCent = 0;

    clockSec++;
    if (clockSec < 60) return;
    clockSec = 0;
    
    clockMin++;
    if (clockMin < 60) return;
    clockMin = 0;

    clockHour++;
    if (clockHour < 12) return;
    clockHour = 0;

  } // incClock()


  void buttonHandler() {
 
    if (frame % 20 == 0) {

      if (analogRead(FRONT_LEFT) > 888) {
        if (clockHour == 11) {
          clockHour = 0;
        } else {
          clockHour++;
        }
      } // if inc hour

      if (analogRead(REAR_LEFT) > 888) {
        if (clockHour == 0) {
          clockHour = 11;
        } else {
          --clockHour;
        }
      } // if dec hour

    } // if hour


    if (frame % 8 == 0) {

      if (analogRead(FRONT_RIGHT) > 888) {
        if (clockMin == 59) {
          clockMin = 0;
        } else {
          clockMin++;
        }
      } // if inc min

      if (analogRead(REAR_RIGHT) > 888) {
        if (clockMin == 0) {
          clockMin = 59;
        } else {
          --clockMin;
        }
      } // if dec min

    } // if min

  } // buttonHandler()


  void loop() {

    if (!updateRequest) return;
    
    calcSmall();
    calcLarge();
    redrawClock();
    
    updateRequest = false;
  
  } // loop
  

  void calcSmall() {

    // calculate hour positions (small ring)
    
    uint8_t clockHour12 = clockHour;
    if (clockHour12 > 11) clockHour -= 12;
    hourFadeOutPos = SMALL_PIX * clockHour12 / 12;
    
    hourFadeInPos = hourFadeOutPos + 1;
    if (hourFadeInPos >= SMALL_PIX) hourFadeInPos -= SMALL_PIX;

    // calculate hour crossfade
    
    centInHourValue = (((clockMin * 60) + clockSec) * 100) + clockCent;
    centOutHourValue = CENT_HOUR_TOTAL - centInHourValue;

    if (hourFadeInPos == 0) {
      hourFadeInValue = centInHourValue / (CENT_HOUR_TOTAL / (256 - BRITE_PAST + BRITE_OVERDRIVE));
      hourFadeInValue += BRITE_PAST;
    } else {
      hourFadeInValue = centInHourValue / (CENT_HOUR_TOTAL / (256 - BRITE_FUTURE + BRITE_OVERDRIVE));
      hourFadeInValue += BRITE_FUTURE;
   }
    if (hourFadeInValue > 255) hourFadeInValue = 255;

    hourFadeOutValue = centOutHourValue / (CENT_HOUR_TOTAL / (256 - BRITE_PAST + BRITE_OVERDRIVE));
    hourFadeOutValue += BRITE_PAST;
    if (hourFadeOutValue > 255) hourFadeOutValue = 255;

  } // calcSmall()


  void calcLarge() {

  } // calcLarge()


  void redrawClock() {

    // if (deciOfHour == lastValue) return;
    // lastValue = deciOfHour;


    # if 1
      static char buffer[120];
      sprintf(buffer,
        "H: %d->%d C: %d:%d:%d Fade: %d:%d\n"
        ,(int)hourFadeOutPos
        ,(int)hourFadeInPos
        ,centInHourValue
        ,centOutHourValue
        ,CENT_HOUR_TOTAL
        ,(int)hourFadeOutValue
        ,(int)hourFadeInValue
      );
      # if ( defined(__unix__) || defined(__APPLE__) )
        printf("%s",buffer);
      # else
        Serial.write(buffer,strlen(buffer));
      # endif
    # endif

    for (int n = 0; n < 12; n++) {

      if (n == hourFadeOutPos) {
        strip.setPixelColor(small[n],0,0,hourFadeOutValue);
        continue;
      }

      if (n == hourFadeInPos) {
        strip.setPixelColor(small[n],0,0,hourFadeInValue);
        continue;        
      }

      if (n < hourFadeOutPos) {
        strip.setPixelColor(small[n],0,0,BRITE_PAST);
      } else {
        strip.setPixelColor(small[n],0,0,BRITE_FUTURE);
      }

    } // for small

    for (int n = 0; n < 24; n++) {
		} // for large


    strip.show();
    
  } // redrawClock() 
  
