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

static char clockHour = HOUR;
static char clockMin = MIN;
static char clockSec = SEC;
static char clockCenti = 0;

void tick();
void setupTimerInterrupt();
void redrawClock();
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

    for (int n = 0; n < 36; n++) strip.setPixelColor(n,0,0,0);
    strip.show();

		setupTimerInterrupt();
		
	} // setup()


	# ifdef SDL_DISPLAY
	void setupEmu() {

		strip.emuSetGridScreenAnchor("se");
		strip.emuSetGridScreenPercent(40);
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
	
		clockCenti++;
		if (clockCenti > 99) {
			clockCenti -= 100;
			clockSec++;
			if (clockSec > 59) {
				clockSec = 0;
				clockMin++;
				if (clockMin > 59) {
					clockMin = 0;
					clockHour++;
					if (clockHour > 23) {
						clockHour = 0;
					} // if hour
				} // if min
			} // if sec
			
			redrawClock();
			
		} // if centi
	
	} // tick()


	void redrawClock() {
	
		int pos1;
		int lit1;
		int pos2;
		int lit2;

		printf("%02d:%02d:%02d \n",(int)clockHour,(int)clockMin,(int)clockSec);
	
		char hour12 = ( clockHour < 12 ? clockHour : clockHour - 12 );
		calc(12,hour12,&pos1,&lit1,&pos2,&lit2);

		for (int n = 0; n < 12; n++) {
			if (pos1 == n) {
				strip.setPixelColor(small[n],lit1,lit1,lit1);
			} else if (pos2 == n) {
				strip.setPixelColor(small[n],lit2,lit2,lit2);
			} else {
				strip.setPixelColor(small[n],0,0,0);
			}
		}

		calc(24,clockMin,&pos1,&lit1,&pos2,&lit2);

		for (int n = 0; n < 24; n++) {
			if (pos1 == n) {
				strip.setPixelColor(large[n],lit1,lit1,lit1);
			} else if (pos2 == n) {
				strip.setPixelColor(large[n],lit2,lit2,lit2);
			} else {
				strip.setPixelColor(large[n],0,0,0);
			}
		}

		calc(24,clockSec,&pos1,&lit1,&pos2,&lit2);

		for (int n = 0; n < 24; n++) {
			if (pos1 == n) {
				strip.setPixelColor(large[n],0,0,lit1);
			} else if (pos2 == n) {
				strip.setPixelColor(large[n],0,0,lit2);
			} else {
				strip.setPixelColor(large[n],0,0,0);
			}
		}
		
		strip.show();
		
	} // redrawClock()
	
	
	void calc(int length,int value,int* pos1,int* lit1,int* pos2,int* lit2) {

		// TODO: this.

		return;
	} // calc()


	void loop() {
  
	
	} // loop

