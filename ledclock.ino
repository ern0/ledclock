# if ( defined(__unix__) || defined(__APPLE__) )
# define SDL_DISPLAY (0)
# define TIMER2 ( 1000 / 100 )   
# include "posixino/posixino.cpp"
# else
#include <Adafruit_NeoPixel.h>
# endif


static uint8_t irqdiv = 0;

# define PIN 13
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24 + 12,PIN,NEO_GRB + NEO_KHZ800);

# ifdef SDL_DISPLAY
static char large[24] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23 };
static char small[12] = { 24,25,26,27,28,29,30,31,32,33,34,35 };
# else 
static char large[24] = { 23,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22 };
static char small[12] = { 24,25,26,27,28,29,30,31,32,33,34,35 };
# endif

# ifdef SDL_DISPLAY
static uint32_t clockHour = HOUR;
static uint32_t clockMin = MIN;
static uint32_t clockSec = SEC;
# else
static uint32_t clockHour = 9;
static uint32_t clockMin = 4;
static uint32_t clockSec = 0;
# endif

static uint32_t stamp;

static uint32_t pos1;
static uint32_t lit1;
static uint32_t pos2;
static uint32_t lit2;

static bool redrawRequest = false;

void tick();
void setupTimerInterrupt();
void setStamp();
void redrawClock();
void calc(uint32_t scale,uint32_t value,uint32_t reduce);

# ifdef SDL_DISPLAY
void setupEmu();
# endif


  void setup() {

    # ifdef SDL_DISPLAY
      setupEmu();
    # endif
    
    strip.begin();
    strip.setBrightness(40);
    strip.show();

    setStamp();
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
  
    stamp += 1;
    if (stamp > (24*60*60*100-1)) stamp = 0; 

    redrawRequest = true;
  
  } // tick()


  void loop() {

    if (!redrawRequest) return;
    redrawClock();
    redrawRequest = false;
  
  } // loop
  
  
  void pri(uint32_t num) {

    Serial.print((uint16_t)(num >> 0x10));
    Serial.print(":");
    Serial.println((uint16_t)(num & 0xFFFF));
    
  } // pri()


  void setStamp() {

    uint32_t h;
    uint32_t m;
    uint32_t s;

    h = clockHour * (60L * 60L * 100L);
    m = clockMin * (60L * 100L);
    s = clockSec * 100L;
    
    stamp = h + m + s; 

  } // setStamp()
  
  
  void redrawClock() {
  
    for (int n = 0; n < 36; n++) strip.setPixelColor(n,0,0,0);

    calc(12,(uint32_t)(12L*60L*60L*100L),1000L);

    for (int n = 0; n < 12; n++) {
      if (pos1 == n) strip.setPixelColor(small[n],0,lit1 / 2,lit1);
      if (pos2 == n) strip.setPixelColor(small[n],0,lit2 / 2,lit2);
    }

    calc(24,(uint32_t)(60L*100L),1L);

    for (int n = 0; n < 24; n++) {
      if (pos1 == n) strip.setPixelColor(large[n],lit1,0,0);
      if (pos2 == n) strip.setPixelColor(large[n],lit2,0,0);
    }
    
    uint32_t p1 = pos1;
    uint32_t l1 = lit1;
    uint32_t p2 = pos2;
    uint32_t l2 = lit2;

    calc(24,(uint32_t)(60L*60L*100L),1L);

    for (int n = 0; n < 24; n++) {
      if (pos1 == n) {
				if (p1 == n || p2 == n) {
					strip.setPixelColor(large[n],lit1,lit1 / 2,0);
				} else {
					strip.setPixelColor(large[n],lit1,lit1,0);
				}
			}
      if (pos2 == n) {
				if (p1 == n || p2 == n) {
					strip.setPixelColor(large[n],lit2,lit2 / 2,0);
				} else{
					strip.setPixelColor(large[n],lit2,lit2,0);
				}
			}
    }
    
    strip.show();
    
  } // redrawClock() 
  
  
  void calc(uint32_t scale,uint32_t modulo,uint32_t r) {

		// TODO: some optimization, too many div and mod 
		
    uint32_t value = stamp % modulo;
    pos1 = (scale * value) / modulo;
		uint32_t a = (value / r) * scale * 255;
		lit2 = (a / (modulo / r)) % 255;
		
    pos2 = 1 + pos1;
    if (pos2 == scale) pos2 = 0;
    lit1 = 255 - lit2;    
    
    return;
  } // calc()


