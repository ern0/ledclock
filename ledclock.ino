# if ( defined(__unix__) || defined(__APPLE__) )
# define SDL_DISPLAY (0)
# define TIMER2 ( 1000 / 100 )   
# include "posixino/posixino.cpp"
# else
#include <Adafruit_NeoPixel.h>
# endif


static char irqdiv = 0;

# define PIN 13
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24 + 12,PIN,NEO_GRB + NEO_KHZ800);

static char large[24] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23 };
static char small[12] = { 24,25,26,27,28,29,30,31,32,33,34,35 };

# ifdef SDL_DISPLAY
static unsigned char clockHour = HOUR;
static unsigned char clockMin = MIN;
static unsigned char clockSec = SEC;
# else
static unsigned char clockHour = 19;
static unsigned char clockMin = 28;
static unsigned char clockSec = 20;
# endif

static unsigned long int stamp = 0;

static unsigned long int pos1 = 0;
static unsigned long int lit1 = 0;
static unsigned long int pos2 = 0;
static unsigned long int lit2 = 0;

void tick();
void setupTimerInterrupt();
void setStamp();
void redrawClock();
void calc(unsigned long int unitSlot,unsigned long int scaleSlot,unsigned long int mod,unsigned long int div);

# ifdef SDL_DISPLAY
void setupEmu();
# endif


	void setup() {

		# ifdef SDL_DISPLAY
			setupEmu();
		# endif

		strip.begin();
		strip.setBrightness(40);
		strip.show(); // Initialize all pixels to 'off'
    for (int n = 0; n < 36; n++) strip.setPixelColor(n,255,0,255);
    strip.show();

		setStamp();
		setupTimerInterrupt();
		
	} // setup()


	# ifdef SDL_DISPLAY
	void setupEmu() {

		strip.emuSetGridScreenAnchor("ne");
		strip.emuSetGridScreenPercent(20);
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
	
		stamp++;
		if (stamp > (60*60*12-1)) stamp = 0; 

		if (stamp % 5 == 0) redrawClock();
	
	} // tick()


	void loop() {
  
	
	} // loop
	

	void setStamp() {
		
		if (clockHour > 11) clockHour -= 12;
		stamp = (clockHour * 12) + (clockMin * 60) + (clockSec * 100);
		
	} // setStamp()
	
	
	void redrawClock() {
	
		for (int n = 0; n < 36; n++) strip.setPixelColor(n,0,0,0);

		calc(12,12,4320000,360000);

		for (int n = 0; n < 12; n++) {
			if (pos1 == n) strip.setPixelColor(small[n],0,lit1 / 2,lit1);
			if (pos2 == n) strip.setPixelColor(small[n],0,lit2 / 2,lit2);
		}

		calc(24,60,6000,100);

		for (int n = 0; n < 24; n++) {
			if (pos1 == n) strip.setPixelColor(large[n],lit1,0,0);
			if (pos2 == n) strip.setPixelColor(large[n],lit2,0,0);
		}

		calc(24,60,360000,6000);

		for (int n = 0; n < 24; n++) {
			if (pos1 == n) strip.setPixelColor(large[n],lit1,lit1,0);
			if (pos2 == n) strip.setPixelColor(large[n],lit2,lit2,0);
		}
		
		strip.show();
		
	} // redrawClock() 
	
	
	void calc(unsigned long int unitSlot,unsigned long int scaleSlot,unsigned long int mod,unsigned long int div) {

		unsigned long int partStamp = stamp % mod;
		
		unsigned long int res = (255 * unitSlot * partStamp) / (scaleSlot * div);
		pos1 = res / 255;
		lit2 = res % 255;
		 
		pos2 = 1 + pos1;
		if (pos2 == scaleSlot) pos2 = 0;
		lit1 = 255 - lit2;		
		
		//printf(" val=%d/%d len=%d pos=%d:%d lit=%d:%d \n",value,limit,length,pos1,pos2,lit1,lit2);
		return;
	} // calc()




