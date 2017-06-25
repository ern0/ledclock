# if ( defined(__unix__) || defined(__APPLE__) )
# define SDL_DISPLAY (0)
# define TIMER2 ( 1000 / 100 )   
# include "posixino/posixino.cpp"
# else
#include <Adafruit_NeoPixel.h>
# endif

# define FRONT_LEFT 1
# define REAR_LEFT 3
# define FRONT_RIGHT 4
# define REAR_RIGHT 2

# define PIN 13
# define LARGE_PIX 24
# define SMALL_PIX 12
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LARGE_PIX + SMALL_PIX,PIN,NEO_GRB + NEO_KHZ800);

# ifdef SDL_DISPLAY
static char large[LARGE_PIX] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23 };
static char small[SMALL_PIX] = { 24,25,26,27,28,29,30,31,32,33,34,35 };
# else 
static char large[LARGE_PIX] = { 23,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22 };
static char small[SMALL_PIX] = { 24,25,26,27,28,29,30,31,32,33,34,35 };
# endif

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

#define BRITE_PAST 0x55
#define BRITE_FUTURE 0x33
#define BRITE_OVERDRIVE 0x33

#define CENT_HOUR_TOTAL (60 * 60 * 100)
uint8_t hourFadeOutPos;
uint8_t hourFadeInPos;
uint16_t hourFadeOutValue;
uint16_t hourFadeInValue;
uint32_t centInHourValue;
uint32_t centOutHourValue;


void setupTimerInterrupt();
void tick();
void buttonHandler();
void incClock();
void calcSmall();
void calcLarge();
void redrawClock();


# ifdef SDL_DISPLAY
void setupEmu();
# endif


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

    setupTimerInterrupt();
    
  } // setup()



  # ifdef SDL_DISPLAY
  void setupEmu() {

    strip.emuSetGridScreenAnchor("ne");
    strip.emuSetGridScreenPercent(30);
    strip.emuSetGridCells(36,36);
    
    int n = 0;
    
    strip.emuSetPixelPos(n++,16,0);
    strip.emuSetPixelPos(n++,20,1);
    strip.emuSetPixelPos(n++,24,2);
    strip.emuSetPixelPos(n++,28,4);
    strip.emuSetPixelPos(n++,30,8);
    strip.emuSetPixelPos(n++,31,12);
    strip.emuSetPixelPos(n++,32,16);
    strip.emuSetPixelPos(n++,31,20);
    strip.emuSetPixelPos(n++,30,24);
    strip.emuSetPixelPos(n++,28,28);
    strip.emuSetPixelPos(n++,24,30);
    strip.emuSetPixelPos(n++,20,31);
    strip.emuSetPixelPos(n++,16,32);
    strip.emuSetPixelPos(n++,12,31);
    strip.emuSetPixelPos(n++,8,30);
    strip.emuSetPixelPos(n++,4,28);
    strip.emuSetPixelPos(n++,2,24);
    strip.emuSetPixelPos(n++,1,20);
    strip.emuSetPixelPos(n++,0,16);
    strip.emuSetPixelPos(n++,1,12);
    strip.emuSetPixelPos(n++,2,8);
    strip.emuSetPixelPos(n++,4,4);
    strip.emuSetPixelPos(n++,8,2);
    strip.emuSetPixelPos(n++,12,1);
    
    strip.emuSetPixelPos(n++,16,6);
    strip.emuSetPixelPos(n++,21,7);
    strip.emuSetPixelPos(n++,25,11);
    strip.emuSetPixelPos(n++,26,16);
    strip.emuSetPixelPos(n++,25,21);
    strip.emuSetPixelPos(n++,21,25);
    strip.emuSetPixelPos(n++,16,26);
    strip.emuSetPixelPos(n++,11,25);
    strip.emuSetPixelPos(n++,7,21);
    strip.emuSetPixelPos(n++,6,16);
    strip.emuSetPixelPos(n++,7,11);
    strip.emuSetPixelPos(n++,11,7);

    for (int n = 0; n < strip.numPixels(); n++ ) {
      strip.emuSetPixelCellSize(n,4,4);
      strip.emuSetPixelPixGap(n,5,5);
    }

  } // setupEmu()
  # endif
  

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
    for (int n = 0; n < 40; n++) ///
    incClock();

    frame += 1;
    updateRequest = true;
  
  } // tick()


  void incClock() {

    clockCent++;
    ///if (clockCent < 100) return;
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


    if (false)
    printf(
      "H: %d->%d C: %d:%d:%d Fade: %d:%d\n"
      ,(int)hourFadeOutPos
      ,(int)hourFadeInPos
      ,centInHourValue
      ,centOutHourValue
      ,CENT_HOUR_TOTAL
      ,(int)hourFadeOutValue
      ,(int)hourFadeInValue
    );

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
  